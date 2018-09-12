#pragma once
#include "PreDefine.h"
#include "WYYTools/Vector3D.h"
#include "ServerMath.h"
#include <string>
#include <vector> 
#include <time.h>
#include <atltime.h>
#include <sstream>
#include "boost/spirit.hpp" 
#include "boost/algorithm/string.hpp"
#include <random>

#include <map>
#include <hash_map>

//注意：不要随意在此文件添加全局数据；可在每个Server添加
using namespace std;
	enum EVENT_TYPE
	{
		EVT_NONE,
		EVT_ON_CS_REGISTERED,
		EVT_ON_CS_DISCONNECT,
		EVT_ON_SS_REGISTERED,
		EVT_ON_SS_DISCONNECT,
		EVT_ON_GS_REGISTERED,
		EVT_ON_GS_DISCONNECT,
		EVT_SS2CS_SyncAllGameInfo,
	};

const	INT32	c_n32MaxInt32 = 0xffffffff / 2;
const	UINT32	c_un32MaxUInt32 = 0xffffffff;
const	INT32	c_n32DefaultBufferSize = 4096;
const	TIME_TICK	c_tDefaultPingCDTick = 1000 * 160;
const	TIME_TICK	c_tSSPingCSCDTick = 1000 * 10;//20秒CS未收到SS消息则不分配战斗//
const	TIME_TICK	c_tUserPingCDTick = 1000 * 180;
const	TIME_TICK	c_tDefaultPingCDTickFromClient = 1000 * 180;

const	INT32	c_n32DefaultNameLen = 30;
const	INT32	c_n32DefaultNickNameLen = 32;
const	INT32	c_n32DefaultUserPwdLen = 33;
const	INT32	c_n32DefaultDeviceKeyLen = c_n32DefaultNameLen * 5;
const	INT32	c_n32ProjectileMaxChildProjectileType = 3;
const	INT32	c_n32DefaultRemoveConsoleKeyLen = 65;

const	INT32	c_n32MaxOrderPathNode = 30;
const	UINT32		c_un32MaxUserInBattleCamp = 3;
const	UINT32		c_un32MaxObserverInBattle = 2;
const	UINT32		c_un32MaxUserInBattle = c_un32MaxUserInBattleCamp * 2;
const	UINT32		c_un32MaxAllUserInBattle = c_un32MaxUserInBattle + c_un32MaxObserverInBattle;
const	UINT32		c_un32TipsToSelectHeroTime = 1000 * 50;
const	UINT32		c_un32BattleSelectHeroTimeoutMilsec = 1000 * 60;
const	UINT32		c_un32BattleLoadingTimeoutMilsec = 1000 * 60;
const	UINT32		c_un32PlayingBattleTimeoutMilsec = 1000 * 3600 * 2; //
const	UINT32		c_un32MaxTimeWaitAllUserOffline = 1000 * 180;
const	UINT32		c_un32BattleShowResult = 1000 * 6;
const	UINT32		c_un32CombatSettlementTime = 1000 * 12;
const	UINT32		c_un32PlayVictoryVideoTime = 1000 * 6;
const	UINT32		c_un32StoreDBDataCDMilsec = 1000 * 60 ;//* 30;
const	UINT32		c_un32ShowTime = 1000 * 10;
const	UINT32		c_un32GlobalObjBeginIdx = 100000000;
const	UINT32		c_un32ObjTypeSpace = 10000;
const	UINT8		c_un32GoodsBagNum = 2;
const	UINT8		c_un32GridNumOfBag = 3;
const	UINT8		c_un32MaxGridNum = c_un32GoodsBagNum * c_un32GridNumOfBag;
const   UINT32		c_un32HeroMaxBornPos	= 6;
const   UINT32		c_n32DefaultFilePathLen     = 32;
const	UINT32		c_un32MaxGoodsDownInfoNum = 10;
const	UINT32		c_un32MaxItemEffectNum = 10;
const	UINT32		c_un32MaxSenceTypeNum = 3;
const	UINT32		c_un32MaxGoodsChildNum = 512;
const	UINT32		c_un32MaxRunesNum = 512;
const	UINT32		c_un32MaxRunesSlot = 9*20;
const	TIME_MILSEC		c_tDefaultNPCBornCDMilsec = 30;

const	INT32		c_n32MiniDistToTar = 25;
const   UINT32		c_un32DefaultRadius = 5;
const	INT32		c_n32MaxSkillEffectNum =3;
const	INT32		c_n32MaxSkillBuffNum =3;
const	INT32		c_n32MaxNPCSkillNum = 5;
const	INT32		c_n32MaxHeroSkillNum = 7;
const	INT32		c_n32MaxHeroSkillNum_ex = 9;

const	INT32		c_n32MaxHeroPassitiveSkillNum = 3;
const	INT32		c_n32NormalAndGasHeroSkillNum = 4;
const	INT32		c_n32MaxGoodsSkillNum = 1;
const INT32			c_n32CellSize = 100;
const INT32			c_n32MiniCollideRadius = 0;
const INT32			c_n32HalfCellSize = 25;
const INT32			c_n32AreaCellNum = 32;
const INT32			c_n32MAxBattlePwdLen = 6;
const INT32			c_n32MaxAbsorbAttriChangeNum = 13;
const	INT32		c_n32MaxMoveSpeed = 1100;
const INT32 c_n32MapDefaultValue = 2;	//the default value of map position proportion 
const INT32 c_n32FindPathDefaultArrNom = 20;
const INT32 c_n32DefaultBlance = 1000;
const INT32 c_n32DefaultBlockStopTime = 1000;
const INT32 c_n32StopBlockNum = 2; //max block time, so this obj stop action
const UINT32 c_un32InValidBattleID = 0;
const UINT32	c_un32MaxBuffCollideNum = 20;
const INT64   c_un32MaxAroundCell = 20;
const UINT32		c_un32MaxHeroLevel = 15;
const UINT32	c_un32HeroMinAbsorbLevel = 3;
const UINT32	c_un32HeroMidAbsorbLevel = 7;
const INT32		c_n32MaxAbsorbSkillNum = 2;
const INT32		c_n32AbsorbTimeout = 1000 * 3;
const INT32		c_n32CloseDist = 5;
const UINT16  c_un16MaxPatrol = 3;
const UINT16  c_un16MaxMonster = 6;
const UINT16  c_un16NoticeLen = 32; 
const UINT32 c_un32NpcGivenFury = 5;
const UINT32 c_un32HeroGivenFury = 40;
const UINT32 c_un32PerFuryValue = 120; 
const UINT32 c_un32FuryRunTime = 18 * 1000;
const UINT32 c_un32RecoverCpTime = 1 * 1000;
const INT32	c_n32DefaultBlockMilsec = 0;
const UINT32 c_un32MoveTarMaxBlockTick = 3000;
const INT32	c_n32DefaultFreeAlarmCDTick = 3000;
const UINT32 c_un32IntervalSendPos = 500;
const UINT32  c_un32CheckMaxSkillTargetNum = 100;
const UINT32 c_un32DefaultGoodsSkillIdx = 85000;
const INT32 c_n32MaxNextSkillEffectCfgNum = 16;
const INT32 c_n32MaxDependSkillEffectCfgNum = 16;
const INT32 c_n32MaxEffectCaculateCateNum = 16;
const INT32 c_n32MaxGameUnitBuffNum = 32;
const INT32	c_n32MaxBattleInSS = 200;

const INT32	c_n32MaxEffectAddNum = 2048;
const UINT32 c_un32AddBattleCost = 35;
const INT32  n_c32SkkEffectNum = 32;
const INT32  n_c32NPCNum = 5;
const INT32 n_c32SkillTriggerRate = 1000;

const INT32 c_n32MaxLimits = 2147483647;
const UINT32 c_un32MaxLimits = 4294967295;
const INT32	c_n32MaxlongExpireSolierTime = 1000 * 60 * 3;
const INT32	c_n32OccupyWalkCheckTimeSpan = 500;

const UINT32 c_n32GuideStepLen = 128;
const UINT32 c_n32GuideBlStepLen = 64;

const TIME_MILSEC PersistTimeAlways = -1;
const UINT8 c_un8MaxCLDaysReward = 7;
//MilSec To time
const UINT64 c_unOneDayMilSec = 1000 * 60 * 60 * 24;
const UINT64 c_unOneHourMiSec = 1000 * 60 * 60;
const UINT64 c_unOneMinMiSec = 1000 * 60;

const UINT32 c_OneDaySec = 24 * 60 * 60;
// Battle end compute
// const UINT16 c_un16BattleWinBaseGold = 200;

//被动技能系统开始
const INT32	n_cPassiveSkillTriggerNum = 64;
//OB
const INT64 c_n64OBUserDefaultId = 3000000; 

const INT32	c_n32DefaultPerCp = 400;

const INT32 c_n32MaxWhileNum = 999;

const INT32 c_n32EnterTimeOver = 60000;

const UINT8 c_un8MaxFriends = 50;
const UINT8 c_un8MaxBlacklist = 50;
const UINT16 c_un8MaxMsgLength = 360; 
const UINT8 c_un8MaxMsgSave = 100; 
const UINT32 c_un32MinOpertionTime = 10 * 1000;

const UINT32 c_un32FriendsLen = 1024; 
const UINT32 c_un32HeroListLen = 128; 
 
const UINT8	c_un8MaxNickNameQuery = 10; 

const INT32 c_NoticeLen = 512;

const UINT64  c_DefaultUserIdx = 10000;
const UINT32  c_DefaultSaveTimeSpace = 180000;

const INT32	  c_BattleTimeOut = 360000;
const INT32   c_BattleLogTimeSpace = 120;

const INT32   c_MaxMailN = 50; 
const INT32   c_MailTitleLen = 64;
const INT32   c_MailContenLen = 256; 
const INT32   c_MailGiftLen = 128; 
const INT32   c_MailSenderLen = 32; 
const INT32   c_MailGetLen = 32;
const INT32   c_MailCreateTimeLen = 32;
const INT32   c_MailEndTimeLen = 32;

const INT32	c_n32MaxPathNumInBattle = 3;
const INT32	c_n32AroundAreaNum = 9;
const UINT32  un32MapObjNum = 100;
const UINT32  c_n32DefaultXCellNum = 400;
const UINT32  c_n32DefaultZCellNum = 400; 

/******************************************************/ 
//hero reborn 相关
const INT32		c_BaseRebornTimes = 3;
const INT32		c_FirBuyRebornCost = 1;
const INT32		c_SecBuyRebornCost = 5;
const INT32		c_ThrBuyRebornCost = 9;
const INT32		c_VipBuyRebornCost = 3;
const INT32		c_MinusRebornTimes = -1;
/******************************************************/ 
//击杀，连杀，助攻相关
const INT32		c_MaxConKillBetween = 10 * 1000;
const INT32		c_MaxAssKillBetween = 10 * 1000;
const INT32		c_MaxValidBetween = 6 * 1000;
const INT32		c_MaxContinueKills = 3;

const INT32		c_n32BaseExp = 50;
const INT32		c_n32CoefficientExp = 22;
const double		c_fLvPowerExp = 1.1;

const INT32		c_n32BaseCP = 240;
const INT32		c_n32CoefficientCP = 6;
const INT32		c_n32RangeOneBaseCP = 80;
const INT32		c_n32RangeOneCoefCP = 14;
const INT32		c_n32RangeTwoBaseCP = 30;
const INT32		c_n32RangeTwoCoefCP = 15;

const INT32		c_FirstBloodReward = 210;
const INT32		c_n32EndKillingSpreeReward = 100;
const INT32		c_n32EndDomanatingReward = 200;
const INT32		c_n32EndMegaKillReward = 300;
const INT32		c_n32EndUnstoppableReward = 400;
const INT32		c_n32EndWhickedSickReward = 500;
const INT32		c_n32EndMonsterKillReward = 600;
const INT32		c_n32EndGodLikeReward = 700;
const INT32		c_n32EndHolyShitReward = 800;

/******************************************************/ 
//击杀英雄相关

/******************************************************/ 
//战斗结算相关
//Exp
const INT32		c_ExpTimelineBegin = 20;
const INT32		c_ExpTimelineEnd = 40;
const INT32		c_ExpTimeFixed = 20;

const float		c_fWELvPower = 1.3;
const INT32		c_WETimeCoef = 2;
const INT32		c_WELvCoef = 2;
const INT32		c_WEBase = 125;

const float		c_fLELvPower = 1.1;
const INT32		c_LETimeCoef = 1;
const INT32		c_LELvCoef = 1;
const INT32		c_LEBase = 100;
//gold
const INT32		c_GTimelineBegin = 10;
const INT32		c_GTimelineMid = 20;
const INT32		c_GTimelineEnd = 55;
const INT32		c_GTimeFixed = 20;

const INT32		c_FWGTimeCoefOne = 20;
const INT32		c_FWGBase = 200;
const INT32		c_FWGTimeCoefTwo = 2;

const INT32		c_WGTimeCoef = 10;
const INT32		c_WGBase = 100;

const INT32		c_LGTimeCoef = 3;
const INT32		c_LGBase = 30;
/************************************************/
//buy reborn
const INT64		c_n64BuyRebornPrice = 0;
const INT64		c_n64vipBuyRebornCost = 0;
const INT16		c_n16MinusRebornTimes = -1;

/************************************************************************/
const INT64		c_MaxHeroClientMatch = 101000;

/******************************************************/ 
//日志间隔符号
#define gSign "$$"
#define gSignTab "\t"
#define gSignPoint ","
#define gSignSemIc ":"
#define gSignPartSemIc ";"
#define gSignSplitIc "|"
 
/******************************************************/ 

enum EConstEnum{
	eObjLevel1Inter = 10000,
	eObjLevel2Inter = 1000,
	eObjLevel3Inter = 100,
};

enum ERunePosType{
	eRunePosType_Bag = 0,
	eRunePosType_Slot
};

enum EServerNetState{
	eSNS_Closed = 0,
	eSNS_Connecting,
	eSNS_Free,
	eSNS_Busy,
	eSNS_Full,
};

enum ERunesPosType{
	eRunesPosType_Bag,
	eRunesPosType_Slot,
	eRunesPosType_Invalid,
};

