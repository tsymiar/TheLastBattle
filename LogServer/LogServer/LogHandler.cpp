#include "StdAfx.h"
#include "LogHandler.h"
#include "DBMgr.h"
#include "CSDataSource.h" 
#include "Net/LogNetSessionMgr.h"

CLogHandler* CLogHandler::m_pCLogHandler = nullptr;
CLogHandler::CLogHandler(void):m_pDBActiveWrapper(NULL)
{
}


CLogHandler::~CLogHandler(void)
{
}

CLogHandler& CLogHandler::GetInstance(){
	if (NULL == m_pCLogHandler){
		m_pCLogHandler = new CLogHandler;
	}
	return *m_pCLogHandler;
}

void CLogHandler::HandleLogMsg(const char* pMsg, int n32MsgLength){
	ELOG(LOG_DBBUG, "");
	ToLog::WriteLog sMsg;
	if (!ParseProtoMsg(pMsg, n32MsgLength, sMsg)){
		return;
	}

	m_pDBActiveWrapper->EncodeAndSendToDBThread(sMsg, sMsg.msgid());
}

void CLogHandler::AsynHanlder(Buffer*& pBuffer){
	if (pBuffer->m_LogLevel == ToLog::eMsgToLog_WriteLog){
		ToLog::WriteLog sMsg;
		if (!ParseProtoMsg(pBuffer->GetDataHeader(), pBuffer->GetDataLength(), sMsg)){
			return;
		}

		/*MysqlDWrapper::*/IDBConnector *pConn = m_pDBActiveWrapper->GetDBConnector();
		if (NULL == pConn){
			ELOG(LOG_ERROR, "");
			return;
		}
		INT32 nRet = pConn->ExecQuery(sMsg.log_str() );
		if (nRet == eNormal)
		{ 
			pConn->CloseQuery(); 
		}
	}
}

bool CLogHandler::Init(){

    char szPath[MAX_PATH];
    ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
    for (int i = strlen(szPath) - 1; i >= 0; --i)
    {
        if (szPath[i] == '\\')
        {
            szPath[i] = 0;
            break;
        }
    }

    std::string strExeDir = szPath;
    strExeDir += '\\';

	std::string szDBCfg = strExeDir + "./LGCfg/DBConfig.xml"; 
	TiXmlDocument doc(szDBCfg.c_str()); 
	if(doc.LoadFile())
	{
		TiXmlHandle dochandle(&doc);
		TiXmlElement *father =  dochandle.FirstChild("DBCfg").FirstChild("DB").ToElement();

		auto port =  atoi(dochandle.FirstChild("DBCfg").FirstChildElement("port").ToElement()->GetText());
		CLogNetSessionMgr* pSessionMgr = new CLogNetSessionMgr;
		INetSessionMgr::GetInstance()->CreateListener(port, 102400, 102400, 0, &gClientSessionFactory);

		SetDBData(father, m_DbCfg);

		m_pDBActiveWrapper = new DBActiveWrapper(std::bind(&CLogHandler::AsynHanlder, this, std::placeholders::_1), m_DbCfg);
		m_pDBActiveWrapper->Start();
		ELOG(LOG_INFO, "Init Success!");
	}
	else{
		ELOG(LOG_ERROR, "Init Failed!");
		return false;
	}

	return true;
}


void CLogHandler::SetDBData(TiXmlElement * father, SDBCfg &sDBCfg){
	TiXmlElement *ip = father->FirstChildElement("ip")->ToElement();
	strcpy(sDBCfg.aszDBHostIP , ip->GetText() );

	TiXmlElement *port = father->FirstChildElement("Port")->ToElement();
	sDBCfg.un32DBHostPort =  atoi(port->GetText());

	TiXmlElement *user = father->FirstChildElement("user")->ToElement();
	strcpy(sDBCfg.aszDBUserName , user->GetText());

	TiXmlElement *pwd = father->FirstChildElement("pwd")->ToElement();
	strcpy(sDBCfg.aszDBUserPwd , pwd->GetText());

	TiXmlElement *dbname = father->FirstChildElement("dbname")->ToElement();
	strcpy(sDBCfg.aszDBName , dbname->GetText());
}
