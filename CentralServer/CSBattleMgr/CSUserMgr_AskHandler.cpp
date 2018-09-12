// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSUserMgr.h"
#include "CSUser.h"
#include "CSKernel.h"
#include "CSGameLogMgr.h"
#include "CSCfgMgr.h"
#include "CSBattleMgr.h"
using namespace std;

namespace CentralServer{
	
INT32	CCSUserMgr::UserAskLogin(const ICSGSInfo *cpiGSInfo, INT32 n32UserGCNSID, GCToCS::Login& pLogin){
	if (pLogin.name().empty() || pLogin.name().size() > c_n32DefaultNameLen){
		return eEC_InvalidUserName;
	}
	SUserNetInfo netinfo(cpiGSInfo->GetGSID(), n32UserGCNSID);
	if (GetUserByNetInfo(netinfo)){
		return eEC_InvalidNetState;
	}
 
	UserCombineKey sUserCombineKey(pLogin.name(), pLogin.sdk());
	auto iter = m_AllUserName2GUIDMap.find(sUserCombineKey);
	UINT64 guid = 0;

	if (iter != m_AllUserName2GUIDMap.end()){
		//老玩家
		guid = iter->second;
		//如果还在内存里
		CCSUser* pUser = GetUserByGuid(guid);
		if (NULL != pUser){
			bool bFlag = CheckIfInGuideBattle(pUser);
			if (bFlag)
			{
				ELOG(LOG_WARNNING, "新手引导玩家不允许顶号");
				return eEC_GuideUserForbit;
			}

			pUser->OnOnline(netinfo,pLogin,false,false); 
			return eNormal;
		}

		//此时首先查询RedisCache，若没有命中，在查询数据库
		CSToDB::QueryUserReq* sQueryUser = new CSToDB::QueryUserReq;
		sQueryUser->set_logininfo(pLogin.SerializeAsString());
		sQueryUser->set_gsid(cpiGSInfo->GetGSID());
		sQueryUser->set_gcnetid(n32UserGCNSID);
		sQueryUser->set_csid(CCSKernel::GetInstance().GetKernelCfg().unCSId);
		sQueryUser->set_objid(guid);
		//若Redis缓存可用，则查询；不可用就直接查询数据库（通过RedisIP配置控制）
		bool ifQueryFromRedis = false;
		do {
			auto pReids = GetUserDBCacheRedisHandler();
			if (!pReids){
				break;
			}
			if (!pReids->CanbeUsed()){
				break;
			}
			ifQueryFromRedis = true;
			pReids->redisAsyncCommand(RedisSCallBack(&CCSUserMgr::redisLoginCallBack, this), (char*)(sQueryUser)
				, "HMGET usercache:%llu "
				"guid nickname runebag "
				"runeslot  hero  friend  black "
				"item  mail diamond  gold "
				"score  Last1WinTime LoginReward  RegisteTime "
				"Cldays UserLv VipLv header "
				"assist deadtimes killbuild gameinns "
				"killhero winInns lvExp vipScore " 
				"guideStr n8Sex taskData", guid);
		} while (false);
		
		if (!ifQueryFromRedis){
			GetNowWorkActor().EncodeAndSendToDBThread(*sQueryUser, sQueryUser->msgid());
			delete sQueryUser;
		}
	}
	else{
		//新玩家，产生GUID
		guid = CombineGUID();

		auto pcUser = new CCSUser;
		SUserDBData sUserDBData;
		sUserDBData.sPODUsrDBData.un64ObjIdx = guid;
		sUserDBData.szUserName = pLogin.name();
		sUserDBData.szUserPwd = pLogin.passwd();
		sUserDBData.sPODUsrDBData.eUserPlatform = (EUserPlatform)pLogin.sdk();
		sUserDBData.sPODUsrDBData.tRegisteUTCMillisec = time(NULL);

		//加入全局表
		UserCombineKey sUserCombineKey;
		sUserCombineKey.sdkid = pLogin.sdk();
		sUserCombineKey.username = pLogin.name();
		auto res1 = m_AllUserName2GUIDMap.insert(std::make_pair(sUserCombineKey, guid));
		Assert(res1.second);  

		pcUser->LoadDBData(sUserDBData);

		pcUser->GetUserBattleInfoEx().mDebugName = pLogin.name(); 

		INT32 nRet = AddUser(pcUser);
		if (nRet != 0){
			delete pcUser;
			return nRet;
		}

		pcUser->OnOnline(netinfo, pLogin, true, true);

		InsertNewUserToMysql(pLogin, pcUser); 
	}
	//log
	{
		stringstream mystream;
		mystream <<  pLogin.name() << LOG_SIGN;
		mystream << pLogin.sdk() << LOG_SIGN;
		mystream << pLogin.platform() << LOG_SIGN;
		mystream << pLogin.equimentid()  << LOG_SIGN ;
		mystream << pLogin.ipaddress();
		CSSGameLogMgr::GetInstance().AddGameLog(eLog_Login, guid, mystream.str());
	}
	return eNormal;
}

INT32 CCSUserMgr::UserAskReconnectGame(const ICSGSInfo *cpiGSInfo, INT32 n32UserGCNSID, const string& cpszUserName, const string& cpszUserPwd){
	SUserNetInfo netinfo(cpiGSInfo->GetGSID(), n32UserGCNSID);
	if (GetUserByNetInfo(netinfo)){
		return eEC_InvalidNetState;
	}
	//需要从消息获取
	INT32 sdkID = 0;
	UserCombineKey sUserCombineKey(cpszUserName, sdkID);
	auto iter = m_AllUserName2GUIDMap.find(sUserCombineKey);
	if (iter == m_AllUserName2GUIDMap.end()){
		return  eEC_NullUser;
	}

	UINT64 guid = iter->second;
	CCSUser *pcUser =  GetUserByGuid(guid);
	if (NULL == pcUser){
		return eEC_NullUser;
	}

	GCToCS::Login login;
	pcUser->OnOnline(netinfo,login,false,false,true);
	return eNormal;
}

void	CCSUserMgr::UpdateUserInfo(boost::shared_ptr<SSToCS::AskUpdateData> &puserData)
{
	for (int i=0; i< puserData->user_data_size(); i++)
	{ 
		UINT64 posUserGUID(puserData->user_data(i).guid());
		CCSUser *pcUser = GetUserByGuid(posUserGUID);
		if(NULL == pcUser){
			continue;
		} 

		pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalDeadTimes,puserData->user_data(i).deadtimes());
		pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalHeroKills,puserData->user_data(i).herokills());
		pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalDestoryBuild,puserData->user_data(i).destorybuilding() );
		pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalGameInns,puserData->user_data(i).gameinn() );
		pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_TotalWinInns,puserData->user_data(i).wininn() );

		pcUser->CheckUpgrade(puserData->user_data(i).exp());

		pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_Gold,puserData->user_data(i).gold() );
	}  
}

void	CCSUserMgr::UpdateSNSList(const UINT64 guid, SUserRelationshipInfo info, EDBOperation type)
{
	CSToDB::AlterSNSList snsinfo;
	snsinfo.set_user_id(guid);
	snsinfo.set_related_id(info.guididx);
	snsinfo.set_related(info.eRelationShip);
	snsinfo.set_opration(type);

	m_UserCacheDBActiveWrapper->EncodeAndSendToDBThread(snsinfo, snsinfo.mgsid());
}

bool	CCSUserMgr::CheckIfInGuideBattle(CCSUser* pUser)
{
	if(pUser->GetUserBattleInfoEx().GetBattleID() > 0 &&  pUser->GetUserBattleInfoEx().GetBattleType() == eBattleType_Guide1 )
		return true;
	return false;
}
}