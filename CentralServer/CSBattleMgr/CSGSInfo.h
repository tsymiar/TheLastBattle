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

namespace CentralServer{

class CCSGSInfo : public ICSGSInfo
{

friend	class	CCSKernel;
friend	class	CCSGSNetListener;

public:
	//property from config file.
	INT32		m_n32GSID;	
	CHAR		m_szName[c_n32DefaultNameLen];
	CHAR		m_szUserPwd[c_n32DefaultUserPwdLen];
	SIPAddr		m_sListenIP;
	INT32		m_n32ListenPort;
	//property from local.
	EServerNetState	m_eGSNetState;
	INT32		m_n32NSID;
	UINT32		m_un32ConnTimes;
	TIME_MILSEC	m_tLastConnMilsec;
	TIME_MILSEC	m_tLastPingMilSec;

	INT64		m_n64MsgReceived;
	INT64		m_n64MsgSent;
	INT64		m_n64DataReceived;
	INT64		m_n64DataSent;

public:
	CCSGSInfo(void);
	virtual ~CCSGSInfo(void);

	virtual	INT32					GetGSID() const {return m_n32GSID;}
	virtual	INT32					GetNSID() const {return m_n32NSID;}
	virtual	EServerNetState			GetGSNetState() const {return m_eGSNetState;}
};

}