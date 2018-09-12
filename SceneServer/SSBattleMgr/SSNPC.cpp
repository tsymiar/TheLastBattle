#include "stdafx.h"
#include "SSNPC.h"
#include "SSCfgMgr.h"
#include "SSBattleMgr.h"
#include "SSBattle.h"
#include "SSPassiveSkillMgr.h"
#include "SSHero.h"
#include <limits>
#include "SSUser.h"

#include "SSBattle_Script.h"
#include "SSVMMgr.h"
#include "SSProfileStatics.h"

#include "SSAI_Soldier.h"
#include "SSAI_Building.h"
#include "SSAI_Wild.h"

#include "log/SSGameLogMgr.h"

namespace SceneServer{
const INT32 n32MaxAtkSpeed = 5000;

CSSNPC::CSSNPC(UINT64 guid, EGameObjectCamp sCamp, UINT32 objectID, UINT64 sMasterGUID, CSSBattle* pBattle, CSSBattleMgr* pMgr):CSSGameUnit(guid, sCamp, objectID, sMasterGUID, pBattle, pMgr)
, m_cpsNPCCfg(NULL)
, m_n32BelongPoint(0)
, m_n32WMGroup(eEWMGroup_none)
, m_n32BelongAltar(0)
, m_bIfAtked(FALSE)
, m_tLifeEndTime(0)
, m_totalLife(0)
, m_bHeadIcoState(false)
, m_SolderType(0)
,m_OwnGuiId(0)
{
	for (INT32 i = 0; i < c_n32MaxNPCSkillNum; ++i){
		m_aspSkillArr[i] = new CSSSkill();
	}
}

CSSNPC::~CSSNPC(void){
	for (INT32 i = 0; i < c_n32MaxNPCSkillNum; ++i){
		delete m_aspSkillArr[i];
		m_aspSkillArr[i] = nullptr;
	}
}

INT32	CSSNPC::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if(m_tLifeEndTime != 0 && m_tLifeEndTime <= tUTCMilsec){
		BeginAction_Dead(TRUE,this);
	}

	if (IsDead() && ((GetFatherHeroGUID() > 0 ) && IfNPC_Building()) || IsDead() && !IfNPC_Building())//  !IfNPC_Building()){
	{
		SetExpire(true);
	}

	//检查状态
	if (0 < GetFPData(eEffectCate_Dizziness) && eOAS_PassiveState > GetGOActionStateInfo().eOAS && eOAS_Free != GetGOActionStateInfo().eOAS){
		BeginAction_Free(true);
	}

	CheckRecover(tTickSpan);
	if(m_pAI != NULL) m_pAI->HeartBeat(tUTCMilsec, tTickSpan);
	CheckSyncMoveState();
	OnPassitiveSkillHeartBeat(tUTCMilsec, tTickSpan);
	m_sFpMgr.OnHeartBeat(tUTCMilsec,tTickSpan);
	return eNormal;
}

const INT32 n32Shop1ID = 21002;
const INT32 n32Shop2ID = 21003;
INT32	CSSNPC::LoadNPCCfg(const SNPCCfg* pcCfg){
	if (NULL == pcCfg){
		return eEC_NullPointer;
	}
	m_cpsNPCCfg = pcCfg;

	for (INT32 i = 0; i < c_n32MaxNPCSkillNum; i++){
		UINT32 un32SkillID = m_cpsNPCCfg->aun32SkillList[i];
		if (0 == un32SkillID){
			continue;
		}
		const SSNewSkillCfg *cpsCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillCfg(un32SkillID);
		if (NULL == cpsCfg){
			continue;
		}

		m_aspSkillArr[i]->cpsCfg = cpsCfg;
		m_aspSkillArr[i]->pcMasterGU = (CSSGameUnit*)this;
		m_aspSkillArr[i]->eSkillState = eSkillState_Free;
		m_aspSkillArr[i]->tStateMilsec = 0;

		if (cpsCfg->bIfNomalAttack){
			m_pNormalAttackSkill = m_aspSkillArr[i];
		}
	}

	if (NULL == m_pNormalAttackSkill && pcCfg->eOT != n32Shop1ID && pcCfg->eOT != n32Shop2ID){
		return eEC_NULLCfg;
	}

	m_n32CollideRadius = pcCfg->n32CollideRadius;
	if (c_n32MiniCollideRadius > m_n32CollideRadius){
		m_n32CollideRadius = c_n32MiniCollideRadius;
	}

	m_sFpMgr.AddBaseGroup(m_cpsNPCCfg->sBaseFP,TRUE);
	m_sFpMgr.SetValueChangeCallBack(eEffectCate_AttackSpeed,m_pNormalAttackSkill);
	FullHP();
	FullMP();

	//添加AI
	CSSAI* pAI = NULL;
	if(IfSolider() || (IfWildNPC() && GetBelongAltar() > 0)){
		pAI = new CSSAI_Soldier(this);
	}
	else if(IfNPC_Building()){
		pAI = new CSSAI_Building(this);
	}
	else if(IfWildNPC()){
		pAI = new CSSAI_Wild(this);
	}
	if(pAI != NULL){
		pAI->SetNormalAttackSkill(m_pNormalAttackSkill);
		SetAI(pAI);
	}

	return eNormal;
}

