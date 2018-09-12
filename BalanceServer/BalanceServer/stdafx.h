// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include <stdio.h>
#include <tchar.h>
#include <map>
#include <vector>
#include <list>
using namespace std;

// TODO: 在此处引用程序需要的其他头文件
#include <WinSock2.h>
#include <Windows.h>
#include <conio.h>
#include <atlconv.h>
#include "Net\BSNetSessionMgr.h"
#include "../../ThirdFunc/MsgLists/BSToGS.pb.h"
#include "../../ThirdFunc/MsgLists/GCToSS.pb.h"
#include "../../ThirdFunc/MsgLists/BSToGC.pb.h"
#include "../../ThirdFunc/MsgLists/GSToBS.pb.h"
#include "../../ThirdFunc/MsgLists/GCToBS.pb.h"
#include "../../ThirdFunc/MsgLists/LSToBS.pb.h"
#include "../../ThirdFunc/MsgLists/BSToLS.pb.h"
#include <boost/shared_ptr.hpp>
#include "../../ThirdFunc/MsgLists/ParseProto.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using namespace google;

struct sBSConfig
{
	INT32 gs_listen_port;
	INT32 gs_full_count;
	INT32 gs_base_index;
	INT32 gs_max_count;
	vector<string> gs_ip_list;
	INT32 client_listen_port;
	string ls_ip;
	INT32 ls_port;
};
extern sBSConfig gBsConfig;

struct sOneGsInfo
{
	bool gs_isLost;
	UINT32 gs_nets;
	INT32 gs_gc_count;//gs 当前连接数//
	string gs_IpExport;//gs 对外公布地址//
	string gs_Ip;//gs 对内验证地址//
	UINT32 gs_Port;
	UINT32 gs_Id;
};
extern map<UINT32,sOneGsInfo> gAllGsInfo; 