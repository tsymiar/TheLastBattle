#include "stdafx.h"
#include "CSBattle.h"
#include "CSBattleMgr.h"
#include <set>
#include "CSWordFilter.h" 

#include "CSUser.h"
#include "CSUserMgr.h"
#include "CSGameLogMgr.h"

#include "boost/tokenizer.hpp"
#include "boost/algorithm/string.hpp"
#include "CSKernel.h"
#include "BattleMatcher\CMatchMgr.h"

namespace CentralServer{

	map<UINT64,CCSUser*> gAllUserInSS;

	CCSBattle::CCSBattle(EBattleMatchType match_type, BattleType type,UINT32 ssId,UINT64 battleId,UINT32 mapId, CCSUserListMap& pCCSUserListMap, map<UINT32,UINT32>* aiRobots/*=NULL*/)
	{
		m_MatctType = match_type;
		m_tStateUTCMilsec = GetUTCMiliSecond();
		m_Type = type;
		m_n32SSID = ssId;
		m_BattleID = battleId;
		m_MapID = mapId;
		m_MapLogicCfg = CCSCfgMgr::getInstance().GetMapLogicCfg(m_MapID);
		Assert(m_MapLogicCfg!=NULL);
		int level_min = INT_MAX;
		int level_max = 0;
		int pos = 0;
		ELOG(LOG_SpecialDebug,"create battle(%u)",m_BattleID);
		for (auto it=pCCSUserListMap.begin();it!=pCCSUserListMap.end();++it){
			pos = it->first;//阵营开始位置//
			for (auto iter = it->second.Begin(); iter != it->second.End(); iter = it->second.Next()){
				CCSUser* pUser = *iter;
				if (pUser->GetUserLv()<level_min) level_min = pUser->GetUserLv();
				if (pUser->GetUserLv()>level_max) level_max = pUser->GetUserLv();
				if (aiRobots) while ((*aiRobots).find(pos)!=(*aiRobots).end()){ pos += m_MapLogicCfg->PlayerModel.size(); }
				if (type==eBattleType_Room) pos = pUser->GetRoomPlayer()->GetRoomPos();
				m_UserMap[pos] = pUser;//位置信息//
				ELOG(LOG_SpecialDebug,"battle(%u) add user(%u)",m_BattleID,pUser->GetGUID());
				pos += m_MapLogicCfg->PlayerModel.size();
				Assert(pUser->GetUserBattleInfoEx().GetBattleID()==0);
				pUser->GetUserBattleInfoEx().ChangeBattleGrp(it->first);//阵营信息//
			}
		}
		if (aiRobots){
			srand(time(NULL));
			vector<UINT32>& headMap = CCSCfgMgr::getInstance().GetHeroHeadMap();
			vector<string>& aiNameMap = CCSCfgMgr::getInstance().GetAiRobotNameMap();
			map<string,bool> hadRandNames;
			static UINT64 startguid = ((UINT64)1)<<48;
			for (auto it=(*aiRobots).begin();it!=(*aiRobots).end();++it){
				SAiRobot& aiRobot = m_AIRobots[it->first];
				aiRobot.guid = ++startguid;
				aiRobot.seat = it->first;
				aiRobot.grpid = it->second;
				aiRobot.headid = headMap[RandInt(0,headMap.size()-1)];
				do {aiRobot.nickname = aiNameMap[RandInt(0,aiNameMap.size()-1)];} while (hadRandNames.find(aiRobot.nickname)!=hadRandNames.end());
				hadRandNames.insert(make_pair(aiRobot.nickname,true));
				aiRobot.level = RandInt(level_min,level_max+1);
			}
		}
	}

	CCSBattle::~CCSBattle(void){

	}

	CCSUser* CCSBattle::GetUserByGUID(UINT64 guid,UINT32* grpId/*=NULL*/){
		for (auto it=m_UserMap.begin();it!=m_UserMap.end();++it){
			if (it->second->GetGUID() == guid){
				if (grpId!=NULL){
					*grpId = it->second->GetUserBattleInfoEx().GetBattleGrpID();
				}
				return it->second;
			}
		}
		return NULL;
	}

