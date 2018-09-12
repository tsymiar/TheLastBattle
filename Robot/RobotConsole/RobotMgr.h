#pragma once
#include <map>
#include "tinyxml.h" 
#include "tinystr.h"
#include "Robot.h"
using namespace LuaPlus;
class Buffer;

namespace ReBot
{
	class CCClient;
	class CRebotHero;
	class CRebortNPC;

	class CRobotMgr
	{
	public:
		friend class CCClient;
		CRobotMgr(void);
		virtual ~CRobotMgr(void);
		std::default_random_engine  m_Generator;
		TIME_MILSEC		m_tLastHearBeat;
		TIME_MILSEC		m_tLastShowLogTime;
		INT32			m_n32RobotInitID;
		INT32			m_n32RobotNum;
		INT32           m_SkipLoginToGS;
		INT32			m_PlatForm;
		DWORD			m_TickCount;
		vector<CCClient*>	m_vRobotList;
		typedef vector<CVector3D> RoadVec;
		static RoadVec		m_HeroRoadVec;
		static boost::atomic_int	m_n32ConnectingRobotNum;
		TIME_MILSEC tLastHeartBeatMilsec;
		INT32		Init();
		INT32		Start();
		INT32		Stop();
		void		SetIpLinkInfo(map<EServerLinkType,SessionInfo *>& value){m_IpLinkInfo = value;}
		CCClient*	GetRobotByID(INT32 n32ID);
		DWORD		GetTickCount() { return m_TickCount; }
		INT32		PostConnect(CCClient* pClient);
		INT32		PostMsg(INT32 n32NSID, google::protobuf::Message& sMsg,int n32MsgID,EServerLinkType linkType = eServerLink_GS);
		void		Update(DWORD curTime,DWORD spanTime);
		void		SetRobotInitID(INT32 value){m_n32RobotInitID = value;}
		vector<CVector3D>& GetLinePath();
		CVector3D GetRandPathNode();
		CVector3D PosToVec(const GSToGC::Pos& pos);
		CVector3D DirToVec(const GSToGC::Dir& dir);
		boost::atomic_int*	GetState();
		BOOLEAN	ExecuteScriptFunc(const char* pszFuncName, const void* un32ParamArray, INT32 nParamCount, INT32& n32Ret, BOOLEAN bWarning, ELuaParamType paramType = eELua_Param_Number);
		void ReadServerLinkInfo(TiXmlDocument doc);
		INT32 GetRandIndexOnce(INT32 n32Max);

	public:
		map<EServerLinkType,SessionInfo *> m_IpLinkInfo;
		boost::atomic_int	m_State[RS_AskEnd];
		map<UINT32,bool> mHadPostLoginRobot;
	};

	extern CRobotMgr* gRobotMgr;
}
