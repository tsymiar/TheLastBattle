#include "stdafx.h"

#include "SSGuideBattle.h"
#include "SSBattleMgr.h"
#include "SSGameObject.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSNPC.h" 
#include "SSCfgMgr.h"
#include "SSPassiveSkillMgr.h" 
#include "SSWorkThreadMgr.h" 
#include "log/SSGameLogMgr.h"
#include "SSUser.h"
#include "SSVMMgr.h"
#include "SSAltarSev.h"
#include "SSGUParameterMgr.h"
#include "Tools.h"
static UINT64 startguid = ((UINT64)1)<<48;
//const INT32  guideHeroId = 50000;
const INT32  guideobjIdx =  60000;
namespace SceneServer{  
	CSSGuideBattle::CSSGuideBattle(void):m_ObjIdx(guideobjIdx)/*,aiHeroId(0)*/,m_AiHeroNum(0){
		m_NoHurtHero.clear();
	} 
	CSSGuideBattle::~CSSGuideBattle(void){

	}

	CSSGuideBattle::CSSGuideBattle(UINT64 bid, EBattleType btype, CSSBattleMgr* pOwer) 
		:CSSBattle(pOwer,btype,bid) 
	{
		m_ObjIdx = guideobjIdx;
		m_AiHeroNum = 1;
		//aiHeroId = guideHeroId;
		m_ObjCompTask.clear();
		m_CurtBornSolder.clear();
		m_CurtBuildBeHurt.clear();
		this->SetBattleState(eSSBS_Loading,FALSE); 

		m_BrokenAltarId.clear();
	}

	void	CSSGuideBattle::CheckSelectHeroTimeout(INT64 tCurUTCMilsec)
	{

	}

	void	CSSGuideBattle::CheckSelectRuneTimeout(INT64 tCurUTCMilsec)
	{

	}
	//bflag: false检查，true删除
	bool	CSSGuideBattle::CheckGuideHeroNoHurt(CSSGameUnit* beHurtObj, bool bFlag)
	{
		if (beHurtObj->IfHero()  )
		{
			CSSHero *pHero = (CSSHero*)beHurtObj;
			if (pHero == NULL || pHero->IfAI())
			{
				return  false;
			}
			auto itr =	m_NoHurtHero.find(pHero->GetObjGUID());
			if (itr != m_NoHurtHero.end())
			{ 
					return true; 
			}
		}
		return false;
	}
	//新手在指定步骤之前攻击建筑有限制
	bool	CSSGuideBattle::CheckObjCanBeHurt(CSSGameUnit* beHurtObj, CSSGameUnit *atkObj)
	{
		assert(NULL == beHurtObj);
		assert(NULL == atkObj);
		if (beHurtObj->IsDead())
		{
			return true;
		}
		//判断新手英雄是否处于无敌状态
		if(CheckGuideHeroNoHurt(beHurtObj, false))
		{
			return false;
		}
		//柱子
		if (atkObj->IfNPC() && atkObj->GetFatherHeroGUID() && beHurtObj->IfNPC_Building() && m_CurtBuildBeHurt.size() < 1)
		{
			return false;
		}
		if (atkObj->IfNPC_Building())
		{
			return true;
		} 
		if(  beHurtObj->IfNPC_Building())
		{  
			auto itr =	m_CurtBuildBeHurt.begin();
			for(; itr != m_CurtBuildBeHurt.end(); itr++)
			{
				//祭坛
				//pAltar->GetNPCCfg()->eNPCCateChild == eNPCChild_BUILD_Altar
				if (itr->second.bBeHurt && (itr->second.cfg.objIdxCfg == beHurtObj->GetObjType()) &&  ((CSSNPC*)beHurtObj)->GetNPCCfg()->eNPCCateChild == eNPCChild_BUILD_Altar) 
				{
					return true;
				}
				//主基地
				if (itr->second.bBeHurt && (itr->second.cfg.objIdxCfg == beHurtObj->GetObjType()) &&  ((CSSNPC*)beHurtObj)->GetNPCCfg()->eNPCCateChild == eNPCChild_BUILD_Base)  
				{
					return true;
				}
				//箭塔
				if (itr->second.bBeHurt && (itr->second.cfg.objIdxCfg == beHurtObj->GetObjType()) && atkObj->IfHero())
				{
					return true;
				} 
			}  
			return false;
		} 

		if (atkObj->IfHero() && !((CSSHero*)atkObj)->IfAI())
		{
			return true;
		}
		return true;
	}

