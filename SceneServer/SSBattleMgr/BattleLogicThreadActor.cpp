#include "SSWorkThreadMgr.h"
#include "../ELogging/ELogging/SSActive.h"
#include "../ELogging/ELogging/Timer.h"
#include "SSBattleMgr.h"

namespace SceneServer{
	BattleLogicThreadActor::BattleLogicThreadActor():m_BattleNum(0)
		, m_pActive(NULL)
		, m_pBattleMgr(new CSSBattleMgr()){
		m_pActive = Active::CreateActive(std::bind(&BattleLogicThreadActor::Handler, this, std::placeholders::_1), 1024, 1024);
		m_pActive->GetTimer().SetInitTime(CSSWorkThreadMgr::GetInstance().GetInitTime());
		m_pActive->GetTimer().AddTimer(std::bind(&CSSBattleMgr::OnBattleHeartBeat, m_pBattleMgr, std::placeholders::_1, std::placeholders::_2), 100, true);
	}

	BattleLogicThreadActor* CSSWorkThreadMgr::GetMiniLoadActor(){
		ActorVec::iterator miniIter = m_pBattleLogicThreadActorVec.begin();

		for (ActorVec::iterator iter = miniIter + 1; iter != m_pBattleLogicThreadActorVec.end(); ++iter){
			if ((*miniIter)->GetPendingWorkNum() > (*iter)->GetPendingWorkNum()){
				miniIter = iter;
			}
		}

		return (*miniIter);
	}

	bool BattleLogicThreadActor::EncodeProtoMsgToBuffer(google::protobuf::Message& sMsg, int n32MsgID, Buffer& pBuffer){
		return true;
	}

