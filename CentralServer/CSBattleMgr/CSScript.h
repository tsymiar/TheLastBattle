#ifndef SSBATTLE_SCRIPT_H_
#define SSBATTLE_SCRIPT_H_  

#ifdef _WINDOWS
#include "stdafx.h"
#endif 
#include <string>
#include <boost/atomic.hpp>
#include	"LuaPlus.h"
 using namespace LuaPlus;
namespace CentralServer{
 

class CSScript
{
	enum eLogType
	{
		eLogType_Debug = 1,
		eLogType_Error,
		eLogType_Warning,
		eLogType_Info
	};

	LuaState*			m_pcLuaState; 
	 
	static boost::atomic<bool> m_IfInitAllCfg;
public:
	CSScript();
	~CSScript();   
	 
	 void				InitLua(); 
	 void				InitGlobalCfg();
	 void				RegisterScriptFunctions();
	 bool				ExecuteScriptFunc(  const char* pszFuncName, const void* un32ParamArray, INT32 nCount,  ELuaParamType paramType = eELua_Param_Number);
	  
	 LuaPlus::LuaObject  GetLuaArgs(LuaState* pState, INT32 n32Index);
	 
	 INT32				LuaLogDebug(LuaState *pcState);
	 INT32				LuaLogInfo(LuaState *pcState);
	 INT32				LuaLogWarning(LuaState *pcState);
	 INT32				LuaLogError(LuaState *pcState);
	 
};
 
}


#endif
