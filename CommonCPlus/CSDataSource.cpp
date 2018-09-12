// CSDataSource.cpp : 定义 DLL 应用程序的导出函数。

#include <iostream>
#include <fstream>
#include "CSDataSource.h"

using namespace std;

CCSDataSource::CCSDataSource(SDBCfg& cfg):piDBConnector(NULL)
	, m_pDBCfg(new  SDBCfg)
{
	memcpy(m_pDBCfg, &cfg, sizeof(cfg));
}

CCSDataSource::~CCSDataSource()
{
	
}

INT32	CCSDataSource::Start(const SDBCfg& rsDBCfg)
{
	piDBConnector = new IDBConnector();
	return piDBConnector->ConnDb(rsDBCfg.aszDBHostIP, rsDBCfg.aszDBUserName, rsDBCfg.aszDBUserPwd, rsDBCfg.aszDBName, rsDBCfg.un32DBHostPort) ? 0 : -1;
}

INT32 CCSDataSource::Start(){
	INT32  nret = Start(*m_pDBCfg);
	 
	 
	delete m_pDBCfg;
	m_pDBCfg = nullptr;

	return nret;
}


INT32	CCSDataSource::Stop()
{
	return eNormal;
}

/*MysqlDWrapper::*/IDBConnector*	CCSDataSource::GetDBConnector()
{
	return piDBConnector;
}