enum	EFBAllErrorCode{
	//Server model error code.
	eEC_Begin = -0x00010000 * 2,
	eEC_InvalidModelStatus, //非法的模块状态
	eEC_InvalidModelID, //非法的模块ID
	eEC_InvalidGSID, //非法的GSID
	eEC_InvalidSSID, //非法的SSID
	eEC_NullDataSource, //找不到数据源
	eEC_NullObjMgr, //找不到ObjMgr
	eEC_NullDataMgr, //找不到DataMgr
	eEC_NullWorldMgr, //找不到WorldMgr
	eEC_NullSceneServer, //找不到SceneServer
	eEC_NullGateServer, //找不到GateServer
	eEC_NullCentralServer, //找不到CentralServer
	eEC_InvalidUserPwd, //非法的用户口令
	eEC_InvalidPosition, //非法的位置
	eEC_InvalidRegionID, //非法的区域ID
	eEC_InvalidAreaID, //非法的区域ID
	eEC_NetSessionCollide, //网络会话冲突
	eEC_NullUser, //找不到用户
	eEC_UserExist, //用户已经存在
	eEC_GameObjectAlreadyExist, //游戏对象已经存在
	eEC_NullGameUnit, //找不到游戏单元
	eEC_LoadLibarayFail, //加载模块失败
	eEC_OpenCfgFileFail, //找开配置文件失败
	eEC_NoneNetListener, //没有网络监听器
	eEC_NoneNetConnector, //没有网络连接器
	eEC_OpenConnectorFail, //打开网络连接器失败
	eEC_TooManyConnectorOpened, //打开了太多的连接
	eEC_InitModelTimeOut, //初始化模块失败
	eEC_StartModelTimeOut, //启动模块超时
	eEC_NullMasterUser, //找不到主用户
	eEC_InvalidAttackTarget, //非法的技能目标
	eEC_NetConnectionClosed, //网络连接已经关闭
	eEC_NetProtocalDataError, //网络协议数据错误
	eEC_TooManySSNum, //SS数量过多
	eEC_TooManyGSNum, //GS数量过多
	eEC_InvalidNetState, //非法的网络状态
	eEC_JustInBattle, //当前正在战役中
	eEC_InvalidBattlePos, //非法的战役座位
	eEC_TheBattleUserFull, //战役用户已满
	eEC_UserDonotInTheBattle, //用户不在战役之中
	eEC_JustNotInBattle, //当前不在战役之中
	eEC_InvalidBattleState, //非法的战役状态
	eEC_InvalidBattleID, //非法的战役ID
	eEC_HeroExist, //英雄已经存在
	eEC_InvalidUserNetInfo, //非法的用户网络信息
	eEC_BattleExist, //战役已经存在
	eEC_BattleDonotExist, //战役不存在
	eEC_MapLoaded, //地图已经加载
	eEC_BattleLocalObjIdxOverflow, //战役本地对象溢出
	eEC_CannotFindoutTheObject, //找不到对象
	eEC_NullHero, //找不到英雄
	eEC_NotAllUserReady, //不是所有用户已经准备好
	eEC_YouAreNotBattleManager, //你不是战役管理员
	eEC_InvalidOrderCate, //非法的Order类别
	eEC_InvalidGameObjectCamp, //非法的游戏对象阵营
	eEC_CreateOrderFail, //创建Order失败
	eEC_InvalidOrderState, //非法的Order状态
	eEC_OrderNoMasterGO, //Order没有主对象
	eEC_AbsentOrderPriority, //Order优先级不够
	eEC_NullBattle, //找不到战役
	eEC_MoveBlocked, //移动被阻挡
	eEC_ReadCfgFileFail, //读取配置文件失败
	eEC_TooManyNPCCfgNum, //NPC配置数量太多
	eEC_TooManyHeroCfgNum, //英雄配置数量太多
	eEc_TooManMapDataCfgNum, //地图配置数量太多
	eEC_InvalidHeroSeat, //非法的英雄座位
	eEC_InvalidMapID, //非法的地图ID
	eEc_InvalidPos, //非法的位置
	eEC_InvalidCellID, //非法的单元格ID
	eEC_NullArea, //非法的区域
	eEC_InvalidScriptParameters, //非法的脚本参数
	eEC_InvalidObjTypeID, //非法的对象类型ID
	eEC_InvalidPathNodeNum, //非法的路径节点数量
	eEC_InvalidVector3D, //非法的3D向量
	eEC_UserInfoUnComplete, //用户信息不全
	eEC_GenerateGUIDFail, //创建GUID失败
	eEC_NotEnemy, //没有敌人
	eEC_NotNormalAttackSkill, //没有普通攻击技能
	eEC_CanNotFindTheSkill, //找不到指定技能ID
	eEC_NotOrderInfo, //没有Order信息
	eEC_InvalidSkillID, //非法的技能ID
	eEC_InvalidSkillState, //非法的技能状态
	eEC_InvalidSkillTarget, //非法的技能目标
	eEC_NullSkillData, //没有技能数据
	eEC_InvalidGameObjectCate, //非法的游戏对象类型
	eEC_HasChoosedHero, //已经选择了英雄
	eEC_OthersHasChoosedHero, //
	eEC_HasNoTheHero,
	eEC_AbsentAttackDistance, //攻击距离不足
	eEC_TargetIsDead, //目标已经死亡
	eEC_InvalidTargetActionState, //目标行为状态非法
	eEC_InvalidBattle, //非法的战役
	eEC_TheBattlIsFull,	 //战役已经满员
	eEC_BattlePDWNotMatch, 
	eEC_NullGameObject,//找不到游戏对象
	eEC_NULLNickName, //找不到昵称
	eEC_TimeExpire, //时间超时
	eEC_NoneAvailbleBuff, //没有有效BUFF
	eEC_BuffOverlapUpperLimit, //超过BUFF上限
	eEC_InvalidCurHPNum, //非法的当前HP
	eEC_InvalidCurMPNum, //非法的当前MP
	eEc_NoPathNode, //找不到路径节点
	eEC_InvalidBuffTypeID, //非法的缓冲类型ID
	eEC_AbsentSkillDistance, //技能距离不足
	eEC_BuildingCanNotMove, //建筑不能移动
	eEC_AbsentCP, //CP不足
	eEC_AbsentMP, //MP不足
	eEC_AbsentHP, //HP不足
	eEC_BuildingCanNotAddBuff, //建筑不能添加BUFF
	eEC_InvalidActionState, //非法的行为状态
	eEC_JustSkillAction,  //正在使用技能
	eEC_AbsorbMonsterFailForLackLevel, 
	eEC_AbsorbMonsterFailForHasFullAbsorb,
	eEC_AbsorbMonsterFailForHasSameSkillID,
	eEC_AbsorbMonsterFailForLackCP,
	eEC_AbsorbMonsterFailForMonsterDead,
	eEC_AbsorbMonsterFailForMonsterCannotBeConstrol,
	eEC_AbsorbMonsterFailForHeroDead,
	eEC_AbsorbMonsterFailForNotMonster,
	eEC_AbsorbMonsterFailForDiffNPC,
	eEC_AbsorbMonsterFailForErrorState,
	eEC_AbsorbMonsterFailForDizziness,
	eEC_NULLNPC, //找不到NPC
	eEC_NULLCfg, //找不到配置信息
	eEC_InvaildSkillID, //非法的技能ID
	eEC_RemoveAbsorbSkillFailed, //移除吸附技能失败
	eEC_StateCanNotUseGas,
	eEc_ExistWildMonsterBornPos, 
	eEc_InvalidControlNPCType,
	eEC_GasExplosionNotFull,
	eEC_MultiAbsortNotAllowed,
	eEC_StaticBlock, //静态阻挡
	eEC_DynamicBlock, //动态阻挡
	eEC_NoDistanceToMove, //没有距离需要移动
	eEC_CannotFindFullPathNode, //找不到完整的路径
	eEC_HeroNotDead, //英雄没有死亡
	eEC_NotEnoughGold, //金币不足
	eEC_NoRebornTimes,
	eEC_BattleIsPlaying,
	eEC_RemoveBuffFailed,	
	eEc_DeadAltar,
	eEC_InvaildCampID,
	eEC_NotInSameBattle,
	eEC_AskBuyRunesFail,
	eEC_AskComposeRunesFail,
	eEC_AskUnUseRunesFail,
	eEC_AskUseRunesFail,
	eEC_AskMoveGoodsFail,
	eEC_AskSellGoodsFail,
	eEC_InvaildGridID,
	eEC_AskUseGoodsFailForCoolDown,
	eEC_rInvalidGoodsNum,
	eEC_AskBuyGoodsFailForLackCP,
	eEC_AskBuyGoodsFailForInvalidCPType,
	eEC_AskBuyGoodsFailForHasSameTypeID,
	eEC_AskBuyGoodsFailForHasFunType,
	eEC_AskBuyGoodsFailForBagFull,
	eEC_CannotCreateVoipServer,
	eEc_AttackOneObj,
	eEc_ExistObj,
	eEc_TheSkillEnd,
	eEc_ErrorSkillId,
	eEc_InvalidMastType,
	eEC_NickNameCollision, //昵称冲突
	eEC_ObjectAlreadyExist,
	eEC_ForbitAbWMSolder, //不能吸附野怪兵
	eEC_TargetCannotLooked,//目标不能锁定
	eEC_ErrorAreaId,//错误的areaId
	eEc_NoWatchUser,//没有观察者
	eEc_MaxBornSolder,
	eEC_AddBattleFailForLackOfGold,
	eEC_CampNotBalance,
	eEC_AskBuyGoodsFailForSole,

	eEC_TimeOut,
	eEC_AddEffectFailed,
	eEC_EffectEnd,

	eEC_UseSkillFailForSilenced,
	eEC_UseSkillFailForDisarmed,
	eEC_UseSkillFailForLackHP,
	eEC_UseSkillFailForLackMP,
	eEC_UseSkillFailForLackCP,
	eEC_UseSkillFailForSkillCoolDown,
	eEC_UseSkillFailForNULLTarget,
	eEC_UseSkillFailForBuildingNullity,

	eEC_UseGoodsFailForDizziness,
	eEC_UseGoodsFailForBuildingNullity,
	eEC_UseGoodsFailForNULLTarget, 
	eEC_UseGoodslFailForSilenced,
	eEC_UseGoodslFailForErrorCamp,
	eEC_UseSkillGasNotInRunState,
	eEC_UseSkillGasHasInRunState,
	eEC_NoAbsorbSkill,

	eEC_UseSkillFailForDizziness,
	eEC_AskBuyGoodsFailForLackTeamCP,
	eEC_CanntAbsorb,
	eEC_SkillPrepareFailed,
	eEC_CancelSkillOrderFailed,

	eEC_TheBattleObserverNotFull,
	eEC_TheBattleObserverFull,
	eEC_BeginBattleFailForNullPlayer,
	eEC_AddBattleFailForAllFull,
	eEC_AddBattleFailForUserFull,
	eEC_WarningToSelectHero,
	eEC_GuideNotOn,
	eEC_HasCompGuideStep,
	eEc_InvalidStepId,
	eEc_DelAbsorbICOFailed,
	eEC_AbsorbMonsterFail,
	eEC_ZeroGUID,
	eEc_NoObjList,

	eEC_JustInThatSeatPos,
	eEC_NickNameNotAllowed,
	eEc_InvalidTarget,	//错误的对象:箭塔
	eEC_GUDead,
	eEC_TooManyUserInBattle,
	eEc_NoAtkObj,
	eEC_InvalidMapInfo,
	eEc_InvalidMapId,
	eEc_NullLuaCfg,
	eEC_NullMapCfg,
	eEC_LoadFilterCfgFailed,
	eEC_RemoveEffectFailed,
	eEC_UserNotExist,
	eEC_BattleFinished,

	eEC_UserWasInFriendList,
	eEC_UserWasInBlackList,
	eEC_UserNotOnline,
	eEC_MsgTooLarge,
	eEC_UserRefuseReceiveYourMsg,
	eEC_UserOfflineFull,

	eEC_HaveBuySameGoods,
	eEC_BuyGoodsFailedLackGold,
	eEC_BuyGoodsFailedLackDiamond,
	eEc_ExistGuidCfg,
	eEC_NullInfo,
	eEC_UserRSExist,
	eEc_ErrorGuideStepId,
	eEc_invalidObjId,
	eEC_NothingContent,
	eEc_DoneDBWrong,
	eEC_AskTooFrequently,
	eEC_UserOfflineMsgFull,
	eEc_InvalidAbsorbTar,
	eEC_BeginAIFailed,
	eEC_CanNotUseChinese,

	eEC_TipsObjAppear,
	eEC_TipsNPCBorn,
	eEC_TipsSuperNPCBorn,
	eEc_ErrorType,
	eEC_FriendsListFull,
	eEC_BlackListFull,
	eEC_JustInFriendsList,
	eEC_JustInBlackList,
	eEC_NullUserRSInfo,
	eEC_UserBusy,
	eEC_YouInOppositeBlackList,
	eEC_CounterpartFriendListFull,
	eEC_UserInYourBlackList,
	eEC_AskHaveSend,
	eEc_existBattle,
 	eEC_CannotBuygoodsWhenDeath,
	eEc_TimeToSaveDB,

	eEc_91LoginFail,
	eEc_91InvalidAppID,
	eEc_91InvalidAct,
	eEc_91InvalidPara,
	eEc_91InvalidSign,
	eEc_91InvalidSessionID,

	eEc_UserNotHaveHero,
	eEC_CannotSellgoodsWhenDeath,
	eEC_PPUserNameRuleWrong,
	eEC_PPUserNotExist,
	eEC_PPInvalidAct,
	eEC_PPUserExisted,
	eEC_PPPwdCheckError,
	eEC_PPUserProhibited,
	eEC_PPDataError,
	eEC_PPSessionTimeout,
	eEC_PPUserHaveBinding,

	eEC_TBInvalidToken,
	eEC_TBInvalidFormat,

	eEc_FunClosed,
	eEC_BattleClosing,	//用于重连
	eEC_InvalidPwdLength,
	eEC_PleaseEnterPwd,
	eEC_InvalidUserNameLegth,

	eEC_NullPointer,
	eEC_InvalidMsgProtocalID,
	eEC_NullMsgHandler,
	eEc_InvalidGUID,
	eEC_NullMsg,
	eEC_InvalidNSID,
	eEC_GUIDCollision,
	eEC_InvalidUserName,
	eEc_InvalidMailId,
	eEC_UserOfflineOrNullUser,
	eEC_UserWasPlaying,
	eEC_RequestSended,
	eEC_AddFriendSeccuse,
	eEC_OppositeSideFriendFull,
	eEC_ReEnterRoomFail,//重进房间失败
	eEC_DiamondNotEnough,
	eEC_ParseProtoError,//解析PB错误
	eEC_UnKnownError,//未知错误//
	eEC_ErrorTimes,
	eEC_MatchLinkInvalid,//匹配链接已失效//
	eEC_AddMatchTeamError,//加入匹配队伍失败
	eEC_NotEnoughItem,		//item number not enough
	eEC_DidNotHaveThisItem,    
	eEC_UserRefuseAddFriends,
	eEC_MatchTeamateStoped,//等待匹配队友
	eEc_ExistGuideTaskId,	//存在的任务id
	eEC_UnknowPlatform,
	eEC_MsgAnalysisFail,
	eEC_PostLoginMsgFail,
	eEC_NickNameTooShort,
	eEC_GuideUserForbit,
	eEC_InvalidCDKey,
	eEC_WashRuneFail,
	eEC_GetCDKeyGiftSuccess,

	eEc_MailHasTimeOver, //邮件邮件过期
	eEc_MailHasRecv, //邮件已经领取 
	eEC_HavedPerpetualHero,
	eEC_InvalidPara,
};

enum eGoodsState{
	eGoodsState_Free,
	eGoodsState_Pending
};

struct SIPAddr 
{
	CHAR	aszIPAddr[16];
	SIPAddr() { memset(aszIPAddr,0,16); }
};

enum	EUserPlayingStatus
{
	eUserPlayingStatus_OffLine = 0,
	eUserPlayingStatus_Playing,
};

enum EPayType{
	ePayType_None = 0,
	ePayType_Gold = 1,
	ePayType_Diamond = 2,
};

enum EFBAllProtocalSpace
{
	eFBAllProtocalSpace_GameClient = 0,
	eFBAllProtocalSpace_CentralServer,
	eFBAllProtocalSpace_SceneServer,
	eFBAllProtocalSpace_GateServer,
	eFBAllProtocalSpace_BalanceServer,
	eFBAllProtocalSpace_LoginServer,
};

enum EBattleState
{
	eBS_Free = 0,
	eBS_Waiting,
	eBS_SelectHero,
	eBS_ShowHero,
	eBS_Loading,//Not in SS
	eBS_Playing,//Start in SS
	eBS_ShowResult,//GC had、SS no、CS had but do nothing.
	eBS_Finished,//All had.
	eBS_GuideFinished,//Only SS had.
	eBS_End,//Only a end flag.
};

enum	ECSModelID
{
	eCSModelID_Kernel = 0,
	eCSModelID_UserMgr,
	eCSModelID_BattleMgr,
	//eCSModelID_GameEntityMgr,
	//eCSModelID_AIMgr,
	eCSModelID_End,
};

enum	ESSModelID
{
	eSSModelID_Kernel = 0,
	eSSModelID_UserMgr,
	eSSModelID_BattleMgr,
	//eSSModelID_WorldMgr,
	//eSSModelID_ObjMgr,
	eSSModelID_End,
};

enum	EGSModelID
{
	eGSModelID_Kernel = 0,
	eGSModelID_End,
};

enum EUserPlatform{
	//ios
	ePlatform_PC = 0,
	ePlatformiOS_91 = 1,
	ePlatformiOS_TB = 2,
	ePlatformiOS_PP = 3,
	ePlatformiOS_CMGE = 4,
	ePlatformiOS_UC = 5,
	ePlatformiOS_iTools = 6,
	ePlatformiOS_OnlineGame = 7,
	ePlatformiOS_As = 8,
	ePlatformiOS_XY = 9,
	ePlatformiOS_CMGE_ZB =  10,
	

	//android
	ePlatformAndroid_CMGE = 104,
	ePlatformAndroid_UC = 105,

	//其他
	ePlatformiOS_CMGEInfo = 304,
	//RC use
	ePlatform_All = 1000,
};

enum ServerState
{ 
	Fluent = 0,//流畅
	Busy = 1,//繁mang
	HouseFull = 2,//爆
};


enum EClientOS
{
	eCOS_IOS,
	eCOS_Android,
};

enum EUserLoginWay
{
	eULW_None = 0,
	eULW_QuickLogin,
};

enum EPreDefine_ErrorCode
{ 
	eEC_UseSkillTargetWrongType = -14,//使用技能错误的类型
	eEC_UseSkillTargetWrongCamp = -13,//使用技能错误的阵营
	eEC_UseSkillTargetIsDead = -12,//使用技能目标已经死亡
	eEC_OverTime = -4,
	eEC_InsertFail  = -3,
	eEC_ConstInvalid = -2,
	eEC_OperatePending = -1,
	eNormal = 0,
};

