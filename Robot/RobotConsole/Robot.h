#pragma once
#include <boost/atomic.hpp>
#include <vector>
#include <map>
#include <random>
#include "RobotNPC.h"
#include "Player.h"
#include "Hero.h"
#include "../ELogging/ELogging/ThreadSafeQueue.h"
#include <boost/atomic.hpp>

enum EBattleMatchType
{
	eMatchType_None,
	eMatchType_VsAI,//人机对战(己方玩家敌方机器人)//
	eMatchType_Nomal,//普通对战//
	eMatchType_Tianti,//天梯对战// 
	eMatchType_Total, 
};

enum RobotBattleState
{
	RBS_None,//你可以选择匹配，房间，新手引导，也可等待其它玩家（如果你喜欢被动）
	RBS_InRoomAsMaster,//你可以选择开启房间（队员必须全准备好）
	RBS_InRoomAsMasterStart,//等待进入战场吧..
	RBS_InRoomAsPlayer,//你在房间中了，请选择准备房间
	RBS_InRoomAsPlayerReady,//你已经准备好了，你可以取消准备、离开房间
	RBS_InTeamAsMaster,//你可以要求小伙伴进来，也可以一人上路
	RBS_InTeamAsMasterMatch,//前方就是战场，弱者可以选择离开
	RBS_InTeamAsPlayer,//你被组队邀请进来，你可以离开的
	RBS_InTeamAsPlayerMatch,//前方就是战场，如果队友太弱，赶快逃吧...
	RBS_InBattleNow,//开始战斗吧，记住你是英雄
};

enum RobotBattleType
{
	RBT_None,
	RBT_Room,
	RBT_Match,
	RBT_Guide,
};

// 扮演角色，如果长时间未进入战场，重新扮演角色
enum RobotRole
{
	RRIR_AsIdle,
	RRIR_AsRoomMaster,
	RRIR_AsRoomMate,
	RRIR_AsTeamMaster,
	//RRIR_AsTeamMate,
	RRIR_AsTotal,
};

enum ESSBattleState
{
	eSSBS_SelectHero = 0,
	eSSBS_SelectRune,
	eSSBS_Loading,
	eSSBS_Playing,
	eSSBS_Finished,
	eSSBS_Invalid = 0xFFFF,
};

enum RobotLoginState
{
	RLS_None,
	RLS_LsConnecting,
	RLS_LsConnected,
	RLS_LsAskLogin,
	RLS_LsAskLoginError,
	RLS_LsHadGetBsAddress,
	RLS_BsConnecting,
	RLS_BsConnected,
	RLS_BsAskLogin,
	RLS_BsAskLoginRet,
	RLS_BsHadGetGsAddress,
	RLS_GsConnecting,
	RLS_GsConnected,
	RLS_GsAskLogin,
	RLS_GsHadGetBaseInfo,
};

enum RobotAskState
{
	RS_AskNone,
	RS_AskCompleteUserInfo,//补充用户信息
	RS_AskCompleteUserInfoFailed,//补充信息失败
	RS_AskCompleteUserInfoOk,//补充信息成功
	RS_AskReEnterRoom,//请求重进房间
	RS_AskReEnterRoomFailed,//没有房间
	RS_AskMatchTeamList,//请求匹配组队
	RS_AskMatchTeamListFailed,//没有组队
	RS_AskCreateMatchTeam,//创建匹配队伍
	RS_AskCreateMatchTeamFailed,//创建匹配队伍失败
	RS_AskStartMatch,//开始匹配
	RS_AskStartMatchFailed,//开始匹配失败
	RS_AskRoomList,//房间列表
	RS_AskRoomListFailed,//请求房间列表失败
	RS_AskCreateRoom,//创建房间
	RS_AskAddRoom,//加入房间
	RS_AskLeaveRoom,//离开房间
	RS_AskReadyRoom,//准备房间
	RS_AskCancelReadyRoom,//取消准备
	RS_AskStartRoom,//开始房间
	RS_AskStartRoomFailed,//开始房间失败
	RS_AskChangeRoomSeat,//切换房间位置
	RS_AskEnterBattle,//加入战斗
	RS_AskEnterBattleFailed,//加入战斗失败
	RS_AskGMCmd,
	RS_AskTryToSelectHero,//尝试选择英雄
	RS_AskTryToSelectHeroFailed,//
	RS_AskTryToSelectHeroOk,//
	RS_AskSelectHero,//选择英雄
	RS_AskSelectHeroFailed,
	RS_AskSelectHeroOk,
	RS_AskLoadMapComplete,//载图完成
	RS_AskEnd,
};

