#include "stdafx.h"
#include <random>
#include "CSUser.h"

namespace CentralServer{

	struct SAiRobot 
	{
		UINT64 guid;
		int seat;
		int grpid;
		string nickname;
		int headid;
		int level;
	};

class CCSBattle
{
public:
	CCSBattle(){}
	CCSBattle(EBattleMatchType match_type, BattleType type,UINT32 ssId,UINT64 battleId,UINT32 mapId, CCSUserListMap& pCCSUserList, map<UINT32,UINT32>* aiRobots=NULL);
	virtual ~CCSBattle(void);

private:
	bool				m_ifAsync;
	TIME_MILSEC			m_tStateUTCMilsec;
	UINT32				m_n32SSID;
	UINT64				m_BattleID;
	UINT32				m_MapID;
	string				m_Password;
	BattleType			m_Type;
	EBattleMatchType	m_MatctType;
	map<UINT64,CCSUser*> m_UserMap;
	map<UINT32,SAiRobot> m_AIRobots;
	SMapLogicCfg*		m_MapLogicCfg;

public:
	bool				IfAsync()									{ return m_ifAsync; }
	INT32				GetBattleMapId()							{ return m_MapID; }
	INT64				GetBattleId()								{ return m_BattleID; }
	CCSUser*			GetUserByGUID(UINT64 guid,UINT32* grpId=NULL);
	INT32				Start();
	INT32				Abort();
	INT32				OnCreate();
	INT32				OnFinish(SSToCS::FinishBattle* pFinishMsg);
	virtual void				CaculateResult(SSToCS::FinishBattle* pFinishMsg);
	INT32				CaculateExp(CCSUser* pUser, INT32 tSpan, bool bIfWiner);
	INT32				CaculateGold(CCSUser* pUser, INT32 tSpan, bool bIfWiner);
	INT32				CaculateFWGold(CCSUser* pUser, INT32 tSpan);
	virtual void				ReCreateRoom();
	virtual  void				ReCreateMatch();
	INT32				GetSSID()const								{return m_n32SSID;}
	void				SetPWD(const char* pwd)						{ m_Password = pwd; }
	//msg to game client.
	INT32				BroadcastMsgToAllUser(google::protobuf::Message& sMsg, int n32MsgID);
	//msg to scene server.
	INT32				PostMsgToBattleSS(google::protobuf::Message& sMsg, int n32MsgID);
	INT32				PostMsgToSS_NotifyUserIsOnline(CCSUser *pUser, bool isOnline);
	//msg to gate server.
	INT32				PostMsgToGS_NotifyCombineUserNetInfoToSS(CCSUser *piUser, ICSSSInfo *piSSInfo);
	INT32				PostMsgToGS_NotifySplitUserNetInfoFromSS(CCSUser *piUser, ICSSSInfo *piSSInfo);

	map<UINT64,CCSUser*> &				GetUserMap()				{return m_UserMap;}

	virtual void	  AddBattleHeroAndOtherList(CCSUser* pUser,CSToSS::CreateBattle_CreateBattleUserInfo*& pUserInfo) ;

};

}