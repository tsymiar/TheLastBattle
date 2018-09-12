#include "stdafx.h"
#include "SSWorkThreadMgr.h"
#include "tinyxml.h" 
#include "tinystr.h"
#include "Net/SSBattleNetSessionMgr.h"
#include "../../Share/Net/INetSessionMgr.h"
#include "SSProfileStatics.h"
#include "../ELogging/ELogging/Buffer.h"
#include "../ELogging/ELogging/Timer.h"
#include "../ELogging/ELogging/SSActive.h"
#include "SSCfgMgr.h"
#include "SSBattleMgr.h"
#include "windows.h"

namespace SceneServer{
	CSSWorkThreadMgr::CSSWorkThreadMgr():m_psSSConfig(NULL)
		, m_un32GSBaseIdx(0)
		, m_un32MaxGSNum(0)
		, m_NPCMaxLifeTime(0)
		,m_BattleFinshProtectTimeSpan(60000)
		, m_MaxStatisMsgToShow(5)
		, m_MsgStaticsInterval(60000)
		, m_IfStatics(false)
		, m_RunCounts(0)
		, m_LastReport(0)
		, m_ThreadNum(1)
		, pInitTime(microsec_clock::universal_time())
		, m_MiniBattleObjNum(0){
		m_InitSpanTime = (pInitTime-ptime(date(1970, 1, 1), boost::posix_time::time_duration(0, 0, 0))).total_milliseconds();
		m_BattleTimer.SetInitTime(pInitTime);
		m_BattleTimer.AddTimer(std::bind(&CSSWorkThreadMgr::CheckCSConnect, this, std::placeholders::_1, std::placeholders::_2), c_tSSPingCSCDTick, true);
		m_BattleTimer.AddTimer(std::bind(&CSSWorkThreadMgr::ProfileReport, this, std::placeholders::_1, std::placeholders::_2), 5000, true);
	}
	CSSWorkThreadMgr& CSSWorkThreadMgr::GetInstance(){
		static CSSWorkThreadMgr lCSSWorkThreadMgr;
		return lCSSWorkThreadMgr;
	}

