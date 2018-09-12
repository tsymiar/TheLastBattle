#ifndef UCOBJECTPOOL_H
#define UCOBJECTPOOL_H

#include "detail/ucobject_allocator.h"
#include "detail/ucobject_allocator_ex.h"
#include "detail/ucobject_dqueue_allocator.h"
#include "ucmacros.h"
#include "ucconfig.h"
#include "uctype.h"
//#include "ucdebug.h"

#if UC_WITH_BOOST
#include <boost/preprocessor/repetition.hpp>
#endif

namespace UCODE
{
    /**
    * @defgroup groupmemorypool 内存池(包括对象池)
    * @ingroup  UCODE
    * @{
    */

    /**
    *@brief 对象池类
    */
    template <typename _Tp,typename MT = CSDNonMutex,
             typename _Alloc=ObjectAllocator<_Tp,MT> >
    class CSDObjectPool
    {
    public:

		/**
		* @brief 构造函数，分配给定大小的内存
		* @param dwInitCount : 内存的初始大小
		* @param dwGrouCount : 内存块的个数
		*/
        CSDObjectPool(UINT32 dwInitCount = 0, UINT32 dwGrouCount = 1)
            :m_allocator(dwInitCount,dwGrouCount)
        {
        }

#if UC_WITH_BOOST

        /**
        * @brief
        * 分配一个对象, 使用boost库，可以将支持的构造函数的参数扩展为20个
        * @return 返回对象的指针，如果失败返回NULL
        */
#define DECL(z,n,text) T##n t##n
#define MALLOC_DECL(z,n,text)  \
	BOOST_PP_IF(n,template < , ) BOOST_PP_ENUM_PARAMS(n,typename T) BOOST_PP_IF(n,>, ) \
	_Tp* UCAPI Alloc(BOOST_PP_ENUM(n,DECL,~))   \
		{ \
		return new (m_allocator.allocate())_Tp(BOOST_PP_ENUM_PARAMS(n,t)); \
		}

        BOOST_PP_REPEAT (20,MALLOC_DECL,~)



#else
        /**
        * @brief
        * 分配一个对象, 如果对象构造函数没有参数，使用此接口
        * @return 返回对象的指针，如果失败返回NULL
        */
        inline _Tp* UCAPI  Alloc()
        {
            return new (m_allocator.allocate())_Tp;
        }

        /**
        * @brief
        * 分配一个对象, 对象的构造函数包含一个输入参数，以下一次为多个参数
        * @return 返回对象的指针，如果失败返回NULL
        */
        template<typename P1>
        _Tp * UCAPI Alloc(P1 &p1)
        {
            return new (m_allocator.allocate())_Tp(p1);
        }

		 /**
        * @brief
        * 分配一个对象, 对象的构造函数包含2个输入参数
        * @return 返回对象的指针，如果失败返回NULL
        */
        template<typename P1,typename P2>
        _Tp* UCAPI Alloc(P1 &p1,P2 &p2)
        {
            return new (m_allocator.allocate())_Tp(p1,p2);
        }

		 /**
        * @brief
        * 分配一个对象, 对象的构造函数包含3个输入参数
        * @return 返回对象的指针，如果失败返回NULL
        */
        template<typename P1,typename P2,typename P3>
        _Tp* UCAPI Alloc(P1 &p1,P2 &p2,P3 &p3)
        {
            return new (m_allocator.allocate())_Tp(p1,p2,p3);
        }

		 /**
        * @brief
        * 分配一个对象, 对象的构造函数包含4个输入参数
        * @return 返回对象的指针，如果失败返回NULL
        */
        template<typename P1,typename P2,typename P3,typename P4>
        _Tp* UCAPI Alloc(P1 &p1,P2 &p2,P3 &p3,P4 &p4)
        {
            return new (m_allocator.allocate())_Tp(p1,p2,p3,p4);
        }

		 /**
        * @brief
        * 分配一个对象, 对象的构造函数包含5个输入参数
        * @return 返回对象的指针，如果失败返回NULL
        */
        template<typename P1,typename P2,typename P3,typename P4,typename P5>
        _Tp* UCAPI Alloc(P1 &p1,P2 &p2,P3 &p3,P4 &p4,P5 &p5)
        {
            return new (m_allocator.allocate())_Tp(p1,p2,p3,p4,p5);
        }

        template<typename P1,typename P2,typename P3,typename P4,typename P5, typename P6>
        _Tp* UCAPI Alloc(P1 &p1,P2 &p2,P3 &p3,P4 &p4,P5 &p5,P6 &p6)
        {
            return new (m_allocator.allocate())_Tp(p1,p2,p3,p4,p5,p6);
        }
#endif // 
        /**
        * @brief
        * 归还一个对象
        * @param pObj : 对象指针
        * @return VOID
        */
        VOID  Free(_Tp * p)
        {
            p->~_Tp();
            m_allocator.deallocate(p);
        }

        /**
        * @brief 获取对象池
        * @return 对象池
        */
        _Alloc & UCAPI GetAllocator()
        {
            return m_allocator;
        }

    private:
        _Alloc m_allocator;

    };

}

#endif //

