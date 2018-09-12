#include "stdafx.h"
#include "SSBattle.h"
#include "SSBattleMgr.h"
#include "SSGameObject.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSNPC.h"
#include "SSArea.h"
#include "SSAltarSev.h"
#include "SSEffectMgr.h"
#include "SSOBSev.h"
#include "SSPassiveSkillMgr.h"
#include "SSightMgr.h"
#include "SSight.h"
#include "SSUser.h"
#include "SSBattle_Script.h"

#include "SSWorkThreadMgr.h"
#include "SSMoveMgr.h"
#include "SSVMMgr.h"
#include "SSProfileStatics.h"
#include "log/SSGameLogMgr.h"
#include "SSSolderSev.h"
#include <time.h>

namespace SceneServer{

const INT16	VipUserHeroRebornTimes = 3;
const INT16	NormalUserHeroRebornTimes = 3;
CSSBattle::CSSBattle(){

}
CSSBattle::CSSBattle(CSSBattleMgr* pOwner,EBattleType type,UINT64 battleID)
	: m_eBattleState(eSSBS_SelectHero)
	, m_bIsCanRushNpc(FALSE)
	, m_pCSSBattleMgr(pOwner)
	, m_eBattleType(type)
	, m_un64BattleID(battleID)
	, m_tBattleStateTime(GetUTCMiliSecond())
	, m_tLastCheckPlayTimeout(0)
	, m_BattleFinishProtectTime(0)
	, m_BattleRushNpcTime(0)
	, m_n64ObjectID(CSSWorkThreadMgr::GetInstance().GetMiniBattleObjNum())
	, m_cpsMapCfg(NULL)
	, m_cGameObjectArray(NULL)
	, m_cGameObjectCount(0)
	, m_cGameObjectCountMax(0)
	, m_cGameReleasePos(0)
	, m_un32MapID(0)
	, m_cpAltarSev(new CSSAltarSev())
	, m_cpOBSev(new CSSOBSev())
	, m_n64TotalCashPledge(0)
	, m_bIfFirstBlood(true)
	, m_bIfAced(FALSE)
	, m_pEffectMgr(new CSSEffectMgr())
	, m_pPassiveSkillMgr(new CSSPassiveSkillMgr())
	, m_pSightMgr(new CSSightMgr())
	, m_pMoveMgr(new CSSMoveMgr())
	, m_n32EffectID(0)
	, m_SSVMMgr(new CSSVMBornMgr())
	,m_BattleHero(0)
	,m_SolderSev(new SSSolderSev)
	, m_Generator(static_cast<long int>(time(NULL)))

	{
		m_pEffectMgr->SetBattleMgr(pOwner,this);
		m_pPassiveSkillMgr->SetBattleMgr(pOwner);
		m_lastSaveLogTime = time(NULL); 
	}

CSSBattle::~CSSBattle(void){
	ELOG(LOG_DBBUG, "Destroy a battle : %llu",GetBattleID());
	CSSWorkThreadMgr::GetInstance().NotifyDelBattle(GetBattleID());
	KickoutAllUser();
	for (auto iter = m_cGameObjectMap.begin(); iter != m_cGameObjectMap.end(); ++iter){
		CSSGameUnit *pcGO = iter->second;
		delete pcGO;
	}

	if (m_pPassiveSkillMgr){
		delete m_pPassiveSkillMgr;
		m_pPassiveSkillMgr = NULL;
	}

	if (m_pEffectMgr){
		delete m_pEffectMgr;
		m_pEffectMgr = NULL;
	}

	if (m_pSightMgr){
		delete m_pSightMgr;
		m_pSightMgr = nullptr;
	}

	if (m_pMoveMgr){
		delete m_pMoveMgr;
		m_pMoveMgr = nullptr;
	}

	if (m_SSVMMgr){
		delete m_SSVMMgr;
		m_SSVMMgr = nullptr;
	}

	if (m_cGameObjectArray){
		delete []m_cGameObjectArray;
	}

	for (auto iter = m_DealySendMsgSet.begin(); iter != m_DealySendMsgSet.end(); ++iter){
		delete (*iter);
	}
	 
	if (m_SolderSev)
	{
		delete m_SolderSev;
		m_SolderSev = nullptr;
	}
}

INT32	CSSBattle::KickoutAllUser(){
	for (UINT32 i = 0; i < c_un32MaxUserInBattle; ++i){
		if (m_asBattleUserInfo[i].pcHero){
			RemoveISSUserToSight(m_asBattleUserInfo[i].pcHero->GetCampID(), m_asBattleUserInfo[i].piUser);
		}
		
		m_pCSSBattleMgr->RemoveUser(m_asBattleUserInfo[i].piUser);
	}
	return eNormal;
}

void	CSSBattle::SetBattleState(ESSBattleState state, BOOLEAN ifSendToClient){
	m_eBattleState = state;
	m_tBattleStateTime = GetUTCMiliSecond();
	if(ifSendToClient){
		GSToGC::BattleStateChange sMsg;
		sMsg.set_state(m_eBattleState);
		INT32 time = 0;
		if(eSSBS_SelectHero == m_eBattleState) time = c_tSelectHeroTime/1000;
		else if(eSSBS_SelectRune == m_eBattleState) time = c_tSelectRuneTime/1000;
		else if(eSSBS_Loading == m_eBattleState) time = c_tLoadTime/1000;
		sMsg.set_statetimeleft(time);
		BroadMsgToGC(sMsg, sMsg.msgid());
	}
}

INT32	CSSBattle::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	bool res = CheckPlayTimeout(tUTCMilsec);
	if (res){
		return eNormal;
	}

	CheckSelectHeroTimeout(tUTCMilsec);
	CheckSelectRuneTimeout(tUTCMilsec); 
	CheckLoadingTimeOut(tUTCMilsec); 
	DoPlayHeartBeat(tUTCMilsec, tTickSpan);
	return eNormal;
}

INT32 CSSBattle::RandomPickHero(vector<INT32>* pPickHeroVec,INT32 grpID,INT32 exceptID){
	vector<INT32> canChooseVec;
	if(pPickHeroVec == NULL || pPickHeroVec->empty()){
		pPickHeroVec = CSSCfgMgr::GetSSCfgInstance().GetRobotUseHeroVec();
	}
	for(auto iter = pPickHeroVec->begin(); iter != pPickHeroVec->end(); iter++){
		//检查别人是否选择了该英雄
		BOOLEAN ifHeroUsed = FALSE;
		for (UINT32 j = 0; j < c_un32MaxUserInBattle; j++){
			if(exceptID == *iter || (grpID == m_asBattleUserInfo[j].un16GrpID &&  m_asBattleUserInfo[j].un32HeroTypeID == *iter)){
				ifHeroUsed = TRUE;
				break;
			}
		}
		if(FALSE == ifHeroUsed)
			canChooseVec.push_back(*iter);
	}
	std::uniform_int_distribution<int> dis(0,canChooseVec.size()-1);  
	return canChooseVec[dis(m_Generator)];
}

