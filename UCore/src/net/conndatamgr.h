#ifndef _CONNDATAMGR_H_
#define _CONNDATAMGR_H_

#include "ucsingleton.h"
#if (defined(WIN32) || defined(WIN64))
#include "cpsock.h"
#else 
#include "epollsock.h"
#endif
 
#include "ucobjectpool.h"
#include <vector>
#include <set>
#include <list>
#include <time.h>
#include "bufpool.h"

#include "ucconnection.h"
class ConnDataAllocator;
class ConnDataAllocator ;

#if !(defined(WIN32) || defined(WIN64))
	typedef CEpollSock CCPSock; 
#endif 
 
#define  INVALID_RUN_INDEX				0xFF
#define  RUN_CONNECTION_LIST_SIZE		5			

struct CConnData 
{
	CConnData(INT32 recvBufSize =DEFAULT_RECVBUF_SIZE , INT32 sendBufSize  =DEFAULT_SENDBUF_SIZE ); 
	
	~CConnData(); 

	// don't modify below values in your application 
	CUCConnection connection; 
	CCPSock sock; 
	char * sendBuf; 
	char * recvBuf; 	 
 
	UINT32 connId; 
};



class CConnDataMgr
{
	CConnDataMgr()
	{
		m_connDataAllocator = NULL; 
		 
		m_initState = 0 ; 
	}
	~CConnDataMgr()
	{
	};
	typedef std::vector<CConnData*> UsedConnDataList; 

public:
	DECLARE_SINGLETON(CConnDataMgr); 

	bool Init(); 

	void Uninit(); 

	CConnData * Alloc(UINT32 recvBufSize, UINT32 sendBufSize); 

	bool Find(CCPSock * pSock); 

    void Release(CConnData * pConnData); 
	/*
	void DelayRelease(CCPSock * pSock); 
	void RemoveDelaySock(CCPSock * pSock); 
	void CheckDelaySock(); 
	*/

	UINT8	AddRunConection(CUCConnection* pConnection);

	void	RunConection();
protected:
	void	__RunConection(std::list<CUCConnection*>& listRunConnection);
private:

	inline void AddUsedConnData(CConnData * pConnData)
	{
		m_usedConnData.push_back(pConnData); 
	}
	void RemoveUsedConnData(CConnData * pConnData)
	{
		for (UsedConnDataList::iterator itr = m_usedConnData.begin(); itr != m_usedConnData.end(); ++itr)
		{
			if (pConnData == (*itr))
			{
				m_usedConnData.erase(itr); 
				return ; 
			}
		}
	}

	bool FindUsedConnSock(CCPSock* pSock)
	{
		for (UsedConnDataList::iterator itr = m_usedConnData.begin(); itr != m_usedConnData.end(); ++itr)
		{
			if (pSock == &((*itr)->sock))
			{
				return true; 
			}
		}
		return false; 
	}
	


	
	friend class ConnDataAllocator; 
	void ReleaseUsedConnData(); 	
 
	ConnDataAllocator * m_connDataAllocator; 	

	static CConnDataMgr * s_instance; 

	UsedConnDataList m_usedConnData; 

	std::list<CCPSock * >  m_delaySockList; 

	CSDMutex   m_delayLock; 

	time_t					m_nLastCheckTime;

	int m_initState; 
	UINT8	m_byRunIndex;
	UINT32	m_dwLastRunTime;
	std::list<CUCConnection*>  m_listRunConnection[RUN_CONNECTION_LIST_SIZE]; 
};


#endif //

