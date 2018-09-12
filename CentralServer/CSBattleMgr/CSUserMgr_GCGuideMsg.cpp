// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
 
 #include "CSBattleMgr.h"
#include "CSUser.h"
#include "CSUserMgr.h"
#include "CSKernel.h"
#include  "CSGuideBattle.h"
#include "../ELogging/ELogging/SSActive.h"
#include "CSCfgMgr.h"
#include "CSGameLogMgr.h"

using namespace std;

namespace CentralServer{

	INT32	CCSBattleMgr::OnMsgFromGC_AskGuideBattle(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
		CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
		boost::shared_ptr<GCToCS::AskCSCreateGuideBattle> pMsg = ParseProtoMsg<GCToCS::AskCSCreateGuideBattle>(pData, n32DataLength);
		if (!pMsg){
			return 0;
		}
		INT32 n32AskRet  = AskCreateGuideBattle(piUser, pMsg->mapid(),pMsg->ntype() );
		if (eNormal != n32AskRet){
			piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskGuideBattle , n32AskRet);
		}

		CSSGameLogMgr::GetInstance().AddGameLog(eLog_Guide,piUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,pMsg->mapid());
		return eNormal;
	}
	 


	INT32	CCSUserMgr::OnMsgFromGC_AskCompCSGuideStep(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
	{
		CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID); 
		boost::shared_ptr<GCToCS::GuideCSStepComp> pMsg = ParseProtoMsg<GCToCS::GuideCSStepComp>(pData, n32DataLength);
		if (!pMsg){
			return 0;
		}  
 		 
		bool bFlag = pcUser->GetUserDBData().mGuideSteps.UpdateGuideInfo(pMsg->guidepart(), pMsg->bcomp()); 
		if (bFlag)
		{
			pcUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskGuideBattle , eEc_ExistGuideTaskId);
			return eEc_ExistGuideTaskId;
		}

		return eNormal;
	}



	INT32 CCSBattleMgr::AskCreateGuideBattle( CCSUser* piUser, INT32 mapId, INT32 ntype )
	{
		set<UINT32>& lMapCfdMap = CCSCfgMgr::getInstance().GetMapCfgMap();
		set<UINT32>::iterator mapIter = lMapCfdMap.find(mapId);
		if (lMapCfdMap.end() == mapIter){
			return eEC_InvalidMapID;
		}

		if(piUser->GetUserBattleInfoEx().GetBattleID() > 0)
		{
			ELOG(LOG_ERROR, "U has in battle:%d",piUser->GetUserBattleInfoEx().GetBattleID());
			return eEC_JustInBattle;
		}

		SMapLogicCfg* pLogicCfg = CCSCfgMgr::getInstance().GetMapLogicCfg(mapId);
		if (pLogicCfg==NULL){
			return eEC_InvalidMapID;
		}
		ICSSSInfo* piSSInfo = GetCSKernelInstance()->GetMiniBattleNumSS();
		if (NULL == piSSInfo){
			return eEC_NullSceneServer;
		}
		if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Free){
			return eEC_JustInBattle;
		}

		CCSUserListMap pListMap;
		pListMap[1].AddElement(piUser); 
 
		if (ntype == eGuideMapType_First)
		{
			piUser->GetUserBattleInfoEx().ChangeTypeWithState(eBattleType_Guide1,eBattleState_Wait);
			CCSGuideBattle* pBattle = new CCSGuideBattle(eMatchType_None, eBattleType_Guide1, piSSInfo->GetSSID(), GenerateBattleID(), mapId, pListMap);
			m_cAllBattleMap.insert(make_pair(pBattle->GetBattleId(),pBattle));
			pBattle->Start();
		}
		else if (ntype == eGuideMapType_Second) 
		{
			map<UINT32,UINT32> aiRobots;  
			UINT32  userCamp = 1; 
			for (int i=1; i<=pLogicCfg->PlayerModel.size(); ++i){ 
				INT32 totalAi = 0;
				if (i == userCamp)
				{
					totalAi = 1;
				} 
				while (totalAi < pLogicCfg->PlayerModel[i-1]){
					aiRobots.insert(make_pair(i+totalAi*pLogicCfg->PlayerModel.size(),i));
					totalAi++;
				}
			}
			piUser->GetUserBattleInfoEx().ChangeTypeWithState(eBattleType_Guide2,eBattleState_Wait);
			CCSBattle* pBattle = new CCSBattle(eMatchType_None, eBattleType_Guide2, piSSInfo->GetSSID(), GenerateBattleID(), mapId, pListMap, &aiRobots);
			m_cAllBattleMap.insert(make_pair(pBattle->GetBattleId(),pBattle));
			pBattle->Start();
		}
		else
		{
			ELOG(LOG_ERROR,"the error guide maptype:%d",ntype);
			Assert(false);
			return eEC_InvalidMapID;
		}
		
		piSSInfo->AddBattleNum(1); 

		return eNormal;
	}


	INT32	CCSUserMgr::OnMsgFromSS_AskUpdateGuideStep(const ICSSSInfo *piSSInfo, const char* pData, int n32DataLength){
 
		return eNormal;
	}
}