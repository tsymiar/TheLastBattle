//////////////////////////////////////////////////////////////////////////
//系统出生野怪 管理
////////////////////////////////////////////////////////////////////////

#ifndef SSVMBORN_H_
#define SSVMBORN_H_

#include <map>
#include "../../CommonCPlus/Include/FBAll_Const.h"

using namespace std;
namespace SceneServer{ 

class CSSVMBornMgr
{    
	//新手引导
	std::map<INT32/*npcid*/ ,tagGuideVmBornCfg>				m_GuideRunCfg;  
	
	

	//map<INT32/*point*/, INT32/*monster id*/>				m_Point2WildNPCMap; 

public:
	CSSVMBornMgr();
	~CSSVMBornMgr();   
 
	void		ReleaseRunCfg(INT32 nstepId);

	void		AddGuideVMCfg(INT32 npcId, INT32 nstepId, INT32  cdTime ,INT32 total);
	 
	void		UpdateGuideVMInfo(INT32 npcid);

	void		RemoveGuideVMCfg( INT32 nstepId );

	bool		IfGuideBornVM(INT32 &npcid, INT32 &nstepId);

	
	//////////////////////////////////////wild monster func////////////////////////////////
private:
	map<EWMLevel,INT64/*time*/>															m_WMLevelDelayTimeCfg; 
	map<INT32/*point*/, CurtWildMonsterInfo>											m_CurtMonsterInfo; 
public:
	map<EWMLevel,INT64>		&					GetWMMonsterDelayCfg()								{return			m_WMLevelDelayTimeCfg;}

	map<INT32/*point*/, CurtWildMonsterInfo>&	GetCurtMonsterInfo()								{return			m_CurtMonsterInfo; }

	void					SetWildMonsterDelayBorn(EWMLevel lv, INT64 dtime);

	void					AddCurtWildMonsterBornInfo(INT32 pointId,INT32 monsterId,  INT64  nextTime);
};
}
#endif