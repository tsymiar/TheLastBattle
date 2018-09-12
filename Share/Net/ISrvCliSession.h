#pragma once

#include "INetSession.h"

class ISrvCliSession : public INetSession
{
public:
	ISrvCliSession();
	virtual ~ISrvCliSession();
public:
	virtual void UCAPI Release(void) {delete this;/*释放该对象*/}
};
