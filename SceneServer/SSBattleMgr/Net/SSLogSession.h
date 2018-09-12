#pragma once

#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ICliSession.h"

class CMsgHandle_SSLog : public IMsgHandle
{
public:
	CMsgHandle_SSLog();
	virtual ~CMsgHandle_SSLog() {}
};

class CSSLogSession : public ICliSession
{
public:
	CSSLogSession();
	virtual ~CSSLogSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_SSLog mHandles;
	
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
};
