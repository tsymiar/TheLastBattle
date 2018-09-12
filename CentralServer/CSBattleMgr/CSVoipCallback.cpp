#include "stdafx.h"

#include "CSVoipCallback.h"
#include "CSLoggingHeader.h"
#ifdef _WINDOWS
namespace CentralServer{
CCSVoipCallback::CCSVoipCallback(const CHAR* server, INT32 port){
	clear();
	m_ic = Ice::initialize();
	m_voipConnStr = new char[100];
	_snprintf(m_voipConnStr, sizeof(m_voipConnStr), "ZXINGroupAgentServant:tcp -h %s -p %d -t 5000",server,port);
}

CCSVoipCallback::~CCSVoipCallback(){
	m_ic->shutdown();
	delete m_voipConnStr;
	m_voipConnStr = NULL;
}

void CCSVoipCallback::clear(){
	m_createRoomCallback = NULL;
}

void CCSVoipCallback::response(Ice::Int  ret,const ZXIN::GrpAgentPack& resp)
{
	if(resp.retCode == 0){
		if(resp.cmd == ZXIN::GRPCREATERESP){
			if(NULL != m_createRoomCallback){
				m_createRoomCallback->onCreateVoipRoom(resp.grpId);
			}
		}
	}
}

void CCSVoipCallback::exception(const Ice::Exception& ex)
{
	//cout << "AMI call failed:\n" << ex << endl;
}

typedef IceUtil::Handle<CCSVoipCallback> CCSVoipCallbackPtr;

BOOLEAN CCSVoipCallback::makeAsyncVoipRoom(){
	try
	{
		ZXIN::ZXINGroupAgentServantPrx prx = ZXIN::ZXINGroupAgentServantPrx::checkedCast(m_ic->stringToProxy(m_voipConnStr));
		if(!prx)
		{
			ELOG(LOG_DBBUG, "invalid proxy");
			return FALSE;
		}

		ZXIN::GrpAgentPack pack;
		pack.cmd = ZXIN::GRPCREATEREQ;
		prx->begin_DoZxinGroupAgentInner(pack,ZXIN::newCallback_ZXINGroupAgentServant_DoZxinGroupAgentInner(this,&CCSVoipCallback::response,&CCSVoipCallback::exception));
		return TRUE;
	}
	catch(std::exception& e)
	{
		//ELOG(LOG_DBBUG, (char*)e.what());
		return FALSE;
	}
}

void CCSVoipCallback::releaseVoipRoom(INT64 roomId){
	try
	{
		ZXIN::ZXINGroupAgentServantPrx prx = ZXIN::ZXINGroupAgentServantPrx::checkedCast(m_ic->stringToProxy(m_voipConnStr));
		if(!prx)
		{
			ELOG(LOG_DBBUG, "invalid proxy");
			return;
		}

		ZXIN::GrpAgentPack pack;
		pack.cmd = ZXIN::GRPREMOVEREQ;

		pack.grpId = roomId;
		prx->begin_DoZxinGroupAgentInner(pack,ZXIN::newCallback_ZXINGroupAgentServant_DoZxinGroupAgentInner(this,&CCSVoipCallback::response,&CCSVoipCallback::exception));
	}
	catch(std::exception& e)
	{
		//ELOG(LOG_DBBUG, (char*)e.what());
	}
}

void CCSVoipCallback::setVoipCallbackCallback(ICSVoipCallbackCallback* callback){
	m_createRoomCallback = callback;
}
}
#endif