void	CSSBattle::CheckSelectHeroTimeout(TIME_MILSEC tCurUTCMilsec){
	if(eSSBS_SelectHero != m_eBattleState){
		return;
	}
	//AI模拟选英雄逻辑开始
	for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		if (FALSE == m_asBattleUserInfo[i].ifAI) continue;
		if(m_asBattleUserInfo[i].un32AIChooseRandomNum == 0){
			std::uniform_int_distribution<int> dis(10001,99999);
			m_asBattleUserInfo[i].un32AIChooseRandomNum = dis(m_Generator);
			m_asBattleUserInfo[i].tAiNextSelectHeroTime = tCurUTCMilsec + m_asBattleUserInfo[i].un32AIChooseRandomNum%9000;
			continue;
		}
		//如果到达了选英雄的时间了
		if(m_asBattleUserInfo[i].bIfHeroChoosed == FALSE && m_asBattleUserInfo[i].tAiNextSelectHeroTime <= tCurUTCMilsec){
			m_asBattleUserInfo[i].un32HeroTypeID = RandomPickHero(NULL,m_asBattleUserInfo[i].un16GrpID,0);
			if(m_asBattleUserInfo[i].un8AIChooseTimes >= m_asBattleUserInfo[i].tAiNextSelectHeroTime%5){
				m_asBattleUserInfo[i].bIfHeroChoosed = TRUE;
				BroadMsgToGC_SelectHero(m_asBattleUserInfo[i].un16Pos,m_asBattleUserInfo[i].un32HeroTypeID);
			}
			else{
				m_asBattleUserInfo[i].un8AIChooseTimes++;
				std::uniform_int_distribution<int> dis(1000,5000);
				m_asBattleUserInfo[i].tAiNextSelectHeroTime = tCurUTCMilsec + dis(m_Generator);
				BroadMsgToGC_TrySelectHero(m_asBattleUserInfo[i].un16Pos,m_asBattleUserInfo[i].un32HeroTypeID);
			}
		}
	}

	//AI模拟选英雄逻辑结束
	bool ifAllUserSelect = TRUE;
	for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		if (m_asBattleUserInfo[i].piUser == NULL && FALSE == m_asBattleUserInfo[i].ifAI){
			continue;
		}
		if(FALSE == m_asBattleUserInfo[i].bIfHeroChoosed){
			ifAllUserSelect = FALSE;
			break;
		}
	}
	//等待时间结束
	if(ifAllUserSelect || tCurUTCMilsec - m_tBattleStateTime > c_tSelectHeroTime){
		//检查是否有人没选英雄
		for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
			if (m_asBattleUserInfo[i].IfNoGameUnit()){
				continue;
			}
			if(FALSE == m_asBattleUserInfo[i].bIfHeroChoosed){
				//如果没选英雄，随机选一个
				if(m_asBattleUserInfo[i].un32HeroTypeID == 0){
					m_asBattleUserInfo[i].un32HeroTypeID = RandomPickHero(m_asBattleUserInfo[i].ifAI?NULL:m_asBattleUserInfo[i].piUser->GetCanUseHeroList(),m_asBattleUserInfo[i].un16GrpID,0);
				}
				m_asBattleUserInfo[i].bIfHeroChoosed = TRUE;
				//将其选择的英雄的消息广播出去
				BroadMsgToGC_SelectHero(m_asBattleUserInfo[i].un16Pos,m_asBattleUserInfo[i].un32HeroTypeID);
			}
		}
		//英雄选择阶段结束，改变阶段，重置时间
		SetBattleState(eSSBS_SelectRune);
	}
}

void	CSSBattle::CheckSelectRuneTimeout(TIME_MILSEC tCurUTCMilsec){
	if(eSSBS_SelectRune != m_eBattleState){
		return;
	}
	//等待时间结束
	if(tCurUTCMilsec - m_tBattleStateTime > c_tSelectRuneTime){
		//符文皮肤选择阶段结束，改变阶段，重置时间
		SetBattleState(eSSBS_Loading);
	}
}

void CSSBattle::CheckLoadingTimeOut(TIME_MILSEC tUTCMilsec){
	if(eSSBS_Loading != m_eBattleState){
		return;
	}

	bool bIfAllPlayerConnect = true;
	if(tUTCMilsec - m_tBattleStateTime < c_tLoadTime){
		//如果时间未到，则检查是否所有玩家已连接
		for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
			if (m_asBattleUserInfo[i].piUser != NULL && FALSE == m_asBattleUserInfo[i].bIfLoadComplete){
				//如果有非空位而玩家又尚未连接，则继续等待
				bIfAllPlayerConnect = false;
				break;
			}
		}
	}
	if (false == bIfAllPlayerConnect)
	{
		return;
	}
	LoadMapConfigNpc();//地图静态NPC先加载//
	bool bGuideFlag = false;
	for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		if (m_asBattleUserInfo[i].IfNoGameUnit()){
			continue;
		}
		do{
			CSSUser* piUser = m_asBattleUserInfo[i].piUser;
			INT32 heroTypeId = m_asBattleUserInfo[i].un32HeroTypeID;
 
			if (heroTypeId < 1){
				heroTypeId = CSSCfgMgr::GetSSCfgInstance().GetUserHeroIdByMapId(this->GetBattleMapId()); 
			}
 
			EGameObjectCamp eGOCamp = (EGameObjectCamp)(m_asBattleUserInfo[i].un16GrpID);

			SHeroBornPos shbp; 
			bool bFlag=	CSSCfgMgr::GetSSCfgInstance().getHeroBornCfg(this->GetBattleMapId(), eGOCamp,shbp);
			if (!bFlag)
			{
				ELOG(LOG_ERROR,"shit, i can not get cfg,mapId;%d,campId:%d",GetBattleMapId(),eGOCamp);
				break;
			}

			CVector3D pcBornPos = shbp.bPos;
			if (pcBornPos.IsZero()){
				Assert(false);
				break;
			}
			//将英雄的位置适当分散，修改出生位置
			float bornAngle = ((i/2)%3) * (3.1415926f * 2 / 3);
			CVector3D bornMoveVec(cosf(bornAngle),0,sinf(bornAngle));
			bornMoveVec = bornMoveVec * 200;
			pcBornPos = pcBornPos + bornMoveVec;

			CVector3D cDir = shbp.bPosDir;
			if (cDir.IsZero()){
				Assert(false);
				break;
			}

			//然后创建英雄
			CSSHero *pcHero = NULL;
			if(m_asBattleUserInfo[i].ifAI){
				pcHero = AddHero((EObjectType)heroTypeId, pcBornPos, cDir, NULL, m_asBattleUserInfo[i].un64Guid, eGOCamp, m_asBattleUserInfo[i].un16Pos,TRUE);		
			}
			else{
				pcHero = AddHero((EObjectType)heroTypeId, pcBornPos, cDir, piUser, piUser->GetGUID(), eGOCamp, m_asBattleUserInfo[i].un16Pos);
				m_BattleHero++;
			}
			if (NULL == pcHero){
				Assert(false && "AddHero Failed");
				break;
			}
			//加入符文页属性
			if(FALSE == m_asBattleUserInfo[i].ifAI){
				vector<SRunePageParameter>* pRunePageVec = piUser->GetRunePageVec();
				if(FALSE == pRunePageVec->empty()){
					SRunePageParameter runePage = pRunePageVec->at(m_asBattleUserInfo[i].un16RunePage);
					pcHero->GetParaMgr()->AddBaseGroup(runePage.fpBase,TRUE);
					pcHero->GetParaMgr()->AddBaseGroup(runePage.fpPercent,TRUE);
				}
			}
			//加入初始金钱
			pcHero->ChangeCP(eCPCP_Personnal, c_n32DefaultPerCp * 1000);
			m_asBattleUserInfo[i].pcHero = pcHero; 

			//将英雄信息同步到客户端
			pcHero->SyncCpToCl();
			BroadTipsByType(eTips_ObjAppear);
			SyncBattleDelayTime(piUser); 

		}while(FALSE);
	}
	for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		if (m_asBattleUserInfo[i].piUser == NULL){
			continue;
		}
		PostMsgToGC_BattleHeroInfo(m_asBattleUserInfo[i].piUser);  
	}
	for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		if (m_asBattleUserInfo[i].IfNoGameUnit()){
			continue;
		}
		
		if(m_asBattleUserInfo[i].piUser != NULL){
			//先给玩家自己发送其Appear消息
			vector<CSSUser*> cTempUserMap;
			cTempUserMap.push_back(m_asBattleUserInfo[i].piUser);
			m_asBattleUserInfo[i].pcHero->OnAppearInSight(cTempUserMap);
			//将用户添加到视野系统中
			AddISSUserToSight(m_asBattleUserInfo[i].piUser,m_asBattleUserInfo[i].pcHero->GetCampID());
		}
	}
	SetBattleState(eSSBS_Playing); 

	//log
	GetBattleHeroLog();
}

