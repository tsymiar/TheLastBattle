// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSUser.h"
#include "CSUserMgr.h"
#include "CSCfgMgr.h"
#include "CSGameLogMgr.h"
#include "boost/spirit.hpp"
#include "boost/algorithm/string.hpp"
#include "CSWordFilter.h"
#include "CSBattle.h"
#include "CSBattleMgr.h"
#include "CSKernel.h"
#include "CSUserMgr.h"
#include <json/json.h>
#include "TaskSys/TaskMgr.h"

using namespace std;

//#define  Test_Goods 1

namespace CentralServer{
	CCSUser::CCSUser()
		: m_tRoomPlayer(this)
		, m_tMatchPlayer(this)
		, m_tGCLastPing(0)
		, m_TimerID(0)
		, m_OfflineTime(0)
	{
		m_RunePageMaxEquipNum.push_back(0);
		m_cTaskMgr = new CTaskMgr(this);
	}

	CCSUser::~CCSUser()
	{
		delete m_cTaskMgr;
		m_cTaskMgr = NULL;
	}

const int  RefreshCardBegin = 130005;
#ifdef Test_Goods
	const int RefreshNum = 6;
#else
	const int RefreshNum = 0;
#endif
	//此函数是在new User时加载数据库，一切都是新的
	INT32	CCSUser::LoadDBData(SUserDBData &crsDBData)
	{
		m_sUserDBData = crsDBData;
		if (0 == m_sUserDBData.sPODUsrDBData.un8UserLv){
			m_sUserDBData.ChangeUserDbData(eUserDBType_UserLv, 1);
			m_sUserDBData.ChangeUserDbData(eUserDBType_VIPLevel, 1);
		}

		for (int i = 0; i < RefreshNum; ++i)
		{
			auto itemID = RefreshCardBegin + i;
			auto cfg = CCSCfgMgr::getInstance().GetOtherItemCfg(itemID);
			if (!cfg){
				ELOG(LOG_ERROR, "洗练全配置为null");
				continue;
			}
			SUserItemInfo sSUserItemInfo;
			sSUserItemInfo.item_id = itemID;
			sSUserItemInfo.item_num = 5;
			sSUserItemInfo.end_time = -1;
			ELOG(LOG_DBBUG, "增加临时洗练");
			m_sUserDBData.item_Map[sSUserItemInfo.item_id] = sSUserItemInfo;
		}

		GetTaskMgr()->UnpackTaskData(m_sUserDBData.szTaskData);//解析任务数据

		return eNormal;
	}

	bool	CCSUser::IsOverTime()
	{
		return (m_eUserPlayingStatus == eUserPlayingStatus_OffLine &&
			GetUserBattleInfoEx().GetBattleType() == eBattleType_Free);
	}

	INT32 CCSUser::SetUserNetInfo(const SUserNetInfo &crsUNI)
	{
		Assert(crsUNI.IsValid());
		memcpy(&m_sUserNetInfo, &crsUNI, sizeof(m_sUserNetInfo));
		m_eUserPlayingStatus = eUserPlayingStatus_Playing;
		return eNormal;
	}

	void CCSUser::ClearNetInfo()
	{
		m_sUserNetInfo.Clear();
		m_eUserPlayingStatus = eUserPlayingStatus_OffLine;
	}

	INT32	CCSUser::AddHero(UINT32 un32HeroGoodsID, bool ifFree)
	{
		SHeroBuyCfg* pCfg = CCSCfgMgr::getInstance().GetHeroBuyCfg(un32HeroGoodsID);
		if (NULL == pCfg){
			ELOG(LOG_ERROR, "herocfg is null");
			return eEC_NullPointer;
		}
		GSToGC::NotifyCSHeroList sMsg;
		GSToGC::NotifyCSHeroList::HeroListCfg* pList = sMsg.add_herocfg();

		SUserHeroDBData sDBData;
		auto itr =  m_sUserDBData.heroListMap.find(pCfg->un32HeroID);
		if (itr != m_sUserDBData.heroListMap.end()){
			//如果是购买时限英雄
			if (pCfg->useTimeSpan > 0){
				if (itr->second.endTime != -1){
					//如果还没过期
					if ( (itr->second.endTime + itr->second.buyTime) - time(NULL) > 0){
						itr->second.endTime += pCfg->useTimeSpan * 3600;
						pList->set_expired_time(itr->second.endTime + itr->second.buyTime - time(NULL));
					}
					else{
						//如果已经过期
						itr->second.buyTime = time(NULL);
						itr->second.endTime =  pCfg->useTimeSpan * 3600;
						pList->set_expired_time(pCfg->useTimeSpan * 3600);
					}
				}
			}
			else{
				//如果是购买永久英雄
				if (itr != m_sUserDBData.heroListMap.end()){
					itr->second.endTime = -1;
					pList->set_expired_time(-1);
				}
			}
			//更新数据库
			UpdateHero(itr->second.un32HeroID, itr->second.endTime);
		}
		else{
			sDBData.un32HeroID = pCfg->un32HeroID;
			sDBData.buyTime = time(NULL);
			//如果购买的是时限英雄
			if (pCfg->useTimeSpan > 0){
				sDBData.endTime = pCfg->useTimeSpan * 3600;
				pList->set_expired_time(pCfg->useTimeSpan * 3600);
			}
			else{
				sDBData.endTime = -1;
				pList->set_expired_time(-1);
			}
			//加入数据库
			AddHero(sDBData);
		}
		//查询英雄对应的永久英雄ID，因为商品表可能是时限英雄
		SHeroBuyCfg *temp_cfg = CCSCfgMgr::getInstance().GetHeroClientMatchCfg(pCfg->un32HeroID);
		if (temp_cfg == NULL)
		{
			pList->set_heroid(un32HeroGoodsID);
		}
		else{
			pList->set_heroid(temp_cfg->un32CommondityID);
		}
		
		pList->set_if_free(ifFree);
		ELOG(LOG_DBBUG, "Notify herolist:%d expitred:%lld, idfree:%d", pCfg->un32HeroID, pCfg->useTimeSpan, ifFree);
		PostMsgToGC(sMsg, sMsg.msgid()); 

		return eNormal;
	}

	INT32 CCSUser::AddExperienceHero(const UINT32 heroid, INT64 addtime, bool iftimeAdd)
	{
		GSToGC::NotifyCSHeroList sMsg;
		GSToGC::NotifyCSHeroList::HeroListCfg* pList = sMsg.add_herocfg();
		SHeroBuyCfg *t_cfg = CCSCfgMgr::getInstance().GetHeroClientMatchCfg(heroid);
		//时限英雄物品ID转为客户端表现所需要ID
		if (t_cfg != NULL)
		{
			pList->set_heroid(t_cfg->un32CommondityID);
		}
		else{
			pList->set_heroid(heroid);
		}

		auto itr =  m_sUserDBData.heroListMap.find(heroid);
		if (itr != m_sUserDBData.heroListMap.end())
		{
			//如果已有永久英雄
			if (-1 == itr->second.endTime){
				return eEC_HavedPerpetualHero;
			}

			if (0 < itr->second.buyTime + itr->second.endTime - time(NULL) && iftimeAdd){
				//时限英雄还未过期
				itr->second.endTime += addtime;
				pList->set_expired_time(itr->second.endTime + itr->second.buyTime - time(NULL));
			}
			else{
				//时限英雄已过期或时间为不可累加
				if (itr->second.buyTime + itr->second.endTime > addtime + time(NULL)){
					//时间不可累加且剩余时间大于累加时间
					return eNormal;
				} 
				else{
					UINT64 CurTime = time(NULL);
					if (itr->second.endTime + itr->second.buyTime - CurTime < addtime){
						itr->second.buyTime = CurTime;
						itr->second.endTime = addtime;
						pList->set_expired_time(addtime);
					}
				}				
			}
		}
		else{			
			SUserHeroDBData sDBData;
			sDBData.un32HeroID = heroid;
			sDBData.buyTime = time(NULL);
			sDBData.endTime = sDBData.buyTime + addtime;
			pList->set_expired_time(addtime);
			AddHero(sDBData);
		}

		pList->set_if_free(false);
		PostMsgToGC(sMsg, sMsg.msgid()); 

		return eNormal;
	}

	INT32 CCSUser::GCAskEqipRunes(UINT32 runeID, INT32 n16RunePage)
	{
		bool ifEquipSuccess = false;
		INT32 toPos = 0;
		do {
			auto rune = CCSCfgMgr::getInstance().GetRunesCfg(runeID);
			if (!rune){
				ELOG(LOG_ERROR, "null rune %u", runeID);
				break;
			}

			if (n16RunePage > m_RunePageMaxEquipNum.size())
			{
				ELOG(LOG_ERROR, "n16RunesToPos(%d) not valid", n16RunePage);
				break;
			}

			int canGetMaxRune = m_sUserDBData.sPODUsrDBData.un8UserLv / 3;
			if (canGetMaxRune == 0){
				break;
			}

			auto iter = m_EquipRuneArrayMap.find(n16RunePage);
			if (iter == m_EquipRuneArrayMap.end()){
				EquipRuneArray sEquipRuneArray;
				sEquipRuneArray.AddElement(runeID);

				m_EquipRuneArrayMap.insert(std::make_pair(n16RunePage, sEquipRuneArray));

				toPos = n16RunePage * c_n32MaxSlotInRunePage;

				ifEquipSuccess = true;
			}
			else{
				EquipRuneArray& sEquipRuneArray = iter->second;
				const int curPos = sEquipRuneArray.GetCurSize();
				if (curPos >= canGetMaxRune){
					break;
				}
				
				ifEquipSuccess = true;

				INT32 removePos = sEquipRuneArray.AddElement(runeID);

				toPos = n16RunePage * c_n32MaxSlotInRunePage + removePos;
			}
		} while (false);

		if (!ifEquipSuccess){
			return eEC_AskUseRunesFail;
		}

		bool res = AddRuneNum(runeID, -1, true);
		if (!res){
			ELOG(LOG_ERROR, "");
			return eEC_AskUseRunesFail;
		}

		UpdateRuneBagJson();
		UpdateRuneSlotJson();

		GSToGC::NotifyRunesList sMsg;
		auto pInfo = sMsg.add_runes_slot_info();
		pInfo->set_runeid(runeID);
		pInfo->set_slotpos(toPos);

		GetCSKernelInstance()->PostMsgToGC(m_sUserNetInfo, sMsg, sMsg.msgid());
		ELOG(LOG_DBBUG, "euip rune(%u) to pos(%d) success", runeID, toPos);
		//日志
		{
			stringstream mystream; 
			mystream << runeID << LOG_SIGN;
			mystream << n16RunePage << LOG_SIGN; 
			mystream << toPos;
			CSSGameLogMgr::GetInstance().AddGameLog(eLog_RuneUse,this->GetUserDBData().sPODUsrDBData.un64ObjIdx,mystream.str() );
		}
		
		return eNormal;
	}

