// SSBattleMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "SSBattle.h"
#include "SSBattleMgr.h"
#include "SSHero.h"
#include <boost/algorithm/string/trim.hpp>
#include "boost/algorithm/string.hpp"
#include "SSUser.h"
#include "SSBattle_Script.h"
#include "SSGameUnit.h"

#include "log/SSGameLogMgr.h"
 

using namespace std;

namespace SceneServer{

INT32	CSSBattleMgr::OnMsgFromGC(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength, int n32MsgID){
	m_pMsgUser =  GetCUserByNetInfo(piGSInfo, n32GCNSID);
	if (NULL == m_pMsgUser){
		return eEC_NullUser;
	}
	m_pMsgUser->ResetPingTimer();
	m_pMsgBattle = NULL;
	INT32 nRet = eNormal;
	do 
	{
		GCMsgHandlerMap::iterator iter = m_asGCMsgHandler.find(n32MsgID);
		if (m_asGCMsgHandler.end() == iter){
			nRet = eEC_NullMsgHandler;
			break;
		}
		m_pMsgBattle = GetBattle(m_pMsgUser->GetBattleID());
		if (NULL == m_pMsgBattle){
			nRet = eEC_NullBattle;
			break;
		}
		nRet = (this->*iter->second)(piGSInfo, n32GCNSID, pData, n32DataLength);
	} while (0);

	if (nRet!=eNormal){
		PostMsgToGC_AskReturn(m_pMsgUser, n32MsgID, nRet);
	}
	return nRet;
}

INT32	CSSBattleMgr::OnMsgFromGC_AskEnterBattle(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::AskEnterBattle> pEnterBattle = ParseProtoMsgWithNewMemory<GCToSS::AskEnterBattle>(pData, n32DataLength);
	if (!pEnterBattle) return eEC_ParseProtoError;
	CSSBattle *pcBattle = GetBattle(pEnterBattle->battleid());
	if (m_pMsgBattle != pcBattle) return eEC_InvalidBattleID;
	return pcBattle->PostMsgToGC_EnterBattleState(m_pMsgUser,pEnterBattle->clienttime());
}

INT32	CSSBattleMgr::OnMsgFromGC_AskTrySelectHero(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::TrySeleceHero> pMsg = ParseProtoMsgWithNewMemory<GCToSS::TrySeleceHero>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	ELOG(LOG_SpecialDebug,"玩家(%s)AskTrySelectHero",m_pMsgUser->GetNickName());
	return m_pMsgBattle->AskTrySelectHero(m_pMsgUser, pMsg->heroid());
}

INT32	CSSBattleMgr::OnMsgFromGC_AskSeleteHero(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::SelectHero> pMsg = ParseProtoMsgWithNewMemory<GCToSS::SelectHero>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	ELOG(LOG_SpecialDebug,"玩家(%s)AskSeleteHero",m_pMsgUser->GetNickName());
	 m_pMsgBattle->AskSelectHero(m_pMsgUser, pMsg->heroid());
	//log
	 if(!m_pMsgBattle->IsGuideBattle()){
		 stringstream mystream;
		 mystream << m_pMsgUser->GetGUID() << LOG_SIGN;
		 mystream << pMsg->heroid() << LOG_SIGN;
		 CSSGameLogMgr::GetInstance().AddBattleLog(eLog_ChoseHero,m_pMsgBattle->GetBattleID(),m_pMsgBattle->GetBattleMapId(),mystream.str());
	 }

	return eNormal;
}

INT32	CSSBattleMgr::OnMsgFromGC_AskSeleteRunePage(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::SelectRunePage> pMsg = ParseProtoMsgWithNewMemory<GCToSS::SelectRunePage>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	ELOG(LOG_SpecialDebug,"玩家(%s)AskSeleteRunePage",m_pMsgUser->GetNickName());
	 m_pMsgBattle->AskSelectRunePage(m_pMsgUser, pMsg->pageindex());
	
	 //log
	 if(!m_pMsgBattle->IsGuideBattle()){
		 stringstream mystream;
		 mystream << m_pMsgUser->GetGUID() << LOG_SIGN;
		 mystream << pMsg->pageindex() << LOG_SIGN;
		  CSSGameLogMgr::GetInstance().AddBattleLog(eLog_ChoseRune,m_pMsgBattle->GetBattleID(),m_pMsgBattle->GetBattleMapId(),mystream.str());
	 } 

	 return eNormal;
}

INT32	CSSBattleMgr::OnMsgFromGC_LoadBattleComplete(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	ELOG(LOG_SpecialDebug,"玩家(%s)AskLoadBattleComplete",m_pMsgUser->GetNickName());
	SBattleUserInfo *pInfo = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser);
	if(NULL == pInfo) return eEC_UnKnownError;
	pInfo->bIfLoadComplete = TRUE;
	if (m_pMsgBattle->GetBattleState()==eSSBS_Playing){
		ELOG(LOG_SpecialDebug,"服务器已进入战场，玩家(%s)走重练流程",m_pMsgUser->GetNickName());
		m_pMsgBattle->UserReconnectToBattleSS(m_pMsgUser); 
	}
	return eNormal;
}

