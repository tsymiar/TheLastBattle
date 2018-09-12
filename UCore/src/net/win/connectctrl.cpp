#include "ucnetconfig.h"
#include "connectctrl.h"
#include <process.h>
#include "eventmgr.h"
#include "cpsockmgr.h"
#include "connectionmgr.h"
#include "conndatamgr.h"
#include <ucstring.h>
#include "ucnetutils.h"
using namespace UCODE ;

IMPLEMENT_SINGLETON(CConnectCtrl)

CConnectCtrl::CConnectCtrl()
{
	m_hThread		= NULL;
	m_pAllReqArray	= NULL;
	m_bTerminate	= false;
	m_pEventsArray	= NULL;
	m_pSockArray	= NULL;
}

CConnectCtrl::~CConnectCtrl()
{

}

bool CConnectCtrl::Init()
{
	INT32 nMaxRequest = MAX_CONNECTION * 2;

	m_pAllReqArray = new SConnReq[nMaxRequest];
	if(NULL == m_pAllReqArray)
	{
		return false;
	}

	if(false == m_oFreeQueue.Init(nMaxRequest+1))
	{
		return false;
	}

	if(false == m_oReqQueue.Init(nMaxRequest+1))
	{
		return false;
	}

	INT32 i;
	for(i = 0; i < nMaxRequest; i++)
	{
		m_oFreeQueue.PushBack(&m_pAllReqArray[i]);
	}

	m_pProcReqArray = new SConnReq*[MAX_CONNECTION];
	if(NULL == m_pProcReqArray)
	{
		CRITICAL(_SDT("CConnectCtrl::Init, new SConnReq*[%d] failed"), MAX_CONNECTION);
		return false;
	}

	m_pEventsArray = new WSAEVENT[MAX_CONNECTION];
	if(NULL == m_pEventsArray)
	{
		CRITICAL(_SDT("CConnectCtrl::Init, new WSAEVENT[%d] failed"), MAX_CONNECTION);
		return false;
	}

	m_pSockArray = new SOCKET[MAX_CONNECTION];
	if(NULL == m_pSockArray)
	{
		CRITICAL(_SDT("CConnectCtrl::Init, new SOCKET[%d] failed"), MAX_CONNECTION);
		return false;
	}

	for(i = 0; i < MAX_CONNECTION; i++)
	{
		m_pProcReqArray[i]	= NULL;
		m_pEventsArray[i]	= WSA_INVALID_EVENT;
		m_pSockArray[i]		= INVALID_SOCKET;
	}
	m_dwSockCount = 0;

	m_bTerminate = false;

	UINT dwThreadID = 0;
	m_hThread = (HANDLE)_beginthreadex(	NULL,					// Security
										0,						// Stack size - use default
										ThreadFunc,     		// Thread fn entry point
										(void*)this,			// Param for thread
										0,						// Init flag
										&dwThreadID);			// Thread address

	if(NULL == m_hThread)
	{
		CRITICAL(_SDT("CConnectCtrl::Init, _beginthreadex failed"));
		return false;
	}

	return true;
}

