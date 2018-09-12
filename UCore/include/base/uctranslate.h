#ifndef UC_TRANSLATE_H
#define UC_TRANSLATE_H

//#include "ucdebug.h"
#include "uctype.h"
#include <string>
using namespace std;

#if UC_WITH_LIBICONV
#include "iconv.h"
#include "localcharset.h"
namespace UCODE
{
    /**
    * @brief 一种字符集到另一种字符集的转换
    */
	class CSDTranslate
	{
	public:

		/**
		* @brief 构造函数，初始化转换句柄
		*/
		CSDTranslate();
		
		/**
		* @析构函数，关闭转换句柄
		*/
		~CSDTranslate();


	public:

        /**
        * @brief 从源字符集转换到目标字符集
        * @param cpSrc : 源字符集
        * @param nSrcLen : 源字符集长度
        * @param cpDesc  : 目标字符集
        * @param nDescLen: 目标字符集长度
        * @return -1:转换失败 否则成功转换
        */
		size_t Translate(CHAR *cpSrc, 
						 size_t nSrcLen, 
						 CHAR * cpDesc,
						 size_t nDescLen );

        /**
        * @brief 初始化源字符集和目的字符集
        * @param cpFromCharset : 源字符集
        * @param cpToCharset   : 目标字符集
        * @return TRUE:初始化成功 FALSE:初始化失败
        */
		bool   Init(const char *cpFromCharset,const char *cpToCharset);
	private:
		iconv_t    m_hHandle;
		string	   m_strFromCharset;
		string	   m_strToCharset;
	};

    /**
    * @brief 字符集转换
    * @param desc : 目标字符集
    * @param src  : 源字符集
    * @param input: 输入缓冲区
    * @param iLen :输入缓冲区的大小
    * @param output : 输出缓冲区
    * @param oLen   : 输出缓冲区的大小
    * @return 0:成功 -1:失败
    */
	INT32		SDIconvCovert(CHAR *desc, CHAR *src, CHAR *input, size_t ilen, CHAR *output, size_t& olen);

    /**
    * @brief 获取本地操作系统的字符集编码规范
    * @return 本地操作系统的字符集编码
    */
	const CHAR *  SDGetLocaleCharset (VOID);
}



#endif

#endif

