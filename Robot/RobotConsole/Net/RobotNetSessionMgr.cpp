#include "stdafx.h"
#include "RobotNetSessionMgr.h"
#include "RobotSession.h"

CRobotNetSessionMgr::CRobotNetSessionMgr()
{

}

CRobotNetSessionMgr::~CRobotNetSessionMgr()
{

}

ISDSession* UCAPI CRobotNetSessionMgr::CreateSession( ISDConnection* pConnection )
{
	return NULL;
}

ICliSession* UCAPI CRobotNetSessionMgr::CreateConnectorSession( SESSION_TYPE type )
{
	ICliSession* pSession = NULL;
	pSession = new CRobotSession();
	pSession->SetType(type);
	pSession->SetMsgHandle(&CRobotSession::mHandles);
	pSession->SetCanReconnectTag(false);
	return pSession;
}