	void CSSGuideBattle::AddCompGuide(INT64 objIdx,  INT32 childType, INT32 stepId)
	{  
		SGuideBattleCompTask ct;
		ct.stepId = stepId;
		ct.childType = (EChildTaskType)childType; 

		auto itr = m_ObjCompTask.find(objIdx);
		if (itr != m_ObjCompTask.end())
		{  
			itr->second.push_back(ct);
		}
		else
		{ 
			std::vector<SGuideBattleCompTask> vec;
			vec.push_back(ct);
			m_ObjCompTask[objIdx] = vec;
		} 
		//检查是否需要关闭出兵
		CheckStopBornSolder(  stepId);
		//检查是否允许祭坛掉血
		CheckAndSetAltarBeHurt(stepId);
	}

	void	CSSGuideBattle::CheckAndSetAltarBeHurt(INT32 stepId)
	{
		SGuideHurtBuildCfg cfg;
		CSSCfgMgr::GetSSCfgInstance().GetGuideHurtBuildCfg(this->GetBattleMapId(),stepId,  cfg);
		if (cfg.objIdxCfg > 0)
		{
			auto itr = 		m_CurtBuildBeHurt.find(stepId);
			if (itr == m_CurtBuildBeHurt.end())
			{
				SGuideCurtBuildBeHurt bbh;
				bbh.bBeHurt = true;
				bbh.cfg = cfg;

				m_CurtBuildBeHurt[stepId] = bbh;
			}
			else
			{
				ELOG(LOG_ERROR,"there exist the same taskid:%d",stepId);
			}
		}
	}

	void	 CSSGuideBattle::CheckBornSolderOrHero(CSSHero* pHero,  boost::shared_ptr<GCToSS::AskSSGuideStepComp> pMsg )
	{  
		for (int i=0; i < pMsg->info_size() ; i++)
		{   
			INT32 dtype = pMsg->info(i).dtype();
			INT32 stepId = pMsg->info(i).stepid();

			switch(dtype)
			{
			case eDotype_NextStep:
				{
					//检查是否需要关闭出兵
					CheckStopBornSolder(  stepId);
				}break;
			case eDotype_CompTask://, //完成任务
				{
					AddCompGuide(pHero->GetUser()->GetGUID(),0,stepId);
				}break;
			case		eDotype_BuyGoods://, //购买
				{

				}break;
			case		eDotype_BornSolder://,//出兵
				{
					SGuideSolderCfg cfg;
					CSSCfgMgr::GetSSCfgInstance().GetGuideSolderCfg(this->GetBattleMapId(),stepId,   cfg);
					if (cfg.vecObjIdCfg.size() < 1)
					{
						return;
					}
					AddGuideNpc(stepId,cfg);
				}break;
			case		eDotype_BornHero://,//出英雄
				{
					SGuideHeroCfg cfg;
					CSSCfgMgr::GetSSCfgInstance().GetGuideHeroCfg(this->GetBattleMapId(),stepId, cfg);
					//出生英雄
					if (cfg.objIdxCfg < 1)
					{
						ELOG(LOG_ERROR,"there is no cfg of born hero:%d",stepId);
						return;
					}

					ELOG(LOG_INFO,"==start  born hero:%d,taskid:%d",cfg.objIdxCfg,stepId);

					AddGuideHero(pHero,cfg);
					if (cfg.ifRage)
					{
						pHero->ChangeFuryValue(eFuryAdd, 1000);  //出生AI英雄后将把新手英雄曝气 
						//设置 曝气到击杀英雄这段时间为无敌状态
						auto itr = m_NoHurtHero.find(pHero->GetObjGUID());
						if (itr == m_NoHurtHero.end())
						{
							m_NoHurtHero[pHero->GetObjGUID()] = true;
						}
					} 
				}break;
			default:
				{
					ELOG(LOG_ERROR,"error,the erro dtype:%d,taskId:%d",dtype,stepId);
				}
				break;
			}
		} 

	}
	 
