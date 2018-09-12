#pragma once

#include "stdafx.h"
#include "SSSkillEffect.h"
#include "SSightObject.h"

namespace SceneServer{

	class CSSSkillEffect_Link : public CSSSkillEffect, public ISSightObject
	{
	public:
		CSSSkillEffect_Link();
		~CSSSkillEffect_Link();

	public:
		virtual	INT32		Begin();
		virtual INT32		Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
		virtual INT32		End();

	private:
		enum ELinkState{
			eLink_Begin,
			eLink_Finish,
			eLink_Break
		};
		virtual void					OnAppearInSight(std::vector<CSSUser*>& cTempUserMap);
		virtual void					OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap);
		virtual float					GetSightPosX();
		virtual float					GetSightPosY();
		virtual ESSSightObjectType	GetSightObjectType(){return ESSSightObjectType_Insensitive;}

	private:
		SSkillModelLinkCfg* m_pLinkCfg;
		CVector3D			m_cLinkPos;
		ELinkState			m_eLinkState;
	};

}
