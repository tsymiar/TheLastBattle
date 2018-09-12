/*
* file name			:SSightLight.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:July 15 2014
* summary			:
*
*/

#ifndef __SSightLight_h__
#define __SSightLight_h__

#include "stdafx.h"
#include <hash_set>

namespace SceneServer{

class ISSightLight;

const UINT16	c_InsensitiveSightLightRecountTimeDiff = 500;
const UINT16	c_SensitiveSightLightRecountTimeDiff = 50;
const UINT16	c_SightCellMaxSize = 484;

class CSSightLight
{
private:
	//UINT32							m_sightCellArr[c_SightCellMaxSize];
	//TIME_MILSEC						m_tRecountMil;
	//UINT16							m_x;
	//UINT16							m_y;
	ISSightLight*					m_iterface;

public:
	CSSightLight();
	~CSSightLight();

	void							Clear();
	void							SetLightInterface(ISSightLight* pInterface);
	bool							IsInSight(float x, float y);
};

}

#endif