#include "stdafx.h"
#include "SSBattle.h"
#include "SSBattleMgr.h"
#include "SSGameObject.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSNPC.h"
#include "SSArea.h"
#include "SSAltarSev.h"
#include "SSOBSev.h"

#include "SSBattle_Script.h"
#include "SSWorkThreadMgr.h"
#include "log/SSGameLogMgr.h"

namespace SceneServer{

INT32	CSSBattle::IfCanSelectHero(CSSUser *piUser, INT32 heroType, bool ifCheckChange){
	if (NULL == piUser){
		return eEC_NullUser;
	}
	if (eSSBS_SelectHero != m_eBattleState){
		return eEC_InvalidBattleState;
	}
	//获取自己的信息
	SBattleUserInfo* pBattleUserInfo = GetUserBattleInfo(piUser);
	//检查是否是能够换英雄的状态
	if(pBattleUserInfo->bIfHeroChoosed || (ifCheckChange && pBattleUserInfo->un32HeroTypeID == heroType)){
		return eEC_HasChoosedHero;
	}
	//检查是否在英雄列表中
	BOOLEAN ifInHeroList = FALSE;
	for(auto iter = pBattleUserInfo->piUser->GetCanUseHeroList()->begin(); iter != pBattleUserInfo->piUser->GetCanUseHeroList()->end(); iter++){
		if(*iter == heroType){
			ifInHeroList = TRUE;
			break;
		}
	}
	if(FALSE == ifInHeroList){
		return eEC_HasChoosedHero;
	}
	//寻找是否有同组的人选择了相同的英雄
	for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		if (m_asBattleUserInfo[i].IfNoGameUnit() || m_asBattleUserInfo[i].piUser == pBattleUserInfo->piUser){
			continue;
		}
		if(m_asBattleUserInfo[i].un16GrpID == pBattleUserInfo->un16GrpID && m_asBattleUserInfo[i].un32HeroTypeID == heroType){
			return eEC_OthersHasChoosedHero;
		}
	}
	return eNormal;
}

INT32	CSSBattle::AskTrySelectHero(CSSUser *piUser, INT32 heroType){
	INT32 rst = IfCanSelectHero(piUser,heroType,TRUE);
	if(rst != eNormal) return rst;
	//选择英雄，并将消息发给其他玩家
	SBattleUserInfo* pBattleUserInfo = GetUserBattleInfo(piUser);
	pBattleUserInfo->un32HeroTypeID = heroType;
	BroadMsgToGC_TrySelectHero(pBattleUserInfo->un16Pos,heroType);
	return eNormal;
}



INT32	CSSBattle::AskSelectHero(CSSUser *piUser, INT32 heroType){
	INT32 rst = IfCanSelectHero(piUser,heroType,FALSE);
	if(rst != eNormal) return rst;
	//选择英雄，并将消息发给其他玩家
	SBattleUserInfo* pBattleUserInfo = GetUserBattleInfo(piUser);
	pBattleUserInfo->un32HeroTypeID = heroType;
	pBattleUserInfo->bIfHeroChoosed = TRUE;
	BroadMsgToGC_SelectHero(pBattleUserInfo->un16Pos,heroType); 
	 
	return eNormal;
}

INT32	CSSBattle::AskSelectRunePage(CSSUser *piUser, INT32 pageIndex){
	SBattleUserInfo* pBattleUserInfo = GetUserBattleInfo(piUser);
	if(NULL == pBattleUserInfo->piUser) return eNormal;
	vector<SRunePageParameter>* pRunePageVec = pBattleUserInfo->piUser->GetRunePageVec();
	if(pageIndex < 0 || pageIndex >= pRunePageVec->size()) return eNormal;
	pBattleUserInfo->un16RunePage = pageIndex;
	return eNormal;
}

INT32	CSSBattle::AskMoveDir(CSSUser *piUser, const CVector3D &crsDir){
	if (NULL == piUser){
		return eEC_NullUser;
	}
	CSSHero* pcHero = piUser->GetHero();
	if (NULL == pcHero){
		return eEC_NullHero;
	}
 
	return pcHero->AskMoveDir(crsDir, eGOOrderPriority_UserAsk);
}

INT32 CSSBattle::AskMoveTar(CSSUser *piUser, const CVector3D& sPos){
	CSSHero* pHero = piUser->GetHero();
	if (pHero){
		pHero->AskMoveTar(sPos);
	}

	return eNormal;
}

