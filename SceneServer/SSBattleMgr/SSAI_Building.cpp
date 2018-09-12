#include "SSAI_Building.h"
#include "SSNPC.h"
#include "SSBattle.h"

namespace SceneServer{

CSSAI_Building::CSSAI_Building(CSSGameUnit* pGU):CSSAI(pGU)
	, m_pcBuildingGU(NULL)
{
	if (m_pcMasterGU){
		m_pcBuildingGU = m_pcMasterGU->TryCastToNPC();
	}
}

INT32	CSSAI_Building::HeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	//没有攻击技能的塔，不执行AI
	if(NULL == m_pAttackSkill){
		return eNormal;
	}
	if(eNormal == m_pAttackSkill->IfSkillUsable()){
		//建筑有可能是召唤建筑。召唤建筑对野怪是有攻击的，箭塔无攻击
		CSSGameUnit* pEnemy = m_pcBuildingGU->LookForEnemy(NULL, 0, m_pcMasterGU->GetFatherHeroGUID() > 0?TRUE:FALSE);
		//有目标，则使用普通攻击技能
		if(NULL != pEnemy){
			m_pAttackSkill->pcTarGU = pEnemy;
			INT32	rst = m_pAttackSkill->Start();
			//如果施法失败(基本不会，为将来可能对塔生效控制技能准备),则返回，下一次心跳再来
			if(eNormal != rst){
				return rst;
			}
		}
	}
	return eNormal;
}



}