#include "../stdafx.h"
#include "BSNetSessionMgr.h"
#include "B2LSession.h"
#include "GateSession.h"
#include "ClientSession.h"

CGateSessionFactory gGateSessionFactory;
CClientSessionFactory gClientSessionFactory;

CBSNetSessionMgr::CBSNetSessionMgr()
{

}

CBSNetSessionMgr::~CBSNetSessionMgr()
{

}

ICliSession* UCAPI CBSNetSessionMgr::CreateConnectorSession(SESSION_TYPE type)
{
	ICliSession* pSession = NULL;
	if (type==ST_CLIENT_B2L)
	{
		pSession = new CB2LSession();
		pSession->SetType(ST_CLIENT_B2L);
		pSession->SetMsgHandle(&CB2LSession::mHandles);
	}
	return pSession;
}

ISDSession* UCAPI CGateSessionFactory::CreateSession( ISDConnection* pConnection )
{
	CGateSession* pSession = new CGateSession();
	pSession->SetType(ST_SERVER_BS_OnlyGS);
	pSession->SetMsgHandle(&CGateSession::mHandles);
	return pSession;
}

ISDSession* UCAPI CClientSessionFactory::CreateSession( ISDConnection* pConnection )
{
	CClientSession* pSession = new CClientSession();
	pSession->SetType(ST_SERVER_BS_OnlyGC);
	pSession->SetMsgHandle(&CClientSession::mHandles);
	return pSession;
}