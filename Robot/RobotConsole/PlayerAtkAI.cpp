#include "StdAfx.h"
#include "PlayerAtkAI.h"
#include "Player.h"
#include "Robot.h"
#include "PlayerMoveAI.h"

namespace ReBot
{
	CPlayerAtkAI::CPlayerAtkAI():m_pPlayerMoveAI(NULL)
		, m_RobotAttackState(eRobotAttackState_Free){

	}

	CPlayerAtkAI::~CPlayerAtkAI(){

	}

	INT32 CPlayerAtkAI::Update(TIME_MILSEC tCurTime){
		if (m_pPlayer->GetState() > eOAS_PassiveState){
			return eNormal;
		}

		CClientGameObject* pEnemy = m_pPlayer->FindRandEnemyAndLock();
		if (NULL == pEnemy){
			m_RebotAIState = eRebotAIState_Complete;
			return eNormal;
		}

		if (m_RobotAttackState == eRobotAttackState_Pursue){
			if (m_pPlayerMoveAI->GetAIState() != eRebotAIState_Running){
				End();
				return eNormal;
			}

			m_pPlayerMoveAI->Update(tCurTime);
		}
		return eNormal;
	}

	INT32 CPlayerAtkAI::End(){
		m_RebotAIState = eRebotAIState_Complete;
		m_RobotAttackState = eRobotAttackState_Free;
		return eNormal;
	}

	INT32 CPlayerAtkAI::Clear(){
		return eNormal;
	}

	INT32 CPlayerAtkAI::Begin(PlayerAIData& sPlayerAIData){
		m_pPlayer = sPlayerAIData.pPlayer;
		m_RebotAIState = eRebotAIState_Running;
		if (NULL == m_pPlayerMoveAI){
			m_pPlayerMoveAI = GetMoveAI();
		}
		else{
			m_pPlayerMoveAI->End();
		}

		return eNormal;
	}

}