	INT32 CSSWorkThreadMgr::Init(){
		m_psSSConfig = new SSSConfig;
		memset(m_psSSConfig, 0, sizeof(*m_psSSConfig));

        char szPath[MAX_PATH];
        ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
        for (int i = strlen(szPath) - 1; i >= 0; --i)
        {
            if (szPath[i] == '\\')
            {
                szPath[i] = 0;
                break;
            }
        }

        std::string strExeDir = szPath;
        strExeDir += '\\';

		std::string filepath = strExeDir + "SSKernel/SSKernelCfg.xml";
		TiXmlDocument doc(filepath.c_str());
		bool loadOkay = doc.LoadFile();
		if (!loadOkay){
			printf("load GSCfg.xml failed for %s\n", doc.ErrorDesc());
		}
		else{
			TiXmlElement* m_pXMLEle = doc.RootElement();
			//////////////////////////////////////////////////////////////////////////
			const char* pCSIP= m_pXMLEle->FirstChildElement("CSIP")->GetText();
			memcpy(m_psSSConfig->sCSIP.aszIPAddr, pCSIP, strlen(pCSIP));
			m_psSSConfig->n32CSPort = atoi(m_pXMLEle->FirstChildElement("CSPort")->GetText());
			//////////////////////////////////////////////////////////////////////////
			const char* pLogSIP= m_pXMLEle->FirstChildElement("LogAddress")->GetText();
			memcpy(m_psSSConfig->sLogSIP.aszIPAddr, pLogSIP, strlen(pLogSIP));
			m_psSSConfig->n32LogSPort = atoi(m_pXMLEle->FirstChildElement("LogPort")->GetText());
			//////////////////////////////////////////////////////////////////////////
			m_psSSConfig->n32SSID = atoi(m_pXMLEle->FirstChildElement("SSID")->GetText());
			//////////////////////////////////////////////////////////////////////////
			const char* pGSIP= m_pXMLEle->FirstChildElement("GSIP")->GetText();
			memcpy(m_psSSConfig->sGSListenIP.aszIPAddr, pGSIP, strlen(pGSIP));
			m_psSSConfig->n32GSListenPort = atoi(m_pXMLEle->FirstChildElement("GSPort")->GetText());
			const char* pPWD = m_pXMLEle->FirstChildElement("GSPWD")->GetText();
			memcpy(m_psSSConfig->aszUserPwd, pPWD, strlen(pPWD));
			//////////////////////////////////////////////////////////////////////////
			m_NPCMaxLifeTime = atoi(m_pXMLEle->FirstChildElement("NPCMaxLifeTime")->GetText());
			m_MaxStatisMsgToShow = atoi(m_pXMLEle->FirstChildElement("MaxStatisMsgToShow")->GetText());
			m_MsgStaticsInterval = atoi(m_pXMLEle->FirstChildElement("MsgStaticsInterval")->GetText()) * 1000;
			m_IfStatics = atoi(m_pXMLEle->FirstChildElement("IfStatics")->GetText());
			m_ThreadNum = atoi(m_pXMLEle->FirstChildElement("ThreadNum")->GetText());
			m_MiniBattleObjNum = atoi(m_pXMLEle->FirstChildElement("MiniBattleObjNum")->GetText());
			m_BattleFinshProtectTimeSpan = atoi(m_pXMLEle->FirstChildElement("BattleFinshProtectTimeSpan")->GetText());
		}

		SSBattleNetSessionMgr* pNetSession = new SSBattleNetSessionMgr;
		INetSessionMgr::GetInstance()->CreateListener(m_psSSConfig->n32GSListenPort,1024000,10240000,0);
		INetSessionMgr::GetInstance()->CreateConnector(ST_CLIENT_S2C, m_psSSConfig->sCSIP.aszIPAddr, m_psSSConfig->n32CSPort,10240000,10240000,0);
		INetSessionMgr::GetInstance()->CreateConnector(ST_CLIENT_S2Log, m_psSSConfig->sLogSIP.aszIPAddr, m_psSSConfig->n32LogSPort,102400,102400,0);
		return eNormal;
	}

	INT32 CSSWorkThreadMgr::Start(){
		SYSTEM_INFO siSysInfo;
		GetSystemInfo(&siSysInfo);
		m_n32ActorNum = siSysInfo.dwNumberOfProcessors;
		for (int i = 0; i < m_n32ActorNum; ++i){
			BattleLogicThreadActor* pBattleLogicThreadActor = new BattleLogicThreadActor();
			m_pBattleLogicThreadActorVec.push_back(pBattleLogicThreadActor);
			pBattleLogicThreadActor->Start();
			m_ThreadID2KernelMap.insert(std::make_pair(pBattleLogicThreadActor->m_pActive->GetThreadID(), pBattleLogicThreadActor));
		}
		return 0;
	}

	SSSConfig& CSSWorkThreadMgr::GetSSConfig(){
		return *m_psSSConfig;
	}

	CSSWorkThreadMgr::~CSSWorkThreadMgr(){
		if (m_psSSConfig){
			delete m_psSSConfig;
		}
	}

	INT32 CSSWorkThreadMgr::PostMsgToGS(INT32 n32NSID, google::protobuf::Message& sMsg, int n32MsgID){
		string sData = sMsg.SerializeAsString();
		INetSessionMgr::GetInstance()->TranMsgToSession(ST_SERVER_SS, n32NSID, sData.c_str(), sData.size(), n32MsgID, 0, 0);
		return eNormal;
	}

	INT32 CSSWorkThreadMgr::PostGateServerDisconnect(INT32 n32NSID){
		INetSessionMgr::GetInstance()->DisconnectOne(n32NSID);
		return eNormal;
	}

