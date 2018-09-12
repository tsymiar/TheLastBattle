#ifndef UC_CONDITION_H
#define UC_CONDITION_H

#if !(defined(WIN32) || defined(WIN64))
#  include <pthread.h>
#  include <errno.h>
#  include <sys/time.h>
#endif
#include "uctype.h"

namespace  UCODE
{

#define CONDITION_WIN32_CONFORMANCE_TO_POSIX

    class CSDMutex;

    class CSDCondition
    {
    public:
        CSDCondition();
        ~CSDCondition();

        BOOL  Wait (CSDMutex &mutex, UINT32 dwMs = SDINFINITE);
        BOOL  Wait (CSDMutex *mutex, UINT32 dwMs = SDINFINITE);
        VOID  Signal();
        VOID  Broadcast();

    private:
        CSDCondition (const CSDCondition&);
        CSDCondition& operator = (const CSDCondition&);

    private:
#if (defined(WIN32) || defined(WIN64))
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
        VOID EnterWait ();
        VOID* m_gate;
        VOID* m_queue;
        VOID* m_mutex;
        UINT32 m_gone;  // # threads that timed out and never made it to m_queue
        ULONG m_blocked; // # threads blocked on the condition
        UINT32 m_waiting; // # threads no longer waiting for the condition but
        // still waiting to be removed from m_queue
#  else
        HANDLE mId;
#  endif

#else
        mutable  pthread_cond_t mId;
#endif
    };

    /** @} */
}

#endif


