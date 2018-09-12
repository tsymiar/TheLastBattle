#include "stdafx.h"
#include "RobotMgr.h"
#include "Robot.h"
#include "CfgMgr.h"
#include "Hero.h"
#include "RobotNPC.h"
#include "../ELogging/ELogging/Buffer.h"
#include "Net/RobotSession.h"

namespace ReBot
{
	CRobotMgr* gRobotMgr = nullptr;
	vector<CVector3D>	CRobotMgr::m_HeroRoadVec;
	boost::atomic_int	CRobotMgr::m_n32ConnectingRobotNum(0);

	CRobotMgr::CRobotMgr()
		:m_Generator(static_cast<long int>(time(NULL)))
		,m_TickCount(GetTickCount())
		,m_n32RobotInitID(0)
	{
			m_tLastShowLogTime = 0;
			m_n32RobotNum = 0;
			for (int i = 0; i < RS_AskEnd; ++i){
				m_State[i] = 0;
			}
	}

	CRobotMgr::~CRobotMgr(void){
		m_vRobotList.clear();
	}

	INT32 CRobotMgr::Init()
	{
		const char* filepath = "robot.xml"; 
		TiXmlDocument doc(filepath);
		bool loadOkay = doc.LoadFile();
		Assert(loadOkay);
		TiXmlElement* pXMLEle = doc.RootElement();
		TiXmlElement* numEle = pXMLEle->FirstChildElement("RobotNum");
		m_n32RobotNum = atoi(numEle->GetText());
		m_SkipLoginToGS = atoi(pXMLEle->FirstChildElement("IfSkipToGS")->GetText());
		m_PlatForm = atoi(pXMLEle->FirstChildElement("PlatForm")->GetText());
		// link server info
		TiXmlElement* ipEle = NULL;
		TiXmlElement* portEle = NULL;		
		SESSION_TYPE seType = ST_CLIENT_C2L;
		const char *ipInfo = NULL;
		UINT32 portInfo = 0;
		for(int i = 0;i < (int)eServerLink_Max;i++){
			switch((EServerLinkType)i){
			case eServerLink_LS:
				ipEle = pXMLEle->FirstChildElement("LoginServer"); 
				portEle = pXMLEle->FirstChildElement("LoginPort");
				seType = ST_CLIENT_C2L;
				break;
			case eServerLink_BS:
				ipEle = pXMLEle->FirstChildElement("BalanceServer"); 
				portEle = pXMLEle->FirstChildElement("BalancePort"); 
				seType = ST_CLIENT_C2B;
				break;
			case eServerLink_GS:
				ipEle = pXMLEle->FirstChildElement("DefaultGateServer"); 
				portEle = pXMLEle->FirstChildElement("DefaultGatePort");  
				seType = ST_CLIENT_C2G;
				break;
			default:
				continue;
			}
			if (ipEle) ipInfo = ipEle->GetText();
			else continue;
			if (portEle) portInfo = atoi(portEle->GetText());
			else continue;
			m_IpLinkInfo.insert(make_pair((EServerLinkType)i,new SessionInfo(ipInfo,portInfo,seType))); 
		}
		CCfgMgr::getInstance().Initalize();
		m_HeroRoadVec.push_back(CVector3D(2887.7, 6006.0, 15650.44));
		m_HeroRoadVec.push_back(CVector3D(4257.71, 6006.0, 14893.87));
		m_HeroRoadVec.push_back(CVector3D(5728.48, 6006.0, 14411.84));
		m_HeroRoadVec.push_back(CVector3D(6936.88, 6006.0, 12186.26));
		m_HeroRoadVec.push_back(CVector3D(8534.46, 6006.0, 9691.17));
		m_HeroRoadVec.push_back(CVector3D(11730.0, 6006.0, 7937.0));
		m_HeroRoadVec.push_back(CVector3D(12834.0, 6006.0, 6834.0));
		m_HeroRoadVec.push_back(CVector3D(14024.0, 6006.0, 5644.0));
		m_HeroRoadVec.push_back(CVector3D(15418.0, 6006.0, 4599.0));
		m_HeroRoadVec.push_back(CVector3D(15418.0, 6006.0, 4599.0));
		m_HeroRoadVec.push_back(CVector3D(14024.0, 6006.0, 5644.0));
		m_HeroRoadVec.push_back(CVector3D(14024.0, 6006.0, 5644.0));
		m_HeroRoadVec.push_back(CVector3D(11730.0, 6006.0, 7937.0));
		m_HeroRoadVec.push_back(CVector3D(9836.0, 6006.0, 9831.0));
		m_HeroRoadVec.push_back(CVector3D(8258.0, 6006.0, 11409.0));
		m_HeroRoadVec.push_back(CVector3D(6871.0, 6006.0, 12796.0));
		m_HeroRoadVec.push_back(CVector3D(5802.0, 6006.0, 13865.0));
		m_HeroRoadVec.push_back(CVector3D(4443.0, 6006.0, 15563));
		return 0;
	}

	INT32	CRobotMgr::Start(){
		tLastHeartBeatMilsec = 0;
		m_tLastHearBeat = gRobotMgr->GetTickCount();
		tLastHeartBeatMilsec = m_tLastHearBeat;
		m_tLastShowLogTime = m_tLastHearBeat;
		for (INT i=0; i<m_n32RobotNum; ++i){
			m_vRobotList.push_back(new CCClient(this,i+m_n32RobotInitID));
		}
		return 0;
	}

