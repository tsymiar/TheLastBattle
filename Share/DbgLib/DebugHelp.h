/*******************************************************************\
| DbgHelp.h
|--------------------------------------------------------------------
| CREATED:		2007/6/1
| AUTHOR:		Martin Fleisz
\********************************************************************/
#ifndef DBGLIB_DBGHELP_H
#define DBGLIB_DBGHELP_H

namespace DbgLib
{

//*****************************************************************************
// class overview:
//   Wrapper for debug helper API and libc debugging functions.
//-----------------------------------------------------------------------------
class CDebugHelp
{
	public:
		// dump types (only used on windows platforms)
		enum DumpType
		{
			TinyDump = 0,
			MiniDump = 1,	// default dump type
			MidiDump = 2,
			FullDump = 3
		};

	public:
		// sets a new dump type
		static void SetDumpType(DumpType p_Type);

		// sets the path were dump files are stored (default path is the current directory)
		static void SetDumpPath(const tchar* p_Path);

		// creates a mini dump (if IgnoreThread is true the current thread will be excluded from the dump)
		static void CreateMiniDump(bool p_IgnoreThread);

#if defined(PLATFORM_WINDOWS)
		// creates a mini dump (if IgnoreThread is true the current thread will be excluded from the dump)
		static void CreateMiniDump(PEXCEPTION_POINTERS p_ExceptionInfo, bool p_IgnoreThread);

		// creates a wer error report
		static void CreateWERReport(PEXCEPTION_POINTERS p_ExceptionInfo);
#endif

		// fills the buffer with the current stack trace, returns the number of elements written into p_Stack
		static ux DoStackWalk(ux* p_CallStack, ux p_BufferSize);

	private:
		// path were we store our dumps
		static tstring m_DumpPath;

#if defined(PLATFORM_WINDOWS)
	private:
		// callback for MiniDumpWriteDump function for querying debugging information (used to exclude mini dump creation thread from the dump file)
		static BOOL CALLBACK MiniDumpCallback(PVOID CallbackParam, const PMINIDUMP_CALLBACK_INPUT CallbackInput, PMINIDUMP_CALLBACK_OUTPUT CallbackOutput);

		static DWORD_PTR GetProgramCounter();

	private:
		// current dump type
		static ux m_DumpType;

		// dump type conversion table (DumpType -> MINIDUMP_TYPE)
		static const ux m_DumpTypeConverter[];
#endif // PLATFORM_WINDOWS

};


//*****************************************************************************
// class overview:
//   Helper class for symbols resolving
//-----------------------------------------------------------------------------
class CSymbolResolver
{
	public:
		CSymbolResolver(const tchar* p_SymbolSearchPath);
		~CSymbolResolver();

	public:
		// returns the standard symbol search path
		static tstring GetDefaultSymbolSearchPath();

		// resolve symbols for a specified address
		void ResolveSymbol(ux p_Address, tstring& p_Function, tstring& p_File, ux& p_Line);

		// resolve symbols for a specified address and module
		void ResolveSymbol(ux p_Address, ModuleHandle p_hModule, const tchar* p_ModuleName, tstring& p_Function, tstring& p_File, ux& p_Line);

#if defined(PLATFORM_WINDOWS)
	private:
		HANDLE m_hProcess;
#endif

};


} // DbgLib

#endif // DBGLIB_DBGHELP_H
