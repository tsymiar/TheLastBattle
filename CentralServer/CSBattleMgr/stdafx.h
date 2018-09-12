// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息

#pragma warning(disable:4996)

#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <Mswsock.h>
#include <process.h>
#include <tchar.h>
#include <assert.h>
#include "async.h"

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
#include "WYYTools/Vector3D.h"
#include "Include/FBAll_Const.h"
#include	"boost/property_tree/ptree.hpp"
#include	"boost/property_tree/xml_parser.hpp"
#include	"boost/foreach.hpp"
#include "../../ELogging/ELoggingHeader.h"
#include	"SafeRefPtr.h"
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <MMSystem.h>
#include <atlconv.h>
#include <boost/date_time/posix_time/posix_time.hpp> 
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace std;


#include "../../ThirdFunc/MsgLists/GSToCS.pb.h"
#include "../../ThirdFunc/MsgLists/CSToGS.pb.h"
#include "../../ThirdFunc/MsgLists/SSToCS.pb.h"
#include "../../ThirdFunc/MsgLists/CSToSS.pb.h"
#include "../../ThirdFunc/MsgLists/GSToGC.pb.h"
#include "../../ThirdFunc/MsgLists/GCToCS.pb.h"
#include "../../ThirdFunc/MsgLists/RCToCS.pb.h"
#include "../../ThirdFunc/MsgLists/CSToRC.pb.h"
#include "../../ThirdFunc/MsgLists/CSToDB.pb.h"
#include "../../ThirdFunc/MsgLists/DBToCS.pb.h"
#include "../../ThirdFunc/MsgLists/ToLog.pb.h"
#include <boost/shared_ptr.hpp>
#include "../../ThirdFunc/MsgLists/ParseProto.h"
#include "..\..\Share\logger.h"
#include "..\..\ucore\include\net\ucnet.h"
#include "Net\CSNetSessionMgr.h"
#include "ELoggingHeader.h"
#include "SSElementArr.h"
#include "SSElementArr.inl"
#include "CSUserDBDataStruct.h"
// TODO: 在此处引用程序需要的其他头文件

namespace CentralServer{
	class ICSSSInfo
	{
	public:
		virtual	INT32					GetSSID() const = 0;
		virtual	INT32					GetNSID() const = 0;
		virtual	EServerNetState			GetSSNetState() const = 0;
		virtual void					SetSSNetState(EServerNetState netState) = 0;
		virtual	INT32					GetBattleNum() = 0;
		virtual	INT32					AddBattleNum(INT32 n32AddNum) = 0;
		virtual void					ResetPing() = 0;
	};

	class ICSGSInfo
	{
	public:
		virtual	INT32					GetGSID() const = 0;
		virtual	INT32					GetNSID() const = 0;
		virtual	EServerNetState			GetGSNetState() const = 0;
	};

	class ICSRCInfo
	{
	public:
		virtual	INT32					GetNSID() const = 0;
		virtual	EServerNetState			GetRCNetState() const = 0;
	};

typedef std::function<INT32(ICSSSInfo *cpiSSInfo, const char* pData, int n32DataLength)> PFSSMsgHandler;
typedef std::function<INT32(ICSGSInfo *cpiGSInfo, const char* pData, int n32DataLength)> PFGSMsgHandler;
typedef std::function<INT32(ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)> PFGCMsgHandler;
typedef std::function<INT32(ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength)> PFRCMsgHandler;


typedef std::map<INT32, PFSSMsgHandler> SSMsgHandlerMap;
typedef std::map<INT32, PFGSMsgHandler> GSMsgHandlerMap;
typedef std::map<INT32, PFGCMsgHandler> GCMsgHandlerMap;
typedef std::map<INT32, PFRCMsgHandler> RCMsgHandlerMap;

#define RedisSCallBack(realCallBack, holder) std::bind(realCallBack, holder, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

class CCSUserMgr;
CCSUserMgr*	GetCSUserMgrInstance();

class CCSKernel;
CCSKernel* GetCSKernelInstance();
#define CheckGSInfo(n32NSID) const ICSGSInfo *cpiGSInfo = GetCSKernelInstance()->GetGSInfoByNSID(n32NSID);\
	if (NULL == cpiGSInfo)\
	{\
		return eEC_NullGateServer;\
	}

#define CheckGetUserByNetInfo(cpiGSInfo, n32GCNSID)\
	CCSUser* piUser =  GetCSUserMgrInstance()->GetUserByNetInfo(cpiGSInfo, n32GCNSID);\
	if (NULL == piUser)\
{\
ELOG(LOG_ERROR, "CheckGetUserByNetInfo failed with n32GCNSID(%d)", n32GCNSID);\
return eEC_NullUser;\
}\
piUser->ResetPingTimer();

enum EBattleCamp
{
	//EBattleCamp_Rand  = -1,
	EBattleCamp_0 = 0,
	EBattleCamp_1,
	EBattleCamp_End,
};

static int CombineCost[] = {0,10,20,100,300,1200}; 

class CCSUser;
struct SBattleUserInfo
{
	ref_ptr<CCSUser> piUser;
	bool bIfReady;
	bool	bIfMaster;
	UINT32	un32HeroTypeID;
	UINT32	un32TempHeroTypeID;
	bool	bLoadCompleted;
	bool bSelectHeroWarning;
	
