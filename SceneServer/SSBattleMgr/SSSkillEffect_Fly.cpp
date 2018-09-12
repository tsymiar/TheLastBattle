#include "SSSkillEffect_Fly.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSEffectMgr.h"

namespace SceneServer{

CSSSkillEffect_Fly::CSSSkillEffect_Fly(){
	m_SkillEffectType = eSkillEffectType_Emit;
}

CSSSkillEffect_Fly::~CSSSkillEffect_Fly(){
}

INT32 CSSSkillEffect_Fly::Begin(){
	if (NULL == m_pcMasterGO){
		return eEC_EffectEnd;
	}

	m_lEmitEntityArr.Clear();
	m_lEmitHitTargetArr.Clear();

	const SSkillModelEmitCfg* pCfg = GetModelConfig();

	//设置基础信息（飞行道具数量，间隔角度）
	INT32 n32ProjNum = pCfg->n32ProjectileNum;
	INT32 n32AngleInter = pCfg->n32ProjectileAngle;
	INT32 n32LeftAngle = -n32AngleInter * (n32ProjNum-1) / 2;

	vector<CSSGameUnit*> vecAreaObj; 
	GO2SightDiffMap sGO2SightDiffMaptvEnemyMap;
	//如果类型为自动寻敌，那么就需要从周围的列表中查找一个合法的目标
	if(pCfg->eEmitType == eSkillModelEmitType_AutoFind){
		//准备周围敌人列表
		GetBattle()->FindAroundGU(m_pcMasterGO->GetCurPos(), pCfg->n32FlyPar2 + 500,vecAreaObj);
		for(vector<CSSGameUnit*>::iterator iter = vecAreaObj.begin(); iter != vecAreaObj.end();){
			CSSGameUnit* pcTempGU = *iter;
			float tempDist = (pcTempGU->GetCurPos() - m_pcMasterGO->GetCurPos()).length();
			if (pcTempGU->IsDead() || !CheckHitTargetType(pcTempGU) || !CheckHitTargetCamp(pcTempGU) || tempDist > pCfg->n32FlyPar2){
				iter = vecAreaObj.erase(iter);
			}
			else{
				iter++;
			}
		}
	}
	//根据飞行物体数量，循环释放
	for(int i = 0; i < n32ProjNum; i++){
		//获取每一个飞行物体的出发角度
		CVector3D cCurDir = m_pcMasterGO->GetGOActionStateInfo().cDir;
		INT32 n32Degree = n32LeftAngle + n32AngleInter * i;
		cCurDir.RotateXZBy((double)n32Degree);

		SSEmitEntity rsSSE;
		rsSSE.Clear();

		//如果类型是自动寻敌，则在所有可用目标中随机找一个
		if(pCfg->eEmitType == eSkillModelEmitType_AutoFind){
			//随机选择一个目标
			if(vecAreaObj.empty()){
				break;
			}
			CSSGameUnit* pTempGU = vecAreaObj.at(rand()%vecAreaObj.size());
			m_pcTarGU = pTempGU;
			cCurDir = (pTempGU->GetCurPos() - m_pcTarGU->GetCurPos()).unit();
			vector<CSSGameUnit*>::iterator iter = find(vecAreaObj.begin(),vecAreaObj.end(),pTempGU);
			vecAreaObj.erase(iter);
		}

		if (NULL != m_pcTarGU){
			rsSSE.sTargetGUID = m_pcTarGU->GetObjGUID();
		}

		//如果是回旋镖或者直接飞行技能，且上一层传过来的技能开始对象不为空，则将其设为飞行起点。否则，技能所有者为飞行起点。
		rsSSE.pcTargetGU = m_pcTarGU;
		CSSGameUnit* pStartUnit = m_pcMasterGO;
		if(pCfg->eEmitType == eSkillModelEmitType_Direct || pCfg->eEmitType == eSkillModelEmitType_Boomerang){
			if(NULL != m_pcStartSkillGU){
				pStartUnit = m_pcStartSkillGU;
			}
		}

		rsSSE.cCurPos = pStartUnit->GetEmitPos();
		rsSSE.cCurDir = cCurDir;
		rsSSE.tLaunchUTCMilsec = GetUTCMiliSecond();
		rsSSE.tLastEffectTime = rsSSE.tLaunchUTCMilsec;
		rsSSE.bIfEnded = false;
		rsSSE.bIfTurnBack = false;
		rsSSE.n32ProjectID = GetBattle()->GetUniqueEffectID();

		//如果是从目标身上开始的弹跳技能，则需要在这里寻找一个弹跳目标
		bool bIfEmitBuilded = TRUE;
		if((pCfg->eEmitType == eSkillModelEmitType_Bounce || pCfg->eEmitType == eSkillModelEmitType_Bounce_NoRepeat) && pCfg->n32FlyPar3 == 2){

			GetBattle()->GetEffectMgr()->AddEffectsFromCfg((SSNextSkillEffectCfg*)m_pCfg->asSkillModelList
				, m_pcMasterGO, m_pcTarGU, m_pcTarGU->GetCurPos(), m_pcMasterGO->GetCurDir(), m_pSkillRuntimeData, GetUTCMiliSecond());

			bool bIfCanRepeat = pCfg->eEmitType == eSkillModelEmitType_Bounce? TRUE : FALSE;
			rsSSE.cCurPos = m_pcTarGU->GetEmitPos();
			rsSSE.cCurDir = m_pcTarGU->GetCurDir();
			//寻找弹跳目标
			CSSGameUnit* pNextTarget = FindNextBounceTarget(&rsSSE,m_pcTarGU, bIfCanRepeat);
			if(pNextTarget != NULL){
				rsSSE.pcTargetGU = pNextTarget;
				rsSSE.sTargetGUID = pNextTarget->GetObjGUID();
				rsSSE.lHitTargetsArr.AddElement(m_pcTarGU->GetObjGUID());
				pStartUnit = m_pcTarGU;
			}
			else{
				bIfEmitBuilded = FALSE;
			}
		}
		if(bIfEmitBuilded){
			m_lEmitEntityArr.AddElement(rsSSE);
			NotifyNewEmitObjectToGC(pStartUnit, &rsSSE);
		}
	}
	return eNormal;
}

INT32 CSSSkillEffect_Fly::Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if(NULL == m_pcMasterGO || m_pcMasterGO->GetExpire()){
		return eEC_EffectEnd;
	}

