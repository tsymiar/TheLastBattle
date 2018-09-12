#include "ucnetconfig.h"
#include "cpsockmgr.h"
#include <time.h>
#include <algorithm>
#include <ucstring.h>
IMPLEMENT_SINGLETON(CCPSockMgr)

CCPSockMgr::CCPSockMgr()
{
	m_pSockArray		= NULL;
	m_nSockCount		= 0;
	m_nLastCheckTime	= 0;
	m_pSockHandleArray	= NULL;
}

CCPSockMgr::~CCPSockMgr()
{

}

bool CCPSockMgr::Init(INT32 nMax)
{
	m_pSockArray = new CCPSock[nMax];
	if(NULL == m_pSockArray)
	{
		CRITICAL(_SDT("CCPSockMgr::Init, Init new CCPSock[%d] failed"), nMax);
		return false;
	}

	m_pSockHandleArray = new SOCKET[nMax];
	if(NULL == m_pSockHandleArray)
	{
		CRITICAL(_SDT("CCPSockMgr::Init, Init new SOCKET[%d] failed"), nMax);
		delete [] m_pSockArray;
		m_pSockArray = NULL;
		return false;
	}

	for(INT32 i = 0; i < nMax; i++)
	{
		m_oFreeList.push_back(&m_pSockArray[i]);
	}

	m_nSockCount = nMax;
	return true;
}

void CCPSockMgr::Uninit()
{
	if(m_pSockArray != NULL)
	{
		for(INT32 i = 0; i < m_nSockCount; i++)
		{
			SOCKET hSock = m_pSockArray[i].GetSock();
			if(hSock != INVALID_SOCKET)
			{
				CancelIo((HANDLE)hSock);
				closesocket(hSock);
			}
		}

		delete [] m_pSockArray;
		m_pSockArray = NULL;
	}

	if(m_pSockHandleArray != NULL)
	{
		delete [] m_pSockHandleArray;
		m_pSockHandleArray = NULL;
	}
	

	for(CMapBufPool::iterator it = m_oMapBufPool.begin(); it != m_oMapBufPool.end(); it++)
	{
		it->second->Uninit();
		delete it->second;
	}
	m_oMapBufPool.clear();
}

CCPSock * CCPSockMgr::Create(UINT32 dwRecvBufSize, UINT32 dwSendBufSize)
{
	CCPSock* poSock;
	m_oLock.Lock();
	poSock = _Create(dwRecvBufSize, dwSendBufSize);
	m_oLock.Unlock();

	return poSock;
}

CCPSock * CCPSockMgr::_Create(UINT32 dwRecvBufSize, UINT32 dwSendBufSize)
{
	if(m_oFreeList.size() == 0)
	{
		CRITICAL(_SDT("CCPSockMgr::_Create, create CCPSock failed"));
		return NULL;
	}

	CCPSock* poSock = m_oFreeList.front();
	m_oFreeList.pop_front();

	// 为Sock分配发送和接收缓冲
	char* pRecvBuf = NULL;
	char* pSendBuf = NULL;
	CBufPool* poSendBufPool = NULL;
	CBufPool* poRecvBufPool = _FindBufPool(dwRecvBufSize);

	if(NULL == poRecvBufPool)
	{
		CRITICAL(_SDT("CCPSockMgr::_Create, FindBufPool Recv size %u failed"), dwRecvBufSize);
		goto fail_ret;
	}

	pRecvBuf = poRecvBufPool->Create();
	if(NULL == pRecvBuf)
	{
		CRITICAL(_SDT("CCPSockMgr::_Create, Create RecvBuf failed, size %u"), dwRecvBufSize);
		goto fail_ret;
	}

	poSendBufPool = _FindBufPool(dwSendBufSize);
	if(NULL == poSendBufPool)
	{
		CRITICAL(_SDT("CCPSockMgr::_Create, FindBufPool Send size %u failed"), dwSendBufSize);
		goto fail_ret;
	}
	pSendBuf = poSendBufPool->Create();
	if(NULL == pSendBuf)
	{
		CRITICAL(_SDT("CCPSockMgr::_Create, Create SendBuf failed, size %u"), dwSendBufSize);
		goto fail_ret;
	}

	poSock->AttachRecvBuf(pRecvBuf, dwRecvBufSize);
	poSock->AttachSendBuf(pSendBuf, dwSendBufSize);

#if UCODENet_Has_CollectBuffer_BeforeSend
    #pragma message("[preconfig]add created socket into send socketlist")
    m_oSendSockList.insert(poSock);

#endif

	return poSock;
	
fail_ret:
	if(pRecvBuf != NULL)
	{
		poRecvBufPool->Release(pRecvBuf);
	}

	if(pSendBuf != NULL)
	{
		poSendBufPool->Release(pSendBuf);
	}

	m_oFreeList.push_back(poSock);
	return NULL;
}

void CCPSockMgr::Release(CCPSock* poSock)
{
	if(NULL == poSock)
	{
		return;
	}

	m_oLock.Lock();
	_Release(poSock);
	m_oLock.Unlock();
}

