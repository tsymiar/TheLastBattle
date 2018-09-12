#ifndef UCSINGLETON_H
#define UCSINGLETON_H
/**
* @file sdsingleton.h
* @brief 单例（单件）系列
*
**/
#include "uctype.h"

namespace   UCODE
{
    /**
    * @defgroup groupsingleton 单件类封装
    * @ingroup  UCODE
    * @{
    */

    /**
    如何使用
    1. 避免随意使用单件对象，你应当声明构造函数和析构函数为私有
    2. 避免随意使用单例对象，你应当声明拷贝构造函数和operator ＝ 为私有
    3. 添加DECLARE_SINGLETON(cls)至你的类的声明处, cls为你类的名称
    4. 添加IMPLEMENT_SINGLETON(cls)至你的类的定义处

    for example:
    //Sample.h
    class CSample
    {
    CSample();
    ~CSample();
    DECLARE_SINGLETON(CSample)
    }

    //Sample.cpp
    IMPLEMENT_SINGLETON(CSample)
    */
#define SIMPLE_DECLARE_SINGLETON(cls)\
public:\
    static cls* CreateInstance()\
    {\
        static cls oCls;\
        return &oCls;\
    }

#define DECLARE_SINGLETON(cls)\
private:\
	 static cls* m_poInstance;\
public:\
	static BOOL CreateInstance()\
	{\
	if(NULL == m_poInstance)\
	m_poInstance = new cls;\
	return m_poInstance != NULL;\
}\
	static cls* Instance(){ return m_poInstance; }\
	static VOID DestroyInstance()\
	{\
	if(m_poInstance != NULL)\
	{\
	delete m_poInstance;\
	m_poInstance = NULL;\
}\
}

#define IMPLEMENT_SINGLETON(cls) \
	cls* cls::m_poInstance = NULL;


//The second way to declare a singleton class
#define  SINGLETON_FRIEND_CLASS_REGISTER(T)  friend  class  CSingleton<T>

    /**
    @brief 单件模板类
    *
    *@remarks
    *1.你的单件类应当以publicly的方式继承自此类并且构造函数，析构函数应当设置为私有的。
    *2.避免随意使用单例对象，你应当声明拷贝构造函数和operator ＝ 为私有
    *3.添加宏 SINGLETON_FRIEND_CLASS_REGISTER(T) 在类的声明处以注册本类为一个friend类
    */
    template<class  T>
    class   CSingleton
    {
    public:

        /**
        * @brief
        * 初始化创建单件类
        * @return false means failed
        */
        static BOOL CreateInstance()
        {
            if(NULL == m_poInstance)
            {
                m_poInstance = new T;
            }
            return m_poInstance != NULL;
        }

        /**
        * @brief
        * 获取单件类指针
        * @return 单件类指针，如果单件类实例初始化失败，返回NULL
        */
        static T* Instance(VOID)
        {
            return m_poInstance;
        }

        /**
        * @brief
        * 销毁单件类实例
        * @return VOID
        */
        static VOID DestroyInstance(VOID)
        {
            SDDelete  m_poInstance;
            m_poInstance = NULL;
        }

    protected:
        CSingleton(VOID) {}
        virtual ~CSingleton(VOID) {}

    private:
        static T*   m_poInstance;
    };

    template<class T>
    T* CSingleton<T>::m_poInstance = NULL;



// 单件模式的另一种实现,用户只需定义 typedef CSDSingleton<Type> TypeSingleton;

    template <class T>
    class CSDSingleton : private T
    {
    private:
        CSDSingleton();
        ~CSDSingleton();

    public:
        static T& Instance();
    };


    template <class T>
    inline CSDSingleton<T>::CSDSingleton()
    {
    }

    template <class T>
    inline CSDSingleton<T>::~CSDSingleton()
    {
    }

    template <class T>
    /*static*/ T &CSDSingleton<T>::Instance()
    {
        // function-local static to force this to work correctly at static
        // initialization time.
        static CSDSingleton<T> s_instance;
        return(s_instance);
    }


}

/** @} */

#endif