namespace ReBot
{
	class CClientGameObject;
	class CRebotHero;
	class CRobotNPC;
	class CRobotPlayer;

	class CCClient
	{
		friend class CRobotMgr;

	public:
		CCClient(CRobotMgr* pMgr,UINT32 robotId);
		~CCClient();

	public:
		CRobotMgr* m_pRobotMgr;//管理器
		UINT32 m_un32RobotID;//机器人编号
		INT8 m_n8RobotSex;//性别
		UINT32 m_un32HeaderID;//头像
		INT32 m_n32NSID;//网络编号
		INT32 m_n32SSID;//SSID
		EServerLinkType m_SLinkState;//当前连接(LS,BS,GS)
		RobotLoginState mLoginState;//登录状态
		RobotAskState m_eAskState;//请求状态
		ESSBattleState m_eServerBattleState;//服务器战斗状态
		DWORD m_ServerStartTime;
		boost::atomic_bool m_bConnected;//是否已连接
		boost::atomic_bool m_bSendConnect;//是否已发送连接
		SGUID_KEY m_UserGUID;//用户guid
		UINT32 m_MapID;//上线初始化，战场邀请//
		UINT64 m_un64BattleID;//上线初始化，战场邀请
		RobotBattleType m_BattleType;//用户自定义战场类型
		CRobotPlayer* m_pPlayer;//AI管理器
		UINT8 m_un8SeatPos;//房间位置号
		string m_sToken;//bs返回连接gs的密码
		string m_gateIp;//bs返回的gate ip
		UINT32 m_gatePort;//bs返回的gate port
		string m_szUserName;//用户名
		string m_szUserPwd;//密码
		string m_szNickName;//昵称
		string m_szUserNameRet;//GS返回的用户名
		map<UINT32,RobotTimes> m_RoomCounter;//房间计数器
		std::vector<UINT32>	m_HeroVec;//可选英雄列表
		GOMap m_pClientGameObjectMap;//游戏对象(断线、战场结束清理)
		RobotRole m_Role;
		map<UINT32,SRoomInfo> mRoomlist;//缓存房间列表
		bool mHadOnce;
		DWORD m_AskRoomListTime;
		DWORD m_AskAddRoomTime;
		DWORD m_RoomMasterWaitTime;
		DWORD m_DisConnectTime;
		map<UINT32,STeamInfo> mTeamList;//队伍信息
		DWORD mTeamID;
		INT32 m_LastErrorCode;

	public:
		void		SetRobotMgr(CRobotMgr*	pRobotMgr) { m_pRobotMgr = pRobotMgr; }
		void		Connect();//网络连接
		void		EnterBattle(DWORD curTime);//进入战场
		void		PlayBattle(DWORD curTime);//战场玩法
		void		FinishBattle(DWORD curTime);//结束战场
		bool		HandleMsgFalter(INT32 n32NSID, const char* pMsg, int n32MsgLength, int n32MsgID);
		INT32		HandleMsg(INT32 n32NSID, const char* pMsg, int n32MsgLength, int n32MsgID);
		void		OnHeartBeat(DWORD curTime);
		CClientGameObject* GetGOByGUID(const SGUID_KEY& guid);
		GOMap&		GetGOMap(){return m_pClientGameObjectMap;}
		void		EraseObj(const SGUID_KEY& guid);
		void		ClearAllGO();
		std::string GetRandStr(INT32 n32Len);
		std::string GetRandStrFromNum(INT32 n32Len);

	public:
		INT32		OnNetConnected(INT32 n32NetSessionID);
		INT32		OnNetRealConnected(INT32 n32NetSessionID);
		INT32		OnNetDisconnected(INT32 n32NetSessionID);
		INT32		EmsgToLs_AskLogin(UINT platform,string uin,string sessionid);
		INT32		EmsgToBs_AskLogin(UINT platform,string uin,string sessionid);
		INT32		EmsgToGs_AskLogin(UINT platform,string name,string pass,string equip); 
		void		NetworkClose(EServerLinkType type);

	public:
		INT32		AskCompleteUserInfo(const CHAR *szNickname, UINT32 un32HeaderID, INT8 n8Sex);
		INT32		AskReEnterRoom();//请求重进房间
		INT32		AskMatchTeamList();//请求匹配列表

	public:
		INT32		AskCreateMatchTeam(UINT32 mapid, EBattleMatchType matchType);
		INT32		AskStartMatch();
		INT32		AskRoomList();//房友策略选择
		INT32		AskCreateRoom();//房主策略选择
		INT32		AskAddRoom(UINT32 room);
		INT32		AskStartRoom();
		INT32		AskLeaveRoom();
		INT32		AskChangeRoomSeat(INT32	n32NewSeat);
		INT32		AskReadyRoom();
		INT32		AskCancelRoom();

