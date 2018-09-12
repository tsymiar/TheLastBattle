#include "../stdafx.h"
#include "LogicRedisSession.h"

#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\CSKernel.h"
#include "..\CSUserMgr.h"
using namespace CentralServer;
CLogicRedisSession::CLogicRedisSession()
{
}

void CLogicRedisSession::SendInitData()
{
	ELOG(LOG_INFO, "连接上Redis");
	redisAsyncContextWrapper* predisAsyncContextWrapper = GetLogicRedisHandler();
	if (!predisAsyncContextWrapper){
		predisAsyncContextWrapper = new redisAsyncContextWrapper();
		predisAsyncContextWrapper->Init(std::bind(&INetSession::Send, this, std::placeholders::_1, std::placeholders::_2)
			, std::bind(&CLogicRedisSession::HandleClosed, this, std::placeholders::_1));

		SetLogicCacheRedisHandler(predisAsyncContextWrapper);
	}

	predisAsyncContextWrapper->SetConnected(true);
	GetCSUserMgrInstance()->InitCDKIDCFromRedis();
	GetCSUserMgrInstance()->InitRedisSucscribe();
}

void CLogicRedisSession::OnRealEstablish(){
	ELOG(LOG_INFO, "连接上Redis");

	redisAsyncContextWrapper* predisAsyncContextWrapper = GetLogicRedisHandler();
	if (!predisAsyncContextWrapper){
		predisAsyncContextWrapper = new redisAsyncContextWrapper();
		predisAsyncContextWrapper->Init(std::bind(&INetSession::Send, this, std::placeholders::_1, std::placeholders::_2)
			, std::bind(&CLogicRedisSession::HandleClosed, this, std::placeholders::_1));

		SetLogicCacheRedisHandler(predisAsyncContextWrapper);
	}

	predisAsyncContextWrapper->SetConnected(true);
}

void CLogicRedisSession::OnClose()
{
	if (GetLogicRedisHandler()){
		GetLogicRedisHandler()->SetConnected(false);
	}
	ELOG(LOG_WARNNING, "与Redis断开连接!");
}

void UCAPI CLogicRedisSession::OnRecv(const char* pBuf, UINT32 dwLen){
	auto pReids = GetLogicRedisHandler();
	if (pReids){
		int res = pReids->redisAsyncReaderFeed(pBuf, dwLen);
		if (res != REDIS_OK){
			ELOG(LOG_ERROR, "");
			return;
		}

		pReids->redisProcessCallbacks();
	}
}

void CLogicRedisSession::HandleClosed(int status){
	if (GetLogicRedisHandler()){
		GetLogicRedisHandler()->SetConnected(false);
	}

	ELOG(LOG_ERROR, "Close for %d", status);
	INetSession::Close();
}