void CConnectCtrl::Uninit()
{
	m_bTerminate = true;
	if(m_hThread != NULL)
	{
		WaitForSingleObject(m_hThread, 3000);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if(m_pAllReqArray != NULL)
	{
		delete [] m_pAllReqArray;
		m_pAllReqArray = NULL;
	}
	if(m_pProcReqArray != NULL)
	{
		delete [] m_pProcReqArray;
		m_pProcReqArray = NULL;
	}
	if(m_pEventsArray != NULL)
	{
		delete [] m_pEventsArray;
		m_pEventsArray = NULL;
	}
	if(m_pSockArray != NULL)
	{
		delete [] m_pSockArray;
		m_pSockArray = NULL;
	}
}

bool CConnectCtrl::PushConnReq(UINT32 dwConnectorID, UINT32 dwIP, UINT16 wPort, 
							   ISDPacketParser* poPacketParser, ISDSession* poSession, 
							   UINT32 dwRecvBufSize, UINT32 dwSendBufSize,
							   bool bNeedBind, const char* pszBindIP, UINT16 wBindPort,const BOOL bNodelay)
{
	SConnReq* pstConnReq = (SConnReq*)m_oFreeQueue.PopFront();
	if(NULL == pstConnReq)
	{
		SDASSERT(false);
		return false;
	}

	pstConnReq->dwConnectorID	= dwConnectorID;
	pstConnReq->dwIP			= dwIP;
	pstConnReq->wPort			= wPort;
	pstConnReq->poPacketParser	= poPacketParser;
	pstConnReq->poSession		= poSession;
	pstConnReq->dwRecvBufSize	= dwRecvBufSize;
	pstConnReq->dwSendBufSize	= dwSendBufSize;
	//// 2009-03-24 cwy add for interface expanding, add bind function
	pstConnReq->bNeedBind = bNeedBind;
	pstConnReq->pszBindIP = pszBindIP;
	pstConnReq->wBindPort = wBindPort;
    pstConnReq->bNoDelay = bNodelay;

	m_oReqQueue.PushBack(pstConnReq);

	return true;
}

unsigned CConnectCtrl::ThreadFunc(LPVOID pParam)
{
	CConnectCtrl* poCtrl = (CConnectCtrl*)pParam;
	poCtrl->OnExecute();

	return 0;
}

void CConnectCtrl::OnExecute()
{
	while(!m_bTerminate)
	{
		_ProcRequests();
		_ProcEvents();
		//CCPSockMgr::Instance()->CheckDelayRelease();
		Sleep(1);
	}
}

bool CConnectCtrl::BindAddress(SOCKET hSock, const char * pIpAddr, UINT16 port)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(pIpAddr);
	addr.sin_port = htons(port);

	bool optVal = true;
	setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, sizeof(optVal));

	if(SOCKET_ERROR == bind(hSock, (sockaddr*)&addr, sizeof(addr)))
	{
		return false;
	}
	return true;
}

void CConnectCtrl::_OnSockError(SOCKET hSock, SConnReq* pstConnReq)
{
	CRITICAL(_SDT("CConnectCtrl::_ProcRequests, socket bind failed, errno %d"), WSAGetLastError());
	CEventMgr::Instance()->PushBindEvt(pstConnReq->dwConnectorID, NET_BIND_FAIL, WSAGetLastError());
	m_oFreeQueue.PushBack(pstConnReq);
	closesocket(hSock);
}

void CConnectCtrl::_ProcRequests()
{
	while(m_dwSockCount < (UINT32)MAX_CONNECTION)
	{
		SConnReq* pstConnReq = (SConnReq*)m_oReqQueue.PopFront();
		if(NULL == pstConnReq)
		{
			break;
		}

		SOCKET hSock = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
		if(INVALID_SOCKET == hSock)
		{
			CRITICAL(_SDT("CConnectCtrl::_ProcRequests, socket failed, errno %d"), WSAGetLastError());
			CEventMgr::Instance()->PushConnErrEvt(WSAGetLastError(), pstConnReq->dwConnectorID);
			m_oFreeQueue.PushBack(pstConnReq);
			break;
		}
	
		//// 2009-04-02 cwy modify for general use
		if (pstConnReq->bNeedBind)
		{
			if ( false == BindAddress(hSock, pstConnReq->pszBindIP, pstConnReq->wBindPort) )
			{
				_OnSockError(hSock, pstConnReq);
				break;
			}
		}
        if (g_bNodelay)
        {
            const CHAR szOpt = 1;
            if (0 != ::setsockopt(hSock, IPPROTO_TCP, TCP_NODELAY, (char *)&szOpt, sizeof(char)))
            {
                WARN(_SDT("setsockopt for new socket on UpdateConetext failed, errno=%d"), ::WSAGetLastError());
            }
        }

		WSAEVENT hEvent = WSACreateEvent();
		if(WSA_INVALID_EVENT == hEvent)
		{
			_OnSockError(hSock, pstConnReq);
			break;
		}

		if(SOCKET_ERROR == WSAEventSelect(hSock, hEvent, FD_CONNECT))
		{
			_OnSockError(hSock, pstConnReq);
			WSACloseEvent(hEvent);
			break;
		}

		sockaddr_in stAddr = {0};
		stAddr.sin_family		= AF_INET;
		stAddr.sin_addr.s_addr	= pstConnReq->dwIP;
		stAddr.sin_port			= htons(pstConnReq->wPort);
		
		if( SOCKET_ERROR == connect(hSock, (sockaddr*)&stAddr, sizeof(stAddr)) )
		{
			if(WSAEWOULDBLOCK != WSAGetLastError())
			{
				_OnSockError(hSock, pstConnReq);
				WSACloseEvent(hEvent);
				break;
			}
		}

		m_pProcReqArray[m_dwSockCount]	= pstConnReq;
		m_pSockArray[m_dwSockCount]		= hSock;
		m_pEventsArray[m_dwSockCount]	= hEvent;
		++m_dwSockCount;
	}
}

