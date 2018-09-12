#include "ucnetconfig.h"
#include "ucnetwin.h"
#include "ucnetopt.h"
#include "uclistener.h"
//#include "cpsockmgr.h"
#include "eventmgr.h"
#include "iocpctrl.h"
//#include "connectionmgr.h"
#include "connectctrl.h"
#include "vercheck.h"
//#include "syssockmgr.h"

IUCNet* UCAPI UCODE::SDNetGetModule(const SUCODEVersion *pstVersion)
{
	if (FALSE == VerCheck(pstVersion, &UCNET_VERSION))
	{
		return NULL;
	}

	if(NULL == CUCODENetWin::Instance())
	{
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);

		if(false == CUCODENetWin::CreateInstance())
		{
			return NULL;
		}

		if(false == CUCODENetWin::Instance()->Init())
		{
			CRITICAL(_SDT("UCODENetGetModule, Init CUCODENetWin failed"));
			CUCODENetWin::DestroyInstance();
			return NULL;
		}
	}

    CUCODENetWin::Instance()->AddRef();

	return CUCODENetWin::Instance();
}

//////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(CUCODENetWin)

void UCAPI UCODE::SDNetSetOpt(UINT32 dwType, void* pOpt)
{
	switch(dwType)
	{
	case NETWIN_OPT_MAX_CONNECTION:
		{
			SNetWinOptMaxConnection* pstConn = (SNetWinOptMaxConnection*)pOpt;
			MAX_CONNECTION = pstConn->nMaxConnection;
		}
		break;
	case NETWIN_OPT_QUEUE_SIZE:
		{
			SNetWinOptQueueSize* pstQueue = (SNetWinOptQueueSize*)pOpt;
			if(pstQueue->nRecvBufSize != -1)
			{
				RECV_LOOP_BUF_SIZE = pstQueue->nRecvBufSize;
			}
			if(pstQueue->nEventQueueSize != -1)
			{
				MAX_NET_EVENT = pstQueue->nEventQueueSize;
			}
		}
		break;
	case NETWIN_OPT_ADVANCE_PARAM:
		{
			SNetWinOptAdvanceParam* pstParam = (SNetWinOptAdvanceParam*)pOpt;
			if(pstParam->nParam1 != -1)
			{
				DELAY_RELEASE_CPSOCK_TIME = pstParam->nParam1;
			}
		}
		break;

    case IOCP_THREAD_NUM:
        {
            SIocpOptThreadNum* poIocpOptThreadNum = (SIocpOptThreadNum*)pOpt;
            g_nThreadNum = poIocpOptThreadNum->byThreadNum;
        }
        break;
    case THREAD_LOOP_SIZE:
        {
            SThreadLoopSize* poThreadLoopSize = (SThreadLoopSize*)pOpt;
            g_nLoopSize = poThreadLoopSize->dwLoopSize;
        }
        break;
    case TCP_NO_DELAY:
        {
            STcpOptSockNoDelay* poTcpOptSockNoDelay = (STcpOptSockNoDelay*)pOpt;
            g_bNodelay = poTcpOptSockNoDelay->bNodelay;
        }
        break;
	default:
		break;
	}
}

CUCODENetWin::CUCODENetWin()
{
	m_dwRef				= 0;
	m_dwNextConnectorID = 0;
	m_pRecvBuf = new char[MAX_PKG_LEN];
}

CUCODENetWin::~CUCODENetWin()
{
	if (m_pRecvBuf)
	{
		delete []m_pRecvBuf;
	}
	
}

void CUCODENetWin::AddRef(void)
{
	m_dwRef++;
}


UINT32 CUCODENetWin::QueryRef(void)
{
	return m_dwRef;
}

void CUCODENetWin::Release(void)
{
	if(m_dwRef <= 1)
	{
		Uninit();
        DestroyInstance();
		return;
	}
	m_dwRef--;
}

SUCODEVersion CUCODENetWin::GetVersion(void)
{
	return UCNET_VERSION;
}