	INT32 CCSBattle::Start(){
		GSToGC::BattleSeatPosInfo sBattleSeatPosInfoMsg;//非房间进入，补充房间消息
		CSToSS::CreateBattle sOrderCreateBattle;
		sOrderCreateBattle.set_battleid(m_BattleID);
		sOrderCreateBattle.set_mapid(m_MapID);
		sOrderCreateBattle.set_battletype((::CSToSS::CreateBattle_BattleType)m_Type);
		CCSUser* pUser = NULL;
		if (m_Type!=eBattleType_Room){
			for (auto it=m_UserMap.begin(); it!=m_UserMap.end(); ++it){
				pUser = it->second;
				GSToGC::BattleSeatPosInfo::PosInfo* pPosInfo = sBattleSeatPosInfoMsg.add_posinfo();
				pPosInfo->set_pos(it->first);
				pPosInfo->set_ifmaster(false);
				pPosInfo->set_ifready(true);
				pPosInfo->set_guid(pUser->GetGUID());
				pPosInfo->set_nickname(pUser->GetNickName());
				pPosInfo->set_headid(pUser->GetHeadID());
				pPosInfo->set_gold(pUser->GetGold());
			}
			for (auto it=m_AIRobots.begin();it!=m_AIRobots.end();++it){
				SAiRobot& aiRobot = it->second;
				GSToGC::BattleSeatPosInfo::PosInfo* pPosInfo = sBattleSeatPosInfoMsg.add_posinfo();
				pPosInfo->set_pos(aiRobot.seat);
				pPosInfo->set_ifmaster(false);
				pPosInfo->set_ifready(true);
				pPosInfo->set_guid(aiRobot.guid);
				pPosInfo->set_nickname(aiRobot.nickname);
				pPosInfo->set_headid(aiRobot.headid);
				pPosInfo->set_gold(0);
			}
		}
		for (auto it=m_UserMap.begin(); it!=m_UserMap.end(); ++it){
			pUser = it->second;
			Assert(gAllUserInSS.find(pUser->GetGUID())==gAllUserInSS.end());
			gAllUserInSS.insert(make_pair(pUser->GetGUID(),pUser));
			Assert(pUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Free);
			Assert(pUser->GetUserBattleInfoEx().GetBattleState()==eBattleState_Wait);
			pUser->GetUserBattleInfoEx().ChangeState(eBattleState_Async);
			pUser->GetUserBattleInfoEx().ChangeBattleID(m_BattleID);
			pUser->SynUser_IsOnSS();
			if (m_Type!=eBattleType_Room) pUser->PostMsgToGC(sBattleSeatPosInfoMsg,sBattleSeatPosInfoMsg.msgid());
			::CSToSS::CreateBattle_CreateBattleUserInfo* pUserInfo = sOrderCreateBattle.add_userinfo();
			pUserInfo->set_guid(pUser->GetGUID());
			pUserInfo->set_seat(it->first);
			pUserInfo->set_grpid(pUser->GetUserBattleInfoEx().GetBattleGrpID());
			pUserInfo->set_nickname(pUser->GetNickName());
			pUserInfo->set_diamond(pUser->GetDiamond());
			pUserInfo->set_gold(pUser->GetGold());
			pUserInfo->set_viplv(0);
			pUserInfo->set_ifob(false);
			pUserInfo->set_ifgm(false);
			pUserInfo->mutable_usernetinfo()->set_n32gsid(pUser->GetUserNetInfo().n32GSID);
			pUserInfo->mutable_usernetinfo()->set_n32gcnsid(pUser->GetUserNetInfo().n32GCNSID);
			
			AddBattleHeroAndOtherList(pUser, pUserInfo);
			//vector<UINT32> heroVec;
			//pUser->GetHeroVec(heroVec);
			//for (auto it = heroVec.begin(); it != heroVec.end(); ++it){
			//	pUserInfo->add_canuseherolist(*it);
			//	ELOG(LOG_DBBUG, "add hero %d to ss", *it);
			//}
			//map<INT32, EquipRuneArray>& userRunePage = pUser->GetEquipRuneArrayMap();
			//for (auto it=userRunePage.begin();it!=userRunePage.end();++it){
			//	::CSToSS::CreateBattle_CreateBattleUserInfo_RunePage* pRunePage = pUserInfo->add_runepages();//加入符文页
			//	pRunePage->set_runepage(it->first);
			//	map<UINT32,::CSToSS::CreateBattle_CreateBattleUserInfo_RunePage_RuneInfo*> createRune;//已加入符文效果列表
			//	EquipRuneArray& runesInOnePage = it->second;
			//	for (auto itOnePage=runesInOnePage.Begin();itOnePage!=runesInOnePage.End();itOnePage=runesInOnePage.Next()){
			//		UINT32 runeId = *itOnePage;
			//		const SRunesCfg* pRunesCfg = CCSCfgMgr::getInstance().GetRunesCfg(runeId);
			//		if (pRunesCfg==NULL) continue;
			//		::CSToSS::CreateBattle_CreateBattleUserInfo_RunePage_RuneInfo* pRuneInfo = createRune[pRunesCfg->un8EffectID];
			//		if (pRuneInfo==NULL) { pRuneInfo = pRunePage->add_runeeffects(); pRuneInfo->set_effectid(pRunesCfg->un8EffectID); pRuneInfo->set_effectname("undefine"); }//加入符文效果
			//		pRuneInfo->set_effectvalue(pRuneInfo->effectvalue()+pRunesCfg->fEffectVal);
			//		pRuneInfo->set_effectper(pRuneInfo->effectper()+pRunesCfg->fEffectPer);
			//	}
			//}
		}
		for (auto it=m_AIRobots.begin();it!=m_AIRobots.end();++it)
		{
			SAiRobot& aiRobot = it->second;
			::CSToSS::CreateBattle_CreateBattleUserInfo* pUserInfo = sOrderCreateBattle.add_userinfo();
			pUserInfo->set_ifairobot(true);
			pUserInfo->set_guid(aiRobot.guid);
			pUserInfo->set_seat(aiRobot.seat);
			pUserInfo->set_grpid(aiRobot.grpid);
			pUserInfo->set_nickname(aiRobot.nickname);
			::CSToSS::CreateBattle_AiRobotInfo* pAiRobot = sOrderCreateBattle.add_airobotlist();
			pAiRobot->set_seat(aiRobot.seat);
			pAiRobot->set_aitype(aiRobot.grpid);
		}
		GetCSKernelInstance()->PostMsgToSS(GetCSKernelInstance()->GetSSInfoBySSID(m_n32SSID), sOrderCreateBattle, sOrderCreateBattle.msgid());
		return eNormal;
	}

