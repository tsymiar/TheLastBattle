#pragma once

#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ISrvCliSession.h"

class CMsgHandle_RemoteConsole : public IMsgHandle
{
public:
	CMsgHandle_RemoteConsole();
	virtual ~CMsgHandle_RemoteConsole() {}
};

class CRemoteConsoleSession : public ISrvCliSession
{
public:
	CRemoteConsoleSession();
	virtual ~CRemoteConsoleSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_RemoteConsole mHandles;
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
};
