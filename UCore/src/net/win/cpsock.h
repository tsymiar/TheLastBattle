#ifndef CPSOCK_H
#define CPSOCK_H

#include "ucnet.h"
#include "uclock.h"
#include "uccondition.h"
#include "ucnetconfig.h"
#include "ucnetopt.h"

using namespace UCODE;

class CCPSock;

enum EIocpOperation
{
	IOCP_RECV = 0,
	IOCP_SEND,
	IOCP_ACCEPT,
	IOCP_CONNECT, 
	IOCP_CLOSE
};

/**
* @brief 每句柄数据,和句柄关联在一起的数据
*/
struct SPerHandleData
{
	bool		bListen;
	void*		ptr;
};

/**
* @brief 每次IO的数据,和每次IO操作关联在一起的数据
*/
struct SPerIoData
{
	OVERLAPPED		stOverlapped;
	SOCKET			hSock;
	EIocpOperation	nOp;
	WSABUF			stWsaBuf;
	char			Buf[128];
};

struct CConnData;

/**
* @brief 代表一个套接字类
*
*/
class CCPSock
{
public:
	CCPSock();
	~CCPSock();

	/**
	* @brief 重置此套接字类
	*
	*/
	void Reset();

	/**
	* @brief 设置此套接字类的套接字
	* @param 被设置的套接字类
	*/
	inline void SetSock(SOCKET hSock) { m_hSock = hSock; }

	/**
	* @brief 返回此套接字类所代表的套接字
	* @return 返回的套接字
	*/
	inline SOCKET GetSock() { return m_hSock; }

	/**
	* @brief 在此套接字上附加发送消息的缓存
	* @param pRecvBuf : 发送消息的缓存
	* @param dwRecvBufSize : 发送消息缓存的大小
	*/
	void AttachRecvBuf(char* pRecvBuf, UINT32 dwRecvBufSize);

	/**
	* @brief 隔离此套接字上附加的发送消息缓存,执行此函数后,此套接字类不再附加任何发送缓存
	* @param ppRecvBuf : [输出参数]获取被隔离的发送消息缓存
	* @return 原附加的发送消息缓存的大小
	*/
	UINT32 DetachRecvBuf(char** ppRecvBuf);

	/**
	* @brief 在此套接字上附加接收消息的缓存
	* @param pSendBuf : 接收消息的缓存
	* @param dwSendBufSize : 接收消息缓存的大小
	*/
	void AttachSendBuf(char* pSendBuf, UINT32 dwSendBufSize);

	/**
	* @brief 隔离此套接字上附加的接收消息缓存,执行此函数后,此套接字类不再附加任何接收缓存
	* @param ppSendBuf : [输出参数]获取被隔离的接收消息缓存
	* @return 原附加的接收消息缓存的大小
	*/
	UINT32 DetachSendBuf(char** ppSendBuf);

	/**
	* @brief 设置与此套接字相关的"连接数据"
	* @param pConnData : 设置的"连接数据"
	*/
	void SetConnData(CConnData* pConnData);

	/**
	* @brief 获取与此套接字相关的"连接数据"
	* @return 获取的"连接数据"
	*/
	CConnData *GetConnData () {return this->m_pConnData;}

	/**
	* @brief 设置与此套接字相关的数据包解析器
	* @param poPacketParser : 设置的数据包解析器
	*/
	inline void SetPacketParser(ISDPacketParser* poPacketParser) { m_poPacketParser = poPacketParser; }

	/**
	* @brief 设置是否连接
	* @param bConnect : 设置是否连接
	* @return void
	*/
	inline void SetConnect(BOOL bConnect)
	{
		m_bConnect = bConnect;
	}

	/**
	* @brief 查看此套接字是否连接
	* @return true为连接,false为没有连接
	*/
	inline BOOL IsConnect()
	{
		return m_bConnect;
	}

	//UINT32 IncDelayReleaseStamp() { return ++m_dwDelayReleaseStamp; }

	/**
	* @brief 使用此套接字类发送数据
	* @param pData : 指向发送数据包的指针
	* @param dwLen : 发送的数据包长度
	*/
	void Send(const char* pData, UINT32 dwLen);

		/**
	* @brief 使用此套接字类发送数据
	* @param pData : 指向发送数据包的指针
	* @param dwLen : 发送的数据包长度
	*/
	void UpdateSend();

	/**
	* @brief 使用此套接字类同步发送数据
	* @param pData : 指向发送数据包的指针
	* @param dwLen : 发送的数据包长度
	*/
	INT32 SyncSend(const char * pData, UINT32 dwLen);

	/**
	* @brief 使用此套接字类异步发送数据
	* @param pData : 指向发送数据包的指针
	* @param dwLen : 发送的数据包长度
	*/
	bool AsyncSend(const char* pData, UINT32 dwLen);

	/**
	* @brief 由用户关闭此套接字类
	* @return void
	*/
	void Close(); //close from user 

