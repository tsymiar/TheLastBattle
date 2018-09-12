#pragma once

class INetSession;
#include <WinSock2.h>
#include <Windows.h>
#include "..\PointerPoll.h"
#include "NetPointerPool.h"

// 网络消息头，获取网络数据
struct SNetHeader
{
	int size;
	int type;
};

// 网络消息结构体
typedef bool (*msg_handle)(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);

// 网络消息节点，解析网络数据==>网络消息，并分发之
struct MsgNode
{
	msg_handle mHandle;
	bool mIsInitMsg;
	MsgNode() { mIsInitMsg=false; mHandle=NULL; }
};

class IMsgHandle
{
public:
	IMsgHandle(int base,int max);
	virtual ~IMsgHandle();
public:
	void RegisterMsgFunc(int type, msg_handle fuc_handle,bool isInitMsg=false);
	void SetUnKnownMsgHandle(msg_handle fuc_handle);
	MsgNode* mNodes;
	msg_handle mUnknownHandle;
	int mMax;
	int mBase;
};
