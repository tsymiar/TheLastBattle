#include <cassert>
#include <climits>
#include "uccondition.h"
#include "ucmutex.h"
#include "uctime.h"

namespace UCODE
{

    CSDCondition::CSDCondition()
    {
        //std::cerr << this << " Condition::Condition" << std::endl;

#if (defined(WIN32) || defined(WIN64))
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
        m_blocked = 0;
        m_gone = 0;
        m_waiting = 0;
        m_gate = reinterpret_cast<void*>(CreateSemaphore(0, 1, 1, 0));
        m_queue = reinterpret_cast<void*>(CreateSemaphore(0, 0, LONG_MAX, 0));
        m_mutex = reinterpret_cast<void*>(CreateMutex(0, 0, 0));

        if (!m_gate || !m_queue || !m_mutex)
        {
            INT32 res = 0;
            if (m_gate)
            {
                res = CloseHandle(reinterpret_cast<HANDLE>(m_gate));
                assert(res);
            }
            if (m_queue)
            {
                res = CloseHandle(reinterpret_cast<HANDLE>(m_queue));
                assert(res);
            }
            if (m_mutex)
            {
                res = CloseHandle(reinterpret_cast<HANDLE>(m_mutex));
                assert(res);
            }

            assert(0);
        }
#  else
        mId =  CreateEvent(
                   NULL, //LPSECURITY_ATTRIBUTES lpEventAttributes,
                   // pointer to security attributes
                   FALSE, // BOOL bManualReset,  // flag for manual-reset event
                   FALSE, //BOOL bInitialState, // flag for initial state
                   NULL //LPCTSTR lpName      // pointer to event-object name
               );
        assert(mId);
#  endif
#else
        INT32  rc =  pthread_cond_init(&mId,0);
        assert( rc == 0 );
#endif
    }


    CSDCondition::~CSDCondition ()
    {
#if (defined(WIN32) || defined(WIN64))
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
        INT32 res = 0;
        res = CloseHandle(reinterpret_cast<HANDLE>(m_gate));
        assert(res);
        res = CloseHandle(reinterpret_cast<HANDLE>(m_queue));
        assert(res);
        res = CloseHandle(reinterpret_cast<HANDLE>(m_mutex));
        assert(res);
#  else
        BOOL ok = CloseHandle(mId);
        assert( ok );
#  endif
#else
        if (pthread_cond_destroy(&mId) == EBUSY)
        {
            //WarningLog (<< "Condition variable is busy");
            assert(0);
        }
#endif
    }


#if defined(WIN32) && defined(CONDITION_WIN32_CONFORMANCE_TO_POSIX)
    void CSDCondition::EnterWait ()
    {
        INT32 res = 0;
        res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
        assert(res == WAIT_OBJECT_0);
        ++m_blocked;
        res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
        assert(res);
    }
#endif



    BOOL  CSDCondition::Wait(CSDMutex& mutex, UINT32 ms)
    {
        if (ms == SDINFINITE)
        {
            //std::cerr << "Condition::Wait " << mutex << std::endl;
#if (defined(WIN32) || defined(WIN64))
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
            EnterWait();

            // Release the mutex
            mutex.Unlock();

            // do Wait
            {
                INT32 res = 0;
                res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue), INFINITE);
                assert(res == WAIT_OBJECT_0);

                unsigned was_waiting=0;
                unsigned was_gone=0;

                res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
                assert(res == WAIT_OBJECT_0);
                was_waiting = m_waiting;
                was_gone = m_gone;
                if (was_waiting != 0)
                {
                    if (--m_waiting == 0)
                    {
                        if (m_blocked != 0)
                        {
                            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0); // open m_gate
                            assert(res);
                            was_waiting = 0;
                        }
                        else if (m_gone != 0)
                            m_gone = 0;
                    }
                }
                else if (++m_gone == (ULONG_MAX / 2))
                {
                    // timeout occured, normalize the m_gone count
                    // this may occur if many calls to Wait with a timeout are made and
                    // no call to notify_* is made
                    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
                    assert(res == WAIT_OBJECT_0);
                    m_blocked -= m_gone;
                    res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
                    assert(res);
                    m_gone = 0;
                }
                res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
                assert(res);

