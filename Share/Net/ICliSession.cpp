#include "ICliSession.h"

ICliSession::ICliSession()
{
	mReconnectTag = true;
	mReconTime = 0;
	m_poConnector = NULL;
}

ICliSession::~ICliSession()
{
	if (m_poConnector)
	{
		m_poConnector->Release();
		m_poConnector = NULL;
	}
}

bool ICliSession::Reconnect()
{
	if (IsHadRecon() && mReconnectTag)
	{
		UINT32 curTime = GetTickCount();

		if (curTime>mReconTime)
		{
			mReconTime = curTime+10000;

			if (m_poConnector->ReConnect())
			{
				//printf("client reconnect server(%s)...\n",mRemoteEndPointer.c_str());
				ResetRecon();
				return true;
			}
		}
	}

	return false;
}

bool ICliSession::Reconnect(const char* ip, UINT32 port)
{
	return m_poConnector->Connect(ip,port);
}

void UCAPI ICliSession::OnEstablish( void )
{
	INetSession::OnEstablish();
	SetRemoteInited(true);
	SetInited(true);
	SendInitData();
	SetInited(false);
}

void ICliSession::Update()
{
	INetSession::Update();
	Reconnect();
}

void ICliSession::SetRemoteEndPointer(const char* ip,UINT32 port)
{
	mRemoteEndPointer = ip;
	char strPort[10];
	mRemoteEndPointer += ":";
	_snprintf(strPort, sizeof(strPort), "%d",port);
	mRemoteEndPointer.append(strPort);
}
