
#include "stdafx.h"
#include "SSOBSev.h"
#include "SSGameObject.h"
#include "SSBattle.h"
#include "SSHero.h"

namespace SceneServer{

static INT64  gOBUserID; 

static INT64 GetOBUserID()
{
	return gOBUserID++;
}

CSSOBSev::CSSOBSev()
{
	 Release();
}
CSSOBSev::~CSSOBSev()
{
	Release();
}

void CSSOBSev::Release()
{
	gOBUserID = c_n64OBUserDefaultId; 
	 
	m_OBUser.clear();  

	m_UserGUID.clear();

	m_OBUserGUID.clear();
}
//
//bool CSSOBSev::IfExistGUID(const SGUID_KEY &guid)
//{
//	if (guid.IfZero())
//	{
//		return false;
//	}
//	OBALLUSER_GUID::const_iterator itr = m_UserGUID.find(guid);
//	if (itr != m_UserGUID.end())
//	{
//		return true;
//	}
//	 
//	m_UserGUID[guid] = guid;
//
//	return false;
//}

INT32 CSSOBSev::AddObGUID(  CSSBattle *pBattle, const SGUID_KEY& guid, INT8 pos)
{
	if ( NULL == pBattle) {
		ELOG(LOG_ERROR,"--AddObGUID OB user:  pBattle is NULL--" );
		return eEC_NullPointer;
	}
	if (guid > 0)
	{
		ELOG(LOG_ERROR,"--AddObGUID OB user:  guid is zero--" );
		return eEc_InvalidGUID;
	}

	OBUSER_GUID_MAP::iterator itr = m_OBUserGUID.find(guid);
	if (itr != m_OBUserGUID.end())
	{
		ELOG(LOG_ERROR,"--AddObGUID OB user:  guid is exist--" );
		return eEc_InvalidGUID;
	}
	m_OBUserGUID[guid]= pos;
	return eNormal;
}

EGameObjectCamp  CSSOBSev::GetOBGUIDCamp(const SGUID_KEY & guid)
{
	INT8  tempCamp = 0;
	OBUSER_GUID_MAP::iterator itr = m_OBUserGUID.find(guid);
	if (itr != m_OBUserGUID.end())
	{
		tempCamp = itr->second;
	}
	return (EGameObjectCamp)tempCamp;
}

INT32  CSSOBSev::AddObUser(  CSSBattle *pBattle,    CSSUser *user  )
{
	if ((NULL == pBattle) || ( NULL == user  ) )	{
		ELOG(LOG_DBBUG, "--something wrong about the OB user  --" );
		return eEC_NullPointer;
	}
	
	INT64 tUserId =  GetOBUserID(); 
	OBUSER_MAP::iterator itr =	m_OBUser.find(tUserId);
	if (itr != m_OBUser.end()){
		return eEc_ExistObj;
	}
 
	m_OBUser[user->GetGUID()] = user;  

	return eNormal;
} 

bool   CSSOBSev::IsAllOBEnter()
{
	if (m_OBUserGUID.size() < 1)
	{
		return true;
	}
	OBUSER_GUID_MAP::iterator itr = m_OBUserGUID.begin();
	for (; itr != m_OBUserGUID.end(); itr++)
	{
		OBUSER_MAP::iterator itr_ex =	m_OBUser.find(itr->first);
		if(itr_ex == m_OBUser.end())
		{
			return false;
		}
	}
	return true;
}

bool	CSSOBSev::IfOBUser(CSSUser* piUser)
{
	map<INT64, CSSUser*>::iterator iter = m_OBUser.begin();
	for (;iter != m_OBUser.end(); iter ++){
		if (iter->second == piUser){
			return true;
		}
	}
	return false;
}
//  
// void CSSOBSev::SyncBattleHerosSkillsInfo(CSSBattle *pBattle   )
// {
// 	if (NULL == pBattle)
// 	{
// 		return;
// 	} 
// 	SBattleUserInfo *m_asBattleUserInfo = pBattle->GetBattleUserInfo();
// 	if(NULL == m_asBattleUserInfo){
// 		return;
// 	}  
// 	 
// 	for (INT32 i = 0; i < c_un32MaxUserInBattle; i++){
// 		CSSHero *pcThisHero = m_asBattleUserInfo[i].pcHero;
// 		if(NULL == pcThisHero){
// 			continue;
// 		} 
// 		CSSArea *pcCurArea = pBattle->GetAreaByPos(pcThisHero->GetCurPos());
// 		if (NULL == pcCurArea){
// 			continue;
// 		}
// 		//pBattle->SyncSight(pcThisHero, NULL, pcCurArea); 
// 
// 		pcThisHero->SyncSkillStateToGC(0);  
// 
// 		pcThisHero->NotifyFuryStateToCl();
// 	} 
// }
//
//void CSSOBSev::SyncBattleBaseInfo( IMsg::PsMsgHeader  psMsgHeader,CSSBattle *pBattle ) 
//{
//	if (NULL == pBattle )
//	{ 
//		WYYTools_LogMgr->Debug("--error: CSSOBSev::GetHeroBaseInfo()- null battle--");
//		return;
//	}
//	
//	OBUSER_MAP::iterator itr =	m_OBUser.begin(); 
//	for (; itr != m_OBUser.end(); itr++)
//	{ 
//		ISSUser *pUser = itr->second;
//		GetSSKernelInstance()->PostMsgToGC(&pUser->GetUserNetInfo(), 1, psMsgHeader);
//	}
//} 
//
//INT32  CSSOBSev::RemoveDeadGUID(const SGUID_KEY & guid)
//{
//	OBALLUSER_GUID::const_iterator itr = m_UserGUID.find(guid);
//	if (itr != m_UserGUID.end())
//	{
//		m_UserGUID.erase(itr);
//	}
//	return eNormal;
//}

//interface 

 

}