// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSUserMgr.h"
#include "CSUser.h"

#include "../ELogging/ELogging/SSActive.h"
#include "CSCfgMgr.h"
#include "CSKernel.h"

using namespace std;

namespace CentralServer{
	void CCSUserMgr::redisLoginCallBack(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pData){
		std::auto_ptr<CSToDB::QueryUserReq> pQueryUser((CSToDB::QueryUserReq*)pData);
		int res = eNormal;
		CCSUser *pcUser = nullptr;
		do {
			if (!predisReply){
				ELOG(LOG_ERROR, "Null Reply");
				res = REDIS_REPLY_NIL;
				break;
			}
			SUserNetInfo netinfo(pQueryUser->gsid(), pQueryUser->gcnetid());
			if (GetUserByNetInfo(netinfo)){
				res = eEC_InvalidNetState;
				break;
			}

			GCToCS::Login sLoginMsg;
			if (!ParseProtoMsg(pQueryUser->logininfo().c_str(), pQueryUser->logininfo().size(), sLoginMsg)){
				res = eEC_InvalidNetState;
				break;
			}

			pcUser = GetUserByGuid(pQueryUser->objid());
			if (NULL != pcUser){
				pcUser->OnOnline(netinfo, sLoginMsg,false,false);
				return;
			}

			if (predisReply->type == REDIS_REPLY_NIL){
				res = REDIS_REPLY_NIL;
				break;
			}
			SUserDBData sSUserDBData;
			PODUsrDBData& sPODUsrDBData = sSUserDBData.sPODUsrDBData;
			sPODUsrDBData.un64ObjIdx = pQueryUser->objid();
			sSUserDBData.szUserName = sLoginMsg.name();

			string bagStr;
			string slotStr;
			string friendStr;
			string blackStr;
			string heroStr;
			string itemStr;
			string mailStr;
			string guideStr;
			for (int i = 0; i < predisReply->elements; ++i){
				if (res != eNormal){
					break;
				}
				redisReply* pOneReply = predisReply->element[i];
				switch(i){
				case 0:
					if (pOneReply->len == 0){
						res = eEC_NULLCfg;
						break;
					}
					else{
						UINT64 tempGUID = _atoi64(pOneReply->str);
						Assert(tempGUID == sPODUsrDBData.un64ObjIdx);
					}
					break;
				case 1:
					if (pOneReply->len == 0){
						break;
					}
					sSUserDBData.szNickName = string(pOneReply->str, pOneReply->len);
					break;
				case 2:
					if (pOneReply->len == 0){
						break;
					}
					bagStr = string(pOneReply->str, pOneReply->len);
					break;
				case 3:
					if (pOneReply->len == 0){
						break;
					}
					slotStr = string(pOneReply->str, pOneReply->len);
					break;
				case 4:
					if (pOneReply->len == 0){
						break;
					}
					heroStr = string(pOneReply->str, pOneReply->len);
				case 5:
					if (pOneReply->len == 0){
						break;
					}
					friendStr = string(pOneReply->str, pOneReply->len);
					break;
				case 6:
					if (pOneReply->len == 0){
						break;
					}
					blackStr = string(pOneReply->str, pOneReply->len);
					break;
				case 7:
					if (pOneReply->len == 0){
						break;
					}
					itemStr = string(pOneReply->str, pOneReply->len);
					break;
				case 8:
					if (pOneReply->len == 0){
						break;
					}
					mailStr = string(pOneReply->str, pOneReply->len);
					break;
				case 9:
					sPODUsrDBData.n64Diamond = atoi(pOneReply->str);
					break;
				case 10:
					sPODUsrDBData.n64Gold = atoi(pOneReply->str);
					break;
				case 11:
					sPODUsrDBData.n64Score = atoi(pOneReply->str);
					break;
				case 12:
					sPODUsrDBData.tLastFirstWinTime = atoi(pOneReply->str);
					break;
				case 13:
					sPODUsrDBData.un32LastGetLoginRewardDay = atoi(pOneReply->str);
					break;
				case 14:
					sPODUsrDBData.tRegisteUTCMillisec = atoi(pOneReply->str);
					break;
				case 15:
					sPODUsrDBData.un16Cldays = atoi(pOneReply->str);
					break;
				case 16:
					sPODUsrDBData.un8UserLv = atoi(pOneReply->str);
					break;
				case 17:
					sPODUsrDBData.un16VipLv = atoi(pOneReply->str);
					break;
				case 18:
					sPODUsrDBData.un16HeaderID = atoi(pOneReply->str);
					break;
				case 19:
					sPODUsrDBData.un32TotalAssist = atoi(pOneReply->str);
					break;
				case 20:
					sPODUsrDBData.un32TotalDeadTimes = atoi(pOneReply->str);
					break;
				case 21:
					sPODUsrDBData.un32TotalDestoryBuildings = atoi(pOneReply->str);
					break;
				case 22:
					sPODUsrDBData.un32TotalGameInns = atoi(pOneReply->str);
					break;
				case 23:
					sPODUsrDBData.un32TotalHeroKills = atoi(pOneReply->str);
					break;
				case 24:
					sPODUsrDBData.un32TotalWinInns = atoi(pOneReply->str);
					break;
				case 25:
					sPODUsrDBData.un32UserCurLvExp = atoi(pOneReply->str);
					break;
				case 26:
					sPODUsrDBData.vipScore = atoi(pOneReply->str);
					break;
				case 27:
					if (pOneReply->len == 0){
						break;
					}
					guideStr = string(pOneReply->str, pOneReply->len);
					break;
				case 28:
					sPODUsrDBData.n16Sex = atoi(pOneReply->str);
					break;
				case 29:
					sSUserDBData.szTaskData = string(pOneReply->str, pOneReply->len);
					break;
				}
			}

			if (res != eNormal){
				break;
			}

			pcUser = new CCSUser();
			pcUser->LoadDBData(sSUserDBData);
			pcUser->InitRunes(bagStr, slotStr);
			pcUser->LoadFromRedisStr(heroStr, friendStr, blackStr, itemStr, mailStr);
			pcUser->GetUserBattleInfoEx().mDebugName = sSUserDBData.szUserName;
			pcUser->GetUserDBData().mGuideSteps.szCSContinueGuide = guideStr;

			res = AddUser(pcUser);
			if (res != eNormal){
				delete pcUser;
				ELOG(LOG_ERROR, "Adduer失败!");
				return;
			}

			pcUser->OnOnline(netinfo, sLoginMsg, false, true);
			//从Redis上删除User信息
			RemoveUserFromRedisLRU(*pcUser);
		} while (false);

		if (res != eNormal){
			if (!pcUser){
				delete pcUser;
			}
			ELOG(LOG_SpecialDebug, "没有命中cache,所以查询数据库");
			GetNowWorkActor().EncodeAndSendToDBThread(*pQueryUser, pQueryUser->msgid());
		}
	}


