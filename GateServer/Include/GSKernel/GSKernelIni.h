/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:GSKernelIni.h
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

#include  "../GateServerIni.h"


#define		Begin_JT_FBAll_GateServer_GSKernel		namespace GateServer{
#define		End_JT_FBAll_GateServer_GSKernel			}

#ifndef  __JT_FBAll_GateServer_GSKernel_Dll__
#define  __JT_FBAll_GateServer_GSKernel_Ext__    __declspec(dllimport)
#else
#define  __JT_FBAll_GateServer_GSKernel_Ext__    __declspec(dllexport)
#endif

Begin_JT_FBAll_GateServer_GSKernel

End_JT_FBAll_GateServer_GSKernel