	INT32 CSSWorkThreadMgr::PostMsgToCS(google::protobuf::Message& sMsg, int n32MsgID){
		INetSessionMgr::GetInstance()->SendMsgToSession(ST_CLIENT_S2C, 0, sMsg, n32MsgID);
		return eNormal;
	}

	CSSGSInfo* CSSWorkThreadMgr::GetGSInfoByNSID(INT32 n32NSID){
		GSNetInfoListMap::iterator iter = m_psGSNetInfoList.find(n32NSID);
		if (m_psGSNetInfoList.end() != iter){
			return iter->second;
		}
		ELOG(LOG_ERROR, "");
		return NULL;
	}

	CSSGSInfo* CSSWorkThreadMgr::GetGSInfoByGSID(INT32 n32GSID){
		GSInfoMap::iterator iter = m_pcGSInfoList.find(n32GSID);
		if (m_pcGSInfoList.end() != iter){
			return iter->second;
		}
		ELOG(LOG_ERROR, "");
		return NULL;
	}

	INT32 CSSWorkThreadMgr::PostMsgToGC(const ISSGSInfo *cpiGSInfo, INT32 n32GCNSID, google::protobuf::Message& sMsg, int n32MsgID){
		if (NULL == cpiGSInfo){
			return eEC_NullMsg;
		}

		SUserNetInfo sTempUserNetInfo;
		sTempUserNetInfo.n32GSID = cpiGSInfo->GetGSID();
		sTempUserNetInfo.n32GCNSID = n32GCNSID;

		return PostMsgToGC(sTempUserNetInfo, sMsg, n32MsgID);
	}

	INT32 CSSWorkThreadMgr::PostMsgToGC(const SUserNetInfo& cpsUserNetInfo, google::protobuf::Message& sMsg, int n32MsgID){
		return PostMsgToGCLIULU(cpsUserNetInfo, sMsg, n32MsgID);
	}

	INT32 CSSWorkThreadMgr::PostMsgToGCLIULU(const SUserNetInfo& cpsUserNetInfo, google::protobuf::Message& sMsg, int n32MsgID){
		if (!cpsUserNetInfo.IsValid()){
			return eNormal;
		}
		ISSGSInfo* pGSInfo = CSSWorkThreadMgr::GetInstance().GetGSInfoByGSID(cpsUserNetInfo.n32GSID);
		if (NULL == pGSInfo){
			ELOG(LOG_ERROR," the error GSID:%d",cpsUserNetInfo.n32GSID);
			return eEC_NullPointer;
		}
		Assert(pGSInfo->GetNSID()>0 && cpsUserNetInfo.n32GCNSID>0);
		CSSProfileStatics::GetInstance().AddMsg(n32MsgID);
		string sData = sMsg.SerializeAsString();
		INetSessionMgr::GetInstance()->TranMsgToSession(ST_SERVER_SS, pGSInfo->GetNSID(), sData.c_str(), sData.size(), n32MsgID, SSToGS::eMsgToGSFromSS_OrderPostToGC, cpsUserNetInfo.n32GCNSID);
		return eNormal;
	}

	INT32 CSSWorkThreadMgr::PostMsgToGCLIULU(const SUserNetInfo& cpsUserNetInfo, std::string& sSerStr, int n32MsgID){
		if (!cpsUserNetInfo.IsValid()){
			return eNormal;
		}
		ISSGSInfo* pGSInfo = CSSWorkThreadMgr::GetInstance().GetGSInfoByGSID(cpsUserNetInfo.n32GSID);
		if (NULL == pGSInfo){
			ELOG(LOG_ERROR," the error GSID:%d",cpsUserNetInfo.n32GSID);
			return eEC_NullPointer;
		}
		Assert(pGSInfo->GetNSID()>0 && cpsUserNetInfo.n32GCNSID>0);
		CSSProfileStatics::GetInstance().AddMsg(n32MsgID);
		INetSessionMgr::GetInstance()->TranMsgToSession(ST_SERVER_SS, pGSInfo->GetNSID(), sSerStr.c_str(), sSerStr.size(), n32MsgID, SSToGS::eMsgToGSFromSS_OrderPostToGC, cpsUserNetInfo.n32GCNSID);
		return eNormal;
	}