enum ERunesConsumeType
{
	eRunesConsumeType_Diamand = 1,
	eRunesConsumeType_Gold,

	eRunesConsumeType_None
};
enum EUserCate
{
	eUserCate_Admin = 0,
	eUserCate_GM,
	eUserCate_Robot,
	eUserCate_DiamondVIP,
	eUserCate_GloldVIP,
	eUserCate_SilverVIP,
	eUserCate_CopperVIP,
	eUserCate_Normal,
};

enum EAttendantCate{
	eAC_None = 0,
	eAC_AddHP,
	eAC_AddMP,
	eAC_AddAttack,
	eAC_AddDefense,
};

enum EMagicCate{
	eMC_Physical,
	eMC_Water,
	eMC_Fire,
	eMC_Grass,
	eMC_Thunder,
};

enum EAICate{
	eAICate_None = 0,
	eAICate_Occupate,
	eAICate_Guard,
	eAICate_Patrol,
	eAICate_End,
};

enum EAttackMode{
	eAM_Passive = 0,
	eAM_Active,
};

enum ERace{
	eRace_APE,
	eRace_Primeval,
	eRace_Amphibian,
	eRace_Ghost,
};

enum ERunesOP{
	eRunesOP_BUY,
	eRunesOP_USE,
	eRunesOP_UNUSE,
	eRunesOP_Compose
};

enum	EObjectType
{
	eObjectType_None = 0,
	eObjectType_Guild,
	eObjectType_User,
	eObjectType_HeroBegin = c_un32ObjTypeSpace,
	eObjectType_NPCBegin = c_un32ObjTypeSpace * 2,
	eObjectType_GoodsBegin = c_un32ObjTypeSpace * 3, 
	eObjectType_AiRobotBegin = c_un32ObjTypeSpace * 4,
};

enum EAttackTar
{
	eAttackTar_None = 0,
	eAttackTar_ToSky = 1,
	eAttackTar_ToWaterSurface = 2,
	eAttackTar_ToUnderWater = 4,
};

enum EEquipTarget
{
	eEquipTarget_Building = 1,
	eEquipTarget_Ship,
	eEquipTarget_Both,
};

enum	EDiamondChangeReason
{
	eDiamondChangeReason_None = 0,
	eDiamondChangeReason_SaleObject,
	eDiamondChangeReason_GMInstruction,
	eDiamondChangeReason_LoginReward = 4,
};

enum	EGoldChangeReason{
	eGoldChangeReason_None = 0,
};

enum	EUserDBDataVer
{
	eUserDBDataVer_Begin = 0,
	eUserDBDataVer_20140224,
	eUserDBDataVer_End,
};

enum	ECityDBDataVer
{
	eCityDBDataVer_Begin = 0,
	eCityDBDataVer_End,
};

enum	EBuildingDBDataVer
{
	eBuildingDBDataVer_Begin = 0,
	eBuildingDBDataVer_End,
};

enum ESex
{
	eSex_Invalid,
	eSex_Male,
	eSex_Female
};
enum	EShipDBDataVer
{
	eShipDBDataVer_Begin = 0,
	eShipDBDataVer_End,
};

enum	EPlaneDBDataVer
{
	ePlaneDBDataVer_Begin = 0,
	ePlaneDBDataVer_End,
};

enum EMissionType
{
	eMissionType_Attack = 0,

	eMissionType_End,
};


enum	EPlatformQueryID{
	ePlatformQueryID_None = 0,
	ePlatformQueryID_ValidateUser,
};

enum	ESkillUseWay{
	eSUW_None = 0,
	eSUW_Active,
	eSUW_Passive,
	eSUW_TriggerWhileAttack,
	eSUW_TriggerWhileBeAttack,
	eSUW_TriggerWhileBeHurt,
	eSUW_TriggerWhileDie,
	eSUW_TriggerWhileUse,
	eSUW_TriggerWhileImpact,
};

enum ESkillTargetCate{
	eSTC_None = 0,
	eSTC_Self,//自己
	eSTC_TeamMember,//队员包含自己
	eSTC_TeamMember_Without_Self,//队员不包含自己
	eSTC_Enemy,//敌人
	eSTC_AllObject,//所有单位
	eSTC_AllFriendUnit,//所有友方单位
};

enum ESkillModelTargetCate{
	eSMTC_None = 0,
	eSMTC_Self,//自己
	eSMTC_Enemy,//敌人
	eSMTC_AllObject,//所有单位
};

enum ESkillRange{
	eSkilLRange_None = 0,
	eSkilLRange_Single,
	eSkilLRange_Circle,
	eSkilLRange_Rectangle,
};

//刘路修改技能系统预备删除
/*
enum ESkillType{
		eSkillType_None = 0,
		eSkillType_ShortDistance,
		eSkillType_LongDistance,
		eSkillType_MonomerNotForceFly,
		eSkillType_PierceNotForceFly,
		eSkillType_CommonRange,
		eSkillType_FollowRange,
		eSkillType_ReBounce,
		eSkillType_NoneBounce,
		eSkillType_FlyRange,
		eSkillType_ExchangPosition,
		eSkillType_CreateFixedNPC,
		eSkillType_CreateFollowNPC,
		eSkillType_FixRange,
};
*/

enum ESkillAOECate{
	eSAOECate_None,
	eSAOECate_SelfCenter,
	eSAOECate_TargetCenter,
	eSAOECate_FixDist_Ex,//以固定距离为坐标点
};

enum EEffectCate{
	eEffectCate_None = 0,
	eEffectCate_PhyHurt,//物理伤害
	eEffectCate_MagicHurt,//魔法伤害
	eEffectCate_CurHP,//当前HP
	eEffectCate_CurMP,//当前MP
	eEffectCate_PhyAttack,//物攻
	eEffectCate_MagicAttack,//魔攻
	eEffectCate_PhyDefense,//物抗
	eEffectCate_MagicDefense,//魔抗
	eEffectCate_MoveSpeed,//移动速度
	eEffectCate_AttackSpeed,//攻速
	eEffectCate_MaxHP,//最大MP
	eEffectCate_MaxMP,//最大HP
	eEffectCate_HPRecoverRate,//HP回复
	eEffectCate_MPRecoverRate,//MP回复
	eEffectCate_ReliveTime,//复活时间	
	eEffectCate_Dizziness,//昏迷
	eEffectCate_Silence,//沉默
	eEffectCate_PosMove,//未使用
	eEffectCate_Rebound,//未使用
	eEffectCate_PhySuckBlood,//物理吸血
	eEffectCate_Disarm,//缴械
	eEffectCate_Restrain,//束缚
	eEffectCate_CriPersent,//暴击概率
	eEffectCate_CriHarm,//暴击伤害加成
	eEffectCate_CPRecover,//金钱回复
	eEffectCAte_PassitiveSkill,//被动
	eEffectCate_Invisible,//隐身
	eEffectCAte_AttackDist,//射程
	eEffectCate_TrueHurt,//真实伤害
	eEffectCate_PhyPassNum,//物理穿透数值
	eEffectCate_PhyPassPercent,//物理穿透比例
	eEffectCate_MagicPassNum,//魔法穿透数值
	eEffectCate_MagicPassPercent,//魔法穿透比例
	eEffectCate_CooldownReduce,//冷却缩减
	eEffectCate_PhyDmgReduce,//物理减伤千分比
	eEffectCate_MagicDmgReduce,//魔法减伤千分比
	eEffectCate_TrueDmgReduce,//真实减伤千分比
	eEffectCate_PhyDmgReduceNum,//物理减伤数值
	eEffectCate_MagicDmgReduceNum,//魔法减伤数值
	eEffectCate_TrueDmgReduceNum,//真实减伤数值
	eEffectCate_MagicSuckBlood,//法术吸血
	eEffectCate_End,//结束分隔符
};

enum EAbsorbDataState{
	eEAbsorbDataState_Invalid = 0,
	eEAbsorbDataState_Paparing ,
	eEAbsorbDataState_Prepared,
};

enum EEffectTar{
	eSET_SkillTar = 0,
	eSET_Releaser,
	eSET_Friends,
	eSET_Enemy,
};

enum ESkillBuffTarCate{
	eSBTC_None = 0,
	eSBTC_Releaser,
	eSBTC_SkillTar,
};

enum ESkillState{
	eSkillState_Free,
	eSkillState_Preparing,
	eSkillState_Releasing,
	eSkillState_Using,
	eSkillState_Lasting,
	eSkillState_End,
};

enum ESkillEffectState{
	eSkillModelState_Free,
	eSkillModelState_Preparing,
	eSkillModelState_Releasing,
	eSkillModelState_End,
};

enum ENPCCateChild{
	eNPCChild_None = 0,
	eNPCChild_NPC_Per_AtkBuilding,
	eNPCChild_NPC_Per_Bomb,
	eNPCChild_NPC_Per_Small,
	eNPCChild_NPC_Per_Big,

	eNPCChild_BUILD_Altar = 10, 
	eNPCChild_BUILD_Base,

	eNPCChild_Summon = 20, //召唤物 
	eNPCChild_Summon_End = 30,//召唤物结束

	eNPCCateChild_Ohter,
};

enum ENPCSolderChild{
	eNPCSChild_None = 0,
	eNPCSChild_AltarSolder,
	eNPCSChild_BuldSolder,
	eNPCSChild_BombSolder,
};

enum ENPCCate{
	eNPCCate_WideAnimal = 1,//野怪
	eNPCCate_Solider,//士兵
	eNPCCate_Building,//攻击塔
	//eNPCCate_Altar,
};


enum	EDBType
{
	eDBType_None = 0,
	eDB_GameDb, 
	eDB_CdkeyDb, 
	eDB_LogDb,
	eDB_MailDb,
};

/*
enum EHPChangeReason{
	eHPCR_NormalHurt,
	eHPCR_BlastHurt,
	eHPCR_BuffEffect,
	eHPCR_Recover,
	eHPCR_SkillConsume,
	eHPCR_SkillHurt,
	
	eHPCR_SkillAbsorbHurt,
	eHPCR_SysteHurt,
};


enum EMPChangeReason{
	eMPCR_SkillConsume,
	eMPCR_BuffEffect,
	eMPCR_Recover,
};
*/

enum ESkillCountdownCate{
	eSCC_Immediately = 1,
	eSCC_Delay,
	eSCC_Script,
};

enum EHateLevel{
	eHateLevel_First,
	eHateLevel_Second,
	eHateLevel_Third,
	eHateLevel_Fourth,
	eHateLevel_Fifth,
	eHateLevel_Sixth,
	eHateLevel_Seventh,
	eHateLevel_Eighth,
	eHateLevel_End,
};

enum EExpChangeReason{
	eECR_KillEnemy,
	eECR_HelpKillEnemy,
	eECR_SystemAward,
};

enum EWMLevel{
	eEWM_None = 0,
	eEWM_low,
	eEWM_Middle,
	eEWM_High,
};

enum EWMGroup{
	eEWMGroup_none = 0,
	eEWMGroup_1,
	eEWMGroup_2,
	eEWMGroup_3,
	eEWMGroup_4,

	eEWMGroup_other,
};


enum ELoginRewardItemType
{
	eLoginRewadItem_None = 0,//未设
	eLoginRewadItem_Gold = 1,//金币
	eLoginRewadItem_Diamond = 2,//钻石
	eLoginRewadItem_GoodsIDBegin = 3,//商品
};

 enum ERewardType
 {
	 eRewardType_None = 0,		//未设
	 eRewardType_Nomal = 1,		//普通
	 eRewardType_Extend = 2,	//额外
 };

enum ESkillSlotIdx{	
	eSkillSlotIdx_NotNormalSkill1 = -1,
	eSkillSlotIdx_NormalSkill1,
	eSkillSlotIdx_NormalSkill2,
	eSkillSlotIdx_GasSkill1,
	eSkillSlotIdx_GasSkill2,
	eSkillSlotIdx_NormalAttack,
	eSkillSlotIdx_SuiteSkill1,
	eSkillSlotIdx_SuiteSkill2,
};

enum	EIntDir{
	eIntDir_Invalid = -1,
	eIntDir_Up = 0,
	eIntDir_UpLeft,
	eIntDir_Left,
	eIntDir_BelowLeft,
	eIntDir_Below,
	eIntDir_BelowRight,
	eIntDir_Right,
	eIntDir_UpRight,
	eIntDir_End,
};


enum ELuaParamType
{
	eELua_Param_Number = 0,
	eELua_Param_String,
	eELua_Param_Struct
};

//set wild monster be atk type
enum EVMBeAtkType
{
	eEVMBeAtk_Hp_Hurt = 0,
	eEVMBeAtk_Lost_Enemy,
	eEVMBeAtk_Get_State,
};

//set the hero atk list
enum EHeroBeAtkType
{
	eEHeroBeAtk_Add =0,
	eEHeroBeAtk_Del,
	eEHeroBeAtk_IfBeAtk,
	eEHeroBeAtk_GetGuid,
};

//堆叠类型
enum EReplaceType
{
	EReplaceType_None = 0,
	EReplaceType_Forbit= 1,//不允许堆叠
	EReplaceType_Reset,// 堆叠，并刷新持续的时间
	EReplaceType_SingleCaculate,   //堆叠，各自计算持续时间
};

//形状
enum ESkillShapeType
{
	eSkillShapeType_None = 0,
	eSkillShapeType_Cicle,
	eSkillShapeType_Rectangle, //矩形
	eSkillShapeType_Sector,//扇形
};

//召唤类型
enum ESummonType
{
	eSkillSummonType_None = 0,
	eSkillSummonType_Building,
	eSkillSummonType_Retinue,//随从
	eSkillSummonType_Monster,//野怪
};
//召唤方式
enum ESummonWayType
{
	eSummonType_None = 0,
	eSummonType_DistDir,//施法距离 朝向
	eSummonType_Tarpos, //目标的召唤 
	 
};

enum EHeroCPAward
{
	eHeroCPAward_None = 0,
	eHeroCPAward_Self,
	eHeroCPAward_Other,
};

enum EHeroLoginState
{
	eHero_None = 0,
	eHero_New,
	eHero_Old,

	eHero_Own,
	 
};

enum EObjInfoType
{
	eObjType_None = 0,

	eObjType_HeroInfo,
	eObjType_HeroLv,  
};

enum ESystemFunc
{
	eSysFunc_None = 0,
	eSysFunc_Normal,
	eSysFunc_OB,
	eSysFunc_Guide,
};
//OB
enum EOBSystemFunc
{
	eOB_None = 0, 
	eOB_InitUserSeat,
	eOB_AddUser,
	eOB_OBBattleHeroInfo, 
	eOB_ObjBaseInfo, 
	eOB_BattleInfo,
	eOB_HerosCount, //比赛统计信息
	eOB_Post,
};


//新手引导
enum EKillReason
{
	eKillReason_NormalAtk = 0,
	eKillReason_SkillAtk,
	eKillReason_SummonSkillAtk,//召唤
	eKillReason_Building,
};
enum EDeadType
{
	eType_User = 0,
	eType_NPC,
	eType_AltarNPC,
	eType_WildNPC,
	eType_Tower,
	eType_AltarTower,
	eType_BaseBuild,
};
struct  GuideCaseDeadInfo
{
	INT32						n32NPCId;
	EDeadType					eNPCType; 
	EKillReason					eCaseDeadReson;

	GuideCaseDeadInfo(){ Release(); }

	void Release()
	{
		n32NPCId = 0;
		eNPCType = eType_User;
		eCaseDeadReson = eKillReason_NormalAtk;
	}
} ; 

enum EGuideType
{
	eGuideType_None = 0,
	eGuideType_GetHeroId,
	eGuideType_Comp,
	eGuideType_BronNPC,
	 eGuideType_ReBronNPCHero,
};

enum EGuideState
{
	eGuideState_Init = 0,
	eGuideState_Comp,
	eGuideState_LastEndStep,
	eGuideState_Finish,
	eGuideState_FinishState,
};

enum EAltarBornNpcType
{
	//eAltarBornNPCType_Close = 0,
	eAltarBornNPCType_Add = 0 ,
	//eAltarBornNPCType_Born,
	eAltarBornNPCType_Del = 1,
};

enum EBattleEndType
{
	eBattleEnd_Normal = 0,
	eBattleEnd_Guide = 1,
	eBattleEnd_UserAllOffline = 2,
	eBattleEnd_Guide_AskGoHall = 3,
};

enum ESummonFunc
{
	eSummonFunc_None = 0,
	eSummonFunc_All,
	eSummonFunc_One,
	eSummonFunc_Update,
	eSummonFunc_Call,
};

