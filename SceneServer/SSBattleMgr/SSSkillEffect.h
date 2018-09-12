/*
* file name			:SSSkillModel.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:May 28 2014
* summary			:
*
*/
#pragma once

#include "stdafx.h"
#include <map>
#include "SafeRefPtr.h"
#include "SSSkill.h"

namespace SceneServer{

class CSSGameUnit;
class CSSBattleMgr;

//所有效果的父类
class CSSSkillEffect : public referable<CSSSkillEffect>
{
public:
	ESkillEffectType			m_SkillEffectType;//效果类型
	ref_ptr<CSSSkill>			m_pSkillRuntimeData;//技能的指针
	const SSkillEffectlBaseCfg*	m_pCfg;//效果配置
	INT32						m_EffectDelayTime;//效果的等待时间
	ref_ptr<CSSGameUnit>		m_pcMasterGO;//技能的拥有者
	ref_ptr<CSSGameUnit>		m_pcStartSkillGU;//效果从哪个点出发，比如A使用技能，打中B，从B身上出现一个效果，对C生效，那么这里的出发点就是B
	TIME_MILSEC					m_tBeginMilsec;//效果的开始时间
	ref_ptr<CSSGameUnit>		m_pcTarGU;//效果的目标对象
	CVector3D					m_cDir;//方向
	CVector3D					m_cPos;//位置
	ref_ptr<CSSSkillEffect>		m_lDependEffect[c_n32MaxDependSkillEffectCfgNum];//依赖自己的效果指针
	bool						m_IsForceStop;//效果是否被强制终止了
	UINT32						m_un32UniqueID;//唯一ID
	CSSBattle*					m_pBattle;//战场的指针
	bool						m_ifExpired;
public:
	CSSBattleMgr*				m_pCSSBattleMgr;
	void						SetBattleMgr(CSSBattleMgr* pMgr);
	virtual		void			Clean();
protected:
	bool						CheckHitTargetType(CSSGameUnit* pTarget);//检查可作用对象类型(英雄，NPC，建筑)
	bool						CheckHitTargetCamp(CSSGameUnit* pTarget);//检查可作用对象阵营
	void						OnSkillHitTarget(CSSGameUnit* pTarget);//技能命中触发
 
public:
	// the following fun should not be overrided
	INT32						CheckCooldown();//检查是否触发冷却
	bool						IfNeedWait(TIME_MILSEC tUTCMilsec);//判断是否还需要等待开始
	INT32						Clear();
	bool						IsForceStop();
	INT32						ForceStop();//强制停止
	virtual bool				IsUsingSkill(){return FALSE;}//效果是否是持续引导中
	virtual bool				IsCanStopUsing(){return TRUE;}//引导技能是否可以被中断
	virtual bool				IfCanMove(){return TRUE;}//移动技能是否可以移动
	INT32						StopDependedEffect();//停止依赖于自己的效果们
	virtual bool				IsNeedSync(){return FALSE;}
	INT32						AddSelfToUsingList();//将自己加入到引导技能列表中
	UINT32						GetUniqueID(){return m_un32UniqueID;}
	virtual void				SetExpired(){m_ifExpired = true;}
public:
	virtual	INT32				Begin() = 0;
	virtual INT32				Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan) = 0;
	virtual	INT32				End() = 0;
	virtual CVector3D			GetCurPos(){return CVector3D();}
	bool						CheckBattle();
	CSSBattle*					GetBattle();
	virtual bool				IfParaInvalid();
	
	CSSSkillEffect();
	virtual ~CSSSkillEffect() ;
};

}