/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:SSUser.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:WangYanYu
* complete date		:March 21 2014
* summary			:
*
*/

#pragma once
#include "stdafx.h"
#include <unordered_map>
#include <random>
#include <map>
 
#include "SafeRefPtr.h"

namespace SceneServer{

class CSSHero;
class CSSBattleMgr;

struct SRunePageParameter{
	SFightProperty fpBase;
	SFightProperty fpPercent;
};

class CSSUser : referable<CSSUser>
{
	UINT64							m_sUserGUID;
	SUserNetInfo					m_sUserNetInfo;
	string							m_stNickName;
	INT64							m_n64Diamond;
	INT64							m_n64DiamondUsed;
	INT64							m_n64Gold;
	INT64							m_n64GoldUsed;
	UINT16							m_un16VipLv;
	TIME_MILSEC						m_tGCLastPing;
	bool							m_bIfConnect;
	vector<INT32>					m_cCanUseHeroVec;
	vector<SRunePageParameter>		m_cRunePageVec;
	UINT64							m_un64BattleID;
	CSSHero*						m_pHero;

public:
	CSSUser();
	~CSSUser();

public:
	const SUserNetInfo&			GetUserNetInfo() const{return m_sUserNetInfo;}
	UINT64						GetGUID() {return m_sUserGUID;}
	void						SetGUID(UINT64 guid){m_sUserGUID = guid;}

	const string&				GetNickName() {return m_stNickName;}
	void						SetNickName(const string& stNickName) {m_stNickName=stNickName;}

	void						SetDiamond(UINT64 value){m_n64Diamond = value;}
	INT64						GetDiamond() {return m_n64Diamond;}

	void						AddDiamondUsed(UINT64 value){m_n64DiamondUsed += value;}
	INT64						GetDiamondUsed(){return m_n64DiamondUsed;}

	void						SetGold(UINT64 value){m_n64Gold = value;}
	INT64						GetGold() { return m_n64Gold; }

	void						AddGoldUsed(UINT64 value){m_n64GoldUsed += value;}
	INT64						GetGoldUsed(){return m_n64GoldUsed;}

	INT32						GetVipLevel() { return m_un16VipLv;}
	void						SetVipLevel(INT16 value){m_un16VipLv = value;}

	INT32						IfConnect() {return m_bIfConnect;}
	void						SetConnectState(bool state) { m_bIfConnect = state;}

	void						AddCanUseHeroID(INT32 id){m_cCanUseHeroVec.push_back(id);}
	vector<INT32>*				GetCanUseHeroList(){return &m_cCanUseHeroVec;}

	vector<SRunePageParameter>*	GetRunePageVec(){return &m_cRunePageVec;}

	UINT64						GetBattleID(){return m_un64BattleID;}
	void						SetBattleID(UINT64 battleID){m_un64BattleID = battleID;}

	CSSHero*					GetHero(){return m_pHero;}
	void						SetHero(CSSHero* pHero){m_pHero = pHero;}
	INT32						GetHeroRebornTimes();

	INT32						PostMsgToGC(google::protobuf::Message& sMsg, int n32MsgID);
	INT32						ResetPingTimer();
	bool						CheckUserPwd(const CHAR *cpszUserPwd);
	void						ClearNetInfo();//清理User下网络信息，未清理UserNetMap表//
	INT32						SetUserNetInfo(const SUserNetInfo &crsUNI);//设置User下网络信息，但未设置玩家上下线状态，未修改UserNetMap表//

	/************************************************************************/
	/* ask handler					                                        */
	/************************************************************************/
	INT32						GCAskPing(const char* pMsg, int n32MsgLength); 
};

}
