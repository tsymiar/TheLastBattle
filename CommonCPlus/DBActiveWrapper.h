#pragma once
#include "../ELogging/ELogging/SSActive.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "CSDataSource.h"

class Active;
class Buffer;
//using namespace MysqlDWrapper;

class DBActiveWrapper{
public:
	DBActiveWrapper(Callback aCallBack, SDBCfg& cfg, BeginInThreadCallback startCallBack = nullptr);
	static bool				EncodeProtoMsgToBuffer(google::protobuf::Message& sMsg, int n32MsgID, Buffer& pBuffer);
	bool					EncodeAndSendToDBThread(google::protobuf::Message& sMsg, int n32MsgID);
	int						GetPendingWorkNum()const{return m_pActive->GetPendingWorkNum();}
	 IDBConnector*			GetDBConnector();
	int						GetActorID();
	void					Start();
private:
	void					InitInThread();

	Active*						m_pActive;
	CCSDataSource*				m_pDataSource;
	BeginInThreadCallback		m_LogicStartCallBack;
};
