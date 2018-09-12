// stdafx.cpp : 只包括标准包含文件的源文件
// UserMgr.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include "SSBattle.h"
#include <windows.h>  
#include <Mmsystem.h>
#include "SSWorkThreadMgr.h"
// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

namespace SceneServer{

int64_t GetUTCMiliSecond(){
	return CSSWorkThreadMgr::GetInstance().GetUTCTimeOfPerThread();
}  

INT64 GetWinMiliseconds(){
	LARGE_INTEGER s_frequency;
	BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (1000LL * now.QuadPart) / s_frequency.QuadPart;
}
}