enum ESysSightType
{
	eSightType_SelfAndOB = 0,
	eSightType_All, 
	eSightType_OB,
	eSightType_State,

	eSightType_End,
};

enum ERelationShip{
	eRSType_None = 0,
	eRSType_Friends,
	eRSType_Detestation,
};

enum EDBOperation{
	eOperationType_None = 0,
	eOperationType_Add = 1,
	eOperationType_Del = 2,
	eOperationType_Upd = 3,
};
 

//cs 新手引导类型
enum ECSGuideType
{
	eCSGuideType_None = 0,
	eCSGuideType_1,//线性
	eCSGuideType_2,//触发
	eCSGuideType_3, //场景次数引导
};

struct NPCHeroCfg
{
	INT32  nStepId;
	INT32  nNPCId;
	INT32  nDelayTimeCfg;
	INT64  bornTime;

	NPCHeroCfg():nStepId(0),nDelayTimeCfg(0),bornTime(0){}

	NPCHeroCfg(INT32 ns,INT32 npcId, INT32 nd)
	{
		nStepId = ns;
		nNPCId = npcId;
		nDelayTimeCfg = nd;
		bornTime = 0;
	}
};


struct tagBornSolderCfg 
{
	INT32 n32BSpaceTimeCfg;
	INT32 n32MaxExistNumCfg;  
	INT32 n32CurtExistNum;
	INT32 n32CurtDeadNum; 

	tagBornSolderCfg():n32BSpaceTimeCfg(0)
		, n32MaxExistNumCfg(0)
		, n32CurtExistNum(0)
		, n32CurtDeadNum(0){}

	bool operator <(const tagBornSolderCfg& p){
		return n32BSpaceTimeCfg < p.n32BSpaceTimeCfg;
	}
	tagBornSolderCfg(INT32 st,  INT32 mb):n32BSpaceTimeCfg(st)
		, n32MaxExistNumCfg(mb)
		, n32CurtExistNum(0)
		, n32CurtDeadNum(0){
	}  
	void ChangeParam(bool b)
	{  
		b ? ++n32CurtExistNum : ++n32CurtDeadNum;  
	}
	void Release()
	{
		n32BSpaceTimeCfg = 0;
		n32MaxExistNumCfg = 0;
		n32CurtExistNum = 0;
		n32CurtDeadNum = 0;
	}
};

struct SWMRefrshCfg
{
	EWMLevel eWmLv;
	UINT32 un32BufId;
	UINT64 un32Cfg; 
	UINT64 un64LastMilsec;	 
	bool operator <(const SWMRefrshCfg& p){
		return eWmLv < p.eWmLv;
	}
	SWMRefrshCfg():eWmLv(eEWM_low)
		, un32BufId(0)
		, un32Cfg(0)
		, un64LastMilsec(0)
	{
	}
	void Release()
	{
		eWmLv = eEWM_low;
		un32BufId = 0;
		un32Cfg = 0;
		un64LastMilsec = 0;
	}
	SWMRefrshCfg(EWMLevel ev,UINT32 bufId, UINT32 cdcfg, UINT64 um)
		: eWmLv(ev)
		, un32BufId(bufId)
		, un32Cfg(cdcfg)
		, un64LastMilsec(um)
	{
	}
}; 

//新手引导未完成步骤，补足野怪
struct tagGuideVmBornCfg 
{
	INT32  stepId;
	INT32  cdTime;
	INT32  totalBornNum; 

	INT32  curtNum; 
	time_t		m_BornTime;  

	tagGuideVmBornCfg():stepId(0)
		, cdTime(0)
		, totalBornNum(0)
		, m_BornTime(0){}
};

//战斗：创建，删除
enum EBattleOPType
{
	eBattleOP_None = 0,
	eBattleOP_Create,
	eBattleOP_Finish,
};
 
//引导提示类型
enum ETipsType
{
	eTips_None = 0,
	eTips_ObjAppear,
	eTips_NPCBorn,
	eTips_SuperNPCBorn,
};
//场次引导
enum EGuideBattleType
{
	eGuideBattle_None = 0,
	eGuideBattle_Primary,
	eGuideBattle_Middle,
	eGuideBattle_Hight,
};


//邮件
enum EMailType
{
	eMailType_None = 0,
	eMailType_1,
	eMailType_2,
	eMailType_3,
	eMailType_4,
};

//可赠送的枚举(公用)
enum EMerchType
{
	eMerchType_None = 0,
	eMerchType_Gold,
	eMerchType_Diamond, 
	eMerchType_Goods,
	/*eMerchType_Runne,
	eMerchType_Hero,
	eMerchType_Skin,
	eMerchType_Goods,
	eMerchType_Exp, */
};

typedef UINT64 SGUID_KEY;

const float c_fPersentDevide = 1000.0f;

struct SFightProperty{
	INT32		n32PhyAttPower;
	INT32		n32MagicAttPower;
	INT32		n32PhyDefAbility;
	INT32		n32MagicDefAbility;
	INT32		n32MoveSpeed;
	INT32		n32AttackSpeed;
	INT32		n32AttackDist;
	INT32		n32MaxHP;
	INT32		n32MaxMP;
	INT32		n32HPRecover;
	INT32		n32MPRecover;
	INT32		n32ReliveSecond;
	INT32		n32CriPersent;
	INT32		n32CriHarm;
	INT32		n32CPRecover;
	INT32		n32DizzinessCounter;
	INT32		n32SilenceCounter;
	INT32		n32DisarmCounter;
	INT32		n32RestrainCounter;
	INT32		n32InvisibleCounter;

	SFightProperty():n32PhyAttPower(0)
		, n32MagicAttPower(0)
		, n32PhyDefAbility(0)
		, n32MagicDefAbility(0)
		, n32MoveSpeed(0)
		, n32AttackSpeed(0)
		, n32AttackDist(0)
		, n32MaxHP(0)
		, n32MaxMP(0)
		, n32HPRecover(0)
		, n32MPRecover(0)
		, n32ReliveSecond(0)
		, n32CriPersent(0)
		, n32CriHarm(0)
		, n32CPRecover(0)
		, n32DizzinessCounter(0)
		, n32SilenceCounter(0)
		, n32DisarmCounter(0)
		, n32RestrainCounter(0)
		, n32InvisibleCounter(0){}


	SFightProperty& operator += (SFightProperty& rsRight)
	{
		n32PhyAttPower		+= rsRight.n32PhyAttPower;
		n32MagicAttPower	+= rsRight.n32MagicAttPower;
		n32PhyDefAbility	+= rsRight.n32PhyDefAbility;
		n32MagicDefAbility	+= rsRight.n32MagicDefAbility;
		n32MoveSpeed		+= rsRight.n32MoveSpeed;
		n32AttackSpeed		+= rsRight.n32AttackSpeed;
		n32AttackDist		+= rsRight.n32AttackDist;
		n32MaxHP			+= rsRight.n32MaxHP;
		n32MaxMP			+= rsRight.n32MaxMP;
		n32HPRecover		+= rsRight.n32HPRecover;
		n32MPRecover		+= rsRight.n32MPRecover;
		n32ReliveSecond		+= rsRight.n32ReliveSecond;

		n32CriPersent		+= rsRight.n32CriPersent;
		n32CriHarm			+= rsRight.n32CriHarm;
		n32CPRecover		+= rsRight.n32CPRecover;

		n32DizzinessCounter += rsRight.n32DizzinessCounter;
		n32SilenceCounter	+= rsRight.n32SilenceCounter;
		n32DisarmCounter	+= rsRight.n32DisarmCounter;
		n32RestrainCounter	+= rsRight.n32RestrainCounter;
		n32InvisibleCounter	+= rsRight.n32InvisibleCounter;
		return *this;
	}

	SFightProperty& operator *= (INT32 i)
	{
		n32PhyAttPower		*= i;
		n32MagicAttPower	*= i;
		n32PhyDefAbility	*= i;
		n32MagicDefAbility	*= i;
		n32MoveSpeed		*= i;
		n32AttackSpeed		*= i;
		n32AttackDist		*= i;

		n32MaxHP			*= i;
		n32MaxMP			*= i;
		n32HPRecover		*= i;
		n32MPRecover		*= i;
		n32ReliveSecond		*= i;

		n32CriPersent		*= i;
		n32CriHarm			*= i;
		n32CPRecover		*= i;
		return *this;
	}

	SFightProperty operator *(INT32 i)
	{
		SFightProperty sNewFP;

		sNewFP.n32PhyAttPower		= n32PhyAttPower		* i;
		sNewFP.n32MagicAttPower		= n32MagicAttPower	* i;
		sNewFP.n32PhyDefAbility		= n32PhyDefAbility	* i;
		sNewFP.n32MagicDefAbility	= n32MagicDefAbility	* i;
		sNewFP.n32MoveSpeed			= n32MoveSpeed		* i;
		sNewFP.n32AttackSpeed		= n32AttackSpeed		* i;
		sNewFP.n32AttackDist		= n32AttackDist		* i;
		sNewFP.n32MaxHP				= n32MaxHP			* i;
		sNewFP.n32MaxMP				= n32MaxMP			* i;
		sNewFP.n32HPRecover			= n32HPRecover		* i;
		sNewFP.n32MPRecover			= n32MPRecover		* i;
		sNewFP.n32ReliveSecond		= n32ReliveSecond		* i;

		sNewFP.n32CriPersent		= n32CriPersent		* i;
		sNewFP.n32CriHarm			= n32CriHarm			* i;
		sNewFP.n32CPRecover			= n32CPRecover		* i;

		return sNewFP;
	}

	SFightProperty operator + (const SFightProperty& rsLeft){
		SFightProperty sNewFP;
		sNewFP.n32PhyAttPower		= n32PhyAttPower	+ rsLeft.n32PhyAttPower		;
		sNewFP.n32MagicAttPower		= n32MagicAttPower	+ rsLeft.n32MagicAttPower	;
		sNewFP.n32PhyDefAbility		= n32PhyDefAbility	+ rsLeft.n32PhyDefAbility	;
		sNewFP.n32MagicDefAbility	= n32MagicDefAbility+ rsLeft.n32MagicDefAbility	;
		sNewFP.n32MoveSpeed			= n32MoveSpeed		+ rsLeft.n32MoveSpeed		;
		sNewFP.n32AttackSpeed		= n32AttackSpeed	+ rsLeft.n32AttackSpeed		;
		sNewFP.n32AttackDist		= n32AttackDist		+ rsLeft.n32AttackDist		;
		sNewFP.n32MaxHP				= n32MaxHP			+ rsLeft.n32MaxHP			;
		sNewFP.n32MaxMP				= n32MaxMP			+ rsLeft.n32MaxMP			;
		sNewFP.n32HPRecover			= n32HPRecover		+ rsLeft.n32HPRecover		;
		sNewFP.n32MPRecover			= n32MPRecover		+ rsLeft.n32MPRecover		;
		sNewFP.n32ReliveSecond		= n32ReliveSecond	+ rsLeft.n32ReliveSecond	;

		sNewFP.n32CriPersent		= n32CriPersent		+ rsLeft.n32CriPersent		;
		sNewFP.n32CriHarm			= n32CriHarm		+ rsLeft.n32CriHarm			;
		sNewFP.n32CPRecover			= n32CPRecover		+ rsLeft.n32CPRecover		;

		sNewFP.n32DizzinessCounter	= n32DizzinessCounter	+ rsLeft.n32DizzinessCounter	;
		sNewFP.n32SilenceCounter	= n32SilenceCounter		+ rsLeft.n32SilenceCounter		;
		sNewFP.n32DisarmCounter		= n32DisarmCounter		+ rsLeft.n32DisarmCounter		;
		sNewFP.n32RestrainCounter	= n32RestrainCounter	+ rsLeft.n32RestrainCounter		;
		sNewFP.n32InvisibleCounter  = n32InvisibleCounter   + rsLeft.n32InvisibleCounter    ;

		return sNewFP;
	}

	void	Clear(){
		n32PhyAttPower		= 0;
		n32MagicAttPower	= 0;
		n32PhyDefAbility	= 0;
		n32MagicDefAbility	= 0;
		n32MoveSpeed		= 0;
		n32AttackSpeed		= 0;
		n32AttackDist		= 0;
		n32MaxHP			= 0;
		n32MaxMP			= 0;
		n32HPRecover		= 0;
		n32MPRecover		= 0;
		n32ReliveSecond		= 0;

		n32CriPersent		= 0;
		n32CriHarm			= 0;
		n32CPRecover		= 0;

		n32DizzinessCounter = 0;
		n32SilenceCounter	= 0;
		n32DisarmCounter	= 0;
		n32RestrainCounter	= 0;
		n32InvisibleCounter = 0;
	}						

	static SFightProperty getTotalFP(SFightProperty& sFPBase ,SFightProperty& sFPPercent){
		SFightProperty newFP;
		newFP.n32PhyAttPower		=	(INT32)(sFPBase.n32PhyAttPower * (1 + (float)sFPPercent.n32PhyAttPower / c_fPersentDevide));
		newFP.n32MagicAttPower		=	(INT32)(sFPBase.n32MagicAttPower * (1 + (float)sFPPercent.n32MagicAttPower / c_fPersentDevide));
		newFP.n32PhyDefAbility		=	(INT32)(sFPBase.n32PhyDefAbility * (1 + (float)sFPPercent.n32PhyDefAbility / c_fPersentDevide));
		newFP.n32MagicDefAbility	=	(INT32)(sFPBase.n32MagicDefAbility * (1 + (float)sFPPercent.n32MagicDefAbility / c_fPersentDevide));
		newFP.n32MoveSpeed			=	(INT32)(sFPBase.n32MoveSpeed * (1 + (float)sFPPercent.n32MoveSpeed / c_fPersentDevide));
		newFP.n32AttackSpeed		=	(INT32)(sFPBase.n32AttackSpeed / (1 +  (float)sFPPercent.n32AttackSpeed / c_fPersentDevide));
		newFP.n32AttackDist			=	(INT32)(sFPBase.n32AttackDist * (1 + (float)sFPPercent.n32AttackDist / c_fPersentDevide));
		newFP.n32MaxHP				=	(INT32)(sFPBase.n32MaxHP * (1 + (float)sFPPercent.n32MaxHP / c_fPersentDevide));
		newFP.n32MaxMP				=	(INT32)(sFPBase.n32MaxMP * (1 + (float)sFPPercent.n32MaxMP / c_fPersentDevide));
		newFP.n32HPRecover			=	(INT32)(sFPBase.n32HPRecover * (1 + (float)sFPPercent.n32HPRecover / c_fPersentDevide));
		newFP.n32MPRecover			=	(INT32)(sFPBase.n32MPRecover * (1 + (float)sFPPercent.n32MPRecover / c_fPersentDevide ));
		newFP.n32ReliveSecond		=	(INT32)(sFPBase.n32ReliveSecond* (1 + (float)sFPPercent.n32ReliveSecond / c_fPersentDevide));
		newFP.n32CriPersent			=	(INT32)(sFPPercent.n32CriPersent);
		newFP.n32CriHarm			=	(INT32)(sFPPercent.n32CriHarm);
		newFP.n32CPRecover			=	(INT32)(sFPBase.n32CPRecover * (1 + (float)sFPPercent.n32CPRecover / c_fPersentDevide));
		newFP.n32DizzinessCounter	=	(INT32)(sFPBase.n32DizzinessCounter);
		newFP.n32SilenceCounter		=	(INT32)(sFPBase.n32SilenceCounter);
		newFP.n32DisarmCounter		=	(INT32)(sFPBase.n32DisarmCounter);
		newFP.n32RestrainCounter	=	(INT32)(sFPBase.n32RestrainCounter);
		newFP.n32InvisibleCounter   =   (INT32)(sFPBase.n32InvisibleCounter);
		return newFP;
	}

	string GetFPString(){
		char sFpString[512];
		sprintf(sFpString, "n32PhyAttPower:%d, n32MagicAttPower:%d, n32PhyDefAbility:%d, n32MagicDefAbility:%d, n32MoveSpeed:%d n32AttackSpeed:%d,"
			"n32AttackDist:%d, n32MaxHP:%d, n32MaxMP:%d, n32HPRecover:%d, n32MPRecover:%d, n32ReliveSecond:%d.", n32PhyAttPower, n32MagicAttPower, n32PhyDefAbility
			, n32MagicDefAbility, n32MoveSpeed, n32AttackSpeed, n32AttackDist, n32MaxHP, n32MaxMP, n32HPRecover, n32MPRecover, n32ReliveSecond);

		return sFpString;
	}
};

