#pragma once

#include "../../../Share/Net/IMsgHandle.h"
#include "../../../Share/Net/ICliSession.h"

class CMsgHandle_B2L : public IMsgHandle
{
public:
	CMsgHandle_B2L();
	virtual ~CMsgHandle_B2L() {}
};

class CB2LSession : public ICliSession
{
public:
	CB2LSession();
	virtual ~CB2LSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_B2L mHandles;
	
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_User_Login(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
};