const char * CUCODENetWin::GetModuleName(void)
{
	return UCNET_MODULENAME;
}

ISDConnector* CUCODENetWin::CreateConnector(UINT32 dwNetIOType)
{
	if(!(dwNetIOType & NETIO_COMPLETIONPORT))
	{
		return NULL;
	}

	if((INT32)m_oMapConnector.size() >= MAX_CONNECTION)
	{
		WARN(_SDT("CUCODENetWin::CreateConnector, Max connector reached"));
		return NULL;
	}

	CUCODEConnector* poConnector = new CUCODEConnector;
	if(NULL == poConnector)
	{
		CRITICAL(_SDT("CUCODENetWin::CreateConnector, new CUCODEConnector failed"));
		return NULL;
	}

	poConnector->SetID(++m_dwNextConnectorID);
	m_oMapConnector[m_dwNextConnectorID] = poConnector;
	
	return poConnector;
}

ISDListener* CUCODENetWin::CreateListener(UINT32 dwNetIOType)
{
    if (dwNetIOType & NETIO_COMPLETIONPORT)
    {
	    CUCODEListener* poListener = new CUCODEListener;
	    if(NULL == poListener)
	    {
		    CRITICAL(_SDT("CUCODENetWin::CreateListener, new CUCODEListener failed"));
		    return NULL;
	    }

	    return poListener;
    }
	else if (dwNetIOType & NETIO_COMPLETIONPORT_GATE)
	{
		return NULL;
	}
    else
    {
        return NULL;
    }
}

void CUCODENetWin::ReleaseConnector(CUCODEConnector* poConnector)
{
	if(NULL == poConnector)
	{
		return;
	}

	m_oMapConnector.erase(poConnector->GetID());
	delete poConnector;
}

CUCODEConnector* CUCODENetWin::FindConnector(UINT32 dwID)
{
	CMapConnector::iterator it = m_oMapConnector.find(dwID);
	if(it == m_oMapConnector.end())
	{
		return NULL;
	}

	return it->second;
}

bool CUCODENetWin::Init()
{
	if(false == _CreateComponent())
	{
		return false;
	}

	if(false == _InitComponent())
	{
		return false;
	}

	return true;
}

void CUCODENetWin::Uninit()
{
	_UninitComponent();
	_DesroryComponent();
}

bool CUCODENetWin::_CreateComponent()
{
	if(false == CEventMgr::CreateInstance())
	{
		CRITICAL(_SDT("CUCODENetWin::_CreateComponent, CEventMgr::CreateInstance failed"));
		return false;
	}

	if (false == CConnDataMgr::CreateInstance())
	{
		CRITICAL(_SDT("CUCODENetWin::_CreateComponent, CConnDataMgr::CreateInstance failed"));
		return false;
	}

	if(false == CConnectCtrl::CreateInstance())
	{
		CRITICAL(_SDT("CUCODENetWin::_CreateComponent, CConnectCtrl::CreateInstance failed"));
	}
	if(false == CIocpCtrl::CreateInstance())
	{
		CRITICAL(_SDT("CUCODENetWin::_CreateComponent, CIocpCtrl::CreateInstance failed"));
		return false;
	}
	
    m_dwLastTick = GetTickCount();
	return true;
}

bool CUCODENetWin::_InitComponent()
{
	if (false == CConnDataMgr::Instance()->Init())
	{
		CRITICAL(_SDT("CUCODENetWin::_InitComponent, Init CConnDataMgr failed" ));
		return false;
	}
	if(false == CEventMgr::Instance()->Init(MAX_NET_EVENT))
	{
		CRITICAL(_SDT("CUCODENetWin::_InitComponent, Init CEventMgr %d failed"), MAX_NET_EVENT);
		return false;
	}


	if(false == CConnectCtrl::Instance()->Init())
	{
		CRITICAL(_SDT("CUCODENetWin::_InitComponent, Init CConnectCtrl failed"));
		return false;
	}

	if(false == CIocpCtrl::Instance()->Init())
	{
		CRITICAL(_SDT("CUCODENetWin::_InitComponent, Init CIocpCtrl failed"));
		return false;
	}

	return true;
}