	//遍历所有已命中的飞行物体，调用下一层的效果，然后检查飞行物体是否需要销毁。
	const INT32 n32MaxHitTargetSize = m_lEmitHitTargetArr.GetMaxSize();
	for (INT32 i = 0; i < n32MaxHitTargetSize; ++i){
		if (TRUE == m_lEmitHitTargetArr.m_Arr[i].isVaild){
			SSEmitHitTargetInfo& entity = m_lEmitHitTargetArr.m_Arr[i].ele;
			if (NULL == entity.pTarget){
				m_lEmitHitTargetArr.m_Arr[i].isVaild = FALSE;
				--m_lEmitHitTargetArr.m_CurSize;
				continue;
			}

			if(FALSE == entity.pTarget->GetExpire() && entity.pTarget->IsDead() == FALSE){
				GetBattle()->GetEffectMgr()->AddEffectsFromCfg((SSNextSkillEffectCfg*)m_pCfg->asSkillModelList
					, m_pcMasterGO, entity.pTarget, entity.cPos, entity.cDir, m_pSkillRuntimeData, GetUTCMiliSecond());
				OnSkillHitTarget(entity.pTarget);
			}

			m_lEmitHitTargetArr.m_Arr[i].isVaild = FALSE;
			--m_lEmitHitTargetArr.m_CurSize;
		}
	}