enum EProperty{
	eProperty_None = 0,
	eProperty_PhyAttPower,
	eProperty_MagicAttPower,
	eProperty_PhyDefAbility,
	eProperty_MagicDefAbility,
	eProperty_MoveSpeed,
	eProperty_AttackSpeed,
	eProperty_MaxHP,
	eProperty_MaxMP,
	eProperty_HPRecover,
	eProperty_MPRecover,
	eProperty_ReliveSecond,
	eProperty_End,
};

struct SSSendRunesInfo{
	SSSendRunesInfo(){}
	SSSendRunesInfo(UINT32 eOT, INT16 Pos
		, INT16 RunesNum
		, INT8  RunesPosType) : eOT(eOT)
		, n16Pos(Pos)
		, n16RunesNum(RunesNum)
		, n8RunesPosType(RunesPosType){

	}
	UINT32 eOT;
	INT16 n16Pos;
	INT16 n16RunesNum;
	INT8  n8RunesPosType;
};

struct SSendData{
	UINT32	un32DestoryBuildings;
	UINT32	un32HeroKills;
	UINT32	un32DeadTimes;
	UINT32	un32Assist;
	UINT32	un32LastHit;

	//for log
	UINT32  un32RealSendHurt;	//真实伤害输出积累
	UINT32  un32SkillPhySendHurt;	//技能物理输出伤害积累
	UINT32  un32SkillMagicSendHurt;	//技能魔法输出伤害积累 
	UINT32	un32SendHurt;	//输出伤害值积累

	UINT32  un32RevRealHurt; //真实承受伤害积累
	UINT32  un32RevSkillPhyHurt;//技能承受物理伤害积累
	UINT32  un32RevSkillMagicHurt;//技能承受魔法伤害积累
	UINT32  un32RevHurt;	//承受伤害值积累

	UINT32  un32KillSolder;	//击杀小兵数量积累
	UINT32  un32KillMonster; //击杀野怪积累
	SSendData() : un32DestoryBuildings(0)
		, un32HeroKills(0)
		, un32DeadTimes(0)
		, un32Assist(0)
		, un32LastHit(0)
		,un32RealSendHurt(0)
		,un32SkillMagicSendHurt(0)
		,un32SkillPhySendHurt(0)
		,un32SendHurt(0)
		,un32RevRealHurt(0)
		,un32RevSkillPhyHurt(0)
		,un32RevSkillMagicHurt(0)
		,un32RevHurt(0)
		,un32KillSolder(0)
		,un32KillMonster(0)
	{

	}

	void Clear(){
		un32DestoryBuildings = 0;
		un32DeadTimes = 0;
		un32HeroKills = 0;
		un32Assist = 0;
		un32LastHit = 0;
		un32RealSendHurt=0;
		un32SkillMagicSendHurt=0;
		un32SkillPhySendHurt=0;
		un32SendHurt=0;
		un32RevRealHurt=0;
		un32RevSkillPhyHurt=0;
		un32RevSkillMagicHurt=0;
		un32RevHurt=0;
		un32KillSolder=0;
		un32KillMonster=0;
	}
};

struct SUserNetInfo
{
	INT32	n32GSID;
	INT32	n32GCNSID;

	SUserNetInfo()
		:n32GSID(0)
		, n32GCNSID(0)
	{
	}

	SUserNetInfo(INT32 n32GSID, INT32 n32GCNSID)
		: n32GSID(n32GSID)
		, n32GCNSID(n32GCNSID)
	{
	}

	void	operator	=	(const SUserNetInfo &crsRight)
	{
		n32GSID = crsRight.n32GSID;
		n32GCNSID = crsRight.n32GCNSID;
	}

	bool	operator == (const SUserNetInfo &crsRight)
	{
		if (n32GSID != crsRight.n32GSID || n32GCNSID != crsRight.n32GCNSID)
		{
			return false;
		}
		return true;
	}

	bool	operator != (const SUserNetInfo &crsRight)
	{
		if (n32GSID != crsRight.n32GSID || n32GCNSID != crsRight.n32GCNSID)
		{
			return true;
		}
		return false;
	}

	bool	operator<(const SUserNetInfo &crsRight)const
	{
		INT32 n32Cmp = memcmp(this, &crsRight, sizeof(SUserNetInfo));
		if (0 > n32Cmp)
		{
			return true;
		}
		return false;
	}

	void Clear()
	{
		n32GSID = 0;
		n32GCNSID = 0;
	}
	bool IsValid() const{
		return n32GCNSID > 0 && n32GSID > 0;
	}
};

struct SDownInfo{
	SDownInfo():un32Child1Type(0)
		, un32Child2Type(0){}
	SDownInfo(UINT32 child1, UINT32 child2):un32Child1Type(child1)
		, un32Child2Type(child2){}
	UINT32 un32Child1Type;
	UINT32 un32Child2Type;
};

struct SUpInfo{
	SUpInfo():un32ParentType(0)
		, un32BrotherType(0){}
	SUpInfo(UINT32 parent, UINT32 brother):un32ParentType(parent)
		, un32BrotherType(brother){}
	UINT32 un32ParentType;
	UINT32 un32BrotherType;
};
struct SGoodsCompose{
	SDownInfo sDownInfo;
	SUpInfo	sUpInfo[c_un32MaxGoodsDownInfoNum];
};
struct SGoodsEffect{
	SGoodsEffect():n32Atrr(0)
		, n32Val(0)
		, n32Percent(0){}
	INT32 n32Atrr;
	INT32 n32Val;
	INT32 n32Percent;
};

struct SGoodsCfg{
	UINT32				un32GoodsID;
	EObjectType			eOT;
	UINT8				un8TypeID;	
	SGoodsEffect		sGoodsEffect[c_un32MaxItemEffectNum];
	INT32				n32UseTimes;
	BOOLEAN				bUsedDestory;
	UINT32				un32SkillID;
	UINT32				lun32PassiveSkillID[c_n32MaxNextSkillEffectCfgNum];
	BOOLEAN				bUsedAfterBuyed;
	UINT32				un32CDTime;
	BOOLEAN				bUniqueID;
	INT32				n32OverlapTimes;
	INT32				n32CPCostType;
	INT32				n32CPCost;
	UINT8				un8FunctionType;
	UINT32				un32UniqueTypeID;
	INT32				n32CombineCPCost;
	SGoodsCompose		sGoodsCompose;
	bool				bIfCanBuyWhenDeath;
	SGoodsCfg(){
		memset(this,0,sizeof(*this));
	}
};

struct SHeroChooseCfg{
	UINT32 un32HeroID;
	INT32 n32IfGuide;
	char szName[32];
};

enum ERobotAIType{
	eRobotAIType_Guide,
	eRobotAIType_PvE,
	eRobotAIType_Match,
};

struct SRobotAICfg{
	ERobotAIType n32AiType;
	INT32 n32ID;
	INT32 n32ModelID;
	INT32 n32NodeType;
	INT32 n32ParentID;
	INT32 an32Parameters[16];
};

struct SRobotAIPathCfg{
	INT32 un32ID;
	INT32 n32MapID;
	INT32 n32CampID;
	INT32 n32PahtID;
	float fX;
	float fY;
	float fZ;
};

struct SUserMgrCfg 
{		
	UINT32	un32UserDBHostPort;
	CHAR	aszUserDBHostIP[c_n32DefaultNameLen];
	CHAR	aszUserDBUserName[c_n32DefaultNameLen * 2];
	CHAR	aszUserDBUserPwd[c_n32DefaultNameLen * 2];
	CHAR	aszUserDBName[c_n32DefaultNameLen * 2];
	CHAR	aszUserTableName[c_n32DefaultNameLen * 2];
};

enum EAttackWay{
	EAttackWay_Building = 0,
	EAttackWay_Near,
	EAttackWay_Range,
};

struct SNPCCfg{ 
	EObjectType	eOT;
	ERace	eRace;
	EAttendantCate	eAttendantCate;
	EMagicCate		eMagicCate;
	ENPCCate		eNPCCate;
	ENPCCateChild	eNPCCateChild;
	INT32			n32AttDist;
	EAICate			eAICate;
	UINT32			un32AITarID;
	INT32			n32PursueDist;
	INT32			n32GuardDist;
	EAttackMode		eAttMode;
	EAttackWay		eAttackWay;
	SFightProperty	sBaseFP;
	INT32			n32GotExp;
	INT32			n32KillGotCP;
	INT32			n32ConsumeCP;
	BOOLEAN			bIfCanControl;
	INT32			n32CollideRadius;
	UINT32			aun32SkillList[c_n32MaxNPCSkillNum];
	UINT32			lun32PassiveSkillID[c_n32MaxNextSkillEffectCfgNum];
	INT32			n32CampID;
	UINT32			un32ShopID;
	EWMGroup		eWMGroupCate;
	INT32			n32ConsumePer[eEffectCate_Restrain+1];
	INT32			n32AbsorbAttriChange[eEffectCate_Restrain+1];
	CVector3D		cEmitPos;
	CVector3D		cOnHitPos;
	BOOLEAN         bIsLocked;	//是否可锁定
};

struct SHeroCfg{
	EObjectType	eOT;
	INT64		n64Cost;
	EMagicCate	eMagicCate;
	EAttackWay		eAttackWay;
	INT32		n32BaseExp;
	INT32		n32ExpGrowth;
	SFightProperty	sBaseFP;
	SFightProperty	sGrowthFP;
	INT32			n32CPRecover;
	INT32			n32CollideRadius;
	UINT32			aun32SkillList[c_n32MaxHeroSkillNum];
	UINT32			aun32PassitiveSkillList[c_n32MaxHeroPassitiveSkillNum];
	CVector3D		cEmitPos;
	CVector3D		cOnHitPos;
	UINT32			aun32BuyItemList[32];

	SHeroCfg()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SGoodCPInfo{
	UINT32		un32GoodsID;
	UINT8		un8CPType;
	INT32		n32CPCost;
};
struct SShopCfg{
	EObjectType	eOT;
	SGoodCPInfo		sGoodCPInfo[c_un32MaxGoodsChildNum];
};

struct SMapDataHeader{
	UINT32			un32MapID;
	INT32			n32XCellNum;
	INT32			n32ZCellNum;
};

struct SMapObjCfg{
	UINT32	un32ID;
	UINT32	un32MapID;
	EObjectType	eObjTypeID;
	UINT32	un32ObjIdx;
	INT32	n32CampID;
	CVector3D	cBornPos;
	CVector3D	cBorDir;
	INT32		n32Visibility;
	EAttackMode	eAttackMode;
};

struct SMapCfg{
	UINT32	un32MapID;
	UINT32	un32MapCfgSize;
	UINT32	un32MapObjNum;
	UINT32	un32HeaderDataOffset;
	UINT32	un32MapObjOffset;
	SMapDataHeader*	psMapHeader;
	SMapObjCfg	*psMapObjCfg;
	//string luaFileStr;
};

struct SMapLogicCfg{
	UINT32 MapId;
	string MapName;
	UINT32 MapPic;
	UINT32 SeatCount;//最大玩家数//
	vector<UINT32> PlayerModel;//玩家模式//3v3 2v4 1v5 5v5//
};

///

#define MAX_TASK_PARAM 10
#define MAX_TASK_REWARD 5

enum TASK_TIME_TYPE
{
	TTT_Infinite,
	TTT_Daily,
};

enum TASK_TYPE
{
	TT_None,
	TT_KillPlayer,//击杀玩家
	TT_KillXiaobin,//击杀小兵
	TT_KillMonster,//击杀野怪
	TT_KillBuild,//击杀建筑
	TT_GamePlay,//进行一场游戏
	TT_GameWin,//胜利一场游戏
	TT_GameGetCP,//游戏获得CP
	TT_GameUseCP,//游戏使用CP
};

struct STaskConfig
{
	UINT32 taskID;
	UINT32 nextTaskID;
	TASK_TIME_TYPE taskTimeType;
	TASK_TYPE taskType;
	UINT32 taskMaxCount;
	UINT32 taskParams[MAX_TASK_PARAM];
	ELoginRewardItemType eItemType[MAX_TASK_REWARD];
	UINT32 un32num[MAX_TASK_REWARD];
	STaskConfig() { memset(this,0,sizeof(STaskConfig)); }
};

///

enum EEffectCaculateCate{
	EEffectCaculateCate_None = 0,
	EEffectCaculateCate_SelfPhyAttack,//1.自己物攻加成
	EEffectCaculateCate_SelfMagicAttack,//2.自己魔攻加成
	EEffectCaculateCate_SelfPhyDefence,//3.自己物防加成
	EEffectCaculateCate_SelfMagicDefence,//4.自己魔防加成
	EEffectCaculateCate_SelfNowHP,//5.自己当前生命值加成
	EEffectCaculateCate_SelfMaxHP,//6.自己最大生命值加成
	EEffectCaculateCate_SelfLostHpPercent,//7.自己损失生命比加成(每1%加成)
	EEffectCaculateCate_SelfNowMP,//8.自己当前魔法值加成
	EEffectCaculateCate_SelfMaxMP,//9.自己最大魔法值加成
	EEffectCaculateCate_SelfLostMpPercent,//10.自己损失魔法比加成(每1%加成)
	EEffectCaculateCate_SelfLevel,//11.自己等级加成
	EEffectCaculateCate_SelfMoveSpeed,//12.自己移动速度加成(每1厘米加成)

	EEffectCaculateCate_TarPhyAttack = 21,//21.目标物攻加成
	EEffectCaculateCate_TarMagicAttack,//22.目标魔攻加成
	EEffectCaculateCate_TarPhyDefence,//23.目标物防加成
	EEffectCaculateCate_TarMagicDefence,//24.目标魔防加成
	EEffectCaculateCate_TarNowHP,//25.目标当前生命值加成
	EEffectCaculateCate_TarMaxHP,//26.目标最大生命值加成
	EEffectCaculateCate_TarLostHpPercent,//27.目标损失生命比加成(每1%加成)
	EEffectCaculateCate_TarNowMP,//28.目标当前魔法值加成
	EEffectCaculateCate_TarMaxMP,//29.目标最大魔法值加成
	EEffectCaculateCate_TarLostMpPercent,//30.目标损失魔法比加成(每1%加成)
	EEffectCaculateCate_TarLevel,//31.目标等级加成
	EEffectCaculateCate_TarMoveSpeed,//32.目标移动速度加成(每1厘米加成)

};

struct SEffectInfo{
	EEffectCate			eEffectCate;
	INT32				n32EffectBaseValue;//基础值，用于伤害和属性改变
	INT32				n32EffectRate;//千分比该变量，只用于属性改变
	EEffectCaculateCate eEffectAddCacuCate[c_n32MaxEffectCaculateCateNum];//伤害的基础值改变类型
	INT32				eEffectAddCacuValue[c_n32MaxEffectCaculateCateNum];//伤害的基础值改变值
	EEffectCaculateCate eEffectMultCacuCate[c_n32MaxEffectCaculateCateNum];//伤害的比例改变类型
	INT32				eEffectMultCacuValue[c_n32MaxEffectCaculateCateNum];//伤害的比例改变值
	BOOLEAN				bIfBlastAttack;
	INT32				n32BlastAttPercent;
	INT32				n32BlastAttRate;
	

	SEffectInfo(): eEffectCate(eEffectCate_None)
		, n32EffectBaseValue(0)
		, n32EffectRate(0)
		, bIfBlastAttack(0)
		, n32BlastAttPercent(0)
		, n32BlastAttRate(0)
		{
			for(int i=0;i<c_n32MaxEffectCaculateCateNum;i++){
				eEffectAddCacuCate[i] = EEffectCaculateCate_None;
				eEffectAddCacuValue[i] = 0;
				eEffectMultCacuCate[i] = EEffectCaculateCate_None;
				eEffectMultCacuValue[i] = 0;
			}
		}
};

//下一步技能配置
struct SSNextSkillEffectCfg{
	UINT32  un32SkillEffectID;//技能模块id
	INT32   n32Delay;//调用等待时间
	INT32	n32DependedArr[c_n32MaxDependSkillEffectCfgNum];
	SSNextSkillEffectCfg(): un32SkillEffectID(0)
		, n32Delay(0){
	}
};

//技能模块类型
enum ESkillEffectType{
	eSkillEffectType_None = 0,
	eSkillEffectType_Caculate,
	eSkillEffectType_Emit,
	eSkillEffectType_Range,
	eSkillEffectType_Leading,
	eSkillEffectType_Summon,
	eSkillEffectType_Move,
	eSkillEffectType_Switch,
	eSkillEffectType_Purification,
	eSkillEffectType_Link,
	eSkillEffectType_Buf = 10,
};

//基础的技能模块配置
struct SSkillEffectlBaseCfg{
	UINT32  un32SkillModelID;//技能模块id
	ESkillEffectType eSkillModelType;//技能类型
	BOOLEAN	bIsCoolDown;//是否开始计算CD
	INT32	n32TriggerRate;//触发概率
	BOOLEAN	bIfAffectBuilding;//对建筑有效
	BOOLEAN	bIfAffectHero;//对英雄有效
	BOOLEAN	bIfAffectMonster;//对NPC有效
	BOOLEAN	bIsCanMove;//是否可以在效果维持阶段移动
	BOOLEAN	bIsCanBreak;//是否可以被其他的动作打断
	ESkillModelTargetCate eTargetType;//buff只能对哪种目标生效，友方敌方还是所有
	ESkillBuffTarCate eBuffTarget;//buff的添加对象，自己或是目标 
	INT32  n32ReleaseTimeDelay;		
	SSNextSkillEffectCfg	asSkillModelList[c_n32MaxNextSkillEffectCfgNum];