	/**
	* @brief 将此套接字与IO完成端口关联起来
	* @return true为关联成功,false为关联失败
	*/
	bool AssociateWithIocp();

	/**
	* @brief 发送接收数据事件
	* @return 是否发送接收数据事件成功,成功返回true,失败返回false
	*/
	bool PostRecv();

	/**
	* @brief 接收到了数据
	* @param dwBytes : 接收到的数据量
	*/
	void OnRecv(DWORD dwBytes);

	/**
	* @brief 发送了数据
	* @param dwBytes : 发送的数据量
	*/
	void OnSend(DWORD dwBytes);

	/**
	* @brief 当IOCP收到关闭连接时,调用此函数
	* @param bPassive : true为业务线程主动关闭,false为内部线程被动关闭
	*/
	void OnClose(bool bPassive = true); // close from IOCP Queue. 

	/**
	* @brief 真实关闭套接字
	* @return void
	*/
	void DoClose();

#ifdef UCODENET_HAS_GATHER_SEND 
	Condition             m_sendCond; 
	void    OnSendOut();
	volatile UINT32  m_hasCollectSent;	
#endif

	/**
	* @brief 增加发送消息计数一
	*/
	inline void IncPostSend()
	{
		InterlockedIncrement((LONG*)&m_dwPostSend);
	}

	/**
	* @brief 减少发送消息计数一
	*/
	inline void DecPostSend()
	{
		InterlockedDecrement((LONG*)&m_dwPostSend);
	}

	/**
	* @brief 获取目前还有的发送消息计数
	* @return 目前还有的发送消息计数
	*/
    UINT32 GetPostSend(void)
	{
		return InterlockedExchange((LONG*)&m_dwPostSend, m_dwPostSend);
	}

	/**
	* @brief 增加接收消息计数一
	*/
	inline void IncPostRecv()
	{
		InterlockedIncrement((LONG*)&m_dwPostRecv);
	}

	/**
	* @brief 减少接收消息计数一
	*/
	inline void DecPostRecv()
	{
		InterlockedDecrement((LONG*)&m_dwPostRecv);
	}

	/**
	* @brief 获取目前还有的接收消息计数
	* @return 目前还有的接收消息计数
	*/
	UINT32 GetPostRecv(void)
	{
		return InterlockedExchange((LONG*)&m_dwPostRecv, m_dwPostRecv);
	}

	//// cwy add 2009-03-23, interface expanding
	// returns the free send-buffer size
	/**
	* @brief 返回未使用的发送缓存
	*/
	inline UINT32 GetSendBufFree(void){ return m_dwSendBufSize - m_nDataSend; }

	/**
	* @brief 获取网络连接ID号
	*/
    inline UINT32 GetConnectionID(); 
	
	/**
	* @brief 获取发送缓存大小
	* @return 发送缓存大小
	*/
	inline UINT32  GetSendBufSize() { return m_dwSendBufSize; }

	/**
	* @brief 获取接收缓存大小
	* @return 接收缓存大小
	*/
	inline UINT32  GetRecvBufSize() { return m_dwRecvBufSize; }

    void SetSendStrategy(const SSendStrategy &stSendStrategy) {m_stSendStrategy = stSendStrategy;}
protected:
	/**
	* @brief 发送消息事件
	* @return 发送事件是否成功,true为成功,false为失败
	*/
	bool _PostSend();

	bool _PostClose(SOCKET hSock); 

	void _OnError(INT32 nUCODEError, INT32 nSysError);

protected:
	SOCKET				m_hSock;
	volatile BOOL		m_bConnect;
	volatile BOOL		m_bSendData;
	bool				m_bThreadSafe;
	//UINT32			m_dwDelayReleaseStamp;	//延迟释放时间戳
	char*				m_pSendBuf;
	char*				m_pRecvBuf;
	UINT32				m_dwRecvBufSize;
	UINT32				m_dwSendBufSize;
	INT32				m_nDataReceived;	//接收到的数据的长度
	INT32				m_nDataSend;		//待发送的数据的长度
	SPerHandleData		m_stPerHandleData;
	
	SPerIoData          m_stCloseIoData; 
	SPerIoData			m_stRecvIoData;
	SPerIoData			m_stSendIoData;	

	//static int user_close_count ;
	//static int on_close_count; 
 
	/*static*/ 
    CSDMutex			m_sendMutex;

	CConnData*			m_pConnData;
	//UINT32				m_dwConnectionID;	//m_poConnection所指向的对象可能被释放后又被重新分配，因此需要ID来检验是否发生过这种情况
	ISDPacketParser*	m_poPacketParser;
    //volatile UINT32     m_dwPostRecv;       //未决的WSARecv操作数
	//Modify by hexi.Add 'volatile' qualifer.2009-08-21
    volatile UINT32		m_dwPostSend;       //未决的WSASend操作数
	volatile UINT32		m_dwPostRecv;       //未决的WSARecv操作数

    SSendStrategy       m_stSendStrategy;
    //bool                m_bIsNeedForceClean;
};

#endif

