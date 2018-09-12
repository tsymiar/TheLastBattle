#pragma once

#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ISrvCliSession.h"

class CMsgHandle_Scene : public IMsgHandle
{
public:
	CMsgHandle_Scene();
	virtual ~CMsgHandle_Scene() {}
};

class CSceneSession : public ISrvCliSession
{
public:
	CSceneSession();
	virtual ~CSceneSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_Scene mHandles;
	
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
};
