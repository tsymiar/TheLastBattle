#pragma once
#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ISrvCliSession.h"

class CSDKMsgHandle_Balance : public IMsgHandle
{
public:
	CSDKMsgHandle_Balance();
	virtual ~CSDKMsgHandle_Balance() {}
};


class SDKSession : public ISrvCliSession
{
public:
	SDKSession(void);
	~SDKSession(void);
	virtual void OnClose();
};

