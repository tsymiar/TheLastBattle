/*******************************************************************\
| DebugFx.h
|--------------------------------------------------------------------
| CREATED:		2007/7/10
| AUTHOR:		Martin Fleisz
\********************************************************************/
#ifndef DBGLIB_DEBUGFX_H
#define DBGLIB_DEBUGFX_H

namespace DbgLib
{
// declaration of the user-defined callback, called by the exception handler
typedef void (*EXCEPTION_CALLBACK)(void*);

//*****************************************************************************
// Debugging framework class that installs exception filters and 
// signal handlers to handle application crashes and provides access
// to the memory leak detector.
//-----------------------------------------------------------------------------
class CDebugFx
{
	public:
		// if p_Enable is true an unhandled exception filter/signal handlers is installed that creates a dump 
		// of the process and calls a user defined callback function (see SetExceptionCallback)
		// if p_Enalbe is false a previously installed handler will be removed
		static void SetExceptionHandler(bool p_Enable);

		// sets a new exception handler callback function
		static void SetExceptionCallback(EXCEPTION_CALLBACK p_Func, void* p_Param);

		static void GetStackTrackInfo(tstringstream& Buffer);

	private:
		// user defined callback that is called after the exception handler finished its work
		static EXCEPTION_CALLBACK m_ExceptionCallback;

		// user defined parameter passed to the exception callback
		static void* m_Param;

#if defined(PLATFORM_WINDOWS)
	private:
		// the previous unhandled exception filter function
		static LPTOP_LEVEL_EXCEPTION_FILTER m_PrevUnhandledExceptionHandler;

		// unhandled exception filter function used for SetUnhandledExceptionFilter
		static LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* p_ExcInfo);

		// unhandled exception handler (can also be used as a thread start procedure in case of a stack overflow)
		static DWORD WINAPI UnhandledExceptionHandlerFunc(LPVOID p_Param);

#else
	private:
		// handle process/thread termination
		//static void SigTermHandler(int p_Signal);

		// handles abort signal in a thread
		static void SigAbortHandler(int p_Signal);

		// handles segmentation faults
		static void SigSegvHandler(int p_Signal);
#endif
};

}

#endif // DBGLIB_DEBUGFX_H
