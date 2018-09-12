#pragma once

#include "stdafx.h"
#include "SSSkillEffect.h"

namespace SceneServer{

class CSSSkillEffect_Leading : public CSSSkillEffect
{
public:
	CSSSkillEffect_Leading();
	~CSSSkillEffect_Leading();

public:
	virtual	INT32		Begin();
	virtual INT32		Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	virtual INT32		End();
	virtual bool		IsUsingSkill(){return m_isUsingSkill;}
	virtual bool		IsCanStopUsing(){return m_pCfg->bIsCanBreak && FALSE == m_pCfg->bIsCanMove;}
	virtual bool		IfCanMove(){return m_pCfg->bIsCanMove;}

private:
	enum ELeadingState{
		eLeading_Begin,
		eLeading_Success,
		eLeading_Failed
	};

	INT32				NotifyGS(ELeadingState eLeadingState);
	bool				m_isUsingSkill;

private:
	ELeadingState		m_LeadingState;
	SSkillModelLeadingCfg* m_pLeadingCfg;
};

}
