// RobotConsole.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "RobotMgr.h"
#include <windows.h>
#include "exception_handler.h"
using namespace ReBot;
using namespace google_breakpad;

static bool callback(const wchar_t *dump_path, const wchar_t *id,  
	void *context, EXCEPTION_POINTERS *exinfo,  
	MDRawAssertionInfo *assertion,  
	bool succeeded) {  
		if (succeeded) {  
			printf("dump guid is %ws\n", id);  
		} else {  
			printf("dump failed\n");  
		}  
		fflush(stdout);  

		return succeeded;  
}  

int main(){
	google_breakpad::ExceptionHandler eh(  
		L".", NULL, callback, NULL,  
		google_breakpad::ExceptionHandler::HANDLER_ALL);

	cout << "please input base user account" << endl;
	INT32 n32Input = 0;
	cin >> n32Input;

	CRobotNetSessionMgr* pNetMgr = new CRobotNetSessionMgr;
	gRobotMgr = new CRobotMgr;
	gRobotMgr->SetRobotInitID(n32Input);
	gRobotMgr->Init();
	gRobotMgr->Start();

	//begin main loop.
	DWORD lastTime = GetTickCount();
	while (true) {
		DWORD curTime = GetTickCount();
		pNetMgr->Update();
		gRobotMgr->Update(curTime,curTime-lastTime);
		lastTime = curTime;
		Sleep(1);
	}

	delete pNetMgr;
	delete gRobotMgr;
	return 0;
}