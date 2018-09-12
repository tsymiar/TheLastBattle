#include "stdafx.h"
#include "DebugHelp.h"
#include "Utils.h"

namespace DbgLib
{

#if defined(PLATFORM_WINDOWS)

//*****************************************************************************
// class overview:
//   Helper class that dynamically loads our required exports from DbgHelp.dll
//-----------------------------------------------------------------------------
class CDbgHelpDll
{
	public:
		CDbgHelpDll()
		{
			// load dbghelp.dll
			m_hDll = LoadLibrary(_T("dbghelp.dll"));
			if(m_hDll)
				LoadMethods();

			InitializeCriticalSection(&m_CSObject);
		}

		~CDbgHelpDll()
		{
			if(m_hDll)
				FreeLibrary(m_hDll);

			DeleteCriticalSection(&m_CSObject);
		}
		
	public:
		
		// lock access to the dbghelp API
		void Lock()
		{
			EnterCriticalSection(&m_CSObject);
		}

		// unlock access to the dbghelp API
		void Unlock()
		{
			LeaveCriticalSection(&m_CSObject);
		}

		// returns true if the dbghelp.dll was successfully loaded
		bool IsLoaded() const { return m_hDll != NULL; }

	public:
		// definition of DbgHlp.dll exported APIs
		typedef BOOL (__stdcall *fpSymInitialize)(HANDLE hProcess, PCTSTR UserSearchPath, BOOL fInvadeProcess);
		typedef BOOL (__stdcall *fpSymCleanup)(HANDLE hProcess);
		typedef DWORD (__stdcall *fpSymSetOptions)(DWORD SymOptions);
		typedef DWORD (__stdcall *fpSymGetOptions)();
		typedef BOOL (__stdcall *fpSymFromAddr)(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol);
		typedef BOOL (__stdcall *fpSymFromName)(HANDLE hProcess, LPSTR Name, PSYMBOL_INFO Symbol);
		typedef BOOL (__stdcall *fpSymGetLineFromAddr64)(HANDLE hProcess, DWORD64 dwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line);
		typedef PVOID (__stdcall *fpSymFunctionTableAccess64)(HANDLE hProcess, DWORD64 AddrBase);
		typedef DWORD64	(__stdcall *fpSymGetModuleBase64)(HANDLE hProcess, DWORD64 dwAddr);	
		typedef BOOL (__stdcall *fpSymGetModuleInfo64)(HANDLE hProcess, DWORD64 qwAddr, PIMAGEHLP_MODULE64 ModuleInfo);
		typedef BOOL (__stdcall *fpStackWalk64)(DWORD MachineType, HANDLE hProcess, HANDLE hThread, LPSTACKFRAME64 StackFrame, PVOID ContextRecord, PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine, PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine, PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine, PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);
		typedef BOOL (__stdcall *fpMiniDumpWriteDump)(HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);
		typedef DWORD64	(__stdcall *fpSymLoadModule64)(HANDLE hProcess, HANDLE hFile, PCSTR ImageName, PCSTR ModuleName, DWORD64 BaseOfDll, DWORD SizeOfDll);
		typedef BOOL (__stdcall *fpSymUnloadModule64)(HANDLE hProcess, DWORD64 BaseOfDll);

	public:
		// exported functions
		fpSymInitialize SymInitialize;
		fpSymCleanup SymCleanup;
		fpSymSetOptions SymSetOptions;
		fpSymGetOptions SymGetOptions;
		fpSymFromAddr SymFromAddr;
		fpSymFromName SymFromName;
		fpSymGetLineFromAddr64 SymGetLineFromAddr64;
		fpSymFunctionTableAccess64 SymFunctionTableAccess64;
		fpSymGetModuleBase64 SymGetModuleBase64;
		fpStackWalk64 StackWalk64;
		fpMiniDumpWriteDump MiniDumpWriteDump;
		fpSymLoadModule64 SymLoadModule64;
		fpSymUnloadModule64 SymUnloadModule64;

	private:
		// dll handle
		HMODULE m_hDll;

		// critical section to synchronize dbghelp.dll api calls
		CRITICAL_SECTION m_CSObject;

	private:

		// loads all required exports from dbghelp.dll
#define CHECK_FUNC_PTR(ptr_) if((ptr_) == NULL) DebugBreak();
		void LoadMethods()
		{
#if defined(_UNICODE)
			CHECK_FUNC_PTR(SymInitialize = reinterpret_cast<fpSymInitialize>(GetProcAddress(m_hDll, _AT("SymInitializeW"))));
			CHECK_FUNC_PTR(SymFromAddr = reinterpret_cast<fpSymFromAddr>(GetProcAddress(m_hDll, _AT("SymFromAddrW"))));
			CHECK_FUNC_PTR(SymFromName = reinterpret_cast<fpSymFromName>(GetProcAddress(m_hDll, _AT("SymFromNameW"))));
			CHECK_FUNC_PTR(SymGetLineFromAddr64 = reinterpret_cast<fpSymGetLineFromAddr64>(GetProcAddress(m_hDll, _AT("SymGetLineFromAddrW64"))));
#else
			CHECK_FUNC_PTR(SymInitialize = reinterpret_cast<fpSymInitialize>(GetProcAddress(m_hDll, _AT("SymInitialize"))));
			CHECK_FUNC_PTR(SymFromAddr = reinterpret_cast<fpSymFromAddr>(GetProcAddress(m_hDll, _AT("SymFromAddr"))));
			CHECK_FUNC_PTR(SymFromName = reinterpret_cast<fpSymFromName>(GetProcAddress(m_hDll, _AT("SymFromName"))));
			CHECK_FUNC_PTR(SymGetLineFromAddr64 = reinterpret_cast<fpSymGetLineFromAddr64>(GetProcAddress(m_hDll, _AT("SymGetLineFromAddr64"))));
#endif
			CHECK_FUNC_PTR(SymCleanup = reinterpret_cast<fpSymCleanup>(GetProcAddress(m_hDll, _AT("SymCleanup"))));
			CHECK_FUNC_PTR(SymSetOptions = reinterpret_cast<fpSymSetOptions>(GetProcAddress(m_hDll, _AT("SymSetOptions"))));
			CHECK_FUNC_PTR(SymGetOptions = reinterpret_cast<fpSymGetOptions>(GetProcAddress(m_hDll, _AT("SymGetOptions"))));
			CHECK_FUNC_PTR(SymFunctionTableAccess64 = reinterpret_cast<fpSymFunctionTableAccess64>(GetProcAddress(m_hDll, _AT("SymFunctionTableAccess64"))));
			CHECK_FUNC_PTR(SymGetModuleBase64 = reinterpret_cast<fpSymGetModuleBase64>(GetProcAddress(m_hDll, _AT("SymGetModuleBase64"))));
			CHECK_FUNC_PTR(StackWalk64 = reinterpret_cast<fpStackWalk64>(GetProcAddress(m_hDll, _AT("StackWalk64"))));
			CHECK_FUNC_PTR(MiniDumpWriteDump = reinterpret_cast<fpMiniDumpWriteDump>(GetProcAddress(m_hDll, _AT("MiniDumpWriteDump"))));
			CHECK_FUNC_PTR(SymLoadModule64 = reinterpret_cast<fpSymLoadModule64>(GetProcAddress(m_hDll, _AT("SymLoadModule64"))));
			CHECK_FUNC_PTR(SymUnloadModule64 = reinterpret_cast<fpSymUnloadModule64>(GetProcAddress(m_hDll, _AT("SymUnloadModule64"))));
		}
#undef CHECK_FUNC_PTR
};

//////////////////////////////////////////////////////////////////////////
// Global CDbgHelpDll instance we will use for our debug utility functions
// init_seg(compiler) will force the object to be initialized before any other object in the applicatoin
// This is usually used to initialize CRT object, but it's safe for us to initialize CDbgHelpDll here 
// because it doesn't use any CRT functions or objects.
#pragma warning(disable : 4074)
#pragma init_seg(compiler)
CDbgHelpDll g_DbgHelpDll;
#pragma warning(default : 4074)

#if defined(USE_WER)
//*****************************************************************************
// class overview:
//   Helper class that dynamically loads our required exports from Wer.dll
//-----------------------------------------------------------------------------
class CWERDll
{
	public:
		CWERDll()
		{
			// load dbghelp.dll
			m_hDll = LoadLibrary(_T("wer.dll"));
			if(m_hDll)
				LoadMethods();
		}

		~CWERDll()
		{
			if(m_hDll)
				FreeLibrary(m_hDll);
		}
		