	void	 CCSBattle::AddBattleHeroAndOtherList(CCSUser* pUser, CSToSS::CreateBattle_CreateBattleUserInfo*& pUserInfo) 
	{ 
		pUser->CheckHeroValidTimer(time(NULL));
		vector<UINT32> heroVec;
		pUser->GetHeroVec(heroVec);
		for (auto it = heroVec.begin(); it != heroVec.end(); ++it){
			pUserInfo->add_canuseherolist(*it);
		}
		map<INT32, EquipRuneArray>& userRunePage = pUser->GetEquipRuneArrayMap();
		for (auto it=userRunePage.begin();it!=userRunePage.end();++it){
			::CSToSS::CreateBattle_CreateBattleUserInfo_RunePage* pRunePage = pUserInfo->add_runepages();//加入符文页
			pRunePage->set_runepage(it->first);
			map<UINT32,::CSToSS::CreateBattle_CreateBattleUserInfo_RunePage_RuneInfo*> createRune;//已加入符文效果列表
			EquipRuneArray& runesInOnePage = it->second;
			for (auto itOnePage=runesInOnePage.Begin();itOnePage!=runesInOnePage.End();itOnePage=runesInOnePage.Next()){
				UINT32 runeId = *itOnePage;
				const SRunesCfg* pRunesCfg = CCSCfgMgr::getInstance().GetRunesCfg(runeId);
				if (pRunesCfg==NULL) continue;
				::CSToSS::CreateBattle_CreateBattleUserInfo_RunePage_RuneInfo* &pRuneInfo = createRune[pRunesCfg->un8EffectID];
				if (pRuneInfo==NULL) { pRuneInfo = pRunePage->add_runeeffects(); pRuneInfo->set_effectid(pRunesCfg->un8EffectID); pRuneInfo->set_effectname("undefine"); }//加入符文效果
				pRuneInfo->set_effectvalue(pRuneInfo->effectvalue()+pRunesCfg->fEffectVal);
				pRuneInfo->set_effectper(pRuneInfo->effectper()+pRunesCfg->fEffectPer);
			}
		}

	}

