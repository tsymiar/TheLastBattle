// SSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "SSBattleMgr.h"
#include "SSUser.h"
#include "SSHero.h"

using namespace std;

namespace SceneServer{

CSSUser*	CSSBattleMgr::GetCUserByNetInfo(const ISSGSInfo *cpiGSInfo, INT32 n32GCNSID){
	if (NULL == cpiGSInfo)
	{
		return NULL;
	}

	SUserNetInfo sNetInfo(cpiGSInfo->GetGSID(), n32GCNSID);
	return GetCUserByNetInfo(sNetInfo);
}

CSSUser*	CSSBattleMgr::GetCUserByNetInfo(const SUserNetInfo &crsUNI)
{
	CSSUser *pcUser = NULL;
	map<SUserNetInfo, CSSUser*>::iterator iterUser = m_cUserNetInfoMap.find(crsUNI);
	if (m_cUserNetInfoMap.end() != iterUser)
	{
		pcUser = iterUser->second;
	}
	return pcUser;
}

CSSUser*	CSSBattleMgr::GetCUserByGUID(const SGUID_KEY& cpsUserGUID){
	CSSUser *pcUser = NULL;
	UINT64 un64ObjIndex = cpsUserGUID;
	map<UINT64, CSSUser*>::iterator iterGUID = m_cUserGUIDMap.find(un64ObjIndex);
	if (m_cUserGUIDMap.end() != iterGUID){
		pcUser = iterGUID->second;
	}

	return pcUser;
}

CSSUser* CSSBattleMgr::AddUser(SUserNetInfo *cpsUserNetInfo, INT64 guid){
	CSSUser *pcUser = new CSSUser();
	pcUser->SetGUID(guid);
	pcUser->SetUserNetInfo(*cpsUserNetInfo);
	Assert(m_cUserGUIDMap.find(pcUser->GetGUID())==m_cUserGUIDMap.end());
	m_cUserGUIDMap.insert(std::make_pair(pcUser->GetGUID(), pcUser));
	m_cUserNetInfoMap[*cpsUserNetInfo] = pcUser;
	return pcUser;
}

INT32 CSSBattleMgr::RemoveUser(CSSUser*& pUser){
	if (pUser==NULL) return eNormal;
	Assert(m_cUserGUIDMap.find(pUser->GetGUID())!=m_cUserGUIDMap.end());
	m_cUserGUIDMap.erase(pUser->GetGUID());
	m_cUserNetInfoMap.erase(pUser->GetUserNetInfo());
	delete pUser;
	pUser = NULL;
	return eNormal;
}

INT32 CSSBattleMgr::SetUserNet(CSSUser *pcUser,SUserNetInfo& netinfo){
	Assert(pcUser!=NULL);
	Assert(m_cUserNetInfoMap.find(netinfo)==m_cUserNetInfoMap.end());
	pcUser->SetUserNetInfo(netinfo);
	m_cUserNetInfoMap[netinfo] = pcUser;
	return eNormal;
}

INT32 CSSBattleMgr::RemoveUserNet(CSSUser *pcUser){
	Assert(pcUser!=NULL);
	Assert(m_cUserNetInfoMap.find(pcUser->GetUserNetInfo())!=m_cUserNetInfoMap.end());
	m_cUserNetInfoMap.erase(pcUser->GetUserNetInfo());
	pcUser->ClearNetInfo();
	return eNormal;
}

}