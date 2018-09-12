/*
* file name			:SSSkillMng.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:May 27 2014
* summary			:
*
*/

#ifndef __SSSkillMng_h__
#define __SSSkillMng_h__

#include "stdafx.h"
#include "SSObjectPool.h"

namespace SceneServer{

class CSSSkillEffect;
class CSSSkillEffect_Caculate;
class CSSSkillEffect_Fly;
class CSSSkillEffect_Leading;
class CSSSkillEffect_Range;
class CSSSkillEffect_Buf;
class CSSSkillEffect_Summon;
class CSSSkillEffect_Move;
class CSSkillModel_Switch;
class CSSArea;
class CSSBattleMgr;
class CSSSkill;

enum E_SynEffect{
	E_SynEffect_Appear,
	E_SynEffect_DisAppear,
	E_SynEffect_Max
};

class CSSEffectMgr
{
public:
	typedef std::map<UINT32, CSSSkillEffect*>		EffectMap;
private:
	EffectMap								m_WaitingEffectMap;//所有等待执行的效果列表
	EffectMap								m_UpdateEffectMap;//所有执行中的效果列表
public:
	CSSEffectMgr();
	~CSSEffectMgr();
public:
	CSSBattleMgr*				m_pCSSBattleMgr;//战场管理器
	CSSBattle*					m_pCSSBattle;
	void						SetBattleMgr(CSSBattleMgr* pMgr,CSSBattle* pBattle);
public:
	INT32				OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	INT32				AddEffectsFromCfg(SSNextSkillEffectCfg* pEffectCfg, CSSGameUnit* pMaster, CSSGameUnit* pTarget, CVector3D cPos
		, CVector3D cDir, CSSSkill* pSkill, TIME_MILSEC beginTime,CSSGameUnit* pStartSkillGU = NULL);//根据配置创建一个新的效果，一般在技能起点或者效果调用别的效果的时候使用
	INT32				UseSkillDirect(UINT32 un32SkillID, CSSGameUnit* pReleaser, CSSGameUnit* pTarget);//跳过技能系统，直接添加一个效果
	void				Clear();

	CSSSkillEffect*		GetEffect(UINT32 un32EffectUniqueID);
	INT32				RemoveEffect(UINT32 un32EffectUniqueID);
	void				RemoveAllEffectBySkillID(INT32 skillID);
	INT32				DirectAddBuff(INT32 n32BuffEffectId, CSSGameUnit* pReleaser, CSSGameUnit* pTarget);//跳过技能系统，直接添加一个buff
	
	static bool			CanEffectBeTriggered(std::default_random_engine& egine, INT32 n32TriggerRate, INT32 n32TriggerBase = 1000);//判断几率触发的效果是否能够随机成功触发
	EffectMap&			GetEffectMap();

private:
	INT32				AddEffect(CSSSkillEffect* pEffect);
	CSSSkillEffect*		CreateSkillEffect(INT32 n32SkillEffectId);//创建一个效果的实例，返回其父类的指针
	INT32				DestroyAFreeSkillEffect(CSSSkillEffect* pcEffect);//回收效果
};

}

#endif