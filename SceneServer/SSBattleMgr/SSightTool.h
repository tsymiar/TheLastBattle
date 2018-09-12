/*
* file name			:SSightTool.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:July 15 2014
* summary			:
*
*/

#ifndef __SSightTool_h__
#define __SSightTool_h__

#include "stdafx.h"

namespace SceneServer{

const UINT32		c_SightCellSize = 200;

class CSSightTool
{
public:
	static UINT32		GetSightCellValue(UINT16 x, UINT16 y){return (x<<16) + y;}
	static UINT16		GetSightPos(float value){return value / c_SightCellSize;}
};

}

#endif