INT32	CSSBattleMgr::OnMsgFromGC_AskMoveDir(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::MoveDir> pMoveDir = ParseProtoMsgWithNewMemory<GCToSS::MoveDir>(pData, n32DataLength);
	if (!pMoveDir) return eEC_ParseProtoError;
	CVector3D pcDir(pMoveDir->dir().x(), 0, pMoveDir->dir().z());
	return m_pMsgBattle->AskMoveDir(m_pMsgUser, pcDir);
}

INT32	CSSBattleMgr::OnMsgFromGC_AskStopMove(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	return m_pMsgBattle->AskStopMove(m_pMsgUser);
}

INT32 CSSBattleMgr::OnMsgFromGC_AskBuyGoods(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CSSHero* pHero = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser)->pcHero;
	if (NULL == pHero) return eEC_HasNoTheHero;
	boost::shared_ptr<GCToSS::BuyGoods> pMsg = ParseProtoMsgWithNewMemory<GCToSS::BuyGoods>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	
	INT32  goodsId =0;
	INT32 nret = pHero->AskBuyGoods(pMsg->typeid_(),goodsId);
	//log
	if(goodsId > 0 && !pHero->IfAI() && m_pMsgBattle->IsGuideBattle()){
		stringstream mystream;
		mystream << ((GetUTCMiliSecond() - m_pMsgBattle->GetBattleStateStartTime())/1000)<<LOG_SIGN;
		mystream << m_pMsgUser->GetGUID()<<LOG_SIGN;
		mystream << pHero->GetObjType()<<LOG_SIGN;
		mystream<<goodsId << LOG_SIGN; 
		CSSGameLogMgr::GetInstance().AddBattleLog(eLog_BattleBuy,m_pMsgBattle->GetBattleID(),m_pMsgBattle->GetBattleMapId(),mystream.str());
	}
	return nret;
}

INT32 CSSBattleMgr::OnMsgFromGC_AskSellGoods(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CSSHero* pHero = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser)->pcHero;
	if (NULL == pHero) return eEC_HasNoTheHero;
	boost::shared_ptr<GCToSS::SellGoods> pMsg = ParseProtoMsgWithNewMemory<GCToSS::SellGoods>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	return pHero->AskSelGoods(pMsg->goodspos());
}

INT32 CSSBattleMgr::OnMsgFromGC_AskMoveGoods(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CSSHero* pHero = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser)->pcHero;
	if (NULL == pHero) return eEC_HasNoTheHero;
	boost::shared_ptr<GCToSS::MoveGoods> pMsg = ParseProtoMsgWithNewMemory<GCToSS::MoveGoods>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	return pHero->AskMoveGoods(pMsg->frompos(), pMsg->topos());
}

INT32 CSSBattleMgr::OnMsgFromGC_AskUseGoods(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CSSHero* pHero = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser)->pcHero;
	if (NULL == pHero) return eEC_HasNoTheHero;
	boost::shared_ptr<GCToSS::UseGoods> pMsg = ParseProtoMsgWithNewMemory<GCToSS::UseGoods>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	return pHero->AskUseGoods(pMsg->goodspos());
}