	//检查所有还在飞行的飞行道具，运行心跳。如果其结束了，发送销毁消息
	const INT32 n32MaxSize = m_lEmitEntityArr.GetMaxSize();
	for (INT32 i = 0; i < n32MaxSize; ++i){
		if (TRUE == m_lEmitEntityArr.m_Arr[i].isVaild){
			SSEmitEntity& entity = m_lEmitEntityArr.m_Arr[i].ele;
			OnHeartBeat_CheckSimpleSkillEntityImpact(&entity, tUTCMilsec, tTickSpan);
			if(TRUE == entity.bIfEnded){
				const SSkillModelEmitCfg* pCfg = GetModelConfig();
				switch(pCfg->eEmitType){
				case eSkillModelEmitType_Direct:
				case eSkillModelEmitType_Boomerang:
				case eSkillModelEmitType_Area:
				case eSkillModelEmitType_AutoFind:
					NotifyDestoryEmitObjectToGC(&entity);
					break;
				}

				entity.Clear();
				m_lEmitEntityArr.m_Arr[i].isVaild = FALSE;
				--m_lEmitEntityArr.m_CurSize;
			}
		}
	}

	if(0 == m_lEmitEntityArr.GetCurSize() && 0 == m_lEmitHitTargetArr.GetCurSize()){
		return eEC_EffectEnd;
	}

	return eNormal;
}

