
#include "stdafx.h"

#include "LSAccountMgr.h"

CLSAccountMgr::CLSAccountMgr()
{
	gGlobalMap.clear();
	m_conn = NULL;
	
}
CLSAccountMgr::~CLSAccountMgr()
{
	gGlobalMap.clear();
	if(m_conn != NULL)
	{
		m_conn->Clear();
		m_conn = NULL;
	}
	memset(&m_dbCfg,0,sizeof(m_dbCfg));
}

void CLSAccountMgr::AddLoginAccount(std::string & cdkey, std::string param)
{
	auto itr = gGlobalMap.find(cdkey);
	if (itr == gGlobalMap.end())
	{
		gGlobalMap[cdkey] = param;
	}
	//exist
}


void CLSAccountMgr::ConnDb()
{ 
	 m_conn = IDBMgr::GetInstance().OpenDBConnector(eDBType_MYSQL_AccountDb, m_dbCfg.aszDBHostIP, m_dbCfg.un32DBHostPort, m_dbCfg.aszDBUserName, m_dbCfg.aszDBUserPwd, m_dbCfg.aszDBName );
	 if (m_conn == NULL)
	 {
		 ELOG(LOG_ERROR,"can not connect to db :%s",m_dbCfg.aszDBName);
	 }
}


void CLSAccountMgr::LoadCfg()
{
	memset(&m_dbCfg,0,sizeof(m_dbCfg));
}