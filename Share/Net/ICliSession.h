#pragma once

#include "INetSession.h"

class ICliSession : public INetSession
{
public:
	ICliSession();
	virtual ~ICliSession();
public:
	bool Reconnect(); 
	bool Reconnect(const char* ip, UINT32 port);
	void SetConnector(ISDConnector* connector) { m_poConnector=connector; }
	void SetRemoteEndPointer(const char* ip,UINT32 port);
	virtual void Update();
	void SetCanReconnectTag(bool can){mReconnectTag = can;}
	bool GetCanReconnectTag(){return mReconnectTag;}
public:
	virtual void UCAPI OnEstablish(void);
	virtual void UCAPI Release(void) {/*触发重连,不释放该对象*/}
private:
	ISDConnector* m_poConnector;
	UINT32 mReconTime;
	string mRemoteEndPointer;
	bool mReconnectTag;
};
