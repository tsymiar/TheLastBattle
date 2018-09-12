/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:CGSGSInfo.h
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

namespace GateServer{
class CGSSSInfo
{
	friend	class	CGSKernel;

public:
	//property from config file.
	INT32		m_n32SSID;
	CHAR		m_szUserPwd[c_n32DefaultUserPwdLen];
	//property  from scene server.
	SIPAddr		m_sListenIP;
	INT32		m_n32ListenPort;
	//property from local.
	EServerNetState	m_eSSNetState;
	INT32		m_n32NSID;
	UINT32		m_un32ConnTimes;
	TIME_MILSEC	m_tLastConnMilsec;
	TIME_MILSEC	m_tPingTickCounter;

	INT64		m_n64MsgReceived;
	INT64		m_n64MsgSent;
	INT64		m_n64DataReceived;
	INT64		m_n64DataSent;

public:
	CGSSSInfo(void);
	virtual ~CGSSSInfo(void);

	virtual	INT32			GetSSID() const {return m_n32SSID;}
	virtual	INT32			GetNSID() const {return m_n32NSID;}
	virtual	EServerNetState GetSSNetState() const {return m_eSSNetState;}
	virtual	void			SetSSStatus(EServerNetState eState){m_eSSNetState = eState;}
};

}