#include "stdafx.h"
 
#include "CSUserMgr.h"
#include "CSUser.h" 
#include "../ELogging/ELogging/SSActive.h"
#include "boost/spirit.hpp" 
#include "boost/algorithm/string.hpp"
 
namespace CentralServer
{
	//cs gm cmd
	//eg:  
	/*	golde	1000;   
		diamond 1000
		level	10
	// 
	*/
	INT32		CCSUserMgr::OnMsgFromGC_AskNewGMCmd(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
	{
		CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
		GCToCS::AddCSGMCmd pMsg;
		if (!ParseProtoMsg(pData, n32DataLength, pMsg)){
			return 0;
		}
		 
		if (pMsg.gmcmd().empty())
		{
			ELOG(LOG_ERROR,"the length of cmd is zero" );
			return eNormal;
		}  
		std::string gmd(strlwr((char*) pMsg.gmcmd().c_str() ));
		std::vector<std::string> vecCmd;
		boost::algorithm::split( vecCmd, gmd, boost::algorithm::is_any_of(" ") );
		if (vecCmd.size() < 2 || vecCmd.size() > 3 )
		{
			ELOG(LOG_ERROR,"the error cmd :%s!",pMsg.gmcmd().c_str() );
			return eNormal;
		}
		string& keyStr = vecCmd[0];


		if (keyStr.compare("diamond") == 0)
		{
			INT32  tn = atoi(vecCmd[1].c_str());
			pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_Diamond, tn);
			pcUser->SynCurDiamond();
		}
		else if (keyStr.compare("gold")  == 0)
		{
			INT32  tn = atoi(vecCmd[1].c_str());
			pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_Gold, tn);
			pcUser->SynCurGold();
		}
		else if (keyStr.compare("goods")  == 0)
		{
			INT32  gid = atoi(vecCmd[1].c_str());
			INT32  tn = atoi(vecCmd[2].c_str());
		}
		else if (keyStr  == "level"){
			auto newLevel = atoi(vecCmd[1].c_str());
			pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_UserLv, newLevel);
			pcUser->SynUserLvInfo();
		}
		else
		{
			ELOG(LOG_ERROR,"the error cmd:%s", pMsg.gmcmd().c_str());
			return eNormal;
		}
		ELOG(LOG_SpecialDebug,"get a new  cmd:%s", pMsg.gmcmd().c_str());
		return eNormal;
	}
}