	void	CSSGuideBattle::AddGuideHero(CSSHero* pHero, const SGuideHeroCfg & cfg)
	{
		assert(pHero); 
		CVector3D bornPos;
		CVector3D bornDir; 
		SHeroBornPos  hbp;

		bool bFlag = CSSCfgMgr::GetSSCfgInstance().getHeroBornCfg(GetBattleMapId(), cfg.campIdCfg, hbp);
		if (!bFlag)
		{
			ELOG(LOG_ERROR,"shit, i can not get cfg,mapId:%d, campId:%d",GetBattleMapId(),cfg.campIdCfg);
			return;
		}
		if  (cfg.bornTypeCfg != eHeroBornType_Pos) 
		{
			bornPos = pHero->GetCurPos();
			float bornAngle = ((m_AiHeroNum/2)%3) * (3.1415926f * 2 / 3); //n*(&*2/3)
			CVector3D bornMoveVec(cosf(bornAngle),0,sinf(bornAngle));
			bornMoveVec = bornMoveVec * 150; 
			bornPos = bornPos + bornMoveVec;
		}
		else
		{
			bornPos =  hbp.bPos;
		} 
		bornDir = (cfg.bornTypeCfg == eHeroBornType_Pos) ? hbp.bPosDir  : pHero->GetCurDir() ;  
		//Assert(bornPos.IsZero() == false);
		if(bornDir.IsZero())
		{
			bornDir.m_fX = 1;
			bornDir.m_fY = 0;
			bornDir.m_fZ = -0.1;
		} 

		CSSHero* aiHero = AddHero((EObjectType)cfg.objIdxCfg, bornPos, bornDir, NULL, ++startguid, (EGameObjectCamp)cfg.campIdCfg, 0,TRUE);
		if (aiHero == NULL)
		{
			ELOG(LOG_ERROR,"shit ,i can not create ai hero:%d",cfg.objIdxCfg);
			return;
		}
		 //设置复活时间  
		aiHero->SetHeroBornPos(hbp.bRelivePos); 
		aiHero->GetParaMgr()->AddPercentValue(eEffectCate_ReliveTime,cfg.reliveTimeCfg);  
	}

	void	CSSGuideBattle::SetCaseDeadReasonInfo(CSSGameUnit* deadObj, CSSGameUnit *pcReasonGO)
	{
		if (  deadObj->IfHero()&&pcReasonGO->IfHero()  ||   deadObj->IfNPC_Building()  ) 
		{ 
			SBattleUserInfo *pInfo =  GetBattleUserInfo();
			if (NULL == pInfo)
			{
				ELOG(LOG_ERROR,"===NULL point!===");
				return;
			}

			GSToGC::GuideKillsInfo pMsg;
			pMsg.set_guid(pcReasonGO->GetObjGUID());	//击杀者
			pMsg.set_npctype(deadObj->GetObjType());  //击杀英雄，击杀祭坛 主基地

			bool bFlag = false;
			for (int i=0; i< c_un32MaxUserInBattle; i++)
			{ 
				if (NULL != pInfo[i].piUser)
				{
					bFlag = true;
					CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(pInfo->piUser->GetUserNetInfo(),pMsg,pMsg.msgid());
					ELOG(LOG_WARNNING,"guide GSToGC::GuideKillsInfo==>battleId:%d, guid:%d, type:%d==",this->GetBattleID(),pcReasonGO->GetObjGUID(), deadObj->GetObjType());
				} 
			} 
			if (!bFlag)
			{
				ELOG(LOG_WARNNING,"guide shit , i can not find one user!==>battleId:%d, guid:%d, type:%d==",this->GetBattleID(),pcReasonGO->GetObjGUID(), deadObj->GetObjType());
			}

			//出超级兵
			if(deadObj->IfNPC_Building() && ((CSSNPC*)deadObj)->GetNPCCfg()->eNPCCateChild == eNPCChild_BUILD_Altar)
			{ 
				return;
				m_BrokenAltarId.push_back( deadObj ->GetObjGUID() );
				CheckBornSuperSolder(deadObj->GetCampID());
			} 
		}  
	}

