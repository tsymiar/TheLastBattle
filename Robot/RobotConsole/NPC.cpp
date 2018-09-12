#include "StdAfx.h"
#include "NPC.h"

namespace Rebot
{
	CNPC::CNPC(void)
	{
	}

	CNPC::~CNPC(void)
	{
	}

	INT32 CNPC::Clear(){
		return eNormal;
	}

	INT32 CNPC::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
		return eNormal;
	}

	SSkillRunTimeData* CNPC::GetSkill(UINT32 un32SkillID){
		return NULL;
	}

}




