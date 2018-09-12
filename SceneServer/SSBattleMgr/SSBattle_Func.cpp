#include "stdafx.h"
#include "SSBattle.h"
#include "SSGameObject.h"
#include "SSCfgMgr.h"
#include "SSArea.h"
#include "SSBattleMgr.h"
#include "SSNPC.h"
#include "SSArea.h"
#include "SSAltarSev.h"
#include "SSOBSev.h"
#include "SSHero.h"
#include "SSBattle_Script.h"
 
#include "SSWorkThreadMgr.h"
#include "log/SSGameLogMgr.h"
#include "SSVMMgr.h"
#include "SSSolderSev.h"
 


namespace SceneServer{
 
CSSGameUnit* CSSBattle::CreateNPCAndAdd(CSSGameUnit *pcMasterObj, UINT32 eOT, const CVector3D & tarPos, INT64 summonEndTime){
	if(NULL == pcMasterObj){
		return NULL;
	}

	CVector3D newPos;
	if (FALSE == GetAroundFreePos(tarPos,newPos,pcMasterObj,pcMasterObj->GetGroupID())){
		ELOG(LOG_ERROR,"--shit, i can not find a free pos--");
		return NULL;
	}
	newPos.m_fY = tarPos.m_fY;
	return AddNPC((EObjectType)eOT, 0, newPos, pcMasterObj->GetCurDir(), pcMasterObj->GetCampID(), 0,summonEndTime); 
}
 
 
INT32	CSSBattle::DoNPCBuildDead(CSSNPC *pNPC , CSSGameUnit*pUnit){
	if (NULL == pNPC){
		return eEC_NullPointer;
	}
	//祭坛兵死亡 ，需要更新 该祭坛出兵数量
	if (pNPC->GetBelongAltar() > 0){
		m_cpAltarSev->MinusAltarSolderNumWhenDead(pNPC->GetBelongAltar());
		return eNormal;
	}  
	//祭坛死亡 需要清理
	if (pNPC->IfNPC_Building() && (pNPC->GetNPCCfg()->eNPCCateChild == eNPCChild_BUILD_Altar) ){ 
		m_cpAltarSev->RemoveAltarInfoWhenDead( pNPC->GetObjGUID());

		std::vector<INT32> tAltarIdx;
		INT32 pathId = CSSCfgMgr::GetSSCfgInstance().GetSSAltarBelongPathCfg(m_un32MapID,pNPC->GetObjGUID(),tAltarIdx);
		if (pathId > 0 && tAltarIdx.size() > 0)
		{
			//出超级兵提示
			this->BroadTipsByType(eTips_SuperNPCBorn); 

			m_SolderSev->SetAltarBrokenAboutSuperSolder(pathId,pNPC->GetObjGUID(),tAltarIdx);
		}
		else
		{
			ELOG(LOG_ERROR,"the error pathid:%d, of altarIdx:%ld",pathId,pNPC->GetObjGUID());
		}
	}
	return eNormal;
}
 
INT32	CSSBattle::OnBornNPCFunc(TIME_TICK tTickSpan)
{  
	bool bFlag = m_SolderSev->CheckBornSolder(tTickSpan);
	if (!bFlag)
	{
		return eNormal;
	}
	this->BroadTipsByType(eTips_NPCBorn); 
	{ 
 
		std::map<INT32/*pathid*/,SSolderGoWayPointCfg > tPathSolderCfg;
		std::map<INT32/*pathid*/,SMapSolderInfoCfg>    tMapSolderCfg; 
		 
		CSSCfgMgr::GetSSCfgInstance().GetSSSolderSearPathPointCfg(m_un32MapID,tPathSolderCfg);
		CSSCfgMgr::GetSSCfgInstance().GetSSSolderBornInfoCfg(m_un32MapID,tMapSolderCfg);  

		auto itr = tMapSolderCfg.begin();
		for (;itr != tMapSolderCfg.end();itr++)
		{ 
			INT32  pathId = itr->first;  
 
			SMapSolderInfoCfg &mSolderInfoCfg = itr->second;  
			 
			SMapCurtBornSolderInfo  curtBornInfo;
			std::map<INT32/*pathId*/, SMapCurtBornSolderInfo >  & curtSolderMap = m_SolderSev->GetMapCurtBornSolderInfo();
			auto itrChild = curtSolderMap.find(pathId);
			if (itrChild != curtSolderMap.end())
			{
				curtBornInfo = itrChild->second;
			}
			else
			{
				curtSolderMap[pathId] = curtBornInfo;
			} 

			bool	bFlag = false;
			INT32  objIdx = 0;
			if ((curtBornInfo.curtBornSolderTimes > 0) && (curtBornInfo.curtBornSolderTimes % mSolderInfoCfg.mChariotSolderCfg.charSolderBornTimes == 0))
			{
				objIdx = mSolderInfoCfg.mChariotSolderCfg.chariotSolderIdx;
				bFlag = true;
			}
			else
			{
			   objIdx = curtBornInfo.bSuperState ? mSolderInfoCfg.mSuperSolderCfg.superSolderIdx[curtBornInfo.curtBornSolderPos] : mSolderInfoCfg.solderIdxCfg[curtBornInfo.curtBornSolderPos];
			}
#if 1
			if (curtBornInfo.bSuperState)
			{
				ELOG(LOG_INFO,"super solder:%d",objIdx);
			}
#endif
			CSSNPC *pcGO = this->AddNPC((EObjectType)objIdx, 0,	mSolderInfoCfg.bornPosCfg, mSolderInfoCfg.bornDirCfg, (EGameObjectCamp)tPathSolderCfg.find(pathId)->second.camp, 0);

			if (NULL == pcGO){
				ELOG(LOG_ERROR,"samething error ,when born npc:%d,pos:%d,pathid:%d,mapId:%d ",objIdx,curtBornInfo.curtBornSolderPos,pathId,m_un32MapID);
				continue;
			}  

			//设置寻路路径 
			pcGO->SetOccupyPath( tPathSolderCfg.find(pathId)->second.vecPoint );   
			 
			if(bFlag)
			{
				curtBornInfo.curtBornSolderTimes = 0;

				m_SolderSev->SetNPCNextBornCDMisec(  mSolderInfoCfg.everyTimeSpace);

				curtSolderMap[pathId] = curtBornInfo;
				continue;
			} 
			curtBornInfo.curtBornSolderPos  += 1;

			if ( curtBornInfo.curtBornSolderPos >= mSolderInfoCfg.totalSolderCfg) 
			{
				m_SolderSev->SetNPCNextBornCDMisec(  mSolderInfoCfg.solderTimeSpaceCfg);
				curtBornInfo.curtBornSolderPos  = 0;
				 
				curtBornInfo.curtBornSolderTimes   +=  1; 
			}
			else
			{  
				m_SolderSev->SetNPCNextBornCDMisec(  mSolderInfoCfg.everyTimeSpace);
			} 

			curtSolderMap[pathId] = curtBornInfo; 
		} 
	} 
	return eNormal;
}

INT32	CSSBattle::SyncOBSystems(EOBSystemFunc eOb, const SGUID_KEY& guid, google::protobuf::Message& sMsg, int n32MsgID,CSSUser *piUser, void* p)
{  
	if (NULL == piUser){
		return eEC_NullUser;
	}

	if (eSSBS_Playing != m_eBattleState){
		return eEC_InvalidBattleState;
	}


	if (false == CSSCfgMgr::GetSSCfgInstance().getOBSystemSwitchCfgByMapId(this->GetBattleMapId()))	{
 		static INT64 nOB;
 		nOB++;
 		if (nOB < 3)
 		{
 			ELOG(LOG_WARNNING, "--OB system switch :0--");
 		} 
		return eEC_GuideNotOn;
	}	

	if (eOb == eOB_InitUserSeat)	{
		UINT8 un8SeatPos = (UINT8)p;
		return m_cpOBSev->AddObGUID( this, guid,un8SeatPos); 
	} 

	if (eOb == eOB_AddUser)	{
		if (NULL == piUser)		{
			return eEC_NullPointer;
		} 
		 
		AddISSUserToSight(piUser,eGOCamp_AllPeace,TRUE); 
		m_cpOBSev->AddObUser( this, piUser); 
		return eNormal;
	}
	 

	return eNormal;
} 

void	CSSBattle::InitBattleParamCfg()
{   
	m_SSVMMgr->SetWildMonsterDelayBorn(eEWM_low, CSSCfgMgr::GetSSCfgInstance().GetSSMapFuncDelayTimeCfg(eDelay_PrimaryMonster, m_un32MapID ));
	m_SSVMMgr->SetWildMonsterDelayBorn(eEWM_Middle, CSSCfgMgr::GetSSCfgInstance().GetSSMapFuncDelayTimeCfg(eDelay_HightMonster, m_un32MapID ));

	m_SolderSev->SetNPCNextBornCDMisec( CSSCfgMgr::GetSSCfgInstance().GetSSMapFuncDelayTimeCfg(eDelay_Solder, m_un32MapID ));
}
 

void	CSSBattle::BroadTipsByType(ETipsType et , INT32 campId)
{  
	bool bFlag = false;
	auto itr = m_mapTips.find(et);
	if (itr == m_mapTips.end())
	{
		m_mapTips[et] = 1;  
		bFlag = true;
	} 
	
	if (bFlag)
	{
		int  temp = 0;
		GSToGC::GameTips pMsg;
		pMsg.set_campid(campId);
		switch(et)
		{
		case	eTips_ObjAppear:
			{
				pMsg.set_errocode(eEC_TipsObjAppear);
			}break;
		case	eTips_NPCBorn: 
			{
				pMsg.set_errocode(eEC_TipsNPCBorn);
			}break; 
		case	eTips_SuperNPCBorn: 
			{
				pMsg.set_errocode(eEC_TipsSuperNPCBorn);  
			}
			break;
		}
		 
		this->SendMsgToAllWatcher(pMsg,pMsg.msgid());  
	}  
}

  

void	CSSBattle::FinishCallScript()
{
#if 0
	UINT32 aun32Para[2]={0};
	aun32Para[0] = this->GetBattleMapId();
	aun32Para[1] = this->GetBattleID();
	m_pCSSBattleMgr->GetBattleScript().ExecuteScriptFunc("onBattleFinish",aun32Para,2 );
#endif
}
 
INT32	CSSBattle::SendAltarIco(INT32 altarIdx,EObjectType eOT, EAltarBornNpcType etype )
{
	GSToGC::NotifyAltarBSIco pMsg; 
	pMsg.set_altarid(altarIdx);
	pMsg.set_objtype(eOT);
	pMsg.set_optype(etype);  
	 
	SendMsgToAllWatcher(pMsg, pMsg.msgid());
	 
	return eNormal;
}