INT32	CSSBattleMgr::OnMsgFromGC_AskUseSkill(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::UseSkill> pUseSkill = ParseProtoMsgWithNewMemory<GCToSS::UseSkill>(pData, n32DataLength);
	if (!pUseSkill) return eEC_ParseProtoError;
	INT32 nret = m_pMsgBattle->AskUseSkill(m_pMsgUser, pUseSkill->skillid());
	
	if(m_pMsgUser && m_pMsgUser->GetHero() && !m_pMsgUser->GetHero()->IfAI() && !m_pMsgBattle->IsGuideBattle()){//log
		stringstream mystream;
		mystream << ( (GetUTCMiliSecond() - m_pMsgBattle->GetBattleStateStartTime()) / 1000 )<<LOG_SIGN;
		mystream<<m_pMsgUser->GetGUID()<<LOG_SIGN;
		mystream<<m_pMsgUser->GetHero()->GetObjType()<<LOG_SIGN;
		mystream<<m_pMsgUser->GetHero()->GetLevel()<<LOG_SIGN;
		mystream<<pUseSkill->skillid();
		CSSGameLogMgr::GetInstance().AddBattleLog(eLog_BattleSkill,m_pMsgBattle->GetBattleID(),m_pMsgBattle->GetBattleMapId(),mystream.str());
	}
	return nret;
}

INT32	CSSBattleMgr::OnMsgFromGC_AskLockTarget(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::LockTar> pLockTar = ParseProtoMsgWithNewMemory<GCToSS::LockTar>(pData, n32DataLength);
	if (!pLockTar) return eEC_ParseProtoError;
	SGUID_KEY sTarGUID(pLockTar->guid());
	return m_pMsgBattle->AskLockTar(m_pMsgUser, sTarGUID);
}

INT32	CSSBattleMgr::OnMsgFromGC_ReportAltarStrawSolder(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::ReportAltarStrawSolder> pMsg = ParseProtoMsgWithNewMemory<GCToSS::ReportAltarStrawSolder>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	INT32 nret = m_pMsgBattle->ReportStrawSolder(m_pMsgUser, pMsg->index(), (EObjectType)pMsg->type());
	 
	//log
	if(m_pMsgUser->GetHero() && !m_pMsgUser->GetHero()->IfAI() && !m_pMsgBattle->IsGuideBattle()){
		stringstream mystream;
		mystream<< ((GetUTCMiliSecond() - m_pMsgBattle->GetBattleStateStartTime()) / 1000) <<LOG_SIGN;
		mystream<<m_pMsgUser->GetHero()->GetUser()->GetGUID() <<LOG_SIGN;
		mystream<<m_pMsgUser->GetHero()->GetObjType()<<LOG_SIGN;
		mystream<<m_pMsgUser->GetHero()->GetLevel()<<LOG_SIGN;
		mystream<< pMsg->index()<<LOG_SIGN;
		mystream<<pMsg->type();
		CSSGameLogMgr::GetInstance().AddBattleLog(eLog_BattleAltarSolder,m_pMsgBattle->GetBattleID(),m_pMsgBattle->GetBattleMapId(),mystream.str());
	} 
	return nret;
}

INT32	CSSBattleMgr::OnMsgFromGC_AskGasExplosion(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	return m_pMsgBattle->AskGasExplosion(m_pMsgUser);
}

INT32 CSSBattleMgr::OnMsgFromGC_AskAbsorbMonster(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::Absorb> pAbsorb = ParseProtoMsgWithNewMemory<GCToSS::Absorb>(pData, n32DataLength);
	if (!pAbsorb) return eEC_ParseProtoError;
	INT32 nret = m_pMsgBattle->AskAbsorbMonster(m_pMsgUser, pAbsorb->removeid());
	
	return nret;
}