	INT32 CCSUser::GCAskUnloadRunes(INT32 page, INT32 n16RuneFromPos)
	{
		ELOG(LOG_DBBUG, "from page:%d pos:%d", page, n16RuneFromPos);
		bool ifUnEquipSuccess = false;
		UINT32 runeId = 0;
		do {
			if (page > m_RunePageMaxEquipNum.size())
			{
				ELOG(LOG_ERROR, "n16RunesToPos(%d) not valid", page);
				break;
			}

			auto iter = m_EquipRuneArrayMap.find(page);
			if (iter == m_EquipRuneArrayMap.end()){
				break;
			}
			else{
				EquipRuneArray& sEquipRuneArray = iter->second;
				UINT32* pRuneId = sEquipRuneArray.GetValueFromIndex(n16RuneFromPos);
				if (!pRuneId){
					ELOG(LOG_ERROR, "符文id找不到");
					break;
				}

				ifUnEquipSuccess = true;

				/*UINT32*/ runeId = *pRuneId;

				sEquipRuneArray.RemoveIndex(n16RuneFromPos);

				AddRuneNum(runeId, 1, true);

				GSToGC::UnloadRune sMsg;
				sMsg.set_page(page);
				sMsg.set_pos(n16RuneFromPos);

				GetCSKernelInstance()->PostMsgToGC(m_sUserNetInfo, sMsg, sMsg.msgid());
				ELOG(LOG_DBBUG, "remove equip rune page(%u) pos:%d", page, n16RuneFromPos);
			}
		} while (false);

		if (!ifUnEquipSuccess){
			ELOG(LOG_ERROR, "符文卸载失败");
			return eEC_AskUseRunesFail;
		}

		UpdateRuneBagJson();
		UpdateRuneSlotJson();
		//日志
		{
			stringstream mystream;
			mystream << page << LOG_SIGN;
			mystream << runeId << LOG_SIGN;
			mystream << n16RuneFromPos;
			CSSGameLogMgr::GetInstance().AddGameLog(eLog_RuneUnUse,this->GetUserDBData().sPODUsrDBData.un64ObjIdx,mystream.str() );
		}
		return eNormal;
	}


	INT32 CCSUser::GCAskComposeRunes(GCToCS::ComposeRunes& sMsg)
	{
		if (c_un32ComposeRunesNum != sMsg.runesid_size())
		{
			ELOG(LOG_ERROR, "invalid runes size!");
			return eEC_AskComposeRunesFail;
		}
		bool ifHvBuyAllRunes = IfHasBuyRunes(sMsg.runesid(0)) && IfHasBuyRunes(sMsg.runesid(1)) && IfHasBuyRunes(sMsg.runesid(2));
		if (!ifHvBuyAllRunes){
			ELOG(LOG_ERROR, "");
			return eEC_AskComposeRunesFail; 
		}

		int level = 0;
		bool bIfSameID = ( sMsg.runesid(0) == sMsg.runesid(1)) && (sMsg.runesid(0) == sMsg.runesid(2));
		SRunesCfg* pRuneCfg = nullptr;
		if (false == bIfSameID){
			const SRunesCfg* pCfg1 = CCSCfgMgr::getInstance().GetRunesCfg(sMsg.runesid(0));
			const SRunesCfg* pCfg2 = CCSCfgMgr::getInstance().GetRunesCfg(sMsg.runesid(1));
			const SRunesCfg* pCfg3 = CCSCfgMgr::getInstance().GetRunesCfg(sMsg.runesid(2));
			if (NULL == pCfg1 || !pCfg2 || !pCfg3){
				ELOG(LOG_ERROR, "");
				return eEC_NULLCfg;
			}

			if (pCfg1->un8Level != pCfg2->un8Level || pCfg1->un8Level != pCfg3->un8Level){
				ELOG(LOG_ERROR, "level is diff when random compose!");
				return eEC_AskComposeRunesFail;
			}

			pRuneCfg = CCSCfgMgr::getInstance().GetRanmRuneFromLevel(pCfg1->un8Level + 1);
			if (NULL == pRuneCfg){
				ELOG(LOG_ERROR, "等级相同符文合成失败");
				return eEC_AskComposeRunesFail;
			}

			level = pCfg1->un8Level;
		}
		else{
			const SRunesCfg* pCfg1 = CCSCfgMgr::getInstance().GetRunesCfg(sMsg.runesid(0));
			if (NULL == pCfg1){
				ELOG(LOG_ERROR, "");
				return eEC_NULLCfg;
			}
			pRuneCfg = CCSCfgMgr::getInstance().GetNextLevelRunesFromFixedRunesID(pCfg1->eOT);
			if (NULL == pRuneCfg){
				ELOG(LOG_ERROR, "同一ID符文 找不到下一随机符文");
				return eEC_AskComposeRunesFail;
			}

			level = pCfg1->un8Level;
		}

		bool ifCostSucess = ConsumeCombine(level);
		if (!ifCostSucess){
			ELOG(LOG_ERROR, "等级相同符文合成失败");
			return eEC_AskComposeRunesFail;
		}

		vector<UINT32> toRemoveRunsVec;
		toRemoveRunsVec.push_back(sMsg.runesid(0));
		toRemoveRunsVec.push_back(sMsg.runesid(1));
		toRemoveRunsVec.push_back(sMsg.runesid(2));
		DecreaseRune(toRemoveRunsVec, true);

		AddRunes(pRuneCfg->eOT, true);
		 //log
		{
			stringstream mystream; 
			mystream << sMsg.runesid(0) << LOG_SIGN_FIRST;
			mystream << sMsg.runesid(1) << LOG_SIGN_FIRST;
			mystream << sMsg.runesid(2) << LOG_SIGN;
			mystream <<pRuneCfg->eOT;
			CSSGameLogMgr::GetInstance().AddGameLog(eLog_RuneCompose,this->GetUserDBData().sPODUsrDBData.un64ObjIdx,mystream.str());	
		}
		
		return eNormal;
	}

	INT32 CCSUser::GCAskUserGameInfo()
	{
		return PostMsgToGC_NotifyUserGameInfo();
	}

	bool CCSUser::IsFirstWin()
	{
		INT64 CurTime = time(NULL) / c_OneDaySec;
		INT64 tempSpan = CurTime - (m_sUserDBData.sPODUsrDBData.tLastFirstWinTime / c_OneDaySec);	
		return tempSpan > 0 ? true : false ;
	}

	INT32 CCSUser::ResetPingTimer(){
		m_tGCLastPing = GetUTCMiliSecond();
		return eNormal;
	}

	INT32 CCSUser::AskBuyGoods(GCToCS::AskBuyGoods& sGoods){
		INT32 n32BuyRes = eNormal;
		INT32 n32Conmsume = 0;
		EGoodsType lEGoodsType = eGoodsType_None;
		UINT32 goodsId = 0;
		bool ifBuySuccess = false;

		if (IfHeroCfgID(sGoods.commondityid())){
			auto pCfg = CCSCfgMgr::getInstance().GetHeroBuyCfg(sGoods.commondityid());
			if (NULL == pCfg){
				return eEC_NULLCfg;
			}
 			CheckHeroValidTimer(time(NULL));

			if (IfHasBuyHero(pCfg->un32HeroID)){
				return eEC_HaveBuySameGoods;
			}
			lEGoodsType = eGoodsType_Hero;
			goodsId = pCfg->un32HeroID;
			n32Conmsume  = CCSCfgMgr::getInstance().GetComsume(eGoodsType_Hero, (EComsumeType)sGoods.consumetype(), sGoods.commondityid());
		}
		else if(IfRuneCfgID(sGoods.commondityid())){
			auto pCfg = CCSCfgMgr::getInstance().GetRunesCfg(sGoods.commondityid());
			if (NULL == pCfg){
				return eEC_NULLCfg;
			}

			if (sGoods.num() < 1){
				return eEC_InvalidPara;
			}

			lEGoodsType = eGoodsType_Runes;
			goodsId = pCfg->eOT;
			n32Conmsume  = CCSCfgMgr::getInstance().GetComsume(eGoodsType_Runes, (EComsumeType)sGoods.consumetype(), sGoods.commondityid()) * sGoods.num();
		}

		do {
			switch (sGoods.consumetype()){
			case eComsume_Gold:
				if (n32Conmsume <= m_sUserDBData.sPODUsrDBData.n64Gold){
					m_sUserDBData.ChangeUserDbData(eUserDBType_Gold, -n32Conmsume);
					ifBuySuccess = true;
					ELOG(LOG_SpecialDebug, "购买花费金币%d，剩余:%lld", n32Conmsume, m_sUserDBData.sPODUsrDBData.n64Gold);
				}
				else{
					return eEC_NotEnoughGold;
				}
				break;
			case eConsmue_Diamond:
				if (n32Conmsume <= m_sUserDBData.sPODUsrDBData.n64Diamond){
					m_sUserDBData.ChangeUserDbData(eUserDBType_Diamond, -n32Conmsume);
					ELOG(LOG_SpecialDebug, "购买花费钻石%d，剩余:%lld", n32Conmsume, m_sUserDBData.sPODUsrDBData.n64Diamond);
					ifBuySuccess = true;
				}
				else{
					return eEC_DiamondNotEnough;
				}
				break;
			default:
				ELOG(LOG_ERROR, "Not Right ConsumeType!");
				break;
			}
		} while (false);

		if (ifBuySuccess){
			switch(lEGoodsType)
			{
			case eGoodsType_Hero:
				AddHero(sGoods.commondityid(), false);
				break;
			case eGoodsType_Runes:
				AddRunes(goodsId, true, sGoods.num());
				break;
			default:
				break;
			}

			switch (sGoods.consumetype()){
			case eComsume_Gold:
				SynCurGold();
				break;
			case eConsmue_Diamond:
				SynCurDiamond();
				break;
			}

			{  
				stringstream mystream;
				mystream << sGoods.commondityid() << LOG_SIGN << sGoods.consumetype() << LOG_SIGN << n32Conmsume;
				CSSGameLogMgr::GetInstance().AddGameLog(eLog_BuyGoods,this->GetUserDBData().sPODUsrDBData.un64ObjIdx,mystream.str());
			}
		} 
		return eNormal;
	}

