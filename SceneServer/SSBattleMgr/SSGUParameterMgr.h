/*
* file name			:SSGUParameterMgr.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:Nov 26 2014
* summary			:
*
*/

#pragma once

#include "stdafx.h"
#include "SSGUParameter.h"
#include "ISSParameterChangeCallBack.h"

namespace SceneServer{

	class CSSGameUnit;

	class CSSGUParameterMgr
	{
	private:
		CSSGUParameter*	paras[eEffectCate_End];
		CSSGameUnit*	m_pMasterGU;

		void			OnDizzChanged(bool ifAdd, INT32 changeValue);
		
	public:
		~CSSGUParameterMgr();
		CSSGUParameterMgr();

		VOID			SetMaster(CSSGameUnit* pGU){m_pMasterGU = pGU;}

		VOID			AddBaseGroup(SFightProperty fp, bool isBaseValue = false);
		VOID			AddPercentGroup(SFightProperty fp);
		VOID			AddBaseValue(INT32 type, INT32 value, INT32 uniqueID = 0);
		VOID			RemoveBaseValue(INT32 type, INT32 value, INT32 uniqueID = 0);
		VOID			AddPercentValue(INT32 type, INT32 value, INT32 uniqueID = 0);
		VOID			RemovePercentValue(INT32 type, INT32 value, INT32 uniqueID = 0);
		VOID			CheckMaxValue(INT32 type);
		INT32			GetValue(INT32 type);
		VOID			ChangeHP(INT32 value,GSToGC::HPMPChangeReason reason);
		VOID			ChangeMP(INT32 value,GSToGC::HPMPChangeReason reason);
		VOID			SynHP(INT32 value,GSToGC::HPMPChangeReason reason);
		VOID			SynMP(INT32 value,GSToGC::HPMPChangeReason reason);


		INT32			SyncFPToGC();
		INT32			OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);

		VOID			SetValueChangeCallBack(INT32 type, ISSParameterChangeCallBack* pCallBack);
	};

}