FLOAT	CSSNPC::GetSight(){
	if (NULL == m_cpsNPCCfg){
		return 0.0f;
	}

	if (m_n32BelongPoint > 0){
		return (FLOAT)GetPursueDist();
	}

	return (FLOAT)m_cpsNPCCfg->n32GuardDist;
}

void CSSNPC::SetIfBeAttack(bool bIfAtked){
	m_bIfAtked = bIfAtked;
}

INT32	CSSNPC::SendSummonLifeMsg(ESummonFunc esf,  CSSGameUnit*	pMasterGO){ 
 	TIME_MILSEC nLift  = GetObjEndLife() - GetUTCMiliSecond() ;
	if (nLift < 0){
		nLift = 0;
		SetObjEndLife(nLift);
		BeginAction_Dead(false,this);
		//SetExpire(TRUE);
		return eEC_TimeExpire;
	}
	
	GSToGC::NotifySummonLifeTime sMsg;
	sMsg.set_lifetime(GetObjTotalLife());
	sMsg.set_guid(GetObjGUID());
	GSToGC::Dir* pDir = new GSToGC::Dir;
	CSSBattle::SetDir(GetCurDir(), *pDir);
	sMsg.set_allocated_dir(pDir);

	sMsg.set_resttime(nLift);

	GSToGC::Pos* pPos = new GSToGC::Pos;
	CSSBattle::SetPos(GetCurPos(), *pPos);
	sMsg.set_allocated_pos(pPos);

	if (esf == eSummonFunc_All){
		pMasterGO->GetCurBattle()->SendMsgToAllWatcher(sMsg, sMsg.msgid(), pMasterGO);
	}
	else if (pMasterGO && (esf == eSummonFunc_One)){
		GetCurBattle()->SendMsgToAllWatcher(sMsg, sMsg.msgid());
	}
	return eNormal;}

void CSSNPC::LoadPassitiveSkill(){
	for(int i=0; i < c_n32MaxHeroPassitiveSkillNum; i++){
		UINT32 u32PSkillID = m_cpsNPCCfg->lun32PassiveSkillID[i];
		if(0 != u32PSkillID){
			GetCurBattle()->GetPassiveSkillMgr()->AddPassiveSkill(this,u32PSkillID);
		}
	}
}

bool CSSNPC::IfSolider(){
	if (NULL == m_cpsNPCCfg){
		return FALSE;
	}
	return m_cpsNPCCfg->eNPCCate == eNPCCate_Solider || GetBelongAltar() > 0;
}

CSSGameUnit* CSSNPC::LookForEnemy(CSSGameUnit* pOldEnemy, FLOAT maxDist, bool ifChooseWildMonster){
	CSSBattle *pcCurBattle = GetCurBattle();
	if (NULL == pcCurBattle){
		return NULL;
	}

	FLOAT fGOSight = GetSight();

	ProfileInScope lProfileInScope(StaticsType_NPCLookEnemy);

	const UINT16 maxGuNum = 64;
	CSSGameUnit* ememyArr[maxGuNum] = {NULL};
	FLOAT distArr[maxGuNum] = {0.0f};
	UINT16 length = 0;
	pcCurBattle->FindAroundEmemy(this,this->GetCurPos(),fGOSight,ememyArr,distArr,length,maxGuNum);

#undef max
	FLOAT fPriorDistSqr = numeric_limits<float>::max();
	CSSGameUnit *pcPriorTarget = pOldEnemy;
	EHateLevel ePriorHateLevel = eHateLevel_End;
	if(pOldEnemy != NULL){
		ePriorHateLevel = GetHateLevel(*pOldEnemy);
	}

	for(int i=0;i<length;i++){
		CSSGameUnit *pcGO = ememyArr[i];
		FLOAT distHoh = distArr[i];
		if (NULL != pcGO){
			CSSNPC* pEnemyNPC = pcGO->TryCastToNPC();
			if (NULL != pEnemyNPC){
				if(FALSE == ifChooseWildMonster && pEnemyNPC->IfWildNPC() && pEnemyNPC->GetBelongAltar() <= 0){
					continue;
				}
				if (eNPCChild_NPC_Per_AtkBuilding == GetNPCCfg()->eNPCCateChild){
					if (pEnemyNPC->IfNPC_Building()){
						fPriorDistSqr = distHoh;
						pcPriorTarget = pEnemyNPC; 
					}

					continue;
				}
				else if(eNPCChild_NPC_Per_Bomb == GetNPCCfg()->eNPCCateChild) {
					if(pcGO->IfNPC_Building() && pEnemyNPC->GetNPCCfg()->eNPCCateChild == eNPCChild_BUILD_Base){
						fPriorDistSqr = distHoh;
						pcPriorTarget = pEnemyNPC; 
					}

					continue;
				}
			}

			EHateLevel eTempHateLevel = GetHateLevel(*pcGO);
			if (eTempHateLevel == eHateLevel_End){
				continue;
			}
			if(maxDist > 0 && distHoh > (maxDist + pcGO->GetCollideRadius()) * (maxDist + pcGO->GetCollideRadius()) ){
				continue;
			}

			if (eTempHateLevel < ePriorHateLevel){
				pcPriorTarget = pcGO;
				fPriorDistSqr = distHoh;
				ePriorHateLevel = eTempHateLevel;
			}
			//只有在没有旧目标的情况下，才根据距离寻找目标
			else if (eTempHateLevel == ePriorHateLevel && pOldEnemy == NULL){
				if(fPriorDistSqr > distHoh){
					pcPriorTarget = pcGO;
					fPriorDistSqr = distHoh;
				}
			}
		}
	}

	return pcPriorTarget;
}