	bool CCSUser::IfHasBuyHero(UINT32 un32HeroID){
		auto iter = m_sUserDBData.heroListMap.find(un32HeroID);
		if (iter != m_sUserDBData.heroListMap.end() && iter->second.endTime == -1){
			return true;
		}
		return false;
	}

	bool CCSUser::IfHasSkinsGoods(UINT32 un32HeroID){
		return FALSE;
	}
	bool	CCSUser::CheckIfEnoughPay(EPayType type, INT32 pay)
	{
		switch(type){
		case ePayType_Gold:
			return pay <= GetGold() ? true : false;
		case ePayType_Diamond:
			return pay <= GetUserDBData().sPODUsrDBData.n64Diamond ? true : false;
		default:
			return false;
		}
	}

	void CCSUser::OnNewDay()
	{
		SynUserCLDays();//新的一天
	}
	void CCSUser::OnNewMonth()
	{
		m_sUserDBData.ChangeUserDbData(eUserDBType_LastGetLoginReward, 0);
		m_sUserDBData.ChangeUserDbData(eUserDBType_CLDay, 0);
		SynUserCLDays();//新的一月
	}
	void CCSUser::OnNewYear()
	{
	}

	void CCSUser::LoginRewardInit()
	{
		const boost::gregorian::date& today = GetCSUserMgrInstance()->GetToday();
		UINT32 curDays = today.julian_day();//当天是第几天(1900年1月1日)
		UINT32 baseDays = curDays-today.day()+1;//月初是第几天(1900年1月1日)
		UINT32 lastDays = m_sUserDBData.sPODUsrDBData.un32LastGetLoginRewardDay;//上次是第几天(1900年1月1日)(数据库)
		UINT32 lastCount = m_sUserDBData.sPODUsrDBData.un16Cldays;//本月签到次数(数据库)
		if (lastDays<baseDays){//当月未签到
			m_sUserDBData.ChangeUserDbData(eUserDBType_LastGetLoginReward, 0);
			m_sUserDBData.ChangeUserDbData(eUserDBType_CLDay, 0);
		}
	}

	INT32 CCSUser::AskGetLoginReward()
	{
		const boost::gregorian::date& today = GetCSUserMgrInstance()->GetToday();
		UINT32 curDays = today.julian_day();//当天是第几天(1900年1月1日)
		UINT32 baseDays = curDays - today.day()+1;//月初是第几天(1900年1月1日)
		UINT32 lastDays = m_sUserDBData.sPODUsrDBData.un32LastGetLoginRewardDay;//上次是第几天(1900年1月1日)(数据库)
		UINT32 lastCount = m_sUserDBData.sPODUsrDBData.un16Cldays;//本月签到次数(数据库)
		if (lastDays < curDays && lastCount < GetCSUserMgrInstance()->GetMonthDays()){//当日未签到
			if (lastDays < baseDays){//当月未签到
				lastDays = 0;
				lastCount = 0;
			}
			lastDays = curDays;
			++lastCount;
			SLoginReward* pLoginReward = CCSCfgMgr::getInstance().GetLoginReward(lastCount);
			if (pLoginReward){
				m_sUserDBData.ChangeUserDbData(eUserDBType_LastGetLoginReward, lastDays);
				m_sUserDBData.ChangeUserDbData(eUserDBType_CLDay, lastCount);
				SynUserCLDays();
				//////////////////////////////////////////////////////////////////////////
				for (int i=0;i<MAX_REWARD;++i){
					int goodsId = pLoginReward->eItemType[i];
					if (goodsId==eLoginRewadItem_None) continue;
					else if (goodsId==eLoginRewadItem_Gold) { 
						m_sUserDBData.ChangeUserDbData(eUserDBType_Gold, pLoginReward->un32num[i]);
						SynCurGold();
					}
					else if (goodsId==eLoginRewadItem_Diamond) {
						m_sUserDBData.ChangeUserDbData(eUserDBType_Diamond, pLoginReward->un32num[i]);
						SynCurDiamond();
					}
					else if (IfHeroCfgID(goodsId)) {
						AddHero(goodsId, true);
					}
					else if (IfSkinCfgID(goodsId)) {}
					else if (IfRuneCfgID(goodsId)) AddRunes(goodsId, true);
					else if (IfOtherCfgID(goodsId)){
						ChangeOtherItemMap(goodsId, *(pLoginReward->un32num));
					}
					else { Assert(false && "can't get this wards."); }
				}
			}
			PostMsgToGC_NotifyGetLoginRewardScuess();

			//log
			CSSGameLogMgr::GetInstance().AddGameLog(eLog_Daily,this->GetUserDBData().sPODUsrDBData.un64ObjIdx,curDays);
		}
		
		return eNormal;
	}
	//******************friend system function *********//

	INT32	CCSUser::AskRemoveFromSNSList(const UINT64 guidIdx, INT32 eFriendType)
	{
		ERelationShip eRelationShip = (ERelationShip)eFriendType;
		AskRemFromSNSMap(guidIdx, eRelationShip);
		//PostMsgToGC_NotifuUserReMoveFromSNSList(eRelationShip, guidIdx);
		//remove from db
		SUserRelationshipInfo t_info;
		t_info.guididx = guidIdx;
		t_info.eRelationShip = eRelationShip;
		GetCSUserMgrInstance()->UpdateSNSList(GetGUID(), t_info, eOperationType_Del);
		
		//if friend and user on line, remove
		if (eRSType_Friends == (ERelationShip)eFriendType){
			CCSUser *piUser = GetCSUserMgrInstance()->GetUserByGuid(guidIdx);
			if (NULL != piUser){
				piUser->AskRemFromSNSMap(GetGUID(), eRelationShip);
			}
		}
		//log
		CSSGameLogMgr::GetInstance().AddGameLog( (eRSType_Friends == (ERelationShip)eFriendType) ? eLog_FriendDel:eLog_EnemyFriendDel,this->GetUserDBData().sPODUsrDBData.un64ObjIdx,guidIdx);
		return eNormal;
	}

	INT32	CCSUser::AskAddToSNSlistByNickName(const string &sz_nickname, ERelationShip type)
	{
		CCSUser *piUser = GetCSUserMgrInstance()->GetUserByNickName(sz_nickname);
		if (eRSType_Friends == type)
		{
			return AddToFriendsLlist(piUser, type);
		}
		else{
			if (NULL == piUser){
				return eEC_UserOfflineOrNullUser;
			}
			else{
				return AddToBlackList(piUser->GetGUID());
			}
		}
	}

	INT32	CCSUser::AskAddToSNSlistByGuid(const UINT64 guid, ERelationShip type)
	{
		if (eRSType_Friends == type)
		{
			CCSUser *piUser = GetCSUserMgrInstance()->GetUserByGuid(guid);
			return AddToFriendsLlist(piUser, type);
		}
		else{
			//log
			CSSGameLogMgr::GetInstance().AddGameLog(eLog_EnemyFriendAdd,this->GetUserDBData().sPODUsrDBData.un64ObjIdx,guid);
			return AddToBlackList(guid);
		}
	}

	INT32	CCSUser::AddToFriendsLlist(CCSUser *piUser, ERelationShip eRStype)
	{
		if (NULL == piUser){
			return eEC_UserOfflineOrNullUser;
		}

		if (piUser->GetGUID() == GetGUID()){
			return eNormal;
		}

		if (piUser->GetUserPlayingStatus() == eUserPlayingStatus_OffLine){
			return eEC_UserOfflineOrNullUser;
		}

		if (piUser->GetUserBattleInfoEx().GetBattleID() != 0){
			return eEC_UserWasPlaying;
		}

		if ( CheckIfInFriendList(piUser->GetGUID()) ){
			return eEC_JustInFriendsList;
		}
		if (CheckIfInBlacklist(piUser->GetGUID())){
			return eEC_UserInYourBlackList;
		}
		if (c_un8MaxFriends <= m_sUserDBData.friendListMap.size()){
			return eEC_FriendsListFull;
		}
		if (piUser->CheckIfInBlacklist(GetGUID()) ){
			return eEC_YouInOppositeBlackList;
		}
		if (c_un8MaxFriends < piUser->GetUserFriendsListSize()){
			eEC_CounterpartFriendListFull;
		}
		//save temp ask message
		TIME_MILSEC tCurMil = GetUTCMiliSecond();
		auto iter = m_cAddFVec.find(piUser->GetGUID());
		if (m_cAddFVec.end() != iter){
			if (c_un32MinOpertionTime >= tCurMil - iter->second.tMilSec){
				return eEC_AskHaveSend;
			}
			else{
				iter->second.tMilSec = tCurMil;
			}
		}
		else{
			SUserRelationshipInfo sRSinfo(piUser->GetNickName(), 0, eRStype, tCurMil, piUser->GetGUID(), piUser->GetUserDBData().sPODUsrDBData.un16VipLv);
			m_cAddFVec[piUser->GetGUID()] = sRSinfo;
		}

		return piUser->PostMsgToGC_NotifyBeAddFriendMsg(GetNickName(), GetGUID(),piUser->GetNickName());
	}

	INT32	CCSUser::AddToBlackList(const UINT64 guid)
	{
		if (guid == GetGUID()){
			return eNormal;
		}

		//check if in black list
		if ( CheckIfInBlacklist(guid) ){
			return eEC_JustInBlackList;
		}

		SUserRelationshipInfo temp_rsinfo;
		//check if in friends list
		if ( CheckIfInFriendList(guid) ){
			//get friends relation info
			GetFriendRSInfo(guid, temp_rsinfo, eRSType_Friends);
			//remove from friends list
			AskRemoveFromSNSList(guid, eRSType_Friends);
			temp_rsinfo.eRelationShip = eRSType_Detestation;
		}
		else{
			CCSUser *piUser = GetCSUserMgrInstance()->GetUserByGuid(guid);
			if (piUser == NULL)
			{
				return eEC_UserOfflineOrNullUser;
			}
			else{
				temp_rsinfo.eRelationShip = eRSType_Detestation;
				temp_rsinfo.guididx = guid;
				temp_rsinfo.nHeadId = piUser->GetHeadID();
				temp_rsinfo.stNickName = piUser->GetNickName();
			}
		}

		m_sUserDBData.blackListMap[guid] = temp_rsinfo;
		GetCSUserMgrInstance()->UpdateSNSList(GetGUID(), temp_rsinfo, eOperationType_Add);
		SynUserSNSList(guid, eRSType_Detestation);
		return eNormal;
	}

