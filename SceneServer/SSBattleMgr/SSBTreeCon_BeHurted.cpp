#include "SSBTreeCon_BeHurted.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_BeHurted::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		INT32	hurtValue = 0;
		map<CSSGameUnit*,INT32> hurtMap;
		pHero->GetHurtMeList(m_parameters[0]*1000,&hurtMap);
		for(auto iter = hurtMap.begin(); iter!= hurtMap.end();iter++){
			if(m_parameters[1] == 1 || (m_parameters[1] == 2 && iter->first->IfSolider()) || (m_parameters[1] == 3 && iter->first->IfHero()) || (m_parameters[1] == 4 && iter->first->IfNPC_Building()) ){
				hurtValue+= iter->second;
			}
		}
		return hurtValue >= (float)m_parameters[2]*100/pHero->GetFPData(eEffectCate_MaxHP);
	}

};