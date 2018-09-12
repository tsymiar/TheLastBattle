#pragma once
#include "stdafx.h"
#include "Timer.h"

class Buffer;
class CTaskMgr;

namespace CentralServer{
enum BattleType
{
	eBattleType_Free,		//自由中//
	eBattleType_Room,		//房间中//
	eBattleType_Match,		//匹配中//
	eBattleType_Guide1,		//引导1中//
	eBattleType_Guide2,		//引导2中//
};

enum BattleState
{
	eBattleState_Free,		//自由中//
	eBattleState_Wait,		//等待中//
	eBattleState_Async,		//异步中//
	eBattleState_Play,		//战斗中//
};

enum ePrivilegeType{
	ePrivilege_None = 0,
	ePrivilege_GlodCopAdd = 1,
	ePrivilege_ExpCopAdd = 2,
	ePrivilege_GoldCostMin =3,
};

struct SPrivilege{
	INT32	add_num;
	UINT64	end_time;
};

/*战斗信息互斥体*/
class CCSUserBattleInfo
{
public:
	CCSUserBattleInfo() {
		mBattleType=eBattleType_Free;
		mBattleState=eBattleState_Free;
		mBattleId=0;
		mBattleGrpId = 0;
	}
	~CCSUserBattleInfo(){
		Assert(mBattleType==eBattleType_Free);
		Assert(mBattleState==eBattleState_Free);
	}
public:
	BattleType GetBattleType() { return mBattleType; }
	BattleState GetBattleState() { return mBattleState; }
	UINT64 GetBattleID() { return mBattleId; }
	UINT32 GetBattleGrpID() { return mBattleGrpId; }
public:
	void Clear(int reason){
		mBattleType=eBattleType_Free;
		mBattleState=eBattleState_Free;
		mBattleId=0;
		mBattleGrpId = 0;
		Assert(mDebugName.size()>0);
		ELOG(LOG_SpecialDebug,"user(%s) clear by reason(%u)",mDebugName.c_str(),reason);
	}
	void ChangeTypeWithState(BattleType type,BattleState state=eBattleState_Free){
		Assert(type!=eBattleType_Free);
		mBattleType = type;
		ChangeState(state);
	}
	void ChangeState(BattleState state){
		mBattleState = state;
	}
	void ChangeBattleID(UINT64 battleid){
		mBattleId = battleid;
	}
	void ChangeBattleGrp(UINT32 grpId){
		mBattleGrpId = grpId;
	}
private:
	BattleType mBattleType;
	BattleState mBattleState;
	UINT64 mBattleId;
	UINT32 mBattleGrpId;
public:
	string mDebugName;
};

typedef ElementArr<UINT32, c_n32MaxSlotInRunePage> EquipRuneArray; 

class CCSUser
{
public:
	CCSUser();
	~CCSUser();

private:
	EUserPlayingStatus			m_eUserPlayingStatus;
	SUserDBData					m_sUserDBData;
	TIME_MILSEC					m_OfflineTime;
	
	SUserNetInfo				m_sUserNetInfo;
	TIME_MILSEC					m_tGCLastPing;
	CCSUserBattleInfo			m_sUserBattleInfoEx;
	IRoomPlayer					m_tRoomPlayer;
	IMatchPlayer				m_tMatchPlayer;
	CTaskMgr*					m_cTaskMgr;
	map<UINT64, SUserRelationshipInfo>	m_cAddFVec;
	int64_t								m_TimerID;
	map<UINT32, SUserRunesBagRuntimeData>	m_RunesMap;
	vector<INT32>							m_RunePageMaxEquipNum;
	map<INT32, EquipRuneArray>				m_EquipRuneArrayMap;//key:符文页 value:符文数组
	map<ePrivilegeType, SPrivilege>			m_ItemAdditionMap;		//加成表
	
	stringstream m_RuneBagStream;
	stringstream m_RuneSlotStream;

private:
	void								UpdateRuneBagJson();
	void								UpdateRuneSlotJson();

public:
	void								InitRunes(const string& bagJson, const string& slotStr);
	map<INT32, EquipRuneArray>&			GetEquipRuneArrayMap() { return m_EquipRuneArrayMap; }
	void								CheckDbSaveTimer(int64_t curTime, int64_t tickSpan);
	void								SetTimerID(int64_t id){m_TimerID = id;}
	int64_t								GetTimerID()const{return m_TimerID;}
	void								OnOnline(SUserNetInfo& netinfo,GCToCS::Login &pLogin,bool isFirstInDB,bool isFirstInMem,bool isReLogin=false);
	void								OnOffline();
	void								CheckHeroValidTimer(int64_t curTime);