	INT32 CSSWorkThreadMgr::PostMsgToGC_KickoutGC(const ISSGSInfo *cpiGSInfo, INT32 n32GCNSID){
		if (NULL == cpiGSInfo){
			return eEC_NullPointer;
		}
		SSToGS::OrderKickoutGC sMsg;
		sMsg.set_gsnid(n32GCNSID);

		PostMsgToGS(cpiGSInfo->GetNSID(), sMsg, sMsg.mgsid());
		return eNormal;
	}

	INT32 CSSWorkThreadMgr::OnMsgFromGS_GSAskRegiste(INT32 n32NSID, const char* pMsg, int n32MsgLength){
		auto_ptr<GSToSS::AskRegiste> pRegister(new GSToSS::AskRegiste);
		pRegister->ParseFromArray(pMsg, n32MsgLength);

		INT32 n32RetFlag = eNormal;

		CSSGSInfo *pcInfo = CSSWorkThreadMgr::GetInstance().GetGSInfoByGSID(pRegister->gsid());
		if (NULL == pcInfo){
			ELOG(LOG_ERROR, "");
			n32RetFlag = eEC_InvalidGSID;
		}

		if (eNormal == n32RetFlag){
			INT32 n32Cmp = strcmp(pcInfo->m_szUserPwd, pRegister->pwd().c_str());
			if (0 != n32Cmp){
				n32RetFlag = eEC_InvalidUserPwd;
			}
		}

		if (eNormal == n32RetFlag){
			pcInfo->m_eGSNetState = eSNS_Free;
			pcInfo->m_n32NSID = n32NSID;
			pcInfo->m_un32ConnTimes++;
			pcInfo->m_tLastConnMilsec = GetUTCMiliSecond();
			CSSWorkThreadMgr::GetInstance().m_psGSNetInfoList[n32NSID] = pcInfo;
			ELOG(LOG_INFO, "GS %d registed at net session %d.", pRegister->gsid(), n32NSID);
		}
		else{
			ELOG(LOG_ERROR, "");
		}

		return n32RetFlag;
	}

	void CSSWorkThreadMgr::Update(){
		while(true){
			INetSessionMgr::GetInstance()->Update();

			CheckDelBattleCallbackQueue();

			++m_RunCounts;

			m_BattleTimer.Run();

			Sleep(1);
		}
	}

	void	CSSWorkThreadMgr::CheckCSConnect(int64_t tUTCMilsec, int64_t tTickSpan){
		SSToCS::AskPing sMsg;
		sMsg.set_time(tUTCMilsec);
		CSSWorkThreadMgr::GetInstance().PostMsgToCS(sMsg, sMsg.mgsid());
	}

	void CSSWorkThreadMgr::OnEvent(UINT32 event_type,PVOID params){
		if (event_type == EVT_ON_CS_REGISTERED)
		{
			if (!CSSCfgMgr::GetSSCfgInstance().IfInitAll())
			{
				SSToCS::AskAllCfgList sAskAllCfgList;
				CSSWorkThreadMgr::GetInstance().PostMsgToCS(sAskAllCfgList, sAskAllCfgList.mgsid());
				SSToCS::GameLogSwitch pLogSwitch;
				CSSWorkThreadMgr::GetInstance().PostMsgToCS(pLogSwitch, pLogSwitch.mgsid());

				CSSCfgMgr::GetSSCfgInstance().SetInitAll(true);
			}
		}
		else if (event_type == EVT_SS2CS_SyncAllGameInfo){
			SSToCS::SyncAllBattleInfo msg;
			msg.set_maxbattleid(0);
			PostMsgToCS(msg, msg.mgsid());
		}
	}

