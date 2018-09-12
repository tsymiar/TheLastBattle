#include "ucnetconfig.h"
#include "ucnetopt.h"
#include "ucconnector.h"
#include "ucnetwin.h"
#include "connectctrl.h"
#include "ucnetutils.h"

CUCODEConnector::CUCODEConnector()
{
	m_dwID				= 0;
	m_poPacketParser	= NULL;
	m_poSession			= NULL;
	m_dwRecvBufSize		= DEFAULT_RECVBUF_SIZE;
	m_dwSendBufSize		= DEFAULT_SENDBUF_SIZE;
	m_dwIP				= 0;
	m_wPort				= 0;
	m_nConnStat			= CONN_NONE;

	//// 2009-03-24 cwy add for interface expanding
	// add bind function
	m_bNeedBind		= false;
	m_pszBindIP		= NULL;
	m_wBindPort		= 0;
    m_bNodelay = FALSE;
	//// ~
}

CUCODEConnector::~CUCODEConnector()
{

}

void CUCODEConnector::SetPacketParser(ISDPacketParser* poPakcetParser)
{
	m_poPacketParser = poPakcetParser;
}

void CUCODEConnector::SetSession(ISDSession* poSession)
{
	m_poSession = poSession;
}

void CUCODEConnector::SetBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize)
{
	m_dwRecvBufSize = dwRecvBufSize;
	m_dwSendBufSize = dwSendBufSize;
}

bool CUCODEConnector::Connect(const char* pszIP, UINT16 wPort)
{
	m_dwIP	= inet_addr(pszIP);
	m_wPort	= wPort;

	return ReConnect();
}

bool CUCODEConnector::ReConnect(void)
{
	if(NULL == m_poPacketParser)
	{
		WARN(_SDT("CUCODEConnector::Connect, Not set PacketParser"));
		return false;
	}
	if(NULL == m_poSession)
	{
		WARN(_SDT("CUCODEConnector::Connect, Not set Session"));
		return false;
	}

	if(m_nConnStat != CONN_NONE)
	{
		WARN(_SDT("CUCODEConnector::ReConnect, m_nConnStat is %d"), m_nConnStat);
		return false;
	}

	if(false == CConnectCtrl::Instance()->PushConnReq(m_dwID, m_dwIP, m_wPort, 
		m_poPacketParser, m_poSession, m_dwRecvBufSize, m_dwSendBufSize, 
		m_bNeedBind, m_pszBindIP, m_wBindPort,m_bNodelay))
	{
		return false;
	}

	m_nConnStat = CONN_PROC;
	
	return true;
}

void CUCODEConnector::Release(void)
{
	CUCODENetWin::Instance()->ReleaseConnector(this);
}

void CUCODEConnector::OnConnect()
{
	m_nConnStat = CONN_OK;
}

void CUCODEConnector::OnConnectErr(INT32 nSysError)
{
	m_nConnStat = CONN_NONE;
	CRITICAL(_SDT("OnConnectErr:Address:%s:%d"), SDInetNtoa(m_dwIP).c_str(),m_wPort);
	m_poSession->OnError(NET_CONNECT_FAIL, nSysError);
}

//// 2009-03-24 cwy add for interface expanding
// add bind function
void CUCODEConnector::OnBindErr( INT32 nUCODEError, INT32 nSysError )
{
	m_nConnStat = CONN_NONE;
	m_poSession->OnError(NET_BIND_FAIL, nSysError);
}
//// ~

void CUCODEConnector::OnClose()
{
	m_nConnStat = CONN_NONE;
}

//// 2009-03-24 cwy add for interface expanding
// add bind function
void UCAPI CUCODEConnector::SetOpt( UINT32 dwType, void* pOpt )
{
	switch(dwType)
	{
	case CONNECTOR_OPT_SOCKBIND:
		{
			SConnectorOptSockBind* pstBind = (SConnectorOptSockBind*)pOpt;
			if (NULL == pstBind->pszIP || 0 == pstBind->wPort)
			{
				return;
			}
			m_bNeedBind = true;
			m_pszBindIP = pstBind->pszIP;
			m_wBindPort = pstBind->wPort;
		}
		break;
    case TCP_NO_DELAY:
        {
            STcpOptSockNoDelay* pstTcpOptSockNoDelay = (STcpOptSockNoDelay*)pOpt;
            if (pstTcpOptSockNoDelay)
            {
                m_bNodelay = pstTcpOptSockNoDelay->bNodelay;
            }
        }
        break;
	default:
		break;
	}
}

//// ~

