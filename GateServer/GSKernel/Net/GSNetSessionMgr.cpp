

#include "stdafx.h"
#include "GSNetSessionMgr.h"
#include "ClientSession.h"
#include "M2CSession.h"
#include "M2SSession.h"
#include "M2BSession.h"

SProfileNet gProfileNet = {0};

GSNetSessionMgr::GSNetSessionMgr()
{

}

GSNetSessionMgr::~GSNetSessionMgr()
{

}

ISDSession* UCAPI GSNetSessionMgr::CreateSession( ISDConnection* pConnection )
{
	CClientSession* pSession = new CClientSession();
	pSession->SetType(ST_SERVER_GS);
	pSession->SetMsgHandle(&CClientSession::mHandles);
	return pSession;
}

ICliSession* UCAPI GSNetSessionMgr::CreateConnectorSession( SESSION_TYPE type )
{
	ICliSession* pSession = NULL;
	if (type==ST_CLIENT_G2C)
	{
		pSession = new CM2CSession();
		pSession->SetType(ST_CLIENT_G2C);
		pSession->SetMsgHandle(&CM2CSession::mHandles);
	}
	else if (type==ST_CLIENT_G2S)
	{
		pSession = new CM2SSession();
		pSession->SetType(ST_CLIENT_G2S);
		pSession->SetMsgHandle(&CM2SSession::mHandles);
	}
	else if (type==ST_CLIENT_G2B)
	{
		pSession = new CM2BSession();
		pSession->SetType(ST_CLIENT_G2B);
		pSession->SetMsgHandle(&CM2BSession::mHandles);
	}
	return pSession;
}