	INT32 CCSBattle::Abort()
	{
		CCSUser* pUser = NULL;
		for (auto it=m_UserMap.begin(); it!=m_UserMap.end(); ++it){
			pUser = it->second;
			pUser->GetUserBattleInfoEx().Clear(1);
			// post abort msg to clients.
		}
		return eNormal;
	}

	INT32 CCSBattle::OnCreate()
	{
		m_ifAsync = false;
		CCSUser* pUser = NULL;
		for (auto it=m_UserMap.begin(); it!=m_UserMap.end(); ++it){
			pUser = it->second;
			Assert(pUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Free);
			Assert(pUser->GetUserBattleInfoEx().GetBattleState()==eBattleState_Async);
			pUser->GetUserBattleInfoEx().ChangeState(eBattleState_Play);
		}
		return eNormal;
	}

	INT32 CCSBattle::OnFinish(SSToCS::FinishBattle* pFinishMsg){
		CSToSS::FinishBattleResponse finishResponse;
		finishResponse.set_battleid(m_BattleID);
		finishResponse.set_rst(true);
		GetCSKernelInstance()->PostMsgToSS(GetCSKernelInstance()->GetSSInfoBySSID(m_n32SSID), finishResponse, finishResponse.msgid());
		CaculateResult(pFinishMsg);
		ELOG(LOG_SpecialDebug,"destory battle(%u)",m_BattleID);
		CCSUser* pUser = NULL;
		for (auto it=m_UserMap.begin(); it!=m_UserMap.end(); ++it){
			pUser = it->second;
			Assert(gAllUserInSS.find(pUser->GetGUID())!=gAllUserInSS.end());
			gAllUserInSS.erase(pUser->GetGUID());
			ELOG(LOG_SpecialDebug,"battle(%u) remove user(%u)",m_BattleID,pUser->GetGUID());
			pUser->GetUserBattleInfoEx().Clear(2);
			pUser->SynUser_IsOnSS();
		}
		ReCreateRoom();
		ReCreateMatch();

		for (auto it=m_UserMap.begin(); it!=m_UserMap.end(); ++it){
			GetCSUserMgrInstance()->CheckIfCanRemoveUser(it->second);
		}
		return eNormal;
	}

