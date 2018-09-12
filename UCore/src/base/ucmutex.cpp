#include <cassert>
#include <cerrno>
#include "ucmutex.h"
#include "uclock.h"

#if (defined(WIN32) || defined(WIN64))
#  include <windows.h>
#  include <winbase.h>
#else
#  include <pthread.h>
#endif

namespace UCODE
{
    BOOL SDMutexInit(SDMutexId & mid)
    {
#if (defined(WIN32) || defined(WIN64))
        InitializeCriticalSection(&mid);
        return TRUE;
#else
        int  rc = pthread_mutex_init(&mid,0);
        assert( rc == 0 );
        return rc == 0;
#endif
    }

    void SDMutexLock(SDMutexId & mid)
    {
#if (defined(WIN32) || defined(WIN64))
        EnterCriticalSection(&mid);
#else
        int  rc = pthread_mutex_lock(&mid);
        assert( rc != EINVAL );
        assert( rc != EDEADLK );
        assert( rc == 0 );
#endif
    }

    void SDMutexUnlock(SDMutexId & mid)
    {
#if (defined(WIN32) || defined(WIN64))
        LeaveCriticalSection(&mid);
#else
        int  rc = pthread_mutex_unlock(&mid);
        assert( rc != EINVAL );
        assert( rc != EPERM );
        assert( rc == 0 );
#endif
    }

    BOOL SDMutexUninit(SDMutexId &mid)
    {
#if (defined(WIN32) || defined(WIN64))
        DeleteCriticalSection(&mid);
        return 0;
#else
        int  rc = pthread_mutex_destroy(&mid);
        assert( rc != EBUSY );  // currently locked
        assert( rc == 0 );
        return rc == 0;
#endif
    }

    CSDMutex::CSDMutex()
    {
        SDMutexInit(m_mutexId);
    }

    CSDMutex::~CSDMutex ()
    {
        SDMutexUninit(m_mutexId);
    }

    void  CSDMutex::Lock()
    {
        SDMutexLock(m_mutexId);
    }

    void  CSDMutex::Unlock()
    {
        SDMutexUnlock(m_mutexId);
    }

    CSDRWMutex::CSDRWMutex() :
        m_readerCount(0),
        m_hasWriterLock(false),
        m_pendingWriteCount(0)
    {

    }

    CSDRWMutex::~CSDRWMutex()
    {

    }

    void CSDRWMutex::LockRead()
    {
        CSDLock<CSDMutex> guard(m_mutex);
        while ( m_hasWriterLock || m_pendingWriteCount > 0 )
        {
            m_readCond.Wait(m_mutex);
        }
        m_readerCount++;
    }

    void CSDRWMutex::LockWrite()
    {
        CSDLock<CSDMutex> lock(m_mutex);

        m_pendingWriteCount++;

        while ( m_hasWriterLock || m_readerCount > 0 )
        {
            m_pendingWriteCond.Wait(m_mutex);
        }

        m_pendingWriteCount--;
        m_hasWriterLock = true;
    }


    void CSDRWMutex::Lock()
    {
        LockWrite();
    }


    void CSDRWMutex::Unlock()
    {
        CSDLock<CSDMutex>   lock(m_mutex);
        // Unlocking a write lock.
        //
        if ( m_hasWriterLock )
        {
            assert( m_readerCount == 0 );

            m_hasWriterLock = false;

            // Pending writers have priority. Could potentially starve readers.
            //
            if ( m_pendingWriteCount > 0 )
            {
                m_pendingWriteCond.Signal();
            }

            // No writer, no pending writers, so all the readers can go.
            //
            else
            {
                m_readCond.Broadcast();
            }
        }

        // Unlocking a read lock.
        //
        else
        {
            assert( m_readerCount > 0 );
            m_readerCount--;

            if ( m_readerCount == 0 && m_pendingWriteCount > 0 )
            {
                m_pendingWriteCond.Signal();
            }
        }
    }

    unsigned int CSDRWMutex::ReaderCount() const
    {
        return ( m_readerCount );
    }

    unsigned int CSDRWMutex::PendingWriterCount() const
    {
        return ( m_pendingWriteCount );
    }


    CSDRecursiveMutex::CSDRecursiveMutex()
    {
#if !(defined(WIN32) || defined(WIN64))
        int rc = pthread_mutexattr_init(&m_mutexAttr);
#if defined(__linux__)
        pthread_mutexattr_settype(&m_mutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
#else
        pthread_mutexattr_settype(&m_mutexAttr, PTHREAD_MUTEX_RECURSIVE);
#endif

        rc = pthread_mutex_init(&m_mutexId, &m_mutexAttr);
        assert( rc == 0 );
#else
        InitializeCriticalSection(&m_mutexId);
#endif
    }


    CSDRecursiveMutex::~CSDRecursiveMutex ()
    {
        SDMutexUninit(m_mutexId);

    }

    void CSDRecursiveMutex::Lock()
    {
        SDMutexLock(m_mutexId);
    }

    void CSDRecursiveMutex::Unlock()
    {
        SDMutexUnlock(m_mutexId);
    }


} // namespace UCODE





