#include "../stdafx.h"
#include "CSNetSessionMgr.h"
#include "GateSession.h"
#include "SceneSession.h"
#include "RemoteConsoleSession.h"
#include "CSLogSession.h"
#include "RedisSession.h"
#include "LogicRedisSession.h"

CGateSessionFactory gGateSessionFactory;
CSceneSessionFactory gSceneSessionFactory;
CRemoteConsoleSessionFactory gRemoteConsoleFactory;

CSNetSessionMgr::CSNetSessionMgr()
{

}

CSNetSessionMgr::~CSNetSessionMgr()
{

}

ICliSession* UCAPI CSNetSessionMgr::CreateConnectorSession(SESSION_TYPE type){
	ICliSession* pSession = NULL;
	if (type==ST_CLIENT_C2R)
	{
		pSession = new CRedisSession();
		pSession->SetType(ST_CLIENT_C2R);
		pSession->SetMsgHandle(&CRedisSession::mHandles);
	}
	else if (type==ST_CLIENT_C2LogicRedis){
		pSession = new CLogicRedisSession();
		pSession->SetType(ST_CLIENT_C2LogicRedis);
	}
	else if (type == ST_CLIENT_C2Log){
		pSession = new CCSLogSession();
		pSession->SetType(ST_CLIENT_C2Log);
		pSession->SetMsgHandle(&CCSLogSession::mHandles);
	}
	return pSession;
}

bool CSNetSessionMgr::CreateConnector(SESSION_TYPE type, const char* ip, int port, int recvsize, int sendsize, int logicId)
{
	ISDConnector* pConnector = mNetModule->CreateConnector(NETIO_COMPLETIONPORT);
	ICliSession* pSession = CreateConnectorSession(type);
	UINT32 sId = AddSession(pSession);
	Assert(sId!=PP_INVALID);
	pSession->SetID(sId);
	pSession->SetLogicID(logicId);
	pSession->SetConnector(pConnector);
	pSession->SetRemoteEndPointer(ip,port);
	pConnector->SetSession(pSession);
	if (type == ST_CLIENT_C2R || type == ST_CLIENT_C2LogicRedis) 
		pConnector->SetPacketParser(&m_CSParser);
	else 
		pConnector->SetPacketParser(this);
	pConnector->SetBufferSize(recvsize,sendsize);
	return pConnector->Connect(ip,port);
}

ISDSession* UCAPI CGateSessionFactory::CreateSession( ISDConnection* pConnection )
{
	CGateSession* pSession = new CGateSession();
	pSession->SetType(ST_SERVER_CS_OnlyGS);
	pSession->SetMsgHandle(&CGateSession::mHandles);
	return pSession;
}

ISDSession* UCAPI CSceneSessionFactory::CreateSession( ISDConnection* pConnection )
{
	CSceneSession* pSession = new CSceneSession();
	pSession->SetType(ST_SERVER_CS_OnlySS);
	pSession->SetMsgHandle(&CSceneSession::mHandles);
	return pSession;
}

ISDSession* UCAPI CRemoteConsoleSessionFactory::CreateSession( ISDConnection* pConnection )
{
	CRemoteConsoleSession* pSession = new CRemoteConsoleSession();
	pSession->SetType(ST_SERVER_CS_OnlyRC);
	pSession->SetMsgHandle(&CRemoteConsoleSession::mHandles);
	return pSession;
}