 INT32		CSSBattle::SendMsgWhenAltarBornSolder(INT64 objIdx, INT32 eOt,INT32 camp,const CVector3D & bornPos)
 {
	 GSToGC::BornSoler pMsg;
	 pMsg.set_type(eOt);
	 pMsg.set_camp(camp); 

	 GSToGC::Pos  *pPos = new GSToGC::Pos();
	 pPos->set_x(bornPos.m_fX);
	 pPos->set_z(bornPos.m_fZ);
	 pMsg.set_allocated_pos(pPos); 

	 pMsg.set_guid(objIdx);  
	
	 SendMsgToAllWatcher(pMsg,pMsg.msgid());

	 return eNormal;
 }

 INT32	CSSBattle::CheckBattleFinish(CSSNPC* pNPC, CSSGameUnit *pcReasonGO)
 {
	 if (!pNPC->IfNPC_Building() || (pNPC->GetNPCCfg()->eNPCCateChild != eNPCChild_BUILD_Base))
	 {
		 return eNormal;
	 }

	 INT32 pathId = 0;
	 bool bFlag = CSSCfgMgr::GetSSCfgInstance().CheckObjIdIsBaseBuild(m_un32MapID,pNPC->GetObjGUID(),pathId);
	 if (bFlag && pathId > 0)
	 { 
		 SetCaseDeadReasonInfo(pNPC, pcReasonGO ); 

		 Finish(eBattleEnd_Normal,pcReasonGO->GetCampID()); 
	 }
	 return eNormal;
 }


}