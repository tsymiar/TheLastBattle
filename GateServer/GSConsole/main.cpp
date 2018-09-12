#include "stdafx.h"
#include <string>
#include <map>
#include "../GSKernel/GSKernel.h"

using namespace std;

void ExceptionCallback(void*)
{
	printf("the app had crush and auto dump self.\n");
}

int main()
{
	DbgLib::CDebugFx::SetExceptionHandler(true);
	DbgLib::CDebugFx::SetExceptionCallback(ExceptionCallback, NULL);

	IGSKernel* piKernel = &(GateServer::CGSKernel::GetStaticGSKernel());

	INT32 n32Start = eNormal;

	n32Start = piKernel->Initialize();
	if (eNormal != n32Start)
	{
		printf("Initialize GSKernel fail, error code is %d", n32Start);
		WYYTools_Instance->Sleep(5000);
		return 0;
	}
	else
	{
		n32Start = piKernel->Start();
		if (eNormal != n32Start)
		{
			printf("Start GSKernel fail, error code is %d", n32Start);
			WYYTools_Instance->Sleep(5000);
			return 0;
		}
	}
	printf("\n");

	INT32 n32Input = 0;
	INT32 n32Compare = 0;

	CHAR myChar = '\r';
	CHAR *pszInputStr = NULL;

	//begin main loop.
	while (true) 
	{
#ifdef _WINDOWS
		pszInputStr = GetWYYToolsInstance()->GetConsoleInputString();
		if (NULL != pszInputStr)
		{
			if (0 == strcmp("exit", pszInputStr))
			{
				break;
			}
		}
#endif
		GetWYYToolsInstance()->Sleep(100);
	}

	piKernel->Stop();
	piKernel->Uninitialize();
	printf("Exit GSConsole!");


	return 0;
}
