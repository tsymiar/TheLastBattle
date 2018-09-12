/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:CSDataSource.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:WangYanYu
* complete date		:March 12 2014
* summary			:
*
*/

#pragma once
#include "DBMgr.h"
#include "Include/FBAll_Const.h"

class CCSDataSource
{
private:
	/*MysqlDWrapper::*/IDBConnector *piDBConnector; 
	SDBCfg*				m_pDBCfg;
public:
	CCSDataSource(SDBCfg& cfg);
	~CCSDataSource();

	INT32					Stop();
	/*MysqlDWrapper::*/IDBConnector*			GetDBConnector();
	INT32					Start();
	
private:
	INT32					Start(const SDBCfg& psDBCfg);
};

