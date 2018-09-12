#pragma once

#include "IMsgHandle.h"
#include <string>
#include <vector>
#include <map>
using namespace std;
#include "..\..\ucore\include\net\ucnetopt.h"
#include "..\..\ucore\include\net\ucnet.h"
using namespace UCODE;
//#ifdef _DEBUG
//#pragma comment(lib,"../../ucore/bin/x64/Debug/ucodebase.lib")
//#pragma comment(lib,"../../ucore/bin/x64/Debug/ucodenet.lib")
//#else
//#pragma comment(lib,"../../ucore/bin/x64/Release/ucodebase.lib")
//#pragma comment(lib,"../../ucore/bin/x64/Release/ucodenet.lib")
//#endif
#ifdef _DEBUG
#pragma comment(lib,"../../Bin/x64/Debug/ucodebase.lib")
#pragma comment(lib,"../../Bin/x64/Debug/ucodenet.lib")
#else
#pragma comment(lib,"../../Bin/x64/Release/ucodebase.lib")
#pragma comment(lib,"../../Bin/x64/Release/ucodenet.lib")
#endif
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"

enum SESSION_TYPE
{
	ST_None,
	///////////////////////////
	ST_SERVER_CS_OnlySS,
	ST_SERVER_CS_OnlyGS,
	ST_SERVER_SS,
	ST_SERVER_GS,
	ST_SERVER_Log,
	ST_SERVER_BS_OnlyGS,
	ST_SERVER_BS_OnlyGC,
	ST_SERVER_LS_OnlyBS,
	ST_SERVER_LS_OnlyGC,
	ST_CLIENT_B2L,
	ST_CLIENT_G2C,
	ST_CLIENT_G2S,
	ST_CLIENT_G2B,
	ST_CLIENT_S2C,//as client to link gs
	ST_SERVER_CS_OnlyRC,
	ST_CLIENT_C2LG,
	ST_CLIENT_S2LG,
	ST_CLIENT_C2L,// link login server
	ST_CLIENT_C2B,//link balance server
	ST_CLIENT_C2G,//link gate server
	ST_CLIENT_C2R,
	ST_CLIENT_S2Log,
	ST_CLIENT_C2Log,
	ST_CLIENT_C2LogicRedis,
};


class INetSession : public ISDSession
{
public:
	INetSession();
	virtual ~INetSession();
public:
	UINT32 GetID() { return mID; }
	void SetID(UINT32 id) { mID=id; }
	UINT32 GetLogicID() { return mLogicID; }
	void SetLogicID(UINT32 logicId) { mLogicID = logicId; }
	UINT32 GetType() { return mType; }
	void SetType(UINT32 type) { mType = type; }
	void SetSendBufferMaxSize(UINT32 maxsize) { mSendBufferMaxSize = maxsize; }
	void Close();
	void DelayClose(UINT32 error_code,UINT32 delayCount=10);
	bool IsConned() { if(mConnection && mConnection->IsConnected()) return true; return false; }
	const char* GetRemoteIp() { return mConnection!=NULL?mConnection->GetRemoteIPStr():""; }
	UINT32 GetRemoteIpEx() { return mConnection!=NULL?mConnection->GetRemoteIP():0; }
	UINT32 GetRemotePort() { return mConnection!=NULL?mConnection->GetRemotePort():0; }

	bool IsClosed() { return mCloseTime>0; }
	void SetInited(bool isInited,bool isTrigger=false);//call this func will trigger any logic.
	bool Send(const char* pBuf,UINT32 dwLen);
	void SetMsgHandle(IMsgHandle* pMsgHandle) { mMsgHandle = pMsgHandle; }
	virtual void Update();
protected:
	bool IsHadRecon() { return mReconFlag; }
	void ResetRecon() { mReconFlag = false; }
	bool IsInited() { return mIsInited; }
	bool IsRemoteInited() { return mIsRemoteInited; }
	void SetRemoteInited(bool isInited) { mIsRemoteInited = isInited; }
protected:
	virtual void SendInitData() = 0;
	virtual void OnRealEstablish() = 0;
	virtual void OnClose() = 0;
public:
	virtual void UCAPI SetConnection(ISDConnection* poConnection);
	virtual void UCAPI OnEstablish(void);
	virtual void UCAPI OnTerminate(void);
	virtual bool UCAPI OnError(INT32 nModuleErr, INT32 nSysErr);
	virtual void UCAPI OnRecv(const char* pBuf, UINT32 dwLen);
	virtual void UCAPI Release(void) {}
protected:
	bool mIsInited;
	bool mIsRemoteInited;
	bool mReconFlag;
	bool mIsUseBuffer;
	bool mIsSrvCli;
	volatile bool mIsLogicClosed;
	UINT32 mType;
	UINT32 mLogicID;
	UINT32 mID;
	static UINT32 mMaxID;
	ISDConnection* mConnection;
	UINT32 mStartTime;
	UINT32 mCloseTime;
	vector<char> mBuffers;
	IMsgHandle* mMsgHandle;
	volatile UINT32 mCloseDelayCount;
	UINT32 mSendBufferMaxSize;//send buffer max size//
};