const INT32 MaxBattleTimeout = 10 * 1000;
bool	CSSBattle::CheckPlayTimeout(TIME_MILSEC tCurUTCMilsec){
	if (0 == m_tLastCheckPlayTimeout){
		m_tLastCheckPlayTimeout = tCurUTCMilsec;
		return false;//初始//
	}

	if (tCurUTCMilsec - m_tLastCheckPlayTimeout < MaxBattleTimeout){
		return false;//时间间隔//
	}

	m_tLastCheckPlayTimeout = tCurUTCMilsec;

	bool bAllUserOffLine = true;
	for (INT32 i = 0; i < c_un32MaxUserInBattle; i++){
		CSSUser *piTempUser = m_asBattleUserInfo[i].piUser;
		if (NULL != piTempUser && TRUE == piTempUser->IfConnect()){
			bAllUserOffLine = false;
			break;
		}
	}

	//如果有用户在线，重置战斗保护时间
	if(!bAllUserOffLine)
	{
		m_BattleFinishProtectTime = 0;
	}
	//发现所有用户离线，设置战斗结束保护时间
	if(bAllUserOffLine && m_BattleFinishProtectTime == 0)
	{
		m_BattleFinishProtectTime = tCurUTCMilsec + CSSWorkThreadMgr::GetInstance().m_BattleFinshProtectTimeSpan;
	}

	//如果所有用户离线，并且超出保护时间，结束战斗
	if(bAllUserOffLine && tCurUTCMilsec>m_BattleFinishProtectTime)
	{
		ELOG(LOG_WARNNING,  "all users offline, so finish it.");
		Finish(eBattleEnd_Normal, 0);
		return true;
	}

	return false;
}

void	CSSBattle::DoPlayHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if(eSSBS_Playing != m_eBattleState){
		return;
	}

	//检查掉线时间。如果玩家掉线超过30秒，则将其AI替换为机器人AI
	for(int i=0; i<c_un32MaxUserInBattle;i++){
		if(m_asBattleUserInfo[i].bIfLoadComplete == FALSE && m_asBattleUserInfo[i].pcHero != NULL && m_asBattleUserInfo[i].pcHero->IfAI() == FALSE && m_asBattleUserInfo[i].offLineTime != 0 && tUTCMilsec - m_asBattleUserInfo[i].offLineTime > 30000){
			ResetAIAtOnUserOffline( m_asBattleUserInfo[i].pcHero);
		}
	}
		
	//位移需要在游戏对象心跳之前运行//
	m_pMoveMgr->OnHeartBeat(tUTCMilsec, tTickSpan);

	for (GameObjectMap::iterator iterObj = m_cGameObjectMap.begin(); iterObj != m_cGameObjectMap.end(); ++iterObj){
		iterObj->second->OnHeartBeat(tUTCMilsec, tTickSpan);
	}

	m_pEffectMgr->OnHeartBeat(tUTCMilsec, tTickSpan);
	m_pPassiveSkillMgr->OnHeartBeat(tUTCMilsec, tTickSpan);
	m_pSightMgr->OnHeartBeat(tUTCMilsec, tTickSpan);

	for (GameObjectMap::iterator iterObj = m_cGameObjectMap.begin(); iterObj != m_cGameObjectMap.end();){
		CSSGameUnit *pcGO = iterObj->second;
		if (pcGO->GetExpire()){
			INT32 n32Ret = RemoveGameUnit(pcGO);
			Assert(n32Ret==eNormal);
			iterObj = m_cGameObjectMap.erase(iterObj);
			continue;
		}

		++iterObj;
	}

	//判断游戏是否进入出兵阶段
	if(FALSE == m_bIsCanRushNpc && tUTCMilsec - m_tBattleStateTime >= CSSCfgMgr::GetSSCfgInstance().GetSSMapFuncDelayTimeCfg(eDelay_Solder,this->GetBattleMapId())){
		m_bIsCanRushNpc = TRUE;
		m_BattleRushNpcTime = tUTCMilsec+CSSWorkThreadMgr::GetInstance().GetUICInitSpanTime();
		for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
			SyncBattleSpanTime(m_asBattleUserInfo[i].piUser);
		}
	}
	//进入刷兵时间了，才开始调用脚本。
	if(TRUE == m_bIsCanRushNpc){ 
		OnAllNPCFunc(tUTCMilsec); 
	}

	for (set<DealySendMsg*>::iterator iter = m_DealySendMsgSet.begin(); iter != m_DealySendMsgSet.end();) {
		DealySendMsg* sDealySendMsg = *iter;
		if (tUTCMilsec - sDealySendMsg->beginSendTime >= sDealySendMsg->delayTime){
			for(auto useriter = sDealySendMsg->pUsers.begin(); useriter != sDealySendMsg->pUsers.end(); ++useriter){
				CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(*useriter, sDealySendMsg->msgStr, sDealySendMsg->n32MsgID);
			}
			iter = m_DealySendMsgSet.erase(iter);
			delete sDealySendMsg;
			continue;
		}
		++iter;
	}
}

