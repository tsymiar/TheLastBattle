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

#ifdef _HAS_ITERATOR_DEBUGGING
#undef _HAS_ITERATOR_DEBUGGING
#define _HAS_ITERATOR_DEBUGGING 0
#endif

#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../../CommonCPlus/WYYTools/Vector3D.h"
#include "../../CommonCPlus/Include/FBAll_Const.h"
#include "SSDataStructure.h"
#include "../../ELogging/ELoggingHeader.h"
#include "../../ThirdFunc/MsgLists/GSToSS.pb.h"
#include "../../ThirdFunc/MsgLists/SSToGS.pb.h"
#include "../../ThirdFunc/MsgLists/SSToCS.pb.h"
#include "../../ThirdFunc/MsgLists/CSToSS.pb.h"
#include "../../ThirdFunc/MsgLists/GSToGC.pb.h"
#include "../../ThirdFunc/MsgLists/GCToCS.pb.h"
#include "../../ThirdFunc/MsgLists/GCToSS.pb.h"
#include "../../ThirdFunc/MsgLists/GCToLS.pb.h"
#include "../../ThirdFunc/MsgLists/GCToBS.pb.h"
#include "../../ThirdFunc/MsgLists/LSToGC.pb.h"
#include "../../ThirdFunc/MsgLists/BSToGC.pb.h"
#include "../../ThirdFunc/MsgLists/ToLog.pb.h"
#include <boost/shared_ptr.hpp>
#include "../../ThirdFunc/MsgLists/ParseProto.h"
#include "SafeRefPtr.h"
#include "SSProfileStatics.h"
#include <boost/date_time/posix_time/posix_time.hpp> 
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace std;

using boost::gregorian::date;
using boost::posix_time::ptime;
using boost::posix_time::microsec_clock;

// TODO: 在此处引用程序需要的其他头文件
#include "SSElementArr.h"
#include "SSElementArr.inl"

namespace SceneServer{
int64_t GetUTCMiliSecond();
INT64 GetWinMiliseconds();
class CSSHero;
class CSSUser;

struct SSSConfig
{
	INT32	n32SSID;
	SIPAddr sCSIP;
	INT32	n32CSPort;
	SIPAddr	sLogSIP;
	INT32	n32LogSPort;
	INT32	n32SSMaxMsgSize;		
	INT32	n32WorkingThreadNum;
	CHAR	aszUserPwd[c_n32DefaultUserPwdLen];
	SIPAddr	sGSListenIP;
	INT32	n32GSListenPort;
};

class ISSGSInfo{
public:
	virtual	INT32			GetGSID() const = 0;
	virtual	INT32			GetNSID() const = 0;
	virtual	EServerNetState	GetGSNetState() const = 0;
};

class CSSGSInfo;
struct	SGSNetInfo
{
	TIME_MILSEC	tConnMilsec;
	CSSGSInfo*	pcGSInfo;
	UINT64		un64MsgSent;
	UINT64		un64MsgRecved;
	UINT64		un64DataSent;
	UINT64		un64DataRecved;
};
#define CheckGetUserByNetInfo(cpiGSInfo, n32GCNSID)\
	CSSUser *piUser = GetCUserByNetInfo(cpiGSInfo, n32GCNSID);\
	if (NULL == piUser)\
{\
	return eEC_NullUser;\
}\
	piUser->ResetPingTimer();
#define  CheckGetBattleByNetInfo(cpiGSInfo, n32GCNSID)\
	CSSUser *piUser =  GetCUserByNetInfo(cpiGSInfo, n32GCNSID);\
	if (NULL == piUser)\
{\
	ELOG(LOG_WARNNING, "null piUser with n32GCNSID(%d)", n32GCNSID);\
	return eEC_NullUser;\
}\
	CSSBattle *pcBattle = GetBattle(piUser->GetBattleID());\
	if (NULL == pcBattle){\
	ELOG(LOG_WARNNING, "User %llu, battle id %llu, but it does not exist.", piUser->GetGUID(), piUser->GetBattleID());\
	return eEC_NullBattle;\
	}\
	piUser->ResetPingTimer();

const INT32	c_n32AroundAStarRegionNum = 8;
const INT32	c_n32DefaultGOBuffSize = 128;
const FLOAT	c_fMiniSqrCollideDeep = (FLOAT)(c_n32HalfCellSize * c_n32HalfCellSize);

class CSSBattle;
struct SBattleStateChangeInfo{
	EBattleState eOriState;
	EBattleState eCurState;
	CSSBattle *pcBattle;
};
const INT32	c_n32AreaCellNum = 48;
const INT32	c_n32UsingEffectsNum = 32;

enum EFixedDir{
	eFixedDir_Center = 0,
	eFixedDir_UpLeft,
	eFixedDir_Up,
	eFixedDir_UpRight,
	eFixedDir_Left,	
	eFixedDir_Right,
	eFixedDir_BelowLeft,
	eFixedDir_Below,
	eFixedDir_BelowRight,
	eFixedDir_End,
};


//class CSSGameUnit;
class CSSGameUnit;
class CSSSkillEffect;

enum EGOActionState{
	eOAS_Free,
	eOAS_Running,
	eOAS_PreparingSkill,
	eOAS_ReleasingSkill,
	eOAS_UsingSkill,
	eOAS_LastingSkill,
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

enum EGoodsSpecificaFlag{
	eGoodsSpecificaFlag_InvalidButNotDestory = -2,
	eGoodsSpecificaFlag_InfiniteUse,
	eGoodsSpecificaFlag_NoSpecifical,
	eGoodsSpecificaFlag_Pending
};

const INT32  n_32MaxSkillEntity = 10;

const INT32 c_n32DefBStarNodeNum = 100;
enum EBStarMoveDir{
	eBStarMoveDir_Left,
	eBStarMoveDir_Right,
};
enum EBStarBranchClimbState{
	eBStarBranchClimbState_Running,
	eBStarBranchClimbState_Cancel,
	eBStarBranchClimbState_Complete,
};
const	INT32	c_n32MaxAheadRegion = 3;
const	INT32	c_n32BattleRegionCellNum = 2;
const	INT32	c_n32PathPointWatchDist = 250;

struct SBattleRegion{
	UINT16 un16XRegionIdx;
	UINT16 un16ZRegionIdx;
	bool	bStaticBlock;
	bool	bStaticTriBlock;
	EIntDir eTriBlockDir;
	CSSGameUnit	*pcRegionNPC;
	CSSGameUnit	*pcRegionHero;
	SBattleRegion	*apsAroundRegion[eIntDir_End];

