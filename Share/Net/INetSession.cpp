#include "INetSession.h"
#include "INetSessionMgr.h"

INetSession::INetSession()
{
	mIsInited = false;
	mIsRemoteInited = false;
	mReconFlag = false;
	mIsUseBuffer = true;
	mIsSrvCli = false;
	mIsLogicClosed = true;
	mType = 0;
	mLogicID = 0;
	mID = PP_INVALID;
	mConnection = NULL;
	mStartTime = 0;
	mCloseTime = 0;
	mMsgHandle = NULL;
	mCloseDelayCount = 0;
	mSendBufferMaxSize = 0;
}

INetSession::~INetSession()
{

}

void UCAPI INetSession::SetConnection( ISDConnection* poConnection )
{
	mConnection = poConnection;

	const char chOpt = 1;
	SConnectionOptSockopt stOpt;
	stOpt.nLevel = IPPROTO_TCP;
	stOpt.nOptName = TCP_NODELAY;
	stOpt.pOptVal = &chOpt;
	stOpt.nOptLen = sizeof(char);

	mConnection->SetOpt(CONNECTION_OPT_SOCKOPT,&stOpt);
}

void UCAPI INetSession::OnEstablish( void )
{
	if (mIsSrvCli){
		Assert(mID==PP_INVALID);
		mID = INetSessionMgr::GetInstance()->AddSession(this);
	}
	if (mID==PP_INVALID){
		DelayClose(222);
		return;
	}
	mIsInited = false;
	mIsRemoteInited = false;
	mReconFlag = false;
	mIsLogicClosed = false;
	mStartTime = GetTickCount();
	mCloseTime = 0;
	mCloseDelayCount = 0;
	mBuffers.clear();
}

void UCAPI INetSession::OnTerminate( void )
{
	mIsInited = false;
	mIsRemoteInited = false;
	mCloseTime = GetTickCount();
	mReconFlag = true;
	if (!mIsLogicClosed) OnClose();
	mIsLogicClosed = true;
	mCloseDelayCount = 0;
	if (mIsSrvCli){
		INetSessionMgr::GetInstance()->RemoveSession(this);
		mID = PP_INVALID;
	}
}

bool UCAPI INetSession::OnError( INT32 nModuleErr, INT32 nSysErr )
{
	mIsInited = false;
	mIsRemoteInited = false;
	mCloseTime = GetTickCount();
	mReconFlag = true;
	if (!mIsLogicClosed) OnClose();
	mIsLogicClosed = true;
	mCloseDelayCount = 0;
	if (mIsSrvCli){
		INetSessionMgr::GetInstance()->RemoveSession(this);
		mID = PP_INVALID;
	}
	return true;
}

void INetSession::Close()
{
	if (IsConned()) 
	{
		mConnection->Disconnect();
		if (!mIsLogicClosed) OnClose();
		mIsLogicClosed = true;
	}
}

void INetSession::DelayClose(UINT32 error_code,UINT32 delayCount/*=10*/)
{
	if (mCloseDelayCount>0) return;
	mCloseDelayCount = delayCount>10?delayCount:10;
}

void UCAPI INetSession::OnRecv(const char* pBuf, UINT32 dwLen){
	if (mIsLogicClosed) 
		return;
	if (mCloseDelayCount > 0) 
		return;
	if (mMsgHandle==NULL) 
		return;
	
	SNetHeader* pNetHeader = (SNetHeader*)pBuf;
	Assert(pNetHeader->size == dwLen);
	int n32MsgLen = pNetHeader->size - 8;
	int id = pNetHeader->type - mMsgHandle->mBase;
	char* pMsgData = (char*)(pBuf + 8);
	if (id >= 0 && id < mMsgHandle->mMax){
		MsgNode* pNode = mMsgHandle->mNodes + id;
		if (pNode->mHandle){
			bool bRet = pNode->mHandle(pMsgData, n32MsgLen, this, pNetHeader->type);
		}
		else{
			mMsgHandle->mUnknownHandle(pMsgData, n32MsgLen, this, pNetHeader->type);
		}
	}
	else if(mMsgHandle->mUnknownHandle){
		mMsgHandle->mUnknownHandle(pMsgData, n32MsgLen, this, pNetHeader->type);
	}
}

bool INetSession::Send( const char* pBuf,UINT32 dwLen )
{
	if (mIsLogicClosed) return false;
	if (mCloseDelayCount>0) return false;
	if (!IsConned()) return false;

	if (mIsUseBuffer)
	{
		UINT32 freesize = 0;

		if (mBuffers.size()==0) freesize = mConnection->GetSendBufFree();

		if (dwLen>freesize)
		{
			UINT32 len = dwLen-freesize;
			const char* pTempBuf = pBuf+dwLen-len;
			dwLen = freesize;

			if (mSendBufferMaxSize>0)
			{
				if (mSendBufferMaxSize>mBuffers.size()+len)
				{
					DelayClose(111);
					return false;
				}
			}

			while(len-->0){ mBuffers.push_back(*pTempBuf); ++pTempBuf;}
		}
	}

	mConnection->Send(pBuf,dwLen);
	return true;
}

void INetSession::Update()
{
	if (!IsConned()) return;
	if (mIsLogicClosed) return;

	if (mCloseDelayCount>0){
		if (--mCloseDelayCount==1){
			Close();
		}
		return;
	}

	if (mIsUseBuffer)
	{
		UINT32 buffersize = mBuffers.size();
		if (buffersize>0)
		{
			UINT32 freesize = mConnection->GetSendBufFree();
			UINT32 sendsize = freesize>=buffersize?buffersize:freesize;
			mConnection->Send(&mBuffers[0],sendsize);
			mBuffers.erase(mBuffers.begin(),mBuffers.begin()+sendsize);
		}
	}
}

void INetSession::SetInited(bool isInited,bool isTrigger/*=false*/)
{
	mIsInited = isInited;

	if (isTrigger)
	{
		if (mIsInited)
		{
			if (!IsRemoteInited())
			{
				SetRemoteInited(true);
				SendInitData();
			}
			OnRealEstablish();
		}
	}
}
