#ifndef CPLISTENER_H
#define CPLISTENER_H
#include <mswsock.h>
#include "cpsock.h"
#include "ucnet.h"
#include "conndatamgr.h"

using namespace UCODE;

/**
* @brief 完成端口监听器,在本机为服务器端时,此类是使用IOCP真实接受建立连接的类
* 此类与CConnectCtrl对应
*/
class CCpListener
{
public:
	CCpListener();
	~CCpListener();

	/**
	* @brief 设置完成端口监听器ID
	* @param dwID : 设置的ID
	* @return void
	*/
	inline void SetID(UINT32 dwID) { m_dwID = dwID; }

	/**
	* @brief 设置数据包解析器
	* @param poPacketParser : 设置的数据包解析器
	* @return void
	*/
	inline void SetPacketParser(ISDPacketParser* poPacketParser) { m_poPacketParser = poPacketParser; }

	/**
	* @brief 设置会话工厂类
	* @param poSessionFactory : 设置的会话工厂类
	* @return void
	*/
	inline void SetSessionFactory(ISDSessionFactory* poSessionFactory) { m_poSessionFactory = poSessionFactory; }

	/**
	* @brief 设置缓存大小
	* @param dwRecvBufSize : 设置接收缓存大小
	* @param dwSendBufSize : 设置发送缓存大小
	* @return void
	*/
	void SetBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize) { m_dwRecvBufSize = dwRecvBufSize; m_dwSendBufSize = dwSendBufSize; }

	/**
	* @brief 开始监听
	* @param pszIP : 本地IP地址
	* @param wPort : 本地端口号
	* @param bReUseAddr : 是否端口复用
	* @return 监听成功返回true,监听失败返回false
	*/
	bool Start(const char* pszIP, UINT16 wPort, bool bReUseAddr);

	/**
	* @brief 停止监听
	* @return void
	*/
	void Stop();

	/**
	* @brief 传输一个建立连接的每IO数据
	* @param pstPerIoData : 此建立连接请求的每IO数据
	* @return 是否建立连接成功，true为成功
	*/
	bool PostAcceptEx(SPerIoData* pstPerIoData);

	/**
	* @brief 当客户端发送一个建立连接请求时，执行此函数
	* @param bSucc : 此连接请求是否成功，true为成功
	* @param pstPerIoData : 此建立连接请求的每IO数据
	* @return void
	*/
	void OnAccept(BOOL bSucc, SPerIoData* pstPerIoData);

    VOID         SetNoDelay(const BOOL bNoDelay = FALSE);
protected:

	/**
	* @brief 初始化并且接受建立连接
	* @return 连接是否建立成功,true为成功,false为失败
	*/
	bool _InitAcceptEx();

	/**
	* @brief 获取由此每IO数据建立的连接的本地数据和远程数据
	* @param pstPerIoData : 每IO数据
	* @param RemoteAddr : 远程服务器的地址
	* @param LocalAddr : 本地服务器的地址
	*/
	void GetSockAddress(SPerIoData * pstPerIoData, sockaddr_in & RemoteAddr, sockaddr_in &  LocalAddr); 

protected:
    SPerHandleData              m_stPerHandleData;
    SOCKET                      m_hListenSock;
    LPFN_ACCEPTEX               m_lpfnAcceptEx;
    LPFN_GETACCEPTEXSOCKADDRS   m_lpfnGetAcceptExSockaddrs;
    SPerIoData*                 m_pPerIoDataArray;

    UINT32                      m_dwID;				//对应的CUCODEListener的ID
    ISDPacketParser*			m_poPacketParser;
    ISDSessionFactory*			m_poSessionFactory;
    UINT32                      m_dwRecvBufSize;
    UINT32                      m_dwSendBufSize;
    volatile bool               m_bStart;
    //CCpLock                   m_oReleaseLock;//Comment by hexi. 2009-08-21
    static CSDMutex             m_soGetAddrMutex; //lcj
    volatile UINT32             m_dwReleaseCount;//Modify by hexi.Add 'volatile' qualifer.2009-08-21
    BOOL                m_bNodelay;
};

#endif