	public:
		INT32		AskEnterBattle(UINT64 un64BattleID);
		INT32		AskGMCmd(const char* gmCmd);//GM命令
		INT32		AskTrySelectHero();
		INT32		AskSelectHero();
		INT32		AskLoadMapComplete();
		INT32		AskMoveDir(const CVector3D& cDir);
		INT32		AskBackToHome();
		INT32		AskMovePos(const CVector3D& cPos);
		INT32		AskStopMove();
		INT32		AskLockTar(const SGUID_KEY& sGUID);
		INT32		AskBuyGoods(INT32 n32Goods);
		INT32		AskUseGoods(UINT8 un8Pos);
		INT32		AskSellGoods(UINT8 un8Pos);
		INT32		AskMoveGoods(UINT8 un8FromPos, UINT8 un8ToPos);
		INT32		AskAutoAtk();
		INT32		AskUseSkill(UINT32 un32SkillID);
		INT32		AskAlwaysAutoAtk(INT8 n8Flag);
		INT32		AskReward();
		INT32		AskAbsorb();
		INT32		AskReportAltarStrawSolder(int n32MonsterID);

	private:
		INT32		OnMsgFromGS_GCAskPingRet(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 1
		INT32		OnMsgFromGS_NotifyUserBaseInfo(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 2
		INT32		OnMsgFromGS_GCAskRetError(const char* pMsg,int n32MsgLength,int n32MsgID);		//message 5
		INT32		OnMsgFromGS_IsOnSS(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsgFromGS_NotifyHeroList(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 7
		INT32		OnMsgFromGS_NotifyBattleBaseInfo(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 8
		INT32		OnMsgFromGS_NotifyBattleSeatPosInfo(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 9
		INT32		OnMsgFromGS_NotifyBattleStateChange(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 10
		INT32		OnMsgFromGS_NotifyCurBattleChange(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 11
		INT32		OnMsgFromGS_NotifyGamgObjectAppear(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 12
		INT32		OnMsgFromGS_NotifyGameObjectDisappear(const char* pMsg,int n32MsgLength,int n32MsgID);  //message 13
		INT32		OnMsgFromGS_NotifyGameObjectFreeState(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 14
		INT32		OnMsgFromGS_NotifyGameObjectRunState(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsgFromGS_NotifyGameObjectPrepareSkillState(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 18
		INT32		OnMsgFromGS_NotifyGameObjectReleaseSkillState(const char* pMsg,int n32MsgLength,int n32MsgID); //message 19
		INT32		OnMsgFromGS_NotifyGameObjectDeadState(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsgFromGS_NotifyBattleHeroInfo(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 23	
		INT32		OnMsgFromGS_NotifyBattleTryHeroInfo(const char* pMsg,int n32MsgLength,int n32MsgID);	//message 23	
		INT32		OnMsgFromGS_NotifySelectTimeOut(const char* pMsg,int n32MsgLength,int n32MsgID); //message 32
		INT32		OnMsgFromGS_NotifyBroadBuildingDestroyByWho(const char* pMsg,int n32MsgLength,int n32MsgID); 
		INT32		OnMsgFromGS_NotifyCurCP(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsgFromGS_NotifySkillInfo(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsgFromGS_NotifyFightPropertyInfo(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsgFromGS_NotifyHPInfo(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsgFromGS_NotifyMPInfo(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsgFromGS_NotifyLevelInfo(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsgFromGS_NotifyFury(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyLeaveBattleSuccess();
		INT32		OnMsg_NotifyGoodsInfo(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyHeroInfo(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyPassitiveSkillLoad(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyPassitiveSkillUnLoad(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyHPChange(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyMPChange(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyPing(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyExp(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyHeroRebornTimes(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyCurDeadTimes(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifySkillID(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32       OnMsg_NotifySdkLoginResult(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32       OnMsg_NotifyServerAddr(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32       OnMsg_OneClinetLoginCheckRet(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32       OnMsg_NotifyGateServerInfo(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32       OnMsg_NotifyAbsorbMonsterResult(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32       OnMsg_NotifyGoodsInfo(const char* pMsg,int n32MsgLength);
		INT32		OnMsg_NotifyRoomListRet(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyTeamBaseInfo(const char* pMsg,int n32MsgLength,int n32MsgID);
		INT32		OnMsg_NotifyTeamPlayerInfo(const char* pMsg,int n32MsgLength,int n32MsgID);
	};

}

