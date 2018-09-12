#ifndef UCODECONNECTION_H
#define UCODECONNECTION_H

#include "ucnetconfig.h"
#include "ucnet.h"


using namespace UCODE;

struct CConnData;

#define	 NET_SEND_BUFFER_SIZE		1024

struct SNetSendBuffer
{
	SNetSendBuffer():dwLen(0){}
	char    cBuffer[NET_SEND_BUFFER_SIZE];
	UINT32  dwLen;
};

/**
* @brief 代表一个网络连接的类
*/
class CUCConnection : public ISDConnection
{
	enum EConnStat
	{
		CONN_NONE = 0,
		CONN_OK,
		CONN_ASSOCIATE,
		CONN_CLOSING,
	};
public:
	CUCConnection();
	virtual ~CUCConnection();

	/**
	* @brief 此连接是否为连接的
	* @return 返回true代表连接的,返回false代表断开的
	*/
	virtual bool UCAPI IsConnected(void);

	/**
	* @brief 使用此连接发送数据
	* @param pBuf : 指向发送的数据的指针
	* @param dwLen : 发送数据的长度
	*/
	virtual void UCAPI Send(const char* pBuf, UINT32 dwLen);

	/**
	* @brief 设置此连接的参数
	* @param dwType : 参数类型
	* @param pOpt : 参数数据
	* @return void
	*/
	virtual void UCAPI SetOpt(UINT32 dwType, void* pOpt);

	/**
	* @brief 断开连接
	* @return void
	*/
	virtual void UCAPI Disconnect(void);

	/**
	* @brief 获取此连接的远程IP地址的数字型
	* @return 此连接的远程IP地址的数字型
	*/
	virtual const UINT32 UCAPI GetRemoteIP(void)	{ return m_dwRemoteIP;	}

	/**
	* @brief 获取此连接的远程IP地址的字符串类型
	*/
	virtual const char* UCAPI GetRemoteIPStr(void)	{ return inet_ntoa( (in_addr&)m_dwRemoteIP );	}

	/**
	* @brief 获取此连接的远程端口号
	* @return 此连接的远程端口号
	*/
	virtual UINT16  UCAPI GetRemotePort(void)		{ return m_wRemotePort; }

	/**
	* @brief 获取此连接的本地IP地址的数字型
	* @return 此连接的本地IP地址的数字型
	*/
	virtual const UINT32 UCAPI GetLocalIP(void)		{ return m_dwLocalIP;	}

	/**
	* @brief 获取此连接的本地IP地址的字符串型
	* @return 此连接的本地IP地址的字符串型
	*/
	virtual const char* UCAPI GetLocalIPStr(void)	{ return inet_ntoa( (in_addr&)m_dwLocalIP );	}

	/**
	* @brief 获取此连接的本地端口号
	* @return 此连接的本地端口号
	*/
	virtual UINT16 UCAPI GetLocalPort(void)			{ return m_wLocalPort;	}

	/**
	* @brief 返回未使用的发送缓存
	*/
	virtual UINT32 UCAPI GetSendBufFree(void);

	/**
	* @brief 重置此连接,清理此连接的信息
	*/
	void Reset();

	/**
	* @brief 设置此连接的ID号
	*/
	inline void SetID(UINT32 dwID) { m_dwID = dwID; }

	/**
	* @brief 获取此连接的ID号
	*/
	UINT32 GetID() { return m_dwID; }

	/**
	* @brief 设置此连接是否为主动连接,抑或被动连接
	* @param bAccept : false为主动连接,true为被动连接
	* @return void
	*/
	inline void SetAccept(bool bAccept)	{ m_bAccept = bAccept;	}

	/**
	* @brief 查看此连接是否为被动连接
	* @return true为被动连接,false为主动连接
	*/
	bool IsAccept()					{ return m_bAccept;		}

	inline void SetParentID(UINT32 dwParentID)	{ m_dwParentID = dwParentID;	}
	UINT32 GetParentID()				{ return m_dwParentID;			}

	/**
	* @brief 设置此连接的远程IP地址
	* @param dwIP : 数字型的远程IP地址
	* @return void
	*/
	void SetRemoteIP(UINT32 dwIP);

	/**
	* @brief 设置此连接的远程端口号
	* @param dwIP : 远程端口号
	* @return void
	*/
	inline void SetRemotePort(UINT16 wPort)	{ m_wRemotePort = wPort;	}

	/**
	* @brief 设置此连接的本地IP地址
	* @param dwIP : 本地IP地址
	* @return void
	*/
	void SetLocalIP(UINT32 dwIP);

	/**
	* @brief 设置此连接的本地端口号
	* @param wPort : 本地端口号
	* @return void
	*/
	inline void SetLocalPort(UINT16 wPort)		{ m_wLocalPort = wPort;		}

	/**
	* @brief 设置此连接的连接数据
	* @param pConnData : 连接数据
	* @return void
	*/
	inline void SetConnData(CConnData* pConnData)		{ m_pConnData = pConnData; }

	/**
	* @brief 设置此连接的会话
	* @param poSession : 连接会话
	*/
	inline void SetSession(ISDSession* poSession) { m_poSession = poSession; }

	/**
	* @brief 当此连接连接成功时,调用此函数
	*/
	void OnConnect();

	/**
	* @brief 当此连接发送数据时,调用此函数
	* @param pBuf : 指向发送的数据的指针
	* @param dwLen : 发送数据的长度
	*/
	void OnSend(const char * pBuf, UINT32 dwLen);

	/**
	* @brief 当此连接连接成功时,调用此类
	*/
	void OnAssociate();

	/**
	* @brief 当此连接关闭时,调用此函数
	*/
	void OnClose();

	/**
	* @brief 当此连接接收到消息时,调用此函数
	*/
	inline void OnRecv(const char* pData, INT32 nLen)
	{
		if(m_nConnStat != CONN_ASSOCIATE)
		{
			return;
		}

		SDASSERT(m_poSession != NULL);
		m_poSession->OnRecv(pData, nLen);
	}

	/**
	* @brief 当此连接发生错误时,调用此函数
	*/
	inline void OnError(INT32 nUCODEError, INT32 nSysError)
	{
		if(m_nConnStat != CONN_ASSOCIATE)
		{
			return;
		}

		SDASSERT(m_poSession != NULL);
		m_poSession->OnError(nUCODEError, nSysError);
	}

	/**
	* @brief 当此连接发送数据时,调用此函数
	* @param pBuf : 指向发送的数据的指针
	* @param dwLen : 发送数据的长度
	*/
	void UpdateSend();

	inline	UINT8	GetRunIndex(){return m_byRunIndex;}
protected:
	UINT32		m_dwID;
	UINT32		m_dwParentID;
	bool		m_bAccept;
	UINT8		m_byRunIndex;
	EConnStat	m_nConnStat;
	UINT32		m_dwLocalIP;
	UINT32		m_dwRemoteIP;
	UINT16		m_wLocalPort;
	UINT16		m_wRemotePort;
	//char		m_szLocalIP[16];
	//char		m_szRemoteIP[16]; //decrease the memory footprint
	CConnData *	m_pConnData;
	ISDSession* m_poSession;
};

#endif

