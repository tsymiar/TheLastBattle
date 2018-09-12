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

class CCSRCInfo : public ICSRCInfo
{
	friend	class	CCSKernel;
	friend	class	CCSGSNetListener;

public:
	//property from config file.
	SIPAddr		m_sListenIP;
	INT32		m_n32ListenPort;
	//property from local.
	EServerNetState	m_eNetState;
	INT32		m_n32NSID;

public:
	CCSRCInfo(void){ m_n32ListenPort=0; m_eNetState=eSNS_Closed; m_n32NSID=0; }
	~CCSRCInfo(void){}

	INT32					GetNSID() const {return m_n32NSID;}
	EServerNetState			GetRCNetState() const {return m_eNetState;}
};

}