INT32	CSSSkillEffect_Fly::OnHeartBeat_CheckSimpleSkillEntityImpact(SSEmitEntity *rsSSE, TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if(tUTCMilsec - rsSSE->tLaunchUTCMilsec < 50){
		return eNormal;
	}
	if(FALSE == CheckBattle()){
		return eEC_ConstInvalid;
	}
	const SSkillModelEmitCfg* pCfg = GetModelConfig();
	switch(pCfg->eEmitType)
	{
		//自动跟踪类的飞行效果，不断检查和目标的距离。距离比对方的半径小，就算击中。
	case eSkillModelEmitType_Follow:
	case eSkillModelEmitType_AutoFind:
		{
			if (rsSSE->sTargetGUID > 0 && NULL != rsSSE->pcTargetGU && FALSE == rsSSE->pcTargetGU->GetExpire() && rsSSE->pcTargetGU->IsDead() == FALSE){
				if (rsSSE->pcTargetGU->GetObjGUID() != rsSSE->sTargetGUID || rsSSE->pcTargetGU->IsDead()){
					rsSSE->bIfEnded = true;
					return eEC_InvalidSkillTarget;
				}
				CVector3D cTarPos = rsSSE->pcTargetGU->GetOnHitPos();
				CVector3D cSelfPos = rsSSE->cCurPos;
				CVector3D cDir = cTarPos - cSelfPos;
				float fDist = cDir.length() - rsSSE->pcTargetGU->GetObjCollideRadius();
				float fCanMoveDist = GetModelConfig()->n32ProjFlySpeed * (FLOAT)tTickSpan / 1000;
				if (fCanMoveDist >= fDist){
					SSEmitHitTargetInfo temp = {rsSSE->pcTargetGU, rsSSE->pcTargetGU->GetCurPos(), cDir};
					m_lEmitHitTargetArr.AddElement(temp);
					rsSSE->bIfEnded = true;
					return eEC_ConstInvalid;
				}
				cDir.unit();
				rsSSE->cCurPos = cSelfPos + cDir * fCanMoveDist;
			}
			else{
				rsSSE->bIfEnded = true;
			}
		}
		break;
		//直接飞行类的效果，需要不断检测是否发生了碰撞。一旦和有效目标碰撞，则算命中，然后根据穿透属性判断是否销毁。
	case eSkillModelEmitType_Direct:
		{
			//检测持续时间
			if(rsSSE->tLaunchUTCMilsec + pCfg->n32ProjectileLifeTime < tUTCMilsec){
				rsSSE->bIfEnded = true;
				return eEC_ConstInvalid;
			}

			CVector3D cSelfPos = rsSSE->cCurPos;
			CVector3D cDir = rsSSE->cCurDir;
			float fCanMoveDist = GetModelConfig()->n32ProjFlySpeed * (FLOAT)tTickSpan / 1000;
			vector<CSSGameUnit*> targetList;
			CheckSimpleSkillEntityImpact(rsSSE,fCanMoveDist,&targetList,pCfg->bIsPenetrate, FALSE);
			
			for(vector<CSSGameUnit*>::iterator iter = targetList.begin();iter != targetList.end(); iter++){
				if(CheckHitTargetType(*iter)){
					SSEmitHitTargetInfo temp = {*iter, (*iter)->GetCurPos(), cDir};
					m_lEmitHitTargetArr.AddElement(temp);
				}
			}
			//命中目标且不是穿透，则飞行道具中止。
			if(FALSE == targetList.empty() && FALSE == pCfg->bIsPenetrate){
				rsSSE->bIfEnded = true;
				return eEC_ConstInvalid;
			}
			
			cDir.unit();
			rsSSE->cCurPos = cSelfPos + cDir * fCanMoveDist;
		}
		break;
	case eSkillModelEmitType_Boomerang:
		{
			if(FALSE == rsSSE->bIfTurnBack){//正在向前飞阶段
				//检测持续时间
				if(rsSSE->tLaunchUTCMilsec + pCfg->n32ProjectileLifeTime < tUTCMilsec){
					NotifyTurnEmitObjectToGC(rsSSE);
					rsSSE->bIfTurnBack = true;
					rsSSE->pcTargetGU = m_pcMasterGO;
					rsSSE->sTargetGUID = m_pcMasterGO->GetObjGUID();
					rsSSE->cCurDir = (rsSSE->pcTargetGU->GetCurPos() - rsSSE->cCurPos).unit();
					rsSSE->Clear();
					return eNormal;
				}

				CVector3D cSelfPos = rsSSE->cCurPos;
				CVector3D cDir = rsSSE->cCurDir;
				float fCanMoveDist = GetModelConfig()->n32ProjFlySpeed * (FLOAT)tTickSpan / 1000;
				vector<CSSGameUnit*> targetList;
				CheckSimpleSkillEntityImpact(rsSSE,fCanMoveDist,&targetList,pCfg->bIsPenetrate, FALSE);

				for(vector<CSSGameUnit*>::iterator iter = targetList.begin();iter != targetList.end(); iter++){
					if(CheckHitTargetType(*iter)){
						SSEmitHitTargetInfo temp = {*iter, (*iter)->GetCurPos(), cDir};
						m_lEmitHitTargetArr.AddElement(temp);
					}
				}
				//命中目标且不是穿透，则飞行道具中止。
				if(FALSE == targetList.empty() && FALSE == pCfg->bIsPenetrate){
					rsSSE->bIfEnded = true;
					return eEC_ConstInvalid;
				}

				cDir.unit();
				rsSSE->cCurPos = cSelfPos + cDir * fCanMoveDist;
			}
			else{//进入回旋阶段
				if (rsSSE->sTargetGUID > 0 && NULL != rsSSE->pcTargetGU && FALSE == rsSSE->pcTargetGU->GetExpire() && rsSSE->pcTargetGU->IsDead() == FALSE){
					if (rsSSE->pcTargetGU->GetObjGUID() != rsSSE->sTargetGUID || rsSSE->pcTargetGU->IsDead()){
						rsSSE->bIfEnded = true;
						return eEC_InvalidSkillTarget;
					}
					//CVector3D cTarPos = rsSSE->pcTargetGU->GetGOActionStateInfo().cPos;
					CVector3D cTarPos = rsSSE->pcTargetGU->GetOnHitPos();
					CVector3D cSelfPos = rsSSE->cCurPos;
					CVector3D cDir = cTarPos - cSelfPos;
					float fDist = cDir.length() - rsSSE->pcTargetGU->GetObjCollideRadius();
					float fCanMoveDist = GetModelConfig()->n32ProjFlySpeed * (FLOAT)tTickSpan / 1000;

					vector<CSSGameUnit*> targetList;
					CheckSimpleSkillEntityImpact(rsSSE,fCanMoveDist,&targetList,pCfg->bIsPenetrate, FALSE);
					for(vector<CSSGameUnit*>::iterator iter = targetList.begin();iter != targetList.end(); iter++){
						if(CheckHitTargetType(*iter)){
							SSEmitHitTargetInfo temp = {*iter, (*iter)->GetCurPos(), cDir};
							m_lEmitHitTargetArr.AddElement(temp);
						}
					}
					//命中目标且不是穿透，则飞行道具中止。
					if(FALSE == targetList.empty() && FALSE == pCfg->bIsPenetrate){
						rsSSE->bIfEnded = true;
						return eEC_ConstInvalid;
					}
					//回到手上，中止
					if (fCanMoveDist >= fDist){
						rsSSE->bIfEnded = true;
						return eEC_ConstInvalid;
					}
					cDir.unit();
					rsSSE->cCurPos = cSelfPos + cDir * fCanMoveDist;
				}
				else{
					rsSSE->bIfEnded = true;
				}
			}
		}
		break;
		//弹跳类技能，当命中的时候，还有根据弹跳次数，寻找下一次弹跳的目标
	case eSkillModelEmitType_Bounce:
	case eSkillModelEmitType_Bounce_NoRepeat:
		{
			bool bIfCanRepeat = pCfg->eEmitType == eSkillModelEmitType_Bounce? TRUE : FALSE;
			if (rsSSE->sTargetGUID > 0 && NULL != rsSSE->pcTargetGU && FALSE == rsSSE->pcTargetGU->GetExpire() && rsSSE->pcTargetGU->IsDead() == FALSE){
				if (rsSSE->pcTargetGU->GetObjGUID() != rsSSE->sTargetGUID || rsSSE->pcTargetGU->IsDead()){
					rsSSE->bIfEnded = true;
					return eEC_InvalidSkillTarget;
				}
				//CVector3D cTarPos = rsSSE->pcTargetGU->GetGOActionStateInfo().cPos;
				CVector3D cTarPos = rsSSE->pcTargetGU->GetOnHitPos();
				CVector3D cSelfPos = rsSSE->cCurPos;
				CVector3D cDir = cTarPos - cSelfPos;
				float fDist = cDir.length() - rsSSE->pcTargetGU->GetObjCollideRadius();
				float fCanMoveDist = GetModelConfig()->n32ProjFlySpeed * (FLOAT)tTickSpan / 1000;
				if (fCanMoveDist >= fDist){//命中目标
					//检查弹跳次数
					if(rsSSE->lHitTargetsArr.GetCurSize() < pCfg->n32FlyPar1)
					{
						//未到次数，弹跳
						CSSGameUnit* pNextTarget = FindNextBounceTarget(rsSSE,rsSSE->pcTargetGU, bIfCanRepeat);
						if(NULL != pNextTarget){
							SSEmitEntity newSSE;
							newSSE.pcTargetGU = pNextTarget;
							newSSE.sTargetGUID = pNextTarget->GetObjGUID();
							//newSSE.cCurPos = rsSSE->pcTargetGU->GetCurPos();
							newSSE.cCurPos = rsSSE->pcTargetGU->GetEmitPos();
							newSSE.cCurDir = rsSSE->pcTargetGU->GetCurDir();
							newSSE.tLaunchUTCMilsec = tUTCMilsec;
							newSSE.bIfEnded = false;
							newSSE.n32ProjectID = GetBattle()->GetUniqueEffectID();
							newSSE.lHitTargetsArr = rsSSE->lHitTargetsArr;
							newSSE.lHitTargetsArr.AddElement(rsSSE->pcTargetGU->GetObjGUID());
							m_lEmitEntityArr.AddElement(newSSE);
							NotifyNewEmitObjectToGC(rsSSE->pcTargetGU, &newSSE);
						}
					}
					//调用伤害等其他模块
					SSEmitHitTargetInfo temp = {rsSSE->pcTargetGU, rsSSE->pcTargetGU->GetCurPos(), cDir};
					m_lEmitHitTargetArr.AddElement(temp);
					rsSSE->bIfEnded = true;

					return eEC_ConstInvalid;
				}
				cDir.unit();
				rsSSE->cCurPos = cSelfPos + cDir * fCanMoveDist;
			}
			else{
				rsSSE->bIfEnded = true;
			}
		}
		break;
		//范围内飞行技能，不断的根据间隔判断范围效果，范围内的所有有效目标计算为命中。范围飞行技能默认是穿透的。
	case eSkillModelEmitType_Area:
		{
			//检测持续时间
			if(rsSSE->tLaunchUTCMilsec + pCfg->n32ProjectileLifeTime < tUTCMilsec){
				rsSSE->bIfEnded = true;
				return eEC_ConstInvalid;
			}
			CVector3D cSelfPos = rsSSE->cCurPos;
			CVector3D cDir = rsSSE->cCurDir;
			float fCanMoveDist = GetModelConfig()->n32ProjFlySpeed * (FLOAT)tTickSpan / 1000;
			if(rsSSE->tLastEffectTime <= tUTCMilsec){
				rsSSE->tLastEffectTime += pCfg->n32FlyPar1;
				vector<CSSGameUnit*> targetList;
				CheckSimpleSkillEntityImpact(rsSSE,fCanMoveDist,&targetList,TRUE,TRUE);

				for(vector<CSSGameUnit*>::iterator iter = targetList.begin();iter != targetList.end(); iter++){
					if(CheckHitTargetType(*iter)){
						SSEmitHitTargetInfo temp = {*iter, (*iter)->GetCurPos(), cDir};
						m_lEmitHitTargetArr.AddElement(temp);
					}
				}
			}

			cDir.unit();
			rsSSE->cCurPos = cSelfPos + cDir * fCanMoveDist;
		}
		break;
	default:
		break;
	//if a skill entity to a specified target.
	}
	return eNormal;
}