	public:
		// returns true if the wer.dll was successfully loaded
		bool IsLoaded() const { return m_hDll != NULL; }

	public:
		// definition of wer.dll exported APIs
		typedef HRESULT (__stdcall *fpWerReportCreate)(PCWSTR pwzEventType, WER_REPORT_TYPE repType, PWER_REPORT_INFORMATION pReportInformation, HREPORT *phReportHandle);
		typedef HRESULT (__stdcall *fpWerReportAddFile)(HREPORT hReportHandle, PCWSTR pwzPath, WER_FILE_TYPE repFileType, DWORD dwFileFlags);
		typedef HRESULT (__stdcall *fpWerReportSubmit)(HREPORT hReportHandle, WER_CONSENT consent, DWORD dwFlags, PWER_SUBMIT_RESULT pSubmitResult);
		typedef HRESULT (__stdcall *fpWerReportAddDump)(HREPORT hReportHandle, HANDLE hProcess, HANDLE hThread, WER_DUMP_TYPE dumpType, PWER_EXCEPTION_INFORMATION pExceptionParam, PWER_DUMP_CUSTOM_OPTIONS pDumpCustomOptions, DWORD dwFlags);
		typedef HRESULT (__stdcall *fpWerReportCloseHandle)(HREPORT hReportHandle);

	public:
		// exported functions
		fpWerReportCreate WerReportCreate;
		fpWerReportAddFile WerReportAddFile;
		fpWerReportSubmit WerReportSubmit;
		fpWerReportAddDump WerReportAddDump;
		fpWerReportCloseHandle WerReportCloseHandle;

	private:
		// dll handle
		HMODULE m_hDll;

