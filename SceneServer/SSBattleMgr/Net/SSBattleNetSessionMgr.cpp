#include "../stdafx.h"
#include "SSBattleNetSessionMgr.h"
#include "GateSession.h"
#include "CentralSession.h"
#include "SSLogSession.h"

SSBattleNetSessionMgr::SSBattleNetSessionMgr()
{
	mIsUnSafeSend = true;
}

SSBattleNetSessionMgr::~SSBattleNetSessionMgr()
{

}

ISDSession* UCAPI SSBattleNetSessionMgr::CreateSession( ISDConnection* pConnection )
{
	CGateSession* pSession = new CGateSession();
	pSession->SetType(ST_SERVER_SS);
	pSession->SetMsgHandle(&CGateSession::mHandles);
	return pSession;
}

ICliSession* UCAPI SSBattleNetSessionMgr::CreateConnectorSession( SESSION_TYPE type )
{
	ICliSession* pSession = NULL;
	if (type==ST_CLIENT_S2C)
	{
		pSession = new CCentralSession();
		pSession->SetType(ST_CLIENT_S2C);
		pSession->SetMsgHandle(&CCentralSession::mHandles);
	}
	else if (type == ST_CLIENT_S2Log){
		pSession = new CSSLogSession();
		pSession->SetType(ST_CLIENT_S2Log);
		pSession->SetMsgHandle(&CSSLogSession::mHandles);
	}
	return pSession;
}
