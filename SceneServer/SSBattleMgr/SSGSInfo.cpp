#include "stdafx.h"
#include "SSGSInfo.h"

namespace SceneServer{

CSSGSInfo::CSSGSInfo(void)
{
	m_n32GSID = 0;
	memset(m_szUserPwd, 0, sizeof(CHAR) * c_n32DefaultUserPwdLen);
	//property  from scene server.
	memset(&m_sListenIP, 0, sizeof(SIPAddr));
	m_n32ListenPort = 0;
	//property from local.
	m_eGSNetState = eSNS_Closed;
	m_n32NSID = 0;
	m_un32ConnTimes = 0;
	m_tLastConnMilsec = 0;
	//m_tLastPingMilSec = 0;
	m_n64MsgReceived = 0;
	m_n64MsgSent = 0;
	m_n64DataReceived = 0;
	m_n64DataSent = 0;
}

CSSGSInfo::~CSSGSInfo(void)
{

}

}