	private:
		// loads all required exports from dbghelp.dll
#define CHECK_FUNC_PTR(ptr_) if((ptr_) == NULL) DebugBreak();
		void LoadMethods()
		{
			CHECK_FUNC_PTR(WerReportCreate = reinterpret_cast<fpWerReportCreate>(GetProcAddress(m_hDll, _AT("WerReportCreate"))));
			CHECK_FUNC_PTR(WerReportAddFile = reinterpret_cast<fpWerReportAddFile>(GetProcAddress(m_hDll, _AT("WerReportAddFile"))));
			CHECK_FUNC_PTR(WerReportSubmit = reinterpret_cast<fpWerReportSubmit>(GetProcAddress(m_hDll, _AT("WerReportSubmit"))));
			CHECK_FUNC_PTR(WerReportAddDump = reinterpret_cast<fpWerReportAddDump>(GetProcAddress(m_hDll, _AT("WerReportAddDump"))));
			CHECK_FUNC_PTR(WerReportCloseHandle = reinterpret_cast<fpWerReportCloseHandle>(GetProcAddress(m_hDll, _AT("WerReportCloseHandle"))));
		}
#undef CHECK_FUNC_PTR
};

// global wer.dll helper class instance
CWERDll g_WERDll;

#endif // USE_WER

#endif // PLATFORM_WINDOWS

// static path object
tstring CDebugHelp::m_DumpPath;

#if defined(PLATFORM_WINDOWS)

// dump type conversion table (DumpType -> MINIDUMP_TYPE)
const ux CDebugHelp::m_DumpTypeConverter[] = 
{
	(MiniDumpNormal),
	(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
	(MiniDumpWithPrivateReadWriteMemory | MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules),
	(MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData | MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules)
};

// initialize current dump type
ux CDebugHelp::m_DumpType = CDebugHelp::m_DumpTypeConverter[CDebugHelp::MiniDump];


//////////////////////////////////////////////////////////////////////////
void CDebugHelp::SetDumpType(DumpType p_Type)
{
	if(p_Type >= CDebugHelp::TinyDump && p_Type <= CDebugHelp::FullDump)
		m_DumpType = m_DumpTypeConverter[p_Type];
}

//////////////////////////////////////////////////////////////////////////
// mini dump information callback function
// here we will exclude the mini dump creation thread from the dump file
BOOL CALLBACK CDebugHelp::MiniDumpCallback(PVOID CallbackParam, const PMINIDUMP_CALLBACK_INPUT CallbackInput, PMINIDUMP_CALLBACK_OUTPUT CallbackOutput)
{
	if(CallbackInput->CallbackType == IncludeThreadCallback && CallbackInput->IncludeThread.ThreadId == static_cast<ULONG>(reinterpret_cast<std::size_t>(CallbackParam)))
		return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void CDebugHelp::CreateMiniDump(PEXCEPTION_POINTERS p_ExceptionInfo, bool p_IgnoreThread)
{
	// build our MINIDUMP_EXCEPTION_INFORMATION (if we got EXCEPTION_POINTERS in p_Param)
	MINIDUMP_EXCEPTION_INFORMATION excInfo;
	PMINIDUMP_EXCEPTION_INFORMATION excInfoPtr = NULL;
	if(p_ExceptionInfo)
	{
		excInfo.ThreadId = GetCurrentThreadId();
		excInfo.ExceptionPointers = p_ExceptionInfo;
		excInfo.ClientPointers = FALSE;
		excInfoPtr = &excInfo;
	}

	MINIDUMP_CALLBACK_INFORMATION callbackInfo;
	PMINIDUMP_CALLBACK_INFORMATION pCallbackInfo = NULL;
	if(p_IgnoreThread)
	{
		// create minidump callback information
		callbackInfo.CallbackParam = reinterpret_cast<PVOID>(static_cast<std::size_t>(GetCurrentThreadId()));
		callbackInfo.CallbackRoutine = CDebugHelp::MiniDumpCallback;
		pCallbackInfo = &callbackInfo;
	}

	// create dump filename
	tstring dateTimeStr;
	SYSTEMTIME localTime;
	GetLocalTime(&localTime);
	dateTimeStr = s_str(localTime.wDay << _T('.') << localTime.wMonth << _T('.') << localTime.wYear << _T('-') <<
		localTime.wHour << _T('.') << localTime.wMinute << _T('.') << localTime.wSecond << _T('.') << localTime.wMilliseconds);

	// create dump file
	tstring fileName(s_cstr(m_DumpPath << _T("Dump-") << dateTimeStr << _T(".dmp")));
	HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		// create dump
		if(!g_DbgHelpDll.MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, static_cast<MINIDUMP_TYPE>(m_DumpType), excInfoPtr, NULL, pCallbackInfo))
			_tprintf(_T("MiniDumpWriteDump failed: %08X\n"), GetLastError());
			
		CloseHandle(hFile);
	}
	else
		_tprintf(_T("CreateFile failed: %d\n"), GetLastError());
}

//////////////////////////////////////////////////////////////////////////
void CDebugHelp::CreateWERReport(PEXCEPTION_POINTERS p_ExceptionInfo)
{
#if defined(USE_WER)
	bool werSuccess = false;
	if(g_WERDll.IsLoaded())
	{
		// create a new report
		HREPORT hReport;
		WER_REPORT_INFORMATION werRepInfo;
		memset(&werRepInfo, 0, sizeof(werRepInfo));
		werRepInfo.dwSize = sizeof(werRepInfo);
		wcscpy_s(werRepInfo.wzFriendlyEventName, _WT("Stopped working"));
		if(SUCCEEDED(g_WERDll.WerReportCreate(APPCRASH_EVENT, WerReportCritical, &werRepInfo, &hReport)))
		{
			// add a dump to the report
			WER_EXCEPTION_INFORMATION werExcInfo;
			werExcInfo.bClientPointers = FALSE;
			werExcInfo.pExceptionPointers = p_ExceptionInfo;
			WER_DUMP_CUSTOM_OPTIONS werDumpOpt;
			memset(&werDumpOpt, 0, sizeof(werDumpOpt));
			werDumpOpt.dwSize = sizeof(werDumpOpt);
			werDumpOpt.dwMask = WER_DUMP_MASK_DUMPTYPE;
			werDumpOpt.dwDumpFlags = static_cast<MINIDUMP_TYPE>(m_DumpType);
			if(SUCCEEDED(g_WERDll.WerReportAddDump(hReport, GetCurrentProcess(), GetCurrentThread(), WerDumpTypeHeapDump, &werExcInfo, &werDumpOpt, 0)))
			{
				// optional - add any log-files to the report
				//g_WERDll.WerReportAddFile(hReport, _WT("game.log"), WerFileTypeOther, WER_FILE_ANONYMOUS_DATA);


				// submit report!
				if(SUCCEEDED(WerReportSubmit(hReport, WerConsentNotAsked, WER_SUBMIT_SHOW_DEBUG | WER_SUBMIT_HONOR_RECOVERY | WER_SUBMIT_HONOR_RESTART | WER_SUBMIT_OUTOFPROCESS, NULL)))
					werSuccess = true;
			}

			g_WERDll.WerReportCloseHandle(hReport);
		}
	}

	if(werSuccess)
	{
		// terminate the process (so that we don't activate WER 2 times for one crash)
		TerminateProcess(GetCurrentProcess(), 1);
	}
	else
	{
		// if we couldn't create an error report - just create a mini dump
		CreateMiniDump(p_ExceptionInfo, false);
	}
#endif // USER_WER
}

//////////////////////////////////////////////////////////////////////////
// Helper function to retrive the current program counter (required for 
// walking the stack using MS DbgHelp.dll). 
// Refer to http://nedbatchelder.com/blog/20051006T065335.html for more information
//////////////////////////////////////////////////////////////////////////
extern "C" void* _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)