                if (was_waiting == 1)
                {
                    for (/**/; was_gone; --was_gone)
                    {
                        // better now than spurious later
                        res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue),
                                                  INFINITE);
                        assert(res == WAIT_OBJECT_0);
                    }
                    res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
                    assert(res);
                }
            }

            // Reacquire the mutex
            mutex.Lock();

#   else
            // FixMe: Race condition between time we get mId and when we
            // re-acquire the mutex.
            mutex.Unlock();
            WaitForSingleObject(mId,INFINITE);
            mutex.lock();
#   endif
#else
            INT32 ret = pthread_cond_wait(&mId, &mutex.m_mutexId);
            assert( ret == 0 );
#endif
            return TRUE;
        }

#if (defined(WIN32) || defined(WIN64))
#   ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
        EnterWait();

        // Release the mutex
        mutex.Unlock();

        //  do timed Wait
        BOOL ret = FALSE;
        UINT32 res = 0;

        res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue),ms);
        assert(res != WAIT_FAILED && res != WAIT_ABANDONED);
        ret = (res == WAIT_OBJECT_0) ? TRUE : FALSE;

        unsigned was_waiting=0;
        unsigned was_gone=0;

        res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
        assert(res == WAIT_OBJECT_0);
        was_waiting = m_waiting;
        was_gone = m_gone;
        if (was_waiting != 0)
        {
            if (!ret) // timeout
            {
                if (m_blocked != 0)
                    --m_blocked;
                else
                    ++m_gone; // count spurious wakeups
            }
            if (--m_waiting == 0)
            {
                if (m_blocked != 0)
                {
                    res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0); // open m_gate
                    assert(res);
                    was_waiting = 0;
                }
                else if (m_gone != 0)
                    m_gone = 0;
            }
        }
        else if (++m_gone == (ULONG_MAX / 2))
        {
            // timeout occured, normalize the m_gone count
            // this may occur if many calls to Wait with a timeout are made and
            // no call to notify_* is made
            res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
            assert(res == WAIT_OBJECT_0);
            m_blocked -= m_gone;
            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
            assert(res);
            m_gone = 0;
        }
        res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
        assert(res);

        if (was_waiting == 1)
        {
            for (/**/; was_gone; --was_gone)
            {
                // better now than spurious later
                res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue), INFINITE);
                assert(res ==  WAIT_OBJECT_0);
            }
            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
            assert(res);
        }

        // Reacquire the mutex
        mutex.Lock();

        return ret;

#   else
        // FixMe: Race condition between time we get mId and when we
        // re-acquire the mutex.
        //
        // SLG: A Note about the Win32 Implementation of Conditions
        //
        // I have investigated a fix for this.  A solution to this problem is
        // non-trivial.  Please read http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
        // for a full explanation.  This is an implementation of the SetEvent solution
        // discussed in that article.  This solution has the following issues:
        // 1.  Unfairness - ie.  First thread to call Wait may not be first thread
        //     to be released from condition.
        // 2.  Incorrectness due to a race condition when a Broadcast occurs
        // (see the link for more details on these issues)
        //
        // There is a solution that corrects these two problem, but also introduces 2 more.
        // This solution (also discussed in the link) requires the use of a primitive only
        // available in WinNT and above.  It also requires that the Mutex passed in be
        // implemented using windows Mutexes instead of CriticalSections - they are less
        // efficient.  Thus the problems with this SignalObjectAndWait solution are:
        // 1.  Not portable to all versions of windows - ie.  will not work with Win98/Me
        // 2.  Less efficient than tthe SetEvent solution
        //
        // I have choosen to stick with the SetEvent Solution for the following reasons:
        // 1.  Speed is important.
        // 2.  The Unfairness issue is not really a big problem since the stack currently
        //     does not call a Wait function from two different threads.  (assuming the
        //     hosting application always calls process() from the same thread).  The only
        //     time multi-threading comes into the picture is when the transports queue
        //     messages from the wire onto the stateMacFifo - but they are retrieved off the
        //     Fifo by a single thread.
        // 3.  The Incorrectness issue is also not a big problem, since the stack currently
        //     doesn't use the Broadcast member of this class.
        //
        // Note:  The implementation of Broadcast remains incomplete - since it is currently
        //        unused and would require an additional CriticalSection Enter and Leave to
        //        keep track of a counter (see the above link for more info).  This can be
        //        easily added in the future if required.
        mutex.Unlock();
        DWORD ret = WaitForSingleObject(mId, ms);
        mutex.lock();
        assert(ret != WAIT_FAILED);
        return (ret == WAIT_OBJECT_0);
