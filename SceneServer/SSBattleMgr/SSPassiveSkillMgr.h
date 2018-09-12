/*
* file name			:SSPassiveSkillMgr.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:June 20 2014
* summary			:
*
*/

#include "stdafx.h"
#include "SSObjectPool.h"

namespace SceneServer{

class CSSPassiveSkill;
class CSSGameUnit;
class CSSBattleMgr;

class CSSPassiveSkillMgr
{
private:
	typedef std::map<INT32, CSSPassiveSkill*>		PassiveSkillMap;
	PassiveSkillMap									m_PassiveSkillMap;
	std::vector<INT32>								m_DelayDeleteVec;
	CSSPassiveSkill*		GetPassiveSkill(INT32 n32PassiveSkillID);
	CSSPassiveSkill*		CreatePassiveSkill(INT32	n32PassiveSkillID);
public:
	CSSBattleMgr*				m_pCSSBattleMgr;
	void						SetBattleMgr(CSSBattleMgr* pMgr);
public:
	CSSPassiveSkillMgr();
	~CSSPassiveSkillMgr();

	INT32					OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);


	INT32					AddPassiveSkill(CSSGameUnit* pTarget, INT32	n32PassiveSkillID);
	void					RevmovePassiveSkill(INT32	n32PassiveSkillUniqueID);
	void					ExpiredPassiveSkill(INT32	n32PassiveSkillUniqueID);
	void					Trigger(INT32 n32PassiveSkillUniqueID, CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1 = 0, INT32 para2 = 0, INT32 para3 = 0);
	void					TryUpgradeSkill(INT32 n32PassiveSkillUniqueID, INT32 n32Level);

};

}