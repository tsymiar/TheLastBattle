#include "../stdafx.h"

#include "../SSCfgMgr.h"
#include <string>
#include "SSGameLogMgr.h"
#include "../SSWorkThreadMgr.h"
#include "CommLog.h" 
namespace SceneServer{


CSSGameLogMgr::CSSGameLogMgr() 
{ 
	mLastRefshTime = GetCurrentTime();
	mSkillHurt.clear(); 
}
CSSGameLogMgr::~CSSGameLogMgr()
{
	
}

void	CSSGameLogMgr::AddSkillHurt( INT64 mapid ,INT64 bid,INT64 userid, INT32 heroId ,INT32 skillId, INT32 hurtValue, EEffectCate ec)
{
	SkillHurt sh;
	sh.heroid = heroId;
	if (ec ==  eEffectCate_PhyHurt)
	{
		sh.phyHurt +=hurtValue;
	}
	else if (ec ==  eEffectCate_MagicHurt)
	{
		sh.magHurt +=hurtValue;
	}  
	else
	{
		sh.realHurt +=hurtValue;
	}
	sh.num =1;

 
	auto itr = mSkillHurt.find(mapid);
	if (itr != mSkillHurt.end())
	{
		std::map<INT64/*battleid*/,std::map<INT64/*userid*/,std::map<INT32/*skillId*/,SkillHurt> > > &tmap = itr->second;
		auto itrChild  = tmap.find(bid);
		if (itrChild != tmap.end())
		{
			std::map<INT64/*userid*/,std::map<INT32/*skillId*/,SkillHurt> > &thirdMap = itrChild->second;
			auto irtThird = thirdMap.find(userid);
			if (irtThird != thirdMap.end())
			{
				std::map<INT32,SkillHurt> &forMap = irtThird->second;
				auto itrFour = forMap.find(skillId);
				if (itrFour != forMap.end())
				{
					itrFour->second.phyHurt +=sh.phyHurt; 
					itrFour->second.magHurt +=sh.magHurt;
					itrFour->second.magHurt +=sh.magHurt;
					itrFour->second.num +=sh.num;
				}
				else
				{ 
					forMap[skillId] = sh;
				}
			}
			else
			{
				std::map<INT32,SkillHurt>  forMap;
				forMap[skillId] = sh;
				thirdMap[userid] = forMap;
			}
		}
		else
		{
			std::map<INT32,SkillHurt>  forMap;
			forMap[skillId] = sh;
			std::map<INT32/*user*/,std::map<INT32,SkillHurt> > mymap;
			mymap[userid] = forMap; 
		}
	}
	else
	{
		std::map<INT32,SkillHurt>  forMap;
		forMap[skillId] = sh;
		std::map<INT64/*userid*/,std::map<INT32/*skillId*/,SkillHurt> > mymap;
		mymap[userid] = forMap; 
		std::map<INT64/*battleid*/,std::map<INT64/*userid*/,std::map<INT32/*skillId*/,SkillHurt> > > fmap;
		fmap [bid] = mymap;
		mSkillHurt[mapid]= fmap;
	}
} 
void	CSSGameLogMgr::PosBattleLogs(EGameLog eLog, std::string & logStr)
{
	ToLog::WriteLog sLog;
	sLog.set_log_type(eLog ); 
	sLog.set_log_str(logStr);	

	CSSWorkThreadMgr::GetInstance().PostMsgToLogServer(sLog, sLog.msgid());  
}
void	CSSGameLogMgr::AddBattleLog(EGameLog eLog, INT32 bid,INT32 mapid, INT32 param)
{
	stringstream mystream;
	 GetGameLogHead(  eLog,   mystream, mapid,bid); 
	  
	mystream<<param; 
	mystream <<"');";
	PosBattleLogs(eLog,mystream.str());
}
void	CSSGameLogMgr::AddBattleLog(EGameLog eLog, INT32 bid,INT32 mapid, std::string & log )
{
	stringstream mystream;
	GetGameLogHead(  eLog,   mystream,mapid,bid ); 
	 
	mystream<<log; 
	mystream <<"');";
	PosBattleLogs(eLog,mystream.str());
}
 

void		CSSGameLogMgr::RefreshLog(INT64 mapid ,INT64 bid, INT64 bstarttime, INT64  curt)
{
	if(mLastRefshTime < curt)
	{
		RefreshGameLog(  mapid ,  bid,   bstarttime);

		INT32  ncfg =  CSSCfgMgr::GetSSCfgInstance().m_FuncSwitch.logRefreshTime;
		if (ncfg < 1)
		{
			ncfg = 120000;
		}
		mLastRefshTime = curt +ncfg;
	}
}
void		CSSGameLogMgr::RefreshGameLog(INT64 mapid ,INT64 bid, INT64 bstarttime)
{
	stringstream mystream; 
	auto itr = mSkillHurt.find(mapid);
	if ( itr != mSkillHurt.end() )
	{ 
		GetGameLogHead(  eLog_BattleSkillHurt,   mystream, mapid,bid);  
		mystream<<((GetUTCMiliSecond() - bstarttime))/1000<<LOG_SIGN;

		std::map<INT64/*battleid*/,std::map<INT64/*userid*/,std::map<INT32/*skillId*/,SkillHurt> > > &tmap = itr->second;
		auto itrChilde = tmap.find(bid);
		if ( itrChilde != tmap.end() )
		{ 
			std::map<INT64/*userid*/,std::map<INT32/*skillId*/,SkillHurt> >  &thirdMap = itrChilde->second; 

			auto itrThird = thirdMap.begin();
			for (;itrThird != thirdMap.end(); itrThird++)
			{
				mystream << itrThird->first << LOG_SIGN_FIRST;  //ÓÃ»§id
				std::map<INT32,SkillHurt> & vecSkill = itrThird->second;

				auto itrFour = vecSkill.begin();
				mystream << itrFour->second.heroid<< LOG_SIGN;  //Ó¢ÐÛid
				
				INT32  total = vecSkill.size();
				INT32  cpos = 0;
				for (;itrFour != vecSkill.end(); itrFour++)
				{
					mystream  << itrFour->first   << LOG_SIGN_FIRST; 
					mystream << itrFour->second.phyHurt << LOG_SIGN_FIRST;
					mystream << itrFour->second.magHurt << LOG_SIGN_FIRST;
					mystream << itrFour->second.realHurt << LOG_SIGN_FIRST;
					mystream << itrFour->second.num; 

					if (cpos + 1 < total)
					{
						mystream << LOG_SIGN_SECOND;
					}
					cpos++;
				} 
				mystream <<"');";
				PosBattleLogs(eLog_BattleSkillHurt, mystream.str()); 
				vecSkill.clear();
			} 
			thirdMap.clear();

			tmap.erase(itrChilde);
		} 
	} 
}
}