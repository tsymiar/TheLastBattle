#include "LogNetSessionMgr.h"
#include "ClientSession.h"

CClientSessionFactory gClientSessionFactory;

CLogNetSessionMgr::CLogNetSessionMgr()
{

}

CLogNetSessionMgr::~CLogNetSessionMgr()
{

}

ISDSession* UCAPI CClientSessionFactory::CreateSession( ISDConnection* pConnection )
{
	CClientSession* pSession = new CClientSession();
	pSession->SetType(ST_SERVER_Log);
	pSession->SetMsgHandle(&CClientSession::mHandles);
	return pSession;
}

