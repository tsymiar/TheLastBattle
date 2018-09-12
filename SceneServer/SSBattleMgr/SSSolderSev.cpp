
#include "stdafx.h"
#include "SSSolderSev.h"
#include "SSCfgMgr.h"
 

namespace SceneServer{

	SSSolderSev::SSSolderSev():m_tNPCNextBornCDMilsec(0){m_curtSolderBornInfo.clear();}
	SSSolderSev::~SSSolderSev(){}

	bool	SSSolderSev::CheckBornSolder(INT64  curttime)
	{
		if (m_tNPCNextBornCDMilsec == 0 || curttime < m_tNPCNextBornCDMilsec){
			return false;
		}

		return true;
	} 

	void	SSSolderSev::SetNPCNextBornCDMisec(INT64 tcfg)
	{
		m_tNPCNextBornCDMilsec = GetUTCMiliSecond() + tcfg;
	}

	 

	void	SSSolderSev::SetAltarBrokenAboutSuperSolder(INT32 pathId, INT32 altarIdx, std::vector<INT32>& altarCfg)
	{
		auto itr = m_curtSolderBornInfo.find(pathId);
		if (itr != m_curtSolderBornInfo.end())
		{
			 itr->second.curtBrokenAltarIdx.push_back(altarIdx);

			bool bFlag = Comp2Vector(itr->second.curtBrokenAltarIdx,altarCfg);
			if(bFlag)
			{
				itr->second.bSuperState = true;
			}
		}
		else
		{
			ELOG(LOG_WARNNING,"shit , i can not find pathdid:%d, altarIdx:%d",pathId, altarIdx);
		}
	}

	bool	 CompValue(INT32 p1, INT32 p2)
	{
		return p1 < p2;
	}

	bool	SSSolderSev::Comp2Vector(std::vector<INT32> & vs, std::vector<INT32> & vd)
	{
		if (vs.size() < 1 || vd.size() < 1 || vs.size() != vd.size() )
		{
			return false;
		}
		std::sort(vs.begin(),vs.end(),CompValue);
		std::sort(vd.begin(),vd.end(),CompValue);

		auto itrs = vs.begin();
		auto itrd = vd.begin();
		while (itrs != vs.end() && itrd != vd.end() && *itrs == *itrd)
		{
			++itrs;
			++itrd;
		}
		if (itrs == vs.end())
		{
			return true;
		}
		return false;
	}
	
}