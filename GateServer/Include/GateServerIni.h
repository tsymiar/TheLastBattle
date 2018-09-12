/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:Common.h
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

#include  "../../CommonCPlus/Include/CommonCPlusIni.h"
#include "../../CommonCPlus/Include/FBAll_Cfg.h"
#include "../../CommonCPlus/Include/FBAll_Const.h"
#include "../../CommonCPlus/Include/FBAll_DBHeader.h"
#include "../../CommonCPlus/Include/FBAll_RunTime.h"
#include "../../CommonCPlus/Include/FBAll_Function.h"
#include "../../CommonCPlus/Include/FBAll_Protocol_ToCS.h"
#include "../../CommonCPlus/Include/FBAll_Protocol_ToGS.h"
#include "../../CommonCPlus/Include/FBAll_Protocol_ToSS.h"
#include "../../CommonCPlus/Include/FBAll_Protocol_ToBS.h"
#include "../../CommonCPlus/Include/FBAll_Protocol_ToGC.h"

using namespace JT::CommonCPlus::WYYTools;
using namespace JT::FBAll::CommonCPlus;

#define		Begin_JT_FBAll_GateServer		namespace GateServer{

#define		End_JT_FBAll_GateServer			}

Begin_JT_FBAll_GateServer

const	INT32	c_n32DefaultCentralServerNetSessionIndex = 0;

class IGSSSInfo
{
public:
	enum	ESSStatus
	{
		eSSStatus_Closed = 0,
		eSSStatus_Connecting,
		eSSStatus_Logining,
		eSSStatus_Free,
		eSSStatus_Busy,
		eSSStatus_Full,
	};

public:
	virtual	INT32			GetSSID() const = 0;
	virtual	INT32			GetNSID() const = 0;
	virtual	EServerNetState	GetSSNetState() const = 0;
};

class IGSModel : public IFBAllModel
{
public:
	virtual	INT32							OnMsgFromCS(const char* pMsg, int n32MsgLength, int n32MsgID) = 0;
	virtual	INT32							OnMsgFromSS(IGSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength, int n32MsgID) = 0;
	virtual	INT32							OnMsgFromGC(INT32 n32NSID, const char* pMsg, int n32MsgLength, int n32MsgID) = 0;
};

End_JT_FBAll_GateServer

