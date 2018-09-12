////////////////////////////////////游戏日志//////////////////////////////////////
#include "CommLog.h"
#include <time.h>
  
#define  INSERT_SQL_HEAD "insert into game_log(log_type,log_time,log_str) values("
const char* 	GetGameLogHeadByType(EGameLog elog)  
{ 
	switch(elog)
	{
	case eLog_Register:	
		return "Register";
	case  eLog_HeadUse:	
		return "HeadUse";
	case 	 eLog_Login:
		return "Login";
	case 		 eLog_Logout:	
		return "Logout"; 
	case 	 eLog_Guide:	
		return "Guide";
	case 		 eLog_Daily:
		return "DailyAward";
	case 	 eLog_Exchange:
		return "ExchangeCode";
	case 	 eLog_ChangeName:	
		return "ChangeName";
	case 	 eLog_CreateMatch:	
		return "CreateMatch";
	case 	 eLog_LogoutMatch:
		return "LogoutMatch";
	case 	 eLog_MatchEnd:	
		return "MatchEnd";
	case 	 eLog_CreateHouse:	
		return "CreateHouse";
	case 	 eLog_AddHouse:	
		return "AddHouse";
	case 	 eLog_HouseStart:
		return "HouseStart";
		//buy
	case 	 eLog_BuyHouse:	
		return "BuyHouse";
	case 	 eLog_BuySkin:	
		return "BuySkin";
	case 	 eLog_BuyRune:	
		return "BuyRune";
	case 	 eLog_BuyGoods:	
		return "BuyGoods";
		//use
	case 	 eLog_RuneUse:	
		return "RuneUse";
	case eLog_RuneUnUse:	
		return "RuneUnUse";
	case 	 eLog_RuneCompose:	
		return "RuneCompose";
	case 	 eLog_RuneWashing:
		return "RuneWashing";
		//friend
	case 	 eLog_FriendAdd:
		return "FirendAdd";
	case 	 eLog_FriendDel:	
		return "FriendDel";
	case 	 eLog_EnemyFriendAdd:
		return "EnemyFriendAdd";
	case 	 eLog_EnemyFriendDel:
		return "EnemyFriendDel";
		//个人数据
	case 	 eLog_ChangeUseName:	
		return "ChagneUseName";
	case 	 eLog_UseInfoLogout:
		return "UseInfoLogout";
		//chat
	case 	 eLog_Chat:		
		return "Chat";
		//UIEvent
	case 	 eLog_UIEvent:	
		return "UIEvent";
		//select hero
	case 	 eLog_ChoseHero:	
		return "ChoseHero";
	case	eLog_ChoseRune:
		return "ChoseRune";
	case eLog_ChoseSkin:
		return "ChoseSkin"; 
		//掉线
	case 	 eLog_UserDiscon:
		return "UserDiscon";
	case 	 eLog_UserRecon:
		return "UserRecon";
	case 	 eLog_KickUser:	
		return "KickUser";

		//battle data 
	case 	 eLog_BattleStart:	
		return "BattleStart";
	case 	 eLog_BattleBuy:	
		return "BattleBuy";
		//skill 71-80
	case 	 eLog_BattleSkill:	
		return "BattleSkill";
 
	case 	 eLog_BattleSkillEquip:
		return "BattleSkillEquip";

		//kill
	case 	 eLog_BattleKillSolder:
		return "BattleKillSolder";
	case 	 eLog_BattleKillMonster:
		return "BattleKillMonster";
	case 	 eLog_BattleKillHero:
		return "BattleKillHero";
	case 	 eLog_BattleKillBuild:
		return "BatlleKillBuild";
	case 	 eLog_BattleStrawMonster:
		return "BattleStrawMonster";
	case 	 eLog_BattleAltarSolder:
		return "BattleAltarSolder";
	case 	 eLog_BattleHeroMounKill:
		return "BattleHeroMounKill"; 
		//disc
	case 	 eLog_BattleRecon:
		return "BattleRecon";
	case 	 eLog_BattleUserOffLine:
		return "BattleUserOffLine"; 
		//other
	case 	 eLog_BattleEnd:
		return "BattleEnd";
	case 	 eLog_BattleSelGoods:
		return "BattleSelGoods";
	case eLog_BattleSkillHurt:
		return "BattleSkillHurt";
	case eLog_BattleExpersion:
		return "BattleExpersion";
	case eLog_BattleGuideStart:
		return "BattleGuideStart";
	case eLog_BattleGuideCompStep:
		return "BattleGuideCompStep";
	case eLog_BattleGuideEnd:
		return "BattleGuideEnd";
	default:
		return "Other";
	}
}
 

void   GetGameLogHead(EGameLog eLog, stringstream  & mystream,INT32 param1, INT64 param2)
{
	mystream<< INSERT_SQL_HEAD;
	mystream <<eLog;
	mystream << ",'";
	mystream << CFunction::GetCurtFormatTime(time(NULL));
	mystream<<"','";
	mystream<< GetGameLogHeadByType(eLog)<<LOG_SIGN; 
	if (param1 > 0)
	{
		mystream<<param1<<LOG_SIGN;
	}
	if (param2 > 0)
	{
		mystream<<param2<<LOG_SIGN;
	} 
}
