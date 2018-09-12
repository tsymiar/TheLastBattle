#include "stdafx.h"
#include <fstream>

#include "SSBattle.h"
#include "SSBattleMgr.h"
#include "SSGameObject.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSNPC.h"
#include "SSArea.h"
#include "SSOBSev.h"
#include "SSAltarSev.h" 
#include "SSEffectMgr.h"
#include "SSSkillEffect.h"
#include "SSWorkThreadMgr.h"
#include "SSBattle_Script.h"
#include "SSVMMgr.h"

namespace SceneServer{	

	INT32	CSSBattle::LoadMapConfigNpc(){
		if (NULL == m_cpsMapCfg){
			return eEC_NullMapCfg;
		}
		const map<UINT32, SMapObjCfg>& mapObjMap = CSSCfgMgr::GetSSCfgInstance().GetMapObjCfg();
		for (auto it=mapObjMap.begin();it!=mapObjMap.end();++it){
			const SMapObjCfg &rsObjCfg = it->second;
			if (rsObjCfg.un32MapID==m_cpsMapCfg->un32MapID){
				CSSGameUnit *pcGO = AddNPC(rsObjCfg.eObjTypeID, rsObjCfg.un32ObjIdx, rsObjCfg.cBornPos, rsObjCfg.cBorDir, (EGameObjectCamp)rsObjCfg.n32CampID, 0);
				Assert(pcGO!=NULL && "battle(%d) map(%d) load static npc(type=%d,id=%d) error",m_un64BattleID,m_cpsMapCfg->un32MapID,rsObjCfg.eObjTypeID,rsObjCfg.un32ObjIdx);
			}
		}
		return eNormal;
	}

	INT32	CSSBattle::EnterBattle(CSSGameUnit *pcGO, const CVector3D &crsPos, const CVector3D &crsDir){
		SGUID_KEY sObjGUID = pcGO->GetObjGUID();
		CSSGameUnit *pcExistObj = GetGameUnitByGUID(sObjGUID);
		if (NULL != pcExistObj){
			return eEC_GameObjectAlreadyExist;
		}

		//获取碰撞用的分组
		UINT8 ui8MoveGrpID = pcGO->GetDefalutImpactGruopID();

		CVector3D cFreePos;
		if (ui8MoveGrpID == SSImpactGruop_Zhuzi || !pcGO->IfNPC_Building()){
			if (FALSE == GetAroundFreePos(crsPos,cFreePos,pcGO,ui8MoveGrpID)){
				return eEC_InvalidPosition;
			}
		}else{
			cFreePos = crsPos;
		}
		pcGO->SetGOActionState_Pos(cFreePos);
		pcGO->SetExpire(false); 

		UINT64 un64ObjIdx = pcGO->GetObjGUID();
		m_cGameObjectMap.insert(std::make_pair(un64ObjIdx, pcGO));
		AddGameArray(pcGO);

		pcGO->SetGOActionState_Dir(crsDir);	
		pcGO->SetEnterBattleUTCMilsec(GetUTCMiliSecond());

		//Add Object to Sight System
		//first, add light
		if(pcGO->GetLightDistance() > 0){
			AddSightLight(pcGO, pcGO, pcGO->GetCampID());
		}
		//second, add can be see sight object
		AddSightObject(pcGO);

 
		//添加碰撞对象
		if(pcGO->IfCanImpact()){
			AddMoveObject(pcGO,ui8MoveGrpID);
		}
	
		if (pcGO->IfHero()){
			CSSHero *pcHero = (CSSHero*)pcGO;
			pcHero->SyncSkillStateToGC(0);
			pcHero->ChangeFuryValue(eFuryUseAll,0);
			pcHero->ChangeFuryState(eFuryNullState);
			pcHero->StartRecoverCp();
	}  
		return eNormal;
	}

