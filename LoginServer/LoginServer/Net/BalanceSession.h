#pragma once

#include "../../../Share/Net/IMsgHandle.h"
#include "../../../Share/Net/ISrvCliSession.h"
class MessageLite;

class CMsgHandle_Balance : public IMsgHandle
{
public:
	CMsgHandle_Balance();
	virtual ~CMsgHandle_Balance() {}
};

class CBalanceSession : public ISrvCliSession
{
public:
	CBalanceSession();
	virtual ~CBalanceSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_Balance mHandles;
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_OneClientLoginCheck(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
};
