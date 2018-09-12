#ifndef BUFPOOL_H
#define BUFPOOL_H

#include <list>
#include <map>
#include <set>

typedef std::list<char*> CListBuf;
typedef std::set<char*> CSetBuf;

/**
* @brief 定长内存池，此内不是线程安全的
*
*/
class CBufPool
{
public:
	CBufPool(void);
	virtual ~CBufPool(void);

	/**
	* @brief 初始化定长内存池
	* @param dwSize : 每片内存的大小
	* @return void
	*/
	void Init(UINT32 dwSize);

	/**
	* @brief 释放此定长内存池的内存，回到未初始化的状态
	* @return void
	*
	*/
	void Uninit();

	/**
	* @brief 获取内存池的每片内存的大小
	* @return 返回每片内存的大小
	* @return void
	*/
	UINT32 GetSize();

	/**
	* @brief 获取一块内存
	* @return 返回一块内存
	* @return void
	*/
	char* Create();

	/**
	* @brief 回收一块内存
	* @param pBuf : 回收的内存
	* @return void
	*/
	void Release(char* pBuf);

protected:
	UINT32		m_dwSize;
	CListBuf	m_oListBuf;
	CSetBuf		m_oSetBuf;
#ifdef _DEBUG
	CSetBuf		m_oSetFree;
#endif
};

/**
* @brief 各种长度的定长内存池
*/
typedef std::map<UINT32, CBufPool*> CMapBufPool;

#endif