INT32	CSSBattle::AskStopMove(CSSUser *piUser){
	if (NULL == piUser){
		return eEC_NullUser;
	}
	CSSHero *pcHero = piUser->GetHero();
	if (NULL == pcHero){
		return eEC_NullHero;
	}
	return pcHero->AskStopMove(eGOOrderPriority_UserAsk);
}

INT32	CSSBattle::AskAttack(CSSUser *piUser, const SGUID_KEY &crsTarGUID){
	if (NULL == piUser){
		return eEC_NullUser;
	}
	CSSHero *pcHero = piUser->GetHero();
	if (NULL == pcHero){
		return eEC_NullHero;
	}
	return pcHero->AskStartAutoAttack();
}

INT32	CSSBattle::AskRebornHero(CSSHero *pcHero){
	CSSUser *pUser = pcHero->GetUser();
	if (pUser){
		PostMsgToGC_NotifyHeroReborn(pUser);
	}
	 
	if (1 == pcHero->GetCampID() % 2)
	{
		CVector3D cvTemp;
		cvTemp.m_fX = 1;
		cvTemp.m_fY = 0;
		cvTemp.m_fZ = -1;
		pcHero->SetGOActionState_Dir(cvTemp);
	} 
	pcHero->BeginAction_Free(false);
 
	ResetPos(pcHero,pcHero->GetHeroBornPos(),TRUE); 
	pcHero->FullHP();
	pcHero->FullMP();
	SyncState(pcHero);
	return eNormal;
}

INT32	CSSBattle::AskLockTar(CSSUser *piUser, const SGUID_KEY &crsTarGUID){
	CSSHero *pcHero = piUser->GetHero();
	if (NULL == pcHero){
		return eEC_NullHero;
	}

	if (pcHero->GetAlwaysAutoAtk()){
		return eNormal;
	}

	if (crsTarGUID < 1){
		pcHero->ClearLockedTargetGUID();
		return eNormal;
	}

	CSSGameUnit *pcGO = GetGameUnitByGUID(crsTarGUID);
	if (NULL == pcGO){
		ELOG(LOG_WARNNING, "Lock not Exit GO");
		return eEC_InvalidAttackTarget;
	}

	if(pcGO->IfNPC()){
		CSSNPC * pNPC = static_cast<CSSNPC*>(pcGO);
		if ((NULL != pNPC) && (!pNPC->GetNPCCfg()->bIsLocked)){
			return eEC_TargetCannotLooked;
		}
	}
	return pcHero->AskLockTar(pcGO);
}

INT32	CSSBattle::AskUseSkill(CSSUser *piUser, UINT32 un32SkillID){
	CSSHero *pcHero = piUser->GetHero();
	if (NULL == pcHero){
		return eEC_NullHero;
	}

	INT32 nret = pcHero->AskUseSkill(un32SkillID, eGOOrderPriority_UserAsk);
	
	return nret; 
}

INT32	CSSBattle::ReportStrawSolder(CSSUser *piUser,  INT32 crsAltarIdex,  EObjectType eOT  ){
	if (NULL == piUser)	{
		return eEC_NullMasterUser;
	} 

	CSSHero *pcHero = piUser->GetHero();
	if (NULL == pcHero){
			return eEC_NullHero;
	}  
 
	return StrawSolder(pcHero,crsAltarIdex,eOT);
}

INT32	CSSBattle::StrawSolder(CSSHero* pcHero,   INT32  crsAltarIdex,   EObjectType eOT ){
	if(!CSSCfgMgr::GetSSCfgInstance().IfNPCCanControlByType(eOT)){
		return eEc_InvalidControlNPCType; 
	}

	INT32 n32RetFlag = pcHero->AskRemoveAbsorbSkill(eOT);
	if (n32RetFlag != eNormal)		{
		return eEc_DelAbsorbICOFailed;
	} 
	CSSGameUnit *pUnit = GetGameObjectByGUIDIdx(crsAltarIdex);
	if (NULL == pUnit)
	{
		return eEC_NullPointer;
	}
	if (!pUnit->IfNPC() || !pUnit->IfNPC_Building() )
	{
		return eEC_InvalidObjTypeID;
	}
	//是否相同阵营
	if (CFunction::IfEnemy(pUnit->GetCampID(), pcHero->GetCampID()) )
	{
		return eEC_CampNotBalance;
	} 

	CSSNPC *pAltar = (CSSNPC *)pUnit;
	if (pAltar && (pAltar->GetNPCCfg()->eNPCCateChild == eNPCChild_BUILD_Altar))
	{ 
		m_cpAltarSev->AddAltarSolderIdx(crsAltarIdex, eOT , pcHero->GetCampID(),m_un32MapID); 
		pAltar->SetHoldNPCState(true,eOT,pcHero->GetObjGUID()); 
		SendAltarIco(crsAltarIdex, eOT, eAltarBornNPCType_Add); 	 
	} 
	return n32RetFlag;  
}
 

