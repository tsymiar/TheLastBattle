#ifndef CONNECTIONMGR_H
#define CONNECTIONMGR_H

#include "ucsingleton.h"
#include "ucconnection.h"
#include "ucmutex.h"
#include <deque>
/**
* @brief 连接管理器
*
*/
class CConnectionMgr
{
	CConnectionMgr();
	~CConnectionMgr();

	DECLARE_SINGLETON(CConnectionMgr)

	typedef std::deque<CUCConnection*> CConnectionPool;
public:

	/**
	* @brief 初始化连接管理器
	* @param nMax 连接管理器中管理的最大连接数量
	* @return 是否初始化成功,true为成功,false为失败
	*/
	bool Init(INT32 nMax);

	/**
	* @brief 清理连接管理器
	* @return void
	*/
	void Uninit();

	/**
	* @brief 创建一个连接
	* @return 返回创建的连接
	*/
	CUCConnection* Create();

	/**
	* @brief 回收一个连接
	* @return void
	*/
	void Release(CUCConnection* poConnection);

protected:
	UINT32					m_dwNextID;
	INT32					m_nMaxConnection;
	CUCConnection*			m_pConnectionArray;
	CConnectionPool			m_oFreeList;

    CSDMutex                   m_mutex;
};

#endif