INT32	CSSBattle::OnAllNPCFunc(INT64  t)
{
	if (CSSCfgMgr::GetSSCfgInstance().GetSSFuncSwitch().solderSwitch)
	{
		OnBornNPCFunc(t);  
	}
	if (CSSCfgMgr::GetSSCfgInstance().GetSSFuncSwitch().altarSwitch)
	{
		DoAltarBornMonster(t);
	}
	if (CSSCfgMgr::GetSSCfgInstance().GetSSFuncSwitch().monsterSwitch)
	{
		CheckWildMonsterBorn(t);
	} 

	return eNormal;
}

INT32	CSSBattle::LoadMapData(UINT32 un32MapID){ 
	//get map cfg.
	m_cpsMapCfg = CSSCfgMgr::GetSSCfgInstance().GetMapCfg(un32MapID);
	if (NULL == m_cpsMapCfg){
		return eEc_InvalidMapId;
	}

	MapStaticData* pMapStaticData = CSSCfgMgr::GetSSCfgInstance().GetMapStaticDataByMapID(un32MapID);
	if (NULL == pMapStaticData){
		ELOG(LOG_ERROR, "Can Find pMapStaticData with mapid:%d", un32MapID);
		return eEc_InvalidMapId;
	}

	m_un32MapID = un32MapID;

	//将地图数据加入MoveMgr
	m_pMoveMgr->SetMapInfo(pMapStaticData->m_n32XRegionNum,pMapStaticData->m_n32ZRegionNum,c_n32CellSize);
	for (INT32 i = 0; i < pMapStaticData->m_n32ZRegionNum; i++){
		for (INT32 j = 0; j < pMapStaticData->m_n32XRegionNum; j++){
			INT32 n32Idx = j + i * pMapStaticData->m_n32XRegionNum;
			SBattleRegion &rsRegion = pMapStaticData->m_pcRegionList[n32Idx];
			if(rsRegion.bStaticBlock){
				m_pMoveMgr->AddStaticBlockInfo(rsRegion.un16XRegionIdx, rsRegion.un16ZRegionIdx);
			}
			if(rsRegion.bStaticTriBlock){
				ETriDir dir;
				if(rsRegion.eTriBlockDir == eIntDir_BelowLeft) dir = ETriDir_LeftBelow;
				else if(rsRegion.eTriBlockDir == eIntDir_BelowRight) dir = ETriDir_RightBelow;
				else if(rsRegion.eTriBlockDir == eIntDir_UpLeft) dir = ETriDir_LeftUp;
				else if(rsRegion.eTriBlockDir == eIntDir_UpRight) dir = ETriDir_RightUp;
				m_pMoveMgr->AddStaticTriBlockInfo(rsRegion.un16XRegionIdx, rsRegion.un16ZRegionIdx, dir);
			}
		}
	}
 
	return eNormal;
}

 
INT32	CSSBattle::Finish(EBattleEndType ebe, INT32 n32WinCampID){
	if (m_eBattleState==eSSBS_Finished) return eNormal;
	FinishCallScript();
	PostMsgToCS_BattleEnd(n32WinCampID);
	m_tBattlePlayTime = GetUTCMiliSecond() - m_tBattleStateTime;
	SetBattleState(eSSBS_Finished);
	PostMsgToGC_BattleResult(n32WinCampID);
	return eNormal;
}

SBattleUserInfo*	CSSBattle::InsertUser(){
	for(int i=0;i<c_un32MaxUserInBattle; i++){
		if(m_asBattleUserInfo[i].IfNoGameUnit()){
			return &m_asBattleUserInfo[i];
		}
	}
	return NULL;
}

void CSSBattle::OnUserOnline(UINT32 pos,SUserNetInfo& netInfo)
{
	m_pCSSBattleMgr->SetUserNet(m_asBattleUserInfo[pos].piUser,netInfo);
	//发送可以进入战场的消息给客户端
	GSToGC::BattleBaseInfo sMsgBattle;
	sMsgBattle.set_battleid(m_un64BattleID);
	sMsgBattle.set_mapid(m_un32MapID);
	sMsgBattle.set_ifreconnect(true);
	ELOG(LOG_SpecialDebug,"玩家上线，邀请玩家(%s)重新加入战场(%d)",m_asBattleUserInfo[pos].piUser->GetNickName(),m_un64BattleID);
	CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(m_asBattleUserInfo[pos].piUser->GetUserNetInfo(), sMsgBattle, sMsgBattle.msgid());
}

void CSSBattle::OnUserOffline(UINT32 pos)
{
	ELOG(LOG_SpecialDebug,"玩家下线，清理玩家(%s)的网络信息",m_asBattleUserInfo[pos].piUser->GetNickName());
	CSSHero* pHero = m_asBattleUserInfo[pos].piUser->GetHero(); 
	//loog
	if(pHero && !pHero->IfAI()&& !IsGuideBattle()){
		stringstream mystream;
		mystream<< ((GetUTCMiliSecond() - this->GetBattleStateStartTime())/1000) <<LOG_SIGN;
		mystream << m_asBattleUserInfo[pos].piUser->GetGUID() << LOG_SIGN;
		mystream << pHero->GetObjType()<<LOG_SIGN;
		mystream <<  ((INT32)pHero->GetCurPos().m_fX)<<LOG_SIGN;
		mystream << ((INT32) pHero->GetCurPos().m_fY)<<LOG_SIGN;
		mystream <<  ((INT32)pHero->GetCurPos().m_fZ);
		CSSGameLogMgr::GetInstance().AddBattleLog(eLog_BattleUserOffLine,this->GetBattleID(),this->GetBattleMapId(),mystream.str());
	}

	if (pHero!=NULL) RemoveISSUserToSight(pHero->GetCampID(),m_asBattleUserInfo[pos].piUser);
	m_pCSSBattleMgr->RemoveUserNet(m_asBattleUserInfo[pos].piUser);
	m_asBattleUserInfo[pos].bIfLoadComplete = FALSE;
	m_asBattleUserInfo[pos].bIfReconnect = TRUE;
	if(m_eBattleState == eSSBS_Playing){
		m_asBattleUserInfo[pos].offLineTime = GetUTCMiliSecond();
	} 
}

void	CSSBattle::ResetAIAtOnUserOffline(CSSHero* pHero)
{
	if(pHero != NULL){
		pHero->SetIfAI(TRUE);
		pHero->ResetAI();
	}
}
INT32	CSSBattle::GetUserSeatPos(CSSUser *piUser){
	Assert(piUser);
	for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		if (m_asBattleUserInfo[i].piUser == piUser){
			return i + 1;
			break;
		}
	}
	return 0;
}

