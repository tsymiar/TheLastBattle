#ifndef CONNECTCTRL_H
#define CONNECTCTRL_H

#include "ucsingleton.h"
#include "ucdataqueue.h"
#include "ucnet.h"
#include "netrequest.h"
using namespace UCODE;

/**
* @brief 连接控制类,在本机为客户端时使用,用来控制对服务器的请求连接,并管理连接
* 此类与CCpListener对应
*
*/
class CConnectCtrl
{
	CConnectCtrl();
	~CConnectCtrl();

	DECLARE_SINGLETON(CConnectCtrl)
public:

	/**
	* @brief 控制连接类的线程函数
	* @param pParam :　线程函数的参数,本函数输入这个连接控制类的指针即可
	* @return 线程函数的返回值
	*/
	static unsigned WINAPI ThreadFunc(LPVOID pParam);

	/**
	* @brief 初始连接控制类
	* @return 是否初始化成功
	*/
    bool Init();

	/**
	* @brief 清理连接控制类
	* @return void
	*/
	void Uninit();

	//// 2009-03-24 cwy add for interface expanding
	// add bind function
	//bool PushBindReq(UINT32 dwConnectorID, const struct sockaddr* pstSockAddr, INT32 nSockAddrLen);
	////~

	/**
	* @brief 插入"连接请求"
	* @param dwConnectorID : 连接器的ID, 该连接器用于请求连接
	* @param dwIP : 远程的IP地址
	* @param wPort : 远程的端口号
	* @param poPacketParser : 数据包解析器
	* @param poSession : 连接的会话
	* @param dwRecvBufSize : 接收缓存大小
	* @param dwSendBufSize : 发送缓存大小
	* @param bNeedBind : 是否需要绑定本机的IP地址
	* @param pszBindIP : 本地绑定的IP地址
	* @param wBindPort : 本地绑定的端口号
	*/
	bool PushConnReq(UINT32 dwConnectorID, UINT32 dwIP, UINT16 wPort, 
		ISDPacketParser* poPacketParser, ISDSession* poSession, 
		UINT32 dwRecvBufSize, UINT32 dwSendBufSize,
		//// 2009-03-24 cwy add for interface expanding, add bind function
		bool bNeedBind, const char* pszBindIP, UINT16 wBindPort,const BOOL bNodelay = FALSE);

	/**
	* @brief hSock代表的套接字绑定IP地址
	* @param hSock : 套接字
	* @param pIpAddr : 需要绑定的IP地址
	* @param port : 需要绑定的端口号
	*/
	bool  BindAddress(SOCKET hSock, const char * pIpAddr, UINT16 port);

	/**
	* @brief 执行功能函数
	* @return void
	*/
	void OnExecute();

protected:
	//SOCKET _CreateSocket(SConnReq* pstConnReq);

	/**
	* @brief 处理"连接请求",向远程服务器请求连接
	* @return void
	*/
	void _ProcRequests();

	/**
	* @brief 处理连接事件,查看连接是否成功
	* @return void
	*/
	void _ProcEvents();

	/**
	* @brief 关闭索引所代表的连接事件
	* @param dwIndex : 代表连接事件的索引
	* @return void
	*/
	void _CloseEvent(UINT32 dwIndex);

	/**
	* @brief 压缩"连接请求"数组,避免事件数组中出现空挡
	* @return void
	*/
	void _CompressEvent();

	/**
	* @brief 清理连接控制类
	* @return void
	*/
	void _CleanUp();

	/**
	* @brief 当连接控制类出错时,执行此类
	* @param hSock : 套接字
	* @param pstConnReq : "连接请求"
	* @return void
	*/
	void _OnSockError(SOCKET hSock ,SConnReq* pstConnReq);

protected:
	bool				m_bTerminate;
	HANDLE				m_hThread;
	SConnReq*			m_pAllReqArray;
	CSDDataQueue		m_oFreeQueue;
	CSDDataQueue		m_oReqQueue;

	UINT32				m_dwSockCount;
	SConnReq**			m_pProcReqArray;
	WSAEVENT*			m_pEventsArray;
	SOCKET*				m_pSockArray;
};

#endif