const SSkillModelEmitCfg* CSSSkillEffect_Fly::GetModelConfig(){
	return (const SSkillModelEmitCfg*)m_pCfg;
}

//检查飞行道具和哪些物体发生了碰撞
INT32	CSSSkillEffect_Fly::CheckSimpleSkillEntityImpact(SSEmitEntity* rsSSE, float fDist, vector<CSSGameUnit*>* pTargetList, bool bIsPenetrate, bool bIfCanRepeat){
	const SSkillModelEmitCfg* pCfg = GetModelConfig();
	//获取周边范围内的所有对象
	vector<CSSGameUnit*> vecAreaObj; 
	GetBattle()->FindAroundGU(rsSSE->cCurPos,(FLOAT)pCfg->n32ProjectileCollideRadius + 500,vecAreaObj);

	for (UINT32 i = 0; i < vecAreaObj.size(); i++){
		if(m_pcMasterGO == vecAreaObj[i]){
			continue;
		}
		CSSGameUnit *pcTempGU = vecAreaObj[i];
		if (CheckHitTargetType(pcTempGU) && CheckHitTargetCamp(pcTempGU) &&
			CFunction::IfImpact(rsSSE->cCurPos, (FLOAT)pCfg->n32ProjectileCollideRadius, pcTempGU->GetGOActionStateInfo().cPos, (FLOAT)pcTempGU->GetObjCollideRadius())){
				if(pcTempGU->GetExpire() || pcTempGU->IsDead()) continue;
			bool bCaculateEffect = true;
			//需要检查是否重复。如果不能重复，则在已命中列表中的对象不可加入
			if (FALSE == bIfCanRepeat){
				bool bExist = rsSSE->lHitTargetsArr.IsHvElement(pcTempGU->GetObjGUID());

				if (FALSE == bExist){
					rsSSE->lHitTargetsArr.AddElement(pcTempGU->GetObjGUID());
				}
				else{
					bCaculateEffect = false;
				}
			}
			if (bCaculateEffect){
				pTargetList->push_back(pcTempGU);
				if(FALSE == bIsPenetrate){
					break;
				}
			}
		}
	}

	return eNormal;
}