	INT32	CCSUser::RemoveUserFromAddVec(UINT64 Idx)
	{
		map<UINT64, SUserRelationshipInfo>::iterator iter = m_cAddFVec.find(Idx);
		if (m_cAddFVec.end() != iter){
			iter = m_cAddFVec.erase(iter);
		}
		return eNormal;
	}

	INT32	CCSUser::GetFriendRSInfo(const UINT64 guid, SUserRelationshipInfo &temp_info, ERelationShip type)
	{
		if (eRSType_Friends == type)
		{
			auto iter = m_sUserDBData.friendListMap.find(guid);
			if (iter != m_sUserDBData.friendListMap.end())
			{
				temp_info = iter->second;
			}
		}
		else{
			auto iter = m_sUserDBData.blackListMap.find(guid);
			if (iter != m_sUserDBData.friendListMap.end())
			{
				temp_info = iter->second;
			}
		}
		return eNormal;
	}

	INT32	CCSUser::AskInviteFriendsToBattle(UINT64 un64RoomID, UINT64 guidIdx)
	{
		CCSUser *piUser = GetCSUserMgrInstance()->GetUserByGuid(guidIdx);
		if (NULL == piUser){
			return eEC_NullUser;
		}

		CCSBattleRoom* pBattleRoom = GetBattleMgrInstance()->GetRoom(un64RoomID);
		if (NULL == pBattleRoom){
			return eEC_NullBattle;;
		}
		string pwd = pBattleRoom->GetPwd();
		piUser->PostMsgToGC_UserBeInvitedToBattle(un64RoomID, pwd, GetNickName());
		return eNormal;
	}

	bool	CCSUser::CheckIfInFriendList(const UINT64 guidIdx)
	{
		return m_sUserDBData.friendListMap.find(guidIdx) != m_sUserDBData.friendListMap.end();
	}

	bool	CCSUser::CheckIfInBlacklist(const UINT64 guidIdx)
	{
		return m_sUserDBData.blackListMap.find(guidIdx) != m_sUserDBData.blackListMap.end();
	}

	bool	CCSUser::CheckIfInAddSNSList(const UINT64 guididx)
	{
		auto iter_b = m_cAddFVec.find(guididx);
		if (iter_b != m_cAddFVec.end()){
			return true;
		}
		return false;
	}

	INT32	CCSUser::AskSendMsgToUser(const UINT64 rGuidIdx,INT32 n32MsgLength, const CHAR *pbMsg)
	{
		if (c_un8MaxMsgLength < n32MsgLength){
			return eEC_MsgTooLarge;
		}
		if (GetGUID() == rGuidIdx){
			return eNormal;
		}

		CCSUser *piUserTo = GetCSUserMgrInstance()->GetUserByGuid(rGuidIdx); 
		if (NULL == piUserTo){
			return eEC_UserNotExist;
		}

		if (piUserTo->CheckIfInBlacklist(GetGUID())){
			return eEC_UserRefuseReceiveYourMsg;
		}

		if (eUserPlayingStatus_OffLine == piUserTo->GetUserPlayingStatus() || 0 != piUserTo->GetUserBattleInfoEx().GetBattleID()){
			return eEC_UserOfflineOrNullUser;
		}

		piUserTo->PostMsgToGC_MsgFromUser(GetGUID(), GetNickName(), GetHeadID(), n32MsgLength, pbMsg);

		return eNormal;
	}

	void	CCSUser::AskChangeheaderId(const UINT32 un32headerid)
	{
		m_sUserDBData.ChangeUserDbData(eUserDBType_HeaderId, un32headerid);
		PostMsgToGC_NotifyNewHeaderid(GetGUID(), un32headerid);
		//notify new header to on-line friend
		auto iter = m_sUserDBData.friendListMap.begin();
		for (; iter != m_sUserDBData.friendListMap.end(); iter ++){
			CCSUser *piUser = GetCSUserMgrInstance()->GetUserByGuid(iter->second.guididx);
			if (piUser != NULL && piUser->GetUserPlayingStatus() == eUserPlayingStatus_Playing){
				piUser->SynUserSNSList(GetGUID(), eRSType_Friends);
			}
		}
		//日志
		{
			stringstream mystream;
			mystream << un32headerid << LOG_SIGN << this->GetUserDBData().szNickName;
			CSSGameLogMgr::GetInstance().AddGameLog(eLog_HeadUse,this->GetUserDBData().sPODUsrDBData.un64ObjIdx,mystream.str());
		} 
	}

	//符文洗练
	INT32	CCSUser::AskRecoinRune(UINT32 rune_id, INT32 cradID)
	{
		auto iter = m_RunesMap.find(rune_id);
		if (iter == m_RunesMap.end()){
			return eEC_DidNotHaveThisItem;
		}

		auto pOriCfg = CCSCfgMgr::getInstance().GetRunesCfg(rune_id);
		if (!pOriCfg){
			return eEC_DidNotHaveThisItem;
		}

		SRunesCfg* pRuneCfg = NULL;
		if (cradID <= 0){
			int needgold = 0;
			switch(pOriCfg->un8Level)
			{
			case 1:
				needgold = 3;
				break;
			case 2:
				needgold = 10;
				break;
			case 3:
				needgold = 30;
				break;
			case 4:
				needgold = 70;
				break;
			case 5:
				needgold = 200;
				break;
			case 6:
				needgold = 480;
				break;
			}

			if (needgold > m_sUserDBData.sPODUsrDBData.n64Gold){
				return eEC_NotEnoughGold;
			}

			pRuneCfg = CCSCfgMgr::getInstance().GetRanmRuneFromLevel(pOriCfg->un8Level);
			if (NULL == pRuneCfg){
				ELOG(LOG_ERROR, "同一ID符文 找不到下一随机符文");
				return eEC_WashRuneFail;
			}

			m_sUserDBData.ChangeUserDbData(eUserDBType_Gold, -needgold);
			SynCurGold();
		}
		else{
			if (cradID < RuneWashCfgBegin || cradID > RuneWashCfgEnd){
				ELOG(LOG_ERROR, "不合法洗练券");
				return eEC_WashRuneFail;
			}
			auto level = pOriCfg->un8Level;
			auto pCurCfg = CCSCfgMgr::getInstance().GetOtherItemCfg(cradID);
			auto cardIter = m_sUserDBData.item_Map.find(cradID);
			if (cardIter == m_sUserDBData.item_Map.end()){
				ELOG(LOG_ERROR, "找不到对应洗练券");
				return eEC_AskComposeRunesFail;
			}

			if (pCurCfg->effect_value < level){
				ELOG(LOG_ERROR, "洗练等级错误!");
				return eEC_AskComposeRunesFail;
			}

			pRuneCfg = CCSCfgMgr::getInstance().GetRanmRuneFromLevel(level);
			if (pRuneCfg){
				auto itemID = cardIter->first;
				auto num = --cardIter->second.item_num;
				INT64 endTime = cardIter->second.end_time;
				GSToGC::NotifyOtherItemInfo Msg;

				if (num== 0){
					m_sUserDBData.item_Map.erase(cardIter);
				}
				GSToGC::NotifyOtherItemInfo_OtherItem *item = Msg.add_item();
				item->set_item_id(itemID);
				item->set_item_num(num);
				item->set_expired_time(endTime);
				PostMsgToGC(Msg, Msg.msgid());
			}

			if (!pRuneCfg){
				ELOG(LOG_ERROR, "同一ID符文 找不到下一随机符文");
				return eEC_AskComposeRunesFail;
			}
		}

		AddRuneNum(rune_id, -1, true);
		AddRuneNum(pRuneCfg->eOT, 1, true);

		UpdateRuneBagJson();
		CCSUserDbDataMgr::InsertOrUpdateRunesBagDataAndToDBThread(m_sUserDBData.sPODUsrDBData.un64ObjIdx, m_RuneBagStream.str()); 
		//log 
		{
			stringstream mystream; 
			mystream << rune_id << LOG_SIGN;
			mystream << cradID << LOG_SIGN;
			mystream << pRuneCfg->eOT ;
			CSSGameLogMgr::GetInstance().AddGameLog(eLog_RuneWashing,this->GetUserDBData().sPODUsrDBData.un64ObjIdx, mystream.str() );
		}
		
		return 0;
	}

	INT32	CCSUser::ReplyAddFriendRequst(INT8 n8Reply, const UINT64 guid_asker, const UINT64 RecGuidIdx)
	{
		CCSUser *piUser = GetCSUserMgrInstance()->GetUserByGuid(guid_asker);
		if (NULL == piUser){
			return eEC_NullUser;
		}

		if(TRUE == n8Reply && piUser->CheckIfInAddSNSList(RecGuidIdx)){
			SUserRelationshipInfo temp_info(piUser->GetNickName(), piUser->GetHeadID(), eRSType_Friends, 0, piUser->GetGUID(), piUser->GetUserDBData().sPODUsrDBData.un16VipLv);
			m_sUserDBData.friendListMap[piUser->GetGUID()] = temp_info;
			//receiver 
			SynUserSNSList(guid_asker, eRSType_Friends);
			GetCSUserMgrInstance()->UpdateSNSList(GetGUID(), temp_info, eOperationType_Add);
			//asker
			piUser->SynUserSNSList(RecGuidIdx, eRSType_Friends);
			SUserRelationshipInfo t_info(GetNickName(), GetHeadID(), eRSType_Friends, 0, GetGUID(), GetUserDBData().sPODUsrDBData.un16VipLv);
			piUser->AskAddToSNSMap(guid_asker, t_info);
			GetCSUserMgrInstance()->UpdateSNSList(guid_asker, t_info, eOperationType_Add);
		}
		else{
			GetCSUserMgrInstance()->PostMsgToGC_AskReturn(piUser->GetUserNetInfo(), GCToCS::eMsgToGSToCSFromGC_AskAddToSNSList, eEC_UserRefuseAddFriends);
		}
		piUser->RemoveUserFromAddVec(RecGuidIdx);

		//log
		CSSGameLogMgr::GetInstance().AddGameLog(eLog_FriendAdd,piUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,0);
		return eNormal;
	} 

	INT32	CCSUser::AddHero(const SUserHeroDBData &db)

	{
		m_sUserDBData.heroListMap.insert(std::make_pair(db.un32HeroID, db));
		//将英雄加入数据库!
		string sql;
		CCSUserDbDataMgr::GetNewUserDbHerosData(m_sUserDBData.sPODUsrDBData.un64ObjIdx, db, sql);
		GetCSUserMgrInstance()->PostUserCacheMsgToDBThread(GetGUID(), sql);

		return eNormal;
	}