	void CCSUserMgr::redisCheckGiftCallBack(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pData){
		AskCDKGiftData* pAskCDKGiftData = (AskCDKGiftData*)pData;
		auto pUser = GetUserByGuid(pAskCDKGiftData->pGuid);
		if (!pUser){
			ELOG(LOG_ERROR, "未查到玩家:%llu", pAskCDKGiftData->pGuid);
			delete pAskCDKGiftData;
			return;
		}

		if (predisReply->type == REDIS_REPLY_ARRAY){
			for (int i = 0; i < predisReply->elements; ++i){
				auto pOneRedis = predisReply->element[i];
				if (pOneRedis->len == 0){
					ELOG(LOG_WARNNING, "无效CDK");
					//通知客户端，CDK无效
					PostMsgToGC_AskReturn(pUser->GetUserNetInfo(), GCToCS::eMsgToGSToCSFromGC_CDKReq, eEC_InvalidCDKey);
					delete pAskCDKGiftData;
				}
				else{
					auto cdkID = _atoi64(pOneRedis->str);

					// 回调数据
					auto pReids = GetLogicRedisHandler();
					if (pReids){
						pReids->redisAsyncCommand(RedisSCallBack(&CCSUserMgr::redisGetGiftInfoCallBack, this), pAskCDKGiftData, "hmget cdkID:%lld platform items", cdkID);
					}
					else{
						delete pAskCDKGiftData;
					}
				}
			}
		}
	}

