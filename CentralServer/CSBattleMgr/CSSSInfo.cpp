#include "stdafx.h"
#include "CSSSInfo.h"

namespace CentralServer{

CCSSSInfo::CCSSSInfo(void)
{
	m_n32SSID = 0;
	m_un32ConnTimes = 0;
	memset(m_szName, 0, sizeof(CHAR) * c_n32DefaultNameLen);
	memset(m_szUserPwd, 0, sizeof(CHAR) * c_n32DefaultUserPwdLen);
	//property  from scene server.
	memset(&m_sListenIP, 0, sizeof(SIPAddr));
	m_n32ListenPort = 0;
	//property from local.
	m_eSSNetState = eSNS_Closed;
	m_n32NSID = 0;
	m_un32ConnTimes = 0;
	m_tLastConnMilsec = 0;
	m_tLastPingMilSec = 0;
	m_n32BattleNum = 0;
}

CCSSSInfo::~CCSSSInfo(void)
{

}

void CCSSSInfo::SetSSNetState(EServerNetState netState)
{
	m_eSSNetState = netState;
}

INT32	CCSSSInfo::AddBattleNum(INT32 n32AddNum) 
{
	m_n32BattleNum += n32AddNum;
	Assert (0 <= m_n32BattleNum);
	if (0 > m_n32BattleNum){
		m_n32BattleNum = 0;
	}
	if (m_n32BattleNum < c_n32MaxBattleInSS / 2){
		m_eSSNetState = eSNS_Free;
	}else if (m_n32BattleNum >= c_n32MaxBattleInSS / 2){
		m_eSSNetState = eSNS_Busy;
	}else{
		m_eSSNetState = eSNS_Full;
	}

	return eNormal;
}

void CCSSSInfo::ResetPing()
{
	m_tLastPingMilSec = GetUTCMiliSecond();
}

}
