#ifndef UC_DATAQUEUE_H
#define UC_DATAQUEUE_H

#include "uctype.h"

namespace UCODE
{
    class CSDDataQueue
    {
    public:
        CSDDataQueue(VOID);
        ~CSDDataQueue(VOID);

        BOOL  Init(INT32 nSize);
        BOOL  PushBack(VOID *ptr);
        VOID* PopFront();

    protected:
        VOID            **m_pArray;
        INT32			m_nHead;
        INT32			m_nTail;
        INT32			m_nSize;
    };
}

#endif