//寻找下一个弹跳对象
CSSGameUnit* CSSSkillEffect_Fly::FindNextBounceTarget(SSEmitEntity* pSSE, CSSGameUnit* exceptGU, bool bIfCanRepeat){
	const SSkillModelEmitCfg* pCfg = GetModelConfig();
	vector<CSSGameUnit*> vecAreaObj; 
	GetBattle()->FindAroundGU(pSSE->cCurPos,(FLOAT)pCfg->n32FlyPar2 + 500,vecAreaObj);

	for(vector<CSSGameUnit*>::iterator iter = vecAreaObj.begin(); iter != vecAreaObj.end();){
		CSSGameUnit* pcTempGU = *iter;
		float tempDist = (pcTempGU->GetCurPos() - pSSE->cCurPos).length();
		//弹跳自然不能选择起点的对象
		if (pcTempGU == exceptGU || pcTempGU->GetExpire() || pcTempGU->IsDead() || !CheckHitTargetType(pcTempGU) || !CheckHitTargetCamp(pcTempGU) || tempDist > pCfg->n32FlyPar2){
			iter = vecAreaObj.erase(iter);
		}
		else{
			//如果不能重复，则已弹跳过的对象也不能
			if(FALSE == bIfCanRepeat){
				bool bIsRepeat = pSSE->lHitTargetsArr.IsHvElement(pcTempGU->GetObjGUID());
				if(bIsRepeat){
					iter = vecAreaObj.erase(iter);
				}
				else{
					++iter;
				}
			}
			else{
				++iter;
			}
		}
	}
	if(FALSE == vecAreaObj.empty()){
		return vecAreaObj[rand() % vecAreaObj.size()];
	}
	return NULL;
}

