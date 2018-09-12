#ifndef UCNETUTILS_H
#define UCNETUTILS_H

#if (defined(WIN32) || defined(WIN64))
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/resource.h>
#endif

#include <string>
#include "uctype.h"
#include "ucstring.h"
namespace UCODE
{

    /**
    * @defgroup groupnetutil 网络操作
    * @ingroup  UCODE
    * @{
    */


    typedef  struct hostent SDHostent;

    /**
    * @brief
    * IPv4 将整型的ip地址转换为点分隔的地址表示 0.0.0.0
    * 或将IPv6地址转换为字符串表示方式
    * @param af : address family, AF_INET/AF_INET6
    * @param pSrc : 需要被转化的网络地址,
    * @param pDst : 转化后的网络地址存放的空间
    * @param size : 网络地址存放空间的最大长度
    * @return 转化后的网络地址存放的空间(`pDst'), 或者失败返回NULL
    */
    const TCHAR*  SDNetInetNtop(INT32 af, const VOID *pSrc, TCHAR *pDst, size_t size);

    /**
    * @brief
    *  将字符串表示的IP地址转换为整型或IPv6结构表示
    * @param af : address family, AF_INET/AF_INET6
    * @param pSrc : 需要被转化的网络地址
    * @param pDst : 转化后的网络地址存放的空间
    * @  正确返回TRUE ,如果src所表示的地址无效,或与地址族不匹配,则返回FALSE
    */
    BOOL  SDNetInetPton(INT32 af, const CHAR *pSrc, VOID *pDst);

    /**
    * @brief
    * 将UINT32类型数据从网络顺序转化为主机顺序
    * @param netlong : UINT32类型网络顺序数据
    * @return UINT32类型主机顺序数据
    */
    inline UINT32  SDNtohl(UINT32 netlong)
    {
        return ntohl(netlong);
    }

    /**
    * @brief
    * 将UINT16类型数据从网络顺序转化为主机顺序
    * @param netshort : UINT16类型网络顺序数据
    * @return UINT16类型主机顺序数据
    */
    inline UINT16  SDNtohs(UINT16 netshort)
    {
        return ntohs(netshort);
    }

    /**
    * @brief
    * 将UINT32类型数据从主机顺序转化为网络顺序
    * @param hostlong : UINT32类型主机顺序数据
    * @return UINT32类型网络顺序数据
    */
    inline UINT32  SDHtonl(UINT32 hostlong)
    {
        return htonl(hostlong);
    }

    /**
    * @brief
    * 将UINT64类型数据从主机顺序转化为网络顺序
    * @param hostlong : UINT64类型主机顺序数据
    * @return UINT64类型网络顺序数据
    */
    inline UINT64  SDHtonll(UINT64 number)
    {
        return ( htonl( UINT32((number >> 32) & 0xFFFFFFFF)) |
                 (UINT64(htonl(UINT32(number & 0xFFFFFFFF))) << 32));
    }

    /**
    * @brief
    * 将UINT64类型数据从网络顺序转化为主机顺序
    * @param hostlong : UINT64类型网络顺序数据
    * @return UINT64类型主机顺序数据
    */
    inline UINT64  SDNtohll(UINT64 number)
    {
        return ( ntohl( UINT32((number >> 32) & 0xFFFFFFFF) ) |
                 (UINT64 (ntohl(UINT32(number & 0xFFFFFFFF)))  << 32));
    }


    /**
    * @brief
    * 将UINT16类型数据从主机顺序转化为网络顺序
    * @param hostlong : UINT16类型主机顺序数据
    * @return UINT16类型网络顺序数据
    */
    inline UINT16   SDHtons(UINT16 hostlong)
    {
        return htons(hostlong);
    }

    /**
    * @brief
    * 转化一个字符串型的IPv4的地址为一个数字型的IP地址
    * @param pszAddr : 字符串型的IPv4的地址
    * @return 数字型的IP地址
    */
    inline ULONG  SDInetAddr(const TCHAR* pszAddr)
    {
        return inet_addr(_SDTT2A(pszAddr));
    }

    /**
    * @brief
    * 转化一个数字型的IPv4的地址为一个字符串型的IP地址
    * @param lIp : 数字型的IPv4的地址
    * @return 字符串型的IP地址，如果出错，返回NULL
    */
    inline tstring   SDInetNtoa(LONG lIp)
    {
        struct in_addr addr;
#if (defined(WIN32) || defined(WIN64))
        addr.S_un.S_addr = lIp;
#else
        addr.s_addr = lIp;
#endif
        std::string strIp = inet_ntoa(addr);
        tstring wstrIp =  _SDTA2T(strIp.c_str());
        return wstrIp;
    }

    /**
    * @brief
    * 获取机器名对应的IP地址
    * @param pszName : 机器名
    * @return 机器的IP地址
    */
    LONG  SDGetIpFromName(const CHAR * pszName);

	/**
	* @brief
	* 获取主机名	
	* @return 机器的主机名
	*/
    tstring  SDGetHostName();

	/**
	* @brief
	* 获取主机名对应的主机信息	
    * @param pszName : 主机名
	* @return 机器的主机信息
	*/
    SDHostent*  SDGetHostByName(const TCHAR * pszName); // 域名解析

    /**
    * @brief
    * 获取当前机器的本地IP地址
    * @param dwIp :  用于获取本地的IP地址的数组
    * @param dwCount : 数组长度
    * @return 本地IP地址的数量
    */
    INT32  SDGetLocalIp(ULONG dwIp[], UINT32 dwCount);

	//TODO get mac address
    /** @} */
    

	/**
    * @brief 设置文件句柄数限制
	* @param dwSetLimit : 受限制的文件句柄数
    * @return  0: success -1: getrlimit failed -2: setrlimit failed
    */
	INT32 SetFileHandleLimit(const UINT32 dwSetLimit);


}//

#endif /// 

