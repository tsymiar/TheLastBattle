#ifndef IOCPCTRL_H
#define IOCPCTRL_H

#include "ucsingleton.h"
#include "cpsock.h"
#include <set>
#include "thread_buffer_pool.h"
using namespace UCODE;

/**
* @brief IOCP控制类,此类是一个单例的类,内部将创建2 * CPU个工作线程(Worker Item).
* 多线程循环查询IOCP内部的网络事件,并分派处理.这里多个工作线程仅仅是将所有的网络事件放入循环
*
*/
class CIocpCtrl
{
	CIocpCtrl();
	~CIocpCtrl();

	DECLARE_SINGLETON(CIocpCtrl)

public:
	static unsigned WINAPI ThreadFunc(LPVOID pParam);

	/**
	* @brief 初始化IOCP控制类
	* @return 是否初始化成功，true为成功，false为失败
	*/
	bool Init();

	/**
	* @brief 清理IOCP控制类
	*
	*/
	void Uninit();

	/**
	* @brief 当IOCP执行时，调用此函数
	*
	*
	*/
	void OnExecute();

	/**
	* @brief 将某个套接字接口绑定在此IOCP上
	* @param hSock : 绑定的套接字
	* @param pstData : 和该套接字相关的数据
	* @return 是否绑定成功
	*/
	bool AssociateWithIocp(SOCKET hSock, SPerHandleData* pstData);

	//inline bool PostIocpCompletionStatus(DWORD dwNumberOfBytesTransferred,
	//	ULONG_PTR dwCompletionKey,
	//	LPOVERLAPPED lpOverlapped)
	//{
	//	return PostQueuedCompletionStatus(m_hCompletionPort,
	//		dwNumberOfBytesTransferred, dwCompletionKey, lpOverlapped); 
	//}

	/**
	* @brief 返回ThreadLocal的缓存
	* @return 返回的ThreadLocal的缓存
	* @remark 这种使用方式，使得返回的ThreadLocal的缓存所在的线程为此类创建的线程
	*/
	ThreadBufferPool * GetThreadBufferPool()
	{
		return & m_threadBufPool; 
	}

protected:
	HANDLE		m_hCompletionPort;//IO完成端口的句柄
	INT32		m_nNumberOfWorkers;//当前IO完成端口关联的工作线程数量
	HANDLE*		m_WorkerArray;

	ThreadBufferPool  m_threadBufPool; 

	static int m_acceptCount; 
};

#endif

