#include "uctime.h"
#include "conndataallocator.h"
#include "eventmgr.h"
CConnDataMgr * CConnDataMgr::s_instance = NULL; 

IMPLEMENT_SINGLETON(CConnDataMgr); 

CConnData::CConnData(INT32 recvBufSize , INT32 sendBufSize   )
{

	connection.SetConnData(this);
	connection.SetID(connId);

	sock.AttachRecvBuf(recvBuf,recvBufSize); 
	sock.AttachSendBuf(sendBuf,sendBufSize); 
	sock.SetConnData(this); 
}

CConnData::~CConnData()
{
	char* pRecvBuf			= NULL;
	char* pSendBuf			= NULL;
	UINT32 dwRecvBufSize	= sock.DetachRecvBuf(&pRecvBuf);
	UINT32 dwSendBufSize	= sock.DetachSendBuf(&pSendBuf); 
}

bool CConnDataMgr::Init()
{
	m_connDataAllocator = new ConnDataAllocator(); 
	m_initState  = 1; 
	m_byRunIndex = 0;
	m_dwLastRunTime = UCODE::SDGetTickCount();
	return true; 
}

void CConnDataMgr::Uninit()
{
	m_initState = -1; 
	CConnData * pFakeData = m_connDataAllocator->allocate(1,1); 
	m_connDataAllocator->deallocate(pFakeData,1,1); 
	delete m_connDataAllocator; 
}

CConnData * CConnDataMgr::Alloc(UINT32 recvBufSize, UINT32 sendBufSize)
{
	CConnData *pConnData = m_connDataAllocator->allocate( recvBufSize,  sendBufSize); 	 
	return new (pConnData)CConnData(recvBufSize,sendBufSize); 
}

bool CConnDataMgr::Find(CCPSock * pSock)
{
	return m_connDataAllocator->find(pSock); 
}

void CConnDataMgr::Release(CConnData * pConnData)
{	 
	UINT8 byRunIndex = pConnData->connection.GetRunIndex();
	if (byRunIndex < RUN_CONNECTION_LIST_SIZE)
	{
		m_listRunConnection[byRunIndex].remove(&(pConnData->connection));
	}

	UINT32 sendBufSize = pConnData->sock.GetSendBufSize(); 
	UINT32 recvBufSize = pConnData->sock.GetRecvBufSize(); 
	pConnData->~CConnData(); 
	m_connDataAllocator->deallocate(pConnData,recvBufSize,sendBufSize); 
}

void CConnDataMgr::ReleaseUsedConnData()
{
	for (UsedConnDataList::iterator itr = m_usedConnData.begin(); itr != m_usedConnData.end(); ++itr)
	{
		(*itr)->connection.Disconnect(); 
	}
	m_usedConnData.clear(); 
}

UINT8 CConnDataMgr::AddRunConection( CUCConnection* pConnection )
{
	if (NULL != pConnection)
	{
		UINT8	byMinIndex = 0;
		UINT32	dwMinNum = m_listRunConnection[0].size();
		for (UINT8 i = 1; i < RUN_CONNECTION_LIST_SIZE; i++)
		{
			if (m_listRunConnection[i].size() < dwMinNum)
			{
				dwMinNum = m_listRunConnection[i].size();
				byMinIndex = i;
			}
		}
		if (byMinIndex < RUN_CONNECTION_LIST_SIZE)
		{
			m_listRunConnection[byMinIndex].push_back(pConnection);
		}
		return byMinIndex;
	}
	else
	{
		return INVALID_RUN_INDEX;
	}
}

void CConnDataMgr::RunConection()
{
	UINT32 dwCurTime = UCODE::SDGetTickCount();
	UINT32 dwPassTime = dwCurTime - m_dwLastRunTime;
	m_dwLastRunTime = dwCurTime;
	if (dwPassTime > RUN_CONNECTION_LIST_SIZE)
	{
		dwPassTime = RUN_CONNECTION_LIST_SIZE;
	}
	for (UINT32 i = 0; i < dwPassTime; i++)
	{
		if (m_byRunIndex < RUN_CONNECTION_LIST_SIZE)
		{
			__RunConection(m_listRunConnection[m_byRunIndex]);
		}
		else
		{
			m_byRunIndex = 0;
			__RunConection(m_listRunConnection[m_byRunIndex]);
		}
		m_byRunIndex++;
	}
}

void CConnDataMgr::__RunConection( std::list<CUCConnection*>& listRunConnection )
{
	for (std::list<CUCConnection*>::iterator pIter = listRunConnection.begin(); pIter != listRunConnection.end(); ++pIter)
	{
		(*pIter)->UpdateSend();
	}
}

/*
void CConnDataMgr::DelayRelease(CCPSock * pSock)
{
	m_delayLock.Lock(); 
	m_delaySockList.push_back(pSock); 
	m_delayLock.Unlock(); 
}
void CConnDataMgr::RemoveDelaySock(CCPSock * pSock)
{
	m_delayLock.Lock(); 

	for(std::list<CCPSock * > ::iterator it = m_delaySockList.begin(); it != m_delaySockList.end();)
	{
		if (*it == pSock)
		{
			m_delaySockList.erase(it); 
			break; 
		}
	}
	m_delayLock.Unlock(); 
}

void CConnDataMgr::CheckDelaySock()
{
	time_t nNow = time(NULL);
	if(nNow - m_nLastCheckTime < 1)
		return;

	m_nLastCheckTime = nNow;

	m_delayLock.Lock();
	for(std::list<CCPSock * > ::iterator it = m_delaySockList.begin(); it != m_delaySockList.end();)
	{
		CCPSock* poSock = *it;

		if ((poSock->GetPostRecv() > 0)&&(0 == poSock->GetPostSend()))
		{
			
			if(poSock->GetSock() != INVALID_SOCKET)
			{
				CancelIo((HANDLE)poSock->GetSock());
				closesocket(poSock->GetSock());
				CEventMgr::Instance()->PushTerminateEvt(poSock->GetConnData(), poSock->GetConnectionID());
				poSock->SetSock(INVALID_SOCKET);
			}
		}

		DBG("[%s:%d]ConnID=%d, PostRecvCnt=%d, PostSendCnt=%d",
			MSG_MARK, poSock->GetConnectionID(), poSock->GetPostRecv(), poSock->GetPostSend());

		if ( ( (0 == poSock->GetPostRecv()) &&  (0 == poSock->GetPostSend()) ) )
		{
			CEventMgr::Instance()->PushTerminateEvt(poSock->GetConnData(), poSock->GetConnectionID());
			this->m_delaySockList.erase(it); 
			continue;
		}

		++it;
	}
	m_delayLock.Unlock();


}
*/

