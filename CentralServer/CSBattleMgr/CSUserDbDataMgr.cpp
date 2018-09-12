
 
#include "stdafx.h" 

#include "CSUserDbDataMgr.h"
#include "CSUserMgr.h"


namespace CentralServer{

void CCSUserDbDataMgr::UpdateUserDbData(SUserDBData & userdb, std::stringstream &mystream)  
{  	 
	mystream.clear();
	mystream.str("");

	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_Channel])
	{
		CheckStreamAndInit(mystream);

		mystream << "sdk_id = " << userdb.sPODUsrDBData.eUserPlatform;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_Channel] = false;
	} 
 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_NickName])
	{
		CheckStreamAndInit(mystream);
		
		mystream << "obj_name = '" << userdb.szNickName<<"'";
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_NickName] = false;
	} 

	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_Sex])
	{	
		CheckStreamAndInit(mystream);

		mystream << " obj_sex = " << (INT32)userdb.sPODUsrDBData.n16Sex;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_Sex] = false;
	}
		 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_UserLv])
	{
		CheckStreamAndInit(mystream);		

		mystream << " obj_lv = " << (INT32)userdb.sPODUsrDBData.un8UserLv;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_UserLv] = false;
	}  
		 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_Score])
	{
		CheckStreamAndInit(mystream);		

		mystream << " obj_score = " << userdb.sPODUsrDBData.n64Score;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_Score] = false;
	} 
		 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_HeaderId])
	{
		CheckStreamAndInit(mystream);

		mystream << " obj_headid = " << userdb.sPODUsrDBData.un16HeaderID;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_HeaderId] = false;
	} 
		 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_Diamond])
	{
		CheckStreamAndInit(mystream);

		mystream << " obj_diamond = " << userdb.sPODUsrDBData.n64Diamond;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_Diamond] = false;
	} 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_Gold])
	{
		CheckStreamAndInit(mystream);

		mystream << " obj_gold = " << userdb.sPODUsrDBData.n64Gold;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_Gold] = false;
	}  
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_RegisterTime])
	{
		CheckStreamAndInit(mystream);

		mystream << "obj_register_time = " << userdb.sPODUsrDBData.tRegisteUTCMillisec;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_RegisterTime] = false;
	}  
	 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_TotalWinInns])
	{
		CheckStreamAndInit(mystream);

		mystream << " obj_game_winns = " << userdb.sPODUsrDBData.un32TotalWinInns;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_TotalWinInns] = false;
	} 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_TotalHeroKills])
	{
		CheckStreamAndInit(mystream);

		mystream << " obj_kill_hero_num = " << userdb.sPODUsrDBData.un32TotalHeroKills;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_TotalHeroKills] = false;
	} 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_TotalAssist])
	{
		CheckStreamAndInit(mystream);

		mystream << " obj_ass_kill_num = " << userdb.sPODUsrDBData.un32TotalAssist;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_TotalAssist] = false;
	} 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_TotalDestoryBuild])
	{
		CheckStreamAndInit(mystream);

		mystream << " obj_dest_building_num = " << userdb.sPODUsrDBData.un32TotalDestoryBuildings;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_TotalDestoryBuild] = false;
	}
		 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_TotalDeadTimes])
	{
		CheckStreamAndInit(mystream);

		mystream << " obj_dead_num = " << userdb.sPODUsrDBData.un32TotalDeadTimes;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_TotalDeadTimes] = false;
	}
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_LastFirstWinTime])
	{
		CheckStreamAndInit(mystream);

		mystream << "obj_first_win_time = " << userdb.sPODUsrDBData.tLastFirstWinTime;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_LastFirstWinTime] = false;
	} 
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_UserLvExp])
	{
		CheckStreamAndInit(mystream);

		mystream << "obj_cur_lv_exp = " << userdb.sPODUsrDBData.un32UserCurLvExp;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_UserLvExp] = false;
	}

	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_CLDay])
	{
		CheckStreamAndInit(mystream);

		mystream << "obj_cldays = " << (INT32)userdb.sPODUsrDBData.un16Cldays;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_CLDay] = false;
	}
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_LastGetLoginReward])
	{
		CheckStreamAndInit(mystream);

		mystream << "obj_last_loginreward_time = " << userdb.sPODUsrDBData.un32LastGetLoginRewardDay;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_LastGetLoginReward] = false;
	}
	if (userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_VIPLevel])
	{
		CheckStreamAndInit(mystream);

		mystream << "obj_vip_lv = " << (INT32)userdb.sPODUsrDBData.un16VipLv;
		userdb.sPODUsrDBData.m_IfChangeArr[eUserDBType_VIPLevel] = false;
	}

	if (userdb.isTaskRush)
	{
		CheckStreamAndInit(mystream);
		mystream << "obj_task_data = '" << userdb.szTaskData << "'";
		userdb.isTaskRush = false;
	}

	if (!mystream.str().empty())
	{
		mystream << " where obj_id= " << userdb.sPODUsrDBData.un64ObjIdx; 
		mystream <<";"; 
	}

	userdb.mGuideSteps.GetCurtGuideInfo(mystream, userdb.sPODUsrDBData.un64ObjIdx);
}

