#ifndef __THREAD_BUFFER_POOL_H__
#define __THREAD_BUFFER_POOL_H__

#include <Windows.h>
#include "simpleloopbuffer.h"
#include <iostream>
#include "ucnetwin.h"

/**
* @brief 实现一个线程安全的，在不同线程代表不同的Buffer池（使用ThreadLocal的方式）
*
*
*/
class ThreadBufferPool
{
public:
	ThreadBufferPool()
	{
		m_tlsIndex = TlsAlloc();
	}
 
	/**
	* @brief 创建ThreadLocal的缓存
	*
	*
	*/
	BOOL CreateThreadBuffer()
	{
		LPVOID pBuf = (LPVOID)LocalAlloc(LPTR, sizeof(CSimpleLoopBuffer));
		if (!TlsSetValue(m_tlsIndex, pBuf))
        {
            LocalFree((HLOCAL) pBuf);
            return FALSE;
        }
        CSimpleLoopBuffer * pLoopBuf = new (pBuf)CSimpleLoopBuffer();
        pLoopBuf->Init( g_nLoopSize );
        return TRUE;
	}

	/**
	* @brief 释放ThreadLocal的缓存
	*
	*
	*/
	void ReleaseThreadBuffer()
	{
		if (CSimpleLoopBuffer * pBuf = (CSimpleLoopBuffer*)TlsGetValue(m_tlsIndex))
        {
            pBuf->~CSimpleLoopBuffer();
			LocalFree((HLOCAL) pBuf);
		}
	}

	/**
	* @brief 向ThreadLocal的缓存插入数据
	* @param pData : 插入的数据指针
	* @param nLen : 数据的长度
	* @return ThreadLocal的缓存，需要注意的是，返回的缓存的线程安全性需要由用户保证。
	* 用户需要明确返回的缓存只在当前的线程中使用
	*/
	CSimpleLoopBuffer * PushData(const char * pData, int nLen)
	{
		if (CSimpleLoopBuffer* pLoopBuf = (CSimpleLoopBuffer*)TlsGetValue(m_tlsIndex))
        {
            INT32 nResult = pLoopBuf->PushBack(pData, nLen);
            if (nResult != nLen)
            {
                CRITICAL(_SDT("[%s:%d]:Loopbuffer is busy!ThreadID:%d,Len:%d,Error:%d"),
                    MSG_MARK,
                    GetCurrentThreadId(),
                    nLen,
                    nResult);
                do
                {
                    Sleep(1);
                } while (pLoopBuf->PushBack(pData, nLen) != nLen);
            }
            return pLoopBuf;
		}
        return 0;
	}
private:

	DWORD  m_tlsIndex;
};


#endif // 

