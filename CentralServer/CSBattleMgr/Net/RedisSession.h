#pragma once

#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ISrvCliSession.h"
#include "../hiredis.h"
#include "../async.h"
class CMsgHandle_Redis : public IMsgHandle
{
public:
	CMsgHandle_Redis();
	virtual ~CMsgHandle_Redis() {}
};

class CRedisSession : public ICliSession
{
public:
	CRedisSession();
	virtual ~CRedisSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_Redis mHandles;
	
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	virtual void UCAPI OnRecv(const char* pBuf, UINT32 dwLen);
	void   HandleClosed(int status);
};
