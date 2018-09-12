#ifndef CPSOCKMGR_H
#define CPSOCKMGR_H

#include <list>
#include "ucobjectpool.h"
#include "ucsingleton.h"
#include "cpsock.h"
#include "ucmutex.h"
#include "bufpool.h"

using namespace UCODE;

typedef std::list<CCPSock*> CCPSockList;

/**
* @brief 套接字类管理器
*/
class CCPSockMgr
{
	CCPSockMgr();
	~CCPSockMgr();

	DECLARE_SINGLETON(CCPSockMgr)

public:
	/**
	* @brief 初始化套接字类管理器
	* @param nMax : 最大套接字类数量
	* @return 初始化成功,返回true,初始化失败,返回false
	*/
	bool Init(INT32 nMax);

	/**
	* @brief 清理套接字类管理器
	* @return void
	*/
	void Uninit();

	/**
	* @brief 创建一个套接字类
	* @param dwRecvBufSize : 该套接字最大接收Buf
	* @param dwSendBufSize : 该套接字最大发送Buf
	* @return 创建的套接字类
	*/
	CCPSock * Create(UINT32 dwRecvBufSize, UINT32 dwSendBufSize);

	/**
	* @brief 回收一个套接字类
	* @param poSock : 回收的套接字类
	* @return void
	*/
	void Release(CCPSock* poSock);

	//void DelayRelease(CCPSock* poSock);
	//void CheckDelayRelease();

#if UCODENet_Has_CollectBuffer_BeforeSend
    void OnSendOut();
    std::set<CCPSock*>      m_oSendSockList;
#endif

protected:

	/**
	* @brief 内部创建一个套接字类
	* @param dwRecvBufSize : 该套接字最大接收Buf
	* @param dwSendBufSize : 该套接字最大发送Buf
	* @return 创建的套接字类
	*/
	CCPSock * _Create(UINT32 dwRecvBufSize, UINT32 dwSendBufSize);

	/**
	* @brief 内部创建一个套接字类
	* @param dwRecvBufSize : 该套接字最大接收Buf
	* @param dwSendBufSize : 该套接字最大发送Buf
	* @return 创建的套接字类
	*/
	void _Release(CCPSock* poSock);

	/**
	* @brief 查找分配dwSize大小缓存的缓存池
	* @param dwSize : 缓存池分配的大小dwSize
	* @param bCreate : 是否仅查找已经创建成功的缓存池,抑或是如果没有创建成功的缓存池,创建该缓存池
	* @return 返回此缓存池
	*/
	CBufPool* _FindBufPool(UINT32 dwSize, bool bCreate = true);

protected:
	CCPSock*				m_pSockArray;
	INT32					m_nSockCount;
	CCPSockList				m_oFreeList;
	CCPSockList				m_oDelayReleaseList;
	CMapBufPool				m_oMapBufPool;
	CSDMutex                  m_oLock;
	time_t					m_nLastCheckTime;
	SOCKET*					m_pSockHandleArray;
};

#endif

