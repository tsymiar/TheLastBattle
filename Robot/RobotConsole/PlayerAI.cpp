#include "StdAfx.h"
#include "PlayerAI.h"
#include "Player.h"
#include "PlayerMoveAI.h"
#include "PlayerAtkAI.h"
#include "AbsorbAI.h"
#include "PlayerOccupyAI.h"

namespace ReBot
{
CPlayerAI::CPlayerAI(void):
		m_pPlayer(NULL)
		,m_RebotAIState(eRebotAIState_Cleared)
		, m_BeginTime(0){
}


CPlayerAI::~CPlayerAI(void)
{
}

INT32 CPlayerAI::Begin(PlayerAIData& sPlayerAIData){
	m_pPlayer = sPlayerAIData.pPlayer;
	return eNormal;
}

CPlayerMoveAI* CPlayerAI::GetMoveAI(){
	return new CPlayerMoveAI;
}

void CPlayerAI::FreeMoveAI(CPlayerMoveAI* aAI){
	delete aAI;
}

void CPlayerAI::FreeAtkAI(CPlayerAtkAI* aAI){
	delete aAI;
}

CPlayerAtkAI* CPlayerAI::GetAtkAI(){
	CPlayerAtkAI* pAI = new CPlayerAtkAI();
	pAI->Clear();
	return pAI;
}

AbsorbAI* CPlayerAI::GetAbosorbAI(){
	AbsorbAI* pAI = new AbsorbAI();
	pAI->Clear();
	return pAI;
}

void CPlayerAI::FreeAbosorbAI(AbsorbAI* pAI){
	delete pAI;
}

CPlayerOccupyAI* CPlayerAI::GetOccupyAI(){
	return new CPlayerOccupyAI;
}

void CPlayerAI::FreeOccupyAI(CPlayerOccupyAI* pAI){
	delete pAI;
}

}