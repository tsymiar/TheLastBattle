#include "ucnetconfig.h"
#include "ucconnection.h"
#include "cpsock.h"
#include "ucnetopt.h"
#include "ucnetwin.h"
#include "cpsockmgr.h"
#include "connectionmgr.h"
#include "ucstring.h"
#include "conndatamgr.h"
#include "ucconfig.h"


CUCConnection::CUCConnection()
{
	Reset();
}

CUCConnection::~CUCConnection()
{

}

void CUCConnection::Reset()
{
	m_dwID			= 0;
	m_dwParentID	= 0;
	m_bAccept		= false;
	m_nConnStat		= CONN_NONE;
	m_dwLocalIP		= 0;
	m_wLocalPort	= 0;
	m_dwRemoteIP	= 0;
	m_wRemotePort	= 0;
	//m_szLocalIP[0]	= '\0';
	//m_szRemoteIP[0]	= '\0';
	m_pConnData = NULL;
	m_poSession		= NULL;
	m_byRunIndex	= INVALID_RUN_INDEX;
}

bool CUCConnection::IsConnected(void)
{
	return (CONN_ASSOCIATE == m_nConnStat);
}

void CUCConnection::SetRemoteIP(UINT32 dwIP)
{
	m_dwRemoteIP = dwIP;
	//StrSafeCopy(m_szRemoteIP, inet_ntoa( (in_addr&)m_dwRemoteIP));
}

void CUCConnection::SetLocalIP(UINT32 dwIP)
{
	m_dwLocalIP = dwIP;
	//StrSafeCopy(m_szLocalIP, inet_ntoa( (in_addr&)m_dwLocalIP));
}

void CUCConnection::Send(const char* pBuf, UINT32 dwLen)
{
	if(m_nConnStat != CONN_ASSOCIATE)
	{
		return;
	}

	if(NULL == pBuf || 0 == dwLen)
	{
		return;
	}

	m_pConnData->sock.Send(pBuf, dwLen);
	return;
}

void CUCConnection::SetOpt(UINT32 dwType, void* pOpt)
{
	if(m_nConnStat != CONN_ASSOCIATE)
	{
		return;
	}

	SDASSERT(m_pConnData != NULL);

	switch(dwType)
	{
	case CONNECTION_OPT_SOCKOPT:
		{
			SConnectionOptSockopt* pstOpt = (SConnectionOptSockopt*)pOpt;
			if(NULL == pstOpt->pOptVal || pstOpt->nOptLen > 8)
			{
				return;
			}

			setsockopt(m_pConnData->sock.GetSock(), pstOpt->nLevel, pstOpt->nOptName, pstOpt->pOptVal, pstOpt->nOptLen);
		}
		break;
    case SEND_STRATEGY:
        {
            SSendStrategy* pstSSendStrategy = (SSendStrategy*)pOpt;
            if (pstSSendStrategy)
            {
                m_pConnData->sock.SetSendStrategy(*pstSSendStrategy);
            }
        }
        break;

	default:
		break;
	}
}

void CUCConnection::Disconnect(void)
{
	if(IsConnected() )
	{
		m_nConnStat = CONN_CLOSING;

		SDASSERT(m_pConnData!= NULL);
		m_pConnData->sock.Close();


		DBG(_SDT("Disconnect [%s:%d]"), _SDTA2T(GetRemoteIPStr()), m_wRemotePort);
	}
}

void CUCConnection::OnConnect()
{
	SDASSERT(CONN_NONE == m_nConnStat);

	m_nConnStat = CONN_OK;

	//如果是主动连接对方，那么要通知Connector连接事件
	if(!m_bAccept)
	{
		CUCODEConnector* poConnector = CUCODENetWin::Instance()->FindConnector(m_dwParentID);
		if(NULL == poConnector)
		{
			return;
		}
		poConnector->OnConnect();
	}
}


void CUCConnection::OnAssociate()
{
	//SDASSERT(CONN_OK == m_nConnStat);
	
	m_nConnStat = CONN_ASSOCIATE;
	m_byRunIndex = CConnDataMgr::Instance()->AddRunConection(this);
	//如果是主动连接对方，那么要通知Connector连接事件
	if(!m_bAccept)
	{
		CUCODEConnector* poConnector = CUCODENetWin::Instance()->FindConnector(m_dwParentID);
		if(NULL == poConnector)
		{
			return;
		}
		poConnector->OnConnect();
	}
	SDASSERT(m_poSession != NULL);
	m_poSession->SetConnection(this);
	m_poSession->OnEstablish();
}

void CUCConnection::OnSend(const char * pBuf, UINT32 dwLen)
{
#ifdef UCODENET_HAS_SEND_REPORT
	if (m_poSession != NULL )
	{
		m_poSession->OnSend(pBuf, dwLen); 
	}
#endif
}

void CUCConnection::OnClose()
{

	INFO(_SDT("[%s:%d]CUCConnection::OnClose  Release session state %d"), MSG_MARK, m_nConnStat);

	if (m_nConnStat == CONN_NONE) 
	{
		return ; 
	}
	

	/*
	if(CONN_ASSOCIATE == m_nConnStat)
	{
		m_poSession->SetConnection(this);
		m_poSession->OnEstablish();
	}
	*/

	m_nConnStat = CONN_NONE;

	//如果是主动连接对方，要通知Connector断线事件
	if(false == m_bAccept)
	{
		CUCODEConnector* poConnector = CUCODENetWin::Instance()->FindConnector(m_dwParentID);
		if(poConnector != NULL)
		{
			poConnector->OnClose();
		}
	}


	SDASSERT(m_poSession != NULL);
	m_poSession->OnTerminate();
	m_poSession->Release();
	m_poSession = NULL;

	CConnDataMgr::Instance()->Release(m_pConnData); 
	//CCPSockMgr::Instance()->Release(m_poSock);
	//CConnectionMgr::Instance()->Release(this);
	//CCPSockMgr::Instance()->DelayRelease(m_poSock);
}

//// cwy add 2009-03-24 add for expanding
//
// returns the free send-buffer size
//
UINT32 UCAPI CUCConnection::GetSendBufFree( void )
{
	if (NULL != m_pConnData)
	{
		return m_pConnData->sock.GetSendBufFree();
	}
	
	return 0;
}

void CUCConnection::UpdateSend()
{
	m_pConnData->sock.UpdateSend();
}

