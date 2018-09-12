#include "SSAI_Soldier.h"
#include "SSNPC.h"
#include "SSBattle.h"
#include "SSMoveTool.h"

namespace SceneServer{

CSSAI_Soldier::CSSAI_Soldier(CSSGameUnit* pGU):CSSAI(pGU), m_cStartAttackPos(0,0,0)
, m_ifNeedRestartMove(TRUE)
, m_eOccupyState(eOccupyState_Walk)
, m_n32CurNodeIdx(0)
, m_ifAttackTarDead(FALSE)
{
	if (m_pcMasterGU){
		m_pcSoldierGU = m_pcMasterGU->TryCastToNPC();
	}
	m_bIfFliter = FALSE;
}

void	CSSAI_Soldier::SetOccupyPath(std::vector<CVector3D> & pathNode)//(const CVector3D *cpcPathNode, UINT32 un32NodeNum){
{	//长度不能超过最大节点数量
//	m_n32NodeNum = un32NodeNum;
	//if (pathNode.size() > (UINT32)c_n32MaxOrderPathNode){
		//m_n32NodeNum = (UINT32)c_n32MaxOrderPathNode;
//	}
	//将数据拷贝保存下来
	m_n32NodeNum = pathNode.size();
	for(int i=0; i< m_n32NodeNum; i++)
	{
		m_acOrderPath[i] = pathNode[i];

		if (i > (UINT32)c_n32MaxOrderPathNode){
			m_n32NodeNum = (UINT32)c_n32MaxOrderPathNode;
			break;
		}
	}

	//memcpy(m_acOrderPath, cpcPathNode, sizeof(CVector3D) * m_n32NodeNum);
	//初始化参数
	m_eOccupyState = eOccupyState_Walk;
	//设置第一个目标节点为最近的节点
	float nearDist = 0;
	for(int i=0;i<m_n32NodeNum;i++){
		if(i > 3) break;
		float dist = m_pcSoldierGU->GetCurPos().GetWatchDistSqr(m_acOrderPath[i]);
		if(nearDist == 0 || dist < nearDist){
			nearDist = dist;
			SetNowPathIndex(i);
		}
	}
	
	
	//开始行走
	if(m_pcSoldierGU->GetCurBattle()->AskStartMoveToTar(m_pcSoldierGU,&m_acOrderPath[m_n32CurNodeIdx],TRUE,FALSE)){
		m_ifMoving = TRUE;
	}
}

void	CSSAI_Soldier::SetAttackTarget(CSSGameUnit* pTarget){
	SetAttackGU(pTarget);
	m_eOccupyState = eOccupyState_Attack;
	m_tLastCheckMoveTarTime = 0;
	m_pcSoldierGU->SetCurAtkGUID(GetAttackGU()->GetObjGUID());
	TryFree();
}

INT32	CSSAI_Soldier::HeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	//检查是否是不能工作状态
	if(TRUE == IfPassitiveState()){
		if(IfMoving())
			m_pcSoldierGU->GetCurBattle()->AskStopMoveObjectTar(m_pcSoldierGU);
		return eEC_AbsentOrderPriority;
	}
	//检查是否已经到了目标点附近。如果是，将其设置为下一个点。如果是在攻击中或者返回中的话，远距离就可以了，避免跑回头路
	if (m_pcSoldierGU->GetCurPos().CanWatch((FLOAT)(eOccupyState_Walk == m_eOccupyState?c_n32PathPointWatchDist:1000), &m_acOrderPath[m_n32CurNodeIdx])){
	//测试新算法，越过那条X的界限，就算过了这个点了。
	//if(  (m_acOrderPath[m_n32NodeNum-1].m_fX - m_acOrderPath[0].m_fX >= 0 && m_pcSoldierGU->GetCurPos().m_fX > m_acOrderPath[m_n32CurNodeIdx].m_fX - 200 )
	//	|| (m_acOrderPath[m_n32NodeNum-1].m_fX - m_acOrderPath[0].m_fX < 0 && m_pcSoldierGU->GetCurPos().m_fX < m_acOrderPath[m_n32CurNodeIdx].m_fX + 200 )  ){
		SetNowPathIndex(m_n32CurNodeIdx+1);
		//如果是行走状态，则转换行走目标
		if (eOccupyState_Walk == m_eOccupyState){
			MoveToTar(&m_acOrderPath[m_n32CurNodeIdx],TRUE, 0);
			m_ifNeedRestartMove = FALSE;
		}
	}

