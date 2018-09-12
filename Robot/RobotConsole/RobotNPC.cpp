#include "StdAfx.h"
#include "RobotNPC.h"
#include "CfgMgr.h"

namespace ReBot
{

CRobotNPC::~CRobotNPC(){

}

INT32 CRobotNPC::LoadNPCCfg(const SNPCCfg* pcCfg){
	if (NULL == pcCfg){
		return eEC_NullPointer;
	}
	m_cpsNPCCfg = pcCfg;

	for (INT32 i = 0; i < c_n32MaxNPCSkillNum; ++i){
		m_asSkillRunTimeData[i].Clear();
	}

	for (INT32 i = 0; i < c_n32MaxNPCSkillNum; i++){
		UINT32 un32SkillID = m_cpsNPCCfg->aun32SkillList[i];
		if (0 == un32SkillID){
			continue;
		}
		const SSNewSkillCfg *cpsCfg = CCfgMgr::getInstance().GetSkillCfg(un32SkillID);
		if (NULL == cpsCfg){
			continue;
		}

		RobotSkillData& cSkill = m_asSkillRunTimeData[i];
		cSkill.cpsCfg = cpsCfg;
		cSkill.eSkillState = eSkillState_Free;
		cSkill.tCooldownMilsec = 0;
	}

	return eNormal;
}

INT32 CRobotNPC::Clear(){
	CClientGameObject::Clear();
	m_cpsNPCCfg = NULL;
	for (INT32 i = 0; i < c_n32MaxNPCSkillNum; ++i){
		m_asSkillRunTimeData[i].Clear();
	}

	return eNormal;
}

}