#   endif
#else
        UINT64 expires64 = SDTimeMilliSec()  + ms;
        timespec expiresTS;
        expiresTS.tv_sec = expires64 / 1000;
        expiresTS.tv_nsec = (expires64 % 1000) * 1000000L;

        assert( expiresTS.tv_nsec < 1000000000L );

        //std::cerr << "Condition::Wait " << mutex << "ms=" << ms << " expire=" << expiresTS.tv_sec << " " << expiresTS.tv_nsec << std::endl;
        INT32 ret = pthread_cond_timedwait(&mId, &mutex.m_mutexId, &expiresTS);

        if (ret == EINTR || ret == ETIMEDOUT)
        {
            return FALSE;
        }
        else
        {
            //std::cerr << this << " pthread_cond_timedwait failed " << ret << " mutex=" << mutex << std::endl;
            assert( ret == 0 );
            return TRUE;
        }
#endif
    }

    BOOL  CSDCondition::Wait (CSDMutex* mutex, UINT32 ms)
    {
        return this->Wait(*mutex, ms);
    }

    void  CSDCondition::Signal ()
    {
#if (defined(WIN32) || defined(WIN64))
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
        unsigned signals = 0;

        INT32 res = 0;
        res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
        assert(res == WAIT_OBJECT_0);

        if (m_waiting != 0) // the m_gate is already closed
        {
            if (m_blocked == 0)
            {
                res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
                assert(res);
                return;
            }

            ++m_waiting;
            --m_blocked;
            signals = 1;
        }
        else
        {
            res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
            assert(res == WAIT_OBJECT_0);
            if (m_blocked > m_gone)
            {
                if (m_gone != 0)
                {
                    m_blocked -= m_gone;
                    m_gone = 0;
                }
                signals = m_waiting = 1;
                --m_blocked;
            }
            else
            {
                res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
                assert(res);
            }
        }

        res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
        assert(res);

        if (signals)
        {
            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_queue), signals, 0);
            assert(res);
        }
#  else
        BOOL ret = SetEvent(
                       mId // HANDLE hEvent   // handle to event object
                   );
        assert(ret);
#  endif
#else
        INT32 ret = pthread_cond_signal(&mId);
        assert( ret == 0);
#endif
    }


    void  CSDCondition::Broadcast()
    {
#if (defined(WIN32) || defined(WIN64))
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
        unsigned int signals = 0;

        INT32 res = 0;
        res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
        assert(res == WAIT_OBJECT_0);

        if (m_waiting != 0) // the m_gate is already closed
        {
            if (m_blocked == 0)
            {
                res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
                assert(res);
                return;
            }

            m_waiting += (signals = m_blocked);
            m_blocked = 0;
        }
        else
        {
            res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
            assert(res == WAIT_OBJECT_0);
            if (m_blocked > m_gone)
            {
                if (m_gone != 0)
                {
                    m_blocked -= m_gone;
                    m_gone = 0;
                }
                signals = m_waiting = m_blocked;
                m_blocked = 0;
            }
            else
            {
                res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
                assert(res);
            }
        }

        res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
        assert(res);

        if (signals)
        {
            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_queue), signals, 0);
            assert(res);
        }
#  else
        assert(0);
#  endif
#else
        pthread_cond_broadcast(&mId);
#endif
    }

} // namespace UCODE