	//走路状态
	if (eOccupyState_Walk == m_eOccupyState){
		CSSGameUnit *pcTarGU = m_pcSoldierGU->LookForEnemy(NULL,0,FALSE);
		//如果发现敌人，则进入攻击状态
		if (NULL != pcTarGU){
			SetAttackTarget(pcTarGU);
			m_cStartAttackPos = m_pcSoldierGU->GetCurPos();
		}
		else{
			//如果停下来了，重新启动移动
			if(FALSE == m_ifMoving || m_ifNeedRestartMove){
				MoveToTar(&m_acOrderPath[m_n32CurNodeIdx],TRUE, tUTCMilsec);
				m_ifNeedRestartMove = FALSE;
			}
		}
	}
	//攻击状态
	if (eOccupyState_Attack == m_eOccupyState){
		do{
			TIME_TICK tTickSpan = tUTCMilsec - m_tLastWatchEnemyMilsec;
			//每1秒检查一次
			if(tTickSpan > 1000){
				//是否超过最大追击距离，如果是，返回
				if (FALSE == m_pcSoldierGU->GetCurPos().CanWatch((FLOAT)m_pcSoldierGU->GetPursueDist(), m_cStartAttackPos)){
					m_eOccupyState = eOccupyState_Back;
					//计算返回点。返回点为路径上直线最近的点
					m_cBackToLinePos = &m_acOrderPath[m_n32CurNodeIdx];
					float minDistSqrt = -1;
					float tempMinDistSqrt = -1;
					CVector3D tempBackPoint;
					for(int i=m_n32CurNodeIdx-1;i<m_n32CurNodeIdx+2;i++){
						if(i < 0) continue;
						if(i >= m_n32NodeNum-1) break;
						IfPointNearPath(m_pcSoldierGU->GetCurPos(),&m_acOrderPath[i],&m_acOrderPath[i+1],tempMinDistSqrt,tempBackPoint);
						//如果这个点比前面找到的点都近，将其设置为目标点
						if(minDistSqrt == -1 || (tempMinDistSqrt < minDistSqrt && tempMinDistSqrt >= 0)){
							minDistSqrt = tempMinDistSqrt;
							m_cBackToLinePos = tempBackPoint;
							SetNowPathIndex(i+1);
						}
					}
					//向返回点移动
					MoveToTar(m_cBackToLinePos,TRUE,tUTCMilsec);
					break;
				}
				//检查是否有高优先级目标
				CSSGameUnit *pcEnemy = m_pcSoldierGU->LookForEnemy(GetAttackGU(),0,FALSE);
				if ( NULL != pcEnemy &&  GetAttackGU() != pcEnemy){
					SetAttackTarget(pcEnemy);
				}
				//检查是否有攻击距离内目标
				else if(m_eAttackState == eAttackState_Pursue){
					pcEnemy = m_pcSoldierGU->LookForEnemy(NULL, m_pcSoldierGU->GetFPData(eEffectCAte_AttackDist) * m_pcSoldierGU->GetFPData(eEffectCAte_AttackDist),FALSE);
					if(NULL != pcEnemy && m_pcSoldierGU->IfInReleaseSkillRange(pcEnemy, m_pcSoldierGU->GetNormalAttackSkill_NEW()->cpsCfg)){
						SetAttackTarget(pcEnemy);
					}
				}
				m_tLastWatchEnemyMilsec = tUTCMilsec;
			}

			//执行攻击行为心跳
			INT32 rst = AttackHeartBeat(tUTCMilsec,tTickSpan);
			if(eNormal != rst){
				if(GetAttackGU() == NULL){
					m_eOccupyState = eOccupyState_Walk;
					//MoveToTar(&m_acOrderPath[m_n32CurNodeIdx],TRUE, 0);
				}
				if(eEC_TargetIsDead == rst){
					m_ifAttackTarDead = TRUE;
					TryFree();
				}
			}
		}
		while(FALSE);
		
		
	}
	
	//返回状态
	if(eOccupyState_Back == m_eOccupyState){
		//检查距离出发点的距离
		if (m_pcSoldierGU->GetCurPos().CanWatch((FLOAT)(500), m_cBackToLinePos)){
			m_eOccupyState = eOccupyState_Walk;
			m_ifNeedRestartMove = TRUE;
			MoveToTar(&m_acOrderPath[m_n32CurNodeIdx],TRUE,tUTCMilsec);
		}
		else{
			//如果停下来了，重新启动移动
			if(FALSE == m_ifMoving){
				if (m_pcSoldierGU->GetCurPos().CanWatch((FLOAT)(800), m_cBackToLinePos)){
					m_eOccupyState = eOccupyState_Walk;
					m_ifNeedRestartMove = TRUE;
					MoveToTar(&m_acOrderPath[m_n32CurNodeIdx],TRUE,tUTCMilsec);
				}
				else{
					MoveToTar(m_cBackToLinePos,TRUE,tUTCMilsec);
				}
			}
		}
	}

	return eNormal;
}

bool	CSSAI_Soldier::IfPointNearPath(CVector3D cPoint, CVector3D cPath1, CVector3D cPath2, float &distSqrt, CVector3D &nearesatPoint){
	float x = cPoint.m_fX;float y = cPoint.m_fZ;
	float x1 = cPath1.m_fX;float y1 = cPath1.m_fZ;
	float x2 = cPath2.m_fX;float y2 = cPath2.m_fZ;
	//如果第一个点大于90度
	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0) {
		distSqrt = (x - x1) * (x - x1) + (y - y1) * (y - y1);
		nearesatPoint = cPath1;
		return FALSE;
	}
	//如果第二个点大于90度
	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2){
		distSqrt = (x - x2) * (x - x2) + (y - y2) * (y - y2);
		nearesatPoint = cPath2;
		return FALSE;
	}
	//两个角都小于90度，点的垂点在线段上
	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;
	distSqrt = (x - px) * (x - px) + (py - y1) * (py - y1);
	nearesatPoint.m_fX = px;
	nearesatPoint.m_fZ = py;
	return TRUE;
}

}