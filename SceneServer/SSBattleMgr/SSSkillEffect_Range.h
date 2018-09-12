 
#ifndef SSSKILLEFFECT_RANGE_H_
#define SSSKILLEFFECT_RANGE_H_


#include "stdafx.h"
#include "SSSkillEffect.h"
#include "SSightObject.h"
#include "ISSightLight.h"
 
   
namespace SceneServer{

class CSSSkillEffect_Range : public CSSSkillEffect, public ISSightObject, public ISSightLight
{
private:  
	TIME_MILSEC     m_tLauchUTCMilsec;
	TIME_MILSEC		m_tNextUTCMilsec;		    
	INT32			m_RangeTimes; 
	EGameObjectCamp m_Camp;
	CVector3D   m_CurPos;
	SSkillModelRangeCfg* m_RangEffectCfg;

public:
	CSSSkillEffect_Range();
	~CSSSkillEffect_Range();
	
public:
	virtual INT32		Begin();
	INT32				Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan); 
	virtual INT32		End();
	virtual bool		IsNeedSync(){return TRUE;}
	CVector3D			GetCurPos();

	void				OnAppearInSight(std::vector<CSSUser*>& cTempUserMap);
	void				OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap);
	UINT32				GetNowSightPos();

	virtual float		GetSightX();
	virtual float		GetSightY();
	virtual float		GetLightDistance(){return 300;}
	virtual float		GetSightPosX();
	virtual float		GetSightPosY();
	virtual ESSSightObjectType	GetSightObjectType(){return ESSSightObjectType_Insensitive;}

	virtual bool		IfParaInvalid();

private:
	INT32				CallNextModule(CSSGameUnit *pUnit);
	INT32				GetRangeModuleTargetVec(CSSGameUnit** pGUArray, UINT16 &length);
	INT					GetHitObjList();
	INT32				DoNextModule(CSSGameUnit** pGUArray, UINT16 length);
};

}

#endif