	EUserPlayingStatus					GetUserPlayingStatus() const{return m_eUserPlayingStatus;}
	const SUserNetInfo&					GetUserNetInfo() const{return m_sUserNetInfo;}
	const INT64							GetGUID() {return m_sUserDBData.sPODUsrDBData.un64ObjIdx;}
	const string&						GetUserName() {return m_sUserDBData.szUserName;}
	const string&						GetNickName() {return m_sUserDBData.szNickName;}
	INT32								GetUserLv() { return m_sUserDBData.sPODUsrDBData.un8UserLv; }
	INT64								GetScore() {return m_sUserDBData.sPODUsrDBData.n64Score;}
	INT64								GetDiamond() {return m_sUserDBData.sPODUsrDBData.n64Diamond;}
	INT64								GetGold() {return m_sUserDBData.sPODUsrDBData.n64Gold;}		
	CCSUserBattleInfo&					GetUserBattleInfoEx() { return m_sUserBattleInfoEx; }
	IRoomPlayer*						GetRoomPlayer() { return &m_tRoomPlayer; }
	IMatchPlayer*						GetMatchPlayer() { return &m_tMatchPlayer; }
	UINT32								GetHeadID() const{return m_sUserDBData.sPODUsrDBData.un16HeaderID;};
	INT32								GetChangeNicknameCost();
	CTaskMgr*							GetTaskMgr() { return m_cTaskMgr; }
	INT32								KickOutOldUser();
	INT32								ResetPingTimer();
	
	INT32								LoadDBData(SUserDBData &crsDBData);
	void								LoadUserSNSList(DBToCS::RSinfo msg_snslist);
	
	void								AddUserItems(DBToCS::ItemInfo& itemInfo);
	INT32								PostMsgToGC(google::protobuf::Message& sMsg, int n32MsgID);
	INT32								PostMsgToGC_AskRetMsg(INT32 n32AskProtocalID, INT32 n32RetFlag);
	INT32								PostMsgToSS_UserInfoChange();

	bool								GetHeroVec(vector<HeroListStruct>& heroVec);
	bool								GetHeroVec(vector<UINT32>& heroVec); 
	 
	INT32								AddHero(UINT32 un32HeroGoodsID, bool ifFree);
	INT32								AddHero(const SUserHeroDBData &db);
	INT32								AddExperienceHero(UINT32 heroid, INT64 addtime, bool iftimeAdd);
	INT32								UpdateHero(const UINT32 heroid, const INT64 endtime);
	
	void								ClearNetInfo(); 
	void								CompSSGuideBattle()				{ m_sUserDBData.ChangeUserDbData(eUserDBType_ClearSSGuideSteps,"");}
	SUserDBData&						GetUserDBData()	{return m_sUserDBData;}
	const INT64&						GetUserGUID()	{return m_sUserDBData.sPODUsrDBData.un64ObjIdx;}
	bool								IsOverTime();
	INT32								SetUserNetInfo(const SUserNetInfo &crsUNI);//设置User下网络信息，但未设置玩家上下线状态，未修改UserNetMap表//

	INT32								AddRunes(SUserRunesBagRuntimeData sSUserRunesBagRuntimeData, bool ifNotifyClient);
	INT32								AddRunes(UINT32 un32RunesID, bool ifNotifyClient);
	INT32								AddRunes(UINT32 un32RunesID, bool ifNotifyClient, int num);
	INT32								DecreaseRune(vector<UINT32>& un32RunesID, bool ifNotifyClient);

	INT32								ReplyAddFriendRequst(INT8 n8Reply,  const UINT64 guid_asker, const UINT64 rGuidIdx);

	bool								CheckIfInFriendList(const UINT64 guididx);
	bool								CheckIfInBlacklist(const UINT64 guididx);
	bool								CheckIfInAddSNSList(const UINT64 guididx);
	bool								IsFirstWin();
	bool								CheckIfEnoughPay(EPayType type, INT32 pay);
	bool								IfHasSkinsGoods(UINT32 un32HeroID);
	bool								IfHasBuyHero(UINT32 un32HeroID);
	bool								IfHasBuyRunes(UINT32 un32RunesID);
	bool								OtherItemIfTimeAdd(ItemEffect effect);

	INT32								GetUserFriendsListSize() { return m_sUserDBData.friendListMap.size(); }
	INT32								RemoveUserFromAddVec(UINT64 Idx); 
	void								CheckUpgrade(INT32 got_exp);
	void								CalculateItemAddition();
	INT32								GetUserAddition(ePrivilegeType type);
	INT32								ChangeOtherItemMap(UINT32 ItemID, INT32 num);
	INT64								GetItemAdditionSurplusTime(ItemEffect effect);
	bool								LoadFromRedisStr(const string& heroStr, const string& friendStr, const string& blackStr, const string& ite, const string& mailStr);