INT32	CSSBattle::AskGasExplosion(CSSUser *piUser  ){
	CSSHero *pcHero = piUser->GetHero();
	if (NULL == pcHero){
		return eEC_NullHero;
	}
	//接触无敌(new guide)
	DelHeroNoHurtStateAtDead(pcHero);
	return pcHero->AskGasExpersion();
}



INT32 CSSBattle::AskAbsorbMonster(CSSUser *piUser, INT32 n32ToRemoveIndex){
	CSSHero *pcHero = piUser->GetHero();
	if (NULL == pcHero){
		return eEC_NullHero;
	}

	return pcHero->AskAbsorbMonster(n32ToRemoveIndex); 
}


INT32 CSSBattle::AskRemoveAbsorb(CSSUser *piUser, UINT32 un32MonsterID){
	CSSHero *pcHero = piUser->GetHero();
	if (NULL == pcHero){
		return eEC_NullHero;
	}
	return pcHero->AskRemoveAbsorbSkill(un32MonsterID);
}

INT32	CSSBattle::AskGetHerosBattleInfo(CSSUser *pUser)
{
	GSToGC::NotifyHerosInfo HerosInfo;
	for (INT32 i = 0; i < c_un32MaxUserInBattle; i ++){
		SBattleUserInfo *Info = &m_asBattleUserInfo[i];
		if (Info->IfNoGameUnit()){
			continue;
		}
		GSToGC::NotifyHerosInfo_heropanelinfo *heroinfo = HerosInfo.add_info();
		heroinfo->set_camgpid(Info->un16GrpID);
		heroinfo->set_guid(Info->pcHero->GetObjGUID());
		heroinfo->set_herolv(Info->pcHero->GetLevel());
		heroinfo->set_nickname(Info->stNickName);
		heroinfo->set_killnum(Info->pcHero->GetSSendData().un32HeroKills);
		heroinfo->set_deadtimes(Info->pcHero->GetSSendData().un32DeadTimes);
		heroinfo->set_asstimes(Info->pcHero->GetSSendData().un32Assist);
		heroinfo->set_lasthit(Info->pcHero->GetSSendData().un32LastHit);
		heroinfo->set_heroid(Info->un32HeroTypeID);
		for (INT32 j = 0; j < c_un32MaxGridNum; j ++){
			GSToGC::NotifyHerosInfo_heropanelinfo_goodsmsg *goods = heroinfo->add_goods();
			goods->set_grid(j);
			if (Info->pcHero->IsEmptyGrid(j)){
				goods->set_goodid(0);
			}
			else{
				if(0 == Info->pcHero->GetGridGoodsInfo(j).n32Num){
					goods->set_goodid(0);
				}
				else{
					goods->set_goodid(Info->pcHero->GetGridGoodsInfo(j).sGoodsCfg->un32GoodsID);
				}
			}
		}
	}
	CSSWorkThreadMgr::GetInstance().PostMsgToGC(pUser->GetUserNetInfo(), HerosInfo, HerosInfo.msgid());
	return eNormal;
}
 
INT32	CSSBattle::AskBuyReborn(CSSHero *piHero)
{
	if (piHero->IsDead() == FALSE){
		return eEC_HeroNotDead;
	}

	if(0 >= piHero->GetHeroRebornTimes()){
		return eEC_NoRebornTimes;
	}

	if ( (piHero->GetUser()->GetDiamond() - piHero->GetUser()->GetDiamondUsed() ) < piHero->GetBuyRebornCost()){
		return eEC_DiamondNotEnough;
	}

	INT32 n32ReFlag = AskRebornHero(piHero);
	if (eNormal == n32ReFlag){
		piHero->GetUser()->AddDiamondUsed(piHero->GetBuyRebornCost());
		piHero->ChangeUserHeroRebornTimes(c_MinusRebornTimes);	
		piHero->AddBuyRebornTimes();
		GSToGC::RebornSuccess sRebornSuccess;
		SendMsgToSelfAndOB(sRebornSuccess, sRebornSuccess.msgid(), piHero->GetUser());
		ELOG(LOG_INFO, "RebornSuccess");
	}
	return eNormal;
}

}