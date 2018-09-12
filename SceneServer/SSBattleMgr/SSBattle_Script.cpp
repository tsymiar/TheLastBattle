
#include "SSBattle_Script.h"

#include "SSBattle.h"
#include "SSBattleMgr.h"
#include "SSGameObject.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSNPC.h"

#include "SSArea.h"
#include "SSAltarSev.h"
#include "SSEffectMgr.h"
#include "SSOBSev.h"
#include "SSVMMgr.h" 

namespace SceneServer{
boost::atomic<bool> SSBattle_Script::m_IfInitAllCfg(false);

#define GetGlobalBattle(bIdx) 				\
	if(bIdx < 1){							\
	return eEC_NullPointer;}			\
	CSSBattle *pBattle = m_pCSSBattleMgr->GetBattle(bIdx);	\
	if (NULL == pBattle)		{					\
	ELOG(LOG_ERROR, "Null Battle with id:%d", bIdx);\
	return eEC_NullPointer;		}									 


	SSBattle_Script::SSBattle_Script():m_pcLuaState(LuaState::Create())
		, m_pCSSBattleMgr(NULL){ 
			m_pcLuaState->OpenLibs();  
			RegisterScriptFunctions();  
	}

	SSBattle_Script::~SSBattle_Script() 
	{
		if (NULL != m_pcLuaState){
			LuaState::Destroy(m_pcLuaState);
			m_pcLuaState = NULL;
		} 
	}

	void SSBattle_Script::InitPublicLua(){ 

		INT32	n32DoStrRet = m_pcLuaState->DoFile("./CSBattleMgr/Lua/SSLua/load.lua");   

		if (eNormal != n32DoStrRet){
			LuaStack st(m_pcLuaState);
			const char *pszError = st[1].GetString();
			ELOG(LOG_ERROR,"--error luaStack:%s",pszError);
		}
	}

	void SSBattle_Script::InitGlobalCfg(){
		//ExecuteScriptFunc("InitRandom", NULL, 0,  false);
		if (!m_IfInitAllCfg){
			ExecuteScriptFunc("initGameConfig", NULL, 0);
			m_IfInitAllCfg = true;
		}
	}

	 
	void	SSBattle_Script::RegisterScriptFunctions( ){  

		LuaObject cLuaObjBattle = m_pcLuaState->GetGlobals().CreateTable("ThisBattle");  
  
		cLuaObjBattle.Register("LogDebug", *this, &SSBattle_Script::LuaLogDebug);
		cLuaObjBattle.Register("LogInfo", *this, &SSBattle_Script::LuaLogInfo);
		cLuaObjBattle.Register("LogWarning", *this, &SSBattle_Script::LuaLogWarning);
		 cLuaObjBattle.Register("LogError", *this, &SSBattle_Script::LuaLogError);
	}

	bool	SSBattle_Script::ExecuteScriptFunc(  const char* pszFuncName, const void* un32ParamArray, INT32 nParamCount,   ELuaParamType paramType) 
	{
		if (NULL == m_pcLuaState){
			return false;
		}
		INT32  n32Ret = 0;
		LuaObject pcLuaObj = m_pcLuaState->GetGlobal(pszFuncName); 
		if (!pcLuaObj.IsFunction())
		{
			Assert(false && "%s, is not lua func.", pszFuncName );
			return false;
		} 
		try{ 
			switch(paramType)
			{
			case eELua_Param_Number:
				{
					LuaFunction<INT32> func(pcLuaObj); 
					const INT32 * paramArray = (INT32*)un32ParamArray;
					if (nParamCount == 0) 
						n32Ret = (INT32) func(); 
					else if (nParamCount == 1)
						n32Ret =  func( paramArray[0] ); 
					else if ( nParamCount == 2 )
						n32Ret =  func( paramArray[0],  paramArray[1]);
					else if ( nParamCount == 3 )
						n32Ret =  func( paramArray[0],  paramArray[1],  paramArray[2]);
					else if ( nParamCount == 4 )
						n32Ret = func( paramArray[0],  paramArray[1],  paramArray[2],  paramArray[3]);
					else if ( nParamCount == 5 )
						n32Ret = func( paramArray[0],  paramArray[1],  paramArray[2],  paramArray[3],paramArray[4]);
					else
						return false;
				}break;
			case	eELua_Param_String:
				{
					LuaFunction<char> func(pcLuaObj); 
					const char * paramArray = (char*)un32ParamArray;
					if (nParamCount == 0)
						n32Ret = (INT32) func();
					else if (nParamCount == 1)
						n32Ret =  (INT32) func( paramArray );  
					else
						return false;
				}break;
			}

		}catch(LuaException& e){
			Assert(false && "%s, ErrMsg %s.", pszFuncName, e.GetErrorMessage());
			return false;
		} 
		return true;
	} 

	 void		SSBattle_Script::SetBattleMgr(CSSBattleMgr* pMgr)
	 {
	 }
 

	INT32	SSBattle_Script::LuaLogDebug(LuaState *pcState){
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

	INT32	SSBattle_Script::LuaLogInfo(LuaState *pcState){
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

	INT32	SSBattle_Script::LuaLogWarning(LuaState *pcState){
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

	INT32	SSBattle_Script::LuaLogError(LuaState *pcState){
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

	LuaPlus::LuaObject SSBattle_Script::GetLuaArgs(LuaState* pState, INT32 n32Index){
		LuaStack args(pState);
		return args[n32Index + 1];
	}

}