	INT32 CCSUser::UpdateHero(const UINT32 heroid, const INT64 endtime)
	{		
		string sql;
		CCSUserDbDataMgr::GetUpdateDbHeroData(GetGUID(), heroid, endtime, sql);

		GetCSUserMgrInstance()->PostUserCacheMsgToDBThread(GetGUID(), sql);

		return eNormal;
	}	 

	bool CCSUser::GetHeroVec(vector<HeroListStruct>& heroVec){
		for (auto iter = m_sUserDBData.heroListMap.begin(); iter != m_sUserDBData.heroListMap.end(); ++iter){
			HeroListStruct sHeroListStruct(iter->first, iter->second.endTime, false);
			if (sHeroListStruct.expiredTime != -1){
				sHeroListStruct.expiredTime = iter->second.endTime + iter->second.buyTime - time(NULL);
			}
			heroVec.push_back(sHeroListStruct);
		}

		for (auto iter_f = CCSCfgMgr::getInstance().GetHeroBuyCfgmap().begin(); CCSCfgMgr::getInstance().GetHeroBuyCfgmap().end() != iter_f; iter_f ++){
			SHeroBuyCfg& pCfg = iter_f->second;
			if (!pCfg.bIfShowInShop){
				continue;
			}
			for (auto consumeIter = pCfg.sConsumeList.Begin(); consumeIter != pCfg.sConsumeList.End(); consumeIter = pCfg.sConsumeList.Next()){
				ConsumeStruct& sConsumeStruct = *consumeIter;
				if (sConsumeStruct.type == eConsumeType_Free && iter_f->second.un32HeroID > 0){
					HeroListStruct sHeroListStruct(iter_f->second.un32HeroID, 0, true);
					heroVec.push_back(sHeroListStruct);
				}
			}
		} 
		return true;
	}

	bool CCSUser::GetHeroVec(vector<UINT32>& heroVec){
		for (auto iter = m_sUserDBData.heroListMap.begin(); iter != m_sUserDBData.heroListMap.end(); ++iter){
				heroVec.push_back(iter->second.un32HeroID);
		}

		for (auto iter_f = CCSCfgMgr::getInstance().GetHeroBuyCfgmap().begin(); CCSCfgMgr::getInstance().GetHeroBuyCfgmap().end() != iter_f; iter_f ++){
			SHeroBuyCfg& pCfg = iter_f->second;
			for (auto consumeIter = pCfg.sConsumeList.Begin(); consumeIter != pCfg.sConsumeList.End(); consumeIter = pCfg.sConsumeList.Next()){
				ConsumeStruct& sConsumeStruct = *consumeIter;
				if (sConsumeStruct.type == eConsumeType_Free && iter_f->second.un32HeroID > 0){
					heroVec.push_back(iter_f->second.un32HeroID);
				}
			}
		} 
		return true;
	}

	void CCSUser::CheckHeroValidTimer(int64_t curTime){
		vector<UINT32> expiredVec;
		string expireSql;
		for (auto iter = m_sUserDBData.heroListMap.begin(); iter != m_sUserDBData.heroListMap.end();){
			SUserHeroDBData& pData = iter->second;
			if (pData.endTime == PersistTimeAlways){
				++iter;
			}
			else if (pData.buyTime + pData.endTime < curTime){
				iter = m_sUserDBData.heroListMap.erase(iter);

				expiredVec.push_back(pData.un32HeroID);

				CCSUserDbDataMgr::GetDelUserDbHerosSql(GetGUID(), pData.un32HeroID, expireSql);
			}
			else{
				++iter;
			}
		}

		if (!expiredVec.empty()){
			//通知客户端 英雄过期
			GSToGC::NotifyGoodsExpired sMsg;
			for (auto iter = expiredVec.begin(); iter != expiredVec.end(); ++iter){
				sMsg.add_commondityid(*iter);
			}

			PostMsgToGC(sMsg, sMsg.msgid());

			//通知从数据库删除
			GetCSUserMgrInstance()->PostUserCacheMsgToDBThread(GetGUID(), expireSql);
		}
	}

	void CCSUser::CheckDbSaveTimer(int64_t curTime, int64_t tickSpan){
		if (m_eUserPlayingStatus == eUserPlayingStatus_Playing){
			GetCSUserMgrInstance()->DBPoster_UpdateUser(this);
		}

		GetCSUserMgrInstance()->CheckPostDelayNewMail(this);
	}

	INT32 CCSUser::KickOutOldUser()
	{
		if (m_sUserNetInfo.IsValid()){
			PostMsgToGC_NetClash();
			const ICSGSInfo *piGSInfo = GetCSKernelInstance()->GetGSInfoByGSID(m_sUserNetInfo.n32GSID);
			if (NULL != piGSInfo){
				GetCSKernelInstance()->PostMsgToGS_KickoutGC(piGSInfo,m_sUserNetInfo.n32GCNSID);
			}
			OnOffline();
		}
		return eNormal;
	}

	void CCSUser::OnOnline(SUserNetInfo& netinfo,GCToCS::Login &pLogin,bool isFirstInDB,bool isFirstInMem,bool isReLogin/*=false*/)
	{
		KickOutOldUser();
		ResetPingTimer();
		LoginRewardInit();
		time_t nowTime = time(NULL);
		if (!isReLogin) 
			m_sUserDBData.ChangeUserDbData(eUserDBType_Channel, pLogin.platform());
		GetCSUserMgrInstance()->OnUserOnline(this, netinfo);
		NotifyUserPlayState();
		SynUser_IsOnSS();
		SynUser_UserBaseInfo();
		SynCommidityCfgInfo();
		SynUserCLDays();
		SynUser_AllHeroList();
		SynUser_AllRunesList();
		CalculateItemAddition();
		SynOtherItemInfo(0);
		GetCSUserMgrInstance()->PostUserCurtMailList(this);
		SynUserSNSList(0, eRSType_Friends);
		SynUserSNSList(0, eRSType_Detestation);
		PosUserCSCurtGuideSteps();
		GetTaskMgr()->NotifyAllTask();
		if (!isFirstInDB)
		{
			PostCSNotice();
		}

		if ( isReLogin){
			//重连
			CSSGameLogMgr::GetInstance().AddGameLog(eLog_UserRecon,this->GetUserDBData().sPODUsrDBData.un64ObjIdx,0); 
		}
		 
		if (GetUserBattleInfoEx().GetBattleState()<eBattleState_Async)
		{//由cs进行管理//
			if (GetUserBattleInfoEx().GetBattleState()!=eBattleState_Free)
			{
				ELOG(LOG_ERROR,"战斗类型(%u)战斗状态(%u)战斗ID(%u)房间(%u)队伍(%u)",
					GetUserBattleInfoEx().GetBattleType(),
					GetUserBattleInfoEx().GetBattleState(),
					GetUserBattleInfoEx().GetBattleID(),
					GetRoomPlayer()->m_RoomID,GetMatchPlayer()->m_MatchTeamId);
				Assert(false);
			}
		}
		else
		{//由ss进行管理，只通知ss一下//
			GetBattleMgrInstance()->NotifyBattleSSUserIsOnline(this, true);
		}
	}

	void CCSUser::OnOffline()
	{
		GetCSUserMgrInstance()->DBPoster_UpdateUser(this);
		//更新下线时间
		m_OfflineTime = time(NULL) + CCSCfgMgr::getInstance().GetCSGlobalCfg().delayDelFromCacheTime;
		GetCSUserMgrInstance()->OnUserOffline(this);
		NotifyUserPlayState();

		CSSGameLogMgr::GetInstance().AddGameLog(eLog_Logout,GetUserDBData().szUserName,0); 

		if (GetUserBattleInfoEx().GetBattleState()<eBattleState_Async)
		{//由cs进行管理//
			INT32 ret = eNormal;
			switch(GetUserBattleInfoEx().GetBattleType())
			{
			case eBattleType_Room:
				ret = GetBattleMgrInstance()->AskLeaveRoom(this);
				break;
			case eBattleType_Match:
				ret = GetBattleMgrInstance()->RemoveMatchUser(this);
				break;
			}
			Assert(ret==eNormal);
		}
		else
		{//由ss进行管理，只通知ss一下//
			GetBattleMgrInstance()->NotifyBattleSSUserIsOnline(this, false);
		}
	}

	bool CCSUser::IfHasBuyRunes(UINT32 un32RunesID){
		return m_RunesMap.find(un32RunesID) != m_RunesMap.end();
	}

	INT32 CCSUser::AddRunes(UINT32 un32RunesID, bool ifNotifyClient){
		AddRuneNum(un32RunesID, 1, ifNotifyClient);
		if (ifNotifyClient){
			UpdateRuneBagJson();
		}

		return eNormal;
	}

	INT32 CCSUser::AddRunes(SUserRunesBagRuntimeData sSUserRunesBagRuntimeData, bool ifNotifyClient){
		m_RunesMap[sSUserRunesBagRuntimeData.un32RunesTypeID] = sSUserRunesBagRuntimeData;

		return eNormal;
	}

	INT32 CCSUser::AddRunes(UINT32 un32RunesID, bool ifNotifyClient, int num){
		if (num < 1){
			return eNormal;
		}
		AddRuneNum(un32RunesID, num, ifNotifyClient);
		if (ifNotifyClient){
			UpdateRuneBagJson();
		}

		return eNormal;
	}

	INT32 CCSUser::DecreaseRune(vector<UINT32>& un32RunesIDVec, bool ifNotifyClient){
		if (un32RunesIDVec.empty()){
			return eNormal;
		}

		GSToGC::NotifyRunesList sMsg;
		int size = un32RunesIDVec.size();
		for (int i = 0; i < size; ++i){
			UINT32 un32RuneID = un32RunesIDVec[i];
			AddRuneNum(un32RuneID, -1, ifNotifyClient);
		}
		return eNormal;
	}

