#ifndef UCMUTEX_H
#define UCMUTEX_H

#include "uctype.h"
#include "uccondition.h"

#if !(defined(WIN32) || defined(WIN64))
#include <pthread.h>
#endif

namespace UCODE
{
    /**
    * @defgroup groupsync 线程同步
    * @ingroup  UCODE
    * @{
    */
#if (defined(WIN32) || defined(WIN64))
    typedef CRITICAL_SECTION SDMutexId;
#else
    typedef pthread_mutex_t SDMutexId;
#endif

    BOOL SDMutexInit(SDMutexId & id);

    VOID SDMutexLock(SDMutexId & id);

    VOID SDMutexUnlock(SDMutexId & id);

    BOOL SDMutexUninit(SDMutexId &id);


    /**
    *@brief 空锁，什么也不做
    */
    class CSDNonMutex
    {
    public:

        /**
        * @brief
        * 什么也不做
        * @return VOID
        */
        inline VOID  Lock() {}

        /**
        * @brief
        * 什么也不做
        * @return VOID
        */
        inline VOID  Unlock() {}

        /**
        * @brief
        * 什么也不做
        * @return VOID
        */
        inline VOID  LockRead() {}

        /**
        * @brief
        * 什么也不做
        * @return VOID
        */
        inline VOID  LockWrite() {}
    };

    /**
    *@brief 线程同步普通锁操作类
    */
    class CSDMutex
    {
        friend class CSDCondition;

    public:
        CSDMutex();
        ~CSDMutex();

        /**
        * @brief
        * 进入锁
        * @return VOID
        */
        VOID  Lock();

        /**
        * @brief
        * 离开锁
        * @return VOID
        */
        VOID  Unlock();

        /**
        * @brief
        * 进入读锁，当前能有多个读线程进入
        * @return VOID
        */
        VOID  LockRead()
        {
            Lock();
        }

        /**
        * @brief
        * 进入写锁，当前只能有一个线程进入
        * @return VOID
        */
        VOID  LockWrite()
        {
            Lock();
        }

    private:
        //  no value sematics, therefore private and not implemented.
        CSDMutex (const CSDMutex&);
        CSDMutex& operator= (const CSDMutex&);

    private:
#if (defined(WIN32) || defined(WIN64))
        SDMutexId m_mutexId;
#else
        mutable SDMutexId m_mutexId;
#endif
    };

    /**
    *@brief 线程同步读写锁操作类
    */
    class CSDRWMutex
    {
    public:
        CSDRWMutex();
        ~CSDRWMutex();

        /**
        * @brief
        * 进入读锁，当前能有多个读线程进入
        * @return VOID
        */
        VOID  LockRead();

        /**
        * @brief
        * 进入写锁，当前只能有一个线程进入
        * @return VOID
        */
        VOID  LockWrite();

        /**
        * @brief
        * 进入锁
        * @return VOID
        */
        VOID  Lock();

        /**
        * @brief
        * 离开锁
        * @return VOID
        */
        VOID  Unlock();

        /**
        * @brief
        * 当前有多少个读取线程同时读取
        * @return 读取者的数量
        */
        unsigned int  ReaderCount() const;

        /**
        * @brief
        * 当前有多少个写操作线程在等待
        * @return 等待写入者的数量
        */
        unsigned int  PendingWriterCount() const;

    private:
        CSDMutex m_mutex;
        CSDCondition m_readCond;
        CSDCondition m_pendingWriteCond;
        unsigned int m_readerCount;
        bool m_hasWriterLock;
        unsigned int m_pendingWriteCount;
    };

    /**
    *@brief 线程同步递归锁操作类
    */
    class CSDRecursiveMutex
    {
    public:
        CSDRecursiveMutex();
        ~CSDRecursiveMutex();

        /**
        * @brief
        * 进入锁
        * @return VOID
        */
        VOID  Lock();

        /**
        * @brief
        * 离开锁
        * @return VOID
        */
        VOID  Unlock();

        /**
        * @brief
        * 进入读锁，当前能有多个读线程进入
        * @return VOID
        */
        VOID  LockRead()
        {
            Lock();
        }

        /**
        * @brief
        * 进入写锁，当前只能有一个线程进入
        * @return VOID
        */
        VOID  LockWrite()
        {
            Lock();
        }
    private:
        SDMutexId m_mutexId;
#if !(defined(WIN32) || defined(WIN64))
        pthread_mutexattr_t m_mutexAttr;
#endif
    };

    /** @} */
} //namespace UCODE



#endif