	INT32	CSSBattle::LeaveBattle(CSSGameUnit *pcGO){

		Assert(pcGO);

		//Remove Object from Sight System
		//first, remove light
		if(pcGO->GetLightDistance() > 0){
			RemoveSightLight(pcGO, pcGO, pcGO->GetCampID());
		}
		//second, remove can be see sight object
		RemoveSightObject(pcGO);

		//移除碰撞对象//
		RemoveMoveObject(pcGO);

		/*SGUID_KEY sObjGUID = pcGO->GetObjGUID();
		EObjectType eOT = sObjGUID.GetGUIDType();
		UINT64 un64ObjIdx = sObjGUID;

		UINT32 aun32ParaList[2];
		aun32ParaList[0] = eOT;
		aun32ParaList[1] = (UINT32)un64ObjIdx;
		INT32 n32RetFlag = eNormal;*/

		//pcGO->SetGOSceneData_Battle(NULL);

		return eNormal;
	}

//同步战斗时间（新开战斗或用户重连时发送）
INT32 CSSBattle::SyncBattleSpanTime(CSSUser *pUser)
{
	if (pUser==NULL) return eEC_NullUser;
	if (!m_bIsCanRushNpc) return eNormal;
	GSToGC::BattleSpanTime sMsg;
	sMsg.set_spantime(m_BattleRushNpcTime);
	CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(pUser->GetUserNetInfo(),sMsg,sMsg.msgid());
	return eNormal;
}

INT32 CSSBattle::SyncBattleDelayTime(CSSUser *pUser)
{
	if (pUser==NULL) return eEC_NullUser;
	TIME_MILSEC curTime = GetUTCMiliSecond();
	INT64 spanTime = m_eBattleState==eSSBS_Playing?(curTime-GetBattleStateStartTime()):0;
	INT64 delayTime = CSSCfgMgr::GetSSCfgInstance().GetSSMapFuncDelayTimeCfg(eDelay_Solder,this->GetBattleMapId()) - spanTime;
	if (delayTime<=0) return eNormal;
	GSToGC::BattleDelayTime sMsg;
	sMsg.set_delaytime(delayTime);
	CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(pUser->GetUserNetInfo(),sMsg,sMsg.msgid());
	return eNormal;
}

	INT32	CSSBattle::SyncState(CSSGameUnit *pcGO, bool spot){
		if (NULL == pcGO){
			return eEC_NullUser;
		}
		int msgid = 0;
		google::protobuf::Message* sMsg = pcGO->ConstructObjStateMsg(msgid,spot);
		SendMsgToAllWatcher(*sMsg, msgid, pcGO); 
		delete sMsg;
		return eNormal;
	}

	INT32	CSSBattle::SyncState(CSSGameUnit *pcGO, std::vector<CSSUser*>& cTempUserMap){
		if (NULL == pcGO){
			return eEC_NullUser;
		}

		int msgid = 0;
		google::protobuf::Message* sMsg = pcGO->ConstructObjStateMsg(msgid);
		if (sMsg){
			SendMsgToWatchers(*sMsg, msgid, cTempUserMap);
			delete sMsg;
		}
		return eNormal;
	}

