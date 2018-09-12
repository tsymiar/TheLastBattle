/*
* file name			:ISSightLight.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:July 16 2014
* summary			:
*
*/

#ifndef __ISSightLight_h__
#define __ISSightLight_h__

#include "stdafx.h"
#include <hash_set>

namespace SceneServer{

class ISSightLight
{
public:
	virtual	~ISSightLight(){}
	virtual float					GetSightX() = 0;
	virtual float					GetSightY() = 0;
	virtual float					GetLightDistance() = 0;
	bool							IsInSight(float x, float y){
		float diffX = GetSightX() - x;
		float diffY = GetSightY() - y;

		float	lightDist = GetLightDistance();

		if(diffX > lightDist || diffX < -lightDist || diffY > lightDist || diffY < -lightDist )
			return FALSE;

		float	rHoh = lightDist * lightDist;

		return diffX*diffX + diffY*diffY < rHoh;
	}
};

}

#endif