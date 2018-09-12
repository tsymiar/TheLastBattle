#include "stdafx.h"
#include "DebugHelp.h"
#include "DebugFx.h"

namespace DbgLib
{

//////////////////////////////////////////////////////////////////////////
EXCEPTION_CALLBACK CDebugFx::m_ExceptionCallback = NULL;
void* CDebugFx::m_Param = NULL;

#if defined(PLATFORM_WINDOWS)
LPTOP_LEVEL_EXCEPTION_FILTER CDebugFx::m_PrevUnhandledExceptionHandler = NULL;
#endif

//////////////////////////////////////////////////////////////////////////
void CDebugFx::SetExceptionHandler(bool p_Enable)
{
#if defined(PLATFORM_WINDOWS)
	if(p_Enable)
		m_PrevUnhandledExceptionHandler = SetUnhandledExceptionFilter(UnhandledExceptionHandler);
	else
		SetUnhandledExceptionFilter(m_PrevUnhandledExceptionHandler);
#else
	// set alternative stack to have some space left in case of a stack overflow
	struct sigaltstack sas;
	if(p_Enable)
	{
		// we have to disable the memleak detector here temporarily so that our altstack memory 
		// won't be reported as a memory leak
		g_MemLeakDetector.Disable();
		sas.ss_sp = malloc(56200);
		g_MemLeakDetector.Enable();
		sas.ss_size = 56200;
		sas.ss_flags = 0;
	}
	else
		sas.ss_flags = SS_DISABLE;
	if(sigaltstack(&sas, NULL) < 0)
		printf("sigaltstack failed: %d\n", errno);

	// set signal handler for abort signal
	struct sigaction sa;
	sa.sa_handler = (p_Enable ? SigAbortHandler : SIG_DFL);
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGABRT, &sa, 0) == -1)
		printf("sigaltstack failed: %d\n", errno);

	// set signal handler for segmentation fault signal
	sa.sa_handler = (p_Enable ? SigSegvHandler : SIG_DFL);
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_ONSTACK;
	if(sigaction(SIGSEGV, &sa, 0) == -1)
		printf("sigaltstack failed: %d\n", errno);
#endif
}

//////////////////////////////////////////////////////////////////////////
void CDebugFx::SetExceptionCallback(EXCEPTION_CALLBACK p_Func, void* p_Param)
{
	m_ExceptionCallback = p_Func;
	m_Param = p_Param;
}

//////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_WINDOWS)
//////////////////////////////////////////////////////////////////////////
LONG WINAPI CDebugFx::UnhandledExceptionHandler(EXCEPTION_POINTERS* p_ExcInfo)
{
	// if a stack overflow occured - run our handler in a thread
	if(p_ExcInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
	{
		HANDLE hThread = CreateThread(NULL, 102400, UnhandledExceptionHandlerFunc, p_ExcInfo, 0, NULL);
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	else
		UnhandledExceptionHandlerFunc(p_ExcInfo);

	return EXCEPTION_CONTINUE_SEARCH;
}

//////////////////////////////////////////////////////////////////////////
DWORD WINAPI CDebugFx::UnhandledExceptionHandlerFunc(LPVOID p_Param)
{
	EXCEPTION_POINTERS* excInfo = reinterpret_cast<EXCEPTION_POINTERS*>(p_Param);

#if defined(USE_WER)
	// create a new report
	CDebugHelp::CreateWERReport(excInfo);	
#else
	// create a dump
	CDebugHelp::CreateMiniDump(excInfo, (excInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW) ? true : false);
#endif

	// call the user-defined callback method
	if(m_ExceptionCallback)
		m_ExceptionCallback(m_Param);

	return 0;
}

void CDebugFx::GetStackTrackInfo(tstringstream& Buffer)
{
	ux Stacks[16];
	int StackNum = DbgLib::CDebugHelp::DoStackWalk(Stacks, 16);

	DbgLib::CSymbolResolver Sr(0);
	tstring FileName, FuncName;
	ux Line;

	//Sr.ResolveSymbol(Stacks[2], FuncName, FileName, Line);
	for (int i = 4; i < StackNum - 4; ++i)
	{
		Sr.ResolveSymbol(Stacks[i], FuncName, FileName, Line);

		Buffer << FuncName << " " << FileName << " " << Line << "\n";
	}
}

#else

//////////////////////////////////////////////////////////////////////////
void CDebugFx::SigAbortHandler(int p_Signal)
{
	// create a dump
	CDebugHelp::CreateMiniDump(false);

	// call the user-defined callback method
	if(m_ExceptionCallback)
		m_ExceptionCallback(m_Param);

	// reset to defaucmalt signal handler and core dump
	signal(SIGABRT, SIG_DFL);
}

//////////////////////////////////////////////////////////////////////////
void CDebugFx::SigSegvHandler(int p_Signal)
{
	// create a dump
	CDebugHelp::CreateMiniDump(false);

	// call the user-defined callback method
	if(m_ExceptionCallback)
		m_ExceptionCallback(m_Param);

	// reset to default signal handler and core dump
	signal(SIGSEGV, SIG_DFL);
}


#endif

} // namespace DbgLib