	INT32	CSSBattle::GetAroundHero(const CVector3D &crsCenterPos, INT32 n32Sight, CSSHero **ppcHeroBuff, INT32 n32BuffSize){
		if (NULL == ppcHeroBuff || c_un32MaxUserInBattle > (UINT32)n32BuffSize){
			return eEC_NullPointer;
		}
		INT32 n32GotIdx = 0;
		for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
			CSSHero *pcTempHero = m_asBattleUserInfo[i].pcHero;
			if (NULL == pcTempHero){
				continue;
			}
			if (crsCenterPos.CanWatch((FLOAT)n32Sight, &pcTempHero->GetGOActionStateInfo().cPos)){
				ppcHeroBuff[n32GotIdx++] = pcTempHero;
			}
		}
		return n32GotIdx;
	}

	INT32 CSSBattle::GetAllEnemyHero(INT32 n32CampID, CSSHero **ppcHeroBuff, INT32 n32BuffSize )
	{
		if (NULL == ppcHeroBuff || c_un32MaxUserInBattle > (UINT32)n32BuffSize){
			return eEC_NullPointer;
		}
		INT32 n32GotIdx = 0;
		for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
			CSSHero *pcTempHero = m_asBattleUserInfo[i].pcHero;
			if (NULL == pcTempHero){
				continue;
			}
			if (!CFunction::IfEnemy(n32CampID, pcTempHero->GetCampID())){
				continue;
			}
			ppcHeroBuff[n32GotIdx++] = pcTempHero;
		}
		return n32GotIdx;
	}

	INT32	CSSBattle::GetAroundEnemyHero(const CVector3D &crsCenterPos, INT32 n32Sight, INT32 n32CampID, CSSHero **ppcHeroBuff, INT32 n32BuffSize){
		if (NULL == ppcHeroBuff || c_un32MaxUserInBattle > (UINT32)n32BuffSize){
			return eEC_NullPointer;
		}
		INT32 n32GotIdx = 0;
		for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
			CSSHero *pcTempHero = m_asBattleUserInfo[i].pcHero;
			if (NULL == pcTempHero){
				continue;
			}
			if (!CFunction::IfEnemy(n32CampID, pcTempHero->GetCampID())){
				continue;
			}
			if (crsCenterPos.CanWatch((FLOAT)n32Sight, &pcTempHero->GetGOActionStateInfo().cPos) && pcTempHero->IsDead() == FALSE){
				ppcHeroBuff[n32GotIdx++] = pcTempHero;
			}
		}
		return n32GotIdx;
	}


	INT32	CSSBattle::GetAroundFriendHero(const CVector3D &crsCenterPos, INT32 n32Sight, INT32 n32CampID, CSSHero **ppcHeroBuff, INT32 n32BuffSize)
	{
		if (NULL == ppcHeroBuff || c_un32MaxUserInBattle > (UINT32)n32BuffSize){
			return eEC_NullPointer;
		}
		INT32 n32GotIdx = 0;
		for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
			CSSHero *pcTempHero = m_asBattleUserInfo[i].pcHero;
			if (NULL == pcTempHero){
				continue;
			}
			if (CFunction::IfEnemy(n32CampID, pcTempHero->GetCampID())){
				continue;
			}
			if (crsCenterPos.CanWatch(n32Sight, &pcTempHero->GetGOActionStateInfo().cPos)){
				ppcHeroBuff[n32GotIdx++] = pcTempHero;
			}
		}
		return n32GotIdx;
	}

	//野怪
	INT32 	CSSBattle::CheckWildMonsterBorn(TIME_MILSEC tUTCMilsec){ 
		 
		for (map<EWMLevel,INT64>::const_iterator  itr = m_SSVMMgr->GetWMMonsterDelayCfg().begin(); itr != m_SSVMMgr->GetWMMonsterDelayCfg().end(); ++itr){
			if (tUTCMilsec > itr->second){
				DoWildMonsterBorn(itr->first,tUTCMilsec);  
			}
		}   

		return eNormal;
	} 

	INT32 CSSBattle::DoWildMonsterBorn(EWMLevel el,TIME_MILSEC tUTCMilsec)
	{  
		std::map<INT32/*wm point*/,SGameWildMonsterCfg>  monsterCfg; 
		CSSCfgMgr::GetSSCfgInstance().GetWildMonsterCfg(m_un32MapID,monsterCfg); 
	 
		auto itr =	monsterCfg.begin();
		for (; itr != monsterCfg.end(); ++itr)
		{  
			SGameWildMonsterCfg & wmcfg = itr->second;
			
			if (wmcfg.lv != el)
			{
				continue;
			} 

			auto itrCurtVm = m_SSVMMgr->GetCurtMonsterInfo().find(itr->first);
			if (itrCurtVm != m_SSVMMgr->GetCurtMonsterInfo().end())
			{
				if ((itrCurtVm->second.mapMonsterId.size() > 0) )
				{
					continue;
				}

				if ( itrCurtVm->second.nextBornTime > tUTCMilsec )
				{
					continue;
				}
			}
			
			StartBornWildMonster(itr->first,itr->second);   
		}   
		return eNormal; 
	}

	INT32		CSSBattle::StartBornWildMonster(INT32 pointId, const SGameWildMonsterCfg &wmcfg )
	{ 
		for(INT32 i=0; i< wmcfg.num; i++)
		{ 
			INT32  monsterId = wmcfg.bIfRandom ?  wmcfg.vecIdx[ CFunction::GetGameRandomN(m_Generator,0,wmcfg.vecIdx.size()-1) ] : wmcfg.vecIdx[0];
			CSSNPC *pNpc = this->AddNPC((EObjectType)monsterId, 0, wmcfg.bornPos, wmcfg.bornDir, eGOCamp_AllEnemy, 0); 
			if (NULL == pNpc) 
			{
				ELOG(LOG_ERROR,"add wild monster error:%d",monsterId);
				continue;
			}
			pNpc->SetBelongPoint(pointId); 
			pNpc->SetWMBornPos(pNpc->GetCurPos());
			m_SSVMMgr->AddCurtWildMonsterBornInfo(pointId,monsterId, wmcfg.refreshTime);
		}  
		return eNormal;
	}

	INT32	CSSBattle::DoWildMonsterDead(  CSSNPC* pNPC)
	{   
		if (pNPC->GetBelongPoint() < 1)
		{
			return eNormal;
		}
		auto itr = m_SSVMMgr->GetCurtMonsterInfo().find(pNPC->GetBelongPoint());

		if (itr != m_SSVMMgr->GetCurtMonsterInfo().end())
		{
			CurtWildMonsterInfo &minfo = itr->second;
			auto itrChild = minfo.mapMonsterId.find( pNPC->GetObjType() );
			if(itrChild != minfo.mapMonsterId.end())
			{
				minfo.mapMonsterId.erase(itrChild);
				if (minfo.mapMonsterId.size() < 1)
				{ 
					minfo.nextBornTime = GetUTCMiliSecond() + minfo.nextBornTimeCfg;
				}
			}
			 
			return eNormal; 
		}
 
		//Assert(0);
		ELOG(LOG_ERROR, "wrong wm point %d",pNPC->GetBelongPoint());
		return eNormal;
	} 

	bool	CSSBattle::SetWildNPCGroupCurEnemy(CSSGameUnit *pcMasterObj, CSSGameUnit *pcReasonGO){  
		if(NULL == pcMasterObj) {
			return false;
		}

		CSSNPC *pNpc = pcMasterObj->TryCastToNPC();
		if (NULL == pNpc || !pNpc->IfWildNPC()){
			return false;
		}  

		SGUID_KEY sAtkGUID;
		if (pcReasonGO){
			if (FALSE == pcReasonGO->IfEnemy(pcMasterObj)){
				return FALSE;
			}
			sAtkGUID = pcReasonGO->GetObjGUID();
		}

		if (pNpc->GetBelongPoint() < 1){
			return eNormal;
		}
 
		return TRUE;
	}

	//祭坛刷兵
	INT32 	CSSBattle::DoAltarBornMonster(TIME_MILSEC tUTCMilsec)
	{
		std::map<INT32/*pathid*/,SSolderGoWayPointCfg > tPathPointCfg;

		std::map<INT32/*altar id */, CurtAltarBornSolder>	& altarMap = m_cpAltarSev->GetCurtAltarInfo();
		auto itr = altarMap.begin();
		for (;itr != altarMap.end(); itr++)
		{
			if (itr->second.nextBornTime > tUTCMilsec)
			{
				continue;
			}
			 
			if ((itr->second.mCfg.maxSolderNum != -1)&& (itr->second.curtBornNum > itr->second.mCfg.maxSolderNum) )
			{
				continue;
			}

			tPathPointCfg.clear(); 
			CSSCfgMgr::GetSSCfgInstance().GetSSSolderSearPathPointCfg(m_un32MapID,tPathPointCfg);
			for(int i=0; i < itr->second.mCfg.bornNum; i++)
			{
				CSSNPC *pNpc = this->AddNPC((EObjectType)itr->second.monsterId, 0, itr->second.mCfg.bornPos, itr->second.mCfg.bornDir, (EGameObjectCamp)itr->second.campId, itr->first );
				if (NULL != pNpc  ){  

					SendMsgWhenAltarBornSolder(pNpc->GetObjType() ,itr->second.monsterId,itr->second.campId,itr->second.mCfg.bornPos);

					m_cpAltarSev->UpdateAltarBornSolder( itr->first, tUTCMilsec + itr->second.mCfg.bornSolderSpaceTime); 

					pNpc->SetOccupyPath( tPathPointCfg.find(itr->second.mCfg.pathId)->second.vecPoint );
				} 
			} 
		} 
		return eNormal;
	}

}