__declspec(noinline) DWORD_PTR CDebugHelp::GetProgramCounter()
{
	return (DWORD_PTR)_ReturnAddress();
}

#endif // PLATFORM_WINDOWS


//////////////////////////////////////////////////////////////////////////
void CDebugHelp::SetDumpPath(const tchar* p_Path)
{
#if defined(PLATFORM_WINDOWS)
	const tchar separator = _T('\\');
#else
	const tchar separator = _T('/');
#endif

	m_DumpPath = p_Path;
	if(!m_DumpPath.empty() && m_DumpPath[m_DumpPath.size() - 1] != separator)
		m_DumpPath += separator;
}

//////////////////////////////////////////////////////////////////////////
void CDebugHelp::CreateMiniDump(bool p_IgnoreThread)
{
#if defined(PLATFORM_WINDOWS)
	CreateMiniDump(NULL, p_IgnoreThread);
#else
	// get current time and format into a string
	char buf[101];
	time_t t;
	struct tm *ptm;
	struct timeval tv;
	struct timezone tz;
	
	t = time(NULL);
	ptm = localtime(&t);   
	strftime(buf, 100, "%Y.%m.%d-%H.%M.%S.", ptm);
	gettimeofday(&tv, &tz);
	ptm = localtime(&tv.tv_sec);

	std::ostringstream fileName;
#if defined(_UNICODE)
	std::string path;
	std::transform(m_DumpPath.begin(), m_DumpPath.end(), std::back_inserter(path), Internal::WideToChar());
	fileName << path << _AT("Dump-") << buf << tv.tv_usec << _AT(".dmp");
#else
	fileName << m_DumpPath << _AT("Dump-") << buf << tv.tv_usec << _AT(".dmp");
#endif
	WriteCoreDump(fileName.str().c_str());
#endif
}

