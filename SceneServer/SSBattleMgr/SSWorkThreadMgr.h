#pragma once
#include "stdafx.h"
#include "SSGSInfo.h"
#include "../ELogging/ELogging/ThreadSafeQueue.h"
#include <boost/thread.hpp>
#include "../ELogging/ELogging/SSActive.h"
#include "../ELogging/ELogging/Timer.h"

class Active;
class Buffer;
namespace SceneServer{

class CSSBattleMgr;
struct SGSNetInfo;

class BattleLogicThreadActor{
public:
	Active*			m_pActive;
	CSSBattleMgr*		m_pBattleMgr;

	int				m_BattleNum;
	BattleLogicThreadActor();
	static bool		EncodeProtoMsgToBuffer(google::protobuf::Message& sMsg, int n32MsgID, Buffer& pBuffer);
	bool			EncodeGSMsgAndSendToLogicThread(ISSGSInfo *pcGSInfo, int n32GcNetID, const char* pMsg, int n32MsgLength, int n32MsgID);
	bool			EncodeCSMsgAndSendToLogicThread(const char* pMsg, int n32MsgLength, int n32MsgID);
	int				GetPendingWorkNum()const;
	int				GetActorID();
	void			Handler(Buffer*& pBuffer);
	void			AddWorkNum(){++m_BattleNum;}
	void			DecWorkNum(){--m_BattleNum;}
	void			Start();
};

class CSSWorkThreadMgr
{
private:
	CSSWorkThreadMgr();
public:
	~CSSWorkThreadMgr();
	static CSSWorkThreadMgr& GetInstance();
	INT32		Init();
	INT32		Start();
	SSSConfig&	GetSSConfig();
	void				Update();

	void CheckDelBattleCallbackQueue();

	void				NotifyDelBattle(INT64 battleid);
	INT32				PostMsgToGS(INT32 n32NSID, google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostGateServerDisconnect(INT32 n32NSID);
	INT32				PostMsgToCS(google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostMsgToLogServer(google::protobuf::Message& sMsg, int n32MsgID);
	CSSGSInfo*			GetGSInfoByNSID(INT32 n32NSID);
	CSSGSInfo*			GetGSInfoByGSID(INT32 n32GSID);

	INT32				PostMsgToGC(const ISSGSInfo *cpiGSInfo, INT32 n32GCNSID, google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostMsgToGC(const SUserNetInfo& cpsUserNetInfo, google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostMsgToGCLIULU(const SUserNetInfo& cpsUserNetInfo, google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostMsgToGCLIULU(const SUserNetInfo& cpsUserNetInfo, std::string& sSerStr, int n32MsgID);
	INT32				PostMsgToGC_KickoutGC(const ISSGSInfo *cpiGSInfo, INT32 n32GCNSID);
	INT32				OnMsgFromGS_GSAskRegiste(INT32 n32NSID, const char* pMsg, int n32MsgLength);

	TIME_MILSEC			GetNPCMaxLifeTime(){return m_NPCMaxLifeTime;}

	void				SendGSMsgToLogicThread(ISSGSInfo *pcGSInfo, int gcNetID, const char* pMsg, int n32MsgLength, int n32MsgID);
	void				SendCSMsgToLogicThread(const char* pMsg, int n32MsgLength, int n32MsgID);

	void				CheckCSConnect(int64_t tUTCMilsec, int64_t tTickSpan);
	void				ProfileReport(int64_t tUTCMilsec, int64_t tTickSpan);

	void				OnEvent(UINT32 event_type,PVOID params);
	
	int64_t				GetUTCTimeOfPerThread();
	int64_t				GetUICInitSpanTime() { return m_InitSpanTime; }
	ptime&				GetInitTime(){return pInitTime;}

	int64_t					AddTimer(HeartbeatCallback pHeartbeatCallback, int64_t interval, bool ifPersist);
	void					RemoveTimer(int64_t timerID);
	INT32				GetMiniBattleObjNum(){return m_MiniBattleObjNum;}
private:
	BattleLogicThreadActor*				GetMiniLoadActor();
	BattleLogicThreadActor*				GetActorFromBattle(INT64 battleID);
	BattleLogicThreadActor*				GetActorFromGUID(SGUID_KEY& guid);
	bool								SendMsgFromBattleID(INT64 battleID, const char* pMsg, int n32MsgLength, int n32MsgID);
	
public:;
	SSSConfig*			m_psSSConfig;
	typedef map<int, CSSGSInfo*> GSInfoMap;
	GSInfoMap			m_pcGSInfoList;
	typedef map<int, CSSGSInfo*> GSNetInfoListMap;
	GSNetInfoListMap	m_psGSNetInfoList;
	UINT32				m_un32GSBaseIdx;
	UINT32				m_un32MaxGSNum;
	TIME_MILSEC			m_NPCMaxLifeTime;
	TIME_MILSEC			m_BattleFinshProtectTimeSpan;
	INT32				m_MaxStatisMsgToShow;
	TIME_MILSEC			m_MsgStaticsInterval;
	bool				m_IfStatics;
	TIME_MILSEC			m_tCSTimeError;
	INT32				m_ThreadNum;

private:
	struct BattleActor{
		BattleLogicThreadActor* pActor;
		vector<SGUID_KEY> guidVec;
	};
	typedef map<INT64,	BattleActor>	BattleID2ActorIDMap;
	BattleID2ActorIDMap	m_BattleID2ActorIDMap;

	struct UserActor{
		BattleLogicThreadActor* pActor;
		SGUID_KEY sSGUID_KEY;
		SUserNetInfo sNetInfo;
	};
	typedef map<SGUID_KEY, UserActor*>	User2ActorMapByGuid;
	User2ActorMapByGuid	m_User2ActorMapByGuid;//真实表：用户GUID信息(截取CS->SS创建战场，SS自己删除战场生成的)
	typedef map<SUserNetInfo, UserActor*>	User2ActorMapByNetInfo;
	User2ActorMapByNetInfo	m_User2ActorMapByNetInfo;//网络快表：用户网络信息(截取CS->SS创建战场、用户上下线消息生成的)

private:
	int		m_n32ActorNum;
	typedef vector<BattleLogicThreadActor*> ActorVec;
	ActorVec	m_pBattleLogicThreadActorVec;
	Concurrency::concurrent_queue<INT64>		m_ToDelBattleCallbackQueue;
	BattleLogicThreadActor*	GetActorByThreaID();

private:
	DWORD				m_RunCounts;
	TIME_TICK			m_LastReport;
	typedef map<unsigned, BattleLogicThreadActor*> ThreadID2KernelMap;
	ThreadID2KernelMap	m_ThreadID2KernelMap;
	ptime  pInitTime;
	int64_t				m_InitSpanTime;
	CBattleTimer		m_BattleTimer;
	INT32				m_MiniBattleObjNum;
};

}