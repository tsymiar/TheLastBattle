/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:GSKernel.h
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
#include	"GSKernelIni.h"
#include "../../../ThirdFunc/google/protobuf/message.h"

Begin_JT_FBAll_GateServer_GSKernel

class	IGSKernel : public IGSModel
{
public:
	virtual	INT32				RegisteModel(EGSModelID	eModelID, IGSModel *piModel) = 0;
	virtual	IGSModel*			GetModel(EGSModelID	eModelID) = 0;
	virtual	INT32				GetMaxGCNum() = 0;
	virtual	INT32				GetMaxGCMsgSize() = 0;
	virtual	INT32				GetGSID() = 0;
	virtual	TIME_MILSEC				GetHeartBeatUTCMilsec() = 0;
	virtual	TIME_TICK				GetHeartBeatTickSpan() = 0;
};


End_JT_FBAll_GateServer_GSKernel