INT32 CSSBattleMgr::OnMsgFromGC_AskBuyReborn(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){

	boost::shared_ptr<GCToSS::BuReborn> pBuReborn = ParseProtoMsgWithNewMemory<GCToSS::BuReborn>(pData, n32DataLength);
	if (!pBuReborn){
		return eEC_ParseProtoError;
	}

	CSSHero* TempHero = m_pMsgUser->GetHero();
	if (NULL == TempHero){
		return eEC_NullHero;
	}

	return m_pMsgBattle->AskBuyReborn(TempHero);
}
 

INT32	CSSBattleMgr::OnMsgFromGC_AskBornNPC(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::AskBornNPC> pBornNPC = ParseProtoMsgWithNewMemory<GCToSS::AskBornNPC>(pData, n32DataLength);
	if (!pBornNPC) return eEC_ParseProtoError;
#if 0
	m_pMsgBattle->DoGuideTaskStep(eGuideType_BronNPC, pBornNPC->stepid(),0,0,pBornNPC->state());
#endif
	return eNormal;
}

INT32 CSSBattleMgr::OnMsgFromGC_AskAlwaysAutoAtk(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::AlwaysAutoAtk> pMsg = ParseProtoMsgWithNewMemory<GCToSS::AlwaysAutoAtk>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	CSSHero* pHero = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser)->pcHero;
	if (NULL == pHero) return eEC_HasNoTheHero;
	pHero->SetAlwaysAutoAtk(pMsg->flag());
	return eNormal;
}

INT32	CSSBattleMgr::OnMsgFromGC_AskAutoAttack(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CSSHero* pHero = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser)->pcHero;
	if (NULL == pHero) return eEC_HasNoTheHero;
	return pHero->AskStartAutoAttack();
}

INT32 CSSBattleMgr::OnMsgFromGC_AskMoveTar(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToSS::MoveTar> pMoveTar = ParseProtoMsgWithNewMemory<GCToSS::MoveTar>(pData, n32DataLength);
	if (!pMoveTar) return eEC_ParseProtoError;
	CVector3D sPos(pMoveTar->pos().x(), 0, pMoveTar->pos().z());
	return m_pMsgBattle->AskMoveTar(m_pMsgUser, sPos);
}

INT32	CSSBattleMgr::OnMsgFromGC_AskQuickBattleEnd(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pMsg, int n32MsgLength){
	boost::shared_ptr<GCToSS::AskQuickBattleEnd> pAskBattleEnd = ParseProtoMsgWithNewMemory<GCToSS::AskQuickBattleEnd>(pMsg, n32MsgLength);
	if (!pAskBattleEnd) return eEC_ParseProtoError;
	CSSHero* pHero = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser)->pcHero;
	if (NULL == pHero) return eEC_HasNoTheHero;
#if 0
	if (!CSSCfgMgr::GetSSCfgInstance().getMapGuideSwitch(pcBattle->GetBattleMapId())){
		return eEC_GuideNotOn;
	}
#endif
	return m_pMsgBattle->Finish(eBattleEnd_Guide_AskGoHall, pHero->GetCampID());
}

INT32	CSSBattleMgr::OnMsgFromGC_AskHerosBattleInfo(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pMsg, int n32MsgLength)
{
	boost::shared_ptr<GCToSS::HerosBattleInfo> pAskBattleEnd = ParseProtoMsgWithNewMemory<GCToSS::HerosBattleInfo>(pMsg, n32MsgLength);
	if (!pAskBattleEnd){
		return eEC_ParseProtoError;
	}

	return m_pMsgBattle->AskGetHerosBattleInfo(m_pMsgUser);
}

INT32	CSSBattleMgr::OnMsgFromGC_AskHeroAttributes(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pMsg, int n32MsgLength)
{
	boost::shared_ptr<GCToSS::AskHeroAttributesInfo> pAskBattleEnd = ParseProtoMsg<GCToSS::AskHeroAttributesInfo>(pMsg, n32MsgLength);
	if (!pAskBattleEnd){
		return eEC_ParseProtoError;
	}

	CSSHero* pcHero = m_pMsgUser->GetHero();
	if (NULL == pcHero){
		return eEC_NullHero;
	}

	return pcHero->AskHeroAttributes();
}

}