	void CSSWorkThreadMgr::SendCSMsgToLogicThread(const char* pMsg, int n32MsgLength, int n32MsgID){
 
		if (n32MsgID >= CSToSS::eMsgToSSFromCS_NotifyBuffCfgList && n32MsgID < CSToSS::eMsgToSSFromCS_NotifyOBEnterBattle){
			CSSCfgMgr::GetSSCfgInstance().HandleMsg(pMsg, n32MsgLength, n32MsgID);
			return;
		}
		switch(n32MsgID){
		case CSToSS::eMsgToSSFromCS_AskPingRet:
			{
				std::auto_ptr<CSToSS::PingRet> pAskPingRet(new CSToSS::PingRet);
				pAskPingRet->ParseFromArray(pMsg, n32MsgLength);
				TIME_MILSEC tMilsecSpan = GetUTCMiliSecond() - pAskPingRet->time();
				return;
			}
		case CSToSS::eMsgToSSFromCS_CreateBattle:
			{
				BattleLogicThreadActor* pActive = GetMiniLoadActor();
				Assert(pActive);
				ELOG(LOG_DBBUG, "Send To %d actor and now it hv %d battle", pActive->GetActorID(), pActive->GetPendingWorkNum());
				pActive->AddWorkNum();
				boost::shared_ptr<CSToSS::CreateBattle> pCreateBattle = ParseProtoMsgWithNewMemory<CSToSS::CreateBattle>(pMsg, n32MsgLength);
				if (!pCreateBattle) return;
				Assert(m_BattleID2ActorIDMap.find(pCreateBattle->battleid())==m_BattleID2ActorIDMap.end());
				ELOG(LOG_SpecialDebug,"创建战场(%u)",pCreateBattle->battleid());
				vector<SGUID_KEY> guidVec;
				for (int i = 0; i != pCreateBattle->userinfo_size(); ++i){
					const CSToSS::CreateBattle_CreateBattleUserInfo* pUserInfo = &pCreateBattle->userinfo(i);
					if (pUserInfo->ifairobot()) continue;
					SUserNetInfo pSUserNetInfo(pUserInfo->usernetinfo().n32gsid(), pUserInfo->usernetinfo().n32gcnsid());
					SGUID_KEY guid = pUserInfo->guid();
					UserActor* pUserActor = new UserActor();
					pUserActor->pActor = pActive;
					pUserActor->sSGUID_KEY = guid;
					pUserActor->sNetInfo = pSUserNetInfo;
					//////////////////////////////////////////////////////////////////////////
					while(m_User2ActorMapByGuid.find(pUserActor->sSGUID_KEY)!=m_User2ActorMapByGuid.end()) { Sleep(1); CheckDelBattleCallbackQueue(); }
					//////////////////////////////////////////////////////////////////////////
					Assert(m_User2ActorMapByGuid.find(pUserActor->sSGUID_KEY)==m_User2ActorMapByGuid.end());//确认玩家没在队列中//
					Assert(pUserActor->sNetInfo.IsValid());//确认玩家是有网络的//
					Assert(m_User2ActorMapByNetInfo.find(pUserActor->sNetInfo)==m_User2ActorMapByNetInfo.end());//确认玩家网络没在队列中//
					m_User2ActorMapByGuid.insert(std::make_pair(pUserActor->sSGUID_KEY, pUserActor));
					m_User2ActorMapByNetInfo.insert(std::make_pair(pUserActor->sNetInfo, pUserActor));
					guidVec.push_back(guid);
					ELOG(LOG_SpecialDebug,"玩家(useguid:%llu)进入战场(%u)",guid,pCreateBattle->battleid());
				}
				BattleActor lBattleActorAndUser;
				lBattleActorAndUser.guidVec = guidVec;
				lBattleActorAndUser.pActor = pActive;
				m_BattleID2ActorIDMap.insert(std::make_pair(pCreateBattle->battleid(), lBattleActorAndUser));
				pActive->EncodeCSMsgAndSendToLogicThread(pMsg, n32MsgLength, n32MsgID);
			}
			break;
		case CSToSS::eMsgToSSFromCS_OneUserOnline:
			{
				boost::shared_ptr<CSToSS::NotifyUserOnline> pMsgUserOnline = ParseProtoMsgWithNewMemory<CSToSS::NotifyUserOnline>(pMsg, n32MsgLength);
				if (!pMsgUserOnline) break;
				if (m_BattleID2ActorIDMap.find(pMsgUserOnline->battleid())==m_BattleID2ActorIDMap.end()) break;//SS先于CS结束战场
				SGUID_KEY guid = pMsgUserOnline->userguid();
				User2ActorMapByGuid::iterator iter = m_User2ActorMapByGuid.find(guid);
				Assert(iter != m_User2ActorMapByGuid.end());
				Assert(iter->second->sNetInfo.IsValid()==false);
				SUserNetInfo sUserNetInfo(pMsgUserOnline->usernetinfo().n32gsid(), pMsgUserOnline->usernetinfo().n32gcnsid());
				Assert(m_User2ActorMapByNetInfo.find(sUserNetInfo)==m_User2ActorMapByNetInfo.end());
				m_User2ActorMapByNetInfo.insert(std::make_pair(sUserNetInfo,iter->second));
				iter->second->sNetInfo = sUserNetInfo;
				iter->second->pActor->EncodeCSMsgAndSendToLogicThread(pMsg, n32MsgLength, n32MsgID);
				ELOG(LOG_SpecialDebug,"玩家(%u)上线加入战场(%u)",guid, pMsgUserOnline->battleid());
			}
			break;
		case CSToSS::eMsgToSSFromCS_OneUserOffline:
			{
				boost::shared_ptr<CSToSS::NotifyUserOffline> pMsgUserOffline = ParseProtoMsgWithNewMemory<CSToSS::NotifyUserOffline>(pMsg, n32MsgLength);
				if (!pMsgUserOffline) break;
				if (m_BattleID2ActorIDMap.find(pMsgUserOffline->battleid())==m_BattleID2ActorIDMap.end()) break;//SS先于CS结束战场
				SGUID_KEY guid =  pMsgUserOffline->userguid();
				User2ActorMapByGuid::iterator iter = m_User2ActorMapByGuid.find(guid);
				Assert(iter != m_User2ActorMapByGuid.end());
				Assert(iter->second->sNetInfo.IsValid());
				Assert(m_User2ActorMapByNetInfo.find(iter->second->sNetInfo)!=m_User2ActorMapByNetInfo.end());
				m_User2ActorMapByNetInfo.erase(iter->second->sNetInfo);
				iter->second->sNetInfo.Clear();
				iter->second->pActor->EncodeCSMsgAndSendToLogicThread(pMsg, n32MsgLength, n32MsgID);
				ELOG(LOG_SpecialDebug,"玩家(%u)下线离开战场(%u)",guid, pMsgUserOffline->battleid());
			}
			break;
		case CSToSS::eMsgToSSFromCS_NotifyUserInfoChange:
			{
				boost::shared_ptr<CSToSS::NotifyUserInfoChange> pNewMsg = ParseProtoMsgWithNewMemory<CSToSS::NotifyUserInfoChange>(pMsg, n32MsgLength);
				if (!pNewMsg){
					return;
				}
				SGUID_KEY sUserGUID(pNewMsg->guid());
				User2ActorMapByGuid::iterator iter = m_User2ActorMapByGuid.find(sUserGUID);
				if (iter != m_User2ActorMapByGuid.end()){
					iter->second->pActor->EncodeCSMsgAndSendToLogicThread(pMsg, n32MsgLength, n32MsgID);
				}
			}
			break;
		/*case CSToSS::eMsgToSSFromCS_NotifySSGameSwitch:
			for (auto iter = m_BattleID2ActorIDMap.begin(); iter != m_BattleID2ActorIDMap.end(); ++iter){
				iter->second.pActor->EncodeCSMsgAndSendToLogicThread(pMsg, n32MsgLength, n32MsgID);
			}
			break;*/
		default:
			ELOG(LOG_WARNNING, "Not handle cs msgid:%d", n32MsgID);
		}
	}
		void CSSWorkThreadMgr::SendGSMsgToLogicThread(ISSGSInfo *pcGSInfo, int n32GcNetID, const char* pMsg, int n32MsgLength, int n32MsgID){
			SUserNetInfo pSUserNetInfo(pcGSInfo->GetGSID(), n32GcNetID);
			User2ActorMapByNetInfo::iterator iter = m_User2ActorMapByNetInfo.find(pSUserNetInfo);
			if (iter != m_User2ActorMapByNetInfo.end()){
				iter->second->pActor->EncodeGSMsgAndSendToLogicThread(pcGSInfo, n32GcNetID, pMsg, n32MsgLength, n32MsgID);
			}
		}

