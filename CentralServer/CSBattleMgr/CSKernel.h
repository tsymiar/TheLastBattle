/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:CSKernel.h
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
#include "stdafx.h"
#include "CSGSInfo.h"
#include "CSSSInfo.h"
#include "CSRCInfo.h"
#include "tinyxml.h"
#include "Timer.h"

namespace CentralServer{
class CCSKernel
{
private:
	struct SCSKernelCfg
	{
		UINT32	unCSId;
		INT32	n32MaxMsgSize;
		INT32	n32SSNetListenerPort;
		INT32	n32GSNetListenerPort;
		INT32	n32RCNetListenerPort;
		INT32	n32WorkingThreadNum;
		UINT32	un32MaxSSNum;
		UINT32	un32SSBaseIdx;
		UINT32	un32MaxGSNum;
		UINT32	un32GSBaseIdx;
		INT32	maxWaitingDBNum;
		std::string ipaddress;
		string	redisAddress;
		INT32   redisPort;
		string	redisLogicAddress;
		INT32   redisLogicPort;
		string	LogAddress;
		INT32   LogPort;
		SCSKernelCfg(){}
		~SCSKernelCfg(){}
	};
	
	struct	SSSNetInfo
	{
		TIME_MILSEC	tConnMilsec;
		CCSSSInfo*	pcSSInfo;
	};

	struct	SRCNetInfo
	{
		TIME_MILSEC	tConnMilsec;
		CCSRCInfo	cRCInfo;
		SRCNetInfo() { tConnMilsec = 0; }
	};

	struct	SGSNetInfo
	{
		TIME_MILSEC	tConnMilsec;
		CCSGSInfo*	pcGSInfo;
	};

public:
	CCSSSInfo*			m_pcSSInfoList;
	CCSGSInfo*			m_pcGSInfoList;
	SSSNetInfo*			m_psSSNetInfoList;//场景列表信息
	SGSNetInfo*			m_psGSNetInfoList;//网关列表信息
	SRCNetInfo*			m_psRCNetInfoList;//场景列表信息
	CHAR				m_szRemoteConsolekey[c_n32DefaultRemoveConsoleKeyLen];//远程管理端密码

	TIME_MILSEC			m_tHeartBeartUTCMilsec;
	TIME_TICK			m_tHeartBeartTickSpan; 

private:
	CCSKernel();

public:
	static CCSKernel& GetInstance();
	~CCSKernel();
	SCSKernelCfg&	GetKernelCfg(){return m_sCSKernelCfg;}
	void			MainLoop();
	UINT32			GetCSID(){return m_sCSKernelCfg.unCSId;}

	int64_t			AddTimer(HeartbeatCallback pHeartbeatCallback, int64_t interval, bool ifPersist){return m_BattleTimer.AddTimer(pHeartbeatCallback, interval, ifPersist);}
	void			RemoveTimer(int64_t timerID){m_BattleTimer.RemoveTimer(timerID);}

    INT32			Initialize();
	INT32			Uninitialize();
	INT32			Start();
	INT32			Stop();
	INT32			OnMsgFromSS(ICSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength, int n32MsgID);
	INT32			OnMsgFromGS(ICSGSInfo *piGSInfo, const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID);
	INT32			OnMsgFromRC(ICSRCInfo *piRCInfo, const char* pMsg, int n32MsgLength, int n32MsgID){return eEC_NullMsgHandler;}
	INT32			HandleMsgFromSS(ICSSSInfo *piGSInfo, const char* pMsg, int n32MsgLength, int n32MsgID);
	INT32			HandleMsgFromGS(ICSGSInfo *piGSInfo, const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID);
	INT32			HandleMsgFromRC(ICSRCInfo *piRCInfo, const char* pMsg, int n32MsgLength, int n32MsgID);

public:
	ICSSSInfo*			GetSSInfoByNSID(INT32 n32NSID);
	ICSSSInfo*			GetSSInfoBySSID(INT32 n32SSID);
	ICSGSInfo*			GetGSInfoByNSID(INT32 n32NSID);
	ICSGSInfo*			GetGSInfoByGSID(INT32 n32GSID);
	ICSRCInfo*			GetRCInfoByNSID(INT32 n32GSID);
	char*				GetRCKey(){return m_szRemoteConsolekey;}
	void				DisconnectOne(INT32 n32NSID);
	INT32				PostMsgToSS(const ICSSSInfo *piInfo, google::protobuf::Message& sMsg, int n32MsgID);
	INT32				BroadCastMsgToSS(google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostMsgToGS(const ICSGSInfo *piInfo, int n32GCNSID, google::protobuf::Message& sMsg, int n32MsgID);
	INT32				BroadCastMsgToGS(google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostMsgToGC(const SUserNetInfo& cpsUserNetInfo, google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostMsgToGS_KickoutGC(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID);
	INT32				PostMsgToLogServer(google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostMsgToRC(const ICSRCInfo *piInfo, google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostMsgToRC(const INT32 nsid, google::protobuf::Message& sMsg, int n32MsgID);
	ICSSSInfo*			GetMiniBattleNumSS();

public:
	UINT32		GetMaxSSNum(){return (INT32)m_sCSKernelCfg.un32MaxSSNum;}
	UINT32		GetMaxGSNum(){return (INT32)m_sCSKernelCfg.un32MaxGSNum;}
private:
	INT32		LoadCfg();
	INT32		UnloadCfg();
	void		ProfileReport(int64_t tUTCMilsec, int64_t tTickSpan);
public:
	//messages from scene server.
	INT32		OnMsgFromSS_AskPing(ICSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength);
	//messages from gate server.
	INT32		OnMsgFromGS_AskRegiste(ICSGSInfo *cpiGSInfo, const char* pData, int n32DataLength);
	INT32		OnMsgFromGS_AskPing(ICSGSInfo *piGSInfo, const char* pMsg, int n32MsgLength);
	INT32		OnMsgFromGS_ReportGCMsg(ICSGSInfo *piGSInfo, const char* pMsg, int n32MsgLength);
public:
	CCSSSInfo*				GetCSSInfoByNSID(INT32 n32NSID);
	CCSSSInfo*				GetCSSInfoBySSID(INT32 n32SSID);
	CCSGSInfo*				GetCGSInfoByNSID(INT32 n32NSID);
	CCSGSInfo*				GetCGSInfoByGSID(INT32 n32GSID);
	CCSRCInfo*				GetCRCInfoByNSID(INT32 n32NSID);
	CCSRCInfo*				GetEmptyCRCInfo();

private:
	CBattleTimer		m_BattleTimer;
	SCSKernelCfg		m_sCSKernelCfg;
	DWORD				m_RunCounts;
	TIME_TICK			m_LastReport;

	SSMsgHandlerMap		m_SSMsgHandlerMap;
	GSMsgHandlerMap		m_GSMsgHandlerMap;
	GCMsgHandlerMap		m_GCMsgHandlerMap;
	RCMsgHandlerMap		m_RCMsgHandlerMap;

    std::string         m_strExeDir;

};
}