void CCPSockMgr::_Release(CCPSock* poSock)
{
	char* pRecvBuf			= NULL;
	char* pSendBuf			= NULL;
	UINT32 dwRecvBufSize	= poSock->DetachRecvBuf(&pRecvBuf);
	UINT32 dwSendBufSize	= poSock->DetachSendBuf(&pSendBuf);

    DBG(_SDT("[%s:%d]Release cpsock, connID=%d, recvBuf=%d, sendBuf=%d"),
         MSG_MARK, poSock->GetConnectionID(), dwRecvBufSize, dwSendBufSize);

#if UCODENet_Has_CollectBuffer_BeforeSend
    #pragma message("[preconfig]erase socket from the send socketlist")
    m_oSendSockList.erase(poSock);
#endif
	//释放RecvBuf
	if(pRecvBuf != NULL)
	{
		CBufPool* poRecvBufPool = _FindBufPool(dwRecvBufSize, false);
		if(poRecvBufPool != NULL)
		{
            DBG(_SDT("[%s:%d]release recvbuf, size=%u"), MSG_MARK, dwRecvBufSize);
			poRecvBufPool->Release(pRecvBuf);
		}
		else
		{
			SDASSERT(false);
			delete pRecvBuf;
		}
	}
	else
	{
		SDASSERT(pRecvBuf != NULL);
	}

	//释放SendBuf
	if(pSendBuf != NULL)
	{
		CBufPool* poSendBufPool = _FindBufPool(dwSendBufSize, false);
		if(poSendBufPool != NULL)
		{
            DBG(_SDT("[%s:%d]release sendbuf, size=%u"), MSG_MARK, dwSendBufSize);
			poSendBufPool->Release(pSendBuf);
		}
		else
		{
			SDASSERT(false);
			delete pSendBuf;
		}
	}
	
    poSock->Reset();
	m_oFreeList.push_back(poSock);
}

CBufPool* CCPSockMgr::_FindBufPool(UINT32 dwSize, bool bCreate)
{
	CMapBufPool::iterator it = m_oMapBufPool.find(dwSize);
	if(it != m_oMapBufPool.end())
	{
		return it->second;
	}

	if(false == bCreate)
	{
		return NULL;
	}

	CBufPool* poBufPool = new CBufPool;
	if(NULL == poBufPool)
	{
		CRITICAL(_SDT("CCPSockMgr::_FindBufPool, new CBufPool failed"));
		return NULL;
	}
	poBufPool->Init(dwSize);
	m_oMapBufPool[dwSize] = poBufPool;

	return poBufPool;
}

/*
void CCPSockMgr::DelayRelease(CCPSock* poSock)
{
	if(NULL == poSock)
		return;

	m_oLock.Lock();
	m_oDelayReleaseList.push_back(poSock);
	m_oLock.Unlock();
}

void CCPSockMgr::CheckDelayRelease()
{
	time_t nNow = time(NULL);
	if(nNow - m_nLastCheckTime < 1)
		return;

	m_nLastCheckTime = nNow;
	INT32 nCount = 0;

	m_oLock.Lock();
	for(CCPSockList::iterator it = m_oDelayReleaseList.begin(); it != m_oDelayReleaseList.end();)
	{
		CCPSock* poSock = *it;

        if ((poSock->GetPostRecv() > 0)&&(0 == poSock->GetPostSend()))
        {
            if(poSock->GetSock() != INVALID_SOCKET)
            {
                m_pSockHandleArray[nCount++] = poSock->GetSock();
                poSock->SetSock(INVALID_SOCKET);
            }
        }
        
        if (poSock->IsNeedForceClean())
        {
            DBG("[%s:%d]do force clean, connID=%d", MSG_MARK, poSock->GetConnectionID());
        }
        
        DBG("[%s:%d]ConnID=%d, PostRecvCnt=%d, PostSendCnt=%d",
            MSG_MARK, poSock->GetConnectionID(), poSock->GetPostRecv(), poSock->GetPostSend());

		//if(poSock->IncDelayReleaseStamp() > DELAY_RELEASE_CPSOCK_TIME)
        if ( poSock->IsNeedForceClean() ||
            ( (0 == poSock->GetPostRecv()) &&  (0 == poSock->GetPostSend()) ) )
		{
			if(poSock->GetSock() != INVALID_SOCKET)
			{
				m_pSockHandleArray[nCount++] = poSock->GetSock();
			}
			it = m_oDelayReleaseList.erase(it);
			_Release(poSock);
			continue;
		}

		++it;
	}
	m_oLock.Unlock();

	for(INT32 i = 0; i < nCount; i++)
	{
		CancelIo((HANDLE)m_pSockHandleArray[i]);
		closesocket(m_pSockHandleArray[i]);
	}
}

*/
#if UCODENet_Has_CollectBuffer_BeforeSend
#pragma message("[preconfig]sdnet has collected buffer before send")

struct SendOutHelper
{
    void operator()(CCPSock* pSock)
    {
        if (NULL != pSock && pSock->IsConnect())
        {
            pSock->OnSendOut();            
        }
    }
};

void CCPSockMgr::OnSendOut()
{
    m_oLock.Lock();
    std::for_each(m_oSendSockList.begin(), m_oSendSockList.end(), SendOutHelper());
    m_oLock.Unlock();
}
#endif