	/************************************************************************/
/*                  ask handle                                          */
/************************************************************************/
public:
	INT32								AskInviteFriendsToBattle(UINT64 un64BattleID, const UINT64 guidIdx);
	
	INT32								AskSendMsgToUser(const UINT64 rGuidIdx,INT32 n32MsgLength, const CHAR *pbMsg);
	INT32								AskRemoveFromSNSList(const UINT64 guidIdx, INT32 eFriendType);
	INT32								AskAddToSNSMap(const UINT64 guidIdx, SUserRelationshipInfo& temp_info);
	INT32								AskRemFromSNSMap(const UINT64 guidIdx, ERelationShip temp_type);
	INT32								AskAddToSNSlistByNickName(const string &sz_nickname, ERelationShip type);
	INT32								AskAddToSNSlistByGuid(const UINT64 guid, ERelationShip type);
	INT32								AddToFriendsLlist(CCSUser *piUser, ERelationShip type);
	INT32								AddToBlackList(const UINT64 guid);
	INT32								GetFriendRSInfo(const UINT64 guid, SUserRelationshipInfo &temp_info, ERelationShip type);
	INT32								GCAskExchangeRunes(INT64 n64ExchangeNum);
	INT32								GCAskEqipRunes(UINT32 runeID, INT32 n16RunePage);
	INT32								GCAskUnloadRunes(INT32 page, INT32 n16RuneFromPos);

	bool								AddRuneNum(UINT32 runeId, INT32 runeNum, bool IfNotifyClient);

	INT32								GCAskComposeRunes(GCToCS::ComposeRunes& sMsg);
	INT32								GCAskUserGameInfo();
	void								OnNewDay();
	void								OnNewMonth();
	void								OnNewYear();
	void								LoginRewardInit();
	INT32								AskGetLoginReward();
	INT32								AskBuyGoods(GCToCS::AskBuyGoods& sGoods);
	void								AskChangeheaderId(const UINT32 un32headerid);
	INT32								AskRecoinRune(UINT32 rune_id, INT32 cradID);
/************************************************************************/
/*                  Post handle                                         */
/************************************************************************/
public:
	INT32								PostMsgToGC_PingRet();

	INT32								SynUser_UserBaseInfo();
	INT32								SynUser_AllHeroList();
	INT32								SynUser_AllRunesList();
	INT32								SynUser_IsOnSS();
	//同步物品信息
	INT32								SynOtherItemInfo(UINT32 item_id);

	INT32								PostMsgToGC_NetClash();
	INT32								PostMsgToGC_NotifyUserGameInfo();
	INT32								SynUserSNSList(const UINT64 guid_friends, ERelationShip eRSType);

	INT32								PostMsgToGC_NotifuUserReMoveFromSNSList(ERelationShip eRSType, const UINT64 guididx);
	INT32								PostMsgToGC_NotifyBeAddFriendMsg(const string& stSender,const UINT64 guid_sender, const string& stRec);
	INT32								PostMsgToGC_UserBeInvitedToBattle(UINT64 un64BattleID, const string& stPwd, const string& stNickName);
	INT32								PostMsgToGC_MsgFromUser(const UINT64 guid, const string nickaneme, UINT32 headerid, INT32 n32MsgLength, const CHAR *pbMsg);
	INT32								SynCurGold();
	INT32								SynUserLvInfo();
	INT32								SynCurDiamond();
	INT32								PostMsgToGC_NotifyGetLoginRewardScuess();
	INT32								PostMsgToGC_NotifyReward();
	INT32								SynCommidityCfgInfo();
	INT32								NotifyCanInviteFriends();
	INT32								SynUserCLDays();
	INT32								NotifyUserPlayState();
	
	INT32								PostMsgToGC_NotifyNewNickname(const UINT64 guid, const string& sznickname);
	INT32								PostMsgToGC_NotifyNewHeaderid( const UINT64 guid, const UINT32 headerid);
	void								PostCSNotice();
	void								NotifyCDKGiftToGC(INT32 item_id, INT32 num);


private:
	void								GetUserHeroList_RedisCallback(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pUserData);

	///////////////////////////////////////新手引导/////////////////////////
public:  
	 INT32								PosUserCSCurtGuideSteps();

	 bool								SaveToRedis();

private:
	 void								LoadFriendFromRedis( const string& friendStr , map<UINT32, SUserRelationshipInfo>& OriMap);
	 void								LoadItemFromRedis( const string& mailStr );
	 void								LoadMailFromRedis( const string& itemStr );
	 void								LoadHeroFromRedis( const string& heroStr );

	 bool								ConsumeCombine(int runeLevel);			

};

}