	SBattleRegion():un16XRegionIdx(0)
	 , un16ZRegionIdx(0)
	, bStaticBlock(0)
	, pcRegionHero(NULL)
	, pcRegionNPC(NULL){
		for (INT32 i = 0; i < eIntDir_End; i++){
			apsAroundRegion[i] = NULL;
		}
	}
};

struct SIntPoint{
	INT32	n32XIdx;
	INT32	n32ZIdx;
	SIntPoint():n32XIdx(0)
	 , n32ZIdx(0){
	}
	SIntPoint(INT32 n32X, INT32 n32Z):n32XIdx(n32X)
	, n32ZIdx(n32Z){
	}
	SIntPoint(const SBattleRegion &crsRegion)
	: n32XIdx(crsRegion.un16XRegionIdx)
	, n32ZIdx(crsRegion.un16ZRegionIdx){
	}
	SIntPoint& operator = (SIntPoint& p){
		if (&p == this){
			return *this;
		}
		n32ZIdx = p.n32ZIdx;
		n32XIdx = p.n32XIdx;
		return *this;
	}
	bool operator == (const SIntPoint &crsRight)  { if (crsRight.n32XIdx == n32XIdx && crsRight.n32ZIdx == n32ZIdx){return true;}else{return false;}}
	bool operator != (const SIntPoint &crsRight){if(crsRight.n32XIdx != n32XIdx || crsRight.n32ZIdx != n32ZIdx){return true;}else{return false;}}
	SIntPoint operator +(const SIntPoint &crsRight){return SIntPoint(crsRight.n32XIdx + n32XIdx, crsRight.n32ZIdx + n32ZIdx);}
	SIntPoint operator -(const SIntPoint &crsRight){return SIntPoint(n32XIdx - crsRight.n32XIdx, n32ZIdx - crsRight.n32ZIdx);}
	void	Abs(){
		if (n32XIdx < 0){
			n32XIdx = -n32XIdx;
		}
		if (n32ZIdx < 0){
			n32ZIdx = -n32ZIdx;
		}
	}
	void Clear(){
		n32XIdx = 0;
		n32ZIdx = 0;
	}
};

struct SGOActionStateInfo{
	EGOActionState		eOAS;
	TIME_MILSEC			tASUTCMilsec;
	CVector3D			cPos;
	CVector3D			cDir;
	UINT32				un32SkillID;
	CVector3D			cSkillTarPos;
	SGUID_KEY			sSkillTarGUID;
	FLOAT				fDistMoved;

	SGOActionStateInfo():eOAS(eOAS_Free)
	 , tASUTCMilsec(GetUTCMiliSecond())
	, un32SkillID(0)
	, fDistMoved(0.0f){
	}

	void Clear(){
		eOAS = eOAS_Free;
		tASUTCMilsec = 0; 
		cPos.Zero(); 
		cDir.LookAhead();
		un32SkillID = 0;
		cSkillTarPos.Zero();
		sSkillTarGUID = 0;
		fDistMoved = 0.0f; 
	}
};

typedef	map<CSSGameUnit*, FLOAT>  GO2SightDiffMap;

struct SSTimer{
	UINT32 runTimes;
	UINT32 timeArray[10];

	SSTimer():runTimes(0){
		memset(timeArray,0,10*sizeof(UINT32));
	}

	void Clear(){
		runTimes = 0;
		memset(timeArray,0,10*sizeof(UINT32));
	}
};

template <class T>
boost::shared_ptr<T> ParseProtoMsg(const char* pData, int n32DataLength)
{
	boost::shared_ptr<T> msg(new T);
	bool res = msg->ParseFromArray(pData, n32DataLength);
	if (!res){
		return NULL;
	}
	return msg;
}

}