	void CSSWorkThreadMgr::NotifyDelBattle(INT64 battleid){
		ELOG(LOG_SpecialDebug,"结束战场(%u)",battleid);
		m_ToDelBattleCallbackQueue.push(battleid);
	}

	void CSSWorkThreadMgr::ProfileReport(int64_t tUTCMilsec, int64_t tTickSpan){
		DWORD fps = 1000.0f*m_RunCounts/(5000+tUTCMilsec-m_LastReport);
		m_LastReport = tUTCMilsec;
		wstringstream strTitle;
		strTitle << "ss" << " id=" << CSSWorkThreadMgr::GetInstance().GetSSConfig().n32SSID << " fps=" << fps<< " AllBattle=" << m_BattleID2ActorIDMap.size() 
			<< " AllUser="  << m_User2ActorMapByGuid.size()
			<< " NetUser=" << m_User2ActorMapByNetInfo.size();
		SetConsoleTitle(strTitle.str().c_str());
		m_RunCounts = 0;
	}

	int64_t CSSWorkThreadMgr::GetUTCTimeOfPerThread(){
		return (microsec_clock::universal_time() - pInitTime).total_milliseconds();
	}

	int64_t CSSWorkThreadMgr::AddTimer(HeartbeatCallback pHeartbeatCallback, int64_t interval, bool ifPersist){
		BattleLogicThreadActor* pActor = GetActorByThreaID();
		return pActor->m_pActive->GetTimer().AddTimer(pHeartbeatCallback, interval, ifPersist);
	}

	BattleLogicThreadActor* CSSWorkThreadMgr::GetActorByThreaID(){
		auto iter = m_ThreadID2KernelMap.find(GetCurrentThreadId());
		if (iter != m_ThreadID2KernelMap.end()){
			return iter->second;
		}

		return NULL;
	}

	void CSSWorkThreadMgr::RemoveTimer(int64_t timerID){
		BattleLogicThreadActor* pActor = GetActorByThreaID();
		return pActor->m_pActive->GetTimer().RemoveTimer(timerID);
	}

	INT32 CSSWorkThreadMgr::PostMsgToLogServer(google::protobuf::Message& sMsg, int n32MsgID){
		INetSessionMgr::GetInstance()->SendMsgToSession(ST_CLIENT_S2Log, 0, sMsg, n32MsgID);
		return eNormal;
	}

	void CSSWorkThreadMgr::CheckDelBattleCallbackQueue(){
		while(!m_ToDelBattleCallbackQueue.empty()){
			INT64 battleid = 0;
			m_ToDelBattleCallbackQueue.try_pop(battleid);
			BattleID2ActorIDMap::iterator iter = m_BattleID2ActorIDMap.find(battleid);
			if (iter != m_BattleID2ActorIDMap.end()){
				BattleActor& lBattleActorAndUser = iter->second;
				for (int i = 0; i < lBattleActorAndUser.guidVec.size(); ++i){
					UserActor* pUserActor = NULL;
					User2ActorMapByGuid::iterator guiditer = m_User2ActorMapByGuid.find(lBattleActorAndUser.guidVec[i]);
					if (guiditer != m_User2ActorMapByGuid.end()){
						ELOG(LOG_SpecialDebug,"战场结束(%u)玩家(%u)移除",battleid, lBattleActorAndUser.guidVec[i]);
						pUserActor = guiditer->second;
						m_User2ActorMapByNetInfo.erase(pUserActor->sNetInfo);
						m_User2ActorMapByGuid.erase(guiditer);
						delete pUserActor;
					}
					else{
						Assert(false && "the battle can't find user....");
					}
				}
				m_BattleID2ActorIDMap.erase(iter);
			}
		}
	}
}


