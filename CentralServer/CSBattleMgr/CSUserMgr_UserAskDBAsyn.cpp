#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSUserMgr.h"
#include "CSUser.h"

#include "../ELogging/ELogging/SSActive.h"
#include "..\..\CommonCPlus\DBMgr.h"
#include "CSKernel.h"
using namespace std;

namespace CentralServer{
void CCSUserMgr::UserAskDBAsynHandler(Buffer*& pBuffer){
	auto pDB = GetDBSource(pBuffer->m_ActorID);
	if (NULL == pDB){
		ELOG(LOG_ERROR, "db is null");
		return;
	}

	switch(pBuffer->m_LogLevel){
		case CSToDB::eQueryUser_DBCallBack:
			DBAsynQueryUserCallBack(pBuffer, pDB);
			break;
		case CSToDB::eUpdateGameMail_DBCallBack:
			DBAsynUpdateGameMail(pBuffer, pDB);
			break;
		case  CSToDB::eChangeNickName_DBCall:
			DBAsynChangeNickNameCallBack(pBuffer, pDB);
			break;
		case CSToDB::eExeSQL_Call:
			DBAsyn_ExeSQL(pBuffer, pDB);
			break;
		case CSToDB::eInsertCDKeyEvents:
			InsertCDKeyEvent(pBuffer, pDB);
			break;
		case CSToDB::eUpdateUserGameMail_DBCallBack:
			DBAsynUpdateUserGameMail(pBuffer, pDB);
			break;
		case CSToDB::eUpdateCDKeyInfo:
			UpdateCDKey(pBuffer, pDB);
			break;
		case CSToDB::eInsertCDKeyInfo:
			InsertCDKey(pBuffer, pDB);
			break;
		default:
			ELOG(LOG_ERROR, "unknown msg");
			break;
	}
} 

INT32	CCSUserMgr::DBAsyn_QueryUser(SUserDBData & sUserData, DBToCS::QueryUser& sQueryUser, IDBConnector* pConn){
	if (NULL == pConn){
		ELOG(LOG_WARNNING,  " can not connect db!",  );
		return eEC_NullDataSource;
	} 
	 
	stringstream mystream;
	mystream << "SELECT * from gameuser, gameuser_runne,gameuser_guide where gameuser.obj_id = " << sUserData.sPODUsrDBData.un64ObjIdx  
		<< " and gameuser_runne.user_id =" << sUserData.sPODUsrDBData.un64ObjIdx
		<< " and  gameuser_guide.obj_id = " << sUserData.sPODUsrDBData.un64ObjIdx  << ";"; 
	INT32 n32QueryRet = pConn->ExecQuery(mystream.str());
	if (n32QueryRet != eNormal){
		ELOG(LOG_ERROR,"errorCode:%d,errorStr:%s.", pConn->GetErrorCode(),pConn->GetErrorStr());
		return n32QueryRet;
	} 
	
	UINT32 un32RowNum = pConn->GetQueryRowNum(); 
	if (un32RowNum < 1)
	{
		pConn->CloseQuery();
		ELOG(LOG_WARNNING, "没有查到老玩家数据");
		return eNormal;
	} 

	INT32 n32TempValue = 0;
	INT64 n64TempValue = 0;  

	pConn->GetQueryFieldData("obj_name", sUserData.szNickName);

	pConn->GetQueryFieldData("obj_sex", n32TempValue);
	sUserData.sPODUsrDBData.n16Sex = n32TempValue;
#if 0
	pConn->GetQueryFieldData("sdk_id", n32TempValue);
	sUserData.sPODUsrDBData.eUserPlatform = (EUserPlatform)n32TempValue;
#endif
	
	pConn->GetQueryFieldData("obj_headid", n32TempValue);
	sUserData.sPODUsrDBData.un16HeaderID = n32TempValue;

	pConn->GetQueryFieldData("obj_score", sUserData.sPODUsrDBData.n64Score);
	pConn->GetQueryFieldData("obj_diamond", sUserData.sPODUsrDBData.n64Diamond);
	pConn->GetQueryFieldData("obj_gold", sUserData.sPODUsrDBData.n64Gold);
	pConn->GetQueryFieldData("obj_game_inns", sUserData.sPODUsrDBData.un32TotalGameInns);
	pConn->GetQueryFieldData("obj_game_winns", sUserData.sPODUsrDBData.un32TotalWinInns);
	pConn->GetQueryFieldData("obj_kill_hero_num", sUserData.sPODUsrDBData.un32TotalHeroKills);
	pConn->GetQueryFieldData("obj_ass_kill_num", sUserData.sPODUsrDBData.un32TotalAssist);
	pConn->GetQueryFieldData("obj_dest_building_num", sUserData.sPODUsrDBData.un32TotalDestoryBuildings);
	pConn->GetQueryFieldData("obj_dead_num", sUserData.sPODUsrDBData.un32TotalDeadTimes);
	pConn->GetQueryFieldData("obj_cur_lv_exp", sUserData.sPODUsrDBData.un32UserCurLvExp);

	pConn->GetQueryFieldData("obj_lv", n32TempValue);
	sUserData.sPODUsrDBData.un8UserLv = n32TempValue;

	pConn->GetQueryFieldData("obj_cldays", n32TempValue);
	sUserData.sPODUsrDBData.un16Cldays = n32TempValue;

	pConn->GetQueryFieldData("obj_vip_lv", n32TempValue);
	sUserData.sPODUsrDBData.un16VipLv = n32TempValue; 
 
	pConn->GetQueryFieldData("obj_last_loginreward_time", sUserData.sPODUsrDBData.un32LastGetLoginRewardDay);  
	pConn->GetQueryFieldData("obj_register_time",  sUserData.sPODUsrDBData.tRegisteUTCMillisec );  
	pConn->GetQueryFieldData("obj_task_data",  sUserData.szTaskData );
	sQueryUser.set_taskdata(sUserData.szTaskData);

	DBToCS::RuneInfo& runedb = *sQueryUser.add_runeinfo();

	string runnebag_json;
	pConn->GetQueryFieldData("runnebag_json", runnebag_json);

	string runeslot_json;
	pConn->GetQueryFieldData("runeslot_json", runeslot_json);

	runedb.set_slotstr(runeslot_json);
	runedb.set_bagstr(runnebag_json);

	std::string str;
	pConn->GetQueryFieldData("obj_cs_guide_com_steps", str);  
	sQueryUser.set_guidestr(str);

	pConn->CloseQuery(); 
 
	n32QueryRet = GetUserHeros(pConn, sUserData.sPODUsrDBData.un64ObjIdx, sQueryUser); 
	if (n32QueryRet != eNormal){
		return n32QueryRet;
	}

	map<UINT64, UINT32> t_map;
	n32QueryRet = DBAsyn_QueryUserSNSList(pConn, sUserData.sPODUsrDBData.un64ObjIdx, t_map);
	if (n32QueryRet == eNormal){
		for (auto iter = t_map.begin(); iter != t_map.end(); ++iter){
			DBToCS::RSinfo *t_info = sQueryUser.add_rsinfo();
			t_info->set_related_id(iter->first);
			t_info->set_relation(iter->second);
			DBAsyn_QueryUserHeaderAndNickname(pConn, iter->first, *t_info);
		}
	}
	else{
		ELOG(LOG_ERROR, "");
	}

	n32QueryRet = DBAsyn_QueryUserItems(pConn, sUserData.sPODUsrDBData.un64ObjIdx, sQueryUser);
	if (n32QueryRet != eNormal){
		ELOG(LOG_ERROR, ""); 
	} 

	n32QueryRet = GetUserShortGiftMail(pConn, sUserData  ,  sQueryUser);
	if (n32QueryRet != eNormal){
		ELOG(LOG_ERROR, ""); 
	} 

	//获取个人邮件
	return n32QueryRet;
}

INT32	CCSUserMgr::GetUserHeros(/*MysqlDWrapper::*/IDBConnector *pConn, UINT64	un64ObjIdx, DBToCS::QueryUser& sQueryUser)
{
	stringstream mystream;
	mystream.str("");
	mystream << "select hero_id,hero_end_time,hero_buy_time from gameuser_hero where user_id=";
	mystream << un64ObjIdx;
	mystream << ";";

	INT32 nRet = pConn->ExecQuery(mystream.str());
	if (nRet != eNormal)
	{
		ELOG(LOG_ERROR," get uer heros error!"); 
		return nRet;
	}
	UINT32 un32RowNum = pConn->GetQueryRowNum(); 
	for ( int i=0; i< un32RowNum; i++)
	{
		DBToCS::HeroCfg& herodb = *sQueryUser.add_herocfg();

		SUserHeroDBData sSUserHeroDBData;
		pConn->GetQueryFieldData("hero_end_time", sSUserHeroDBData.endTime); 
		pConn->GetQueryFieldData("hero_buy_time", sSUserHeroDBData.buyTime);
		pConn->GetQueryFieldData("hero_id", sSUserHeroDBData.un32HeroID); 

		herodb.set_buytime(sSUserHeroDBData.buyTime);
		herodb.set_expiredtime(sSUserHeroDBData.endTime);
		herodb.set_commodityid(sSUserHeroDBData.un32HeroID);

		pConn->QueryNext();
	} 
	pConn->CloseQuery();

	return eNormal; 
}
 
INT32	CCSUserMgr::GetUserRune(/*MysqlDWrapper::*/IDBConnector *pConn, UINT64 un64ObjIdx, DBToCS::QueryUser& sQueryUser){
	stringstream mystream;
	mystream << "select runnebag_json,runeslot_json from gameuser_runne where user_id=" << un64ObjIdx << ";";

	INT32 nRet = pConn->ExecQuery(mystream.str().c_str());
	if (nRet != eNormal)
	{
		ELOG(LOG_ERROR, "GetUserRune error!"); 
		return nRet;
	}
	UINT32 un32RowNum = pConn->GetQueryRowNum(); 
	for (int i=0; i< un32RowNum; i++)
	{
		DBToCS::RuneInfo& runedb = *sQueryUser.add_runeinfo();

		string runnebag_json;
		pConn->GetQueryFieldData("runnebag_json", runnebag_json);

		string runeslot_json;
		pConn->GetQueryFieldData("runeslot_json", runeslot_json);
		
		runedb.set_slotstr(runeslot_json);
		runedb.set_bagstr(runnebag_json);
		pConn->QueryNext();
	} 
	pConn->CloseQuery();

	return eNormal; 
}

INT32	CCSUserMgr::DBAsyn_QueryUserSNSList(/*MysqlDWrapper::*/IDBConnector *pConn, const UINT64 un64ObjIdx, map<UINT64, UINT32>& t_map)
{
	stringstream mystream;
	mystream.str("");
	mystream << "select * from gameuser_sns where user_id=";
	mystream << un64ObjIdx;
	mystream << ";";

	INT32 nRet = pConn->ExecQuery(mystream.str());
	if (0 != nRet){
		ELOG(LOG_ERROR, "get user sns list fail with mysql error:%d, dis:%s", pConn->GetErrorCode(), pConn->GetErrorStr());
		return nRet;
	}

	UINT32 un32RowNum = pConn->GetQueryRowNum(); 
	 
	if (un32RowNum > 0)
	{
		for (INT32 i = 0; i < un32RowNum; i++){
			UINT32 temp_rs = 0;
			UINT64 temp_guid = 0;
			pConn->GetQueryFieldData("related_id", temp_guid);

			pConn->GetQueryFieldData("relation", temp_rs);

			t_map[temp_guid] = temp_rs;

			pConn->QueryNext();
		}

	}
	pConn->CloseQuery();
	return eNormal;
}

INT32	CCSUserMgr::DBAsyn_QueryUserHeaderAndNickname(/*MysqlDWrapper::*/IDBConnector *pConn, const UINT64 un64ObjIdx, DBToCS::RSinfo& rs_info)
{
	stringstream mystream;
	mystream.str("");
	mystream << "select obj_name,obj_headid,obj_vip_lv from gameuser where obj_id=";
	mystream << un64ObjIdx;
	mystream << ";";

	INT32 nRet = pConn->ExecQuery(mystream.str());
	if (0 != nRet){
		ELOG(LOG_ERROR, "get user sns list fail with mysql error:%d, dis:%s", pConn->GetErrorCode(), pConn->GetErrorStr());
		return nRet;
	}

	UINT32 un32RowNum = pConn->GetQueryRowNum(); 
	if (un32RowNum > 0){
		string st_name = "";
		pConn->GetQueryFieldData("obj_name", st_name);

		UINT32 t_header = 0;
		pConn->GetQueryFieldData("obj_headid", t_header);

		UINT32 t_viplv = 0;
		pConn->GetQueryFieldData("obj_vip_lv", t_viplv);
		rs_info.set_related_name(st_name);
		rs_info.set_related_header(t_header);
		rs_info.set_related_vip(t_viplv);
	}

	pConn->CloseQuery();
	return eNormal;
}

INT32	CCSUserMgr::DBAsyn_QueryUserItems(/*MysqlDWrapper::*/IDBConnector *pConn, const UINT64 user_id, DBToCS::QueryUser& sQueryUser)
{	
	stringstream mystream;
	mystream.str("");
	mystream << "select * from gameuser_item where user_id=";
	mystream << user_id;
	mystream << ";";

	INT32 nRet = pConn->ExecQuery(mystream.str());
	if (0 != nRet){
		ELOG(LOG_ERROR, "get user sns list fail with mysql error:%d, dis:%s", pConn->GetErrorCode(), pConn->GetErrorStr());
		return nRet;
	}

	UINT32 un32RowNum = pConn->GetQueryRowNum(); 
	if (un32RowNum > 0)
	{
		UINT32	temp_id = 0;
		UINT64	temp_num = 0;
		UINT32	temp_end = 0;
		UINT32	temp_buy = 0;
		for (INT32 i = 0; i < un32RowNum; i++){
			DBToCS::ItemInfo &item_info = *sQueryUser.add_item_info();
			pConn->GetQueryFieldData("item_id", temp_id);
			pConn->GetQueryFieldData("item_num", temp_num);
			pConn->GetQueryFieldData("buy_time", temp_buy);
			pConn->GetQueryFieldData("end_time", temp_end);

			item_info.set_item_id(temp_id);
			item_info.set_item_num(temp_num);
			item_info.set_buy_time(temp_buy);
			item_info.set_end_time(temp_end);
			pConn->QueryNext();
		}

	}
	pConn->CloseQuery();
	return eNormal;
}
 
 

INT32	CCSUserMgr::AddNewGameUserInfo(SUserDBData & sUserData,/*MysqlDWrapper::*/IDBConnector* pDB)
{
	if (NULL == pDB)
	{
		ELOG(LOG_ERROR,"the null conn, objidx:%ld, sdkId:%d, cdkey:%s", sUserData.sPODUsrDBData.un64ObjIdx, sUserData.sPODUsrDBData.eUserPlatform, sUserData.szUserName.c_str());
		return eEC_NullPointer;
	}

	stringstream mystream;
	mystream<<"INSERT gameuser(obj_id,sdk_id,obj_cdkey) values( ";
	mystream<<sUserData.sPODUsrDBData.un64ObjIdx;
	mystream<<",";
	mystream<<sUserData.sPODUsrDBData.eUserPlatform;
	mystream<<",'";
	mystream<<sUserData.szUserName;
	mystream<<"');INSERT gameuser_runne(user_id)VALUES(";
	mystream<<sUserData.sPODUsrDBData.un64ObjIdx;
	mystream<<");";
 
	INT32  nRet = pDB->ExecQuery(mystream.str());
	if (nRet != eNormal)
	{ 
		ELOG(LOG_WARNNING,"errorCode:%d,errorStr:%s--", pDB->GetErrorCode(),pDB->GetErrorStr());
		return nRet;
	} 
	 
	ELOG(LOG_DBBUG, "add new user ok:%d!",sUserData.sPODUsrDBData.un64ObjIdx );

	return nRet;
}

INT32 CCSUserMgr::AlterUserSNSList(/*MysqlDWrapper::*/IDBConnector *pConn, const UINT64 asker_guid, const UINT64 r_guid, ERelationShip rs_type, EDBOperation sOpType)
{
	if (NULL == pConn){
		return eEC_NullPointer;
	}

	if (eOperationType_Add == sOpType){	
		stringstream mystream;
		mystream<<"insert into gameuser_sns(user_id,related_id,relation) values(";
		mystream<<asker_guid;
		mystream<<",";
		mystream<<r_guid;
		mystream<<",";
		mystream<<rs_type;
		mystream<<");"; 

		INT32  nRet = pConn->ExecQuery(mystream.str());
		ELOG(LOG_INFO, "Add User:%d to User:%d SNS As type:%d.", asker_guid, r_guid, rs_type);
		if (nRet != 0){
			ELOG(LOG_WARNNING,"errorCode:%d,errorStr:%s--", pConn->GetErrorCode(),pConn->GetErrorStr());
			return nRet;
		}
		ELOG(LOG_DBBUG,"User:%d add User:%d To SNS List, RS:%d.", asker_guid, r_guid, rs_type);
	}
	else{

		stringstream t_stream;
		t_stream<<"delete from gameuser_sns where user_id=";
		t_stream<<asker_guid;
		t_stream<<" and related_id=";
		t_stream<<r_guid;
		t_stream<<";";

		ELOG(LOG_INFO, "Remove User:%d from User:%d SNS As type:%d.", r_guid, asker_guid, rs_type);
		INT32  nRet = pConn->ExecQuery(t_stream.str());
		if (nRet != 0){
			ELOG(LOG_WARNNING,"errorCode:%d,errorStr:%s--", pConn->GetErrorCode(),pConn->GetErrorStr());
			return nRet;
		}

		if (rs_type == eRSType_Friends){
			stringstream to_stream;
			to_stream<<"delete from gameuser_sns where user_id=";
			to_stream<<r_guid;
			to_stream<<" and related_id=";
			to_stream<<asker_guid;
			to_stream<<";";
			ELOG(LOG_INFO, "Remove User:%d from User:%d SNS As type:%d.", asker_guid, r_guid, rs_type);
			INT32  nRet = pConn->ExecQuery(to_stream.str());
			if (nRet != 0){
				ELOG(LOG_WARNNING,"---222--- errorCode:%d,errorStr:%s--", pConn->GetErrorCode(),pConn->GetErrorStr());
				return nRet;
			}
		}
	}
	return eNormal;
}

INT32	CCSUserMgr::DBAsyAlterItemCallBack(Buffer*& pBuffer, /*MysqlDWrapper::*/IDBConnector* pDB)
{
	boost::shared_ptr<CSToDB::AlterItem> pMsg = ParseProtoMsgWithNewMemory<CSToDB::AlterItem>(pBuffer->GetDataHeader(), pBuffer->GetDataLength());
	if (!pMsg)
	{
		ELOG(LOG_ERROR, "Login Fail With Msg Analysis Error.");
		return 0;
	}

	DBAsyn_AlterUserItem(pDB, pMsg->sql_str());

	return eNormal;
}

INT32	CCSUserMgr::DBAsyn_AlterUserItem(IDBConnector *pConn, const string sql_str)
{
	if (NULL == pConn){
		return eEC_NullPointer;
	}

	INT32 ret = pConn->ExecQuery(sql_str);
	if (ret != eNormal){
		ELOG(LOG_WARNNING,"errorCode:%d,errorStr:%s--", pConn->GetErrorCode(),pConn->GetErrorStr());
		return ret;
	}
	return eNormal;
}

}