	SBattleUserInfo():piUser(NULL)
		, bIfMaster(FALSE)
		, bIfReady(FALSE)
		, un32HeroTypeID(0)
		, un32TempHeroTypeID(0)
		, bLoadCompleted(FALSE)
		, bSelectHeroWarning(TRUE){
	}

	void Clear(){
		piUser = NULL;
		bIfReady = FALSE;
		bIfMaster = FALSE;
		un32HeroTypeID = 0;
		un32TempHeroTypeID = 0;
		bLoadCompleted = FALSE;
		bSelectHeroWarning = TRUE;
	}
};

struct SBattleOBInfo{
	CCSUser		*piUser;
	bool		bIfMaster;
	bool		bLoadCompleted;
		SBattleOBInfo() : piUser(NULL)
		, bIfMaster(FALSE)
		, bLoadCompleted(FALSE){
	}

	void Clear(){
		piUser = NULL;
		bIfMaster = FALSE;
		bLoadCompleted = FALSE;
	}
};

class CCSBattle;
struct SBattleStateChangeInfo
{
	EBattleState eOriState;
	EBattleState eCurState;
	CCSBattle *pcBattle;

	SBattleStateChangeInfo():eOriState(eBS_Free)
		, eCurState(eBS_Free)
		, pcBattle(NULL){

	}

	void Clear(){
		eOriState = eBS_Free;
		eCurState = eBS_Free;
		pcBattle = NULL;
	}
};

struct SUserLoginInfo
{
	SUserNetInfo sUNI;
	CHAR szUserName[c_n32DefaultNameLen];
	CHAR szUserPwd[c_n32DefaultUserPwdLen];
};

#define CheckGSInfo(n32NSID) const ICSGSInfo *cpiGSInfo = GetCSKernelInstance()->GetGSInfoByNSID(n32NSID);\
	if (NULL == cpiGSInfo)\
{\
	ELOG(LOG_ERROR, "CheckGSInfo failed with n32NSID :%d", n32NSID);\
	return eEC_NullGateServer;\
}

#define CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID)\
	CCSUser *pcUser = GetCSUserMgrInstance()->GetUserByNetInfo(cpiGSInfo, n32GCNSID);\
	if (NULL == pcUser)\
{\
	ELOG(LOG_ERROR, "CheckGetUserByNetInfo failed with n32NSID :%d", n32GCNSID);\
	return eEC_NullUser;\
}\
	else{\
	pcUser->ResetPingTimer();\
}
class CCSBattleMgr;
CCSBattleMgr*	GetBattleMgrInstance();
redisAsyncContextWrapper*	GetUserDBCacheRedisHandler();
void	SetUserDBCacheRedisHandler(redisAsyncContextWrapper* predisAsyncContextWrapper);
void	SetLogicCacheRedisHandler(redisAsyncContextWrapper* predisAsyncContextWrapper);

redisAsyncContextWrapper*	GetLogicRedisHandler();
enum EGoodsType{
	eGoodsType_None = 0,
	eGoodsType_Hero = 1,
	eGoodsType_Skin,
	eGoodsType_Runes
};

enum EComsumeType{
	eComsume_Gold=1,
	eConsmue_Diamond=2,
	eConsume_RefreshCard
};

string GetLocalDayStr();
INT32	GetDaysDiffAwayToday(boost::gregorian::date& preDay);

enum	EDBConnectorID
{
	eDBConnectorID_None = 0,
	eDBConnectorID_GameDB,
	eDBConnectorID_LogDB,
	eDBConnectorID_UserDB,
};

using boost::gregorian::date;
using boost::posix_time::ptime;
using boost::posix_time::microsec_clock;
INT64 GetUTCMiliSecond();
ptime&	GetInitPtime();
}
using namespace CentralServer;
/*房间人员*/
class IRoomPlayer
{
public:
	IRoomPlayer(CCSUser* pUser){
		m_pUser = pUser;
		m_RoomID = 0;
		m_FindPos = -1;
		bIfReady = false;
		bIfMaster = false;
		bIfReStart = false;
	}
public:
	void SetFindPos(UINT32 pos) { m_FindPos=pos; }
	UINT32 GetFindPos() { return m_FindPos; }
	UINT32 GetRoomPos() { return m_FindPos+1; }
public:
	CCSUser* m_pUser;
	UINT32 m_RoomID;
	UINT32 m_FindPos;
	BOOLEAN bIfReady;
	BOOLEAN	bIfMaster;
	BOOLEAN bIfReStart;
};

/*匹配人员*/
class IMatchPlayer
{
public:
	IMatchPlayer(CCSUser* pUser){
		m_pUser = pUser;
		m_MatchTeamId = 0;
		bIfMaster = false;
	}
public:
	CCSUser* m_pUser;
	UINT32 m_MatchTeamId;
	BOOLEAN	bIfMaster;
};

struct tagMailState
{
	EMailCurtState curtState;
	bool		   bFlag; 
};
#include "BattleMatcher/CMatchTeam.h"
#include "BattleMatcher/IMatchList.h"
#include "BattleMatcher/CMatchList_VsAI.h"
#include "BattleMatcher/CMatchList_Nomal.h"
#include "BattleMatcher/CMatchList_Tianti.h"
#include "BattleMatcher/CMatchMgr.h"
#include "TaskSys/TaskMgr.h"