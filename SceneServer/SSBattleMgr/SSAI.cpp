#include "SSAI.h"
#include "SSGameUnit.h"
#include "SSBattle.h"
#include "SSSkill.h"

namespace SceneServer{

CSSAI::CSSAI(CSSGameUnit* pMaster):m_pcMasterGU(pMaster), m_eAttackState(eAttackState_Pursue)
, m_ifMoving(FALSE)
, m_pcAttackGU(NULL)
, m_pcMoveTarPos(0,0,0)
, m_tLastCheckMoveTarTime(0)
, m_bIfFliter(FALSE)
, m_pAttackSkill(NULL)
, m_tLastTryMoveTime(0)
{
	
}

CSSAI::~CSSAI(){

}

bool	CSSAI::IfPassitiveState(){
	if(NULL == m_pcMasterGU){
		return TRUE;
	}
	if (eOAS_PassiveState < m_pcMasterGU->GetGOActionStateInfo().eOAS){
		return TRUE;
	}
	if( m_pcMasterGU->GetFPData(eEffectCate_Dizziness) > 0){
		return TRUE;
	}
	return FALSE;
}

void	CSSAI::MoveToTar(CVector3D pos, bool ifMoveToBlackPoint, TIME_TICK nowTime){
	if(m_tLastTryMoveTime != 0 && nowTime - m_tLastTryMoveTime < 300){
		return;
	}
	if(m_pcMasterGU->GetCurBattle()->AskStartMoveToTar(m_pcMasterGU,pos,ifMoveToBlackPoint,m_bIfFliter)){
		m_ifMoving = TRUE;
		m_tLastTryMoveTime = 0;
	}
	else{
		m_ifMoving = FALSE;
		m_pcMasterGU->BeginAction_Free(TRUE);
		m_tLastTryMoveTime = GetUTCMiliSecond();
	}
	m_pcMasterGU->SetCurEnemyGUID(0);
}

void	CSSAI::CancleAttack(){
	m_pcMoveTarPos.Clear();
	if (eAttackState_Pursue == m_eAttackState){
		m_pcMasterGU->GetCurBattle()->AskStopMoveObjectTar(m_pcMasterGU);
	}
	//尝试取消技能。普通攻击技能其实是肯定可以取消成功的,所以不用判断返回值
	m_pAttackSkill->TryCancle();
	/*
	else if (eAttackState_UseSkill == m_eAttackState){
		//尝试取消技能。普通攻击技能其实是肯定可以取消成功的,所以不用判断返回值
		m_pAttackSkill->TryCancle();
	}
	*/
}

void	CSSAI::StopAttack(){
	CancleAttack();
	SetAttackGU(NULL);
	m_pcMoveTarPos.Clear();
	m_tLastCheckMoveTarTime = 0;
}

INT32	CSSAI::AttackHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	INT32 rst = eNormal;
	//检查技能
	if(NULL == m_pAttackSkill){
		return eEC_AbsentOrderPriority;
	}
	//检查状态
	if(TRUE == IfPassitiveState()){
		if(IfMoving())
			m_pcMasterGU->GetCurBattle()->AskStopMoveObjectTar(m_pcMasterGU);
		m_pAttackSkill->End();
		return eEC_AbsentOrderPriority;
	}

	//检查攻击目标状态
	if(GetAttackGU() == NULL || GetAttackGU()->IsDead() || GetAttackGU()->GetGOActionStateInfo().eOAS == eOAS_Reliving){
		SetAttackGU(NULL);
		if(IfMoving())
			m_pcMasterGU->GetCurBattle()->AskStopMoveObjectTar(m_pcMasterGU);
		m_eAttackState = eAttackState_Pursue;
		m_pAttackSkill->End();
		return eEC_TargetIsDead;
	}

	if (eAttackState_Pursue == m_eAttackState){
		//检查是否到了施法距离
		if (TRUE == m_pcMasterGU->IfInReleaseSkillRange(GetAttackGU(), m_pAttackSkill->cpsCfg)){
			m_eAttackState = eAttackState_UseSkill;
			m_pcMasterGU->GetCurBattle()->AskStopMoveObjectTar(m_pcMasterGU);
			m_pcMasterGU->GetCurBattle()->AskStopMoveObjectDir(m_pcMasterGU);
		}else{
			//检查移动状态，是否需用开始或者重新开始移动
			TIME_TICK tLastCheckTickSpan = tUTCMilsec - m_tLastCheckMoveTarTime;
			if (500 <= tLastCheckTickSpan){
				if(m_pcMoveTarPos.m_fX != GetAttackGU()->GetCurPos().m_fX || m_pcMoveTarPos.m_fZ != GetAttackGU()->GetCurPos().m_fZ){
					m_pcMoveTarPos = GetAttackGU()->GetCurPos();
					//开始移动
					MoveToTar(m_pcMoveTarPos,FALSE, 0);
				}
				m_tLastCheckMoveTarTime = tUTCMilsec;
			}
			//如果停下来了，重新启动移动
			if(FALSE == m_ifMoving){
				MoveToTar(m_pcMoveTarPos,FALSE, tUTCMilsec);
			}
			
		}
	}

	if (eAttackState_UseSkill == m_eAttackState){
		if(IfMoving()){
			m_pcMasterGU->GetCurBattle()->AskStopMoveObjectTar(m_pcMasterGU);
			m_pcMasterGU->GetCurBattle()->AskStopMoveObjectDir(m_pcMasterGU);
		}
		if(m_pAttackSkill != NULL){
			//检查是否是正在施法状态
			if(FALSE == m_pAttackSkill->ifRunning){
				//如果技能没有在使用中，则检查施法距离
				if (TRUE == m_pcMasterGU->IfInReleaseSkillRange(GetAttackGU(), m_pAttackSkill->cpsCfg)){
					//距离内，则开始施法
					m_pAttackSkill->pcTarGU = GetAttackGU();
					rst = m_pAttackSkill->Start();
					//如果施法失败(CD中，缴械等原因),则返回，下一次心跳再来
					if(eNormal != rst){
						return rst;
					}
				}
				else{
					//距离外，切换到移动模式
					m_eAttackState = eAttackState_Pursue;
					return eNormal;
				}
			}
			else{
				//技能在施法中，调用技能心跳
				INT32 rst = m_pAttackSkill->HeartBeat(tUTCMilsec, tTickSpan);
				if(eNormal != rst){
					if(eSkillState_End != rst && m_pcMasterGU->GetGOActionStateInfo().eOAS != eOAS_Free){
						m_pcMasterGU->BeginAction_Free(TRUE);
					}
					m_pAttackSkill->End();
					return rst;
				}
			}
		}
	}
	

	return eNormal;
}

bool	CSSAI::IfAttacking(){
	return m_pAttackSkill != NULL && m_pAttackSkill->ifRunning;
}

void	CSSAI::OnMoveBlock(){
	m_ifMoving = FALSE;
}

void	CSSAI::TryFree(){
	if(m_pcMasterGU->GetGOActionStateInfo().eOAS < eOAS_PassiveState){
		m_pcMasterGU->BeginAction_Free(TRUE);
	}
}

void	CSSAI::SetAttackGU(CSSGameUnit* pcAttackGU){
	m_pcAttackGU = pcAttackGU;
	if(m_pAttackSkill != NULL){
		m_pAttackSkill->pcTarGU = m_pcAttackGU;
	}
}

}