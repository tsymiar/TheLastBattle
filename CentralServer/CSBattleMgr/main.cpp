#include "stdafx.h"
#include "../../Share/DbgLib/DbgLib.h"
 
#include "CSKernel.h"
#include "CSBattleMgr.h"
#include "CSUserMgr.h"
using namespace CentralServer;

void ExceptionCallback(void*)
{
	printf("the app had crush and auto dump self.\n");
}

int main(){
	DbgLib::CDebugFx::SetExceptionHandler(true);
	DbgLib::CDebugFx::SetExceptionCallback(ExceptionCallback, NULL);
	 
	GetCSKernelInstance();
	GetCSUserMgrInstance();
	GetBattleMgrInstance();
	GetCSKernelInstance()->Initialize();
	GetBattleMgrInstance()->Initialize();
	GetCSUserMgrInstance()->Initialize();

	GetCSKernelInstance()->Start();
	mysql_library_init(0, NULL, NULL);
	GetCSKernelInstance()->MainLoop();
}

