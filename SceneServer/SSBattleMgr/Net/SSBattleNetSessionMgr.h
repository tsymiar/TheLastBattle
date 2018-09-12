#pragma once

#include "../../Share/Net/INetSessionMgr.h"

class SSBattleNetSessionMgr : public INetSessionMgr
{
public:
	SSBattleNetSessionMgr();
	virtual ~SSBattleNetSessionMgr();
public:
	virtual ISDSession* UCAPI CreateSession(ISDConnection* pConnection);
	virtual ICliSession* UCAPI CreateConnectorSession(SESSION_TYPE type);
};
