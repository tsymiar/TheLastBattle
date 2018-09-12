#pragma once
#include "GameUnit.h"

namespace ReBot
{
class CRobotNPC : public CClientGameObject
{
public:
	virtual ~CRobotNPC();

	virtual	RobotSkillData*		GetSkillData(UINT8 un8SkillSlotID){return &m_asSkillRunTimeData[un8SkillSlotID];}
	const SNPCCfg*				GetNPCCfg(){return m_cpsNPCCfg;}
	INT32						LoadNPCCfg(const SNPCCfg* pcCfg);
	virtual INT32				Clear();
	virtual	INT32				GetCollideRadius(){return m_cpsNPCCfg->n32CollideRadius;}

private:
	const SNPCCfg*				m_cpsNPCCfg;
	RobotSkillData				m_asSkillRunTimeData[c_n32MaxNPCSkillNum];
};
}
