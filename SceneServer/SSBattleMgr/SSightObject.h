/*
* file name			:ISSightObject.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:July 16 2014
* summary			:
*
*/

#ifndef __ISSightObject_h__
#define __ISSightObject_h__

#include "stdafx.h"

namespace SceneServer{

class CSSUser;
enum ESSSightObjectType{
	ESSSightObjectType_Sensitive = 0,
	ESSSightObjectType_Insensitive,
};

class CSSight;

class ISSightObject
{
public:
	virtual ~ISSightObject(){}
	virtual void	OnAppearInSight(std::vector<CSSUser*>& cTempUserMap) = 0;
	virtual void	OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap) = 0;
	virtual float	GetSightPosX() = 0;
	virtual float	GetSightPosY() = 0;
	virtual ESSSightObjectType	GetSightObjectType() = 0;
	virtual bool	IfInvisible(){return FALSE;}
public:
	void SetOwner(CSSight* pSight) { mOwner = pSight; }
	CSSight* GetOwner() { return mOwner; }
	CSSight* mOwner;
};

}

#endif