	SSkillEffectlBaseCfg():un32SkillModelID(0)
		, eSkillModelType(eSkillEffectType_None)
		, bIsCoolDown(FALSE)
		, n32TriggerRate(0)
		, bIfAffectBuilding(FALSE)
		, bIfAffectHero(FALSE)
		, bIfAffectMonster(FALSE)
		, bIsCanMove(TRUE)
		, bIsCanBreak(TRUE)
		, eTargetType(eSMTC_None){
	}
};

//发射类技能模块技能类型
enum ESkillModelEmitType{
	eSkillModelEmitType_Follow = 1,
	eSkillModelEmitType_Direct,
	eSkillModelEmitType_Boomerang,
	eSkillModelEmitType_Bounce,
	eSkillModelEmitType_Bounce_NoRepeat,
	eSkillModelEmitType_Area,
	eSkillModelEmitType_AutoFind,
};

//发射类技能模块配置
struct SSkillModelEmitCfg : public SSkillEffectlBaseCfg{
	INT32	n32UseMP;//消耗MP
	INT32	n32UseHP;//消耗HP
	INT32	n32UseCP;//消耗CP
	ESkillModelEmitType eEmitType;
	INT32		n32ProjFlySpeed;
	INT32		n32FlyPar1;
	INT32		n32FlyPar2;
	INT32		n32FlyPar3;
	BOOLEAN		bIsPenetrate;
	INT32		n32ProjectileLifeTime;
	INT32		n32ProjectileCollideRadius;
	INT32		n32ProjectileNum;
	INT32		n32ProjectileAngle;
};

//结算类技能模块技能类型
enum ESkillModelAccountType{
	eSkillModelAccountType_None = 0,
	eSkillModelAccountType_PhyDmg,
	eSkillModelAccountType_MagicDmg,
	eSkillModelAccountType_ChangeHp,
	eSkillModelAccountType_ChangeMp,
	eSkillModelAccountType_SuckBlood,
};

//结算类技能模块配置
struct SSkillModelAccountCfg : public SSkillEffectlBaseCfg{
	SEffectInfo sEffectInfo;
};

//引导类技能模块配置
struct SSkillModelLeadingCfg : public SSkillEffectlBaseCfg{
	INT32 eLeadingTime;
	BOOLEAN bIsCheckTargetSta;
};

//施放朝向
enum ESkillReleaseWay{
	ESReleaseWay_NO_TARGET = 0,
	ESReleaseWay_NEED_TARGET_NO_TURN,
	ESReleaseWay_NEED_TARGET,
	ESReleaseWay_AUTO,
};

enum ESkillType{
	ESkillType_Plus = 1,
	ESkillType_Hurt,
	ESkillType_Control,
	ESkillType_Move,
	ESkillType_TransHome,
};

//新的技能配置
struct SSNewSkillCfg{
	UINT32	un32SkillID;//技能id
	ESkillType eSkillType;//技能类型
	BOOLEAN bIfNomalAttack;//是否普通攻击
	BOOLEAN bIsConsumeSkill;//是否吸附技能
	ESkillReleaseWay eReleaseWay;//施放朝向
	INT32	n32UpgradeLevel;//对应等级
	INT32	n32UseMP;//消耗MP
	INT32	n32UseHP;//消耗HP
	INT32	n32UseCP;//消耗CP
	INT32	n32PrepareMilsec;
	INT32   n32CoolDown;//冷却时间
	INT32	n32ReleaseMilsec;//准备动作时间
	INT32	n32SkillLastTime;//技能后摇时间
	FLOAT	fReleaseDist;//释放距离
	ESkillUseWay	eUseWay;//技能触发方式
	INT32	n32TriggerRate;//触发概率
	BOOLEAN	bIfAffectBuilding;//对建筑有效
	BOOLEAN	bIfAffectHero;//对英雄有效
	BOOLEAN	bIfAffectMonster;//对NPC有效
	BOOLEAN	bIfCheckTarState;
	BOOLEAN	bIfImpact;//是否是瞬发技能
	ESkillTargetCate	eSkillTargetCate;//技能施放时所选择的目标类型
	SSNextSkillEffectCfg     asSkillModelList[c_n32MaxNextSkillEffectCfgNum];
};

enum EBuffType{
	eBuffType_Ohter = 0,//其他类型
	eBuffType_Positive,//增益
	eBuffType_Negative,//减益
};

//buf
struct SSkillModelBufCfg : public SSkillEffectlBaseCfg
{ 
	INT32   n32UseMp;
	INT32   n32UseHp;
	INT32   n32UseCP;  

	EBuffType	eBuffType;//类型
	INT32   n32EffectLastTick;//持续时间
	INT32   n32EffectInterval;//间隔时间
	EReplaceType    eReplaceType; //堆叠类型
	INT32   n32ReplaceTimes; //堆叠数量
	INT32   n32RejectId;	//互斥id
	INT32   n32ReplaceId;  //替换id
	BOOLEAN bIfClearAtDie; //死亡是否清理
	BOOLEAN n32FlyEffectID;//击飞ID
	SSNextSkillEffectCfg	asSkillStartModelList[c_n32MaxNextSkillEffectCfgNum];	//开始启动模块
	SSNextSkillEffectCfg	asSkillIntervalModelList[c_n32MaxNextSkillEffectCfgNum]; //间隔模块id
	SSNextSkillEffectCfg	asSkillEndModelList[c_n32MaxNextSkillEffectCfgNum]; //结束模块id  
	SEffectInfo sEffectInfo;

	SSkillModelBufCfg()
		: n32UseMp(0)
		, n32UseHp(0)
		, n32UseCP(0)
		, n32EffectLastTick(0)
		, n32EffectInterval(0)
		, eReplaceType(EReplaceType_None)
		, n32ReplaceTimes(0)
		, n32RejectId(0)
		, n32ReplaceId(0)
		, bIfClearAtDie(TRUE){
	}
};
//范围
struct SSkillModelRangeCfg : public SSkillEffectlBaseCfg
{ 
	INT32   n32UseMp;
	INT32   n32UseHp;
	INT32   n32UseCP; 

	ESkillAOECate    eSkillAOECate;
	INT32   n32RangeInterval;
	INT32   n32RangeTimes;
	ESkillShapeType  eSkillShapeType;
	INT32   n32RangePar1;
	INT32   n32RangePar2;
	INT32   n32MaxEffectObj;
	INT32   n32LifeTime; 
	char    szattackEffect[n_c32SkkEffectNum];

	INT32	n32ReleaseDist;

	BOOLEAN bIfHasEffect;

	SSkillModelRangeCfg()
	{
		memset(this, 0, sizeof(*this));
	}
};

//召唤物 
struct SSkillModuleSummonCfg: public SSkillEffectlBaseCfg
{
	INT32   n32UseMp;
	INT32   n32UseHp;
	INT32   n32UseCP;  
	ESummonType			eSummonType;
	ESummonWayType		eSummonWayType;

	INT32  nNPCId[n_c32NPCNum]; //召唤物id
	INT32  n32SummonNum[n_c32NPCNum];//召唤物数量
	INT32  n32Distance;				
	INT32  n32LifeTime; 

	SSkillModuleSummonCfg()
	{
		memset(this,0,sizeof(*this));
	}
};


//被动技能系统开始

//被动技能触发条件
enum EPassiveSkillTriggerType
{
	EPassiveSkillTriggerType_None = 0,
	EPassiveSkillTriggerType_HeartBeat,
	EPassiveSkillTriggerType_Attack,
	EPassiveSkillTriggerType_Attacked,
	EPassiveSkillTriggerType_Hurted,
	EPassiveSkillTriggerType_Die,
	EPassiveSkillTriggerType_SkillAttack,
	EPassiveSkillTriggerType_UseSkill,
	EPassiveSkillTriggerType_TargetDie,
	EPassiveSkillTriggerType_NormalAttackCaculate_Before,
	EPassiveSkillTriggerType_NormalAttackCaculate_After,
	EPassiveSkillTriggerType_NormalAttackHurt,
	EPassiveSkillTriggerType_SkillHurt,
	EPassiveSkillTriggerType_Move,
	EPassiveSkillTriggerType_Max,
};


//被动技能类型
enum EPassiveSkillType
{
	EPassiveSkillType_None = 0,
	EPassiveSkillType_BloodSeek,
	EPassiveSkillType_Rebound,
	EPassiveSkillType_Relive,
};

enum EPassiveSkillTargetType
{
	EPassiveSkillTargetType_Self = 1,
	EPassiveSkillTargetType_Trigger,
	EPassiveSkillTargetType_Both,
};

enum EPassiveRangeType{
	EPassiveRangeType_All = 0,
	EPassiveRangeType_Near,
	EPassiveRangeType_Range,
};

struct SSPassiveSkilllCfg
{
	UINT32	un32PassiveSkillID;//被动技能id
	ESkillReleaseWay eReleaseWay;//施放朝向
	INT32	n32UpgradeLevel;//对应等级
	INT32	n32UseMP;//消耗MP
	INT32	n32UseHP;//消耗HP
	INT32	n32UseCP;//消耗CP
	INT32	n32Cooldown;//冷却时间,0代表无冷却
	EPassiveRangeType eRangeType;//射程触发类型。0都可以使用。1近战，2远程
	EPassiveSkillTriggerType lePassiveSkillTriggerType[n_cPassiveSkillTriggerNum];//触发条件
	INT32	n32TriggerRate;//触发概率
	INT32	n32TriggerInterval;//触发频率
	BOOLEAN	bIfAffectBuilding;//对建筑有效
	BOOLEAN	bIfAffectHero;//对英雄有效
	BOOLEAN	bIfAffectMonster;//对NPC有效
	EPassiveSkillTargetType	ePassiveSkillTargetType;//技能触发时所选择的目标类型
	BOOLEAN	bIfEffectOnDis;//死亡是否移除
	SSNextSkillEffectCfg     asStartSkillModelList[c_n32MaxNextSkillEffectCfgNum];//被动挂载时触发的主动技能效果
	SSNextSkillEffectCfg     asEndSkillModelList[c_n32MaxNextSkillEffectCfgNum];//被动卸载时触发的主动技能效果
	SSNextSkillEffectCfg     asSkillModelList[c_n32MaxNextSkillEffectCfgNum];//触发后接续的主动技能效果
	INT32	lPassitiveEffectList[c_n32MaxNextSkillEffectCfgNum];//触发后接续的被动技能效果

	//特效部分
	BOOLEAN bIfHasStartEffect;//是否具有启动特效
	BOOLEAN bIfHasReleaseEffect;//是否具有触发特效

	BOOLEAN	bIfSendColdDown;//是否向客户端发送冷却消息

	SSPassiveSkilllCfg()
	{
		memset(this,0,sizeof(*this));
	}
};

//吸血被动
struct SSPassiveEffectCfg_BloodSeek
{
	INT32	n32PassitiveEffectID;//被动模块ID
	INT32	n32BaseValue;//基础值
	INT32	n32Percent;//基础千分比
	INT32	n32TargetHealthPercent;//对方最大生命千分比
	INT32	n32SelfHealthPercent;//自己最大生命千分比
};

//反伤被动
struct SSPassiveEffectCfg_Rebound
{
	INT32	n32PassitiveEffectID;//被动模块ID
	INT32	n32BaseValue;//基础值
	INT32	n32Percent;//基础千分比
	INT32	n32TargetHealthPercent;//对方最大生命千分比
	INT32	n32SelfHealthPercent;//自己最大生命千分比
};

//复活被动
struct SSPassiveEffectCfg_Relive
{
	INT32	n32PassitiveEffectID;//被动模块ID
	INT32	n32WaitMilSec;//复活等待时间
	INT32	n32BaseHP;//基础复活HP
	INT32	n32PercentHP;//HP千分比
	INT32	n32BaseMP;//基础复活MP
	INT32	n32PercentMP;//MP千分比
};


//被动技能系统结束

enum ESkillEffectMoveType{
	ESkillEffectMoveType_HOME = 1,//回城
	ESkillEffectMoveType_Absolute,//移动到目标坐标点
	ESkillEffectMoveType_Opposite,//在现有基础上相对移动
};

enum ESkillEffectMovedTargetType{
	ESkillEffectMovedTargetType_Self = 1,//自己
	ESkillEffectMovedTargetType_Target,//目标
};

enum ESkillEffectMoveToTargetType{
	ESkillEffectMoveToTargetType_Hit_Anger = 0,//根据受击角度
	ESkillEffectMoveToTargetType_Self,//自己
	ESkillEffectMoveToTargetType_Target,//模块的目标
	ESkillEffectMoveToTargetType_Target_Anger,//根据受击目标的角度
	ESkillEffectMoveToTargetType_Master_Anger,//根据施法者的角度
	ESkillEffectMoveToTargetType_SkillTarget,//技能施放时候的目标
};

//位移技能模块
struct SSkillModelMoveCfg : public SSkillEffectlBaseCfg{
	INT32	n32UseMP;//消耗MP
	INT32	n32UseHP;//消耗HP
	INT32	n32UseCP;//消耗CP
	ESkillEffectMoveType eMoveType;//移动类型
	ESkillEffectMovedTargetType eMovedTargetType;//移动者类型
	ESkillEffectMoveToTargetType eMoveToTargetType;//移动目标类型
	INT32	n32Angle;//相对移动角度
	INT32	n32Distance;//相对移动距离
	INT32	n32Speed;//移动速度
};

//开关技能模块
struct SSkillModelSwitchCfg : public SSkillEffectlBaseCfg{
	INT32	n32UseMP;//消耗MP
	INT32	n32UseHP;//消耗HP
	INT32	n32UseCP;//消耗CP
	INT32	lPassitiveEffectList[c_n32MaxNextSkillEffectCfgNum];//开关的被动技能效果
	SSkillModelSwitchCfg()
	{
		memset(this,0,sizeof(*this));
	}
};

enum	EPurificationType{
	ePurType_byType = 1,
	ePurType_byID,
	ePurType_byEffect,
};

//净化技能模块
struct SSkillModelPurificationCfg : public SSkillEffectlBaseCfg{
	INT32	n32UseMP;//消耗MP
	INT32	n32UseHP;//消耗HP
	INT32	n32UseCP;//消耗CP
	EPurificationType ePurType;//净化类型
	INT32	lPurParameters[c_n32MaxNextSkillEffectCfgNum];//参数
	INT32	n32MaxNum;//最大净化数量
	SSkillModelPurificationCfg()
	{
		memset(this,0,sizeof(*this));
	}
};

enum	ELinkStartPointType{
	eLinkStartPointType_Releaser = 1,
	eLinkStartPointType_ReleaserPos,
};

//链接技能模块
struct SSkillModelLinkCfg : public SSkillEffectlBaseCfg{
	INT32	n32UseMP;//消耗MP
	INT32	n32UseHP;//消耗HP
	INT32	n32UseCP;//消耗CP
	BOOLEAN bIfCanBeBreak;//是否可以被打断
	ELinkStartPointType eStartPointType;//链接起点类型
	BOOLEAN bIfMoveBreak;//是否会被移动打断
	INT32	n32FarthestDistance;//最大距离
	INT32	n32LastTime;//持续时间
	SSNextSkillEffectCfg	asBreakModelList[c_n32MaxNextSkillEffectCfgNum];
	SSNextSkillEffectCfg	asFinishModelList[c_n32MaxNextSkillEffectCfgNum];
	SSkillModelLinkCfg()
	{
		memset(this,0,sizeof(*this));
	}
};

struct SUserLvUpExp
{
	UINT16 un16UserLv;
	UINT32 un32LvUpExp;
};

struct SRunesSlotCfg{
	EObjectType eOT;
	UINT8		un8Level;
	UINT8		un8OpenLevel;
};

struct SRunesComposeInfo{
	SRunesComposeInfo():eOTMin((EObjectType)0)
		, eOTMax((EObjectType)0){
	}
	EObjectType eOTMin;
	EObjectType eOTMax;
};

struct SUserLvUpExpCfg
{
	UINT16 un16UserLv;
	UINT32 un32LvUpExp;
};

enum GoodsType
{
	eGoodsType_None,
	eGoodsType_Hero,
	eGoodsType_Skin,
	eGoodsType_Rune
};

enum EHeroKind
{
	eHeroKind_None = 0,
	eHeroKind_DPS = 1,
	eHeroKind_Magic = 2,
	eHeroKind_ASS = 3,
	eHeroKind_Tank = 4,
};

enum ESkinType{
	eSkin_None = 0,
};

enum EConsumeType
{
	eConsumeType_Free = 0,
	eConsumeType_Gold = 1,
	eConsumeType_Diamond = 2,
};

enum LobbyType
{
	LobbyNull = -1,
	LobbyEle = 0, //初级场
	LobbyMid = 1, //中级场
	LobbyHey = 2, //高级场
	LobbyTaW , 
	LobbyPve1 ,
	LobbyTowerDef,
	LobbyPvp007 ,
	LobbyGuid_1 ,
	LobbyGuid_2,
	LobbyGuid_3 ,
	LobbyGuid_4 ,
};

struct SUserBuyGoods
{
	UINT32  n32GoodId;
	UINT32  n32GoodsType;
	UINT32  nTotal; 

