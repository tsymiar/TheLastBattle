/*
* file name			:CSVoipCallback.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* build date        :May 19 2014
* complete date		:
* summary			:
*
*/

#pragma once
#include "stdafx.h"
#ifdef _WINDOWS
#include <Ice/Ice.h>
#include "ZXINGroupAgentServant.h"

namespace CentralServer{

class ICSVoipCallbackCallback
{
public:
	virtual void	onCreateVoipRoom(INT64 roomId) = 0;
};

class CCSVoipCallback : public IceUtil::Shared
{

private:
	Ice::CommunicatorPtr		m_ic;
	CHAR*						m_voipConnStr;

	ICSVoipCallbackCallback*	m_createRoomCallback;

	void clear();

	void response(Ice::Int  ret,const ZXIN::GrpAgentPack& resp);
	void exception(const Ice::Exception& ex);

public:
	CCSVoipCallback(const CHAR* server, INT32 port);
	virtual	~CCSVoipCallback();

	void setVoipCallbackCallback(ICSVoipCallbackCallback* callback);

	BOOLEAN makeAsyncVoipRoom();
	void releaseVoipRoom(INT64 roomId);
};

}

#endif