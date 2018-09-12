// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSUser.h"
#include "CSUserMgr.h"
#include "CSCfgMgr.h"
#include "CSWordFilter.h"
#include "CSKernel.h"
#include "CSBattle.h"
#include "CSBattleMgr.h"
#include "boost/spirit.hpp"
#include "boost/algorithm/string.hpp"
using namespace std;

namespace CentralServer{
INT32	CCSUser::PostMsgToGC(google::protobuf::Message& sMsg, int n32MsgID){
	const ICSGSInfo *cpiGSInfo = GetCSKernelInstance()->GetGSInfoByGSID(m_sUserNetInfo.n32GSID);
	if (NULL == cpiGSInfo){
		return eEC_NullGateServer;
	}

	return GetCSKernelInstance()->PostMsgToGS(cpiGSInfo, m_sUserNetInfo.n32GCNSID, sMsg, n32MsgID);
}

INT32	CCSUser::PostMsgToGC_PingRet(){
	GSToGC::PingRet aPingRet;
	PostMsgToGC(aPingRet, aPingRet.msgid());
	return eNormal;
}

INT32	CCSUser::PostMsgToGC_NetClash(){
	GSToGC::NetClash sMsg;
	PostMsgToGC(sMsg, sMsg.msgid());
	return eNormal;
}

INT32	CCSUser::SynUser_UserBaseInfo(){
	GSToGC::UserBaseInfo sUserBaseInfo;
	sUserBaseInfo.set_nickname(m_sUserDBData.szNickName);
	sUserBaseInfo.set_name(m_sUserDBData.szUserName);
	sUserBaseInfo.set_sex(m_sUserDBData.sPODUsrDBData.n16Sex);
	sUserBaseInfo.set_curscore(m_sUserDBData.sPODUsrDBData.n64Score);
	sUserBaseInfo.set_curgold(m_sUserDBData.sPODUsrDBData.n64Gold);
	sUserBaseInfo.set_curdiamoand(m_sUserDBData.sPODUsrDBData.n64Diamond);
	sUserBaseInfo.set_curgold(m_sUserDBData.sPODUsrDBData.n64Gold); 
	sUserBaseInfo.set_guid(m_sUserDBData.sPODUsrDBData.un64ObjIdx);  
	sUserBaseInfo.set_mapid(GetBattleMgrInstance()->GetBattleMapID(m_sUserBattleInfoEx.GetBattleID()));
	sUserBaseInfo.set_battleid(m_sUserBattleInfoEx.GetBattleID()); 
	sUserBaseInfo.set_ifreconnect(false);
	sUserBaseInfo.set_level(m_sUserDBData.sPODUsrDBData.un8UserLv);
	sUserBaseInfo.set_headid(m_sUserDBData.sPODUsrDBData.un16HeaderID);
	sUserBaseInfo.set_vip_level(m_sUserDBData.sPODUsrDBData.un16VipLv);
	sUserBaseInfo.set_vip_score(m_sUserDBData.sPODUsrDBData.vipScore);
	sUserBaseInfo.set_cur_exp(m_sUserDBData.sPODUsrDBData.un32UserCurLvExp);
	return PostMsgToGC(sUserBaseInfo, sUserBaseInfo.msgid());
}

INT32	CCSUser::PostMsgToGC_NotifyUserGameInfo(){
	const SUserDBData& sTempDB = GetUserDBData();
	GSToGC::UserGameInfo sUserGameInfo;
	sUserGameInfo.set_headid(sTempDB.sPODUsrDBData.un16HeaderID);
	sUserGameInfo.set_nickname(sTempDB.szNickName);
	sUserGameInfo.set_level(sTempDB.sPODUsrDBData.un8UserLv);
	sUserGameInfo.set_upgradeexp(CCSCfgMgr::getInstance().GetUserLvUpExp(sTempDB.sPODUsrDBData.un8UserLv));
	sUserGameInfo.set_curexp(sTempDB.sPODUsrDBData.un32UserCurLvExp);
	sUserGameInfo.set_totalwintimes(sTempDB.sPODUsrDBData.un32TotalWinInns);
	sUserGameInfo.set_totalgameinns(sTempDB.sPODUsrDBData.un32TotalGameInns);
	sUserGameInfo.set_herokills(sTempDB.sPODUsrDBData.un32TotalHeroKills);
	sUserGameInfo.set_destorybuildings(sTempDB.sPODUsrDBData.un32TotalDestoryBuildings);
	sUserGameInfo.set_deadtimes(sTempDB.sPODUsrDBData.un32TotalDeadTimes);
	sUserGameInfo.set_total_achnum(0);
	sUserGameInfo.set_achnum(0);
	sUserGameInfo.set_vipscore(0);
	sUserGameInfo.set_assistnum(sTempDB.sPODUsrDBData.un32TotalAssist);
	sUserGameInfo.set_ladderscore(0);
	sUserGameInfo.set_guildname("");
// 	sUserGameInfo.set_gold_addtime(GetItemAdditionSurplusTime(eItemEffect_GoldGain));
// 	sUserGameInfo.set_exp_adtime(GetItemAdditionSurplusTime(eItemEffect_ExpGain));

	INT32 SurplusTime_gold = GetItemAdditionSurplusTime(eItemEffect_GoldGain);
	INT32 SurplusTime_exp = GetItemAdditionSurplusTime(eItemEffect_ExpGain);

	if (SurplusTime_gold == 0){
		SurplusTime_gold = 1;
	}
	if (SurplusTime_exp == 0){
		SurplusTime_exp = 1;
	}

	sUserGameInfo.set_gold_addtime(SurplusTime_exp);
	sUserGameInfo.set_exp_adtime(SurplusTime_gold);

	ELOG(LOG_DBBUG, "%lld, %lld.", GetItemAdditionSurplusTime(eItemEffect_GoldGain), GetItemAdditionSurplusTime(eItemEffect_ExpGain));

	PostMsgToGC(sUserGameInfo, sUserGameInfo.msgid());
	return eNormal;
}

INT32	CCSUser::SynUser_AllHeroList()
{ 
	GSToGC::NotifyCSHeroList sMsg;
	vector<HeroListStruct> heroVec;
	GetHeroVec(heroVec);
	for (auto iter = heroVec.begin(); iter != heroVec.end(); ++iter){
		HeroListStruct& sHeroListStruct = *iter;
		GSToGC::NotifyCSHeroList::HeroListCfg* pCfg = sMsg.add_herocfg();

		SHeroBuyCfg *t_cfg = CCSCfgMgr::getInstance().GetHeroClientMatchCfg(sHeroListStruct.heroid);
		if (t_cfg == NULL){
			continue;
		}

		if (sHeroListStruct.expiredTime <= 0 && sHeroListStruct.expiredTime != -1 && sHeroListStruct.ifFree != true){
			continue;
		}

		pCfg->set_heroid(t_cfg->un32CommondityID);
		pCfg->set_expired_time(sHeroListStruct.expiredTime);
		pCfg->set_if_free(sHeroListStruct.ifFree);
	//	ELOG(LOG_DBBUG, "Notify herolist:%d expitred:%lld, idfree:%d", sHeroListStruct.heroid, sHeroListStruct.expiredTime, sHeroListStruct.ifFree);
	}
	return PostMsgToGC(sMsg, sMsg.msgid()); 
}

INT32	CCSUser::SynUserSNSList(const UINT64 guid_friends, ERelationShip eRSType){

	GSToGC::NotifyUserSNSList sNotifyUserSNSList;
	if (guid_friends != 0){
		CCSUser *piUser = GetCSUserMgrInstance()->GetUserByGuid(guid_friends);
		if (NULL == piUser){
			return eEC_UserNotExist;
		}
		GSToGC::SNSInfo* pInfo = sNotifyUserSNSList.add_info();
		pInfo->set_type(eRSType);
		pInfo->set_nickname(piUser->GetNickName());
		pInfo->set_headid(piUser->GetHeadID());
		pInfo->set_status(piUser->GetUserPlayingStatus());
		pInfo->set_guididx(guid_friends);
		pInfo->set_viplv(piUser->GetUserDBData().sPODUsrDBData.un16VipLv);
	}
	else{
		if (eRSType_Friends == eRSType){
			auto iter = m_sUserDBData.friendListMap.begin();
			while(iter != m_sUserDBData.friendListMap.end()){
				GSToGC::SNSInfo* pInfo = sNotifyUserSNSList.add_info();
				pInfo->set_type(eRSType);
				pInfo->set_guididx(iter->second.guididx);
				CCSUser *pTempUser = GetCSUserMgrInstance()->GetUserByGuid(iter->second.guididx);
				if (NULL != pTempUser){
					pInfo->set_headid(pTempUser->GetHeadID());
					pInfo->set_nickname(pTempUser->GetNickName());
					pInfo->set_status(pTempUser->GetUserPlayingStatus());
					pInfo->set_viplv(pTempUser->GetUserDBData().sPODUsrDBData.un16VipLv);
				}
				else{
					pInfo->set_headid(iter->second.nHeadId);
					pInfo->set_nickname(iter->second.stNickName);
					pInfo->set_status(eUserPlayingStatus_OffLine);
					pInfo->set_viplv(iter->second.viplv);
				}
				++ iter;
			}
		}
		else{
			auto iter = m_sUserDBData.blackListMap.begin();
			while(iter != m_sUserDBData.blackListMap.end()){

				GSToGC::SNSInfo* pInfo = sNotifyUserSNSList.add_info();
				pInfo->set_type(eRSType);
				pInfo->set_headid(iter->second.nHeadId);
				pInfo->set_guididx(iter->second.guididx);
				CCSUser *pTempUser = GetCSUserMgrInstance()->GetUserByGuid(iter->second.guididx);
				if (NULL != pTempUser){
					pInfo->set_nickname(pTempUser->GetNickName());
				}
				else{
					pInfo->set_nickname(iter->second.stNickName);
				}
				++ iter;
			}
		}
	}
	PostMsgToGC(sNotifyUserSNSList, sNotifyUserSNSList.msgnum());
 	return eNormal;
 }

INT32	CCSUser::PostMsgToGC_NotifuUserReMoveFromSNSList(ERelationShip eRSType, const UINT64 guididx)
{
	GSToGC::NotifyUserSNSListChange sMsg;;
	sMsg.set_guididx(guididx);
	sMsg.set_type(eRSType);
	//ELOG(LOG_INFO,"Nickname:%s, length:%d.", stNickname, stNickname.length());
	PostMsgToGC(sMsg, sMsg.msgnum());
	return eNormal;
}

INT32	CCSUser::PostMsgToGC_UserBeInvitedToBattle(UINT64 un64BattleID, const string& stPwd, const string& stNickName)
{
	GSToGC::UserBeInvitedToBattle msg;
	msg.set_pwd(stPwd);
	msg.set_invitor(stNickName);
	msg.set_battleid(un64BattleID);
	PostMsgToGC(msg, msg.msgnum());
	return eNormal;
}

INT32	CCSUser::PostMsgToGC_NotifyBeAddFriendMsg(const string& stSender,const UINT64 guid_sender, const string& stRec)
{
	GSToGC::NotifyBeAddFriendMs sMsg;
	sMsg.set_sendnickname(stSender);
	sMsg.set_sdnder_guididx(guid_sender);
	sMsg.set_recnickname(stRec);
	ELOG(LOG_INFO,"Sender:%s,length:%d, Rec:%s, legth:%d.", stSender, stSender.length(), stRec, stRec.length());
	return PostMsgToGC(sMsg, sMsg.msgnum());
}

INT32	CCSUser::PostMsgToGC_MsgFromUser(const UINT64 guid, const string stSender, UINT32 headerid, INT32 n32MsgLength, const CHAR *pbMsg)
{
	string charStr(pbMsg, n32MsgLength);

	GSToGC::NotifyMsgFromUser sMsg;
	sMsg.set_guididx(guid);
	sMsg.set_nickname(stSender);
	sMsg.set_chatstr(charStr);
	sMsg.set_headid(headerid);

	ELOG(LOG_INFO,"nickname:%s, length:%d, MsgLength:%d.", stSender, stSender.length(), charStr.length());
	return PostMsgToGC(sMsg, sMsg.msgnum());
}

INT32	CCSUser::PostMsgToGC_AskRetMsg(INT32 n32AskProtocalID, INT32 n32RetFlag)
{
	return GetCSUserMgrInstance()->PostMsgToGC_AskReturn(m_sUserNetInfo, n32AskProtocalID, n32RetFlag);
}

INT32	CCSUser::SynUserCLDays()
{
	GSToGC::NotifyUserCLDays sMsg;
	sMsg.set_month(GetCSUserMgrInstance()->GetToday().month());//当前月
	sMsg.set_today(GetCSUserMgrInstance()->GetToday().day());//当前日
	sMsg.set_totalcldays(GetCSUserMgrInstance()->GetMonthDays());//月总天数
	sMsg.set_cldays(m_sUserDBData.sPODUsrDBData.un16Cldays);//已领天数
	sMsg.set_istodaycan((m_sUserDBData.sPODUsrDBData.un32LastGetLoginRewardDay)!=GetCSUserMgrInstance()->GetToday().julian_day());//今天是否可以领取
	return PostMsgToGC(sMsg, sMsg.msgid());
}

INT32	CCSUser::SynCurGold()
{
	GSToGC::NotifyCurGold sMsg;
	sMsg.set_gold(m_sUserDBData.sPODUsrDBData.n64Gold);
	return PostMsgToGC(sMsg, sMsg.msgid());
}

INT32	CCSUser::SynUserLvInfo()
{  
	GSToGC::NotifyUserLvInfo sMsg;
	sMsg.set_curlv(GetUserDBData().sPODUsrDBData.un8UserLv);
	sMsg.set_curexp(GetUserDBData().sPODUsrDBData.un32UserCurLvExp);
	return PostMsgToGC(sMsg, sMsg.msgid());
	return eNormal;
}

INT32	CCSUser::SynCurDiamond()
{
	GSToGC::NotifyCurDiamond sMsg;
	sMsg.set_diamond(m_sUserDBData.sPODUsrDBData.n64Diamond);
	return PostMsgToGC(sMsg, sMsg.msgid());
}

INT32	CCSUser::PostMsgToGC_NotifyGetLoginRewardScuess()
{
	GSToGC::GetloginRewardSuccess Msg;
	return PostMsgToGC(Msg, Msg.msgid());
}

INT32	CCSUser::PostMsgToSS_UserInfoChange(){
	CSToSS::NotifyUserInfoChange sMsg;
	sMsg.set_guid(m_sUserDBData.sPODUsrDBData.un64ObjIdx);
	sMsg.set_gold(m_sUserDBData.sPODUsrDBData.n64Gold);
	return GetBattleMgrInstance()->PostMsgToBattleSS(m_sUserBattleInfoEx.GetBattleID(), sMsg, sMsg.msgid());
}

INT32 CCSUser::SynCommidityCfgInfo(){
	GSToGC::GoodsBuyCfgInfo sMsg;
	{
		map<UINT32, SHeroBuyCfg>& pHeroMap = CCSCfgMgr::getInstance().GetHeroBuyCfgmap();
		for (auto iter = pHeroMap.begin(); iter != pHeroMap.end(); ++iter){
			SHeroBuyCfg& pCfg = iter->second;
			if (!pCfg.bIfShowInShop){
				continue;
			}
			GSToGC::GoodsCfgInfo& sGoodsCfgInfo = *sMsg.add_info();
			sGoodsCfgInfo.set_goodid(iter->first);
			for (auto consumeIter = pCfg.sConsumeList.Begin(); consumeIter != pCfg.sConsumeList.End(); consumeIter = pCfg.sConsumeList.Next()){
				GSToGC::GoodsCfgInfo::Consume& pConsume = *sGoodsCfgInfo.add_consume();
				ConsumeStruct& sConsumeStruct = *consumeIter;
				pConsume.set_consumetype(sConsumeStruct.type);
				pConsume.set_price(sConsumeStruct.price);
			}
			sGoodsCfgInfo.set_cfg_type(GSToGC::GoodsCfgInfo::CfgType_Common);
		}

		map<INT32, SRunesCfg>& pRuneMap = CCSCfgMgr::getInstance().GetRunesMap();
		for (auto iter = pRuneMap.begin(); iter != pRuneMap.end(); ++iter){
			SRunesCfg& pCfg = iter->second;
			if (!pCfg.bIfShowInShop){
				continue;
			}
			GSToGC::GoodsCfgInfo& sGoodsCfgInfo = *sMsg.add_info();
			sGoodsCfgInfo.set_goodid(iter->first);
			for (auto consumeIter = pCfg.sConsumeList.Begin(); consumeIter != pCfg.sConsumeList.End(); consumeIter = pCfg.sConsumeList.Next()){
				GSToGC::GoodsCfgInfo::Consume& pConsume = *sGoodsCfgInfo.add_consume();
				ConsumeStruct& sConsumeStruct = *consumeIter;
				pConsume.set_consumetype(sConsumeStruct.type);
				pConsume.set_price(sConsumeStruct.price);
			}
			sGoodsCfgInfo.set_cfg_type(GSToGC::GoodsCfgInfo::CfgType_Common);
		}
	}

	{
		map<UINT32, SDiscountCfg>& pDiscountMap = CCSCfgMgr::getInstance().GetDisCountMapByType(eGoodsType_Hero);
		for (auto iter = pDiscountMap.begin(); iter != pDiscountMap.end(); ++iter){
			SDiscountCfg& pCfg = iter->second;
			GSToGC::GoodsCfgInfo& sGoodsCfgInfo = *sMsg.add_info();
			sGoodsCfgInfo.set_goodid(iter->first);
			for (auto consumeIter = pCfg.sConsumeList.Begin(); consumeIter != pCfg.sConsumeList.End(); consumeIter = pCfg.sConsumeList.Next()){
				GSToGC::GoodsCfgInfo::Consume& pConsume = *sGoodsCfgInfo.add_consume();
				ConsumeStruct& sConsumeStruct = *consumeIter;
				pConsume.set_consumetype(sConsumeStruct.type);
				pConsume.set_price(sConsumeStruct.price);
			}
			sGoodsCfgInfo.set_cfg_type(GSToGC::GoodsCfgInfo::CfgType_Discount);
		}
	}

	{
		vector<UINT32>& sNewGoodsCfg = CCSCfgMgr::getInstance().GetNewCfgVec();
		for (auto iter = sNewGoodsCfg.begin(); iter != sNewGoodsCfg.end(); ++iter){
			GSToGC::GoodsCfgInfo& sGoodsCfgInfo = *sMsg.add_info();
			sGoodsCfgInfo.set_goodid(*iter);	
			sGoodsCfgInfo.set_cfg_type(GSToGC::GoodsCfgInfo::CfgType_New);
		}
	}
		
	{
		vector<UINT32>& sHotGoodsCfg = CCSCfgMgr::getInstance().GetHotGoodsCfg();
		for (auto iter = sHotGoodsCfg.begin(); iter != sHotGoodsCfg.end(); ++iter){
			GSToGC::GoodsCfgInfo& sGoodsCfgInfo = *sMsg.add_info();
			sGoodsCfgInfo.set_goodid(*iter);	
			sGoodsCfgInfo.set_cfg_type(GSToGC::GoodsCfgInfo::CfgType_hot);
		}
	}
	GetCSKernelInstance()->PostMsgToGC(m_sUserNetInfo, sMsg, sMsg.msgid());
	return eNormal;
}

INT32 CCSUser::SynUser_AllRunesList(){
	GSToGC::NotifyRunesList sNotifyRunesList;
	for (auto iter = m_RunesMap.begin(); iter != m_RunesMap.end(); ++iter){
		auto pBagIno = sNotifyRunesList.add_runesbaginfo();
		pBagIno->set_runeid(iter->second.un32RunesTypeID);
		pBagIno->set_num(iter->second.n16Num);
		pBagIno->set_gottime(iter->second.n64GotTime);
	}

	for (auto iter = m_EquipRuneArrayMap.begin(); iter != m_EquipRuneArrayMap.end(); ++iter){
		EquipRuneArray& sEquipRuneArray = iter->second;
		for (INT32 i = 0; i < sEquipRuneArray.GetMaxSize(); ++i){
			UINT32* pRuneID = sEquipRuneArray.GetValueFromIndex(i);
			if (pRuneID){
				auto pSlotInfo = sNotifyRunesList.add_runes_slot_info();
				pSlotInfo->set_runeid(*pRuneID);
				pSlotInfo->set_slotpos(i);
				pSlotInfo->set_page(iter->first);
			}
		}
	}
	PostMsgToGC(sNotifyRunesList, sNotifyRunesList.msgid());
	return eNormal;
}

INT32	CCSUser::SynUser_IsOnSS()
{
	GSToGC::NotifyIsOnSS msg;
	CCSBattle* pBattle = GetBattleMgrInstance()->GetBattle(GetUserBattleInfoEx().GetBattleID());
	msg.set_ssid(pBattle==NULL?0:pBattle->GetSSID());
	PostMsgToGC(msg, msg.msgid());
	return eNormal;
}

INT32	CCSUser::NotifyCanInviteFriends()
{
	GSToGC::CanInviteFriends Msg;
	for(auto iter = m_sUserDBData.friendListMap.begin(); iter != m_sUserDBData.friendListMap.end(); ++iter){
		CCSUser* t_User = GetCSUserMgrInstance()->GetUserByGuid(iter->second.guididx);
		if (t_User != NULL && eBS_Free == t_User->GetUserBattleInfoEx().GetBattleState() 
			&& t_User->GetUserPlayingStatus() == eUserPlayingStatus_Playing){
				GSToGC::CanInviteFriends_Friends *friendsInfo = Msg.add_friends();
				friendsInfo->set_headerid(iter->second.nHeadId);
				friendsInfo->set_nickname(iter->second.stNickName);
				friendsInfo->set_guididx(iter->second.guididx);
		}
	}
	return PostMsgToGC(Msg, Msg.msgid());
}

INT32	CCSUser::NotifyUserPlayState()
{
	auto iter_f = m_sUserDBData.friendListMap.begin();
	for (; iter_f != m_sUserDBData.friendListMap.end(); iter_f ++){
		CCSUser *piUser = GetCSUserMgrInstance()->GetUserByGuid(iter_f->second.guididx);

		if (NULL != piUser){
			if (!piUser->CheckIfInFriendList(GetGUID())){
				/*m_sUserDBData.friendListMap.erase(iter_f);*/
				continue;
			}

			if (eUserPlayingStatus_Playing == piUser->GetUserPlayingStatus()){
				piUser->SynUserSNSList(GetGUID(), iter_f->second.eRelationShip);
			}
		}
	}
	return eNormal;
}


INT32 CCSUser::PostMsgToGC_NotifyNewNickname(const UINT64 guid, const string& sznickname)
{
	GSToGC::NotifyNewNickname sMsg;
	sMsg.set_guid(guid);
	sMsg.set_newnickname(sznickname);
	return PostMsgToGC(sMsg, sMsg.msgid());
}

INT32 CCSUser::PostMsgToGC_NotifyNewHeaderid(const UINT64 guid, const UINT32 headerid)
{
	GSToGC::NotifyNewHeaderid sMsg;
	sMsg.set_guid(guid);
	sMsg.set_newheaderid(headerid);
	return PostMsgToGC(sMsg, sMsg.msgid());
}


INT32 CCSUser::SynOtherItemInfo(UINT32 item_id)
{
	GSToGC::NotifyOtherItemInfo Msg;
	if (item_id == 0)
	{
		for (auto iter = m_sUserDBData.item_Map.begin(); iter != m_sUserDBData.item_Map.end(); ++iter){
			GSToGC::NotifyOtherItemInfo_OtherItem *item = Msg.add_item();
			item->set_item_id(iter->second.item_id);
			item->set_item_num(iter->second.item_num);
			item->set_expired_time(iter->second.end_time);
		}
	}
	else{
		GSToGC::NotifyOtherItemInfo_OtherItem *item = Msg.add_item();
		auto iter_f = m_sUserDBData.item_Map.find(item_id);
		if (iter_f == m_sUserDBData.item_Map.end()){
			return eEC_NullPointer;
		}
		item->set_item_id(iter_f->second.item_id);
		item->set_item_num(iter_f->second.item_num);
		item->set_expired_time(iter_f->second.end_time);
	}
	PostMsgToGC(Msg, Msg.msgid());
	return eNormal;
}



void	CCSUser::PostCSNotice()
{ 
	GSToGC::GameNotice pMsg;
	auto iter = GetCSUserMgrInstance()->GetNotice().begin();
	for (; iter != GetCSUserMgrInstance()->GetNotice().end(); iter ++)
	{
		SNotice temp_notice = *iter;

		if (temp_notice.msg.length() < 1){
			continue;
		}

		//平台判断
		if (temp_notice.platform != ePlatform_All){
			if (temp_notice.platform != GetUserDBData().sPODUsrDBData.eUserPlatform){
				continue;
			}
		}
		//过期判断
		UINT64 temp_date = time(NULL);
		INT64 temp_end = temp_notice.end_time - temp_date;
		if (temp_end < 0){
			continue;
		}

		//是否到发送时间
		INT64 temp_star = temp_notice.star_time - temp_date;
		if (temp_star > 0){
			continue;
		}

		GSToGC::GameNotice_Notice *notice = pMsg.add_notice();
		notice->set_title(temp_notice.title);
		notice->set_flag(temp_notice.flag);
		notice->set_status(temp_notice.state);
		notice->set_priority(temp_notice.priority);
		notice->set_notice(temp_notice.msg);

	}
	PostMsgToGC(pMsg, pMsg.msgid());
}

void CCSUser::NotifyCDKGiftToGC(INT32 item_id, INT32 num)
{
	if (item_id == eLoginRewadItem_None) 
		return;

	else if (item_id == eLoginRewadItem_Gold) { 
		m_sUserDBData.ChangeUserDbData(eUserDBType_Gold, num);
		SynCurGold();
	}
	else if (item_id == eLoginRewadItem_Diamond) {
		m_sUserDBData.ChangeUserDbData(eUserDBType_Diamond, num);
		SynCurDiamond();
	}
	else if (IfHeroCfgID(item_id)) {
		AddHero(item_id, true);
	}
	else if (IfSkinCfgID(item_id)) {}
	else if (IfRuneCfgID(item_id)) AddRunes(item_id, true);
	else if (IfOtherCfgID(item_id)){
		ChangeOtherItemMap(item_id, num);
	}
	else { Assert(false && "can't get this wards."); }
}

}