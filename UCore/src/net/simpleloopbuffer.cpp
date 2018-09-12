#include "simpleloopbuffer.h"
#include <stdio.h>
#include <memory.h>
#include "ucnetconfig.h"
using namespace UCODE;

CSimpleLoopBuffer::CSimpleLoopBuffer() : 
m_pBuffer(NULL),
m_pNextRead(NULL),
m_pNextWrite(NULL),
m_pEnd(NULL)
{
}

CSimpleLoopBuffer::~CSimpleLoopBuffer()
{
	if(NULL != m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
}

bool CSimpleLoopBuffer::Init(INT32 nSize)
{
	m_pBuffer = new char[nSize + 1];
	if(NULL == m_pBuffer)
	{
		return false;
	}
	m_pNextRead = m_pBuffer;
	m_pNextWrite = m_pBuffer;
	m_pEnd = m_pBuffer + nSize + 1;
	return true;
}

INT32 CSimpleLoopBuffer::PushBack(const CHAR *pData, INT32 nLen)
{
    CSDLock oLock(m_oMutex);

    CHAR* poRead = m_pNextRead;
	if(m_pNextWrite >= poRead)
	{
        //  1、尾部指针减去写起始位置小于 nLen 
        //  2、读的起始位置减去内存首地址 小于nLen
        //  内存空间不够，不能将数据写入缓存
        /*                          == 内存模型 ==
		           (empty)             m_pNextRead         m_pNextWrite       (empty)
		|----------------------------------|--------------------|---------------------|
		*/
        INT32 nRight = m_pEnd - m_pNextWrite;
        INT32 nLeft = poRead - m_pBuffer;
		if (nLeft + nRight <= nLen)
		{
			return -1;
		}
	}
    else
	{
        /*                          == 内存模型 ==
		                               m_pNextWrite (empty) m_pNextRead
		|----------------------------------|--------------------|---------------------|
		*/
		if(poRead - m_pNextWrite <= nLen)
		{
			return -2;
		}
	}
//     else  //  m_pNextWrite == m_pNextRead
//     {
//         if (m_pNextWrite > m_pBuffer)
//         {
//             m_oMutex.Unlock();
//             return -3;
//         }
//     }
    if (m_pEnd - m_pNextWrite > nLen)
    {
        memcpy(m_pNextWrite, pData, nLen);
        m_pNextWrite += nLen;
    }
    else if (m_pEnd - m_pNextWrite == nLen)
    {
        memcpy(m_pNextWrite, pData, nLen);
        m_pNextWrite = m_pBuffer; 
    }
    else
    {
        INT32 nStartLen = m_pEnd - m_pNextWrite;
        memcpy(m_pNextWrite, pData, nStartLen);
        memcpy(m_pBuffer , pData + nStartLen , nLen - nStartLen);
        m_pNextWrite = m_pBuffer + nLen - nStartLen;
    }
	return nLen;
}

INT32 CSimpleLoopBuffer::PopFront(CHAR * &pBuf, INT32 nLen,CHAR* szData)
{
    CSDLock oLock(m_oMutex);

    CHAR* poNextWrite = m_pNextWrite;
	if(poNextWrite == m_pNextRead)
	{
        CRITICAL(_SDT("[%s:%d]:PopFront failed! ErrCode:-1"),MSG_MARK);
		return -1;
	}
	if(poNextWrite > m_pNextRead)
	{
        /*                          == 内存模型 ==
		           (empty)             m_pNextRead         m_pNextWrite       (empty)
		|----------------------------------|--------------------|---------------------|
		*/
		if(poNextWrite - m_pNextRead < nLen)
		{
            CRITICAL(_SDT("[%s:%d]:PopFront failed! ErrCode:-2"),MSG_MARK);
			return -2;
		}
	}
    else
	{
        /*                          == 内存模型 ==
                                      m_pNextWrite (empty) m_pNextRead
		|----------------------------------|--------------------|---------------------|
		*/
        INT32 nRight = m_pEnd - m_pNextRead;
        INT32 nLeft = poNextWrite - m_pBuffer;
		if (nLeft + nRight < nLen)
		{
            CRITICAL(_SDT("[%s:%d]:PopFront failed! ErrCode:-3"),MSG_MARK);
			return -3;
		}
	}
    if ( m_pEnd - m_pNextRead > nLen)
    {
        memcpy(szData, m_pNextRead, nLen);
        m_pNextRead += nLen;
    }
    else if (m_pEnd - m_pNextRead == nLen)
    {
        memcpy(szData, m_pNextRead, nLen);
        m_pNextRead = m_pBuffer;
    }
    else
    {
        INT32 nStartLen = m_pEnd - m_pNextRead;
        memcpy(szData, m_pNextRead, nStartLen);
        memcpy(szData+nStartLen , m_pBuffer, nLen - nStartLen);
        m_pNextRead = m_pBuffer + nLen - nStartLen;
    }
    return nLen;
}