	void	CSSGuideBattle::CheckBornSuperSolder(INT32 campid)
	{
		std::map<INT32,SMapSolderInfoCfg> solderCfg; 
		CSSCfgMgr::GetSSCfgInstance().GetSSSolderBornInfoCfg(this->GetBattleMapId(),solderCfg);  
		auto  itr = solderCfg.begin();
		for(; itr != solderCfg.end(); itr++)
		{
			SMapSolderInfoCfg &scfg = itr->second;
			
			std::vector<INT32>  & vecAltar = scfg.mSuperSolderCfg.altarIdx;
			 
			bool bFlag = Comp2Vector(vecAltar,m_BrokenAltarId);
			if(bFlag){
				auto itr  =		m_CurtBornSolder.begin();
				for (; itr != m_CurtBornSolder.end(); itr++)
				{
					SGuideCurtBornSolder & scbs = itr->second;
					if (scbs.cfg.campidCfg == campid)
					{
						scbs.cfg.vecObjIdCfg.clear();
						scbs.cfg.vecObjIdCfg = scfg.mSuperSolderCfg.superSolderIdx;
						return;
					}
				}
			}
		}
	}

	void		CSSGuideBattle::AddGuideNpc(INT32 stepid,  const SGuideSolderCfg & cfg)
	{ 
		SGuideCurtBornSolder cbs;
		cbs.cfg = cfg;
		auto itr = m_CurtBornSolder.find(stepid);
		if (itr == m_CurtBornSolder.end() )
		{
			m_CurtBornSolder[stepid] = cbs;
		}else
		{
			ELOG(LOG_ERROR,"exist stepid:%d",stepid);
		}
	} 

	INT32		CSSGuideBattle::OnBornNPCFunc(INT64 tTickSpan)
	{  
		auto itr  =		m_CurtBornSolder.begin();
		for (; itr != m_CurtBornSolder.end(); itr++)
		{
			SGuideCurtBornSolder & scbs = itr->second;
			if (scbs.nextBornTime > tTickSpan)
			{
				continue;
			}
			//开始出兵
			INT32 solderIdx = scbs.cfg.vecObjIdCfg[scbs.curtBornSolderPos];

			CSSNPC *pcGO = this->AddNPC((EObjectType)solderIdx, 0,	scbs.cfg.bornPosCfg , scbs.cfg.bornDirCfg, (EGameObjectCamp)scbs.cfg.campidCfg, 0); 
			if (NULL == pcGO){
				ELOG(LOG_ERROR,"samething error ,when born npc:%d,pos:%d, mapId:%d ",solderIdx,scbs.curtBornSolderPos, this->GetBattleMapId());
				continue;
			} 
			if (scbs.cfg.campidCfg != eGOCamp_AllEnemy)
			{
				//设置寻路路径
				pcGO->SetOccupyPath( scbs.cfg.mGoPathPointCfg  ); 
			} else
			{ 
				pcGO->SetWMBornPos(pcGO->GetCurPos());
				pcGO->SetBelongPoint(solderIdx);  //没有配置所属点，这里暂时以配置id作为野怪出生点 
				GetVMBornMgr()->AddCurtWildMonsterBornInfo(solderIdx,solderIdx,  scbs.cfg.bornSpaceTimeCfg);
			} 

			scbs.curtBornSolderPos +=1;
			if (scbs.curtBornSolderPos >= scbs.cfg.vecObjIdCfg.size())
			{
				scbs.curtBornSolderPos = 0;
				scbs.nextBornTime = tTickSpan + scbs.cfg.bornAheadTimeCfg;
			} 
			else
			{
				scbs.nextBornTime = tTickSpan + scbs.cfg.bornSpaceTimeCfg;
			} 
		}

		return  eNormal;
	} 