void CUCODENetWin::_UninitComponent()
{
	CIocpCtrl::Instance()->Uninit();
	CConnectCtrl::Instance()->Uninit();
	CConnDataMgr::Instance()->Uninit(); 

	//SysSockMgr::Instance()->Uninit(); 
	CEventMgr::Instance()->Uninit();

	//if(m_pRecvBuf != NULL)
	//{
	//	delete m_pRecvBuf;
	//	m_pRecvBuf = NULL;
	//}

	_ClearConnector();
}

void CUCODENetWin::_DesroryComponent()
{
	CIocpCtrl::DestroyInstance();
	CConnectCtrl::DestroyInstance();
	CConnDataMgr::DestroyInstance(); 
	//SysSockMgr::DestroyInstance(); 
//	CConnectionMgr::DestroyInstance();
//	CCPSockMgr::DestroyInstance();
	CEventMgr::DestroyInstance();
}

void CUCODENetWin::_ClearConnector()
{
	for(CMapConnector::iterator it = m_oMapConnector.begin(); it != m_oMapConnector.end(); it++)
	{
		delete it->second;
	}

	m_oMapConnector.clear();
}

bool CUCODENetWin::Run(INT32 nCount)
{
	CConnDataMgr::Instance()->RunConection();
	do
	{
// #ifdef UCODENET_HAS_GATHER_SEND       
// #pragma message("[preconfig]sdnet collect buffer, has a internal timer")        
//         if (m_pTimerModule)        
//         {
//             m_pTimerModule->Run();
//         }        
// #endif
#ifdef UCODENET_HAS_GATHER_SEND 
        static INT32 sendCnt = 0;
        ++sendCnt;
        if (sendCnt == 10)
        {
            sendCnt = 0;
            UINT32 now = GetTickCount();
            if (now < m_dwLastTick)
            {
                /// 溢出了,发生了数据回绕 \///
                m_dwLastTick = now;
            }

            if ((now - m_dwLastTick) > 50)
            {
                m_dwLastTick = now;            
                FlushBufferedData();
            }
        }       
#endif // 
		//SNetEvent stEvent; 
		SNetEvent *pstEvent  = CEventMgr::Instance()->PopFrontNetEvt();
		if (pstEvent == NULL)
		{
			return false;
		}
		SNetEvent & stEvent = *pstEvent; 
		
		switch(stEvent.nType)
		{
		case NETEVT_RECV:
			_ProcRecvEvt(&stEvent.stUn.stRecv);
			break;
		case NETEVT_SEND:
			_ProcSendEvt(&stEvent.stUn.stSend); 
			break; 
		case NETEVT_ESTABLISH:
			_ProcEstablishEvt(&stEvent.stUn.stEstablish);
			break;
		case NETEVT_ASSOCIATE:
			_ProcAssociateEvt(&stEvent.stUn.stAssociate);
			break;
		case NETEVT_TERMINATE:
			_ProcTerminateEvt(&stEvent.stUn.stTerminate);
			break;
		case NETEVT_CONN_ERR:
			_ProcConnErrEvt(&stEvent.stUn.stConnErr);
			break;
		case NETEVT_ERROR:
			_ProcErrorEvt(&stEvent.stUn.stError);
			break;
		case NETEVT_BIND_ERR:
			_ProcBindErrEvt(&stEvent.stUn.stBindErr);
			break;
		default:
			SDASSERT(false);
			break;
		}
		CEventMgr::Instance()->ReleaseNetEvt(pstEvent); 
	}while(--nCount != 0);
	return true;
}

void CUCODENetWin::_ProcEstablishEvt(SEstablishEvt* pstEvent)
{
	CConnData* pConnData = pstEvent->pConnData;
	SDASSERT(pConnData != NULL);
	pConnData->connection.OnConnect();
}

