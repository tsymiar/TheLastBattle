/*
* file name			:SSPassiveSkill.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:June 19 2014
* summary			:
*
*/
#pragma once

#include "stdafx.h"
#include <map>
#include "SSObjectPool.h"
#include "SSightObject.h"

namespace SceneServer{

class CSSGameUnit;
class CSSPassiveEffect;
class CSSPassiveEffect_BloodSeek;
class CSSPassiveEffect_Rebound;
class CSSPassiveEffect_Relive;
class CSSBattleMgr;

class CSSPassiveSkill : public ISSightObject
{
public:
	CSSBattleMgr*				m_pCSSBattleMgr;
	void						SetBattleMgr(CSSBattleMgr* pMgr);
protected:
	void						SetCooldown();
public:
	const SSPassiveSkilllCfg*		m_pCfg;
	TIME_MILSEC						m_tLastEffectMilsec;
	ref_ptr<CSSGameUnit>			m_pcMasterGO;
	UINT32							m_un32UniqueID;
	UINT32							m_un32TriggerTimes;
	bool							m_IfExpired;
	INT32						OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	void						Trigger(CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1 = 0, INT32 para2 = 0, INT32 para3 = 0);

	void						OnAdd();
	void						SetExpired(){m_IfExpired = true;}
			
	CSSPassiveSkill(const SSPassiveSkilllCfg* pCfg);
	~CSSPassiveSkill();

private:
	CSSPassiveEffect*			m_lKeepedEffects[c_n32MaxNextSkillEffectCfgNum];

	CSSPassiveEffect*			CreateEffect(INT32	n32EffectID, CSSGameUnit* pMaster);
	static INT32				DestroyAFreePassitiveEffect(CSSPassiveEffect*& pcEffect);
	void						OnTrigger(CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1 = 0, INT32 para2 = 0, INT32 para3 = 0);
	bool						CheckTriggerTimes(){m_un32TriggerTimes++; if(m_un32TriggerTimes >= m_pCfg->n32TriggerInterval){m_un32TriggerTimes = 0; return TRUE;} return FALSE;};	
	void						AddKeepedEffect(CSSPassiveEffect* pEffect);
	CSSPassiveEffect*			GetKeepedEffect(INT32	pEffectUniqueID);
	void						RemoveAllKeepedEffect();

	INT32						NotifyPassitiveSkillReleaseToGC();
	void						OnRemove();

	virtual void					OnAppearInSight(std::vector<CSSUser*>& cTempUserMap);
	virtual void					OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap);
	virtual float					GetSightPosX(){return 0;}
	virtual float					GetSightPosY(){return 0;}
	virtual ESSSightObjectType	GetSightObjectType(){return ESSSightObjectType_Insensitive;}
};

}