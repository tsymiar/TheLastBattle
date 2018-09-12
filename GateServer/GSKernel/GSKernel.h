#pragma once
#include "stdafx.h"
#include "GSSSInfo.h"
#include <hash_map>
#include <string>
#include <boost/thread.hpp>
#include <functional>
#include <boost/thread/mutex.hpp>

namespace GateServer{

class CGSSSInfo;
class CGSKernel
{
typedef	INT32	(CGSKernel::* PFCSMsgHandler)(const char* pMsg, int n32MsgLength);
typedef	INT32	(CGSKernel::* PFSSMsgHandler)(CGSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength);
typedef INT32	(CGSKernel::* PFGCMsgHandler)(INT32 n32NSID, const char* pMsg, int n32MsgLength);

private:
	struct SUserToken
	{
		string UserName;
		string UserToken;
		TIME_TICK OverTime;
		UINT32 ReconnectCount;
		UINT32 NetSessionID;
	};

	struct SGSConfig
	{
		INT32	n32GSID;//网关分配号
		SIPAddr sCSIP;//中心服务器地址
		INT32	n32CSPort;//中心服务器端口
		CHAR	aszMyUserPwd[c_n32DefaultUserPwdLen];//很奇怪,该密码发给了中心服务器和场景服务器
		INT32	n32CSMaxMsgSize;//中心服务器最大消息长度
		INT32	n32WorkingThreadNum;//似乎没用这个
		SIPAddr	sGCListenIP;//网关监听地址
		INT32	n32GCListenPort;//网关监听端口
		INT32	n32GCMaxMsgSize;//网关最大消息长度
		INT32	n32MaxGCNum;//网关最大连接数
		SIPAddr	sBSListenIP;//负载监听地址
		INT32	n32BSListenPort;//负载监听端口	
		INT32	n32SkipBalance;//是否跳过BS认证
	};

	struct	SSSNetInfo
	{
		TIME_MILSEC	tConnMilsec;
		CGSSSInfo*	pcSSInfo;
		UINT64		un64MsgSent;
		UINT64		un64MsgRecved;
		UINT64		un64DataSent;
		UINT64		un64DataRecved;
	};

private:
	PFCSMsgHandler		m_asCSMsgHandler[100];//中心服务器消息
	PFSSMsgHandler		m_asSSMsgHandler[100];//场景服务器消息
	SGSConfig			m_sGSConfig;
	TIME_SECOND			m_tStartTime;
	INT32				m_n32HeartBeatTimes;
	TIME_TICK			m_tHeartBeatTicks;
	INT32				m_n32MaxSSNum;

public:
	TIME_SECOND			m_tCSTimeError;
	INT64				m_tLastPingCSTickCounter;
	typedef std::map<INT32, CGSSSInfo*> CGSSSInfoMap;
	INT32				m_CSNetSessionId;
	CGSSSInfoMap		m_CGSSSInfoMap;
	UINT32				m_un32SSBaseIdx;
	INT32				m_n32PostSSConnectNum;
	INT32				m_n32SuccessSSConnectNum;
	map<string,SUserToken*> m_UserTokenList;//token主表
	map<UINT32,SUserToken*> m_UserTokenListByNsId;//token快表
private:
	typedef map<INT32, CGSSSInfo*> User2SSInfoMap;
	User2SSInfoMap		m_User2SSInfoMap;

	CGSKernel();
public:
	virtual ~CGSKernel();
	static CGSKernel& GetStaticGSKernel();