INT32 CSSNPC::ChangeCurHP(CSSGameUnit *pcReasonGO, GSToGC::HPMPChangeReason eHPCR, INT32 n32ChangeValue, INT32 skillId ,EEffectCate	eE  ){
	if(pcReasonGO->GetFatherHeroGUID() != 0){
		CSSGameUnit* pFather = pcReasonGO->GetCurBattle()->GetGameUnitByGUID(pcReasonGO->GetFatherHeroGUID());
		if(pFather != NULL){
			pcReasonGO = pFather;
		}
	}

	INT32 n32Res = DoChangeHP(n32ChangeValue, eHPCR, pcReasonGO,this);

	if(pcReasonGO != NULL && n32ChangeValue < 0 && eEC_GUDead != n32Res){
		this->SetCurEnemyGUID(pcReasonGO->GetObjGUID());
	}
	if (eEC_NullPointer == n32Res){
		return eNormal;
	}
	if (eEC_GUDead != n32Res){
		return eNormal;
	}

	CSSHero* pReasonHero = pcReasonGO->TryCastToHero();

	if (NULL != pReasonHero){
		if (!IfNPC_Building()){
			pReasonHero->ChangeFuryValue(eFuryAdd, c_un32NpcGivenFury);
			INT32 n32GetCp = GetNPCCfg()->n32KillGotCP;
			if (GetBelongAltar()){
				n32GetCp = n32GetCp / 2;
			}
			pReasonHero->ChangeCP(eCPCP_Personnal, n32GetCp, TRUE);

			if (GetCampID() != eGOCamp_AllEnemy){
				pReasonHero->AddSSendData_LastHit();
				ELOG(LOG_DBBUG, "LastHit:%d.", pReasonHero->GetSSendData().un32LastHit);
			}
		}
	}

	if (!GetBelongAltar()){
		CSSHero* heroArray[c_un32MaxUserInBattle] = {NULL};
		UINT8	heroArraySize = 0;
		CSSHero *apcHeroList[c_un32MaxUserInBattle];
		INT32 n32AllNum = GetCurBattle()->GetAllEnemyHero(GetCampID(), apcHeroList, c_un32MaxUserInBattle);
		for (INT32  i = 0; i < n32AllNum; ++i){
			CSSHero* pTempHero = apcHeroList[i];
			if (IfNPC_Building()){
				pTempHero->ChangeCP(eCPCP_Personnal, GetNPCCfg()->n32KillGotCP, TRUE);
			}
			if (GetCurPos().CanWatch(2500, pTempHero->GetCurPos())){
				for(int j=0;j<c_un32MaxUserInBattle;j++){
					if(heroArray[j] == NULL){
						heroArray[j] = pTempHero;
						heroArraySize++;
						break;
					}
				}
			}
		}

		if(heroArraySize == 0){
			return eNormal;
		}

		INT32 n32GotExp = GetNPCCfg()->n32GotExp / (FLOAT)heroArraySize;

		for(int i=0;i<c_un32MaxUserInBattle;i++){
			CSSHero* pTempHero = heroArray[i];
			if(pTempHero == NULL) continue;
			pTempHero->ChangeExp(n32GotExp);
		}
	}

	//log
	if (n32ChangeValue < 0 && pReasonHero && !pReasonHero->IfAI() )
	{
		CelObjHurtLog( pcReasonGO,   skillId,n32ChangeValue, eE);
	}  
	return eNormal;
}

