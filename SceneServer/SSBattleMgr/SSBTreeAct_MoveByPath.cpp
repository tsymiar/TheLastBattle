#include "SSBTreeAct_MoveByPath.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSCfgMgr.h"

namespace SceneServer{
	BOOLEAN	CSSBTreeAct_MoveByPath::Action(CSSAI_HeroRobot* pAI){
		pAI->SetLastActionTime(GetUTCMiliSecond());
		//ELOG(LOG_DBBUG,"Action: Start Move By Path");
		CSSHero* pHero = pAI->GetHero();

		if(m_parameters[0] == 1){
			//寻找一个进攻的路点
			INT32 nearestPathIndex = 0;
			INT32 nearestNodeIndex = 0;
			//如果在家，则随机选择一条路
			if(pHero->GetCurPos().CanWatch(500,pHero->GetHeroBornPos())){
				nearestNodeIndex = 0;
				nearestPathIndex = pAI->Random(0,pAI->GetPathVec().size()-1);
			}
			//否则，选择最近的一条路
			else{
				FindNearestPathNode(pAI,nearestPathIndex,nearestNodeIndex);
			}
			//向最近点的下一个点移动
			if(nearestNodeIndex < pAI->GetPathVec()[nearestPathIndex].size() - 1) nearestNodeIndex++;
			//移动
			return pAI->MoveByPath(nearestPathIndex,nearestNodeIndex,TRUE);
		}
		else if(m_parameters[0] == 2){
			//寻找一个回家的路点
			INT32 nearestPathIndex = 0;
			INT32 nearestNodeIndex = 0;
			FindNearestPathNode(pAI,nearestPathIndex,nearestNodeIndex);
			//向最近点的前一个点移动
			if(nearestNodeIndex > 0) nearestNodeIndex--;
			//移动
			return pAI->MoveByPath(nearestPathIndex,nearestNodeIndex,FALSE);
		}
		
		return FALSE;
	}

	void	CSSBTreeAct_MoveByPath::FindNearestPathNode(CSSAI_HeroRobot* pAI,INT32 &nearestPathIndex,INT32 &nearestNodeIndex){
		CSSHero* pHero = pAI->GetHero();
		CVector3D curPos = pHero->GetCurPos();
		vector<vector<CVector3D>> pathVec = pAI->GetPathVec();
		float distSqr = 0;
		for(int pathIndex = 0; pathIndex < pathVec.size(); pathIndex++){
			vector<CVector3D>* pPath = &pathVec[pathIndex];
			for(int nodeIndex = 0; nodeIndex < pPath->size(); nodeIndex++){
				CVector3D node = pPath->at(nodeIndex);
				float dist = curPos.GetWatchDistSqr(node);
				if(distSqr == 0 || dist < distSqr){
					distSqr = dist;
					nearestPathIndex = pathIndex;
					nearestNodeIndex = nodeIndex;
				}
			}
		}
	}
};