	void CCSUser::GetUserHeroList_RedisCallback(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pUserData){
		if (!predisReply){
			ELOG(LOG_ERROR, "Null Reply");
			return;
		}

		if (predisReply->type != REDIS_REPLY_ARRAY){
			ELOG(LOG_ERROR, "type is %d not array", predisReply->type);
			return;
		}

		int heroStructIndex = 0;
		SUserHeroDBData sSUserHeroDBData;
		for (int i = 0; i < predisReply->elements; ++i){
			redisReply* pOneReply = predisReply->element[i];
			if (heroStructIndex == 0){
				if (pOneReply->type == REDIS_REPLY_INTEGER){
					sSUserHeroDBData.un32HeroID = pOneReply->integer;
				}
				else if (pOneReply->type == REDIS_REPLY_STRING){
					sSUserHeroDBData.un32HeroID = atoi(pOneReply->str);
				}
				++heroStructIndex;
			}
			else if (heroStructIndex == 1){
				if (pOneReply->type == REDIS_REPLY_INTEGER){
					sSUserHeroDBData.buyTime = pOneReply->integer;
				}
				else if (pOneReply->type == REDIS_REPLY_STRING){
					sSUserHeroDBData.buyTime = _atoi64(pOneReply->str);
				}
				++heroStructIndex;
			}
			else if (heroStructIndex == 2){
				if (pOneReply->type == REDIS_REPLY_INTEGER){
					sSUserHeroDBData.endTime = pOneReply->integer;
				}
				else if (pOneReply->type == REDIS_REPLY_STRING){
					sSUserHeroDBData.endTime = _atoi64(pOneReply->str);
				}

				ELOG(LOG_DBBUG, "Get Herocfg:id:%d, buytime:%lld, lifetime:%lld", sSUserHeroDBData.un32HeroID, sSUserHeroDBData.buyTime, sSUserHeroDBData.endTime);
				heroStructIndex = 0;
			} 
		}
	}

	INT32	CCSUser::GetChangeNicknameCost(){
		return 0;
	}

	INT32	CCSUser::AskAddToSNSMap(const UINT64 guidIdx, SUserRelationshipInfo& temp_info)
	{
		if (eRSType_Friends == temp_info.eRelationShip)
		{
			m_sUserDBData.friendListMap[temp_info.guididx] = temp_info;
		}
		else{
			m_sUserDBData.blackListMap[temp_info.guididx] = temp_info;
		}
		return eNormal;
	}

	INT32	CCSUser::AskRemFromSNSMap(const UINT64 guididx, ERelationShip eRelationShip)
	{
		if (eRSType_Friends == eRelationShip){
			auto iter_f = m_sUserDBData.friendListMap.find(guididx);
			if (iter_f != m_sUserDBData.friendListMap.end()){
				m_sUserDBData.friendListMap.erase(iter_f);
				PostMsgToGC_NotifuUserReMoveFromSNSList(eRelationShip, guididx);
				ELOG(LOG_INFO, "Remove User %s from User %s Friend List.", iter_f->second.stNickName, GetNickName());
			}
		}
		else{
			auto iter_b = m_sUserDBData.blackListMap.find(guididx);
			if(iter_b != m_sUserDBData.blackListMap.end()){
				m_sUserDBData.blackListMap.erase(iter_b);
				PostMsgToGC_NotifuUserReMoveFromSNSList(eRelationShip, guididx);
				ELOG(LOG_INFO, "Remove User %s from User %s Black List.", iter_b->second.stNickName, GetNickName());
			}
		}
		return eNormal;
	}

	void	CCSUser::LoadUserSNSList(DBToCS::RSinfo msg_snslist)
	{
		SUserRelationshipInfo temp_info;
		temp_info.guididx = msg_snslist.related_id();
		temp_info.nHeadId = msg_snslist.related_header();
		temp_info.stNickName = msg_snslist.related_name();
		temp_info.viplv = msg_snslist.related_vip();
		temp_info.eRelationShip = (ERelationShip)msg_snslist.relation();
		temp_info.tMilSec = 0;

		if (eRSType_Friends == (ERelationShip)msg_snslist.relation())
		{
			m_sUserDBData.friendListMap[msg_snslist.related_id()] = temp_info;
		}

		if (eRSType_Detestation == (ERelationShip)msg_snslist.relation())
		{
			m_sUserDBData.blackListMap[msg_snslist.related_id()] = temp_info;
		}
	}

	void	CCSUser::AddUserItems(DBToCS::ItemInfo& itemInfo)
	{
		SUserItemInfo temp_info;
		temp_info.item_id = itemInfo.item_id();
		temp_info.buy_time = itemInfo.buy_time();
		temp_info.end_time = itemInfo.end_time();
		temp_info.item_num = itemInfo.item_num();
		//若不为永久拥有，过期则删除
		if (-1 != itemInfo.end_time() && 0 > itemInfo.end_time() + itemInfo.buy_time() - time(NULL)){
			GetCSUserMgrInstance()->UserAskUdateItem(temp_info, eOperationType_Del, GetGUID());
		}
		else{
			m_sUserDBData.item_Map[temp_info.item_id] = temp_info;
		}
	}

	INT32	CCSUser::ChangeOtherItemMap(UINT32 ItemID, INT32 num)
	{
		SOtherItem *t_cfg = CCSCfgMgr::getInstance().GetOtherItemCfg(ItemID);
		if (t_cfg == NULL){
			ELOG(LOG_ERROR, "未找到配置!");
			return eEC_DidNotHaveThisItem;
		}

		auto iter = m_sUserDBData.item_Map.find(ItemID);
		//用户是否已拥有该物品
		if (iter != m_sUserDBData.item_Map.end())
		{
			//该物品是否是时间累加类型
			if (OtherItemIfTimeAdd(t_cfg->effect_type)){
				//是否已过期
				if (iter->second.end_time + iter->second.buy_time - time(NULL) > 0){
					//剩余时间
					iter->second.end_time += t_cfg->item_duration * 60 * 60;
				}
				else{
					iter->second.buy_time = time(NULL);
					iter->second.end_time = t_cfg->item_duration * 60 * 60;
				}
				GetCSUserMgrInstance()->UserAskUdateItem(iter->second, eOperationType_Upd, GetGUID());
			}
			else{
				//数量是否为0
				iter->second.item_num += num;
				if (iter->second.item_num <= 0){
					GetCSUserMgrInstance()->UserAskUdateItem(iter->second, eOperationType_Del, GetGUID());
					m_sUserDBData.item_Map.erase(iter);
				}else{
					GetCSUserMgrInstance()->UserAskUdateItem(iter->second, eOperationType_Upd, GetGUID());
				}
			}
		}
		else{
			//全英雄体验券
			if (eItemEffect_AllHeroFree == t_cfg->effect_type){
				bool ifAdd = false;
				auto iter_heromap = CCSCfgMgr::getInstance().GetHeroBuyCfgmap().begin();
				for (; iter_heromap != CCSCfgMgr::getInstance().GetHeroBuyCfgmap().end(); iter_heromap ++){
					if (iter_heromap->second.useTimeSpan > 0){
						continue;
					}
					//如果是非免费英雄，则添加
					for (auto consumeIter = iter_heromap->second.sConsumeList.Begin(); 
						consumeIter != iter_heromap->second.sConsumeList.End(); consumeIter = iter_heromap->second.sConsumeList.Next()){
						ConsumeStruct& sConsumeStruct = *consumeIter;
						if (sConsumeStruct.price > 0){
							ELOG(LOG_SpecialDebug,"ID:%d, P:%d.", iter_heromap->second.un32HeroID, sConsumeStruct.type);
							ifAdd = true;
						}
					}

					if (ifAdd){
						ELOG(LOG_INFO,"Hero:%d.", iter_heromap->second.un32HeroID);
						AddExperienceHero(iter_heromap->second.un32HeroID, t_cfg->item_duration * 60 * 60, false);
					}
					ifAdd = false;
				}
				return eNormal;
			}
			//其他物品
			SUserItemInfo t_info;
			t_info.item_id = ItemID;
			t_info.buy_time = time(NULL);

			t_info.item_num = num;
			
			if (t_cfg->item_duration != -1){
				if (OtherItemIfTimeAdd(t_cfg->effect_type)){
					//如果是时间累加类型
					t_info.end_time = t_cfg->item_duration * 60 * 60 * num;
					t_info.item_num = 1;
				}
				else{
					t_info.end_time = t_cfg->item_duration * 60 * 60;
				}
			}
			else{
				t_info.end_time = t_cfg->item_duration;
			}

			m_sUserDBData.item_Map[t_info.item_id] = t_info;

			GetCSUserMgrInstance()->UserAskUdateItem(t_info, eOperationType_Add, GetGUID());
		}

		SynOtherItemInfo(ItemID);
		
		return eNormal;
	}

	INT64	CCSUser::GetItemAdditionSurplusTime(ItemEffect effect)
	{
		INT64 temp_time = 0;
		CalculateItemAddition();
		switch(effect){
		//经验加成卡剩余时间
		case eItemEffect_ExpGain:
			{
				auto iter = m_ItemAdditionMap.find(ePrivilege_ExpCopAdd);
				if (iter != m_ItemAdditionMap.end()){
					//end_time 为buy_time + expried_time
					temp_time = iter->second.end_time - time(NULL);
				}
			}
			break;
		//金币加成卡剩余时间
		case eItemEffect_GoldGain:
			{
				auto iter = m_ItemAdditionMap.find(ePrivilege_GlodCopAdd);
				if (iter != m_ItemAdditionMap.end()){
					temp_time = iter->second.end_time - time(NULL);
				}
			}
			break;
		default:
			return 0;
		}

		if (temp_time > 0)
		{
			return temp_time;
		}
		else{
			return 0;
		}
	}

	bool	CCSUser::OtherItemIfTimeAdd(ItemEffect effect)
	{
		switch(effect){
		case eItemEffect_GoldGain:
		case eItemEffect_ExpGain:
			return true;
		default:
			return false;
		}
	}