//////////////////////////////////////////////////////////////////////////
ux CDebugHelp::DoStackWalk(ux* p_CallStack, ux p_BufferSize)
{
#if defined(PLATFORM_WINDOWS)
	if(!g_DbgHelpDll.IsLoaded())
		return 0;

	// For detailed info and updates how to walk the stack see:
	// "Walking the callstack" @CodeProject
	// By Jochen Kalmbach [MVP VC++]
	// http://www.codeproject.com/threads/StackWalker.asp

#if defined(_M_IX86) || defined(_M_X64) || defined(_M_IA64)
	HANDLE hProcess;
	HANDLE hThread;
	DWORD machineType;
	STACKFRAME64 stackFrame;
	CONTEXT contextRecord;

	// initialize context structure
	ZeroMemory(&contextRecord, sizeof(contextRecord));
	contextRecord.ContextFlags = CONTEXT_FULL;
#if defined(_M_IX86)
	// get the required values for initialization of the STACKFRAME64 structure
	// to be passed to StackWalk64(). Required fields are AddrPC and AddrFrame.
	contextRecord.Eip = static_cast<DWORD>(GetProgramCounter());
	__asm mov [contextRecord.Esp], esp // get the stack pointer
	__asm mov [contextRecord.Ebp], ebp // get the frame pointer (aka base pointer)
#elif defined(_M_X64) || defined(_M_IA64)
	RtlCaptureContext(&contextRecord);
#endif

	// initialize the STACKFRAME64 structure.
	ZeroMemory(&stackFrame, sizeof(stackFrame));
#ifdef _M_IX86
	machineType = IMAGE_FILE_MACHINE_I386;
	stackFrame.AddrPC.Offset = contextRecord.Eip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = contextRecord.Ebp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = contextRecord.Esp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
	machineType = IMAGE_FILE_MACHINE_AMD64;
	stackFrame.AddrPC.Offset = contextRecord.Rip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = contextRecord.Rsp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = contextRecord.Rsp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
	machineType = IMAGE_FILE_MACHINE_IA64;
	stackFrame.AddrPC.Offset = contextRecord.StIIP;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = contextRecord.IntSp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrBStore.Offset = contextRecord.RsBSP;
	stackFrame.AddrBStore.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = contextRecord.IntSp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
#endif

	// initialize the remaining stack info
	hProcess = GetCurrentProcess();
	hThread = GetCurrentThread();
	
	// synchronize access to dbghelp api
	ux curFrame = 0;

	g_DbgHelpDll.Lock();
	__try
	{
		while(curFrame < p_BufferSize)
		{
			// walk the stack.
			if(!g_DbgHelpDll.StackWalk64(machineType, hProcess, hThread, &stackFrame, &contextRecord, NULL, g_DbgHelpDll.SymFunctionTableAccess64, g_DbgHelpDll.SymGetModuleBase64, NULL))
			{
				// couldn't trace back through any more frames.
				break;
			}
			if(stackFrame.AddrFrame.Offset == 0)
			{
				// end of stack.
				break;
			}
			// stack position is stored in StackFrame.AddrPC.Offset
			p_CallStack[curFrame] = (ux)stackFrame.AddrPC.Offset;
			++curFrame;
		}
	}
	__finally
	{
		// unlock dbghelp api
		g_DbgHelpDll.Unlock();
	}
	
	return curFrame;
#else
	// If you want to retarget the stack trace to another processor
	// architecture then you'll need to provide architecture-specific code to
	// retrieve the current frame pointer and program counter in order to initialize
	// the STACKFRAME64 structure below.
	#pragma message("Stack trace is not supported on this architecture.")
	return 0;
#endif // #if defined(_M_IX86) || defined(_M_X64) || defined(_M_IA64)

#elif defined(PLATFORM_UNIX)
	return backtrace(reinterpret_cast<void**>(p_CallStack), p_BufferSize);
#endif // #if defined(PLATFORM_WINDOWS)
}

//////////////////////////////////////////////////////////////////////////
CSymbolResolver::CSymbolResolver(const tchar* p_SymbolSearchPath)
{
#if defined(PLATFORM_WINDOWS)
	m_hProcess = GetCurrentProcess();

	// synchronize access to dbghelp api
	g_DbgHelpDll.Lock();

	// set sym options, load all in process modules deferred
	g_DbgHelpDll.SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

	// initialize dbghelp
	if(!g_DbgHelpDll.SymInitialize(m_hProcess, p_SymbolSearchPath, TRUE))
		_tprintf(_T("Unable to initialize 'DbgHelp.dll'\n"));

	// set sym options, load all dynamic modules immediately
	g_DbgHelpDll.SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
#endif
}

CSymbolResolver::~CSymbolResolver()
{
#if defined(PLATFORM_WINDOWS)
	// cleanup dbghelp
	g_DbgHelpDll.SymCleanup(m_hProcess);

	// unlock dbghelp api
	g_DbgHelpDll.Unlock();
#endif
}

//////////////////////////////////////////////////////////////////////////
tstring CSymbolResolver::GetDefaultSymbolSearchPath()
{
	tostringstream pathStr;

#if defined(PLATFORM_WINDOWS)
	pathStr << _T(".\\;");
	pathStr << Internal::CWin32Utils::GetApplicationDirectory() << _T(";");
	pathStr << Internal::CWin32Utils::GetWorkingDirectory() << _T(";");
	pathStr << Internal::CWin32Utils::GetSystemRootDirectory() << _T(";");
	pathStr << Internal::CWin32Utils::GetSystemDirectory() << _T(";");
	pathStr << Internal::CWin32Utils::GetSystemRootDirectory() << _T("\\Symbols\\dll;");

	// append %_NT_SYMBOL_PATH% and %_NT_ALT_SYMBOL_PATH%.
	char EnvBuffer[2048];
	size_t BufferUsed;
	if(!getenv_s(&BufferUsed, EnvBuffer, _AT("_NT_SYMBOL_PATH")))
		pathStr << EnvBuffer << _T(";");
	if(!getenv_s(&BufferUsed, EnvBuffer, _AT("_NT_ALT_SYMBOL_PATH")))
		pathStr << EnvBuffer << _T(";");
#endif

	return pathStr.str();
}

