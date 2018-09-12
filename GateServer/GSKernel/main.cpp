#include "stdafx.h"
#include <string>
#include <map>
#include "GSKernel.h"
#include "../../Share/DbgLib/DbgLib.h"

using namespace std;

void ExceptionCallback(void*)
{
	printf("the app had crush and auto dump self.\n");
}

int main()
{
	DbgLib::CDebugFx::SetExceptionHandler(true);
	DbgLib::CDebugFx::SetExceptionCallback(ExceptionCallback, NULL);

	GateServer::CGSKernel* piKernel = &(GateServer::CGSKernel::GetStaticGSKernel());

	INT32 n32Start = eNormal;

	n32Start = piKernel->Initialize();
	if (eNormal != n32Start)
	{
		printf("Initialize GSKernel fail, error code is %d", n32Start);
		Sleep(5000);
		return 0;
	}
	else
	{
		n32Start = piKernel->Start();
		if (eNormal != n32Start)
		{
			printf("Start GSKernel fail, error code is %d", n32Start);
			//WYYTools_Instance->Sleep(5000);
			return 0;
		}
	}

	piKernel->Stop();
	piKernel->Uninitialize();
	google::protobuf::ShutdownProtobufLibrary();
	printf("Exit GSConsole!");

	return 0;
}
