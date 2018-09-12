#ifndef UCLOCK_H
#define UCLOCK_H

#include "ucmutex.h"

namespace  UCODE
{
    template <class MT = CSDMutex>
    class CSDLock
    {
    public:
        CSDLock(MT & mt):m_mutex(mt)
        {
            m_mutex.Lock();
        }
        ~CSDLock()
        {
            m_mutex.Unlock();
        }
    private:
        MT & m_mutex;
    };

    typedef CSDLock<CSDMutex>  CSDMutexLock;

    typedef CSDLock<CSDRecursiveMutex>  CSDRecursiveLock;

    class CSDReadLock
    {
    public:
        CSDReadLock (CSDRWMutex &lock):m_mutex(lock)
        {
            m_mutex.LockRead();
        }
        ~CSDReadLock()
        {
            m_mutex.Unlock();
        }

    private:
        CSDRWMutex & m_mutex;
    };

    class CSDWriteLock
    {
    public:
        CSDWriteLock (CSDRWMutex &lock):m_mutex(lock)
        {
            m_mutex.LockWrite();
        }
        ~CSDWriteLock()
        {
            m_mutex.Unlock();
        }

    private:
        CSDRWMutex & m_mutex;
    };

    template<class Mutex>
    class CSDLockGuard
    {
    private:
        Mutex& m_lock;
    public:
        CSDLockGuard(Mutex& oLock) : m_lock(oLock)
        {
            m_lock.Lock();
        }
        ~CSDLockGuard()
        {
            m_lock.Unlock();
        }

        class GuardOff
        {
            CSDLockGuard& m_guard;
        public:
            GuardOff(CSDLockGuard<Mutex>& g):m_guard(g)
            {
                m_guard.m_lock.Unlock();
            }
            ~GuardOff()
            {
                m_guard.m_lock.Lock();
            }
        };
    };


    /** @} */
}
#endif