	SUserBuyGoods() {memset(this,0,sizeof(*this));}
};

struct SDBCfg 
{
	UINT32	un32DBHostPort;
	CHAR	aszDBHostIP[c_n32DefaultNameLen];
	CHAR	aszDBUserName[c_n32DefaultNameLen * 2];
	CHAR	aszDBUserPwd[c_n32DefaultNameLen * 2];
	CHAR	aszDBName[c_n32DefaultNameLen * 2];
	CHAR	aszUpgradeScriptDir[c_n32DefaultNameLen * 20];

	SDBCfg() { memset(this,0,sizeof(*this)); }
};


enum EMailCurtState
{
	eMailState_None = 0, 
	eMailState_New, 
	eMailState_LookedButNotGotGift,
	eMailState_Look, 
	eMailState_Del,
};
 

struct MailDBData 
{
	INT64		objIdx;
	INT32		mailId;
	INT32		channelId; 
	EMailCurtState		curtState;
	EMailType   mailType; //邮件类型 
	INT64		n64CreateTime ;//邮件发送时间(mCreateTime创建时间)
	INT64		n64EndTime ; //邮件过期时间 

	string		mailTitle;
	string		mailContent; 
	string		mailGift;	//邮件礼包type:key:value eg:1:1:1000;2:2:1000;3:goodsid:1;3:goodsid:2;
	string		szSender; 
	string		mCreateTime;
	string		mEndTime; 
	 
	bool		bIfPerDel;	 
	bool		bIfNewMail;  

	MailDBData():objIdx(0), mailId(0), channelId(0), curtState(eMailState_None), mailType(eMailType_None), n64CreateTime(0),n64EndTime(0){}
	~MailDBData(){}
};

struct UserMailDBData 
{
	INT32		mailId;
	EMailCurtState mState;
	UserMailDBData():mailId(0), mState(eMailState_None){}
	UserMailDBData(INT32 mailId, EMailCurtState mState):mailId(mailId), mState(mState){}
};

struct SUserOfflineMsg
{
	UINT64				sSenderObjIdx;
	UINT64				sReceiverObjIdx;
	TIME_MILSEC			tMsgSendTime;
	CHAR				szMessageInfo[c_n32DefaultNameLen * 5];
};
	class	CFunction{

	public:
		static SGUID_KEY		MakeGUID(EObjectType eObjectType, UINT64 un64ObjIndex)
		{
			return un64ObjIndex;
		}

		static INT32	SplitInt(const CHAR *pszStr, INT32 *pn32IntBuff, INT32 n32IntBuffSize){
			if (NULL == pszStr || NULL == pn32IntBuff || n32IntBuffSize <= 0){
				return 0;
			}

			const char *pszTemp = pszStr;
			INT32 n32FindIntNum = 0;

			while (n32FindIntNum < n32IntBuffSize && *pszTemp != '\0'){
				//go to the first number char.
				while (*pszTemp != '\0'){
					if (*pszTemp >='0' && *pszTemp <= '9'){
						break;
					}else if (*pszTemp == '-' && (*(pszTemp + 1) >='0' && *(pszTemp + 1) <= '9')){
						break;
					}
					pszTemp++;
				}

				pn32IntBuff[n32FindIntNum++] = atoi(pszTemp);

				//go to the end of the number.
				while (*pszTemp != '\0'){
					if (*pszTemp >='0' && *pszTemp <= '9'){
						pszTemp++;
					}else if (*pszTemp == '-' && (*(pszTemp + 1) >='0' && *(pszTemp + 1) <= '9')){
						pszTemp++;
					}else{
						break;
					}				
				}
			}

			return n32FindIntNum;
		}

		static INT32	SplitFloat(const CHAR *pszStr, FLOAT *pfFloatBuff, INT32 n32FloatBuffSize){
			if (NULL == pszStr || NULL == pfFloatBuff || n32FloatBuffSize <= 0){
				return 0;
			}

			const char *pszTemp = pszStr;
			INT32 n32FindIntNum = 0;

			while (n32FindIntNum < n32FloatBuffSize && *pszTemp != '\0'){
				//go to the first number char.
				while (*pszTemp != '\0'){
					if (*pszTemp >='0' && *pszTemp <= '9'){
						break;
					}				
					else if (*pszTemp == '-' && (*(pszTemp + 1) >='0' && *(pszTemp + 1) <= '9')){
						break;
					}
					else if (*pszTemp == '.' && (*(pszTemp + 1) >= '0' && *(pszTemp + 1) <= '9')){
						break;
					}
					pszTemp++;
				}

				pfFloatBuff[n32FindIntNum++] = (FLOAT)atof(pszTemp);

				//go to the end of the number.
				while (*pszTemp != '\0'){
					if (*pszTemp >='0' && *pszTemp <= '9'){
						pszTemp++;
					}else if (*pszTemp == '-' && (*(pszTemp + 1) >='0' && *(pszTemp + 1) <= '9')){
						pszTemp++;
					}else if (*pszTemp == '.' && (*(pszTemp + 1) >= '0' && *(pszTemp + 1) <= '9')){
						pszTemp++;
					}else{
						break;
					}
				}
			}

			return n32FindIntNum;
		}

		static	bool	IfEnemy(INT32 n32LeftCamp, INT32 n32RightCamp){
			if (0 == n32LeftCamp || 0 == n32RightCamp || n32LeftCamp == n32RightCamp){
				return false;
			}
			if (n32LeftCamp % 2 == n32RightCamp %2 ){
				return false;
			}
			return true;
		}

		static	bool IfImpact(const CVector3D &crsPos1, float fRadius1, const CVector3D &crsPos2, float fRadius2){
			float fMiniImpactDist = fRadius1 +fRadius2;
			CVector3D cTempPos = crsPos2;
			cTempPos = cTempPos - crsPos1;
			cTempPos.m_fY = 0;
			if (cTempPos.GetAbs(cTempPos.m_fX) + cTempPos.GetAbs(cTempPos.m_fY) + cTempPos.GetAbs(cTempPos.m_fZ) <= fMiniImpactDist){
				return true;
			}
			float fLength = cTempPos.length();
			if (fLength <= fMiniImpactDist){
				return true;
			}
			return false;
		}

		static	FLOAT	GetAbountDistXZ(const CVector3D &crsLeft, const CVector3D &crsRight){
			CVector3D cDist = crsLeft - crsRight;
			cDist.m_fY = 0;
			return cDist.length();
		}

		static EIntDir	GetIntDirByPoint(INT32 n32X1, INT32 n32Z1, INT32 n32X2, INT32 n32Z2){
			EIntDir eIntDir = (EIntDir)g_Dir64To8(g_GetDirIndex(n32X1, n32Z1, n32X2, n32Z2));
			return eIntDir;
		}

		static	EIntDir	GetIntDirByIndx(INT32 n32BeginX, INT32 n32BeginZ, INT32 n32EndX, INT32 n32EndZ){
			INT32 n32XDiff = n32EndX - n32BeginX;
			INT32 n32ZDiff = n32EndZ - n32BeginZ;
			return GetIntDirByIndxDiff(n32XDiff, n32ZDiff);
		}

		static	EIntDir	GetIntDirByIndxDiff(INT32 n32XDiff, INT32 n32ZDiff){
			EIntDir eDir = eIntDir_Invalid;
			if (0 == n32XDiff){
				if (0 > n32ZDiff){
					eDir = eIntDir_Below;
				}else if (0 < n32ZDiff){
					eDir = eIntDir_Up;
				}
			}else if (0 > n32XDiff){
				if (0 > n32ZDiff){
					eDir = eIntDir_BelowLeft;
				}else if (0 == n32ZDiff){
					eDir = eIntDir_Left;
				}else if (0 < n32ZDiff){
					eDir = eIntDir_UpLeft;
				}
			}else if (0 < n32XDiff){
				if (0 > n32ZDiff){
					eDir = eIntDir_BelowRight;
				}else if (0 == n32ZDiff){
					eDir = eIntDir_Right;
				}else if (0 < n32ZDiff){
					eDir = eIntDir_UpRight;
				}
			}
			return eDir;
		}

		static INT32	GetIntDiffByIntDir(EIntDir eIntDir, INT32 &rn32XDiff, INT32 &rn32ZDiff){
			if (eIntDir_UpLeft == eIntDir){
				rn32XDiff = -1;
				rn32ZDiff = 1;
			}else if (eIntDir_Up == eIntDir){
				rn32XDiff = 0;
				rn32ZDiff = 1;
			}else if (eIntDir_UpRight == eIntDir){
				rn32XDiff = 1;
				rn32ZDiff = 1;
			}else if (eIntDir_Left == eIntDir){
				rn32XDiff = -1;
				rn32ZDiff = 0;
			}else if (eIntDir_Right == eIntDir){
				rn32XDiff = 1;
				rn32ZDiff = 0;
			}else if (eIntDir_BelowLeft == eIntDir){
				rn32XDiff = -1;
				rn32ZDiff = -1;
			}else if (eIntDir_Below == eIntDir){
				rn32XDiff = 0;
				rn32ZDiff = -1;
			}else if (eIntDir_BelowRight == eIntDir){
				rn32XDiff = 1;
				rn32ZDiff = -1;
			}else{
				rn32XDiff = 0;
				rn32ZDiff = 0;
			}
			return eNormal;
		}

		static double GetPointDistance(const CVector3D &crcP1, const CVector3D &crcP2)   
		{  
			return sqrt((crcP1.m_fX-crcP2.m_fX)*(crcP1.m_fX-crcP2.m_fX)+(crcP1.m_fZ-crcP2.m_fZ)*(crcP1.m_fZ-crcP2.m_fZ)); 
		}

		static double GetNearestDistance(const CVector3D &crcPA, const CVector3D &crcPB, const CVector3D &crcP3)
		{
			double a,b,c;  
			a=GetPointDistance(crcPB,crcP3);  
			if(a<=0.00001){
				return 0.0f;
			}		
			b=GetPointDistance(crcPA,crcP3);
			if(b<=0.00001){
				return 0.0f;
			}
			c=GetPointDistance(crcPA,crcPB);  
			if(c<=0.00001) {
				return a;//如果PA和PB坐标相同，则退出函数，并返回距离   
			}
			if(a*a>=b*b+c*c){//--------图3--------   
				return b;      //如果是钝角返回b   
			}
			if(b*b>=a*a+c*c){//--------图4-------   
				return a;      //如果是钝角返回a   
			}
			double l=(a+b+c)/2;     //周长的一半   
			double s=sqrt(l*(l-a)*(l-b)*(l-c));  //海伦公式求面积，也可以用矢量求   
			return 2*s/c;  
		} 

		static INT32  GetGameRandomN(std::default_random_engine & sGenerator, INT32 beginN,  INT32  endN)
		{ 	 
			std::uniform_int_distribution<int> distribution(beginN, endN);

			INT32  nRet = distribution(sGenerator);
			return nRet;
		}

		static INT64 GetCurrentUTCTime(){
			static LARGE_INTEGER s_frequency;
			static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
			if (s_use_qpc) {
				LARGE_INTEGER now;
				QueryPerformanceCounter(&now);
				return (1000LL * now.QuadPart) / s_frequency.QuadPart;
			} else {
				return GetTickCount();
			}
		}

		//2014/5/9 17:41:12
	static	std::string GetCurtFormatTime(time_t t )
		{	 
			time_t curt; 
			struct tm * ptm;
			if (t > 0)
			{
				curt = t;
			}else
			{
				time(&curt); 
			}

			ptm = localtime(&curt);

			char szTime[32]={0};
			sprintf(szTime,"%d-%d-%d %d:%d:%d",ptm->tm_year + 1900,ptm->tm_mon + 1, ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);

			return string(szTime);
		}
	static	void GetYMDHMSInfo(std::string & str,const char* signd, INT32 &p1,INT32& p2, INT32& p3)
		{
			std::vector<std::string> tVec;   
			boost::algorithm::split(  tVec,str, boost::algorithm::is_any_of(signd) ); 
			if (tVec.size() != 3)
			{
				return;
			}
			p1= atoi(tVec[0].c_str());
			p2= atoi(tVec[1].c_str());
			p3= atoi(tVec[2].c_str()); 
		}
		//xxx-xxx-xxx hh:mm:ss
	static	INT64  FormatTime2TimeT(const char * dt)
		{
			if(NULL == dt  )
			{
				return 0;
			}
			std::string timeStr(dt);

			INT32 y,m,d,mh,ms,mss;
			std::vector<std::string> tVec; 
			boost::algorithm::split( tVec, timeStr, boost::algorithm::is_any_of(" ") ); 
			if (tVec.size() != 2)
			{
				return 0;
			}
			GetYMDHMSInfo(tVec[0],"-",y,m,d);
			GetYMDHMSInfo(tVec[1],":",mh,ms,mss);
			CTime  t(y,m,d,mh,ms,mss);  

			return t.GetTime();
		}

		//xxx-xx-xx hh:mm:ss
	static	bool CompareTime(time_t st, const char * dt)
		{
			if(NULL == dt || st < 1)
			{
				return false;
			}
#if 0
			std::string  smt = GetCurtFormatTime(st);

			char cdump;

			stringstream mystream(smt);

			mystream >> y >> cdump >> m >> cdump >> d >> cdump >> h >> cdump >> ms >> cdump >> mss;
			CTime  t1(y,m,d,h,ms,mss);

			mystream.clear();
			mystream.str(dt);
			mystream >> y >> cdump >> m >> cdump >> d >> cdump >> h >> cdump >> ms >> cdump >> mss;
			CTime  t2(y,m,d,h,ms,mss);

			if( ( t2 - t1).GetTotalSeconds() > 0)
			{
				return true ;
			}
#endif
			INT64 crt = FormatTime2TimeT(dt);

			if( crt >= st  )
			{
				return true ;
			}
			return false;		
		} 

		
	};

	//字符装换: 
	template<class T>
	void addto_string(std::string &result, const T &t)
	{
		ostringstream myoss;
		myoss << t;
		result += myoss.str();
	}  

	enum EMapType{
		eMT_Normal = 0,
	};

	enum EMapAIType{
		eMAIT_Normal = 0,
	};
	enum eMapObjType
	{
		eObj_Build_HomeBase		= 0, 
		eObj_Build_Arrowhead,		//箭塔

		eObj_Build_Shop,

		eObj_Ohter = 20,


		eBuild_End,
	};

	struct SMapObjInfo
	{
		UINT32	un32ObjIdx;
		eMapObjType eType; 

		CVector3D cPos;
		CVector3D cDir; 
	};

	

	struct SCellInfo{
		UINT16 un16XCellIdx;
		UINT16 un16ZCellIdx;
	};

	
	
//////////////////////////////////出兵相关配置/////////////////////////////////////////
	struct SHeroBornPos 
	{ 
		CVector3D bPos;
		CVector3D bPosDir;
		CVector3D bRelivePos;

		SHeroBornPos(){bPos.Zero();bPosDir.Zero();bRelivePos.Zero();};
		~SHeroBornPos(){}
	};

	 
	struct SBornChariotCfg
	{
		INT32	charSolderBornTimes;	//出工程兵的波数
		INT32	chariotSolderIdx;	//工程车编号

		SBornChariotCfg() {memset(this,0,sizeof(*this));}
	};