SBattleUserInfo*	CSSBattle::GetUserBattleInfo(CSSUser *piUser){
	if (NULL == piUser){
		return NULL;
	}
	for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		if (m_asBattleUserInfo[i].piUser == NULL){
			continue;
		}
		if(m_asBattleUserInfo[i].piUser->GetGUID() == piUser->GetGUID())
			return &m_asBattleUserInfo[i];
	}
	return NULL;
}

CSSGameUnit* CSSBattle::GetGameObjectByGUIDIdx(INT64 guidIdx){
	if (guidIdx < 1){
		return NULL;
	}
	GameObjectMap::iterator iterObj = m_cGameObjectMap.find(guidIdx);
	if (m_cGameObjectMap.end() != iterObj){
		return iterObj->second;
	}
	return NULL;
}

CSSGameUnit*	CSSBattle::GetGameUnitByGUID(const SGUID_KEY &crsGUID){
	return GetGameObjectByGUIDIdx(crsGUID);
}

// AddGameObject,RemoveGameUnit,EnterBattle,LeaveBattle 
// 4个接口,实体能离开场景存在吗？完全不能呀.//
INT32	CSSBattle::RemoveGameUnit(CSSGameUnit *pcGO){
	INT32  n32Ret = LeaveBattle(pcGO);
	if (n32Ret != eNormal){
		ELOG(LOG_DBBUG, " --- LeaveBattle(pcGO)... failed!--" );
		return n32Ret;
	}
	DelGameArray(pcGO);
	delete pcGO;
	return eNormal;
}

bool CSSBattle::IsInRect(const CVector3D &position,const CVector3D &center,const float length,const float width,CVector3D& dir){
	dir.m_fY = 0.0f;
	CVector3D f2 = position - center;
	float totLen = f2.length();
	float dirLen = dir.length();
	if ((dirLen < 0.000001)  && (dirLen > -0.000001)){
		return FALSE;
	}

	float angle = f2 * dir / (totLen * dirLen);
	if (angle < 0){
		return FALSE;
	}

	float curLength = totLen * angle;
	if (curLength > length){
		return FALSE;
	}

	float curWidth = totLen * sqrt(1 - angle * angle);
	if (curWidth > width){
		return FALSE;
	}

	return TRUE;
}

bool		CSSBattle::IsInCircle(const CVector3D &position,const CVector3D &origin,const float radius){
	return position.CanWatch(radius, origin);
}

bool		CSSBattle::IsInFan(const CVector3D &position,const CVector3D &origin,const float radius,const CVector3D& startRadius,const CVector3D& endRadius){
	if(IsInCircle(position, origin, radius) == false){
		return false;
	}

	CVector3D s = startRadius;
	CVector3D e = endRadius;
	CVector3D c = s + e;
	//
	CVector3D dir = origin - position;
	s = s / s.length();
	c = c / c.length();
	dir = dir / dir.length();

	float t1 = dir.m_fX * c.m_fX + dir.m_fZ * c.m_fZ;
	float t2 = s.m_fX * c.m_fX + s.m_fZ * c.m_fZ;
	if(t1 < t2){
		return true;
	}

	return false;
}

bool	CSSBattle::CheckIfAced(EGameObjectCamp CampID)
{
	INT32 n32TeamDeadNum = 0;
	for (INT32 i = 0; i < c_un32MaxUserInBattle; i ++){
		SBattleUserInfo& pcUserInfo = m_asBattleUserInfo[i];
		if(NULL == pcUserInfo.pcHero){
			continue;
		}
		if(pcUserInfo.pcHero->IsDead() && pcUserInfo.pcHero->GetCampID() % 2 == CampID % 2){
			n32TeamDeadNum ++;
		}
	}

	if (3 <= n32TeamDeadNum){
		return true;
	}
	else{
		return false;
	}
}

INT32	CSSBattle::GetUniqueEffectID(){
	if (m_n32EffectID >= INT_MAX) m_n32EffectID = 0;
	return ++m_n32EffectID;
}

INT32 CSSBattle::UserReconnectToBattleSS(CSSUser *piUser)
{
	GSToGC::BattleStateChange sMsg;
	sMsg.set_state(m_eBattleState);
	INT32 time = 0;
	if(eSSBS_SelectHero == m_eBattleState) time = c_tSelectHeroTime/1000;
	else if(eSSBS_SelectRune == m_eBattleState) time = c_tSelectRuneTime/1000;
	else if(eSSBS_Loading == m_eBattleState) time = c_tLoadTime/1000;
	sMsg.set_statetimeleft(time);
	CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(piUser->GetUserNetInfo(), sMsg, sMsg.msgid());
	SyncBattleDelayTime(piUser);
	SyncBattleSpanTime(piUser);
	Assert(piUser!=NULL);
	SBattleUserInfo* pBUInfo = GetUserBattleInfo(piUser);
	if (NULL == pBUInfo){
		return eEC_HasNoTheHero;
	}
	CSSHero* pHero = pBUInfo->pcHero;
	if (NULL == pHero){
		return eEC_HasNoTheHero;
	}
	CSSGameUnit* pGameObj = GetGameUnitByGUID(pHero->GetObjGUID());
	RemoveISSUserToSight(pHero->GetCampID(),piUser);
	//先给玩家自己发送其Appear消息
	vector<CSSUser*> cTempUserMap;
	cTempUserMap.push_back(piUser);
	pHero->OnAppearInSight(cTempUserMap);
	//再将其加入视野系统中
	AddISSUserToSight(piUser, pHero->GetCampID());
	PostMsgToGC_BattleHeroInfo(piUser);
	SyncState(pGameObj); 

	INT32 n32TimeGone =  GetUTCMiliSecond() - pHero->GetHeroLastDeadMilsec();
	INT32 n32SurplusTimes = pHero->GetFPData(eEffectCate_ReliveTime) - n32TimeGone;
	if (pGameObj->IsDead()){
		GSToGC::RebornTimes sMsg;
		sMsg.set_masterguid(pHero->GetObjGUID());
		sMsg.set_sendtimes(pHero->GetHeroLastDeadMilsec());
		sMsg.set_remain_times(n32SurplusTimes);
		sMsg.set_reborn_time(pHero->GetHeroRebornTimes());
		SendMsgToSelfAndOB(sMsg, sMsg.msgid(), piUser);
	}
	pHero->UserAskReconnect();
	
	//LOG
	if(pHero &&!pHero->IfAI()&& !IsGuideBattle()){
		stringstream mystream;
	 
		mystream<< (( GetUTCMiliSecond() -  GetBattleStateStartTime() )/1000) <<LOG_SIGN;
		mystream <<  piUser->GetGUID()<<LOG_SIGN;
		mystream << pHero->GetObjType()<<LOG_SIGN; 
		CSSGameLogMgr::GetInstance().AddBattleLog(eLog_BattleRecon,GetBattleID(), GetBattleMapId(),mystream.str());
	} 

	return eNormal;
}