void CConnectCtrl::_ProcEvents()
{
	if(0 == m_dwSockCount)
	{
		return;
	}

	WSANETWORKEVENTS	stNetworkEvents;
	WSAEVENT*			pEvents;
	UINT32				dwCount;
	UINT32				dwIndex;
	UINT32				dwStart = 0;

	do
	{
		pEvents = &m_pEventsArray[dwStart];
		if(dwStart + WSA_MAXIMUM_WAIT_EVENTS > m_dwSockCount)
		{
			dwCount	= m_dwSockCount - dwStart;
		}
		else
		{
			dwCount	= WSA_MAXIMUM_WAIT_EVENTS;
		}
		
		dwIndex = WSAWaitForMultipleEvents(dwCount, pEvents, false, 0, false);
		if(WSA_WAIT_FAILED == dwIndex || WSA_WAIT_TIMEOUT == dwIndex)
		{
			dwStart += dwCount;
			continue;
		}

		dwIndex -= WSA_WAIT_EVENT_0;
		dwIndex += dwStart;
		++dwStart;

		SDASSERT(m_pProcReqArray[dwIndex] != NULL && m_pSockArray[dwIndex] != INVALID_SOCKET && m_pEventsArray[dwIndex] != WSA_INVALID_EVENT);

		if(SOCKET_ERROR == WSAEnumNetworkEvents(m_pSockArray[dwIndex], m_pEventsArray[dwIndex], &stNetworkEvents))
		{
			if(WSAEWOULDBLOCK != WSAGetLastError())
			{
				CEventMgr::Instance()->PushConnErrEvt(WSAGetLastError(), m_pProcReqArray[dwIndex]->dwConnectorID);
				_CloseEvent(dwIndex);
			}
			continue;
		}

		if(stNetworkEvents.lNetworkEvents & FD_CONNECT)
		{
			if(stNetworkEvents.iErrorCode[FD_CONNECT_BIT] != 0)
			{
				CEventMgr::Instance()->PushConnErrEvt(stNetworkEvents.iErrorCode[FD_CONNECT_BIT], m_pProcReqArray[dwIndex]->dwConnectorID);
				_CloseEvent(dwIndex);
				continue;
			}

			//
			// 连接成功
			//
			SConnReq* pstReq = m_pProcReqArray[dwIndex];

			CConnData * pConnData = CConnDataMgr::Instance()->Alloc(pstReq->dwRecvBufSize, pstReq->dwSendBufSize);
			if (pConnData == NULL)
			{
				CRITICAL(_SDT("CConnectCtrl::_ProcEvents, create ConnData failed"));
				CEventMgr::Instance()->PushConnErrEvt(0, pstReq->dwConnectorID);
				_CloseEvent(dwIndex);
				continue;
			}
			CCPSock *poSock = &pConnData->sock;
			CUCConnection * poConnection = &pConnData->connection;

			poSock->SetSock(m_pSockArray[dwIndex]);

			m_oFreeQueue.PushBack(m_pProcReqArray[dwIndex]);
			WSACloseEvent(m_pEventsArray[dwIndex]);
			m_pProcReqArray[dwIndex]	= NULL;
			m_pSockArray[dwIndex]		= INVALID_SOCKET;
			m_pEventsArray[dwIndex]		= WSA_INVALID_EVENT;

			sockaddr_in stAddr = {0};
			INT32 nAddrLen = sizeof(stAddr);
			getsockname(poSock->GetSock(), (sockaddr*)&stAddr, &nAddrLen);

			poConnection->SetAccept(false);
			poConnection->SetParentID(pstReq->dwConnectorID);
			poConnection->SetSession(pstReq->poSession);
			poConnection->SetLocalIP(stAddr.sin_addr.s_addr);
			poConnection->SetLocalPort(SDNtohs(stAddr.sin_port));
			poConnection->SetRemoteIP(pstReq->dwIP);
			poConnection->SetRemotePort(pstReq->wPort);
			//poConnection->SetCpSock(poSock);

			//poSock->SetConnection(poConnection);
			poSock->SetPacketParser(pstReq->poPacketParser);
			poSock->SetConnect(TRUE);

			//CEventMgr::Instance()->PushEstablishEvt(pConnData, false, pstReq->dwConnectorID);

			if(false == poSock->AssociateWithIocp())
			{
				poSock->Close();
			}
			else
			{
				if(false == poSock->PostRecv())
				{
					poSock->Close();
				}
			}
		}
	}while(dwStart < m_dwSockCount);

	_CompressEvent();
}

