#pragma once
#include "stdafx.h"
#include "SSCfgMgr.h"
#include <set>
#include "SSBattle_Script.h"
 

namespace SceneServer{

class CSSBattle;
class CSSGoods;
class CSSNPC;
class CSSHero;
class CSSSkillEffect;
class CSSUser;
class CSSKernel;
 

class CSSBattleMgr
{
	typedef	INT32	(CSSBattleMgr::*PFCSMsgHandler)(const char* pMsg, int n32MsgLength);
	typedef	INT32	(CSSBattleMgr::*PFGSMsgHandler)(ISSGSInfo *piGSInfo, const char* pMsg, int n32MsgLength);
	typedef	INT32	(CSSBattleMgr::*PFGCMsgHandler)(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	typedef std::map<INT32, PFCSMsgHandler> CSMsgHandlerMap;
	typedef std::map<INT32, PFGSMsgHandler> GSMsgHandlerMap;
	typedef std::map<INT32, PFGCMsgHandler> GCMsgHandlerMap;

private:
	UINT64						m_un64MaxBattleID;
	map<UINT64, CSSBattle*>		m_cAllBattleMap;
	CSMsgHandlerMap				m_asCSMsgHandler;
	GSMsgHandlerMap				m_asGSMsgHandler;
	GCMsgHandlerMap				m_asGCMsgHandler;
	map<UINT64, CSSUser*>		m_cUserGUIDMap;
	map<SUserNetInfo, CSSUser*>	m_cUserNetInfoMap;
	TIME_MILSEC					m_tHeartBeartUTCMilsec;

private:
	CSSUser*					m_pMsgUser;
	CSSBattle*					m_pMsgBattle;
	
	 
public:
	CSSBattleMgr();
	~CSSBattleMgr();
	TIME_MILSEC					GetHeartBeatUTCMilsec();

	void						OnBattleHeartBeat(int64_t tUTCMilsec, int64_t tTickSpan);
	void						OnEvent(UINT32 event_type,PVOID params);
	 
	/************************************************************************/
	/* interface functions from ISSModel                                    */
	/************************************************************************/
 
	INT32			OnMsgFromCS(const char* pData, int n32DataLength, int n32MsgID);
	INT32			OnMsgFromGS(ISSGSInfo *piGSInfo, const char* pData, int n32DataLength, int n32MsgID);
	INT32			OnMsgFromGC(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength, int n32MsgID);
	INT32			OnMsgFromGS(const char* pMsg, int n32MsgLength, int n32MsgID);
	
public:
	CSSBattle*				GetBattle(UINT64 un64BattleID);
	INT32					PostMsgToCS_SyncAllBattleInfo();
	/************************************************************************/
	/* Msg to CS                                                            */
	/************************************************************************/	
private:
	INT32				Initialize();
	INT32				Uninitialize();
	CSSBattle*			CreateNewBattle(CSToSS::OrderBeginNewBattle& sOrderBeginNewBattle, INT32 &rn32RetFlag);
	INT32				PostMsgToGC_AskReturn(CSSUser *piUser, INT32 n32AskProtocalID, INT32 n32RetFlag);
	//////////////////////////////////////////////////////////////////////////
	INT32				OnMsgFromCS_CreateBattle(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_UserOnline(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_UserOffline(const char* pData, int n32DataLength);
	//////////////////////////////////////////////////////////////////////////
	INT32				OnMsgFromGC_AskEnterBattle(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskTrySelectHero(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskSeleteHero(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskSeleteRunePage(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskMoveDir(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskStopMove(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskBuyGoods(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskSellGoods(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskUseGoods(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskMoveGoods(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_LoadBattleComplete(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskUseSkill(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskLockTarget(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_ReportAltarStrawSolder(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32               OnMsgFromGC_AskGasExplosion(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);	
	INT32				OnMsgFromGC_AskAbsorbMonster(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskBuyReborn(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	
	INT32				OnMsgFromGC_AskBornNPC(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength); 
	INT32				OnMsgFromGC_AskAutoAttack(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskMoveTar(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskAlwaysAutoAtk(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_GMCmd(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32				OnMsgFromGC_AskQuickBattleEnd(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pMsg, int n32MsgLength);
	INT32				OnMsgFromGC_AskHerosBattleInfo(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pMsg, int n32MsgLength);
	INT32				OnMsgFromGC_AskHeroAttributes(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pMsg, int n32MsgLength);
public:// For SSUser
	CSSUser*			AddUser(SUserNetInfo *cpsUserNetInfo, INT64 guid);
	INT32				RemoveUser(CSSUser*& pUser);
	INT32				SetUserNet(CSSUser *pcUser,SUserNetInfo& netinfo);
	INT32				RemoveUserNet(CSSUser *pcUser);//只移除UserMgr下的网络快表，User下信息未清理//
	CSSUser*			GetCUserByGUID(const SGUID_KEY& cpsUserGUID);
	CSSUser*			GetCUserByNetInfo(const SUserNetInfo &crsUNI);
	CSSUser*			GetCUserByNetInfo(const ISSGSInfo *cpiGSInfo, INT32 n32GCNSID);
	INT32				OnMsgFromGS_ReportGCMsg(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pMsg, int n32MsgLength, int n32MsgID);

	//new guide
public: 

	INT32				OnMsgFromGC_CompGuideStep(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);

	INT32				OnMsgFromGC_AskSSGuideStepComp(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
 
};

}