	void				OnEvent(UINT32 event_type,PVOID params);
	INT32				Initialize();
	INT32				Uninitialize();
	INT32				Start();
	INT32				Stop();
	INT32				OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	/************************************************************************/
	/* interface functions from IGSModel                                    */
	/************************************************************************/
	INT32				OnMsgFromCS(const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID);
	INT32				OnMsgFromSS(CGSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID);
	INT32				OnMsgFromGC(INT32 n32NSID, const char* pMsg, int n32MsgLength, int n32MsgID);
	INT32				HandleMsgFromCS(const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID);
	INT32				HandleMsgFromSS(CGSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID);
	INT32				HandleMsgFromGC(INT32 n32NSID, const char* pMsg, int n32MsgLength, int n32MsgID);
	/************************************************************************/
	/* interface functions from IGSKernel                                   */
	/************************************************************************/
	INT32						PostToGameClient(INT32 n32NetSessioNID, const char* pRealMsg, int n32MsgLen, int n32MsgID);
	INT32						PostToGameClient(INT32 n32NetSessioNID, google::protobuf::Message& sMsg, int n32MsgID);
	INT32						BroadcastToGameClient(const char* pRealMsg, int n32MsgLen, int n32MsgID);
	INT32						PostGameClientDisconnect(INT32 n32NSID);
	INT32						TransToCS(const char* sMsgBuffer,int n32MsgLen,int n32MsgID,int n32TransID,int gcNetID);
	INT32						TransToSS(CGSSSInfo *piSSInfo,const char* sMsgBuffer,int n32MsgLen,int n32MsgID,int n32TransID,int gcNetID);
	INT32						GetMaxGCNum() {return m_sGSConfig.n32MaxGCNum;}
	INT32						GetMaxGCMsgSize() {return m_sGSConfig.n32GCMaxMsgSize;}
	INT32						GetGSID(){return m_sGSConfig.n32GSID;}
	const SGSConfig*			GetGSConfig(){return &m_sGSConfig;}
	CGSSSInfoMap&				GetCGSSSInfoMap(){return m_CGSSSInfoMap;}
public:
	INT32						GetMaxSSNum(){return  m_n32MaxSSNum;}
	void						SetMaxSSNum(INT32 n32Num){m_n32MaxSSNum = n32Num;}
	void						MainLoop();
private:
	INT32						LoadConfig();
	INT32						UnloadConfig();
	INT32						CheckCSConnect(TIME_MILSEC tTime);
	INT32						CheckSSConnect(TIME_MILSEC tCurTime);
	INT32						ReportGsInfo(TIME_TICK tUICMilsec);
	void						ProfileReport(TIME_TICK tUTCMilsec);
	INT32						AddSSInfo(INT32 n32SSID, const CHAR *cpszUserPwd, SIPAddr *cpsIPAddr, INT32 n32Port);
public:
	CGSSSInfo*					GetGSSSInfoBySSID(INT32 n32SSID);//根据场景服务器ID查找
	void						ClearAllUserToken();
	void						AddUserToken(const std::string& sUserName,const std::string sToken);
	INT32						ChechUserToken(TIME_TICK tUICMilsec);
	bool						IsUserCanLogin(const string& sToken,const std::string& sPwd,INT32 nsId);
	void						OnUserLost(INT32 nsId);
private:
	INT32						ResetHeartBeatChange(TIME_MILSEC tLastUTCMilsec, TIME_MILSEC tCurUTCMilsec);
	//////////////////////////////////////////////////////////////////////////
	INT32						OnMsgFromCS_AskPingRet(const char* pMsg, int n32MsgLength);
	INT32						OnMsgFromCS_OrderOpenListen(const char* pMsg, int n32MsgLength);
	INT32						OnMsgFromCS_OrderCloseListen(const char* pMsg, int n32MsgLength);
	INT32						OnMsgFromCS_OrderKickoutGC(const char* pMsg, int n32MsgLength);
	INT32						OnMsgFromCS_UserConnectedToSS(const char* pMsg, int n32MsgLength);
	INT32						OnMsgFromCS_UserDisConnectedToSS(const char* pMsg, int n32MsgLength);
	//////////////////////////////////////////////////////////////////////////
	INT32						OnMsgFromSS_AskPingRet(CGSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength);
	INT32						OnMsgFromSS_OrderKickoutGC(CGSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength);
};
}
