/*
* file name			:SSMoveObjectHolder.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:Auguest 8 2014
* summary			:
*
*/

#ifndef __ISSMoveObjectHolder_h__
#define __ISSMoveObjectHolder_h__

#include "stdafx.h"

namespace SceneServer{


class ISSMoveObjectHolder
{
public:
	virtual ~ISSMoveObjectHolder(){}

	//进行事件通知的抽象接口
	virtual void				OnStopMove() = 0;
};

}

#endif