CSSight* CSSBattle::GetSightByCamp(EGameObjectCamp eCamp, bool bIfAutoBuild, bool bIfOBSight )
{
	auto iter = m_pSightMap.find(eCamp);
	if (iter != m_pSightMap.end()){
		if(bIfOBSight){
			iter->second->m_sightType = ESightType_OB;
		}
		return iter->second;
	}

	if(bIfAutoBuild && eCamp != eGOCamp_AllEnemy){
		CSSight* pSight = m_pSightMgr->CreateSight();
		m_pSightMap[eCamp] = pSight;
		if(bIfOBSight){
			//如果是OB系统，设置视野的参数
			pSight->m_sightType = ESightType_OB;
		}
		return pSight;
	}
	else{
		return NULL;
	}
}

void CSSBattle::RemoveSightByCamp(EGameObjectCamp eCamp){
	auto iter = m_pSightMap.find(eCamp);
	if (iter != m_pSightMap.end())
	{
		m_pSightMgr->RemoveSight(iter->second);
		m_pSightMap.erase(iter);
	}
}

void CSSBattle::AddISSUserToSight(CSSUser* pIUser, EGameObjectCamp eCamp, bool bIfOBSight){
	CSSight* pSight = GetSightByCamp(eCamp,TRUE,bIfOBSight);
	if(pSight == NULL){
		return;
	}
	m_pSightMgr->AddWatcherAndCheck(pSight, pIUser);
}

void CSSBattle::RemoveISSUserToSight(EGameObjectCamp eCamp, CSSUser* pWatcher){
	CSSight* pSight = GetSightByCamp(eCamp);
	if(pSight == NULL){
		return;
	}
	pSight->RemoveWatcher(pWatcher);
}

void CSSBattle::AddSightObject(ISSightObject* pSO){
	m_pSightMgr->AddSightObject(pSO);
}

void CSSBattle::AddChildSightObject(ISSightObject* pFather, ISSightObject* pChild){
	m_pSightMgr->AddChildSightObject(pFather, pChild);
}

void CSSBattle::RemoveSightObject(ISSightObject* pSO){
	m_pSightMgr->RemoveSightObject(pSO);
}

void CSSBattle::RemoveChildSightObject(ISSightObject* pFather, ISSightObject* pChild){
	m_pSightMgr->RemoveChildSightObject(pFather, pChild);
}

void CSSBattle::AddSightLight(ISSightLight* pLight, ISSightObject* pAsSightObj, EGameObjectCamp eCamp){
	CSSight* pSight = GetSightByCamp(eCamp,true);
	if(pSight == NULL){
		return;
	}
	pSight->AddSightLight(pLight,pAsSightObj);
}

void CSSBattle::RemoveSightLight(ISSightLight* pLight, ISSightObject* pAsSightObj, EGameObjectCamp eCamp){
	CSSight* pSight = GetSightByCamp(eCamp);
	if(pSight == NULL){
		return;
	}
	pSight->RemoveSightLight(pLight,pAsSightObj);
}

void CSSBattle::SendMsgToAllWatcher(google::protobuf::Message& sMsg, int n32MsgID, CSSGameUnit* pGU){
	CSSUser* watcherArray[10] = {NULL};
	UINT16 length = 0;
	m_pSightMgr->GetWatcher(pGU, watcherArray, length);

	for(int i=0;i<length;i++){
		CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(watcherArray[i]->GetUserNetInfo(), sMsg, n32MsgID);
	}
}

void CSSBattle::SendMsgToWatchers(google::protobuf::Message& sMsg, int n32MsgID,std::vector<CSSUser*>& cTempUserMap, TIME_MILSEC dealyTime){
	if (dealyTime > 0){
		DealySendMsg* pDealySendMsg = new DealySendMsg;
		for (auto iter = cTempUserMap.begin(); iter != cTempUserMap.end(); ++iter){
			CSSUser* piWatchUser = *iter;
			pDealySendMsg->pUsers.push_back(piWatchUser->GetUserNetInfo());
		}
		if (pDealySendMsg->pUsers.empty()){
			delete pDealySendMsg;
			return;
		}
		pDealySendMsg->n32MsgID = n32MsgID;
		pDealySendMsg->delayTime = dealyTime;
		pDealySendMsg->msgStr = sMsg.SerializeAsString();
		pDealySendMsg->beginSendTime = m_pCSSBattleMgr->GetHeartBeatUTCMilsec();
		m_DealySendMsgSet.insert(pDealySendMsg);
		return;
	}
	for(std::vector<CSSUser*>::iterator iter = cTempUserMap.begin(); iter != cTempUserMap.end(); ++iter){
		CSSUser* piWatchUser = *iter;
		if (NULL != piWatchUser && piWatchUser->IfConnect()){
			CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(piWatchUser->GetUserNetInfo(), sMsg, n32MsgID);
		}
	}
}

void CSSBattle::SendMsgToSelfAndOB(google::protobuf::Message& sMsg, int n32MsgID, CSSUser* pISelfUser){
	vector<CSSUser*> cTempUserVec;
	m_pSightMgr->GetWatcherBySightType(ESightType_OB, cTempUserVec);
	if (pISelfUser){
		cTempUserVec.push_back(pISelfUser);
	}
	SendMsgToWatchers(sMsg, n32MsgID, cTempUserVec);
}


void CSSBattle::SendMsgToOB(google::protobuf::Message& sMsg, int n32MsgID){
	vector<CSSUser*> cTempUserVec;
	m_pSightMgr->GetWatcherBySightType(ESightType_OB, cTempUserVec);
	SendMsgToWatchers(sMsg, n32MsgID, cTempUserVec);
}

void CSSBattle::SendMsgToAllWatcher(google::protobuf::Message& sMsg, int n32MsgID){
	vector<CSSUser*> pWatcherVec;
	m_pSightMgr->GetAllWatcher(pWatcherVec);
	SendMsgToWatchers(sMsg, n32MsgID, pWatcherVec);
}

/************************************************************************/
/*Sight  End                                                            */
/************************************************************************/



/************************************************************************/
/* new move system                                                      */
/************************************************************************/
void CSSBattle::AddMoveObject(ISSMoveObject* pMObject, UINT8 ui8GrpID){
	m_pMoveMgr->AddMoveObject(pMObject,ui8GrpID);
}

void CSSBattle::RemoveMoveObject(ISSMoveObject* pMObject){
	m_pMoveMgr->RemoveMoveObject(pMObject);
}

bool CSSBattle::AskStartMoveDir(ISSMoveObject* pMObject, const CVector3D& dir){
	return m_pMoveMgr->AskStartMoveDir(pMObject,ColVector(dir.m_fX, dir.m_fZ));
}

bool CSSBattle::AskStartMoveToTar(ISSMoveObject* pMObject, const CVector3D& pos, bool ifMoveToBlackPos, bool bIfFliter){
	return m_pMoveMgr->AskStartMoveToTar(pMObject,ColVector(pos.m_fX, pos.m_fZ),ifMoveToBlackPos,bIfFliter);
}

