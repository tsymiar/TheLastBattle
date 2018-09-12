#include "../stdafx.h"
#include "RedisSession.h"

#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\CSKernel.h"

CMsgHandle_Redis CRedisSession::mHandles;
CMsgHandle_Redis::CMsgHandle_Redis()
	:IMsgHandle(GSToCS::eMsgToCSFromGS_Begin, GSToCS::eMsgToCSFromGS_End-GSToCS::eMsgToCSFromGS_Begin){
}

CRedisSession::CRedisSession()
{
}

CRedisSession::~CRedisSession()
{

}

void CRedisSession::SendInitData()
{
	ELOG(LOG_INFO, "连接上Redis");
	redisAsyncContextWrapper* predisAsyncContextWrapper = GetUserDBCacheRedisHandler();
	if (!predisAsyncContextWrapper){
		predisAsyncContextWrapper = new redisAsyncContextWrapper();
		predisAsyncContextWrapper->Init(std::bind(&INetSession::Send, this, std::placeholders::_1, std::placeholders::_2)
			, std::bind(&CRedisSession::HandleClosed, this, std::placeholders::_1));

		SetUserDBCacheRedisHandler(predisAsyncContextWrapper);
	}

	predisAsyncContextWrapper->SetConnected(true);
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	//发送消息//
}

void CRedisSession::OnRealEstablish(){
	ELOG(LOG_INFO, "连接上Redis");

	redisAsyncContextWrapper* predisAsyncContextWrapper = GetUserDBCacheRedisHandler();
	if (!predisAsyncContextWrapper){
		predisAsyncContextWrapper = new redisAsyncContextWrapper();
		predisAsyncContextWrapper->Init(std::bind(&INetSession::Send, this, std::placeholders::_1, std::placeholders::_2)
			, std::bind(&CRedisSession::HandleClosed, this, std::placeholders::_1));

		SetUserDBCacheRedisHandler(predisAsyncContextWrapper);
	}

	predisAsyncContextWrapper->SetConnected(true);
}

void CRedisSession::OnClose()
{
	if (GetUserDBCacheRedisHandler()){
		GetUserDBCacheRedisHandler()->SetConnected(false);
	}
	ELOG(LOG_WARNNING, "与Redis断开连接!");
}

bool CRedisSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	//解码消息//
	ELOG(LOG_INFO, "");
	vthis->SetInited(true,true);
	return true;
}

bool CRedisSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	ELOG(LOG_INFO, "");
	return true;
}


void UCAPI CRedisSession::OnRecv(const char* pBuf, UINT32 dwLen){
	if (GetUserDBCacheRedisHandler()){
		int res = GetUserDBCacheRedisHandler()->redisAsyncReaderFeed(pBuf, dwLen);
		if (res != REDIS_OK){
			ELOG(LOG_ERROR, "");
			return;
		}

		GetUserDBCacheRedisHandler()->redisProcessCallbacks();
	}
}

void CRedisSession::HandleClosed(int status){
	if (GetUserDBCacheRedisHandler()){
		GetUserDBCacheRedisHandler()->SetConnected(false);
	}

	ELOG(LOG_ERROR, "Close for %d", status);
	INetSession::Close();
}
