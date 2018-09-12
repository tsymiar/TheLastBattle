#pragma once
#include "FBAll_Const.h"
#include "SSElementArr.h"
#include "SSElementArr.inl"
#include "json/json.h"
#include "stdafx.h"

namespace CentralServer{
//UserDBDate类型
enum EUserDBDataType
{
	eUserDBType_None = 0,//			un64ObjIdx; 
	eUserDBType_Channel,//		eUserPlatform;
	eUserDBType_Name,//		szUserName[c_n32DefaultNameLen];
	eUserDBType_Pwd,//			szUserPwd[c_n32DefaultUserPwdLen]; 
	eUserDBType_NickName,//				szNickName[c_n32DefaultNickNameLen+1];
	eUserDBType_HeaderId,//		un32HeaderID;
	eUserDBType_Sex,//				n8Sex; 
	eUserDBType_Diamond,//			n64Diamond;
	eUserDBType_Gold,//			n64Gold;;
	eUserDBType_RegisterTime,//			tRegisteUTCMillisec;	//注册时间(s)
	eUserDBType_Score,//				n64Score;
	eUserDBType_TotalGameInns,//			un32TotalGameInns;
	eUserDBType_TotalWinInns,//		un32TotalWinInns;
	eUserDBType_TotalHeroKills,//				un32TotalHeroKills;
	eUserDBType_TotalDestoryBuild,//				un32TotalDestoryBuildings;
	eUserDBType_TotalDeadTimes,//				un32TotalDeadTimes;

	eUserDBType_UserLv,//				un16UserLv;
	eUserDBType_UserLvExp,//				un32UserCurLvExp;
	eUserDBType_LastFirstWinTime,//			tLastFirstWinTime; 

	eUserDBType_CLDay,//				un16Cldays;
	eUserDBType_LastGetLoginReward,//			tLastGetLoginReward;

	eUserDBType_Friends,	//				szFriends[c_un32FriendsLen]; 

	eUserDBType_UpSSGuideSteps,	//SUserGuideSteps		mGuideSteps;
	eUserDBType_ClearSSGuideSteps,	//SUserGuideSteps		mGuideSteps; 

	eUserDBType_UpCSGuideConSteps,	//CSUserGuideConSteps		mGuideSteps;
	eUserDBType_UpCSGuideNoConSteps,	//CSUserGuideNoConSteps		mGuideSteps;


	eUserDBType_HeroList,//		szHeroList[c_un32HeroListLen];
	eUserDBType_BuyGoods,////SUserBuyGoods		mSUserBuyGoods[c_TotalGoods];

	eUserDBType_VIPLevel,
	eUserDBType_VIPScore,

	eUserDBType_TotalAssist,
	eUserDBType_End
};

struct GoodsCfgInfo{
	UINT8	goodsType;
	UINT32	goodsID;
};
struct NewGoodsCfg{
	GoodsCfgInfo sGoodsCfgInfo;
};

struct HotGoodsCfg{
	GoodsCfgInfo sGoodsCfgInfo;
};

struct SUserRunesBagRuntimeData{
	SUserRunesBagRuntimeData(UINT32	un32cRunesTypeID,
		INT16	n16cNum
		, TIME_MILSEC buyTime):un32RunesTypeID(un32cRunesTypeID)
		, n16Num(n16cNum)
		, n64GotTime(buyTime){
	}
	SUserRunesBagRuntimeData():un32RunesTypeID(0)
		, n16Num(0)
		, n64GotTime(0){
	}
	UINT32	un32RunesTypeID;
	INT16	n16Num;
	TIME_MILSEC	n64GotTime;

