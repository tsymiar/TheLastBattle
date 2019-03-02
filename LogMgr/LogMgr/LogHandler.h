#pragma once
#include "stdafx.h"
#include "DBActiveWrapper.h"
#include "tinyxml.h" 

class CLogHandler
{
private:
	CLogHandler(void);
	static CLogHandler* m_pCLogHandler;
public:
	static CLogHandler&	GetInstance();
	~CLogHandler(void);
	bool	Init();
	void	HandleLogMsg(const char* pMsg, int n32MsgLength);

private:
	void	AsynHanlder(Buffer*& pBuffer);
	void	SetDBData(TiXmlElement * father, SDBCfg &sDBCfg);
private:
	DBActiveWrapper* m_pDBActiveWrapper;
	SDBCfg			m_DbCfg;
};

