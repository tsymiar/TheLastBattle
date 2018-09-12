#include "StdAfx.h"
#include "PlayerMoveAI.h"
#include "Player.h"
#include "Robot.h"
#include "RobotMgr.h"

namespace ReBot
{
	CPlayerMoveAI::CPlayerMoveAI():m_BIfBegin(FALSE){

	}

	CPlayerMoveAI::~CPlayerMoveAI(){

	}

	INT32 CPlayerMoveAI::Update(TIME_MILSEC tCurTime){
		if (NULL == m_pPlayer){
			return eNormal;
		}

		if ((m_BeginPos - m_pPlayer->GetCurPos()).length() < 100){
			return eNormal;
		}

		if (m_pPlayer->GetState() != eOAS_Running){
		//	ELOG(LOG_ERROR, "Stop");
			End();
		}

		//if (m_TarPos.CanWatch((FLOAT)(c_n32TitleBattleRegionSize + c_n32BattleHalfRegionSize), m_pPlayer->GetCurPos())){
		//	ELOG(LOG_ERROR, "Stop");
		//	End();
		//}

		return eNormal;
	}

	INT32 CPlayerMoveAI::End(){
		if (FALSE == m_BIfBegin){
			return eNormal;
		}

		if (NULL == m_pPlayer){
			return eNormal;
		}

		if (m_pPlayer->GetState() == eOAS_Running){
			m_pPlayer->GetClient()->AskStopMove();
		}

		m_RebotAIState = eRebotAIState_Complete;
		return eNormal;
	}

	INT32 CPlayerMoveAI::Clear(){
		m_TarPos.Clear();
		m_BeginPos.Clear();
		m_BIfBegin = FALSE;
		return eNormal;
	}

	INT32 CPlayerMoveAI::Begin(PlayerAIData& sPlayerAIData){
		m_BIfBegin = FALSE;
		m_BeginTime = gRobotMgr->GetTickCount();
		m_pPlayer = sPlayerAIData.pPlayer;
		m_TarPos = sPlayerAIData.sTarPos;
		m_BeginPos = m_pPlayer->GetCurPos();
		if (NULL == m_pPlayer){
			return eEC_BeginAIFailed;
		}

		//if (m_pPlayer->GetState() != eOAS_Free){
		//	return eEC_BeginAIFailed;
		//}

		m_pPlayer->GetClient()->AskMovePos(m_TarPos);
		m_RebotAIState = eRebotAIState_Running;
		m_BIfBegin = true;
		return eNormal;
	}

}

