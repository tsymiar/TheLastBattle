// SSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "SSUser.h"
#include "SSBattleMgr.h"
#include "SSUserCfgMgr.h"

#include "boost/spirit.hpp"
#include "boost/algorithm/string.hpp"
#include "SSWorkThreadMgr.h"
#include "SSHero.h"

using namespace std;

namespace SceneServer{

CSSUser::CSSUser(): m_sUserGUID(0)
	, m_sUserNetInfo(0,0)
	, m_n64Diamond(0)
	, m_n64DiamondUsed(0)
	, m_n64Gold(0)
	, m_n64GoldUsed(0)
	, m_un16VipLv(0)
	, m_tGCLastPing(0)
	, m_bIfConnect(FALSE)
	, m_un64BattleID(0)
	, m_pHero(NULL)
	 
{

}

CSSUser::~CSSUser()
{
	 
}

INT32	CSSUser::SetUserNetInfo(const SUserNetInfo &crsUNI){
	Assert(crsUNI.IsValid());
	Assert(!m_sUserNetInfo.IsValid());
	m_sUserNetInfo.n32GCNSID = crsUNI.n32GCNSID;
	m_sUserNetInfo.n32GSID = crsUNI.n32GSID;
	m_bIfConnect = TRUE;
	return eNormal;
}

INT32	CSSUser::GCAskPing(const char* pMsg, int n32MsgLength){
	if (NULL == pMsg){
		return eEC_NullMsg;
	}

	std::auto_ptr<GCToSS::AskPingSS> pPingSS(new GCToSS::AskPingSS);
	pPingSS->ParseFromArray(pMsg, n32MsgLength);

	GSToGC::PingRet sPingRet;
	sPingRet.set_time(pPingSS->time());
	sPingRet.set_flag(1);
	CSSWorkThreadMgr::GetInstance().PostMsgToGC(m_sUserNetInfo, sPingRet, sPingRet.msgid());
	return eNormal;
}

INT32	CSSUser::ResetPingTimer(){
	m_tGCLastPing = GetUTCMiliSecond();
	return eNormal;
}
 

void CSSUser::ClearNetInfo(){
	ELOG(LOG_DBBUG, "clear nsid:%d", m_sUserNetInfo.n32GCNSID);
	m_sUserNetInfo.Clear();
	m_bIfConnect = FALSE;
}
 
INT32	CSSUser::GetHeroRebornTimes(){
	return c_BaseRebornTimes;

}
 
}