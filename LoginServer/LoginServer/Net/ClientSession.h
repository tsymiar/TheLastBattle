#pragma once

#include "../../../Share/Net/IMsgHandle.h"
#include "../../../Share/Net/ISrvCliSession.h"

class MessageLite;

class CMsgHandle_Client : public IMsgHandle
{
public:
	CMsgHandle_Client();
	virtual ~CMsgHandle_Client() {}
};


class CClientSession : public ISrvCliSession
{
public:
	CClientSession();
	virtual ~CClientSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_Client mHandles;
	
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_User_Choice(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);

	bool mLogicInited;
};
