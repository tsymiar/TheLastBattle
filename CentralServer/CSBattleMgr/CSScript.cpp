
#include "CSScript.h"

 

namespace CentralServer{
boost::atomic<bool> CSScript::m_IfInitAllCfg(false); 

	CSScript::CSScript():
		m_pcLuaState(LuaState::Create()) { 
			m_pcLuaState->OpenLibs();  

			RegisterScriptFunctions();  

			InitLua();

			/*InitGlobalCfg();*/ 
	}

	CSScript::~CSScript() 
	{
		if (NULL != m_pcLuaState){
			LuaState::Destroy(m_pcLuaState);
			m_pcLuaState = NULL;
		} 
	}

	void CSScript::InitLua(){ 

		INT32	n32DoStrRet = m_pcLuaState->DoFile("./CSBattleMgr/Lua/CSLua/load.lua");   

		if (eNormal != n32DoStrRet){
			LuaStack st(m_pcLuaState);
			const char *pszError = st[1].GetString();
			ELOG(LOG_ERROR,"--error luaStack:%s",pszError);
		}
	}

	void CSScript::InitGlobalCfg(){ 
		if (!m_IfInitAllCfg){
			ExecuteScriptFunc("initGameConfig", NULL, 0);
			m_IfInitAllCfg = true;
		}
	} 

	void	CSScript::RegisterScriptFunctions( ){  

		LuaObject cLuaObjBattle = m_pcLuaState->GetGlobals().CreateTable("ThisBattle");   
		 
		cLuaObjBattle.Register("LogDebug", *this, &CSScript::LuaLogDebug);
		cLuaObjBattle.Register("LogInfo", *this, &CSScript::LuaLogInfo);
		cLuaObjBattle.Register("LogWarning", *this, &CSScript::LuaLogWarning);
		cLuaObjBattle.Register("LogError", *this, &CSScript::LuaLogError);
		//cLuaObjBattle.Register("AddMapBattleError", *this, &CSScript::LuaAddMapBattleError);
	}

	bool	CSScript::ExecuteScriptFunc(  const char* pszFuncName, const void* un32ParamArray, INT32 nCount,   ELuaParamType paramType) 
	{
		if (NULL == m_pcLuaState){
			return false;
		}
		INT32  n32Ret = 0;
		LuaObject pcLuaObj = m_pcLuaState->GetGlobal(pszFuncName); 
		if (!pcLuaObj.IsFunction())
		{
			ELOG(LOG_WARNNING, "%s, is not lua func.", pszFuncName );
			return false;
		} 
		try{ 
			switch(paramType)
			{
			case eELua_Param_Number:
				{
					LuaFunction<INT32> func(pcLuaObj); 
					const INT32 * paramArray = (INT32*)un32ParamArray;
					if (nCount == 0) 
						n32Ret = (INT32) func(); 
					else if (nCount == 1)
						n32Ret =  func( paramArray[0] ); 
					else if ( nCount == 2 )
						n32Ret =  func( paramArray[0],  paramArray[1]);
					else if ( nCount == 3 )
						n32Ret =  func( paramArray[0],  paramArray[1],  paramArray[2]);
					else if ( nCount == 4 )
						n32Ret = func( paramArray[0],  paramArray[1],  paramArray[2],  paramArray[3]);
					else if ( nCount == 5 )
						n32Ret = func( paramArray[0],  paramArray[1],  paramArray[2],  paramArray[3],paramArray[4]);
					else
						return false;
				}break;
			case	eELua_Param_String:
				{
					LuaFunction<char> func(pcLuaObj); 
					const char * paramArray = (char*)un32ParamArray;
					if (nCount == 0)
						n32Ret = (INT32) func();
					else if (nCount == 1)
						n32Ret =  (INT32) func( paramArray );  
					else
						return false;
				}break;
			}

		}catch(LuaException& e){
			ELOG(LOG_WARNNING, "%s, ErrMsg %s.", pszFuncName, e.GetErrorMessage());
			return false;
		} 
		return true;
	}  


	INT32	CSScript::LuaLogDebug(LuaState *pcState){
		LuaObject cArgs1 = GetLuaArgs(pcState, 1);
		if (!cArgs1.IsString()){
			return eEC_InvalidScriptParameters;
		}

		const CHAR *cpszLogStr = cArgs1.GetString();	
		UINT32 un32StrLen = (UINT32)strlen(cpszLogStr);
		if (0 >= un32StrLen){
			return eEC_InvalidScriptParameters;
		}
		ELOG(LOG_DBBUG, (char*)cpszLogStr);
		return eNormal;
	}

	INT32	CSScript::LuaLogInfo(LuaState *pcState){
		LuaObject cArgs1 = GetLuaArgs(pcState, 1);
		if (!cArgs1.IsString()){
			return eEC_InvalidScriptParameters;
		}

		const CHAR *cpszLogStr = cArgs1.GetString();	
		UINT32 un32StrLen = (UINT32)strlen(cpszLogStr);
		if (0 >= un32StrLen){
			return eEC_InvalidScriptParameters;
		}
		ELOG(LOG_INFO, (char*)cpszLogStr);
		return eNormal;
	}

	INT32	CSScript::LuaLogWarning(LuaState *pcState){
		LuaObject cArgs1 = GetLuaArgs(pcState, 1);
		if (!cArgs1.IsString()){
			return eEC_InvalidScriptParameters;
		}

		const CHAR *cpszLogStr = cArgs1.GetString();	
		UINT32 un32StrLen = (UINT32)strlen(cpszLogStr);
		if (0 >= un32StrLen){
			return eEC_InvalidScriptParameters;
		}

		ELOG(LOG_WARNNING, (char*)cpszLogStr);
		return eNormal;
	}

	INT32	CSScript::LuaLogError(LuaState *pcState){
		LuaObject cArgs1 = GetLuaArgs(pcState, 1);
		if (!cArgs1.IsString()){
			return eEC_InvalidScriptParameters;
		}

		const CHAR *cpszLogStr = cArgs1.GetString();
		UINT32 un32StrLen = (UINT32)strlen(cpszLogStr);
		if (0 >= un32StrLen){
			return eEC_InvalidScriptParameters;
		}
		ELOG(LOG_ERROR, (char*)cpszLogStr);
		return eNormal;
	}

	LuaPlus::LuaObject CSScript::GetLuaArgs(LuaState* pState, INT32 n32Index){
		LuaStack args(pState);
		return args[n32Index + 1];
	}

}
