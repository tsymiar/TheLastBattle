#pragma once

#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ICliSession.h"

class CMsgHandle_Central : public IMsgHandle
{
public:
	CMsgHandle_Central();
	virtual ~CMsgHandle_Central() {}
};

class CCentralSession : public ICliSession
{
public:
	CCentralSession();
	virtual ~CCentralSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_Central mHandles;
	
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
};