void CCSUserDbDataMgr::GetNewUserDbData(SUserDBData & userdb,std::string &strSql)
{
	stringstream mystream;
	mystream.str("");
	mystream.clear();

	mystream << "INSERT INTO GameUser(un64ObjIdx, eUserPlatform,szUserName,szNickName,n8Sex,un16UserLv,n64Score,n32HeadId,n64Diamond,n64Gold,tRegisteUTCMillsec,"
		"un32TotalGameInns,un32TotalWinInns,un32TotalHeroKills,"
		"un32TotalDestoryBuildings,  un32TotalDeadTimes,  un32UserCurLvExp, "
		"un16Cldays,tLastGetLoginReward,un16VipLv) values(";
	mystream << userdb.sPODUsrDBData.un64ObjIdx	<< ",";
	mystream << userdb.sPODUsrDBData.eUserPlatform	<< ",'"; 
	mystream << userdb.szUserName	<< "','"; 
	mystream << userdb.szNickName	<< "',"; 
	mystream << (INT32)userdb.sPODUsrDBData.n16Sex	<< ","; 
	mystream << (INT32)userdb.sPODUsrDBData.un8UserLv	<< ",";  
	mystream << userdb.sPODUsrDBData.n64Score	<< ",";   
	mystream << userdb.sPODUsrDBData.un16HeaderID	<< ",";    
	mystream << userdb.sPODUsrDBData.n64Diamond	<< ",";    
	mystream << userdb.sPODUsrDBData.n64Gold	<< ",";    
	mystream << userdb.sPODUsrDBData.tRegisteUTCMillisec	<< ",";     
	mystream << userdb.sPODUsrDBData.un32TotalGameInns	<< ",";     
	mystream << userdb.sPODUsrDBData.un32TotalWinInns << ",";      
	mystream << userdb.sPODUsrDBData.un32TotalHeroKills	<< ",";      
	mystream << userdb.sPODUsrDBData.un32TotalDestoryBuildings	<< ",";     
	mystream << userdb.sPODUsrDBData.un32TotalDeadTimes	<< ",";     
	mystream << userdb.sPODUsrDBData.un32UserCurLvExp	<< ",'";     
	mystream << (INT32)userdb.sPODUsrDBData.un16Cldays	<< ",'"; 
	mystream << userdb.sPODUsrDBData.un32LastGetLoginRewardDay	<< "',";     
	mystream << (INT32)userdb.sPODUsrDBData.un16VipLv	<< ");";  

	strSql = mystream.str(); 
}

