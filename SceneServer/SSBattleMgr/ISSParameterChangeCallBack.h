/*
* file name			:ISSParameterChangeCallBack.h
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

namespace SceneServer{
	class ISSParameterChangeCallBack
	{
	public:
		virtual VOID		OnValueChanged(INT32 oldValue,INT32 newValue) = 0;
	};

}