	void	CCSUser::CalculateItemAddition()
	{
		//使用到相关数据时计算一次
		auto iter = m_sUserDBData.item_Map.begin();
		for(; iter != m_sUserDBData.item_Map.end(); iter++){
			SOtherItem* temp_cfg = CCSCfgMgr::getInstance().GetOtherItemCfg(iter->first);
			if (!temp_cfg){
				ELOG(LOG_ERROR, "null cfg:%d", iter->first);
				continue;
			} 
			//检测过期
			if (0 > iter->second.buy_time + iter->second.end_time - time(NULL)){
				GetCSUserMgrInstance()->UserAskUdateItem(iter->second, eOperationType_Del, GetGUID());
			}

			switch(temp_cfg->effect_type){
			case eItemEffect_GoldGain:{
				auto iter_f = m_ItemAdditionMap.find(ePrivilege_GlodCopAdd);
				if (iter_f != m_ItemAdditionMap.end()){
					//物品加成以最高为主
					if (iter_f->second.add_num >= temp_cfg->effect_value){
						break;
					}
					else{
						iter_f->second.add_num = temp_cfg->effect_value;
					}
				}
				else{
					SPrivilege t_sp;
					t_sp.add_num = temp_cfg->effect_value;
					//end_time = 购买时间 + 剩余时间
					t_sp.end_time = iter->second.end_time + iter->second.buy_time;
					m_ItemAdditionMap[ePrivilege_GlodCopAdd] = t_sp;
				}
									  }
				break;

			case eItemEffect_ExpGain:{
				auto iter_f = m_ItemAdditionMap.find(ePrivilege_ExpCopAdd);
				if (iter_f != m_ItemAdditionMap.end()){
					if (iter_f->second.add_num >= temp_cfg->effect_value){
						break;
					}
					else{
						iter_f->second.add_num = temp_cfg->effect_value;
					}
				}
				else{
					SPrivilege t_sp;
					t_sp.add_num = temp_cfg->effect_value;
					t_sp.end_time = iter->second.end_time + iter->second.buy_time;
					m_ItemAdditionMap[ePrivilege_ExpCopAdd] = t_sp;
				}
									 }
				break;
			}
		}
	}

	INT32	CCSUser::GetUserAddition(ePrivilegeType type)
	{
		auto iter = m_ItemAdditionMap.find(type);
		if (iter != m_ItemAdditionMap.end()){
			return iter->second.add_num;
		}
		return 0;
	}

	void	CCSUser::CheckUpgrade(INT32 got_exp)
	{
		m_sUserDBData.sPODUsrDBData.un32UserCurLvExp += got_exp;
		UINT32	CfgExp = CCSCfgMgr::getInstance().GetUserLvUpExp(m_sUserDBData.sPODUsrDBData.un8UserLv);
		while (CfgExp <= m_sUserDBData.sPODUsrDBData.un32UserCurLvExp)
		{
			m_sUserDBData.sPODUsrDBData.un32UserCurLvExp -= CfgExp;
			if (CCSCfgMgr::getInstance().GetUserMaxLv() > m_sUserDBData.sPODUsrDBData.un8UserLv){
				m_sUserDBData.ChangeUserDbData(eUserDBType_UserLv, 1);
			}
			else{
				m_sUserDBData.ChangeUserDbData(eUserDBType_UserLv, CCSCfgMgr::getInstance().GetUserLvUpExp(m_sUserDBData.sPODUsrDBData.un8UserLv));
				m_sUserDBData.ChangeUserDbData(eUserDBType_UserLvExp, 1);
				break;
			}
			CfgExp = CCSCfgMgr::getInstance().GetUserLvUpExp(m_sUserDBData.sPODUsrDBData.un8UserLv);
		}
	}

	const string DeviedeStr = "|";
	const string PreStr = "'";
	const string MidStr = ",";

	void CCSUser::UpdateRuneBagJson(){
		m_RuneBagStream.str("");
		m_RuneBagStream << PreStr;
		for (auto iter = m_RunesMap.begin(); iter != m_RunesMap.end(); ++iter){
			SUserRunesBagRuntimeData& sSUserRunesBagRuntimeData = iter->second;
			m_RuneBagStream << sSUserRunesBagRuntimeData.un32RunesTypeID << MidStr << sSUserRunesBagRuntimeData.n16Num << MidStr << sSUserRunesBagRuntimeData.n64GotTime << DeviedeStr;
		}
		m_RuneBagStream << PreStr;
		CCSUserDbDataMgr::InsertOrUpdateRunesBagDataAndToDBThread(m_sUserDBData.sPODUsrDBData.un64ObjIdx, m_RuneBagStream.str());
	}

	void CCSUser::UpdateRuneSlotJson(){
		m_RuneSlotStream.str("");
		m_RuneSlotStream << PreStr;
		for (auto iter = m_EquipRuneArrayMap.begin(); iter != m_EquipRuneArrayMap.end(); ++iter){
			EquipRuneArray& sEquipRuneArray = iter->second;
			for (INT32 i = 0; i < sEquipRuneArray.GetMaxSize(); ++i){
				UINT32* pRuneID = sEquipRuneArray.GetValueFromIndex(i);
				if (pRuneID){
					m_RuneSlotStream << iter->first <<MidStr<< *pRuneID <<MidStr<<i<<DeviedeStr;
				}
			}
		}
		m_RuneSlotStream << PreStr;
		CCSUserDbDataMgr::InsertOrUpdateRunesSlotDataAndToDBThread(m_sUserDBData.sPODUsrDBData.un64ObjIdx, m_RuneSlotStream.str());
	}

	void CCSUser::InitRunes(const string& bagJson, const string& slotStr){
		typedef vector< string > split_vector_type;
		split_vector_type sBagVec;
		boost::algorithm::split(sBagVec, bagJson, boost::algorithm::is_any_of(DeviedeStr) );

		int size = sBagVec.size();
		for (int i = 0; i < size; ++i){
			split_vector_type sBagItemVec;
			boost::algorithm::split(sBagItemVec, sBagVec[i], boost::algorithm::is_any_of(MidStr) );
			if (sBagItemVec.size() != 3){
				continue;
			}
			SUserRunesBagRuntimeData sSUserRunesBagRuntimeData;
			sSUserRunesBagRuntimeData.un32RunesTypeID = atoi(sBagItemVec[0].c_str());
			sSUserRunesBagRuntimeData.n16Num = atoi(sBagItemVec[1].c_str());
			sSUserRunesBagRuntimeData.n64GotTime = _atoi64(sBagItemVec[2].c_str());

			ELOG(LOG_DBBUG, "add rune id:%u, num:%d, gotime:%lld", sSUserRunesBagRuntimeData.un32RunesTypeID, sSUserRunesBagRuntimeData.n16Num, sSUserRunesBagRuntimeData.n64GotTime);
			m_RunesMap.insert(std::make_pair(sSUserRunesBagRuntimeData.un32RunesTypeID, sSUserRunesBagRuntimeData));
		}


		split_vector_type sSlotVec;
		boost::algorithm::split(sSlotVec, slotStr, boost::algorithm::is_any_of(DeviedeStr) );
		size = sSlotVec.size();
		for (int i = 0; i < size; ++i){
			split_vector_type sSlotItemVec;
			boost::algorithm::split(sSlotItemVec, sSlotVec[i], boost::algorithm::is_any_of(MidStr) );
			if (sSlotItemVec.size() != 3){
				continue;
			}
			int n16RunePage = atoi(sSlotItemVec[0].c_str());
			int pos = atoi(sSlotItemVec[2].c_str());
			int runeID = atoi(sSlotItemVec[1].c_str());
			ELOG(LOG_DBBUG, "Add page:%d, pos:%d,runeid:%d from db", n16RunePage, pos, runeID);

			auto iter = m_EquipRuneArrayMap.find(n16RunePage);
			if (iter == m_EquipRuneArrayMap.end()){
				EquipRuneArray sEquipRuneArray;
				sEquipRuneArray.AddElement(runeID);

				m_EquipRuneArrayMap.insert(std::make_pair(n16RunePage, sEquipRuneArray));
			}
			else{
				EquipRuneArray& sEquipRuneArray = iter->second;
				sEquipRuneArray.AddElement(runeID);
			}
		}
	}

	bool CCSUser::AddRuneNum(UINT32 runeId, INT32 runeNum, bool IfNotifyClient){
		INT64 curTime = time(NULL);
		INT32 num = 0;
		auto runedIter = m_RunesMap.find(runeId);
		if (runedIter == m_RunesMap.end()){
			if (runeNum < 0){
				return false;
			}
			SUserRunesBagRuntimeData aSUserRunesBagRuntimeData(runeId, runeNum, curTime);
			num = 1;
			m_RunesMap.insert(std::make_pair(runeId, aSUserRunesBagRuntimeData));
		}
		else{
			runedIter->second.n16Num += runeNum;
			num = runedIter->second.n16Num;
			runedIter->second.n64GotTime = curTime;

			if (runedIter->second.n16Num == 0){
				ELOG(LOG_DBBUG, "删除符文！");
				m_RunesMap.erase(runedIter);
			}
		}

		if (IfNotifyClient){
			GSToGC::NotifyRunesList sNotifyRunesList;

			auto pBagInfo = sNotifyRunesList.add_runesbaginfo();
			pBagInfo->set_num(num);
			pBagInfo->set_runeid(runeId);
			pBagInfo->set_gottime(curTime);

			GetCSKernelInstance()->PostMsgToGC(m_sUserNetInfo, sNotifyRunesList, sNotifyRunesList.msgid());

			ELOG(LOG_DBBUG, "更新符文信息num:%d, runeid:%d", num, runeId);
		}

		return true;
	}

	bool CCSUser::ConsumeCombine(int runeLevel){
		if (runeLevel < 0 || runeLevel > 5){
			return false;
		}

		const int cost = CombineCost[runeLevel];
		if (cost > m_sUserDBData.sPODUsrDBData.n64Gold){
			return false;
		}

		m_sUserDBData.ChangeUserDbData(eUserDBType_Gold, -cost);
		SynCurGold();

		return true;
	}