		bool BattleLogicThreadActor::EncodeGSMsgAndSendToLogicThread(ISSGSInfo *pcGSInfo, int n32GcNetID, const char* pMsg, int n32MsgLength, int n32MsgID){
			Buffer* pBuffer = m_pActive->GetBuffer();
			pBuffer->m_LogLevel = 2;
			pBuffer->append(pcGSInfo->GetGSID());
			pBuffer->append(n32GcNetID);
			pBuffer->append(n32MsgID);
			pBuffer->append(n32MsgLength);
			pBuffer->append(pMsg, n32MsgLength);
			m_pActive->Send(pBuffer);
			return true;
		}

		bool BattleLogicThreadActor::EncodeCSMsgAndSendToLogicThread(const char* pMsg, int n32MsgLength, int n32MsgID){
			Buffer* pBuffer = m_pActive->GetBuffer();
			pBuffer->m_LogLevel = 1;
			pBuffer->append(n32MsgID);
			pBuffer->append(n32MsgLength);
			pBuffer->append(pMsg, n32MsgLength);
			m_pActive->Send(pBuffer);
			return true;
		}

		BattleLogicThreadActor* CSSWorkThreadMgr::GetActorFromBattle(INT64 battleID){
			BattleID2ActorIDMap::iterator iter = m_BattleID2ActorIDMap.find(battleID);
			if (iter != m_BattleID2ActorIDMap.end()){
				return iter->second.pActor;
			}
			return NULL;
		}

		BattleLogicThreadActor* CSSWorkThreadMgr::GetActorFromGUID(SGUID_KEY& guid){
			User2ActorMapByGuid::iterator iter = m_User2ActorMapByGuid.find(guid);
			if (iter != m_User2ActorMapByGuid.end()){
				return iter->second->pActor;
			}
			return NULL;
		}

		int BattleLogicThreadActor::GetPendingWorkNum() const{
			return m_BattleNum;
		}

		int BattleLogicThreadActor::GetActorID(){
			return m_pActive->GetActorID();
		}

		bool CSSWorkThreadMgr::SendMsgFromBattleID(INT64 battleID, const char* pMsg, int n32MsgLength, int n32MsgID){
			BattleLogicThreadActor* pBattleLogicThreadActor = GetActorFromBattle(battleID);
			if (pBattleLogicThreadActor){
				pBattleLogicThreadActor->EncodeCSMsgAndSendToLogicThread(pMsg, n32MsgLength, n32MsgID);
				return true;
			}
			return false;
		}

		void BattleLogicThreadActor::Start(){
			m_pActive->Start();
		}

		void BattleLogicThreadActor::Handler(Buffer*& pBuffer){
			if (pBuffer->m_LogLevel == 1){
				int msgid = 0;
				pBuffer->GetInt(msgid);
				int msglen = 0;
				pBuffer->GetInt(msglen);
				m_pBattleMgr->OnMsgFromCS(pBuffer->GetCurData(), msglen, msgid);
			}
			else if (pBuffer->m_LogLevel == 2){
				int gsid = 0;
				pBuffer->GetInt(gsid);
				int gcid = 0;
				pBuffer->GetInt(gcid);
				int msgid = 0;
				pBuffer->GetInt(msgid);
				int msglen = 0;
				pBuffer->GetInt(msglen);
				ISSGSInfo* pGSInfo = CSSWorkThreadMgr::GetInstance().GetGSInfoByGSID(gsid);
				if (pGSInfo){
					m_pBattleMgr->OnMsgFromGS_ReportGCMsg(pGSInfo, gcid, pBuffer->GetCurData(), msglen, msgid);
				}
			}
		}
}