	struct SSuperSolderCfg 
	{
		INT32				 pathId;					//出超级兵对应的路径
		std::vector<INT32>   altarIdx ;					//祭坛编号
		std::vector<INT32>   superSolderIdx ;			//超级兵编号

		SSuperSolderCfg() { memset(this,0,sizeof(*this));}

		void Set(SSuperSolderCfg &t )
		{
			altarIdx = t.altarIdx;
			superSolderIdx = t.superSolderIdx;
		}
	};

	struct SSolderGoWayPointCfg
	{
		INT32		camp;
		std::vector<CVector3D> vecPoint;
	};

	struct SMapSolderInfoCfg 
	{
		INT32	baseBuildIdxCfg;			//基地编号
		//INT32	campIdCfg;					//阵营
		INT32	totalSolderCfg;				//本次出兵总数
		INT32	solderTimeSpaceCfg;			//出兵轮数时间间隔
		INT32	everyTimeSpace;				//单个出兵时间间隔

		std::vector<INT32>	solderIdxCfg;				//小兵编号

		SBornChariotCfg	mChariotSolderCfg;	//出工程车配置 
		SSuperSolderCfg  mSuperSolderCfg;	//超级兵配置 

		CVector3D  bornPosCfg;					//出兵坐标
		CVector3D  bornDirCfg;					//出兵朝向 
		SMapSolderInfoCfg(){memset(this,0,sizeof(*this));}

		void Set(SMapSolderInfoCfg & t)
		{
			baseBuildIdxCfg = t.baseBuildIdxCfg;
			//campIdCfg = t.campIdCfg;
			totalSolderCfg = t.totalSolderCfg;
			solderTimeSpaceCfg = t.solderTimeSpaceCfg;
			everyTimeSpace = t.everyTimeSpace;
			bornPosCfg = t.bornPosCfg;
			bornDirCfg = t.bornDirCfg;
			for (int i=0; i< t.solderIdxCfg.size(); i++)
			{
				solderIdxCfg.push_back(t.solderIdxCfg[i]);
			}

			mChariotSolderCfg = t.mChariotSolderCfg;

			mSuperSolderCfg.Set(t.mSuperSolderCfg);
		}
	};

	struct SMapCurtBornSolderInfo
	{
		INT32   curtBornSolderPos;			//当前出兵的位置
		INT32	curtBornSolderTimes;		//当前出兵波数

		bool	bSuperState;				 //出超级兵状态 
		
		std::vector<INT32>   curtBrokenAltarIdx; //当前打爆的祭坛配置	

		SMapCurtBornSolderInfo(){memset(this,0,sizeof(*this));}

		~SMapCurtBornSolderInfo(){curtBrokenAltarIdx.clear();}

		void Set(SMapCurtBornSolderInfo & t)
		{
		   curtBornSolderPos = t.curtBornSolderPos;
		   curtBornSolderTimes = t.curtBornSolderTimes;
		   bSuperState = t.bSuperState;

		   for (int i=0; i < t.curtBrokenAltarIdx.size(); i++)
		   {
			   curtBrokenAltarIdx.push_back(t.curtBrokenAltarIdx[i]);
		   }
		}
	};

	struct SAltarSolderCfg
	{
		INT32  pathId;					//祭坛所在路径
		INT32  bornSolderSpaceTime;		//祭坛兵出兵 时间间隔
		INT32  bornNum;					//祭坛每次出兵个数
		INT32  maxSolderNum;			//地图上祭坛兵最大存在数量(-1不限制)

		CVector3D bornPos;				//出兵坐标
		CVector3D bornDir;				//出兵朝向

		SAltarSolderCfg(){memset(this,0,sizeof(*this));}

		~SAltarSolderCfg(){bornPos.Zero();bornDir.Zero();};
	};

	//SS功能延迟
	enum  EGameFunDelay
	{
		eDelay_None = 0,
		eDelay_Solder = 1, //出兵延迟
		eDelay_PrimaryMonster,		//初级野怪
		eDelay_HightMonster,//高级野怪
		eDelay_AltarSolder,	//祭坛兵
	};

	struct SGameDelayTimeCfg
	{
		INT64	mSoldeDelay;				//小兵出生延迟
		INT64   mPrimaryWMMonsterDelay;		//初级野怪出生延迟
		INT64   mHighWMMonsterDelay;			//高级野怪出生延迟
		INT64   mAltarSolderDelay;			//祭坛出生延迟

		SGameDelayTimeCfg(){memset(this,0,sizeof(*this));}
	};

	struct SGameWildMonsterCfg
	{
		INT32    lv;//等级
		INT32    num;//出生数量
		INT32	 refreshTime;		//死亡刷新时间
		bool	 bIfRandom;			//是否随机
		std::vector<INT32>	vecIdx;//野怪ID
		CVector3D   bornPos;		//出生坐标
		CVector3D   bornDir;		//出生朝向坐标

		SGameWildMonsterCfg(){vecIdx.clear(); bornPos.Zero(); bornDir.Zero(); }
		~SGameWildMonsterCfg(){vecIdx.clear(); bornPos.Zero(); bornDir.Zero();}
	};


	struct CurtAltarBornSolder
	{
		INT32	monsterId;		//当前野怪ID
		INT32	curtBornNum;	//当前出生兵数量
		INT64   nextBornTime;	//即将出兵时间

		INT32			 campId;	//路径所属阵营
		SAltarSolderCfg  mCfg;  //祭坛配置

		CurtAltarBornSolder(){memset(this,0,sizeof(*this));}
		~CurtAltarBornSolder(){}
	};

	struct CurtWildMonsterInfo
	{
		INT64	nextBornTimeCfg;
		INT64	nextBornTime;
		std::map<INT32/*monsterid*/,bool>  mapMonsterId;

		CurtWildMonsterInfo(){nextBornTimeCfg = 0;nextBornTime = 0; mapMonsterId.clear();}
		~CurtWildMonsterInfo(){mapMonsterId.clear();}
	};
	/////////////////////////////////新手引导/////////////////////////////////////////////

	//新手奖励类型
	enum EGuideAward
	{
		eAward_None = 0,
		eAward_Gold,
		eAward_Diamond,
		eAward_Other,
	};


	enum EGuideBornObjType
	{
		eGuideObjType_None = 0,
		eGuideObjType_Solder,		//小兵
		eGuideObjType_Monster,		//野怪
		eGuideObjType_Hero,			//英雄
	};
	enum EGuideRageObjType
	{
		eRage_None = 0,
		eRage_UserHero,	//玩家英雄
		eRage_AiHero,		//AI英雄
	};

	struct SGuideSolderCfg
	{
		INT32	  bornSpaceTimeCfg;	    //小兵之间时间间隔
		INT32	  bornAheadTimeCfg;		//每波出兵的时间间隔  

		INT32     campidCfg;			//阵营
		INT32	  closeTaskIdCfg;		//关闭该步骤任务id
		

		CVector3D bornPosCfg;			//出生点
		CVector3D bornDirCfg;			//出生方向 

		char	altaridVecCfg[32];//祭坛ID
		std::vector<CVector3D>	mGoPathPointCfg; //寻路点 
		std::vector<INT32/*objId*/>	vecObjIdCfg;//出生对象id

		SGuideSolderCfg(){memset(altaridVecCfg,0,sizeof(altaridVecCfg)); mGoPathPointCfg.clear();vecObjIdCfg.clear();}
		~SGuideSolderCfg(){}
	};

	enum EGuideHeroBornType
	{
		eHeroBornType_None = 0,
		eHeroBornType_Pos,		//配置出生点
		eHeroBornType_UserArround,		//玩家周围
	};
	struct SGuideHeroCfg 
	{
		INT32     objIdxCfg;
		INT32     campIdCfg;

		INT32     reliveTimeCfg;//复活时间

		bool	  ifRage;

		EGuideHeroBornType bornTypeCfg;

		//CVector3D bornPosCfg;
		//CVector3D relivePosCfg; 
		//CVector3D bornDirCfg; 
		
		std::vector<CVector3D>	mGoPathPointCfg; //寻路点 

		SGuideHeroCfg():objIdxCfg(0),campIdCfg(0),reliveTimeCfg(0),bornTypeCfg(eHeroBornType_None){mGoPathPointCfg.clear();} 
	};
	/* taskmanager.xml
		1,路径任务
		2,点击按钮任务
		3,击杀任务
		4,时间控制任务
		5,吸附任务
		6,游戏暂停任务
		7,物体显示任务
		8,奖励任务
		9,字幕 
		10,语音任务
		11,移动摄像机任务
		12,物体动画任务
		13,阻挡任务
		14,弹窗提示任务
		15,出兵
		16,出英雄
*/
	enum EChildTaskType
	{
		eTaskType_None = 0,
		eTaskType_Path,
		eTaskType_Button,
		eTaskType_Kill,
		eTaskType_TimeEvent,
		eTaskType_Straw,
		 eTaskType_Stop,
		 eTaskType_Display,
		 eTaskType_Award,
		 eTaskType_Tips,
		 eTaskType_Autio,
		 eTaskType_MoveRadio,
		 eTaskType_Animation,
		 eTaskType_Block,
		 eTaskType_ShowBox,
		 eTaskType_BornSolder,
		 eTaskType_BornHero,
	};
	//引导 操作类型：
	enum EGuidDotype
	{
		eDotype_None = 0,
		eDotype_CompTask, //完成任务
		eDotype_BuyGoods, //购买
		eDotype_BornSolder,//出兵
		eDotype_BornHero,//出英雄
		eDotype_NextStep,
	};
	struct SGuideBattleCompTask 
	{
		EChildTaskType			childType;//子类型(用于查找对应的文件)
		INT32					stepId;	//子类型编号

		SGuideBattleCompTask(){memset(this,0,sizeof(*this));}
		~SGuideBattleCompTask(){}
	}; 

	struct SGuideCurtBornSolder
	{
		INT32   curtBornSolderPos;			//当前出兵的位置 
		INT64	nextBornTime;				//下次出兵时间

		SGuideSolderCfg	  cfg;

		SGuideCurtBornSolder(){memset(this,0,sizeof(*this));}
		~SGuideCurtBornSolder(){}
	}; 

	 enum EGuideHurtObjType
	 {
		 eHurtObj_None = 0,
		 eHurtObj_Arrow,		//箭塔
		 eHurtObj_Altar,		//祭坛
		 eHurtObj_Base,			//主基地
	 };
	 enum EGuideAtkType
	 {
		 eAtkType_None = 0,
		 eAtkType_Solder,		//小兵
		 eAtkType_User,			//wanji9
		 eAtkType_All,			//所有
	 };
	 struct SGuideHurtBuildCfg
	 {
		EGuideHurtObjType  objTypeCfg;
		INT32			   objIdxCfg;
		EGuideAtkType		atkTypeCfg;

		SGuideHurtBuildCfg():objTypeCfg(eHurtObj_None),objIdxCfg(0),atkTypeCfg(eAtkType_None){}
		~SGuideHurtBuildCfg(){}
	 };

	 struct SGuideCurtBuildBeHurt
	 { 
		 bool	  bBeHurt;					//是否此时允许被攻击

		 SGuideHurtBuildCfg  cfg;

		 SGuideCurtBuildBeHurt() {memset(this,0,sizeof(*this));}
		 ~SGuideCurtBuildBeHurt(){}
	 };

	 struct CSGuideAward
	 {
		 INT32	taskId;
		 INT32  gold;
		 INT32  diamond;
		 INT32  heroid;
		 INT32  runeid;
		 INT32	skinid;
		 INT32  exp; 

		 CSGuideAward(){memset(this,0,sizeof(*this));}
		 ~CSGuideAward(){}
	 };
	 //新手引导地图类型
	 enum  EGuideMapType
	 {
		 eGuideMapType_None = 0,
		 eGuideMapType_First,
		 eGuideMapType_Second,
	 };

	/////////////////////////////////////////////新手结束/////////////////////////////////////////

	 enum ENoticeFlag
	 {
		 eFlag_None = 0,
		 eFlag_OnSale = 1,
		 eFlag_Notice = 2,
		 eFlag_Services = 3,
	 };

	 enum ENoticeState
	 {
		 eState_None = 0,
		 eState_Hot = 1,
		 eState_New = 2,
	 };

	 struct SNotice
	 {
		 //字段、属性、方法、事件
		 INT64	noticeID;//数据库主键
		 UINT32 id;
		 EUserPlatform platform;
		 string title;
		 ENoticeFlag flag;
		 ENoticeState state;
		 int priority;
		 string msg;
		 UINT64 star_time;
		 UINT64 end_time;

		 SNotice():noticeID(0){
			 platform = ePlatform_PC;
			 title = "";
			 flag = eFlag_None;
			 state = eState_None;
			 priority = 0;
			 msg = "";
			 star_time = 0;
			 end_time = 0;
		 }
	 };
 

	 ////////////////////////////////////游戏日志//////////////////////////////////////
	 //日志类型
	#define  LOG_SIGN "#"  //字段分隔符号
	#define LOG_SIGN_FIRST ":" //组内字段分隔符号
	#define LOG_SIGN_SECOND ";" //小组字段分隔符号
	 #define LOG_SIGN_LEFT "(" //小组字段分隔符号
	 #define LOG_SIGN_RIGHT ")" //小组字段分隔符号
	 enum EGameLog
	 { 
		 eLog_None = 0,
		 eLog_Register, //Register
		 eLog_HeadUse,	//HeadUse
		 eLog_Login,	//Login
		 eLog_Logout,	//Logout

		 eLog_Guide = 10,	//Guide
		 eLog_Daily,	//DailyAward
		 eLog_Exchange,//eLog_Exchange
		 eLog_ChangeName,//eLog_ChangeName

		 eLog_CreateMatch=14,//CreateMatch
		 eLog_LogoutMatch,//LogouMatch
		 eLog_MatchEnd,	//FaceAI
		 eLog_CreateHouse, //CreateHouse
		 eLog_AddHouse,		//AddHouse
		 eLog_HouseStart,	//HouseStart

		 //buy
		 eLog_BuyHouse,		//BuyHouse
		 eLog_BuySkin,		//BuySkin
		 eLog_BuyRune,		//BuyRune
		 eLog_BuyGoods,		//BuyGoods
		 //use
		 eLog_RuneUse,		//RuneUse
		 eLog_RuneUnUse,		//RuneUnUse
		 eLog_RuneCompose,	//RuneCompose
		 eLog_RuneWashing,	//RuneWashing
		 //friend
		 eLog_FriendAdd,	//FirendAdd
		 eLog_FriendDel,	//FriendDel
		 eLog_EnemyFriendAdd,//EnemyFriendAdd
		 eLog_EnemyFriendDel,//EnemyFriendDel 
		 //个人数据
		 eLog_ChangeUseName, //ChagneUseName
		 eLog_UseInfoLogout, //UseInfoLogout
		 //chat
		 eLog_Chat,			//Chat
		 //UIEvent
		 eLog_UIEvent,		//UIEvent
		 //select hero
		 eLog_ChoseHero,	//ChoseHero
		 eLog_ChoseRune,	//ChoseRune
		 eLog_ChoseSkin,	//ChoseSkin
		 //掉线
		 eLog_UserDiscon = 40,//UserDiscon
		 eLog_UserRecon,		//UserRecon
		 eLog_KickUser,			//KickUser

		 //battle data 
		 eLog_BattleStart = 60,		//BattleStart 
		 eLog_BattleBuy,			//BattleBuy
		 //skill 71-80
		 eLog_BattleSkill = 71,			//		BattleSkill
		 eLog_BattleSkillHurt,			//BattleSkillPhysicalHurt  
		 eLog_BattleSkillEquip,				//BattleSkillEquip

		 //kill
		 eLog_BattleKillSolder=81,				//BattleKillSolder
		 eLog_BattleKillMonster,				//BattleKillMonster
		 eLog_BattleKillHero,					//BattleKillHero,
		 eLog_BattleKillBuild,					//BatlleKillBuild
		 eLog_BattleStrawMonster,				//BattleStrawMonster
		 eLog_BattleAltarSolder,				//BattleAltarSolder
		 eLog_BattleHeroMounKill,				//BattleHeroMounKill  英雄多杀

		 //disc
		 eLog_BattleRecon = 95,						//BattleRecon
		 eLog_BattleUserOffLine,					//BattleKickUser

		 //other
		 eLog_BattleEnd = 110,					//BattleEnd
		 eLog_BattleSelGoods,					//BattleSelGoods
		 eLog_BattleExpersion,					//曝气

		 eLog_BattleGuideStart = 150,					//新手引导开始
		 eLog_BattleGuideCompStep,						//新手完成的步奏		
		 eLog_BattleGuideEnd,							//新手引导结束
	 };

	 
	  ////////////////////////////////////游戏日志结束//////////////////////////////////////