	bool operator<(const SUserRunesBagRuntimeData& sSUserRunesRuntimeData){
		return n64GotTime < sSUserRunesRuntimeData.n64GotTime;
	}
};

struct SUserRunesSlotRuntimeData{
	SUserRunesSlotRuntimeData(UINT32	un32cRunesTypeID,
		INT16	page
		, INT16 pos):un32RunesTypeID(un32cRunesTypeID)
		, page(page)
		, pos(pos){
	}
	SUserRunesSlotRuntimeData():un32RunesTypeID(0)
		, page(0)
		, pos(0){
	}
	UINT32	un32RunesTypeID;
	INT16	page;
	INT16   pos;
};

struct SUserRunesDBData{
	SUserRunesBagRuntimeData sUserRunesRuntimeData;
	UINT8				  un8PosType;
};

struct SUSerSkinDBData{
	UINT32	un32SkinTypeID;
};

struct	SUserRelationshipInfo{
	string	stNickName; 
	UINT16	nHeadId;
	UINT64	guididx;
	UINT32	viplv;
	ERelationShip eRelationShip;
	TIME_MILSEC tMilSec;

	//默认构造函数
	SUserRelationshipInfo()
		:stNickName(""),
		nHeadId(0),
		eRelationShip(ERelationShip::eRSType_None),
		guididx(0),
		tMilSec(0),
		viplv(0)
	{

	}

	//构造函数
	SUserRelationshipInfo(const string& stName, INT32 headId, ERelationShip ers, TIME_MILSEC tm, UINT64 GuidIdx, UINT32 vipLv)
		:stNickName(stName.length()<= c_n32DefaultNickNameLen ? stName : ""),
		nHeadId(headId),
		eRelationShip(ers),
		tMilSec(tm),
		guididx(GuidIdx),
		viplv(vipLv)
	{

	}

};

struct	SUserItemInfo{
	UINT32	item_id;
	bool	ifusing;
	INT32	item_num;
	UINT64	buy_time;
	UINT64	end_time;

	SUserItemInfo():item_id(0)
		, end_time(0)
		, ifusing(false)
		, item_num(0)
		, buy_time(0)
	{}

	void Clear(){
		item_id = 0;
		end_time = 0;
		ifusing = false;
		item_id = 0;
		buy_time = 0;
	}
};

struct SUserBattleInfo
{
	INT32	n32SSID;
	UINT64	un64CurBattleID;
	UINT32	un32MapID;
	INT32	n32BattleState;

	bool	bGuideBattle;

	SUserBattleInfo() {memset(this,0,sizeof(*this));}

	void Clear(){
		n32SSID = 0;
		un64CurBattleID = 0;
		un32MapID = 0;
		n32BattleState = eBS_Free;
		bGuideBattle = false;
	}
};


struct ConsumeStruct{
	INT32 type;
	INT32	price;
	ConsumeStruct(INT32 type, INT32 price):type(type), price(price){}
	ConsumeStruct():type(0), price(0){}
};
typedef ElementArr<ConsumeStruct, 2> ConsumeList;


struct HeroListStruct{
	UINT32 heroid;
	TIME_MILSEC	expiredTime;
	bool ifFree;
	HeroListStruct(UINT32 heroid, TIME_MILSEC expiredTime, bool ifFree):heroid(heroid), expiredTime(expiredTime), ifFree(ifFree){}
};

struct SHeroBuyCfg{
	UINT32	un32CommondityID;
	UINT32	un32HeroID;
	EHeroKind eHeroKind[3];
	BOOLEAN	bIsShowHot;
	ESkinType	eDefaultSkin;
	ESkinType	eOnSaleSkins;
	TIME_MILSEC	useTimeSpan;
	ConsumeList	sConsumeList;
	bool bIfShowInShop;
};

struct SDiscountCfg
{
	UINT32	un32CommdityID;
	GoodsType	eGoodsType;
	UINT32	un32GoodsID;
	ConsumeList sConsumeList;
};

struct SGoodsBuyCfg
{
	UINT32	n32GoodsID;
	UINT8	un8ConsumeTypeArr[2];
	INT32	n32PriceArr[2];
};

struct SUserHeroDBData{
	UINT32			un32HeroID;
	INT64			endTime; // time(NULL)
	INT64			buyTime; //购买时间   time(NULL)

