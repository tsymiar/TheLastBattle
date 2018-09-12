#include "stdafx.h"
#include "CSGuideBattle.h"
#include "CSBattleMgr.h" 
#include "CSCfgMgr.h"
#include "CSUser.h"
#include "CSUserMgr.h"
#include "CSGameLogMgr.h" 

#include "CSKernel.h"


namespace CentralServer{
	CCSGuideBattle::CCSGuideBattle(EBattleMatchType mtype, BattleType type,UINT32 ssId,UINT64 battleId,UINT32 mapId, CCSUserListMap& pCCSUserList, map<UINT32,UINT32>* aiRobots )
		:CCSBattle(mtype,type,ssId,battleId,mapId,pCCSUserList,aiRobots) 
	{
		
	}
	CCSGuideBattle::~CCSGuideBattle(void)
	{}

	void	CCSGuideBattle::ReCreateRoom(){}
	void	CCSGuideBattle::ReCreateMatch(){}
	void	CCSGuideBattle::AddBattleHeroAndOtherList(CCSUser* pUser,CSToSS::CreateBattle_CreateBattleUserInfo* pUserInfo) {}

	//战斗结束，给予奖励， 不需要结算界面。
	//并且直接跳转到购买英雄商城
	void	CCSGuideBattle::CaculateResult(SSToCS::FinishBattle* pFinishMsg)
	{ 	 
		GSToGC::GuideAward aw;
		bool bFlag = false;

		for (auto it= this->GetUserMap().begin(); it!= this->GetUserMap().end(); ++it){
			CCSUser* pUser   = it->second;
			if (NULL != pUser  )
			{
				CSGuideAward award;

				CCSCfgMgr::getInstance().GetGuideAwardInfo( this->GetBattleMapId(), award);
				if(pUser->GetUserDBData().mGuideSteps.CheckGuideTaskId(award.taskId))
				{
					ELOG(LOG_WARNNING,"there exist taskId:%d, or  has guide end!",award.taskId);
					continue;
				}  
				if (award.gold > 0)
				{
					pUser->GetUserDBData().ChangeUserDbData(eUserDBType_Gold, award.gold);
					pUser->SynCurGold();

					auto itr = aw.add_awardlist();
					itr->set_dt( GSToGC::GuideAward::dtype::GuideAward_dtype_type_gold);
					itr->set_idxn( award.gold);
					bFlag = true;
				}
				if ( award.diamond > 0)
				{
					pUser->GetUserDBData().ChangeUserDbData(eUserDBType_Diamond, award.diamond); 
					pUser->SynCurDiamond();

					auto itr = aw.add_awardlist();
					itr->set_dt( GSToGC::GuideAward::dtype::GuideAward_dtype_type_diamond);
					itr->set_idxn( award.diamond);

					bFlag = true;
				}
				if (award.exp > 0)
				{
					pUser->CheckUpgrade(award.exp);
					pUser->SynUserLvInfo();
				}
				if (award.heroid > 0)
				{
					pUser->AddHero(award.heroid,true); 

					auto itr = aw.add_awardlist();
					itr->set_dt( GSToGC::GuideAward::dtype::GuideAward_dtype_type_goodsid);
					itr->set_idxn( award.heroid);

					bFlag = true;
				} 
				if (award.runeid > 0)
				{
					pUser->AddRunes(award.runeid, true);

					auto itr = aw.add_awardlist();
					itr->set_dt( GSToGC::GuideAward::dtype::GuideAward_dtype_type_goodsid);
					itr->set_idxn( award.heroid);

					bFlag = true;
				}
				if (award.taskId > 0)
				{//完成的任务
					bool bFlag = pUser->GetUserDBData().mGuideSteps.UpdateGuideInfo(award.taskId,false);
					if (bFlag)
					{
						ELOG(LOG_ERROR,"taskid has exist:%d",award.taskId);
						continue;
					}
					pUser->PosUserCSCurtGuideSteps();
				} 
				GetCSUserMgrInstance()->DBPoster_UpdateUser(pUser);//存盘// 
				if (bFlag)
				{
					pUser->PostMsgToGC(aw,aw.msgid()); 
				} 
			}
		}  
	}
}