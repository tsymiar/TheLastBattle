#include <atlconv.h>
#include "INetSessionMgr.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <sstream>

INetSessionMgr* INetSessionMgr::mInstance = NULL;

INetSessionMgr::INetSessionMgr()
{
	if (mInstance!=NULL) { char* p = NULL; *p = 0; }
	mInstance = this;
	InitializeCriticalSection(&mNetworkCs);
	SDNetSetLogger(this,LOGLEVEL);
	SYSTEM_INFO stSysInfo;
	GetSystemInfo(&stSysInfo);
	SIocpOptThreadNum threadNum;
	threadNum.byThreadNum = stSysInfo.dwNumberOfProcessors;
	SDNetSetOpt(IOCP_THREAD_NUM,&threadNum);
	mNetModule = SDNetGetModule(&UCNET_VERSION);
	memset(&mListener[0],0,sizeof(ISDListener*)*MAX_COUNT_LISTENER);
	mIsUnSafeSend = false;
}

INetSessionMgr::~INetSessionMgr()
{
	for (int i=0;i<MAX_COUNT_LISTENER; ++i)
	{
		if (mListener[i])
		{
			mListener[i]->Stop();
		}
	}

	if (mNetModule) mNetModule->Release();
	DeleteCriticalSection(&mNetworkCs);
}

bool UCAPI INetSessionMgr::LogText( const TCHAR* pszLog )
{
	USES_CONVERSION;
	ELOG(LOG_DBBUG, W2A(pszLog));
	return true;
}

bool UCAPI INetSessionMgr::LogBinary( const UINT8* pLog, UINT32 dwLen )
{
	return true;
}

INT32 UCAPI INetSessionMgr::ParsePacket( const char* pBuf, UINT32 dwLen ){
	if (pBuf==NULL) 
		return -1;

	if (dwLen < sizeof(int)) 
		return 0;

	int totLength = *(int*)pBuf;
	if (totLength <= dwLen)
		return totLength;

	return 0;
}

bool INetSessionMgr::CreateListener( int port, int recvsize, int sendsize, int pos, ISDSessionFactory* pOtherFactory/*=NULL*/ )
{
	if (pos>=MAX_COUNT_LISTENER) return false;
	if (mListener[pos]!=NULL) return false;
	mListener[pos] = mNetModule->CreateListener(NETIO_COMPLETIONPORT);
	mListener[pos]->SetSessionFactory(pOtherFactory==NULL?this:pOtherFactory);
	mListener[pos]->SetBufferSize(recvsize,sendsize);
	mListener[pos]->SetPacketParser(this);
	return mListener[pos]->Start("0", port);
}

bool INetSessionMgr::CreateConnector( SESSION_TYPE type, const char* ip, int port, int recvsize, int sendsize, int logicId )
{
	ISDConnector* pConnector = mNetModule->CreateConnector(NETIO_COMPLETIONPORT);
	ICliSession* pSession = CreateConnectorSession(type);	
	UINT32 sId = AddSession(pSession);
	Assert(sId!=PP_INVALID);
	pSession->SetID(sId);
	pSession->SetLogicID(logicId);
	pSession->SetConnector(pConnector);
	pSession->SetRemoteEndPointer(ip,port);
	pConnector->SetSession(pSession);
	pConnector->SetPacketParser(this);
	pConnector->SetBufferSize(recvsize,sendsize);
	return pConnector->Connect(ip,port);
}

void INetSessionMgr::Update()
{
	mNetModule->Run();

	vector<char*> tempQueue;
	EnterCriticalSection(&mNetworkCs);
	tempQueue.swap(m_SafeQueue);
	LeaveCriticalSection(&mNetworkCs);

	for (auto it=tempQueue.begin();it!=tempQueue.end();++it){
		char* pBuffer = (*it);
		int nType = *(((int*)pBuffer)+0);
		int nSessionID = *(((int*)pBuffer)+1);
		Send((SESSION_TYPE)nType,nSessionID,pBuffer+2*sizeof(int));
		delete []pBuffer;
	}

	auto &map = m_AllSessions.GetPointerMap();
	for (auto it=map.begin();it!=map.end();++it)
	{
		(*it)->Update();
	}
}

UINT32 INetSessionMgr::AddSession(INetSession* pSession)
{
	return m_AllSessions.AddPointer(pSession);
}

void INetSessionMgr::RemoveSession(INetSession* pSession)
{
	m_AllSessions.RemovePointer(pSession->GetID());
}

void INetSessionMgr::Send(SESSION_TYPE stype, int sessionId, char* pBuffer)
{
	int Len = *(int*)pBuffer;
	if (sessionId>0)
	{
		INetSession* pSession = m_AllSessions.GetPointer(sessionId);
		if (pSession)
		{
			pSession->Send(pBuffer,Len);
		}
	}
	else
	{
		auto &map = m_AllSessions.GetPointerMap();
		for (auto it=map.begin();it!=map.end();++it)
		{
			INetSession* pSession = (*it);
			if (pSession)
			{
				if (pSession->GetType()==stype)
				{
					pSession->Send(pBuffer,Len);
					if (sessionId==0) break;
				}
			}
		}
	}
}