	SUserHeroDBData() {memset(this,0,sizeof(*this));}
	SUserHeroDBData(UINT32 idx, time_t life, TIME_MILSEC btime)
	{
		un32HeroID = idx;
		endTime = life;
		buyTime = btime;
	}
	bool operator ==(const SUserHeroDBData& sData){
		if (this == &sData){
			return true;
		}

		return un32HeroID == sData.un32HeroID && endTime == sData.endTime && buyTime == sData.buyTime;
	}
};

#define MAX_REWARD 5
struct SLoginReward
{
	INT32				n32Days;
	ERewardType			eRewardType[MAX_REWARD];
	ELoginRewardItemType	eItemType[MAX_REWARD];
	UINT32				un32num[MAX_REWARD];

	SLoginReward():n32Days(0){
		for (INT32 i = 0; i < MAX_REWARD; i++ ){
			eRewardType[i] = eRewardType_None;
			eItemType[i] = eLoginRewadItem_None;
			un32num[i] = 0;
		}
	}
};

enum ItemEffect{
	eItemEffect_None = 1000,
	eItemEffect_GoldGain = 1001,
	eItemEffect_ExpGain = 1002,
	eItemEffect_AllHeroFree = 1003,
	eItemEffect_OneHeroFree = 1004,
	eItemEffect_RuneRecoin = 1005,
};

struct SOtherItem{
	UINT32		item_id;			//物品ID
	ItemEffect	effect_type;		//物品类型
	UINT32		effect_value;		//物品生效值或等级
	UINT32		consume_type;		//消费类型
	bool		bIsGetUse;			//是否获取即使用
	UINT32		price_series;		//价格
	UINT8		b_inshop;			//是否在商店显示
	UINT32		item_duration;		//物品有效时间
};


//CS user config
struct UserDbSaveConfig 
{
	INT32 dbSaveTimeSpace;
	INT32 delayDelFromCacheTime;

	bool  dbCSLogSwitch;
	bool  dbSSLogSwitch;
	INT32	logRefreshTime;

	bool  sSGMCmdSwitch;
	bool  ssSolderBornSwitch;
	bool  ssMonsterBornSwitch;
	bool  ssAltarSolderSwitch;

	INT32	m_MaxUserMailN;
	bool	m_IfDelAtLooked;
	bool	m_ifSortAtLooked;

	UserDbSaveConfig() {memset(this,0,sizeof(*this));}

	void clear()		{memset(this,0,sizeof(*this));}
};

#define GUIDE_SIGN ","
struct SUserGuideSteps
{ 
	string   szCSContinueGuide;						//CS界面引导,记录的都是已经完成的引导(格式:1001,1002,....,ok),ok代表完成该引导
	bool   bSSGuideState;							//战场引导是否完成(战场引导应完成一个CS的界面引导)
	bool   bIsChange;								//是否该引导有改变(用于保存)

	SUserGuideSteps():bSSGuideState(false), bIsChange(false) {}

	bool ifGuideEnd()
	{
		if (-1!= szCSContinueGuide.find("ok") ) 
		{
			return true;
		}
		return false;
	}

	bool CheckGuideTaskId(INT32 taskId)
	{
		if(ifGuideEnd())
		{
			return true;
		} 
		char szTask[12]={0};
		sprintf(szTask,"%d",taskId);
		if (  -1 != szCSContinueGuide.find(szTask))
		{
			return true;
		}
		return false;
	}
	bool UpdateGuideInfo(INT32 taskId, bool b)
	{
		if (CheckGuideTaskId(taskId))
		{
			return true;
		}
		bIsChange = true;

		stringstream mystream;
		mystream << szCSContinueGuide;
		mystream <<GUIDE_SIGN;
		mystream <<taskId;

		if (b)
		{
			mystream << GUIDE_SIGN;
			mystream << "ok";
		}

		mystream >> szCSContinueGuide; 
		return false;
	} 
	void GetCurtGuideInfo(std::stringstream & mystream, UINT64 objID)
	{ 
		if (bIsChange)
		{
			mystream << "update gameuser_guide set obj_cs_guide_com_steps = '" <<  szCSContinueGuide << "'" << " where obj_id = " << objID << ";";

			bIsChange = false;
		} 
	}
};

typedef ElementArr<SUserHeroDBData, c_un32HeroListLen> HeroList;
struct PODUsrDBData{
	UINT64				un64ObjIdx;//玩家唯一标识
	EUserPlatform		eUserPlatform;
	UINT16				un16HeaderID;//玩家头像ID在200内
	INT16				n16Sex; 
	INT64				n64Diamond;
	INT64				n64Gold;;
	time_t				tRegisteUTCMillisec;	//注册时间(s)
	INT64				n64Score;
	UINT32				un32TotalGameInns;
	UINT32				un32TotalWinInns;
	UINT32				un32TotalHeroKills;
	UINT32				un32TotalDestoryBuildings;
	UINT32				un32TotalDeadTimes;
	UINT32				un32TotalAssist;