void CConnectCtrl::_CloseEvent(UINT32 dwIndex)
{
	SDASSERT(dwIndex < (UINT32)MAX_CONNECTION);

	if(m_pProcReqArray[dwIndex] != NULL)
	{
		m_oFreeQueue.PushBack(m_pProcReqArray[dwIndex]);
		m_pProcReqArray[dwIndex] = NULL;
	}

	if(m_pSockArray[dwIndex] != INVALID_SOCKET)
	{
		closesocket(m_pSockArray[dwIndex]);
		m_pSockArray[dwIndex] = INVALID_SOCKET;
	}
	
	if(m_pEventsArray[dwIndex] != WSA_INVALID_EVENT)
	{
		WSACloseEvent(m_pEventsArray[dwIndex]);
		m_pEventsArray[dwIndex] = WSA_INVALID_EVENT;
	}
}

void CConnectCtrl::_CompressEvent()
{
	UINT32 dwTo = 0;
	for(UINT32 i = 0; i < m_dwSockCount; i++)
	{
		if(m_pProcReqArray[i] != NULL)
		{
			if(i != dwTo)
			{
				m_pProcReqArray[dwTo]	= m_pProcReqArray[i];
				m_pEventsArray[dwTo]	= m_pEventsArray[i];
				m_pSockArray[dwTo]		= m_pSockArray[i];
			}
			++dwTo;
		}
	}

	m_dwSockCount = dwTo;
}

void CConnectCtrl::_CleanUp()
{
	for(UINT32 dwIndex = 0; dwIndex < m_dwSockCount; ++dwIndex)
	{
		if(m_pSockArray[dwIndex] != INVALID_SOCKET)
		{
			closesocket(m_pSockArray[dwIndex]);
			m_pSockArray[dwIndex] = INVALID_SOCKET;
		}

		if(m_pEventsArray[dwIndex] != WSA_INVALID_EVENT)
		{
			WSACloseEvent(m_pEventsArray[dwIndex]);
			m_pEventsArray[dwIndex] = WSA_INVALID_EVENT;
		}
	}
}