void INetSessionMgr::SendMsgToSession(SESSION_TYPE stype, int sessionId, google::protobuf::Message& sMsg, int n32MsgID)
{
	if (!mIsUnSafeSend)
	{
		int n32MsgSize = sMsg.ByteSize();
		int n32Length = n32MsgSize+2*sizeof(int);
		char* pBuffer = new char[n32Length];
		memcpy(pBuffer+0*sizeof(int), (char*)&n32Length, sizeof(int));
		memcpy(pBuffer+1*sizeof(int), (char*)&n32MsgID, sizeof(int));
		bool res = sMsg.SerializeToArray(pBuffer+2*sizeof(int), n32MsgSize);
		Assert(res);
		Send(stype,sessionId,pBuffer);
		delete []pBuffer;
	}
	else
	{
		int n32MsgSize = sMsg.ByteSize();
		int n32Length = n32MsgSize+4*sizeof(int);
		char* pBuffer = new char[n32Length];
		memcpy(pBuffer+0*sizeof(int), (char*)&stype, sizeof(int));
		memcpy(pBuffer+1*sizeof(int), (char*)&sessionId, sizeof(int));
		n32Length = n32MsgSize + 8;
		memcpy(pBuffer+2*sizeof(int), (char*)&n32Length, sizeof(int));
		memcpy(pBuffer+3*sizeof(int), (char*)&n32MsgID, sizeof(int));
		bool res = sMsg.SerializeToArray(pBuffer+4*sizeof(int), n32MsgSize);
		Assert(res);
		EnterCriticalSection(&mNetworkCs);
		m_SafeQueue.push_back(pBuffer);
		LeaveCriticalSection(&mNetworkCs);
	}
}

void INetSessionMgr::SendMsgToSession(SESSION_TYPE stype, int sessionId, const std::string& sMsg, int n32MsgID)
{
	SendMsgToSession(stype,sessionId,sMsg.c_str(),sMsg.size(),n32MsgID);
}

void INetSessionMgr::SendMsgToSession(SESSION_TYPE stype, int sessionId, const char* pMsgBuffer, int n32MsgLen, int n32MsgID)
{
	if (!mIsUnSafeSend)
	{
		int n32Length = n32MsgLen+2*sizeof(int);
		char* pBuffer = new char[n32Length];
		memcpy(pBuffer+0*sizeof(int), (char*)&n32Length, sizeof(int));
		memcpy(pBuffer+1*sizeof(int), (char*)&n32MsgID, sizeof(int));
		memcpy(pBuffer+2*sizeof(int), pMsgBuffer, n32MsgLen);
		Send(stype,sessionId,pBuffer);
		delete []pBuffer;
	}
	else
	{
		int n32Length = n32MsgLen+4*sizeof(int);
		char* pBuffer = new char[n32Length];
		memcpy(pBuffer+0*sizeof(int), (char*)&stype, sizeof(int));
		memcpy(pBuffer+1*sizeof(int), (char*)&sessionId, sizeof(int));
		n32Length = n32MsgLen + 8;
		memcpy(pBuffer+2*sizeof(int), (char*)&n32Length, sizeof(int));
		memcpy(pBuffer+3*sizeof(int), (char*)&n32MsgID, sizeof(int));
		memcpy(pBuffer+4*sizeof(int), pMsgBuffer, n32MsgLen);
		EnterCriticalSection(&mNetworkCs);
		m_SafeQueue.push_back(pBuffer);
		LeaveCriticalSection(&mNetworkCs);
	}
}

void INetSessionMgr::TranMsgToSession( SESSION_TYPE stype, int sessionId, const char* pMsgBuffer, int n32MsgLen, int n32MsgID, int n32TransId, int n32GcNet )
{
	if (n32TransId==0) n32TransId = n32MsgID;//无法伪装

	if (!mIsUnSafeSend)
	{
		int n32Length = n32MsgLen+4*sizeof(int);
		char* pBuffer = new char[n32Length];
		memcpy(pBuffer+0*sizeof(int), (char*)&n32Length, sizeof(int));
		memcpy(pBuffer+1*sizeof(int), (char*)&n32TransId, sizeof(int));//伪装消息ID
		memcpy(pBuffer+2*sizeof(int), (char*)&n32MsgID, sizeof(int));//真实消息ID
		memcpy(pBuffer+3*sizeof(int), (char*)&n32GcNet, sizeof(int));//插入
		memcpy(pBuffer+4*sizeof(int), pMsgBuffer, n32MsgLen);
		Send(stype,sessionId,pBuffer);
		delete []pBuffer;
	}
	else
	{
		int n32Length = n32MsgLen+6*sizeof(int);
		char* pBuffer = new char[n32Length];
		memcpy(pBuffer+0*sizeof(int), (char*)&stype, sizeof(int));
		memcpy(pBuffer+1*sizeof(int), (char*)&sessionId, sizeof(int));
		n32Length = n32MsgLen + 16;
		memcpy(pBuffer+2*sizeof(int), (char*)&n32Length, sizeof(int));
		memcpy(pBuffer+3*sizeof(int), (char*)&n32TransId, sizeof(int));//伪装消息ID
		memcpy(pBuffer+4*sizeof(int), (char*)&n32MsgID, sizeof(int));//真实消息ID
		memcpy(pBuffer+5*sizeof(int), (char*)&n32GcNet, sizeof(int));//插入
		memcpy(pBuffer+6*sizeof(int), pMsgBuffer, n32MsgLen);
		EnterCriticalSection(&mNetworkCs);
		m_SafeQueue.push_back(pBuffer);
		LeaveCriticalSection(&mNetworkCs);
	}
}

void INetSessionMgr::DisconnectOne( int sessionId )
{
	INetSession* pSession = m_AllSessions.GetPointer(sessionId);
	if (pSession) pSession->DelayClose(111);
}