bool CSSBattle::AskStartMoveForced(ISSMoveObject* pMObject, const CVector3D& dir, FLOAT speed, bool bIfImpact){
	return m_pMoveMgr->AskStartMoveForced(pMObject, ColVector(dir.m_fX, dir.m_fZ),speed,bIfImpact);
}

bool CSSBattle::AskStopMoveObjectAll(ISSMoveObject* pMObject){
	return m_pMoveMgr->AskStopMoveObject(pMObject,EnumAskStopMoveType_ALL);
}
bool CSSBattle::AskStopMoveObjectDir(ISSMoveObject* pMObject){
	return m_pMoveMgr->AskStopMoveObject(pMObject,EnumAskStopMoveType_DIR);
}
bool CSSBattle::AskStopMoveObjectTar(ISSMoveObject* pMObject){
	return m_pMoveMgr->AskStopMoveObject(pMObject,EnumAskStopMoveType_TAR);
}
bool CSSBattle::AskStopMoveObjectForceMove(ISSMoveObject* pMObject){
	return m_pMoveMgr->AskStopMoveObject(pMObject,EnumAskStopMoveType_FORCEMOVE);
}

bool CSSBattle::GetAroundFreePos(const CVector3D& pos, CVector3D &outPos, ISSMoveObject *pGU, UINT8 ui8MoveGrpID){
	outPos.m_fY = pos.m_fY;
	ColVector cOutPos;
	bool rst =  m_pMoveMgr->GetAroundFreePos(ColVector(pos.m_fX, pos.m_fZ), cOutPos, pGU, ui8MoveGrpID);
	if(rst == TRUE){
		outPos.m_fX = cOutPos.x;
		outPos.m_fZ = cOutPos.y;
	}
	return rst;
}

bool CSSBattle::ResetPos(CSSGameUnit *pGU,const CVector3D& pos, bool bIfImpact){
	bool rst = m_pMoveMgr->ResetPos(pGU,ColVector(pos.m_fX, pos.m_fZ),bIfImpact);
	if(rst){
		pGU->GetCurBattle()->SyncState(pGU);
	}
	return rst;
}

void CSSBattle::FindAroundGU(const CVector3D& startPos, FLOAT radios, vector<CSSGameUnit*> &objVec)
{
	FLOAT radiosHoh = radios*radios;

	for (UINT32 i=0;i<m_cGameObjectCount;++i)
	{
		CSSGameUnit *pGU = m_cGameObjectArray[i];
		if (pGU)
		{
			CVector3D& pos = pGU->GetCurPos();

			if(abs(pos.m_fX - startPos.m_fX) > radios || abs(pos.m_fZ - startPos.m_fZ) > radios)
			{
				continue;
			}
			if( (pos.m_fX - startPos.m_fX)*(pos.m_fX - startPos.m_fX) + (pos.m_fZ - startPos.m_fZ)*(pos.m_fZ - startPos.m_fZ) < radiosHoh)
			{
				objVec.push_back(pGU);
			}
		}
	}
}

void CSSBattle::FindAroundEmemy(CSSGameUnit *pMaster, const CVector3D& startPos, FLOAT radios, CSSGameUnit** targetArr, FLOAT* distArr, UINT16 &length, UINT16 maxLength)
{
	length = 0;
	FLOAT radiosHoh = radios*radios;

	for (UINT32 i=0;i<m_cGameObjectCount;++i)
	{
		if(length >= maxLength){
			break;
		}
		CSSGameUnit *pGU = m_cGameObjectArray[i];

		if (pGU && pGU->IfCanBeTarget() && pMaster->IfEnemy(pGU))
		{
			CVector3D& pos = pGU->GetCurPos();
			if(abs(pos.m_fX - startPos.m_fX) > radios || abs(pos.m_fZ - startPos.m_fZ) > radios)
			{
				continue;
			}
			FLOAT distHoh = (pos.m_fX - startPos.m_fX)*(pos.m_fX - startPos.m_fX) + (pos.m_fZ - startPos.m_fZ)*(pos.m_fZ - startPos.m_fZ);
			if( distHoh < radiosHoh)
			{
				targetArr[length] = pGU;
				distArr[length] = distHoh;
				length++;
			}
		}
	}
}

INT32 CSSBattle::GetFirstFreePosByPath(CSSGameUnit* pGU, const CVector3D& cStartPos, const CVector3D& cEndPos, CVector3D &rcOutPos){
	CVector3D cDir = cEndPos - cStartPos;
	if(cDir.length() == 0){
		return eEC_InvalidPosition;
	}
	cDir = cDir.unit();
	float moveStep = 100;
	CVector3D cTempPos = cStartPos;
	while(TRUE){
		if(cTempPos == cEndPos){
			return eEC_InvalidPosition;
		}
		float fDist = (cEndPos - cTempPos).length();
		if(fDist < moveStep){
			cTempPos = cEndPos;
		}
		else{
			CVector3D cMoved = cDir * moveStep;
			cTempPos += cMoved;
		}
		//判断，如果该点不能停留，则继续寻找下一个
		if(m_pMoveMgr->IfPosImpact(pGU, cTempPos.m_fX, cTempPos.m_fZ,pGU->GetColRadius(), pGU->GetGroupID())){
			continue;
		}
		rcOutPos = cTempPos;
		return eNormal;
	}
}

INT32 CSSBattle::GetLastFreePosByPath(CSSGameUnit* pGU, const CVector3D& cStartPos, const CVector3D& cEndPos, CVector3D &rcOutPos){
	rcOutPos = cStartPos;
	CVector3D cDir = cEndPos - cStartPos;
	if(cDir.length() == 0){
		return eEC_InvalidPosition;
	}
	INT32 tMaxNum = 0;
	cDir = cDir.unit();
	float moveStep = 100;
	CVector3D cTempPos = cStartPos;
	while(TRUE){
		if(cTempPos == cEndPos){
			return eNormal;
		}
		float fDist = (cEndPos - cTempPos).length();
		if(fDist < moveStep){
			cTempPos = cEndPos;
		}
		else{
			CVector3D cMoved = cDir * moveStep;
			cTempPos += cMoved;
		}
		/*
		SBattleRegion *psCentralBattleRegion = GetRegionByPos(cTempPos);
		if (NULL == psCentralBattleRegion || psCentralBattleRegion->bStaticBlock){
			return eEC_InvalidPosition;
		}
		if(pGU->IfHero() && psCentralBattleRegion->pcRegionHero != NULL && psCentralBattleRegion->pcRegionHero != pGU){
			continue;
		}
		*/
		//判断，如果该点不能停留，则继续寻找下一个
		ColSphere sph = pGU->GetColSphere();
		if(m_pMoveMgr->IfPosImpact(pGU, sph.c.x, sph.c.y,sph.r, pGU->GetGroupID())){
			continue;
		}
		rcOutPos = cTempPos;

		tMaxNum++;
		if (c_n32MaxWhileNum < tMaxNum)
		{
			break;
		}
	}

	return eNormal;
}