//////////////////////////////////////////////////////////////////////////
void CSymbolResolver::ResolveSymbol(ux p_Address, tstring& p_Function, tstring& p_File, ux& p_Line)
{
#if defined(PLATFORM_WINDOWS)
	// try to get the source file and line number associated with this program counter address.
	DWORD displacement = 0;
	IMAGEHLP_LINE64 sourceInfo;
	sourceInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	if(g_DbgHelpDll.SymGetLineFromAddr64(m_hProcess, static_cast<DWORD64>(p_Address), &displacement, &sourceInfo) == TRUE)
	{
		p_File = sourceInfo.FileName;
		p_Line = sourceInfo.LineNumber;
	}
	else
	{
		p_File = _T("");
		p_Line = 0;
	}

	// try to get the name of the function containing this program counter address.
	static const ux MaxSymFuncNameLen = 1024;
	byte symbolBuffer[sizeof(SYMBOL_INFO) + (MaxSymFuncNameLen + 1) * sizeof(TCHAR)];
	PSYMBOL_INFO pSymbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
	ZeroMemory(pSymbol, sizeof(SYMBOL_INFO));
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MaxSymFuncNameLen;
	DWORD64 Displacement64 = 0;

	if(g_DbgHelpDll.SymFromAddr(m_hProcess, static_cast<DWORD64>(p_Address), &Displacement64, pSymbol))
	{
		p_Function = pSymbol->Name;
	}
	else
	{
		p_Function = _T("");
	}

#else	// PLATFORM_WINDOWS
	char** sym = backtrace_symbols((void*const*)&p_Address, 1);
	if(sym)
	{
#if defined(_UNICODE)
		std::wstring curSym;
		std::string tmpSym(sym[0]);
		std::transform(tmpSym.begin(), tmpSym.end(), std::back_inserter(curSym), Internal::CharToWide());
#else
		std::string curSym(sym[0]);
#endif

		Internal::CTokenizer tokenizer(curSym, _T(" ()"));
		if(tokenizer.GetNumTokens() > 1)
		{	
			p_File = tokenizer.GetToken(0);
			if(tokenizer.GetNumTokens() > 2)
				p_Function = tokenizer.GetToken(1);
			else
				p_Function = _T("");
		}
		else
		{
			p_Function = curSym;
			p_File = _T("");
		}

		free(sym);
	}
	else
	{
		p_Function = _T("");
		p_File = _T("");
	}

	p_Line = 0;
#endif	// PLATFORM_WINDOWS
}

void CSymbolResolver::ResolveSymbol(ux p_Address, ModuleHandle p_hModule, const tchar* p_ModuleName, tstring& p_Function, tstring& p_File, ux& p_Line)
{
#if defined(PLATFORM_WINDOWS)
	// check module
	if(p_hModule != NULL && p_ModuleName != NULL)
	{
		ModuleHandle hLoadedModule = GetModuleHandle(p_ModuleName);
		if(hLoadedModule != p_hModule)
		{
			// convert the module name if necessary (SymLoadModule64 only takes ansi strings)
#if defined(_UNICODE)
			std::string moduleName;
			std::wstring tmpModName(p_ModuleName);
			std::transform(tmpModName.begin(), tmpModName.end(), std::back_inserter(moduleName), Internal::WideToChar());
#else
			std::string moduleName(p_ModuleName);
#endif

			// Load module and query symbols
			DWORD64 DllBase = g_DbgHelpDll.SymLoadModule64(m_hProcess, NULL, moduleName.c_str(), NULL, static_cast<DWORD64>(reinterpret_cast<size_t>(p_hModule)), 0);
			assert(DllBase == static_cast<DWORD64>(reinterpret_cast<size_t>(p_hModule)));
			ResolveSymbol(p_Address, p_Function, p_File, p_Line);
			g_DbgHelpDll.SymUnloadModule64(m_hProcess, DllBase);
			return;
		}
	}
	ResolveSymbol(p_Address, p_Function, p_File, p_Line);
#else
	UNREFERENCED_PARAMETER(p_hModule);
	UNREFERENCED_PARAMETER(p_ModuleName);
	ResolveSymbol(p_Address, p_Function, p_File, p_Line);
#endif
}


} // namespace DbgLib
