#ifndef UCODELISTENER_H
#define UCODELISTENER_H

#include "ucnet.h"
#include "cplistener.h"

using namespace UCODE;

/**
* @brief 网络监听器,监听远程客户端对此联机
*/
class CUCODEListener : public ISDListener
{
public:
	CUCODEListener();
	virtual ~CUCODEListener();

	//Interface ISDListener
	/**
	* @brief 设置数据包解析器
	* @param poPacketParser : 设置的数据包解析器
	* @return void
	*/
	virtual void UCAPI SetPacketParser(ISDPacketParser* poPacketParser);

	/**
	* @brief 设置连接会话的工厂类
	* @param poSessionFactory : 连接会话的工厂类
	* @return void
	*/
	virtual void UCAPI SetSessionFactory(ISDSessionFactory* poSessionFactory);

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
	* @brief 开始监听
	* @param pszIP : 本地IP地址
	* @param wPort : 本地端口号
	* @param bReUseAddr : 是否端口复用
	* @return 监听成功返回true,监听失败返回false
	*/
	virtual bool UCAPI Start(const char* pszIP, UINT16 wPort, bool bReUseAddr);

	/**
	* @brief 停止监听
	* @return void
	*/
	virtual bool UCAPI Stop(void);

	/**
	* @brief 释放此监听器
	* @return void
	*/
	virtual void UCAPI Release(void);

protected:
	ISDPacketParser*	m_poPacketParser;
	ISDSessionFactory*	m_poSessionFactory;
	UINT32				m_dwRecvBufSize;
	UINT32				m_dwSendBufSize;
	CCpListener*		m_poCpListener;
	bool				m_bStart;
    BOOL                m_bNodelay;
};

#endif

