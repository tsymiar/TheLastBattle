// SSBattleMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "SSBattleMgr.h"
#include "SSGuideBattle.h" 
#include "SSHero.h"
#include <boost/algorithm/string/trim.hpp>
#include "boost/algorithm/string.hpp"
#include "SSUser.h" 
#include "SSGameUnit.h"
#include "log/SSGameLogMgr.h"
 
namespace SceneServer{
	 

	INT32	CSSBattleMgr::OnMsgFromGC_CompGuideStep(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
		CSSHero* pHero = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser)->pcHero;
		if (NULL == pHero) return eEC_NullPointer;
		boost::shared_ptr<GCToSS::StepID> pStepID = ParseProtoMsgWithNewMemory<GCToSS::StepID>(pData, n32DataLength);
		if (!pStepID) return eEC_ParseProtoError;  
		return eNormal;
	}


	INT32		CSSBattleMgr::OnMsgFromGC_AskSSGuideStepComp(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
	{
		CSSHero* pHero = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser)->pcHero;
		if (NULL == pHero) return eEC_NullPointer;
		boost::shared_ptr<GCToSS::AskSSGuideStepComp> pMsg = ParseProtoMsgWithNewMemory<GCToSS::AskSSGuideStepComp>(pData, n32DataLength);
		if (!pMsg) return eEC_ParseProtoError;
		 
		CSSGuideBattle *pBattle =  dynamic_cast<CSSGuideBattle*>(pHero->GetCurBattle());
		if (NULL == pBattle)
		{
			ELOG(LOG_ERROR,"hero have not belong battle! objId:%ld, heroId:%d,  taskSize:%d, type:%d, taskid:%d ",
				m_pMsgUser->GetGUID(), pHero->GetHeroID(), pMsg->info_size(),pMsg->info(0).dtype(),pMsg->info(0).stepid());
			return eEC_NullPointer;
		}
		if (pBattle->GetBattleType() != eBattleType_Guide1 )
		{
			ELOG(LOG_ERROR,"the error battletype %d! ",  pBattle->GetBattleType());
			return eEC_NullPointer;
		}

		pBattle->CheckBornSolderOrHero(pHero,pMsg); 

		//log
		if(!pHero->IfAI()){
			stringstream  mystream;
			mystream << ((GetUTCMiliSecond() - m_pMsgBattle->GetBattleStateStartTime()) / 1000) << LOG_SIGN;
			mystream << m_pMsgUser->GetGUID() << LOG_SIGN;
			for (int i=0; i < pMsg->info_size() ; i++)
			{  
				mystream << pMsg->info(i).stepid() << LOG_SIGN_FIRST;
			}
			CSSGameLogMgr::GetInstance().AddBattleLog(eLog_BattleGuideCompStep,m_pMsgBattle->GetBattleID(),m_pMsgBattle->GetBattleMapId(),mystream.str());
		}

		return eNormal;
	}
}
