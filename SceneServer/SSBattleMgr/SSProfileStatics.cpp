#include "StdAfx.h"
#include "SSProfileStatics.h"
#include <iostream>
#include <fstream>
#include "SSWorkThreadMgr.h"

#include <iomanip> 

namespace SceneServer{
CSSProfileStatics::CSSProfileStatics(void):m_LastMsgShow(0)
{
}


CSSProfileStatics::~CSSProfileStatics(void)
{
}

void CSSProfileStatics::Begin(StaticsType aType){
	auto iter = m_ProfileMap.find(aType);
	TIME_MILSEC now = GetWinMiliseconds();
	if (iter == m_ProfileMap.end()){
		ProfileData lProfileData;
		lProfileData.beginTime = now;
		switch(aType){
		case StaticsType_NPCLookEnemy:
			lProfileData.debugString = "NPCLookEnemy";
			break;
		case StaticsType_Move:
			lProfileData.debugString = "Move";
			break;
		case StaticsType_Sight:
			lProfileData.debugString = "Sight";
			break;
		}
		m_ProfileMap.insert(std::make_pair(aType, lProfileData));
	}
	else{
		iter->second.beginTime = now;
	}
}

void CSSProfileStatics::End(StaticsType aType){
	auto iter = m_ProfileMap.find(aType);
	TIME_MILSEC now = GetWinMiliseconds();
	if (iter != m_ProfileMap.end()){
		iter->second.tottime += now - iter->second.beginTime;
		iter->second.beginTime = 0;
	}
}

CSSProfileStatics& CSSProfileStatics::GetInstance(){
	static CSSProfileStatics lCSSProfileStatics;
	return lCSSProfileStatics;
}

void CSSProfileStatics::GetProfileReport(wstringstream &report){
	if (!CSSWorkThreadMgr::GetInstance().m_IfStatics){
		return;
	}

	//if (CSSWorkThreadMgr::GetInstance().GetKernel()->GetHeartBeatUTCMilsec() - m_LastMsgShow < CSSWorkThreadMgr::GetInstance().m_MsgStaticsInterval){
	//	return;
	//}

	stringstream lTestStream;
	//m_LastMsgShow = CSSWorkThreadMgr::GetInstance().GetKernel()->GetHeartBeatUTCMilsec();

	//for (auto iter = m_ProfileMap.begin(); iter != m_ProfileMap.end(); ++iter){
	//	ProfileData& lProfileData = iter->second;
	//	if (lProfileData.tottime != 0){
	//		lTestStream << " " << setw(7) << lProfileData.debugString.c_str() << ":"  << lProfileData.tottime;
	//		lProfileData.tottime = 0;
	//	}
	//}

	int i = 0;
	set<MsgInterval> tempSet;
	for (auto iter = m_TotNumForPerMsg.begin(); iter != m_TotNumForPerMsg.end(); ++iter){
		MsgInterval lMsgInterval(iter->first, iter->second);
		tempSet.insert(lMsgInterval);
	}
	int max = CSSWorkThreadMgr::GetInstance().m_MaxStatisMsgToShow;
	for (auto iter = tempSet.begin(); iter != tempSet.end(); ++iter){
		if (i > max){
			break;
		}
		++i;
		lTestStream << " " << setw(7) << iter->msgid << "," << iter->msgnum;
	}
	if (!lTestStream.str().empty()){
		ELOG(LOG_SpecialDebug, "%s", lTestStream.str().c_str());
	}
}

void CSSProfileStatics::AddMsg(int Msgid){
	if (!CSSWorkThreadMgr::GetInstance().m_IfStatics){
		return;
	}

	auto iter = m_TotNumForPerMsg.find(Msgid);
	if (iter == m_TotNumForPerMsg.end()){
		m_TotNumForPerMsg.insert(std::make_pair(Msgid, 1));
	}
	else{
		++iter->second;
	}
}


ProfileInScope::ProfileInScope(StaticsType aType):m_Type(aType){
	if (!CSSWorkThreadMgr::GetInstance().m_IfStatics){
		return;
	}

	CSSProfileStatics::GetInstance().Begin(m_Type);
}

ProfileInScope::~ProfileInScope(){
	if (!CSSWorkThreadMgr::GetInstance().m_IfStatics){
		return;
	}

	CSSProfileStatics::GetInstance().End(m_Type);
}

}