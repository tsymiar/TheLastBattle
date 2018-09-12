#include "stdafx.h"
#include <string>
#include <map>
#include <MMSystem.h>
#include "../../Share/DbgLib/DbgLib.h"
#include "SSWorkThreadMgr.h"
#include "SSCfgMgr.h"
using namespace std;
using namespace SceneServer;
void ExceptionCallback(void*)
{
	printf("the app had crush and auto dump self.\n");
}

#ifndef BOOST_HAS_THREADS
printf("1111")
#endif
int main()
{
    DbgLib::CDebugFx::SetExceptionHandler(true);
	DbgLib::CDebugFx::SetExceptionCallback(ExceptionCallback, NULL);
	timeBeginPeriod(1);
	CSSCfgMgr::GetSSCfgInstance().Initialize();
	CSSWorkThreadMgr::GetInstance().Init();
	CSSWorkThreadMgr::GetInstance().Start();
	CSSWorkThreadMgr::GetInstance().Update();
	google::protobuf::ShutdownProtobufLibrary();
	timeEndPeriod(1);
	return 0;
}
