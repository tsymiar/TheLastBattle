#include "StdAfx.h"
#include "AbsorbAI.h"
#include "Player.h"
#include "RobotNPC.h"
#include "Robot.h"
#include "RobotMgr.h"

namespace ReBot
{

	INT32 AbsorbAI::Begin(PlayerAIData& sPlayerAIData){
		m_IfSend = FALSE;
		m_pPlayer = sPlayerAIData.pPlayer;
		INT32 n32Res = CheckValid();
		if (eNormal != n32Res){
			return n32Res;
		}
		m_pPlayer->GetClient()->AskAbsorb();
		m_RebotAIState = eRebotAIState_Running;
		return eNormal;
	}

	INT32 AbsorbAI::Update(TIME_MILSEC tCurTime){
		if (tCurTime - m_BeginTime > 10*1000){
			End();
		}

		return eNormal;
	}

	INT32 AbsorbAI::End(){
		m_RebotAIState = eRebotAIState_Complete;
		return eNormal;
	}

	INT32 AbsorbAI::CheckValid(){
		if (m_pPlayer->m_un32Level < 3){
			return eEC_AbsorbMonsterFail;
		}

		if (NULL == m_pPlayer->m_LockEnemy){
			return eEC_AbsorbMonsterFail;
		}

		CRobotNPC* pMonster = (CRobotNPC*)m_pPlayer->m_LockEnemy;
		if (pMonster->GetState() == eOAS_Dead){
			return eEC_AbsorbMonsterFailForMonsterDead;
		}

		m_BeginTime = gRobotMgr->GetTickCount();
		return eNormal;
	}

	AbsorbAI::~AbsorbAI(){

	}

	INT32 AbsorbAI::Clear(){
		End();
		return eNormal;
	}

	AbsorbAI::AbsorbAI(){
	}

}