//void CCSUserDbDataMgr::GetUserDbGuideStepsData(  SUserDBData& userdb ,  std::string &strSql)
//{   
//	std::string strGuide("");
//	userdb.sPODUsrDBData.mGuideSteps.GetCurtGuideInfo(strGuide);
//
//	if (strGuide.size() > 0)
//	{
//		stringstream mystream;
//		mystream << "update gameuser_guide set obj_cs_guide_com_steps = '" <<  strGuide << "'" << " where obj_id = " << userdb.sPODUsrDBData.un64ObjIdx << ";";
//		strSql = strSql + mystream.str();   
//	}  
//}
void CCSUserDbDataMgr::GetNewUserDbHerosData(  UINT64	un64ObjIdx, const SUserHeroDBData &pDb, std::string &strSql)
{ 
	stringstream instream; 
	instream << "insert INTO gameuser_hero(user_id,hero_id,hero_end_time,hero_buy_time) values(";  
	instream << un64ObjIdx;
	instream << ",";
	instream <<  pDb.un32HeroID;
	instream << ",";
	instream <<  pDb.endTime;
	instream << ",";
	instream <<  pDb.buyTime;
	instream << ");"; 
	strSql.append(instream.str());
}

void CCSUserDbDataMgr::GetNewUserDbHerosData(UINT64 un64ObjIdx, const vector<SUserHeroDBData> &db, std::string &strSql){
	for (auto iter = db.begin(); iter != db.end(); ++iter){
		GetNewUserDbHerosData(un64ObjIdx, *iter, strSql);
	}
}
 

void CCSUserDbDataMgr::GetDelUserDbHerosSql(UINT64 un64ObjIdx, UINT32 commodityID, std::string &strSql){
	stringstream delstream; 
	delstream << "delete from gameuser_hero where hero_id= ";
	delstream << commodityID;
	delstream << " and user_id=";
	delstream << un64ObjIdx;
	delstream <<";";
	strSql.append(delstream.str());
}

void CCSUserDbDataMgr::InsertOrUpdateRunesBagData(UINT64 un64ObjIdx, const string& bagSql, std::string &strSql){
	stringstream mystream;
	mystream <<"update gameuser_runne set runnebag_json=" << bagSql << " where user_id=" << un64ObjIdx << ";";
	strSql += mystream.str();
}

void CCSUserDbDataMgr::InsertOrUpdateRunesBagDataAndToDBThread(UINT64 un64ObjIdx, const string& bagSql){
	string sql;
	InsertOrUpdateRunesBagData(un64ObjIdx, bagSql, sql);
	GetCSUserMgrInstance()->PostUserCacheMsgToDBThread(un64ObjIdx, sql);
}

void CCSUserDbDataMgr::InsertOrUpdateRunesSlotData(UINT64 un64ObjIdx, const string& bagSql, std::string &strSql){
	stringstream mystream;
	mystream <<"update gameuser_runne set runeslot_json=" << bagSql << " where user_id=" << un64ObjIdx << ";";
	strSql += mystream.str();
}

void CCSUserDbDataMgr::InsertOrUpdateRunesSlotDataAndToDBThread(UINT64 un64ObjIdx, const string& bagSql){
	string sql;
	InsertOrUpdateRunesSlotData(un64ObjIdx, bagSql, sql);
	GetCSUserMgrInstance()->PostUserCacheMsgToDBThread(un64ObjIdx, sql);
}

void CCSUserDbDataMgr::InsertOrUpdateAllRunesDataAndToDBThread(UINT64 un64ObjIdx, const string& bagstr, const string& slotstr){
	stringstream mystream;
	mystream <<"update gameuser_runne set runnebag_json=" << bagstr << ", runeslot_json = " << slotstr << " where user_id=" << un64ObjIdx << ";";
	GetCSUserMgrInstance()->PostUserCacheMsgToDBThread(un64ObjIdx, mystream.str());
}

void CCSUserDbDataMgr::GetUpdateDbHeroData(const UINT64 userid, const UINT32 heroid, const INT64 endtime, std::string &strSql)
{
	stringstream mystr;
	mystr<<"update gameuser_hero set hero_end_time=";
	mystr<<endtime;
	mystr<<" where user_id=";
	mystr<<userid;
	mystr<<" and hero_id=";
	mystr<<heroid;
	mystr<<";";
	strSql.append(mystr.str());
}

void CCSUserDbDataMgr::CheckStreamAndInit(std::stringstream &mystream){
	if (!mystream.str().empty())
	{
		mystream <<",";
	}
	else
	{
		mystream << "update gameuser set ";
	}
}

}