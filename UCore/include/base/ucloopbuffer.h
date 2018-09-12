#ifndef UCLOOPBUFFER_H
#define UCLOOPBUFFER_H

/**
* @file sdloopbuffer.h
* @brief 循环缓冲区类
**/

#include "uctype.h"

namespace UCODE
{
    /**
    * @defgroup grouploopbuffer 循环缓冲区
    * @ingroup  UCODE
    * @{
    */

    /**
    * @brief 循环缓冲区，支持单线程读写
    */
    class CSDLoopBuffer
    {
    public:
        CSDLoopBuffer(VOID);
        ~CSDLoopBuffer(VOID);

        /**
        * @brief
        * 初始化循环缓冲区
        * @param nSize : 初始化虚幻缓冲区的大小，实际大小为nSize+1
        * @return 成功返回TRUE，失败返回FALSE
        */
        BOOL  Init(INT32 nSize);

        /**
        * @brief
        * 将需要存储的Buffer拷贝到循环缓冲区的结尾
        * @param pData : [输入参数]指向需要插入循环缓冲区的Buffer起始位置
        * @param nLen : 指向需要插入的Buffer的长度
        * @return 如果循环缓冲区拥有的大小大于等于nLen，返回TRUE，否则返回FALSE
        * @remark 此函数不是线程安全的
        */
        BOOL  PushBack(const CHAR *pData, INT32 nLen);

        /**
        * @brief
        * 从循环缓冲区的起始位置取nLen长度的Buffer，拷贝放入Buffer中
        * @param pBuf : [输入输出参数]获取数据的Buffer的起始指针
        * @param nLen : 需要读出的Buffer长度
        * @return 如果有足够所需读出的数据，返回TRUE，否则返回FALSE
        * @remark 此函数不是线程安全的
        */
        BOOL  PopFront(CHAR *pBuf, INT32 nLen);

        /**
        * @brief
        * 丢弃nLen长度的数据
        * @param nLen : 需要丢弃的长度
        * @return VOID
        * @remark 此函数不是线程安全的
        */
        VOID  DiscardFront(INT32 nLen);

    private:
        CHAR    *m_pBuffer;
        CHAR    *m_pHead;
        CHAR    *m_pTail;
        INT32	m_nSize;
    };

    /** @} */
}

#endif

