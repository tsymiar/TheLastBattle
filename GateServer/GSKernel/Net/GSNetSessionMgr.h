#pragma once

#include "../../Share/Net/INetSessionMgr.h"
#include "ClientSession.h"

struct SProfileNet
{
	UINT32 mClientCount;
	UINT32 mTimeForDealWithMsg;//处理消息时间//
	UINT32 mTimeForDealWithOther;//其他网络时间//
	std::map<UINT32,UINT32> mMessageCount;//消息数量//
};

extern SProfileNet gProfileNet;

class GSNetSessionMgr : public INetSessionMgr
{
public:
	GSNetSessionMgr();
	virtual ~GSNetSessionMgr();
public:
	virtual ISDSession* UCAPI CreateSession(ISDConnection* pConnection);
	virtual ICliSession* UCAPI CreateConnectorSession(SESSION_TYPE type);
};