	//将玩家数据存储到Redis
	bool CCSUser::SaveToRedis(){
		if (!GetUserDBCacheRedisHandler()){
			return false;
		}

		if (!GetUserDBCacheRedisHandler()->CanbeUsed()){
			return false;
		}

		GetTaskMgr()->PackTaskData(m_sUserDBData.szTaskData,m_sUserDBData.isTaskRush);//存Redis时增加任务数据

		string runeBagStr = "";
		{
			stringstream sRuneBagStream;
			for (auto iter = m_RunesMap.begin(); iter != m_RunesMap.end(); ++iter){
				SUserRunesBagRuntimeData& sSUserRunesBagRuntimeData = iter->second;
				sRuneBagStream << sSUserRunesBagRuntimeData.un32RunesTypeID << MidStr << sSUserRunesBagRuntimeData.n16Num <<MidStr<< sSUserRunesBagRuntimeData.n64GotTime << DeviedeStr;
			}
			runeBagStr = sRuneBagStream.str();
		}

		string runeSlotStr = "";
		{
			stringstream sRuneSlotStream;
			for (auto iter = m_EquipRuneArrayMap.begin(); iter != m_EquipRuneArrayMap.end(); ++iter){
				EquipRuneArray& sEquipRuneArray = iter->second;
				for (INT32 i = 0; i < sEquipRuneArray.GetMaxSize(); ++i){
					UINT32* pRuneID = sEquipRuneArray.GetValueFromIndex(i);
					if (pRuneID){
						sRuneSlotStream << iter->first <<MidStr<< *pRuneID <<MidStr<<i<<DeviedeStr;
					}
				}
			}

			runeSlotStr = sRuneSlotStream.str();
		}

		string heroStr = "";
		{
			stringstream sHeroListVal;
			for (auto iter = m_sUserDBData.heroListMap.begin(); iter != m_sUserDBData.heroListMap.end(); ++iter){
				SUserHeroDBData& sSUserHeroDBData = iter->second;
				sHeroListVal << sSUserHeroDBData.un32HeroID <<MidStr << sSUserHeroDBData.buyTime <<MidStr<< sSUserHeroDBData.endTime << DeviedeStr;
			}

			heroStr = sHeroListVal.str();
		}

		string friendStr = "";
		{
			stringstream sFriendListVal;
			for (auto iter = m_sUserDBData.friendListMap.begin(); iter != m_sUserDBData.friendListMap.end(); ++iter){
				SUserRelationshipInfo& sSUserRelationshipInfo = iter->second;
				sFriendListVal << sSUserRelationshipInfo.guididx << MidStr << sSUserRelationshipInfo.nHeadId << MidStr
					<< sSUserRelationshipInfo.tMilSec << MidStr<< sSUserRelationshipInfo.eRelationShip << MidStr
					<< sSUserRelationshipInfo.stNickName << DeviedeStr;
			}
			friendStr = sFriendListVal.str();
		}

		string blackStr = "";
		{
			stringstream sFriendListVal;
			for (auto iter = m_sUserDBData.blackListMap.begin(); iter != m_sUserDBData.blackListMap.end(); ++iter){
				SUserRelationshipInfo& sSUserRelationshipInfo = iter->second;
				sFriendListVal << sSUserRelationshipInfo.guididx << MidStr << sSUserRelationshipInfo.nHeadId <<MidStr
					<< sSUserRelationshipInfo.tMilSec << MidStr << sSUserRelationshipInfo.eRelationShip << MidStr
					<< sSUserRelationshipInfo.stNickName << DeviedeStr;
			}
			blackStr = sFriendListVal.str();
		}

		string itemStr = "";
		{
			stringstream sItemListVal;
			for (auto iter = m_sUserDBData.item_Map.begin(); iter != m_sUserDBData.item_Map.end(); ++iter){
				SUserItemInfo& sSUserItemInfo = iter->second;
				sItemListVal << sSUserItemInfo.item_id << MidStr <<  sSUserItemInfo.item_num << MidStr<< sSUserItemInfo.ifusing <<MidStr <<  sSUserItemInfo.buy_time << MidStr <<  sSUserItemInfo.end_time << DeviedeStr;
			}

			itemStr = sItemListVal.str();
		}

		string mailStr = "";
		{
			stringstream sVal;
			std::map<INT32,tagMailState>* pMailMap = GetCSUserMgrInstance()->GetMailMgr().GetUserMailState(GetGUID());
			if (pMailMap){
				for (auto iter = pMailMap->begin(); iter != pMailMap->end(); ++iter){
					sVal << iter->first <<MidStr<< iter->second.curtState  << DeviedeStr;
				}
			}

			mailStr = sVal.str();
		}

		PODUsrDBData& podData = m_sUserDBData.sPODUsrDBData;
		GetUserDBCacheRedisHandler()->redisAsyncCommand(nullptr, nullptr, "hmset usercache:%llu " 
			"guid %llu nickname %s  runebag %s "
			"runeslot %s hero %s friend %s black %s "
			"item %s mail %s diamond %lld gold %lld "
			"score %lld Last1WinTime %lld LoginReward %u RegisteTime %lld "
			"Cldays %d UserLv %d VipLv %d header %d " 
			"assist %d deadtimes %d killbuild %d gameinns %d " 
			"killhero %d winInns %d lvExp %d vipScore %d " 
			"guideStr %s n8Sex %d taskData %s"
			, m_sUserDBData.sPODUsrDBData.un64ObjIdx
			, m_sUserDBData.sPODUsrDBData.un64ObjIdx
			, m_sUserDBData.szNickName.c_str(), runeBagStr.c_str()
			, runeSlotStr.c_str(), heroStr.c_str(), friendStr.c_str(), blackStr.c_str()
			, itemStr.c_str(), mailStr.c_str(), podData.n64Diamond, podData.n64Gold
			, podData.n64Score, podData.tLastFirstWinTime, podData.un32LastGetLoginRewardDay, podData.tRegisteUTCMillisec
			, podData.un16Cldays, podData.un8UserLv, podData.un16VipLv, podData.un16HeaderID
			, podData.un32TotalAssist, podData.un32TotalDeadTimes, podData.un32TotalDestoryBuildings, podData.un32TotalGameInns
			, podData.un32TotalHeroKills, podData.un32TotalWinInns, podData.un32UserCurLvExp, podData.vipScore
			, m_sUserDBData.mGuideSteps.szCSContinueGuide.c_str(), podData.n16Sex, m_sUserDBData.szTaskData.c_str());

		return true;
	}

	bool CCSUser::LoadFromRedisStr(const string& heroStr, const string& friendStr, const string& blackStr, const string& itemStr, const string& mailStr){
		LoadHeroFromRedis(heroStr);
		LoadItemFromRedis(itemStr);
		LoadMailFromRedis(mailStr);
		LoadFriendFromRedis(friendStr, m_sUserDBData.friendListMap);
		LoadFriendFromRedis(blackStr, m_sUserDBData.blackListMap);
		return true;
	}

	void CCSUser::LoadHeroFromRedis(const string& heroStr){
		typedef vector< string > split_vector_type;
		split_vector_type sHeroVec;
		boost::algorithm::split(sHeroVec, heroStr, boost::algorithm::is_any_of(DeviedeStr) );

		int size = sHeroVec.size();
		for (int i = 0; i < size; ++i){
			split_vector_type sHeroItemVec;
			boost::algorithm::split(sHeroItemVec, sHeroVec[i], boost::algorithm::is_any_of(MidStr) );
			if (sHeroItemVec.size() != 3){
				continue;
			}
			SUserHeroDBData sSUserHeroDBData;
			sSUserHeroDBData.un32HeroID = atoi(sHeroItemVec[0].c_str());
			sSUserHeroDBData.buyTime = _atoi64(sHeroItemVec[1].c_str());
			sSUserHeroDBData.endTime = _atoi64(sHeroItemVec[2].c_str());


			m_sUserDBData.heroListMap.insert(std::make_pair(sSUserHeroDBData.un32HeroID, sSUserHeroDBData));
			ELOG(LOG_DBBUG, "Add redis hero:%d", sSUserHeroDBData.un32HeroID);
		}
	}

	void CCSUser::LoadItemFromRedis(const string& itemStr){
		typedef vector< string > split_vector_type;
		split_vector_type sItemListVec;
		boost::algorithm::split(sItemListVec, itemStr, boost::algorithm::is_any_of(DeviedeStr) );

		int size = sItemListVec.size();
		for (int i = 0; i < size; ++i){
			split_vector_type sItemVec;
			boost::algorithm::split(sItemVec, sItemListVec[i], boost::algorithm::is_any_of(MidStr) );
			if (sItemVec.size() != 5){
				continue;
			}
			SUserItemInfo sSUserItemInfo;
			sSUserItemInfo.item_id = atoi(sItemVec[0].c_str());
			sSUserItemInfo.item_num = _atoi64(sItemVec[1].c_str());
			sSUserItemInfo.ifusing = atoi(sItemVec[2].c_str());
			sSUserItemInfo.buy_time = _atoi64(sItemVec[3].c_str());
			sSUserItemInfo.end_time = _atoi64(sItemVec[4].c_str());

			m_sUserDBData.item_Map.insert(std::make_pair(sSUserItemInfo.item_id, sSUserItemInfo));
			ELOG(LOG_DBBUG, "Add redis item:%d", sSUserItemInfo.item_id);
		}
	}

	void CCSUser::LoadMailFromRedis(const string& mailStr){
		typedef vector< string > split_vector_type;
		split_vector_type sMailListVec;
		boost::algorithm::split(sMailListVec, mailStr, boost::algorithm::is_any_of(DeviedeStr) );

		int size = sMailListVec.size();
		for (int i = 0; i < size; ++i){
			split_vector_type sMailItemVec;
			boost::algorithm::split(sMailItemVec, sMailListVec[i], boost::algorithm::is_any_of(MidStr) );
			if (sMailItemVec.size() != 2){
				continue;
			}
			UserMailDBData sUserMailDBData;
			sUserMailDBData.mailId = atoi(sMailItemVec[0].c_str());
			sUserMailDBData.mState = (EMailCurtState)atoi(sMailItemVec[1].c_str());
			GetCSUserMgrInstance()->GetMailMgr().updatePerMailList(sUserMailDBData.mailId,   m_sUserDBData.sPODUsrDBData.un64ObjIdx  ,sUserMailDBData.mState); 
			ELOG(LOG_DBBUG, "Add redis mail:%d", sUserMailDBData.mailId);
		}
	}

	void CCSUser::LoadFriendFromRedis(const string& friendStr, map<UINT32, SUserRelationshipInfo>& OriMap){
		typedef vector< string > split_vector_type;
		split_vector_type sFriendListVec;
		boost::algorithm::split(sFriendListVec, friendStr, boost::algorithm::is_any_of(DeviedeStr) );

		int size = sFriendListVec.size();
		for (int i = 0; i < size; ++i){
			split_vector_type sFrinedItemVec;
			boost::algorithm::split(sFrinedItemVec, sFriendListVec[i], boost::algorithm::is_any_of(MidStr) );
			if (sFrinedItemVec.size() != 5){
				continue;
			}
			SUserRelationshipInfo sSUserRelationshipInfo;
			sSUserRelationshipInfo.eRelationShip = (ERelationShip)atoi(sFrinedItemVec[3].c_str());
			sSUserRelationshipInfo.guididx = _atoi64(sFrinedItemVec[0].c_str());
			sSUserRelationshipInfo.nHeadId = atoi(sFrinedItemVec[1].c_str());
			sSUserRelationshipInfo.tMilSec = _atoi64(sFrinedItemVec[2].c_str());
			sSUserRelationshipInfo.stNickName = sFrinedItemVec[4];

			OriMap.insert(std::make_pair(sSUserRelationshipInfo.guididx, sSUserRelationshipInfo));
		}
	}
}
