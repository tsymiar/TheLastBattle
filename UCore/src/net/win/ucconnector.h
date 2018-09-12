#ifndef UCODECONNECTOR_H
#define UCODECONNECTOR_H

#include "ucnet.h"
#include <map>

using namespace UCODE;

/**
* @brief 网络连接器,用于本地连接远程服务器
*/
class CUCODEConnector : public ISDConnector
{
	enum EConnStat
	{
		CONN_NONE = 0,
		CONN_PROC,
		CONN_OK
	};

public:
	CUCODEConnector();
	virtual ~CUCODEConnector();

	/**
	* @brief 设置数据包解析器
	* @param poPakcetParser : 设置的数据包解析器
	* @return void
	*/
	virtual void UCAPI SetPacketParser(ISDPacketParser* poPakcetParser);

	/**
	* @brief 设置连接会话
	* @param poSession : 设置的连接会话
	* @return void
	*/
	virtual void UCAPI SetSession(ISDSession* poSession);

	/**
	* @brief 设置缓存大小
	* @param dwRecvBufSize : 设置接收缓存大小
	* @param dwSendBufSize : 设置发送缓存大小
	* @return void
	*/
	virtual void UCAPI SetBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize);

	/**
	* @brief 设置此连接器参数
	* @param dwType : 设置此连接器的类型
	* @param pOpt : 设置此连接器的值
	* @return void
	*/
	virtual void UCAPI SetOpt(UINT32 dwType, void* pOpt);

	/**
	* @brief 使用此连接器连接远程服务器
	* @param pszIP : 设置远程服务器IP地址
	* @param wPort : 设置远程服务器的端口号
	* @return 是否连接成功,true为成功,false为失败
	*/
	virtual bool UCAPI Connect(const char* pszIP, UINT16 wPort);

	/**
	* @brief 使用此连接器重新连接远程服务器
	* @return 是否重新连接成功,true为成功,false为失败
	* @remark 重新连接的远程服务器的IP和端口为m_dwIP和m_wPort
	*/
	virtual bool UCAPI ReConnect(void);

	/**
	* @brief 释放此连接器
	* @return void
	*/
	virtual void UCAPI Release(void);

	/**
	* @brief 设置此连接器的ID号
	* @param dwID : 设置连接器的ID号
	*/
	void SetID(UINT32 dwID) { m_dwID = dwID; }

	/**
	* @brief 获取此连接器的ID号
	* @return 获取的ID号
	*/
	UINT32 GetID() { return m_dwID; }

	//// 2009-03-24 cwy add for interface expanding, add bind function
	/**
	* @brief 当绑定本地IP地址失败时,调用此函数
	* @param nUCODEError : UCODE Error
	* @param nSysError : Sys Error
	* @return void
	*/
	void OnBindErr(INT32 nUCODEError, INT32 nSysError);

	/**
	* @brief 当使用此连接器连接成功时,调用此函数
	* @return void
	*/
	void OnConnect();

	/**
	* @brief 当连接错误时,调用此函数
	* @param nSysError : Sys Error
	* @return void
	*/
	void OnConnectErr(INT32 nSysError);

	/**
	* @brief 当连接关闭时,调用此函数
	* @return void
	*/
	void OnClose();

protected:
	UINT32				m_dwID;
	ISDPacketParser*	m_poPacketParser;
	ISDSession*			m_poSession;
	UINT32				m_dwRecvBufSize;
	UINT32				m_dwSendBufSize;
	UINT32				m_dwIP;
	UINT16				m_wPort;
	EConnStat			m_nConnStat;
	bool					m_bNeedBind;
	const char*		m_pszBindIP;
	UINT16				m_wBindPort;
    BOOL                m_bNodelay;
};

typedef std::map<UINT32, CUCODEConnector*> CMapConnector;

#endif