	void CCSBattle::CaculateResult(SSToCS::FinishBattle* pFinishMsg)
	{
		if (pFinishMsg==NULL) return;

		INT32 tSpanMin = (INT32)ceil( (double)(GetUTCMiliSecond()-m_tStateUTCMilsec) / 1000 / 60);//消耗时长//
		int winGrpId = pFinishMsg->wingrpid();

		GSToGC::BroadcastBattelePersonalRes sBroadcastBattelePersonalRes;
		CCSUser* pUser = NULL;
		UINT32 userGrpId = 0;

		bool isCanGetGoldAndCoin = m_UserMap.size()==c_un32MaxUserInBattle?true:false; 

		if (eMatchType_VsAI != m_MatctType){
			for (auto it=pFinishMsg->batteluserdatalist().begin();it!=pFinishMsg->batteluserdatalist().end();++it){
				//////////////////////////////////////////////////////////////////////////
				const SSToCS::FinishBattle::BattleUserData &userResult = (*it);
				pUser = GetUserByGUID(userResult.guid(),&userGrpId);
				if (pUser==NULL) continue;
				if (userGrpId==winGrpId) {
					pUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalWinInns, 1);
					pUser->GetTaskMgr()->OnEvent(TT_GameWin,1,0,0);
				}
				pUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalGameInns, 1);
				pUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalDeadTimes, userResult.deadtimes());
				pUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalHeroKills, userResult.herokills());
				pUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalDestoryBuild, userResult.destorybuilding());
				pUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalAssist, userResult.asskillnum());
				if (userResult.diamondused()>0) { pUser->GetUserDBData().ChangeUserDbData(eUserDBType_Diamond, -userResult.diamondused()); pUser->SynCurDiamond(); }
				if (userResult.goldused()>0) { pUser->GetUserDBData().ChangeUserDbData(eUserDBType_Gold, -userResult.goldused()); pUser->SynCurGold(); }
				pUser->GetTaskMgr()->OnEvent(TT_GamePlay,1,0,0);
				pUser->GetTaskMgr()->OnEvent(TT_KillPlayer,userResult.herokills(),0,0);
				pUser->GetTaskMgr()->OnEvent(TT_KillBuild,userResult.destorybuilding(),0,0);
			}
		}

		INT32 n32GetGold = 0;
		INT32 n32Exp = 0;
		//满人或人机对战，则获得经验和金币
		if(isCanGetGoldAndCoin || eBattleType_Match == m_Type || eBattleType_Guide2 == m_Type){
			for (auto it=m_UserMap.begin(); it!=m_UserMap.end(); ++it){
				pUser = it->second;
				bool bIfWinner = false;
				//计算玩家物品加成
				pUser->CalculateItemAddition();

				sBroadcastBattelePersonalRes.set_old_lv(pUser->GetUserDBData().sPODUsrDBData.un8UserLv);
				sBroadcastBattelePersonalRes.set_old_exp(pUser->GetUserDBData().sPODUsrDBData.un32UserCurLvExp);

				userGrpId = it->second->GetUserBattleInfoEx().GetBattleGrpID();
				if (userGrpId == winGrpId){//胜利者结算//
					bIfWinner = true;
					n32Exp = CaculateExp(pUser, tSpanMin, bIfWinner);
					if (pUser->IsFirstWin()){//首次胜利者结算//
						n32GetGold = CaculateFWGold(pUser, tSpanMin);
						pUser->GetUserDBData().ChangeUserDbData(eUserDBType_LastFirstWinTime, time(NULL));
					}
					else{
						n32GetGold = CaculateGold(pUser, tSpanMin, bIfWinner);
					}
				}
				else{//失败结算//
					bIfWinner = false;
					n32Exp = CaculateExp(pUser, tSpanMin, bIfWinner);
					n32GetGold = CaculateGold(pUser, tSpanMin, bIfWinner);
				}
				//sBroadcastBattelePersonalRes.set_curexp(pUser->GetUserDBData().sPODUsrDBData.un32UserCurLvExp);
				ELOG(LOG_DBBUG, "CurExp:%d, GotExp:%d, GotGold:%d.", pUser->GetUserDBData().sPODUsrDBData.un32UserCurLvExp, n32Exp, n32GetGold);

				pUser->GetUserDBData().ChangeUserDbData(eUserDBType_Gold, n32GetGold);
				pUser->SynCurGold();
				pUser->CheckUpgrade(n32Exp);
				//pUser->SynUserLvInfo();

				sBroadcastBattelePersonalRes.set_got_gold(n32GetGold);
				sBroadcastBattelePersonalRes.set_cur_lv(pUser->GetUserDBData().sPODUsrDBData.un8UserLv);
				sBroadcastBattelePersonalRes.set_cur_exp(pUser->GetUserDBData().sPODUsrDBData.un32UserCurLvExp);
				sBroadcastBattelePersonalRes.set_got_exp(n32Exp);

				GetCSKernelInstance()->PostMsgToGC(it->second->GetUserNetInfo(), sBroadcastBattelePersonalRes, sBroadcastBattelePersonalRes.msgid());
			}
		}
	}

	INT32	CCSBattle::CaculateExp(CCSUser* pUser, INT32 tSpan, bool bIfWinner)
	{
		INT32 n32TimeSpan = 0;
		double t_exp = 0;

		if (c_ExpTimelineBegin >= tSpan){
			n32TimeSpan = c_ExpTimelineBegin;
		}
		else if(c_ExpTimelineEnd <= tSpan){
			n32TimeSpan = c_ExpTimelineEnd;
		}
		else{ 
			n32TimeSpan = tSpan;
		}

		if (bIfWinner){
			double LvExp = pow(pUser->GetUserDBData().sPODUsrDBData.un8UserLv, c_fWELvPower);
			t_exp = (c_WEBase + c_WELvCoef * LvExp + c_WETimeCoef * (n32TimeSpan - c_ExpTimeFixed));
		}
		else{
			double LvExp = pow(pUser->GetUserDBData().sPODUsrDBData.un8UserLv, c_fLELvPower);
			t_exp = c_LEBase + c_LELvCoef * LvExp + c_LETimeCoef * (n32TimeSpan - c_ExpTimeFixed);
		}

		ELOG(LOG_SpecialDebug, "ExpAddition:%d.", pUser->GetUserAddition(ePrivilege_ExpCopAdd));
		double addtion = pUser->GetUserAddition(ePrivilege_ExpCopAdd);
		addtion = addtion / 100 + 1;
		return ceil(t_exp * addtion);
	}

	INT32	CCSBattle::CaculateGold(CCSUser* pUser, INT32 tSpan, bool bIfWinner)
	{
		double t_gold = 0;
		if (c_GTimelineBegin >= tSpan){
			if (bIfWinner){
				t_gold = c_WGTimeCoef * tSpan;
			}
			else{
				t_gold = c_LGTimeCoef * tSpan;
			}
		}
		else{
			INT32 n32temptime = 0;
			if (c_GTimelineMid > tSpan){
				n32temptime = c_GTimelineMid;
			}
			else if(c_GTimelineEnd <= tSpan){
				n32temptime = c_GTimelineEnd;
			}
			else {
				n32temptime = tSpan;
			}

			if (bIfWinner){
				t_gold = c_WGBase - (n32temptime - c_GTimeFixed);
			}
			else{
				t_gold = c_LGBase + (n32temptime - c_GTimeFixed);
			}
		}

		ELOG(LOG_SpecialDebug, "GoldAddition:%d.", pUser->GetUserAddition(ePrivilege_GlodCopAdd));
		double addition = pUser->GetUserAddition(ePrivilege_GlodCopAdd);
		addition = addition / 100 + 1;
		return ceil(t_gold * addition);
	}

	INT32	CCSBattle::CaculateFWGold(CCSUser* pUser, INT32 tSpan)
	{
		double addition = pUser->GetUserAddition(ePrivilege_ExpCopAdd);
		addition = addition / 100 + 1;
		ELOG(LOG_SpecialDebug, "GoldAddition:%d.", pUser->GetUserAddition(ePrivilege_GlodCopAdd));
		if (c_GTimelineBegin >= tSpan){
			return c_FWGTimeCoefOne * tSpan * addition;
		}
		else{
			INT32 n32TempTime = 0;
			if (c_GTimelineMid >= tSpan){
				n32TempTime = c_GTimelineMid;
			}
			else if(c_GTimelineEnd <= tSpan){
				n32TempTime = c_GTimelineEnd;
			}
			else{
				n32TempTime = tSpan;
			}

			return ceil( (c_FWGBase - c_FWGTimeCoefTwo * (n32TempTime - c_GTimeFixed)) * addition);
		}
	}

	void CCSBattle::ReCreateRoom()
	{
		if (m_Type==eBattleType_Room){
			CCSUser* pUser = NULL;
			UINT32 roomid = 0;
			for (auto it=m_UserMap.begin(); it!=m_UserMap.end(); ++it){
				pUser = it->second;
				if (pUser->GetUserPlayingStatus()==eUserPlayingStatus_OffLine) continue;//下线玩家可删除了//
				if (roomid==0 && eNormal!=GetBattleMgrInstance()->AskCreateRoom(pUser,m_MapID,m_Password,true,it->first)) break;
				else if (roomid==0){ roomid = pUser->GetRoomPlayer()->m_RoomID; continue; }
				GetBattleMgrInstance()->AskAddRoom(pUser,roomid,m_Password,true,it->first);
			}
		}
	}

	void CCSBattle::ReCreateMatch()
	{
		if (m_Type==eBattleType_Match){
			CCSUser* pUser = NULL;
			for (auto it=m_UserMap.begin(); it!=m_UserMap.end(); ++it){
				pUser = it->second;
				if (pUser->GetUserPlayingStatus()==eUserPlayingStatus_OffLine){
					GetBattleMgrInstance()->GetMatcher()->TeamStopMatch(pUser->GetMatchPlayer());
					GetBattleMgrInstance()->GetMatcher()->UserRemoveTeam(pUser->GetMatchPlayer());
					Assert(pUser->GetUserBattleInfoEx().GetBattleType()==eBattleType_Free);
					Assert(pUser->GetUserBattleInfoEx().GetBattleState()==eBattleState_Free);
				}
				else{
					GetBattleMgrInstance()->GetMatcher()->UserStopTeam(pUser->GetMatchPlayer());
					pUser->GetUserBattleInfoEx().ChangeTypeWithState(eBattleType_Match,eBattleState_Wait);
				}
			}
		}
	}

}