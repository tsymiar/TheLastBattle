#include "../stdafx.h"
#include "LSNetSessionMgr.h"
#include "BalanceSession.h"
#include "ClientSession.h"
#include "../SDKSession.h"

CBalanceSessionFactory gBalanceSessionFactory;
CClientSessionFactory gClientSessionFactory;

CLSNetSessionMgr::CLSNetSessionMgr()
{

}

CLSNetSessionMgr::~CLSNetSessionMgr()
{

}

ISDSession* UCAPI CBalanceSessionFactory::CreateSession( ISDConnection* pConnection )
{
	CBalanceSession* pSession = new CBalanceSession();
	pSession->SetType(ST_SERVER_LS_OnlyBS);
	pSession->SetMsgHandle(&CBalanceSession::mHandles);
	return pSession;
}

ISDSession* UCAPI CClientSessionFactory::CreateSession( ISDConnection* pConnection )
{
	CClientSession* pSession = new CClientSession();
	pSession->SetType(ST_SERVER_LS_OnlyGC);
	pSession->SetMsgHandle(&CClientSession::mHandles);
	return pSession;
}

ISDSession* UCAPI CSDKSessionFactory::CreateSession(ISDConnection* pConnection){
}
