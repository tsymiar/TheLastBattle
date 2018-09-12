/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:stdafx.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:WangYanYu
* complete date		:November 30 2010
* summary			:
*
*/

#pragma once

#ifdef _WINDOWS

#define WIN32_LEAN_AND_MEAN

#pragma warning(disable:4996)

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif  // _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#ifdef _DEBUG
//#define new DEBUG_CLIENTBLOCK
//#endif  // _DEBUG

#include <windows.h>
#include <winsock2.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <Mswsock.h>
#include <process.h>

#ifndef WINVER                 
#define WINVER 0x0600          
#endif

#ifndef _WIN32_WINNT           
#define _WIN32_WINNT 0x0600     
#endif

#ifndef _WIN32_WINDOWS         
#define _WIN32_WINDOWS 0x0410 
#endif

#ifndef _WIN32_IE               
#define _WIN32_IE 0x0700        
#endif


#else

#include <termios.h>
#include <term.h>
#include <curses.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <memory.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <curses.h>
#include <dirent.h>
#include <execinfo.h>
#include <signal.h>
#include <exception>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dlfcn.h>

#endif

#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

#include <atlconv.h>
#include "..\..\ucore\include\net\ucnet.h"
using namespace UCODE;
#include "Net\RobotNetSessionMgr.h"

#include "../../CommonCPlus/Include/FBAll_Const.h"
#include "../../ThirdFunc/LuaPlus/LuaPlus.h"
#include "../../SceneServer/SSBattleMgr/stdafx.h"
#include "ELoggingHeader.h"
#include <boost/date_time/posix_time/posix_time.hpp> 
#include "boost/date_time/gregorian/gregorian.hpp"
using namespace SceneServer;
namespace ReBot
{
enum ERobotState{
	eRS_Free,
	eRS_Logining,
	eRS_Playing,
};

enum PlayingAct
{
	MoveDir = 0,
	StopMove,
	PrintInfo,
	WaitExit,
};

class RobotTimes
{	
public :
	RobotTimes(){
		Seat = 0; UserGuid = 0; AppearTimes = 0;
		IfMaster = false; IfReady = false; HeadId = 0;
		HeroId = 0; TryHeroId = 0;
	}
	UINT8       Seat;
	UINT32		UserGuid;
	UINT32		AppearTimes;
	BOOLEAN		IfMaster;
	BOOLEAN		IfReady;
	INT32		HeadId;
	INT32       HeroId;
	INT32		TryHeroId;
};

struct STeamInfo
{
	UINT8 Seat;
	string NickName;
	UINT32 headId;
	UINT32 level;
};

struct SRoomInfo
{
	UINT32 roomId;
	UINT32 ifPWD;
	UINT32 curCount;
	UINT32 maxCount;
};

class ServerInfo
{
public:
	int index;
	string name; 
	string addr;
	int port; 
};

enum EGOActionState{
	eOAS_Free,
	eOAS_Running,
	eOAS_Flying,
	eOAS_Swimming,
	eOAS_PreparingSkill,
	eOAS_ReleasingSkill,
	eOAS_PassiveState,
	eOAS_Controlled,
	eOAS_Reliving,
	eOAS_Dead,
	eOAS_End,
};

enum	EGOOrderCate{
	eGOOrderCate_None = 0,
	eGOOrderCate_Occupy,
	eGOOrderCate_Guard,
	eGOOrderCate_AbsorbMonster,
	eGOOrderCate_Building,
	eGOOrderCate_Attack,
	eGOOrderCate_MoveDir,
	eGOOrderCate_MoveTar,
	eGOOrderCate_UseSkill,
	eGOOrderCate_GoodsSkill,

	eGOOrderCate_End,
};

enum EGOOrderState{
	eGOOrderState_Cleared = 0,
	eGOOrderState_Running,
	eGOOrderState_Complete,
};

enum EGOOrderPriority{
	eGOOrderPriority_DefaultOrder = 0,
	eGOOrderPriority_UserAsk,
	eGOOrderPriority_BuffAsk,
};

enum EObjAttackModel{
	eObjAttackModel_Active = 0,
	eObjAttackModel_Passive,
};

enum EGameObjectCamp{
	eGOCamp_AllEnemy = -1,
	eGOCamp_AllPeace = 0,
	eGOCamp_1,
	eGOCamp_2,
	eGOCamp_3,
	eGOCamp_4,
	eGOCamp_5,
	eGOCamp_6,
};


enum	RobotStepTime
{
	Robot_StartRobot = 0,
	Robot_OnConnect,
	Robot_Login,
	Robot_CompleteUserInfo,
	Robot_QuickBattle,
	Robot_ReadyBattle,
	Robot_BeginBattle,
	Robot_SelectHero,
	Robot_ReportComplete,
	Robot_EnterBattle,
	Robot_MoveDir,
};

enum RobotMsg{
	eEC_ConnectSuccess = 1000,
	eEC_ConnectFailed,
};

struct RobotSkillData{
	const SSNewSkillCfg	*cpsCfg;
	ESkillState		eSkillState;
	TIME_MILSEC		tCooldownMilsec;
	TIME_MILSEC		tBeginTime;
	int		goodsIndex;
	void Clear(){
		cpsCfg = NULL;
		eSkillState = eSkillState_Free;
		tCooldownMilsec = 0;
		tBeginTime = 0;
		goodsIndex = -1;
	}
};
struct SGoodsRunTimeData{
	SGoodsRunTimeData() : sGoodsCfg(NULL)
		, tStateBeginMilsec(0)
		, sGoodsState(eGoodsState_Free)
		, n32Num(0){
			Clear();
	}

	SGoodsCfg*			sGoodsCfg;
	eGoodsState			sGoodsState;
	TIME_MILSEC			tStateBeginMilsec;
	INT32				n32Num;

	void Clear(){
		sGoodsCfg = NULL;
		sGoodsState = eGoodsState_Free;
		n32Num = 0;
		tStateBeginMilsec = 0;
	}
}; 

enum EPlayerState{
	ePlayerState_Free,
	ePlayerState_Running,
	ePlayerState_Attacking,
	ePlayerState_Pursuing,
};

enum EServerLinkType
{
	eServerLink_None,
	eServerLink_LS,
	eServerLink_BS,
	eServerLink_GS,
	eServerLink_Max,
};

class CRobotPlayer;
struct PlayerAIData{
	CRobotPlayer* pPlayer;
	CVector3D	sTarPos;
};

class SessionInfo
{
public:
	SessionInfo();
	SessionInfo(string addr,int port,SESSION_TYPE se):ipAddr(addr),ipPort(port),ipSession(se){};
	string ipAddr;
	int ipPort;
	SESSION_TYPE ipSession;
};

inline int RandInt(int Low, int Up)
{
	return int(double(rand()) / (RAND_MAX + 1) * (Up - Low + 1)) + Low;
}

const INT32 n32MaxRobotPlayerSkillNum = 16;
const INT32	n32AbsorbSkillID1 = 7;
const INT32	n32AbsorbSkillID2 = 8;
class CClientGameObject;
typedef std::map<SGUID_KEY, CClientGameObject*> GOMap;
const UINT32	c_un32MaxPendingLoginRobotNum = 50;
//INT64 GetUTCMiliSecond(){
//	using boost::gregorian::date;
//	using boost::posix_time::ptime;
//	using boost::posix_time::microsec_clock;
//	static ptime const epoch(date(1970, 1, 1));
//	return (microsec_clock::universal_time() - epoch).total_milliseconds();
//}
//float g_Height = 6000.0f;
}