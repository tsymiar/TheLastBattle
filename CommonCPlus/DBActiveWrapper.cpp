#include "DBActiveWrapper.h"
#include "DBMgr.h"
#include "../ELogging/ELogging/SSActive.h"
#include "../ELogging/ELogging/Buffer.h"
#include "../ELoggingHeader.h"
#include "CSDataSource.h" 

DBActiveWrapper::DBActiveWrapper(Callback aCallBack, SDBCfg& cfg, BeginInThreadCallback startCallBack):m_pActive(Active::CreateActive(aCallBack, 128, 10240))
	, m_pDataSource(new CCSDataSource(cfg)), m_LogicStartCallBack(startCallBack){
		
		INT32 nret = m_pDataSource->Start();
		if (nret != 0)
		{
			ELOG(LOG_ERROR,"#####  i can not connect db,ret:%d,dbip:%s,dbName:%s###########",nret,cfg.aszDBHostIP,cfg.aszDBName);
		}
		else
		{
			ELOG(LOG_INFO,"##### connect db ok !############" );
		}
		m_pActive->SetThreadStartCallback(std::bind(&DBActiveWrapper::InitInThread, this));
}


bool DBActiveWrapper::EncodeProtoMsgToBuffer(google::protobuf::Message& sMsg, int n32MsgID, Buffer& pBuffer){
	string str = sMsg.SerializeAsString();
	pBuffer.append(str);
	pBuffer.m_LogLevel = n32MsgID;

	return true;
}

bool DBActiveWrapper::EncodeAndSendToDBThread(google::protobuf::Message& sMsg, int n32MsgID){
	Buffer* pBuffer = m_pActive->GetBuffer();
	bool res = EncodeProtoMsgToBuffer(sMsg, n32MsgID, *pBuffer);
	if (!res){
		m_pActive->ReleaseBuffer(pBuffer);
		return false;
	}
	pBuffer->m_ActorID = m_pActive->GetActorID();
	m_pActive->Send(pBuffer);
	return true;
}

void DBActiveWrapper::InitInThread(){
	if (m_LogicStartCallBack){
		m_LogicStartCallBack();
	}
}

int DBActiveWrapper::GetActorID(){
	if (m_pActive){
		return m_pActive->GetActorID();
	}

	return 0;
}

IDBConnector* DBActiveWrapper::GetDBConnector(){
	if (m_pDataSource){
		return m_pDataSource->GetDBConnector();
	}

	return NULL;
}

void DBActiveWrapper::Start(){
	m_pActive->Start();
}
