#include "stdafx.h"
#include "SSBattle.h" 
#include "SSHero.h" 
#include "SSUser.h"

#include "log/SSGameLogMgr.h"
 
namespace SceneServer{

 void		CSSBattle::GetBattleHeroLog()
 {
	 stringstream mystream;  
	 mystream << (m_tBattleStateTime/1000) << LOG_SIGN;
	 for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		 if(m_asBattleUserInfo[i].piUser == NULL) continue;
		 if(m_asBattleUserInfo[i].pcHero == NULL ||  m_asBattleUserInfo[i].pcHero->IfAI()) continue; 
		 mystream << m_asBattleUserInfo[i].piUser->GetGUID() << LOG_SIGN_FIRST;
		 mystream << m_asBattleUserInfo[i].pcHero->GetObjType() << LOG_SIGN_FIRST;
		 mystream << m_asBattleUserInfo[i].pcHero->GetCampID();

		 if (i+1 < m_BattleHero)
		 {
			 mystream << LOG_SIGN_SECOND;
		 } 
	 }
	 CSSGameLogMgr::GetInstance().AddBattleLog(IsGuideBattle() ? eLog_BattleGuideStart :  eLog_BattleStart,this->GetBattleID(),this->GetBattleMapId(),mystream.str());
 }
 void		CSSBattle::GetBattleEndLog(INT32  campId)
 {
	 stringstream mystream;
	 { 
		 mystream<< ( m_tBattlePlayTime/1000) <<LOG_SIGN;
		 mystream<<campId<< LOG_SIGN; 
	  
		 INT32 BattleHeroPos = 0;
		 for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
			 if(m_asBattleUserInfo[i].piUser == NULL) continue;
			 if(m_asBattleUserInfo[i].pcHero == NULL ||   m_asBattleUserInfo[i].pcHero->IfAI()) continue;
				 mystream << m_asBattleUserInfo[i].piUser->GetGUID()<<LOG_SIGN_FIRST;
				 mystream << m_asBattleUserInfo[i].pcHero->GetCampID()<<LOG_SIGN_FIRST;
				mystream << m_asBattleUserInfo[i].pcHero->GetObjType()<<LOG_SIGN_FIRST;
				mystream <<m_asBattleUserInfo[i].pcHero->GetLevel()<<LOG_SIGN_FIRST;
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32HeroKills<<LOG_SIGN_FIRST; 
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32DeadTimes<<LOG_SIGN_FIRST;
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32DestoryBuildings<<LOG_SIGN_FIRST;
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32Assist<<LOG_SIGN_FIRST;
				mystream << ((m_asBattleUserInfo[i].pcHero != NULL ) ? m_asBattleUserInfo[i].pcHero->GetTotalPersonCP() : 0)<<LOG_SIGN_FIRST;
				mystream <<m_asBattleUserInfo[i].piUser->GetDiamondUsed()<<LOG_SIGN_FIRST;
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32RealSendHurt<<LOG_SIGN_FIRST;
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32SkillPhySendHurt<<LOG_SIGN_FIRST;
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32SkillMagicSendHurt<<LOG_SIGN_FIRST;  
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32RevRealHurt<<LOG_SIGN_FIRST; 
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32RevSkillPhyHurt<<LOG_SIGN_FIRST; 
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32RevSkillMagicHurt<<LOG_SIGN_FIRST;  
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32KillSolder<<LOG_SIGN_FIRST; 
				mystream <<m_asBattleUserInfo[i].pcHero->GetSSendData().un32KillMonster;
				if (++BattleHeroPos==m_BattleHero) continue;
				mystream <<  LOG_SIGN_SECOND;
		 } 
	 } 
	 CSSGameLogMgr::GetInstance().AddBattleLog(IsGuideBattle() ? eLog_BattleGuideEnd : eLog_BattleEnd,this->GetBattleID(),this->GetBattleMapId(), mystream.str());

	 CSSGameLogMgr::GetInstance().RefreshGameLog( m_un64BattleID, m_un32MapID, GetBattleStateStartTime()) ;
 }
}