	UINT8				un8UserLv;//玩家等级，最高30级
	UINT32				un32UserCurLvExp;
	UINT64				tLastFirstWinTime; 

	UINT16				un16Cldays;
	UINT32				un32LastGetLoginRewardDay;

	INT16				un16VipLv;
	INT32				vipScore;
	volatile bool		m_IfChangeArr[eUserDBType_End];
};

struct SUserDBData
{
	PODUsrDBData		sPODUsrDBData;
	string				szNickName;
	string				szUserName;
	string				szUserPwd;
	SUserGuideSteps		mGuideSteps;
	string				szTaskData;
	bool				isTaskRush;
	map<UINT32, SUserHeroDBData> heroListMap;
	map<UINT32, SUserRelationshipInfo> friendListMap;
	map<UINT32, SUserRelationshipInfo> blackListMap;
	map<UINT32,	SUserItemInfo>	item_Map;
			
	SUserDBData() {Clear();}
	void Clear(){
		memset(&sPODUsrDBData, 0, sizeof(sPODUsrDBData));
		isTaskRush = false;
		heroListMap.clear();
		friendListMap.clear();
		blackListMap.clear();
		item_Map.clear();
	}

	SUserDBData& operator = (const SUserDBData& sData){
		if (this == &sData){
			return *this;
		}
		memcpy(&this->sPODUsrDBData, &sData.sPODUsrDBData, sizeof(sData.sPODUsrDBData));
		szNickName = sData.szNickName;
		szUserName = sData.szUserName;
		szUserPwd = sData.szUserPwd;
		szTaskData = sData.szTaskData;
		isTaskRush = sData.isTaskRush;
		heroListMap = sData.heroListMap;
		friendListMap = sData.friendListMap;
		blackListMap = sData.blackListMap;
		item_Map = sData.item_Map;
		return *this;
	}
	void AddHero(SUserHeroDBData& sSUserHeroDBData){
		heroListMap.insert(std::make_pair(sSUserHeroDBData.un32HeroID, sSUserHeroDBData));
	}

