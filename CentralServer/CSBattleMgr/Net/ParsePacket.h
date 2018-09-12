#pragma once

#include "../../UCore\include\net/ucnet.h"

class CSParser : public UCODE::ISDPacketParser{
	virtual INT32 UCAPI ParsePacket(const char* pBuf, UINT32 dwLen){
		if (!pBuf){
			return -1;
		}

		return dwLen;
	}
};