	INT32	CRobotMgr::Stop(){
		for (auto it=m_vRobotList.begin();it!=m_vRobotList.end();++it){
			delete (*it);
		}
		return 0;
	}

	CCClient*	CRobotMgr::GetRobotByID(INT32 n32ID){
		Assert(m_vRobotList.size()>n32ID-m_n32RobotInitID);
		return m_vRobotList[n32ID-m_n32RobotInitID];
	}

	const string EServerLinkTypeStr[]=
	{
		"eServerLink_InitType",
		"eServerLink_LoginType",
		"eServerLink_BalanceType",
		"eServerLink_GateType",
		"eServerLink_Max",
	};

	INT32	CRobotMgr::PostConnect(CCClient* pClient)
	{
		++CRobotMgr::m_n32ConnectingRobotNum;
		//ELOG(LOG_ERROR,"cur ping count(%u)",CRobotMgr::m_n32ConnectingRobotNum);
		EServerLinkType linkType = eServerLink_None;
		if (pClient->mLoginState==RLS_LsHadGetBsAddress) { pClient->mLoginState=RLS_BsConnecting; linkType=eServerLink_BS; }
		else if (pClient->mLoginState==RLS_BsHadGetGsAddress || pClient->mLoginState==RLS_GsHadGetBaseInfo) { pClient->mLoginState=RLS_GsConnecting; linkType=eServerLink_GS; }
		else { pClient->mLoginState=RLS_LsConnecting;  linkType=eServerLink_LS; }
		map<EServerLinkType,SessionInfo *>::iterator iter = m_IpLinkInfo.find(linkType);
		Assert(iter!=m_IpLinkInfo.end());
		SessionInfo *info = iter->second;
		ELOG(LOG_DBBUG,"robot:%d Type(%s)",pClient->m_un32RobotID, EServerLinkTypeStr[linkType].c_str());
		//////////////////////////////////////////////////////////////////////////
		if (pClient->m_n32NSID>0){
			INetSession* pCli = INetSessionMgr::GetInstance()->GetSession(pClient->m_n32NSID);
			CRobotSession* pCliSession = dynamic_cast<CRobotSession*>(pCli);
			Assert(pCliSession!=NULL);
			pCliSession->SetType(info->ipSession);
			pCliSession->Reconnect(/*linkType==eServerLink_GS?pClient->m_gateIp.c_str():*/info->ipAddr.c_str(),
				linkType==eServerLink_GS?pClient->m_gatePort:info->ipPort);
		}
		else{
			INetSessionMgr::GetInstance()->CreateConnector(info->ipSession,
				/*linkType==eServerLink_GS?pClient->m_gateIp.c_str():*/info->ipAddr.c_str(),
				linkType==eServerLink_GS?pClient->m_gatePort:info->ipPort,10240,10240,pClient->m_un32RobotID);
		}
		pClient->m_SLinkState = linkType;
		pClient->m_bSendConnect = true;
		return eNormal;
	}

	//pb±‰∂Ø£¨protobuf÷ÿ‘ÿ
	INT32	CRobotMgr::PostMsg(INT32 n32NSID, google::protobuf::Message& sMsg,int n32MsgID,EServerLinkType linkType)
	{
		map<EServerLinkType,SessionInfo *>::iterator iter = m_IpLinkInfo.find(linkType);
		Assert(iter!=m_IpLinkInfo.end());
		INetSessionMgr::GetInstance()->SendMsgToSession(iter->second->ipSession,n32NSID,sMsg,n32MsgID);		
		return eNormal;
	}

	void CRobotMgr::Update(DWORD curTime,DWORD spanTime)
	{
		m_TickCount = curTime;
		srand(time(NULL));
		for (INT32 i = 0; i < m_n32RobotNum; i++){
			m_vRobotList[i]->OnHeartBeat(curTime);
		}
	}

	CVector3D CRobotMgr::GetRandPathNode(){
		INT32 n32Index = GetRandIndexOnce(m_HeroRoadVec.size());
		return m_HeroRoadVec[n32Index];
	}

	vector<CVector3D>& CRobotMgr::GetLinePath(){
		return m_HeroRoadVec;
	}

	CVector3D CRobotMgr::PosToVec(const GSToGC::Pos& pos){
		return CVector3D(pos.x(), 6000.0f, pos.z());
	}

	CVector3D CRobotMgr::DirToVec(const GSToGC::Dir& dir){
		float angle = (float)(dir.angle()) / 10000;
		return CVector3D(cos(angle), 0, sin(angle));
	}

	boost::atomic_int* CRobotMgr::GetState(){
		for (int i = 0; i < RS_AskEnd; ++i){
			m_State[i] = 0;
		}
		for (INT32 i = 0; i < m_n32RobotNum; i++)
		{
			++m_State[m_vRobotList[i]->m_eAskState];
		}
		return m_State;
	}

	INT32 CRobotMgr::GetRandIndexOnce( INT32 n32Max )
	{
		if (n32Max<1) n32Max = 1;
		std::uniform_int_distribution<INT32> distribution(0, n32Max - 1);
		return distribution(m_Generator);
	}

}