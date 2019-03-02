#include "stdafx.h"
#include "CSKernel.h"
#include <iostream>
#include <fstream> 
#include "tinystr.h"
#include "boost/tokenizer.hpp"
#include "boost/algorithm/string.hpp"
#include "CSBattleMgr.h"
#include "CSUserMgr.h"
#include "CSCfgMgr.h"

using namespace std;

namespace CentralServer{
static CCSKernel* pInstance = nullptr;
CCSKernel::CCSKernel():m_RunCounts(0)
	, m_LastReport(0)
{
	timeBeginPeriod(1);
	m_pcSSInfoList = NULL;
	m_psGSNetInfoList = NULL;
	m_psSSNetInfoList = NULL;
	m_pcGSInfoList = NULL;
	memset(&m_sCSKernelCfg, 0, sizeof(SCSKernelCfg));

	m_tHeartBeartUTCMilsec = 0;

	m_BattleTimer.SetInitTime(GetInitPtime());

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

    m_strExeDir = szPath;
    m_strExeDir += '\\';
}

CCSKernel::~CCSKernel()
{
	timeEndPeriod(1);
}

typedef vector< string > split_vector_type;
INT32	CCSKernel::LoadCfg()
{
	std::string filepath = m_strExeDir + "./CSConfig/CSKernelCfg.xml";
	TiXmlDocument doc(filepath.c_str());
	bool loadOkay = doc.LoadFile();
	if (!loadOkay){
		printf("load GSCfg.xml detail: %s\n", doc.ErrorDesc());
	}
	else{
		TiXmlElement* m_pXMLEle = doc.RootElement();
		m_sCSKernelCfg.unCSId = atoi(m_pXMLEle->FirstChildElement("GameWorldID")->GetText());
		m_sCSKernelCfg.ipaddress  = string(m_pXMLEle->FirstChildElement("CSIP")->GetText());

		m_sCSKernelCfg.n32SSNetListenerPort = atoi(m_pXMLEle->FirstChildElement("SSPort")->GetText());
		m_sCSKernelCfg.n32GSNetListenerPort = atoi(m_pXMLEle->FirstChildElement("GSPort")->GetText());

		m_sCSKernelCfg.un32MaxSSNum = atoi(m_pXMLEle->FirstChildElement("MaxSSNum")->GetText());
		m_sCSKernelCfg.un32SSBaseIdx = atoi(m_pXMLEle->FirstChildElement("SSBaseIndex")->GetText());
		m_sCSKernelCfg.un32MaxGSNum = atoi(m_pXMLEle->FirstChildElement("MaxGSNum")->GetText());
		m_sCSKernelCfg.un32GSBaseIdx = atoi(m_pXMLEle->FirstChildElement("GSBaseIndex")->GetText());
		m_sCSKernelCfg.maxWaitingDBNum = atoi(m_pXMLEle->FirstChildElement("WaitingDBNum")->GetText());

		m_sCSKernelCfg.redisAddress = string(m_pXMLEle->FirstChildElement("redisAddress")->GetText());
		m_sCSKernelCfg.redisPort = atoi(m_pXMLEle->FirstChildElement("redisPort")->GetText());

		m_sCSKernelCfg.redisLogicAddress = string(m_pXMLEle->FirstChildElement("redisLogicAddress")->GetText());
		m_sCSKernelCfg.redisLogicPort = atoi(m_pXMLEle->FirstChildElement("redisLogicPort")->GetText());

		m_sCSKernelCfg.LogAddress = string(m_pXMLEle->FirstChildElement("LogAddress")->GetText());
		m_sCSKernelCfg.LogPort = atoi(m_pXMLEle->FirstChildElement("LogPort")->GetText());

		const string ssIndexStr(m_pXMLEle->FirstChildElement("AllSSIndex")->GetText());
		split_vector_type ssIndexVec;
		boost::algorithm::split( ssIndexVec, ssIndexStr, boost::algorithm::is_any_of(";") );
		m_pcSSInfoList = new CCSSSInfo[ssIndexVec.size()];

		for (size_t i = 0; i != ssIndexVec.size(); ++i){
			split_vector_type ssInfoVec;
			boost::algorithm::split(ssInfoVec, ssIndexVec[i], boost::algorithm::is_any_of(",") );
			if (ssInfoVec.size() != 3){
				ELOG(LOG_ERROR, "Parser error!");
			}
			else{
				m_pcSSInfoList[i].m_n32SSID = atoi(ssInfoVec[0].c_str());
				strcpy(m_pcSSInfoList[i].m_szName, ssInfoVec[1].c_str());
				strcpy(m_pcSSInfoList[i].m_szUserPwd, ssInfoVec[2].c_str());
			}
		}

		const string gsIndexStr(m_pXMLEle->FirstChildElement("AllGSIndex")->GetText());
		split_vector_type gsIndexVec;
		boost::algorithm::split( gsIndexVec, gsIndexStr, boost::algorithm::is_any_of(";") );
		m_pcGSInfoList = new CCSGSInfo[gsIndexVec.size()];

		for (size_t i = 0; i != gsIndexVec.size(); ++i){
			split_vector_type gsInfoVec;
			boost::algorithm::split(gsInfoVec, gsIndexVec[i], boost::algorithm::is_any_of(",") );
			if (gsInfoVec.size() != 3){
				ELOG(LOG_ERROR, "Parser error!");
			}
			else{
				m_pcGSInfoList[i].m_n32GSID = atoi(gsInfoVec[0].c_str());
				strcpy(m_pcGSInfoList[i].m_szName, gsInfoVec[1].c_str());
				strcpy(m_pcGSInfoList[i].m_szUserPwd, gsInfoVec[2].c_str());
			}
		}

		m_sCSKernelCfg.n32RCNetListenerPort = atoi(m_pXMLEle->FirstChildElement("RSPort")->GetText()); 

		strcpy(m_szRemoteConsolekey, m_pXMLEle->FirstChildElement("RSKey")->GetText()); 
	}

	return eNormal;
}

INT32	CCSKernel::UnloadCfg()
{
	if (NULL != m_pcSSInfoList)
	{
		delete[] m_pcSSInfoList;
		m_pcSSInfoList = NULL;
	}
	if (NULL != m_pcGSInfoList)
	{
		delete[] m_pcGSInfoList;
		m_pcGSInfoList = NULL;
	}
	memset(m_szRemoteConsolekey,0,c_n32DefaultRemoveConsoleKeyLen*sizeof(char));
	return eNormal;
}
 
INT32	CCSKernel::Initialize()
{
	//JJIAZ加载配置的时候 不要随便调整顺序
	CCSCfgMgr::getInstance().Initalize(); 

	INT32 n32Init = LoadCfg();   
	if (eNormal != n32Init)
	{
		ELOG(LOG_ERROR," loadCfg()............failed!");
		return n32Init;
	}

	if(m_sCSKernelCfg.un32MaxSSNum > 0 )
	{
		m_psSSNetInfoList = new SSSNetInfo[m_sCSKernelCfg.un32MaxSSNum];
		memset(m_psSSNetInfoList, 0, sizeof(SSSNetInfo) * m_sCSKernelCfg.un32MaxSSNum);
	 
		m_psGSNetInfoList = new SGSNetInfo[m_sCSKernelCfg.un32MaxGSNum];
		memset(m_psGSNetInfoList, 0, sizeof(SGSNetInfo) * m_sCSKernelCfg.un32MaxGSNum);

		m_psRCNetInfoList = new SRCNetInfo[10];
	}

	m_GSMsgHandlerMap[GSToCS::eMsgToCSFromGS_AskRegiste] = std::bind(&CCSKernel::OnMsgFromGS_AskRegiste, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_GSMsgHandlerMap[GSToCS::eMsgToCSFromGS_AskPing] = std::bind(&CCSKernel::OnMsgFromGS_AskPing, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_GSMsgHandlerMap[GSToCS::eMsgToCSFromGS_ReportGCMsg] = std::bind(&CCSKernel::OnMsgFromGS_ReportGCMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_AskPing] = std::bind(&CCSKernel::OnMsgFromSS_AskPing, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	AddTimer(std::bind(&CCSKernel::ProfileReport, this, std::placeholders::_1, std::placeholders::_2), 5000, true);
	
	return eNormal;
}

INT32	CCSKernel::Start()
{
	CSNetSessionMgr* pNetSession = new CSNetSessionMgr;

	GetBattleMgrInstance()->RegisterMsgHandle(m_SSMsgHandlerMap, m_GSMsgHandlerMap,  m_GCMsgHandlerMap, m_RCMsgHandlerMap);
	GetCSUserMgrInstance()->RegisterMsgHandle(m_SSMsgHandlerMap, m_GSMsgHandlerMap,  m_GCMsgHandlerMap, m_RCMsgHandlerMap);

	ELOG(LOG_INFO, "success!");

	return 0;
}

INT32	CCSKernel::Stop()
{
	delete CSNetSessionMgr::GetInstance();

	ELOG(LOG_INFO, "success ");

	return eNormal;
}

INT32	CCSKernel::Uninitialize()
{
	if (NULL != m_psGSNetInfoList)
	{
		delete[] m_psGSNetInfoList;
		m_psGSNetInfoList = NULL;
	}
	if (NULL != m_psSSNetInfoList)
	{
		delete[] m_psSSNetInfoList;
		m_psSSNetInfoList = NULL;
	}
	if (NULL != m_psRCNetInfoList)
	{
		delete[] m_psRCNetInfoList;
		m_psRCNetInfoList = NULL;
	}

	UnloadCfg();

	ELOG(LOG_INFO, "success ");
	return eNormal;
}

CCSSSInfo*	CCSKernel::GetCSSInfoByNSID(INT32 n32NSID)
{
	for (int i=0;i<GetMaxSSNum();++i)
	{
		CCSSSInfo *pcInfo = m_psSSNetInfoList[i].pcSSInfo;
		if (NULL != pcInfo)
		{
			if (pcInfo->GetNSID()==n32NSID)
			{
				return pcInfo;
			}
		}
	}
	return NULL;
}

CCSSSInfo*	CCSKernel::GetCSSInfoBySSID(INT32 n32SSID){
	INT32 un32Idx = n32SSID - m_sCSKernelCfg.un32SSBaseIdx;
	if (un32Idx < GetMaxSSNum()){
		return &m_pcSSInfoList[un32Idx];
	}
	return NULL;
}

CCSGSInfo*	CCSKernel::GetCGSInfoByNSID(INT32 n32NSID){
	for (int i = 0;i < GetMaxGSNum(); ++i){
		CCSGSInfo *pcInfo = m_psGSNetInfoList[i].pcGSInfo;
		if (NULL != pcInfo){
			if (pcInfo->GetNSID()==n32NSID){
				return pcInfo;
			}
		}
	}
	return NULL;
}

CCSRCInfo*	CCSKernel::GetCRCInfoByNSID(INT32 n32NSID){
	for (int i = 0;i < 10; ++i){
		CCSRCInfo *pcInfo = &(m_psRCNetInfoList[i].cRCInfo);
		if (NULL != pcInfo){
			if (pcInfo->GetNSID()==n32NSID){
				return pcInfo;
			}
		}
	}
	return NULL;
}

CCSRCInfo*	CCSKernel::GetEmptyCRCInfo(){
	for (int i = 0;i < 10; ++i){
		CCSRCInfo *pcInfo = &(m_psRCNetInfoList[i].cRCInfo);
		if (NULL != pcInfo){
			if (pcInfo->GetNSID()==0){
				return pcInfo;
			}
		}
	}
	return NULL;
}

CCSGSInfo*	CCSKernel::GetCGSInfoByGSID(INT32 n32GSID)
{
	CCSGSInfo *pcInfo = NULL;
	UINT32 un32Idx = (UINT32)n32GSID - m_sCSKernelCfg.un32GSBaseIdx;
	if (un32Idx < GetMaxGSNum())
	{
		pcInfo = &m_pcGSInfoList[un32Idx];
	}
	return pcInfo;
}

ICSSSInfo*	CCSKernel::GetSSInfoByNSID(INT32 n32NSID)
{
	return GetCSSInfoByNSID(n32NSID);
}

ICSSSInfo*	CCSKernel::GetSSInfoBySSID(INT32 n32SSID)
{
	return GetCSSInfoBySSID(n32SSID);
}

ICSGSInfo*	CCSKernel::GetGSInfoByNSID(INT32 n32NSID)
{
	return GetCGSInfoByNSID(n32NSID);
}

ICSGSInfo*	CCSKernel::GetGSInfoByGSID(INT32 n32GSID)
{
	return GetCGSInfoByGSID(n32GSID);
}

ICSRCInfo*	CCSKernel::GetRCInfoByNSID(INT32 n32NSID)
{
	return GetCRCInfoByNSID(n32NSID);
}

void CCSKernel::DisconnectOne(INT32 n32NSID)
{
	INetSessionMgr::GetInstance()->DisconnectOne(n32NSID);
}

INT32 CCSKernel::PostMsgToSS(const ICSSSInfo *piInfo, google::protobuf::Message& sMsg, int n32MsgID){
	if (NULL == piInfo) return eEC_NullGateServer;
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_CS_OnlySS, piInfo->GetNSID(), sMsg, n32MsgID);
	return eNormal;
}

INT32 CCSKernel::PostMsgToRC(const ICSRCInfo *piInfo, google::protobuf::Message& sMsg, int n32MsgID){
	if (NULL == piInfo) return eEC_NullGateServer;
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_CS_OnlyRC, piInfo->GetNSID(), sMsg, n32MsgID);
	return eNormal;
}

INT32 CCSKernel::PostMsgToRC(const INT32 nsid, google::protobuf::Message& sMsg, int n32MsgID)
{
	if (0 == nsid) return eEC_NullGateServer;
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_CS_OnlyRC, nsid, sMsg, n32MsgID);
	return eNormal;
}

INT32	CCSKernel::BroadCastMsgToSS(google::protobuf::Message& sMsg, int n32MsgID)
{
	INT32 n32Post = eNormal;
	for (UINT32 i = 0; i <= GetMaxSSNum(); i++)
	{
		CCSSSInfo *psSSInfo = m_psSSNetInfoList[i].pcSSInfo;
		if (NULL == psSSInfo)
		{
			continue;
		}

		PostMsgToSS(psSSInfo, sMsg, n32MsgID);
	}
	return eNormal;
}

//net msg function.

INT32	CCSKernel::PostMsgToGS_KickoutGC(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID){
	CSToGS::OrderKickoutGC sMsg;
	sMsg.set_gcnid(n32GCNSID);
	return PostMsgToGS(cpiGSInfo, 0, sMsg, sMsg.msgid());
}

INT32 CCSKernel::PostMsgToGS(const ICSGSInfo *piInfo, int n32GCNSID, google::protobuf::Message& sMsg, int n32MsgID){
	if (piInfo==NULL) return eNormal;
	string sData = sMsg.SerializeAsString();
	INetSessionMgr::GetInstance()->TranMsgToSession(ST_SERVER_CS_OnlyGS,piInfo->GetNSID(),
		sData.c_str(),sData.size(),n32MsgID,n32GCNSID==0?0:CSToGS::eMsgToGSFromCS_OrderPostToGC,n32GCNSID);
	return eNormal;
}

INT32	CCSKernel::BroadCastMsgToGS(google::protobuf::Message& sMsg, int n32MsgID){
	string sData = sMsg.SerializeAsString();
	INetSessionMgr::GetInstance()->TranMsgToSession(ST_SERVER_CS_OnlyGS,-1,
		sData.c_str(),sData.size(),n32MsgID,0,0);
	return eNormal;
}

INT32	CCSKernel::PostMsgToGC(const SUserNetInfo& cpsUserNetInfo, google::protobuf::Message& sMsg, int n32MsgID){
	if (cpsUserNetInfo.n32GCNSID==0) return eNormal;
	PostMsgToGS(GetGSInfoByGSID(cpsUserNetInfo.n32GSID),cpsUserNetInfo.n32GCNSID,sMsg,n32MsgID);
	return eNormal;
}

CCSKernel& CCSKernel::GetInstance(){
	if (NULL == pInstance){
		pInstance = new CCSKernel;
	}
	return *pInstance;
}

INT32 CCSKernel::PostMsgToLogMgr(google::protobuf::Message& sMsg, int n32MsgID){
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_CLIENT_C2Log, 0, sMsg, n32MsgID);
	return eNormal;
}
}