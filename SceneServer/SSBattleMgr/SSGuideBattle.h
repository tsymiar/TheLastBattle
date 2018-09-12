 
#pragma once
#include "stdafx.h"
#include "SSBattle.h"
#include <boost/shared_ptr.hpp>
#include <hash_map>
#include <map>
using namespace std;
namespace SceneServer{

class CSSBattleMgr; 
class CSSGameObject;
class CSSGameUnit;
class CSSNPC;
class CSSHero; 
class CSSUser;
 
class CSSGuideBattle  : public CSSBattle
{    
	INT64																			m_ObjIdx;
	INT32																			m_AiHeroNum;
	//INT64																			aiHeroId;
	std::map<INT64/*objid*/,std::vector<SGuideBattleCompTask> >						m_ObjCompTask; 

	std::map< INT32 /*taskid*/,  SGuideCurtBornSolder >								m_CurtBornSolder;
	 
	std::map<INT32 /*taskid*/, SGuideCurtBuildBeHurt >								m_CurtBuildBeHurt;

	std::map<INT64/*heroId*/,bool /*ÎÞµÐÓ¢ÐÛ*/>										m_NoHurtHero;

	std::vector<INT32/*ailtarid*/>													m_BrokenAltarId;
public:
	CSSGuideBattle(); 
	~CSSGuideBattle(void); 
	CSSGuideBattle(UINT64 bid,EBattleType btype, CSSBattleMgr* pOwer);  
	virtual bool		CheckPlayTimeout(TIME_MILSEC tCurUTCMilsec); 

private:
	void					AddCompGuide(INT64 objIdx,   INT32 childType, INT32 stepId); 
public:
	bool					CheckObjCanBeHurt(CSSGameUnit* beHurtObj, CSSGameUnit *atkObj );

	void					SetCaseDeadReasonInfo(CSSGameUnit* deadObj, CSSGameUnit *pcReasonGO);  

	void					CheckBornSolderOrHero(CSSHero* pHero, boost::shared_ptr<GCToSS::AskSSGuideStepComp> pmsg);

	void					AddGuideHero(CSSHero* pHero, const SGuideHeroCfg & cfg);

	void					AddGuideNpc(INT32 stepid,  const SGuideSolderCfg & cfg);

	void					CheckStopBornSolder( INT32 stepId);

	void					CheckAndSetAltarBeHurt(INT32 stepId);

	bool					CheckGuideHeroNoHurt(CSSGameUnit* beHurtObj,  bool bFlag);

	void					DelHeroNoHurtStateAtDead(CSSHero* obj );

	void					CheckBornSuperSolder(INT32 campid);
public:
	INT32					OnBornNPCFunc(INT64 tTickSpan);

	INT32					DoAltarBornMonster(INT64 tUTCMilsec);  

	void					CheckSelectHeroTimeout(INT64 tCurUTCMilsec);

	void					CheckSelectRuneTimeout(INT64 tCurUTCMilsec); 

	void					OnBattleFinish(); 

	void					ResetAIAtOnUserOffline(CSSHero* pHero);

	void					OnBattleStateChange();

	void					BroadTipsByType(ETipsType et, INT32 campId = 0){} 

	void					PostMsgToGC_BattleResultHeroInfo(INT32 n32WinCampID){}

	UINT64					GenerateObjGUID(){ m_ObjIdx +=2; return  m_ObjIdx;}  
};

}