INT32 CSSNPC::BeginAction_Dead(bool bIfNotifyGC, CSSGameUnit *pcReasonGO){
	m_sCurOASI.eOAS = eOAS_Dead;
	m_sCurOASI.tASUTCMilsec = GetUTCMiliSecond();
	CSSBattle *pBattle = GetCurBattle(); 

	if (NULL == pBattle){
		return eNormal;
	}

	pBattle->SetCaseDeadReasonInfo(this, pcReasonGO ); 

	if (NULL == pcReasonGO){
		return eNormal;
	} 
	 
	pBattle->DoNPCBuildDead(this, pcReasonGO ); 

	pBattle->DoWildMonsterDead(this);
	 
	if( (eNPCCate_Building == GetNPCCfg()->eNPCCate) && (GetNPCCfg()->eNPCCateChild < eNPCChild_Summon)){
		GSToGC::BroadcastBuildingDestory sBroadcastBuildingDestory;
		sBroadcastBuildingDestory.set_buildingguid(GetObjGUID());
		sBroadcastBuildingDestory.set_building_camp(GetCampID());
		CSSHero *pcTempHero = pcReasonGO->TryCastToHero();
		sBroadcastBuildingDestory.set_killer_guid(pcReasonGO->GetObjGUID());
		sBroadcastBuildingDestory.set_killer_camp(pcReasonGO->GetCampID());

		pBattle->BroadMsgToGC(sBroadcastBuildingDestory, sBroadcastBuildingDestory.msgid());
		if (NULL != pcTempHero){
			pcTempHero->AddSSendData_DestoryBuilding();
		}
	} 
	if (pcReasonGO->IfHero()){
		CSSHero *pHero = (CSSHero*)pcReasonGO;
		if (NULL != pHero){
			pHero->AddKillNPC();
		} 
	}   
	 
	pBattle->CheckBattleFinish(this,pcReasonGO );

	if (bIfNotifyGC)
	{
		pBattle->SyncState(this,true);
	} 

	m_CurEnemyGUID = 0;
	m_CurAtkGUID = 0;

	
	CelObjDeadLog(pcReasonGO);
	return eNormal;
}

void CSSNPC::OnAppearInSight(std::vector<CSSUser*>& cTempUserMap){
	if(FALSE == IfNotNeedSynSight()){
		SendAppearMsg(cTempUserMap, m_cpsNPCCfg->eOT);

		CheckSummonGUNEndLife(eSummonFunc_One, NULL);
	}

	SendHPMPMsg(cTempUserMap);

	GetCurBattle()->SyncState(this,cTempUserMap);

	//更新祭坛上放置的小兵头像
   CheckGUnitNPCICO(eAltarBornNPCType_Add, cTempUserMap); 
}

void CSSNPC::OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap){
	if(IfNotNeedSynSight()){
		return;
	}

	SendDisppearMsg(cTempUserMap);
}

bool CSSNPC::IfNotNeedSynSight(){
	return IfNPC_Building() && GetNPCCfg()->eNPCCateChild != eNPCChild_Summon && GetNPCCfg()->eNPCCateChild != eNPCChild_Summon_End;
}

INT32	CSSNPC::SetOccupyPath(std::vector<CVector3D> & tNode)//(const CVector3D *cpcPathNode, UINT32 un32NodeNum){
{	//只有小兵，才能强转并设置巡逻路径
	if(IfSolider()){
		((CSSAI_Soldier*)m_pAI)->SetOccupyPath(tNode);//(cpcPathNode,un32NodeNum);
	}
	return eNormal;
}

void	CSSNPC::AskStopAI(){
	//只有野怪才能中断和回复AI
	if(IfWildNPC() && GetBelongAltar() <= 0){
		((CSSAI_Wild*)m_pAI)->StopAI();
	}
}
void	CSSNPC::AskResumeAI(){
	//只有野怪才能中断和回复AI
	if(IfWildNPC() && GetBelongAltar() <= 0){
		((CSSAI_Wild*)m_pAI)->ResumeAI();
	}
}

void CSSNPC::CheckDeadState(){
	if (!IfNPC_Building()){
		SetExpire(true);
	}
}

void	CSSNPC::CheckGUnitNPCICO(EAltarBornNpcType ebtype, std::vector<CSSUser*>& cTempUserMap){
	if ((false == m_bHeadIcoState) || (m_SolderType < 1 ) || m_OwnGuiId < 1)
	{
		return;
	}
	 
	if (!this->IfNPC_Building())			{
		return;
	} 
	 
	if (this->GetNPCCfg()->eNPCCateChild != eNPCChild_BUILD_Altar)
	{ 
		return;
	}
	this->GetCurBattle()->SendAltarIco(this->GetObjGUID(), (EObjectType)m_SolderType, ebtype ); 
}
}