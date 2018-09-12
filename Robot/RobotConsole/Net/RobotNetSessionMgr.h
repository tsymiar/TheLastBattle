#pragma once

#include "../../Share/Net/INetSessionMgr.h"

class CRobotNetSessionMgr : public INetSessionMgr
{
public:
	CRobotNetSessionMgr();
	virtual ~CRobotNetSessionMgr();
public:
	virtual ISDSession* UCAPI CreateSession(ISDConnection* pConnection);
	virtual ICliSession* UCAPI CreateConnectorSession(SESSION_TYPE type);
};