void CUCODENetWin::_ProcAssociateEvt(SAssociateEvt* pstEvent)
{
	CConnData* pConnData = pstEvent->pConnData;
	SDASSERT(pConnData != NULL);

#ifdef UCODENET_HAS_GATHER_SEND 
	m_connSockets.push_back(&pConnData->sock);
#endif //

	if(pConnData->connId != pstEvent->dwConnectionID)
	{
		return;
	}

	pConnData->connection.OnAssociate();
}

void CUCODENetWin::_ProcConnErrEvt(SConnErrEvt* pstEvent)
{
	CUCODEConnector* poConnector = FindConnector(pstEvent->dwConnectorID);
	if(NULL == poConnector)
	{
		return;
	}

	poConnector->OnConnectErr(pstEvent->nSysErrCode);
}

//// 2009-03-24 cwy add for interface expanding, add bind function
void CUCODENetWin::_ProcBindErrEvt( SBindErrEvt* pstEvent )
{
	CUCODEConnector* poConnector = FindConnector(pstEvent->dwConnectorID);
	if(NULL == poConnector)
	{
		return;
	}
	poConnector->OnBindErr(pstEvent->nUCODEErrCode, pstEvent->nSysErrCode);
}

void CUCODENetWin::_ProcErrorEvt(SErrorEvt* pstEvent)
{
	CConnData* pConnData = pstEvent->pConnData;
	SDASSERT(pConnData != NULL);
	if(pConnData->connId != pstEvent->dwConnectionID)
	{
		return;
	}

	pConnData->connection.OnError(pstEvent->nUCODEErrCode, pstEvent->nSysErrCode);
}

void CUCODENetWin::_ProcTerminateEvt(STerminateEvt* pstEvent)
{
	CConnData* pConnData = pstEvent->pConnData;
	SDASSERT(pConnData != NULL);

#ifdef UCODENET_HAS_GATHER_SEND 
	for (ConnectedSockets::iterator itr = m_connSockets.begin();
		itr != m_connSockets.end(); ++itr)
	{
		if ( (*itr) == &pConnData->sock)
		{
			m_connSockets.erase(itr);
			break;
		}
	}
#endif 

	pConnData->connection.OnClose();
}

void CUCODENetWin::ProcRecvData(CConnData * pConnData, const char * pData, int len)
{
	SDASSERT(pConnData != NULL);
	pConnData->connection.OnRecv(pData, len);
}

void CUCODENetWin::_ProcRecvEvt(SRecvEvt* pstEvent)
{
	CHAR * pRecvBuf = NULL;
	CConnData* pConnData = pstEvent->pConnData;
	SDASSERT(pConnData != NULL);

	/*
	if(false == m_oRecvQueue.PopFront(m_pRecvBuf, pstEvent->nLen))
	{
		CRITICAL("%s, PopFront RecvData Len %d failed", __FUNCTION__, pstEvent->nLen);
		return;
	}
	*/

	INT32 nResult = pstEvent->pLoopBuf->PopFront(pRecvBuf, pstEvent->nLen,m_pRecvBuf);

	if(pConnData->connId != pstEvent->dwConnectionID)
	{
		CRITICAL(_SDT("%s, on recv connection id error!"), __FUNCTION__);
		return;
	}

	if (nResult == pstEvent->nLen)
	{
		pConnData->connection.OnRecv(m_pRecvBuf, pstEvent->nLen);
	}
	else
	{
		WARN(_SDT("[%s:%d]:Loopbuffer PopFront failed!ThreadID:%d,Len:%d,Error:%d"),
			MSG_MARK,
			GetCurrentThreadId(),
			pstEvent->nLen,
			nResult);
	}
}

void CUCODENetWin::_ProcSendEvt(SSendEvt * pstEvent)
{
	CConnData* pConnData = pstEvent->pConnData;
	SDASSERT(pConnData != NULL);
	pConnData->connection.OnSend(pstEvent->pBuf, pstEvent->dwLen);
}
 