	void CCSUserMgr::redisGetGiftInfoCallBack(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pData){
		AskCDKGiftData* pAskCDKGiftData = (AskCDKGiftData*)pData;
		auto pUser = GetUserByGuid(pAskCDKGiftData->pGuid);
		if (!pUser){
			ELOG(LOG_ERROR, "未查到玩家:%llu", pAskCDKGiftData->pGuid);
			delete pAskCDKGiftData;
			return;
		}

		bool bIfGetSuccess = false;	
		do {
			if (predisReply->type == REDIS_REPLY_ARRAY){
				for (int i = 0; i < predisReply->elements; ++i){
					auto pOneRedis = predisReply->element[i];
					switch(i){
					case 1:
						if (pOneRedis->len > 0){
							bIfGetSuccess = true;
							typedef vector< string > split_vector_type;
							split_vector_type sItemVec;
							boost::algorithm::split(sItemVec, pOneRedis->str, boost::algorithm::is_any_of(";"));
							for (int i = 0;i < sItemVec.size(); ++i){
								split_vector_type sItemInfoVec;
								boost::algorithm::split(sItemInfoVec, sItemVec[i], boost::algorithm::is_any_of(","));
								if (sItemInfoVec.size() == 2){
									auto itemID = atoi(sItemInfoVec[0].c_str());
									auto num = atoi(sItemInfoVec[1].c_str());
									ELOG(LOG_DBBUG, "获取item:%d,num:%d", itemID, num);
									bIfGetSuccess = true;
									pUser->NotifyCDKGiftToGC(itemID, num);
								}
							}
						}
						break;
					case 0:
						if (pOneRedis->len != 0){
							int platFrom = atoi(pOneRedis->str);
							if (platFrom != ePlatform_All && platFrom != pUser->GetUserDBData().sPODUsrDBData.eUserPlatform){
								ELOG(LOG_WARNNING, "平台不一致!");
								break;
							}
						}
						break;
					}
				}
			}
		} while (false);

		if (!bIfGetSuccess){
			PostMsgToGC_AskReturn(pUser->GetUserNetInfo(), GCToCS::eMsgToGSToCSFromGC_CDKReq, eEC_InvalidCDKey);
		}
		else{
			auto pReids = GetLogicRedisHandler();
			if (pReids){
				pReids->redisAsyncCommand(NULL, NULL, "del cdk:%s", pAskCDKGiftData->cdk);
			}

			string sql_str = "";
			UpdateCDKeySQL(pUser->GetUserDBData().szUserName, pAskCDKGiftData->cdk, sql_str);
			CSToDB::UpdateCDKeyInfo info;
			info.set_sql_str(sql_str);

			m_CdkeyWrapper->EncodeAndSendToDBThread(info, info.msgid());

			PostMsgToGC_AskReturn(pUser->GetUserNetInfo(), GCToCS::eMsgToGSToCSFromGC_CDKReq, eEC_GetCDKeyGiftSuccess);
		}

		delete pAskCDKGiftData;
	}

	void CCSUserMgr::redisQueryCDKIDCallBack(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pData){
		if (predisReply->type == REDIS_REPLY_STRING){
			m_GiftCDKID = _atoi64(predisReply->str);
			ELOG(LOG_SpecialDebug, "更新m_GiftCDKID=%lld", m_GiftCDKID);
		}
	}

	bool CCSUserMgr::RemoveUserFromRedisLRU(CCSUser& pUser){
		if (GetUserDBCacheRedisHandler() && GetUserDBCacheRedisHandler()->CanbeUsed()){
			GetUserDBCacheRedisHandler()->redisAsyncCommand(NULL, NULL, "del usercache:%llu", pUser.GetGUID());
			ELOG(LOG_DBBUG, "删除Redis Cache guid:%llu", pUser.GetGUID());
			return true;
		}

		return false;
	}

	void CCSUserMgr::InitCDKIDCFromRedis(){
		GetLogicRedisHandler()->redisAsyncCommand(RedisSCallBack(&CCSUserMgr::redisQueryCDKIDCallBack, this), nullptr, "get m_GiftCDKID");
	}


	void CCSUserMgr::InitRedisSucscribe(){
		GetLogicRedisHandler()->redisAsyncCommand(NULL, NULL, "config set notify-keyspace-events Kx");
		GetLogicRedisHandler()->redisAsyncCommand(RedisSCallBack(&CCSUserMgr::redisSubsribeCallBack, this), NULL, "psubscribe __key*__:*");
	}

	const string SubsribeKeyBegin = "__keyspace@0__:";
	const int SubsribeKeyBeginSize = SubsribeKeyBegin.size();
	void CCSUserMgr::redisSubsribeCallBack(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pData){
		string expiredKey;
		if (predisReply->type == REDIS_REPLY_ARRAY){
			for (int i = 2; i < predisReply->elements; ++i){
				if (i == 2){
					//解析key
					auto pOneReply = predisReply->element[i];
					string key(pOneReply->str, pOneReply->len);
					if (key.find(SubsribeKeyBegin) != -1){
						expiredKey = string(pOneReply->str + SubsribeKeyBeginSize, pOneReply->len - SubsribeKeyBeginSize);
					}
				}
				else if (i == 3){
					//解析事件
					auto pOneReply = predisReply->element[i];
					string eventKey(pOneReply->str, pOneReply->len);
					if (eventKey != "expired"){
						ELOG(LOG_WARNNING, "不是redis过期事件!");
						return;
					}
				}
			}
		}


	}

}