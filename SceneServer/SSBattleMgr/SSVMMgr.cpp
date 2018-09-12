#if WIN32
#include "stdafx.h"
#endif

#include "SSVMMgr.h"
#include <time.h>
 

namespace SceneServer{
CSSVMBornMgr::CSSVMBornMgr() 
{  
	  m_GuideRunCfg.clear();
}
CSSVMBornMgr::~CSSVMBornMgr() 
{
	 
} 

void	CSSVMBornMgr::ReleaseRunCfg(INT32 nstepId)
{
	auto itr = m_GuideRunCfg.begin();
	for (;itr != m_GuideRunCfg.end(); itr++)
	{
		if (itr->second.stepId == nstepId)
		{
			m_GuideRunCfg.erase(itr);
			break;
		} 
	}
}

void	CSSVMBornMgr::AddGuideVMCfg(INT32 npcId, INT32 nstepId, INT32  cdTime ,INT32  total)
{
	auto itr = m_GuideRunCfg.find(npcId);
	if (itr != m_GuideRunCfg.end())
	{
		itr->second.cdTime = cdTime/1000;
		itr->second.m_BornTime = 0;
		itr->second.stepId = nstepId;
		itr->second.totalBornNum = total;
		itr->second.curtNum += 1;
	} else
	{ 
		tagGuideVmBornCfg tg;
		tg.cdTime = cdTime/1000;
		tg.stepId = nstepId;
		tg.totalBornNum = total;
		tg.curtNum += 1;

		m_GuideRunCfg[npcId] = tg;
	}
}

 
 
void	CSSVMBornMgr::UpdateGuideVMInfo(INT32 npcid)
{
	auto itr = m_GuideRunCfg.find(npcid);
	if (itr != m_GuideRunCfg.end())
	{
		itr->second.m_BornTime = time(NULL) + itr->second.cdTime ;

		itr->second.curtNum -= 1;
	}
}

bool	CSSVMBornMgr::IfGuideBornVM(INT32 &npcId, INT32 &nstepId)
{
	if (m_GuideRunCfg.size() < 1)
	{
		return false;
	}

	time_t curt = time(NULL);
	auto itr = m_GuideRunCfg.begin();
	for (; itr != m_GuideRunCfg.end(); itr++)
	{
		if ( (itr->second.m_BornTime > 0 )&& (curt > itr->second.m_BornTime) && (itr->second.curtNum < itr->second.totalBornNum) )
		{  
			npcId = itr->first;
			nstepId= itr->second.stepId;

			itr->second.m_BornTime = curt;

			itr->second.curtNum += 1;
			return true;
		}
	} 
	return false;
}

 

void		CSSVMBornMgr::SetWildMonsterDelayBorn(EWMLevel lv, INT64 dtime)
{
	auto itr =		m_WMLevelDelayTimeCfg.find(lv);
	if (itr == m_WMLevelDelayTimeCfg.end())
	{
		m_WMLevelDelayTimeCfg[lv] = GetUTCMiliSecond() + dtime;
	}
}

void		CSSVMBornMgr::AddCurtWildMonsterBornInfo(INT32 pointId,INT32 monsterId, INT64  cfgtime)
{
	//map<INT32/*point*/, CurtWildMonsterInfo>											m_CurtMonsterInfo; 
	auto itr = m_CurtMonsterInfo.find(pointId);
	if (itr != m_CurtMonsterInfo.end())
	{
		itr->second.mapMonsterId[monsterId] = true;
		itr->second.nextBornTimeCfg =   cfgtime;
	}
	else
	{
		CurtWildMonsterInfo cwinfo;
		cwinfo.nextBornTimeCfg  =   cfgtime;
		cwinfo.mapMonsterId[monsterId] = true;

		m_CurtMonsterInfo[pointId] = cwinfo;
	}
}
}