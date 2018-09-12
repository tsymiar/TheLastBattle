#ifndef __OBJECT_DQUEUE_ALLOCATOR_H__
#define __OBJECT_DQUEUE_ALLOCATOR_H__

#include "ucmutex.h"
#include "uclock.h"
#include <deque>
// dual queue implement for object allocator

namespace UCODE
{

    template<typename T, typename ReadMutex = CSDNonMutex,typename WriteMutex = CSDNonMutex >
    class ObjectDQueueAllocator
    {
        enum
        {
            POOL_INFLATE_SIZE= 100,
        };
    public:
        typedef size_t   size_type;
        typedef T*       pointer;
        typedef T&       reference;
        typedef const T* const_pointer;
        typedef const T& const_reference;
        typedef T        value_type;

        typedef std::deque<pointer> ObjectQueue;
        typedef typename ObjectQueue::iterator ObjectQueueItr;

        ObjectDQueueAllocator(int initSize, int growSize) throw()
        {
            m_totalSize =0;
            m_growSize = growSize;
            CSDLock<ReadMutex> lock(m_readMutex);
            inflate(initSize);
        }

        ~ObjectDQueueAllocator() throw()
        {
            {
                CSDLock<ReadMutex> lock(m_readMutex);
                for(ObjectQueueItr itr = m_readObjects.begin(); itr != m_readObjects.end(); ++itr)
                {
                    ::operator delete (*itr);
                }
                this->m_readObjects.clear();
            }

            {
                CSDLock<WriteMutex> lock(m_writeMutex);
                for(ObjectQueueItr itr = m_writeObjects.begin(); itr != m_writeObjects.end(); ++itr)
                {
                    ::operator delete (*itr);
                }
                this->m_writeObjects.clear();
            }
        }

        pointer address(reference __x) const
        {
            return &__x;
        }

        const_pointer address(const_reference __x) const
        {
            return &__x;
        }

        // NB: __n is permitted to be 0.  The C++ standard says nothing
        // about what the return value is when __n == 0.
        T* allocate()
        {
            CSDLock<ReadMutex> lock(m_readMutex);
            if (m_readObjects.empty())
            {
                this->inflate();
            }

            T * ret = m_readObjects.front();
            m_readObjects.pop_front();
            return ret;
        }

        // __p is not permitted to be a null pointer.
        void deallocate(pointer __p)
        {
            CSDLock<WriteMutex> lock(m_writeMutex);
            m_writeObjects.push_back(__p);
        }

        inline size_type ReadSize()
        {
            CSDLock<ReadMutex> lock(m_readMutex);
            return m_readObjects.size();
        }

        size_type WriteSize() const throw()
        {
            CSDLock<ReadMutex> lock(m_writeMutex);
            return m_writeObjects.size();
        }

    private:
        void inflate(int count = POOL_INFLATE_SIZE)
        {
            //NOTICE: read is locked outside
            CSDLock<WriteMutex> lock(m_writeMutex);
            if (m_writeObjects.empty())
            {
                for (int i = 0; i < count ; i++)
                {
                    pointer  _p = static_cast<pointer>(::operator new (sizeof(value_type)));
                    m_readObjects.push_back(_p);
                }
                m_totalSize += count;
            }
            else
            {
                m_readObjects.swap(m_writeObjects);
            }
        }

        ObjectDQueueAllocator(const ObjectDQueueAllocator&) throw()
        {
        }

        // not allow copy
        template<typename T1>
        ObjectDQueueAllocator(const ObjectDQueueAllocator<T1>&) throw()
        {
        }

        template<typename T1>
        void operator = (const ObjectDQueueAllocator<T1>&) throw()
        {

        }

        ObjectQueue m_readObjects;
        ObjectQueue m_writeObjects;

        ReadMutex m_readMutex;
        WriteMutex m_writeMutex;
        int m_totalSize ;
        int m_growSize;
    };
}
#endif // 