/************************************************************************/
/* new move system end                                                  */
/************************************************************************/

bool CSSBattle::IsPointInCircularSector(const CVector3D &position,const CVector3D &origin, FLOAT theta, FLOAT radius, const CVector3D &dir){
	FLOAT dx = position.m_fX - origin.m_fX;
	FLOAT dz = position.m_fZ - origin.m_fZ;

	FLOAT squareLength = dx * dx + dz * dz;

	if (squareLength > radius * radius){
		return FALSE;
	}

	FLOAT cosTheta = cos(theta);
	FLOAT dotT = dx * dir.m_fX + dz * dir.m_fZ;

	if (dotT >= 0 && cosTheta >= 0){
		return dotT * dotT > squareLength * cosTheta * cosTheta;
	}
	else if (dotT < 0 && cosTheta < 0){
		return dotT * dotT < squareLength * cosTheta * cosTheta;
	}
	else{
		return dotT >= 0;
	}
}

void CSSBattle::SetPos(const CVector3D& pos, GSToGC::Pos& toPos){
	toPos.set_x(pos.m_fX);
	toPos.set_z(pos.m_fZ);
}

void CSSBattle::SetDir(const CVector3D& dir, GSToGC::Dir& toDir){
	toDir.set_angle(atan2f(dir.m_fZ,dir.m_fX) *  10000);
}

void CSSBattle::AddGameArray( CSSGameUnit* pUnit )
{
	if (m_cGameObjectCount==m_cGameObjectCountMax)
	{//已满//
		CSSGameUnit** lastUnitArray = m_cGameObjectArray;
		m_cGameObjectCountMax += 100;
		m_cGameObjectArray = new CSSGameUnit*[m_cGameObjectCountMax];
		for (INT32 i = 0; i < m_cGameObjectCountMax; ++i){
			m_cGameObjectArray[i] = nullptr;
		}
		if (lastUnitArray)
		{
			memcpy(m_cGameObjectArray,lastUnitArray,sizeof(CSSGameUnit*)*m_cGameObjectCount);
			delete []lastUnitArray;
		}
	}
	if (m_cGameReleasePos>0)
	{
		--m_cGameReleasePos;
		UINT32 pos = m_cGameReleased[m_cGameReleasePos];
		m_cGameObjectArray[pos] = pUnit;
		pUnit->m_n32FastArrayID = pos;
	}
	else
	{
		m_cGameObjectArray[m_cGameObjectCount] = pUnit;
		pUnit->m_n32FastArrayID = m_cGameObjectCount;
		++m_cGameObjectCount;
	}
}

void CSSBattle::DelGameArray( CSSGameUnit* pUnit )
{
	if (m_cGameReleasePos<100)
	{
		m_cGameReleased[m_cGameReleasePos] = pUnit->m_n32FastArrayID;
		++m_cGameReleasePos;
	}
	m_cGameObjectArray[pUnit->m_n32FastArrayID] = NULL;
	pUnit->m_n32FastArrayID = 0;
}

CSSNPC* CSSBattle::AddNPC(EObjectType eOT, UINT64 un64ObjIdx, const CVector3D &crsPos, const CVector3D &crsDir, EGameObjectCamp eCampID, INT32 altar, INT32 summonEndTime){
	const SNPCCfg *cpsNPCCfg = CSSCfgMgr::GetSSCfgInstance().GetNPCCfg(eOT);
	if (NULL == cpsNPCCfg){
		return NULL;
	}

	SGUID_KEY sObjGUID = un64ObjIdx;
	if (sObjGUID < 1){
		sObjGUID = GenerateObjGUID();
	}

	CSSNPC* pcNPC = new CSSNPC(sObjGUID, eCampID, eOT, 0, this, m_pCSSBattleMgr);
	pcNPC->SetBattleMgr(m_pCSSBattleMgr);
	pcNPC->SetBelongAltar(altar);
	pcNPC->SetObjEndLife(summonEndTime);

	INT32 n32Res = pcNPC->LoadNPCCfg(cpsNPCCfg);
	Assert(n32Res==eNormal);
	if (n32Res!=eNormal){
		delete pcNPC;
		pcNPC = NULL;
		return NULL;
	}

	pcNPC->BeginAction_Free(false);

	INT32	rn32RetFlag = EnterBattle(pcNPC, crsPos, crsDir);
	if (eNormal != rn32RetFlag){
		ELOG(LOG_ERROR, "%llu EnterBattle failed", sObjGUID);
		delete pcNPC;
		pcNPC = NULL;
		return NULL;
	}
	if (eNPCChild_BUILD_Base == pcNPC->GetNPCCfg()->eNPCCateChild){
		m_BuildingBaseSet.insert(pcNPC);
	}
	pcNPC->LoadPassitiveSkill();
	return pcNPC;
}

CSSHero* CSSBattle::AddHero(EObjectType eOT, const CVector3D &crsPos, const CVector3D &crsDir, 
	CSSUser *piWatchUser, SGUID_KEY rsMasterGUID, EGameObjectCamp eCampID, INT32 seat, bool ifAI)
{
	const SHeroCfg *cpsHeroCfg = CSSCfgMgr::GetSSCfgInstance().GetHeroCfg(eOT);
	if (NULL == cpsHeroCfg){
		return NULL;
	}
	if (FALSE == ifAI &&!piWatchUser){
		return NULL;
	}

	SGUID_KEY sObjGUID = GenerateObjGUID();
	CSSHero*  pcHero = new CSSHero(sObjGUID, eCampID, eOT, rsMasterGUID, this, m_pCSSBattleMgr, ifAI);
	INT32 n32Res = pcHero->LoadHeroCfg(cpsHeroCfg);
	Assert(n32Res==eNormal);
	if (n32Res!=eNormal){
		delete pcHero;
		pcHero = NULL;
		return NULL;
	}

	if (piWatchUser){
		pcHero->ChangeUserHeroRebornTimes(piWatchUser->GetHeroRebornTimes());
	}

	pcHero->SetUser(piWatchUser);
	pcHero->SetSeatID(seat);
	pcHero->BeginAction_Free(false);
	pcHero->SetHeroBornPos(crsPos);

	INT32	rn32RetFlag = EnterBattle(pcHero, crsPos, crsDir);
	if (eNormal != rn32RetFlag){
		ELOG(LOG_ERROR, "%llu EnterBattle failed", sObjGUID);
		delete pcHero;
		pcHero = NULL;
		return NULL;
	}

	pcHero->LoadPassitiveSkill();
	pcHero->ResetAI();
	return pcHero;
}

EGameObjectCamp	CSSBattle::GetHeroCampByGrp(INT32 grpID){
	return (EGameObjectCamp)grpID;
} 
 
}