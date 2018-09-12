/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:CCSGSInfo.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:WangYanYu
* complete date		:March 12 2014
* summary			:
*
*/

#pragma once
#include "stdafx.h"
//#include "CSKernel.h"

namespace CentralServer{

class CCSSSInfo : public ICSSSInfo
{
	friend	class	CCSKernel;
	friend	class	CCSGSNetListener;

public:
	//property from config file.
	INT32		m_n32SSID;
	CHAR		m_szName[c_n32DefaultNameLen];
	CHAR		m_szUserPwd[c_n32DefaultUserPwdLen];
	SIPAddr		m_sListenIP;
	INT32		m_n32ListenPort;
	//property from local.
	EServerNetState	m_eSSNetState;
	INT32		m_n32NSID;
	UINT32		m_un32ConnTimes;
	TIME_MILSEC	m_tLastConnMilsec;
	TIME_MILSEC	m_tLastPingMilSec;

	INT32		m_n32BattleNum;

public:
	CCSSSInfo(void);
	virtual ~CCSSSInfo(void);

	virtual	INT32					GetSSID() const {return m_n32SSID;}
	virtual	INT32					GetNSID() const {return m_n32NSID;}
	virtual	EServerNetState			GetSSNetState() const {return m_eSSNetState;}
	virtual void					SetSSNetState(EServerNetState netState);
	virtual	INT32					GetBattleNum() {return m_n32BattleNum;}
	virtual	INT32					AddBattleNum(INT32 n32AddNum);
	virtual void					ResetPing();
};

}