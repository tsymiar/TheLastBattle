#pragma once

#include "../../Share/Net/INetSessionMgr.h"
#include "ParsePacket.h"

class CSNetSessionMgr : public INetSessionMgr
{
public:
	CSNetSessionMgr();
	virtual ~CSNetSessionMgr();
public:
	virtual ISDSession* UCAPI CreateSession(ISDConnection* pConnection) { return NULL; /*ÖØÐ´*/}
	virtual ICliSession* UCAPI CreateConnectorSession(SESSION_TYPE type);
	virtual bool CreateConnector(SESSION_TYPE type, const char* ip, int port, int recvsize, int sendsize, int logicId);

private:
	CSParser m_CSParser;
};

class CGateSessionFactory : public ISDSessionFactory
{
public:
	virtual ISDSession* UCAPI CreateSession(ISDConnection* pConnection);
};

class CSceneSessionFactory : public ISDSessionFactory
{
public:
	virtual ISDSession* UCAPI CreateSession(ISDConnection* pConnection);
};

class CRemoteConsoleSessionFactory : public ISDSessionFactory
{
public:
	virtual ISDSession* UCAPI CreateSession(ISDConnection* pConnection);
};

extern CGateSessionFactory gGateSessionFactory;
extern CSceneSessionFactory gSceneSessionFactory;
extern CRemoteConsoleSessionFactory gRemoteConsoleFactory;
