#pragma once
#include "../ELogging/ELogging/ThreadSafeQueue.h"
#include <boost/thread.hpp>
#include "curl/curl.h"
#include "SDKAsynHandler.h"
#include "stdafx.h"
class Active;
struct Buffer;
/* Global information, common to all connections */
struct GlobalInfo
{
	CURLM *multi;
	int still_running;
	GlobalInfo():multi(NULL), still_running(0){}
};

/* Information associated with a specific easy handle */
struct ConnInfo
{
	CURL *easy;
	GlobalInfo *global;
	char error[CURL_ERROR_SIZE];
	int msgid;
	int gcnetid;
	int sessionid;
	int platformid;
	bool retflag;
	string resData;
	ConnInfo():easy(NULL)
	, global(NULL)
	, msgid(0)
	, gcnetid(0)
	, sessionid(0)
	, platformid(0)
	, retflag(false){
		resData = "";
		memset(error, 0, CURL_ERROR_SIZE);
	}
	~ConnInfo(){
	}
};
class SDKAsynHandler
{
private:
	SDKAsynHandler(void);
public:
	~SDKAsynHandler(void);
	static SDKAsynHandler&	GetInstance();
	int				CheckLogin(GCToLS::AskLogin& sAskLogin, int gcnetid);
	void			Init();
	void			AsynHandleLoiginCheckMsg(ConnInfo* pConnInfo);

// 	void				AsynHandleLoiginCheckMsg_91(ConnInfo* pConnInfo);
// 	void				AsynHandleLoiginCheckMsg_TB(ConnInfo* pConnInfo);
// 	void				AsynHandleLoiginCheckMsg_PP(ConnInfo* pConnInfo);
// 	void				AsynHandleLoiginCheckMsg_Cmge(ConnInfo* pConnInfo);
// 	void				AsynHandleLoiginCheckMsg_Cmge_Info(ConnInfo* pConnInfo);
 	void				AsynHandleLoiginCheckMsg_PC(int gcnetid);
// 	void				AsynHandleLoiginCheckMsg_UC(ConnInfo* pConnInfo);
// 	void				AsynHandleLoiginCheckMsg_iTools(ConnInfo* pConnInfo);
// 	void				AsynHandleLoiginCheckMsg_OnlineGame(ConnInfo* pConnInfo);
// 	void				AsynHandleLoiginCheckMsg_As(ConnInfo* pConnInfo);
	void				AsynHandleLoiginCheckMsg_XY(ConnInfo* pConnInfo);

	void				PostToLoginFailQueue(EFBAllErrorCode ErrorCode, int gcnetid);

private:
	int				AddUserToLoginMap(ConnInfo* pConnInfo, EUserPlatform eplat, string str_uin);

	void			PostMsg(const char* pMsg, int length, int msgid, int gcnetid, EUserPlatform eplat);
	void			PostToFailQueue(int gcnetid, int errorcode);

	void			InitThread();
	void			InitCfg();
	
public:
	Concurrency::concurrent_queue<Buffer*>		m_SDKCallbackQueue;
	CThreadSafeObejctPool<Buffer>				m_SDKCallbackQueuePool;
	typedef map<int, UserLoginData>		UserLoginDataMap;
	UserLoginDataMap	m_UserLoginDataMap;
	boost::mutex		m_UserLoginDataMapMutex;
};

