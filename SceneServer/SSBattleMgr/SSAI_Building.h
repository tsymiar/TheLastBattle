/*
* file name			:SSAI_Building.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:Octorber 13 2014
* summary			:
*
*/

#pragma once

#include "stdafx.h"
#include "SSAI.h"
#include "SSGameUnit.h"

namespace SceneServer{
	class CSSSkill;
	class CSSBuilding;
	class CSSNPC;

	class CSSAI_Building : public CSSAI
	{
	private:
		CSSNPC*				m_pcBuildingGU;	//·ÀÓùËþ

	protected:

	public:
		~CSSAI_Building(){}
		CSSAI_Building(CSSGameUnit* pGU);

		INT32				HeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	};

}