	//关闭任务出兵
	void		CSSGuideBattle::CheckStopBornSolder( INT32 stepId)
	{
		auto itr =	m_CurtBornSolder.begin(); 
		for (;itr != m_CurtBornSolder.end();itr++)
		{
			if (itr->second.cfg.closeTaskIdCfg == stepId)
			{
				m_CurtBornSolder.erase(itr);
				break;
			} 
		}
	}

	void	CSSGuideBattle::OnBattleFinish()
	{ 
		this->Finish(eBattleEnd_Guide,1);
	}
	void	CSSGuideBattle::ResetAIAtOnUserOffline(CSSHero* pHero)
	{
	}

	void	CSSGuideBattle::OnBattleStateChange()
	{
		this->SetBattleState(this->GetBattleState(), TRUE);
	}

	//祭坛刷兵
	INT32 	CSSGuideBattle::DoAltarBornMonster(TIME_MILSEC tUTCMilsec)
	{ 
		std::vector<CVector3D> vecPoint;

		std::map<INT32/*altar id */, CurtAltarBornSolder>	& altarMap =  GetSSAltarSev()->GetCurtAltarInfo();
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

			vecPoint.clear();
		 
			CSSCfgMgr::GetSSCfgInstance().GetGuideSolderPathPointCfg(GetBattleMapId(),itr->first, vecPoint);
			if (vecPoint.size() < 1)
			{
				ELOG(LOG_ERROR,"there is no path! altarId;%d",itr->first);
			}
			for(int i=0; i < itr->second.mCfg.bornNum; i++)
			{
				CSSNPC *pNpc =  AddNPC((EObjectType)itr->second.monsterId, 0, itr->second.mCfg.bornPos, itr->second.mCfg.bornDir, (EGameObjectCamp)itr->second.campId, itr->first );
				if (NULL != pNpc  ){  

					SendMsgWhenAltarBornSolder(pNpc->GetObjType() ,itr->second.monsterId,itr->second.campId,itr->second.mCfg.bornPos);

					 GetSSAltarSev()->UpdateAltarBornSolder( itr->first, tUTCMilsec + itr->second.mCfg.bornSolderSpaceTime); 

					pNpc->SetOccupyPath( vecPoint);
				} 
			} 
		} 
		return eNormal;
	}

	void	CSSGuideBattle::DelHeroNoHurtStateAtDead(CSSHero* obj)
	{
		//std::map<INT64/*heroId*/,bool /*无敌英雄*/>										m_NoHurtHero;
		if (NULL != obj && !obj->IfAI() && !obj->IsDead() )
		{ 
			auto itr = m_NoHurtHero.find(obj->GetObjGUID());
			if (itr != m_NoHurtHero.end())
			{
				m_NoHurtHero.erase(itr);
			} 
		}
	}

	bool CSSGuideBattle::CheckPlayTimeout(TIME_MILSEC tCurUTCMilsec){
		bool bAllUserOffLine = true;
		for (INT32 i = 0; i < c_un32MaxUserInBattle; i++){
			CSSUser *piTempUser = m_asBattleUserInfo[i].piUser;
			if (NULL != piTempUser && TRUE == piTempUser->IfConnect()){
				bAllUserOffLine = false;
				break;
			}
		}

		//如果所有用户离线，并且超出保护时间，结束战斗
		if(bAllUserOffLine)
		{
			ELOG(LOG_WARNNING,  "all users offline, so finish it.");
			Finish(eBattleEnd_Guide, 0);
			return true;
		}

		return false;
	}

}