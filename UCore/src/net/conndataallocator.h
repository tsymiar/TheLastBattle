#ifndef __OBJECT_ALLOCATOR_EX_H__
#define __OBJECT_ALLOCATOR_EX_H__

#include "ucmutex.h"
#include "uclock.h"
#include <deque>

#include "conndatamgr.h"

static UINT32 g_connectionId = 0; 

/**
* @brief 连接数据分配器,此分配器是线程安全的
*
*/
class ConnDataAllocator
{	
	enum 
	{
		POOL_INFLATE_SIZE= 200, 
	};
public:
	typedef size_t   size_type;
	typedef CConnData*       pointer;
	typedef CConnData&       reference;
	typedef const CConnData* const_pointer;
	typedef const CConnData& const_reference;
	typedef CConnData        value_type;

	typedef std::deque<pointer> ObjectQueue; 
	typedef ObjectQueue::iterator ObjectQueueItr; 

	ConnDataAllocator() throw() 
	{
		m_totalSize = 0; 
		m_allocSize = 0; 
		m_releaseSize = 0; 
		CSDLock<CSDMutex> lock(m_mutex); 
		inflate();
	}

	~ConnDataAllocator() throw() 
	{
		CSDLock<CSDMutex> lock(m_mutex); 
		for(ObjectQueueItr itr = m_objects.begin(); itr != m_objects.end(); ++itr)
		{
			::operator delete (*itr);
		}
		this->m_objects.clear();

		for (CMapBufPool::iterator itr = m_mapBufPool.begin(); itr != m_mapBufPool.end(); ++itr)
		{
			itr->second->Uninit();
			delete itr->second; 
		}
		m_mapBufPool.clear(); 
	}

	/**
	* @brief 获取引用的指针
	*/
	pointer address(reference __x) const { return &__x; }

	/**
	* @brief 获取常量引用的常量指针
	*/
	const_pointer address(const_reference __x) const { return &__x; }

	/**
	* @brief 分配一个连接数据
	* @param recvBufSize : 接收数据的缓存大小
	* @param sendBufSize : 发送数据的缓存大小
	* @return 分配的连接数据
	*/
	CConnData* allocate(UINT32 recvBufSize, UINT32 sendBufSize)
	{
		m_allocSize ++;
		m_mutex.Lock();
		if (m_objects.empty())
		{
			this->inflate();
		}

		CConnData * pConnData = m_objects.front();
		m_objects.pop_front();

		CConnDataMgr * pConnDataMgr = CConnDataMgr::Instance();
		pConnData->connId= g_connectionId ++;
		pConnDataMgr->AddUsedConnData(pConnData);

		CBufPool * pRecvBufPool = FindBufPool(recvBufSize);
		pConnData->recvBuf = pRecvBufPool->Create();

		CBufPool * pSendBufPool = FindBufPool(sendBufSize);
		pConnData->sendBuf = pSendBufPool->Create();

		m_mutex.Unlock();
		return pConnData;
	}

	// __p is not permitted to be a null pointer.
	/**
	* @brief 释放一个连接数据
	* @param pConnData : 需要被释放的连接数据
	* @param recvBufSize : 接收数据的缓存大小
	* @param sendBufSize : 发送数据的缓存大小
	* @return 分配的连接数据
	*/
	void deallocate(pointer pConnData, UINT32 recvBufSize, UINT32 sendBufSize)
	{ 
		m_releaseSize++;
		m_mutex.Lock();
		m_objects.push_back(pConnData);

		ReleaseBufPool(pConnData->recvBuf, recvBufSize);
		ReleaseBufPool(pConnData->sendBuf, sendBufSize);

		CConnDataMgr * pConnDataMgr = CConnDataMgr::Instance();

		
		pConnDataMgr->RemoveUsedConnData(pConnData);
		if (pConnDataMgr->m_initState == -1)
		{
			pConnDataMgr->m_initState = 0;
			// release all used data; 
			pConnDataMgr->ReleaseUsedConnData();		
		}
		m_mutex.Unlock();
	}

	/**
	* @brief 查找某个Socke是否被使用
	* @param pSock : 需要查找的Socket
	* @return 如果在使用返回true,如果没有被使用返回false
	*/
	bool find(CCPSock * pSock)
	{
		CSDLock<CSDMutex> lock(m_mutex);
		CConnDataMgr * pConnDataMgr = CConnDataMgr::Instance();
		return pConnDataMgr->FindUsedConnSock(pSock);
	}

	/**
	* @brief 获取分配dwSize大小内存的定长内存池
	* @param dwSize : 分配内存的大小
	* @param bCreate : 如果没有该内存池,是否创建
	* @return 返回分配dwSize大小内存的定长内存池,如果没有该大小的内存池,返回NULL
	*/
	CBufPool* FindBufPool(UINT32 dwSize, bool bCreate = true);

	/**
	* @brief 释放内存到内存池
	* @param pBuf : 需要释放的内存
	* @param dwSize : 该内存的大小
	*/
	void ReleaseBufPool(char * pBuf, UINT32 dwSize);

	/**
	* @brief 返回还可以分配连接数据的数量
	* @return 还可以分配连接数据的数量
	*/
	inline size_type size()
	{
		CSDLock<CSDMutex> lock(m_mutex);
		return m_objects.size();
	}

	/**
	* @brief 返回总共分配的连接数据的数量
	* @return 总共分配的连接数据的数量
	*/
	inline size_type max_size() const throw() 
	{ 
		return m_totalSize;
	}

private:
	/**
	* @brief 设置总共分配的连接数据的数量
	* @param count : 总共分配的连接数据的数量
	* @return void
	*/
	void inflate(int count = POOL_INFLATE_SIZE)
	{
		//NOTICE: locked outside
		for (int i = 0; i < count ;i++)
		{
			pointer  _p = static_cast<pointer>(::operator new (sizeof(value_type)));
			m_objects.push_back(_p);
		}
		m_totalSize += count;
	}

	/**
	* @brief 不允许拷贝构造此类
	*/
	ConnDataAllocator(const ConnDataAllocator&) throw() 
	{
	}
 

	ObjectQueue m_objects;
	CSDMutex m_mutex;
	int m_totalSize;
	int m_allocSize;
	int m_releaseSize;

	CMapBufPool		 m_mapBufPool;

};

#endif // 

