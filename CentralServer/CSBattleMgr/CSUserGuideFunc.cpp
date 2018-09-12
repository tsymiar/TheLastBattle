// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
 
#include "CSUser.h"
#include "CSUserMgr.h"
#include "CSCfgMgr.h" 

using namespace std; 

namespace CentralServer{   
	INT32	CCSUser::PosUserCSCurtGuideSteps(){
 
		GSToGC::GuideCSStepInfo  pMsg;

		std::string guideStr(m_sUserDBData.mGuideSteps.szCSContinueGuide);
		if(-1 != guideStr.find("ok"))
		{
			pMsg.set_allcomp(true);
		}
		else
		{
			pMsg.set_allcomp(false);

			std::vector<std::string> guideVec;
			boost::algorithm::split( guideVec, guideStr, boost::algorithm::is_any_of(GUIDE_SIGN) );  
			for (int i=0; i< guideVec.size(); i++)
			{
				if (guideVec[i].length() > 0)
				{
					pMsg.add_taskid( atoi(guideVec[i].c_str()));
				}
			}
		} 
		PostMsgToGC(pMsg,pMsg.msgid()); 
		return eNormal;
	}
}
