#ifndef __OBJECT_ALLOCATOR_EX_H__
#define __OBJECT_ALLOCATOR_EX_H__

#include "ucmutex.h"
#include "uclock.h"
#include <deque>

namespace UCODE
{

    template<class T >
    class ObjectConstructor
    {
    public:
        void operator ()(T * )
        {
        }
    };

    template<class T>
    class ObjectDeconstructor
    {
    public:
        void operator ()(T * )
        {
        }
    };
    template<typename T, typename MT = CSDNonMutex,
             typename Constructor=ObjectConstructor <T> , typename Deconstructor =ObjectDeconstructor <T> ,
             int INIT_SIZE = 100  >
    class ObjectAllocatorEx
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

        ObjectAllocatorEx(int initSize =0 , int growSize = 1 ) throw()
        {
            m_totalSize =0;
            m_growSize = growSize;
            CSDLock<MT> lock(m_mutex);
            inflate(initSize );

        }



        ~ObjectAllocatorEx() throw()
        {
            CSDLock<MT> lock(m_mutex);
            for(ObjectQueueItr itr = m_objects.begin(); itr != m_objects.end(); ++itr)
            {
                ::operator delete (*itr);
            }
            this->m_objects.clear();
        }

        pointer address(reference __x) const
        {
            return &__x;
        }

        const_pointer address(const_reference __x) const
        {
            return &__x;
        }


        T* allocate()
        {
            CSDLock<MT> lock(m_mutex);
            if (m_objects.empty())
            {
                this->inflate();
            }

            T * ret = m_objects.front();
            m_objects.pop_front();

            Constructor   constructor;
            constructor(ret);
            return ret;
        }

        // __p is not permitted to be a null pointer.
        void deallocate(pointer __p)
        {
            CSDLock<MT> lock(m_mutex);
            m_objects.push_back(__p);
            Deconstructor deconstructor;
            deconstructor(__p);
        }

        inline size_type size()
        {
            CSDLock<MT> lock(m_mutex);
            return m_objects.size();
        }

        inline size_type max_size() const throw()
        {
            return m_totalSize;
        }

    private:
        void inflate(int count = POOL_INFLATE_SIZE)
        {
            //NOTICE: locked outside
            for (int i = 0; i < count ; i++)
            {
                pointer  _p = static_cast<pointer>(::operator new (sizeof(value_type)));
                m_objects.push_back(_p);
            }
            m_totalSize += count;
        }

        ObjectAllocatorEx(const ObjectAllocatorEx&) throw()
        {
        }

        // not allow copy
        template<typename T1>
        ObjectAllocatorEx(const ObjectAllocatorEx<T1>&) throw()
        {
        }

        template<typename T1>
        void operator = (const ObjectAllocatorEx<T1>&) throw()
        {

        }

        ObjectQueue m_objects;
        MT m_mutex;
        int m_totalSize ;
        int m_growSize;


    };
}
#endif // 
 



