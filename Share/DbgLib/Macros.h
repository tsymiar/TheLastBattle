/*******************************************************************\
| Macros.h
|--------------------------------------------------------------------
| CREATED:		2006/11/15/
| AUTHOR:		Martin Fleisz
|--------------------------------------------------------------------
| DESCRIPTION:
| 
| Definition of some useful macros
\********************************************************************/
#ifndef DBGLIB_MACROS_H
#define DBGLIB_MACROS_H

//////////////////////////////////////////////////////////////////////////
// cleanup macros
#define SAFE_DELETE(p)		{ delete (p);	(p)=NULL; }
#define SAFE_ARRAYDELETE(p)	{ delete [](p);	(p)=NULL; }
#define SAFE_RELEASE(p)		{ if((p)) { (p)->Release(); (p)=NULL; } }

//////////////////////////////////////////////////////////////////////////
// text constant macros

// helper macro for _WT()
#define __WT(x)		L ## x

// define explicit unicode (_WT) and ansi (_AT) text macros
#define _WT(x)		__WT(x)
#define _AT(x)		x

#if !defined(_T)
#if defined(_UNICODE)
#define _T(x)      _WT(x)
#else
#define _T(x)      _AT(x)
#endif	// _UNICODE
#endif

#if defined(_UNICODE)
#define _TFILE		__FILEW__
#define _TFUNCTION	__FUNCTIONW__
#else	// _UNICODE
#define _TFILE		__FILE__
#define _TFUNCTION	__FUNCTION__
#endif	// _UNICODE

//////////////////////////////////////////////////////////////////////////
#if !defined(UNREFERENCED_PARAMETER)
#define UNREFERENCED_PARAMETER(P)          (P)
#endif

//////////////////////////////////////////////////////////////////////////
// string concatenation helper macros

// the ugly casting is necessary for the following reasons:
// first cast (to basic_ostream) is necessary to ensure that the _T macro works
// second cast (to tostringstream) is required for .str() to work

// concatenates the given stream elements in _expr and returns a const tchar* value (don't save this value for later use!!!)
#define s_cstr(expr_) (reinterpret_cast<tostringstream&>(reinterpret_cast<std::basic_ostream<tchar>&>(tostringstream()) << expr_)).str().c_str()
#define s_cstrA(expr_) (reinterpret_cast<std::ostringstream&>(reinterpret_cast<std::basic_ostream<char>&>(std::ostringstream()) << expr_)).str().c_str()
#define s_cstrW(expr_) (reinterpret_cast<std::wostringstream&>(reinterpret_cast<std::basic_ostream<wchar_t>&>(std::wostringstream()) << expr_)).str().c_str()

// concatenates the given stream elements in _expr and returns a tstring value
#define s_str(expr_) (reinterpret_cast<tostringstream&>(reinterpret_cast<std::basic_ostream<tchar>&>(tostringstream()) << expr_)).str()
#define s_strA(expr_) (reinterpret_cast<std::ostringstream&>(reinterpret_cast<std::basic_ostream<char>&>(std::ostringstream()) << expr_)).str()
#define s_strW(expr_) (reinterpret_cast<std::wostringstream&>(reinterpret_cast<std::basic_ostream<wchar_t>&>(std::wostringstream()) << expr_)).str()

#endif // DBGLIB_MACROS_H