	void ChangeUserDbData(EUserDBDataType dtype, INT64 param ){
		switch(dtype){
		case	eUserDBType_Channel:
			sPODUsrDBData.eUserPlatform = (EUserPlatform)param;
			break;
		case	eUserDBType_HeaderId:
			sPODUsrDBData.un16HeaderID = param;
			break;
		case		eUserDBType_Sex:
			sPODUsrDBData.n16Sex = param;
			break;
		case	eUserDBType_RegisterTime:
			sPODUsrDBData.tRegisteUTCMillisec = param;
			break;
		case		eUserDBType_Score:
			sPODUsrDBData.n64Score = param;
			break;
		case		eUserDBType_LastFirstWinTime:
			sPODUsrDBData.tLastFirstWinTime = param;
			break;
		case		eUserDBType_CLDay:
			sPODUsrDBData.un16Cldays = param;
			break;
		case		eUserDBType_Diamond:
			sPODUsrDBData.n64Diamond += param; 
			break;
		case	eUserDBType_Gold:
			sPODUsrDBData.n64Gold += param;
			break;
		case		eUserDBType_TotalGameInns:
			sPODUsrDBData.un32TotalGameInns += param;
			break;
		case		eUserDBType_TotalWinInns:
			sPODUsrDBData.un32TotalWinInns += param;
			break;
		case		eUserDBType_TotalHeroKills:
			sPODUsrDBData.un32TotalHeroKills += param;
			break;
		case		eUserDBType_TotalDestoryBuild:
			sPODUsrDBData.un32TotalDestoryBuildings += param;
			break;
		case		eUserDBType_TotalDeadTimes:
			sPODUsrDBData.un32TotalDeadTimes += param;
			break;
		case		eUserDBType_UserLv:
			sPODUsrDBData.un8UserLv += param; 
			break;
		case		eUserDBType_UserLvExp:
			sPODUsrDBData.un32UserCurLvExp += param;
			break;
		case eUserDBType_VIPLevel:
			sPODUsrDBData.un16VipLv = param;
			break;
		case eUserDBType_VIPScore:
			sPODUsrDBData.vipScore = param;
			break;
		case  eUserDBType_TotalAssist:
			sPODUsrDBData.un32TotalAssist += param;
			break;
		case eUserDBType_LastGetLoginReward:
			sPODUsrDBData.un32LastGetLoginRewardDay = param;
			break;
		}

		sPODUsrDBData.m_IfChangeArr[dtype] = true;
	}
	void ChangeUserDbData(EUserDBDataType dtype, const string& param )
	{
		switch(dtype){
		case	eUserDBType_Name:
			szUserName = param;
			break;
		case		eUserDBType_Pwd:
			szUserPwd = param;
			break;
		case		eUserDBType_NickName:
			szNickName = param;
			break;
		}

		sPODUsrDBData.m_IfChangeArr[dtype] = true;
	}

};

struct  CalculateAdd
{
	UINT32	user_exp_add;
	UINT32	user_gold_add;
};

const INT32 HeroBuyCfgBegin = 100001;
const INT32 HeroBuyCfgEnd = 109999;
static bool IfHeroCfgID(UINT32 id){
	return id >= HeroBuyCfgBegin && id <= HeroBuyCfgEnd;
}
const INT32 RuneBuyCfgBegin = 120000;
const INT32 RuneBuyCfgEnd = 130000;
static bool IfRuneCfgID(UINT32 id){
	return id >= RuneBuyCfgBegin && id <= RuneBuyCfgEnd;
}
const INT32 SkinBuyCfgBegin = 120001;
const INT32 SkinBuyCfgEnd = 129999;
static bool IfSkinCfgID(UINT32 id){
	return id >= SkinBuyCfgBegin && id <= SkinBuyCfgEnd;
}
const INT32 OtherBuyCfgBegin = 130001;
const INT32 OtherBuyCfgEnd = 139999;
static bool IfOtherCfgID(UINT32 id){
	return id >= OtherBuyCfgBegin && id <= OtherBuyCfgEnd;
}

const INT32 RuneWashCfgBegin = 130005;
const INT32 RuneWashCfgEnd = 130010;

const UINT16 c_matcherDelay = 100;
const UINT8	c_maxMatcherNum = 12;

class CCSUser;
typedef ElementArr<CCSUser*, c_maxMatcherNum> CCSUserList;
typedef map<UINT32,CCSUserList> CCSUserListMap;

struct SRunesCfg{
	EObjectType eOT;
	UINT8 un8Level;
	UINT8 un8EffectID;
	FLOAT fEffectVal;
	FLOAT fEffectPer;
	BOOLEAN bIsCanComposed;
	INT32 n32ComposedSubID;
	ConsumeList	sConsumeList;
	bool bIfShowInShop;
};

const	UINT32		c_un32ComposeRunesNum = 3;
const	UINT32		c_un32MaxRunesOverlapNum = 9;
const	UINT32		c_un32MaxRuneBagNum = 100;
const INT32	c_n32MaxSlotInRunePage = 10;
struct AskCDKGiftData{
	INT64 pGuid;
	string cdk;
};
}