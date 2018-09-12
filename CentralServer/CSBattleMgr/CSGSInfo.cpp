#include "stdafx.h"
#include "CSGSInfo.h"

namespace CentralServer{

CCSGSInfo::CCSGSInfo(void)
{
	m_n32GSID = 0;
	m_n32NSID = 0;
	m_eGSNetState = eSNS_Closed;
	memset(&m_sListenIP, 0, sizeof(SIPAddr));
	memset(m_szName, 0, sizeof(CHAR) * c_n32DefaultNameLen);
	memset(m_szUserPwd, 0, sizeof(CHAR) * c_n32DefaultUserPwdLen);

	m_n32NSID = 0;
	m_un32ConnTimes = 0;
	m_tLastConnMilsec = 0;
	m_tLastPingMilSec = 0;
	m_n64MsgReceived = 0;
	m_n64MsgSent = 0;
	m_n64DataReceived = 0;
	m_n64DataSent = 0;
}

CCSGSInfo::~CCSGSInfo(void)
{

}

}
