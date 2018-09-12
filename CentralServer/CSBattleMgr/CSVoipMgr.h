/*
* file name			:CSVoipMgr.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* build date        :May 19 2014
* complete date		:
* summary			:
*
*/

#pragma once
#include "stdafx.h"

namespace CentralServer{

class CCSVoipMgr
{
private:
	CCSVoipMgr();
	virtual	~CCSVoipMgr();


public:
	

	static CCSVoipMgr &getInstance(){
		static CCSVoipMgr voip;
		return voip;
	}

	INT32			Initalize();
	INT32			Uninitialize();
};

}