//发送创建新的飞行道具消息给客户端
INT32 CSSSkillEffect_Fly::NotifyNewEmitObjectToGC(CSSGameUnit* pStartGU, SSEmitEntity* pSSE){
	if(m_pcMasterGO == NULL){
		return eEC_EffectEnd;
	}

	GSToGC::EmitSkill sEmitSkill;
	sEmitSkill.set_guid(pStartGU->GetObjGUID());
	sEmitSkill.set_effectid(m_pCfg->un32SkillModelID);
	sEmitSkill.set_uniqueid(pSSE->n32ProjectID);
	GSToGC::Dir* dir = new GSToGC::Dir;
	CSSBattle::SetDir(pSSE->cCurDir, *dir);
	sEmitSkill.set_allocated_dir(dir);
	//飞行道具如果没有目标(非跟踪)，则不发
	if(pSSE->pcTargetGU != NULL){
		sEmitSkill.set_targuid(pSSE->pcTargetGU->GetObjGUID());
		GSToGC::Pos* tarPos = new GSToGC::Pos;
		CSSBattle::SetPos(pSSE->pcTargetGU->GetGOActionStateInfo().cPos, *tarPos);
		sEmitSkill.set_allocated_tarpos(tarPos);
	}
	if (m_pSkillRuntimeData){
		sEmitSkill.set_ifabsorbskill(m_pSkillRuntimeData->cpsCfg->bIsConsumeSkill);
	}

	GetBattle()->SendMsgToAllWatcher(sEmitSkill, sEmitSkill.msgid(), m_pcMasterGO);

	return eNormal;
}
//发送销毁飞行道具消息给客户端
INT32 CSSSkillEffect_Fly::NotifyDestoryEmitObjectToGC(SSEmitEntity* pSSE){
	if(m_pcMasterGO == NULL){
		return eEC_EffectEnd;
	}
	
	GSToGC::DestroyEmitEffect sMsg;
	sMsg.set_uniqueid(pSSE->n32ProjectID);
	GetBattle()->SendMsgToAllWatcher(sMsg, sMsg.msgid(), m_pcMasterGO);

	return eNormal;
}
//发送飞行道具转向消息给客户端(只有回旋镖需要用到)
INT32 CSSSkillEffect_Fly::NotifyTurnEmitObjectToGC(SSEmitEntity* pSSE){
	if(m_pcMasterGO == NULL){
		return eEC_EffectEnd;
	}
	
	GSToGC::NotifySkillModelEmitTurn sMsg;
	sMsg.set_progectid(pSSE->n32ProjectID);
	sMsg.set_guid(m_pcMasterGO->GetObjGUID());
	GetBattle()->SendMsgToAllWatcher(sMsg, sMsg.msgid(), m_pcMasterGO);
	return eNormal;
}

INT32 CSSSkillEffect_Fly::End(){
	return eNormal;
}

bool CSSSkillEffect_Fly::IfParaInvalid(){
	return TRUE == IsForceStop();
}

}