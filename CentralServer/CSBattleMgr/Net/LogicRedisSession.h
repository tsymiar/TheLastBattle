#pragma once

#include "../../Share/Net/ISrvCliSession.h"
#include "../hiredis.h"
#include "../async.h"
class CLogicRedisSession : public ICliSession
{
public:
	CLogicRedisSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:

	virtual void UCAPI OnRecv(const char* pBuf, UINT32 dwLen);
	void   HandleClosed(int status);
};
