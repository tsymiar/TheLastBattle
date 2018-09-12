#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSCfgMgr.h"
#include "SSMoveTool.h"
#include "SSBTreeNode.h"
#include "SSBTreeAction.h"
#include "SSBTreeSelector.h"
#include "SSBTreeSequence.h"

#include "SSBTreeCon_GameInStartTime.h"
#include "SSBTreeCon_HasItemToBuy.h"
#include "SSBTreeCon_HasCpToBuyItem.h"
#include "SSBTreeCon_Control.h"
#include "SSBTreeCon_Dead.h"
#include "SSBTreeCon_Moving.h"
#include "SSBTreeCon_Restrain.h"
#include "SSBTreeCon_Attacking.h"
#include "SSBTreeCon_TransHome.h"
#include "SSBTreeCon_HeroNumLower.h"
#include "SSBTreeCon_SelfHPLower.h"
#include "SSBTreeCon_BeHurted.h"
#include "SSBTreeCon_HaveSkillByType.h"
#include "SSBTreeCon_SkillNotCD.h"
#include "SSBTreeCon_SkillHasCost.h"
#include "SSBTreeCon_NotSilence.h"
#include "SSBTreeCon_SkillInDist.h"
#include "SSBTreeCon_NoEnemyHero.h"
#include "SSBTreeCon_FriendHeroHurted.h"
#include "SSBTreeCon_EnemyHeroAttackFriendHero.h"
#include "SSBTreeCon_SkillOutDist.h"
#include "SSBTreeCon_HasEnemyHero.h"
#include "SSBTreeCon_EnemyHeroHPLower.h"
#include "SSBTreeCon_FullFury.h"
#include "SSBTreeCon_NoEnemy.h"
#include "SSBTreeCon_HasAbsorbMonster.h"
#include "SSBTreeCon_HasSelfAltar.h"
#include "SSBTreeCon_Random.h"
#include "SSBTreeCon_LevelBigger.h"
#include "SSBTreeCon_CpBigger.h"
#include "SSBTreeCon_HasTarget.h"
#include "SSBTreeCon_HasEmptyAbsorbSlot.h"
#include "SSBTreeCon_Absorbing.h"
#include "SSBTreeCon_PrepareSkill.h"
#include "SSBTreeCon_EnemyTargetHpLowerThanHit.h"
#include "SSBTreeCon_AutoAttacking.h"
#include "SSBTreeCon_UsingSkill.h"
#include "SSBTreeCon_HeroInSpring.h"
#include "SSBTreeCon_NotBeHurt.h"
#include "SSBTreeCon_SkillTypeCheck.h"
#include "SSBTreeCon_MonsterType.h"
#include "SSBTreeCon_CheckAttackTarget.h"
#include "SSBTreeCon_HasCpForAbsorb.h"

#include "SSBTreeAct_Wait.h"
#include "SSBTreeAct_BuyItem.h"
#include "SSBTreeAct_UseSkill.h"
#include "SSBTreeAct_TransHome.h"
#include "SSBTreeAct_MoveByPath.h"
#include "SSBTreeAct_NormalAttack.h"
#include "SSBTreeAct_FuryExpersion.h"
#include "SSBTreeAct_PutAltarMonster.h"
#include "SSBTreeAct_Absorb.h"

namespace SceneServer{

const static UINT8	c_maxRobotActionNum = 3;

CSSAI_HeroRobot::CSSAI_HeroRobot(CSSGameUnit* pGU,ERobotAIType eAiType):CSSAI_Hero(pGU)
	, m_pBTreeRoot(NULL)
	, m_bIfMoveByPath(FALSE)
	, m_un8ChoosedPath(0)
	, m_un16PathIndex(0)
	, m_bIfPathForward(TRUE)
	, m_buyItemIndex(0)
	, m_startWaitTime(0)
	, m_tActionTime(0)
{
	if (m_pcMasterGU){
		m_pcHeroGU = m_pcMasterGU->TryCastToHero();
	}
	m_bIfFliter = TRUE;
	//读取并保存地图节点
	const SRobotGrpPath* pGrpPath = CSSCfgMgr::GetSSCfgInstance().GetRobotAIPath(m_pcMasterGU->GetCurBattle()->GetBattleMapId(),m_pcMasterGU->GetCampID());
	for(auto iter = pGrpPath->pathMap.begin(); iter != pGrpPath->pathMap.end(); iter++){
		const SRobotPath* path = &iter->second;
		m_path.push_back(path->path);
	}
	//加载AI行为树
	map<INT32,CSSBTreeNode*> tempNodeMap;
	auto aiCfgMap = CSSCfgMgr::GetSSCfgInstance().GetRobotAICfgVec();
	for(auto iter = aiCfgMap.begin(); iter != aiCfgMap.end(); ++iter){
		SRobotAICfg* pCfg = &*iter;
		if(pCfg->n32AiType != eAiType) continue;
		if(m_pBTreeRoot == NULL && pCfg->n32ParentID == 0){
			m_pBTreeRoot = CreateTreeNode(pCfg->n32ID,pCfg->n32NodeType,pCfg->n32ModelID,pCfg->an32Parameters);
			if(m_pBTreeRoot != NULL)
				tempNodeMap.insert(std::make_pair(m_pBTreeRoot->GetID(), m_pBTreeRoot));
			continue;
		}
		auto parentNodeIter = tempNodeMap.find(pCfg->n32ParentID);
		if(parentNodeIter == tempNodeMap.end()) continue;
		CSSBTreeNode* pParentNode = parentNodeIter->second;
		CSSBTreeNode* pNode = CreateTreeNode(pCfg->n32ID,pCfg->n32NodeType,pCfg->n32ModelID,pCfg->an32Parameters);
		if(pNode == NULL) continue;
		pParentNode->AddChild(pNode);
		tempNodeMap.insert(std::make_pair(pNode->GetID(), pNode));		
	}
}

CSSAI_HeroRobot::~CSSAI_HeroRobot(){
	m_pcHeroGU = NULL;
	if(NULL != m_pBTreeRoot){
		delete m_pBTreeRoot;
		m_pBTreeRoot = NULL;
	}
	m_path.clear();
	m_skillVec.clear();
}

CSSBTreeNode*	CSSAI_HeroRobot::CreateTreeNode(INT32 ID, INT32 type, INT32 modelID, INT32 parameters[16]){
	if(type == eBTreeNodeType_Selector){
		CSSBTreeSelector* pNode = new CSSBTreeSelector(ID);
		return pNode;
	}
	else if(type == eBTreeNodeType_Sequence){
		CSSBTreeSequence* pNode = new CSSBTreeSequence(ID);
		return pNode;
	}
	else if(type == eBTreeNodeType_Condition){
		return CreateCondition(ID,modelID,parameters);
	}
	else if(type == eBTreeNodeType_Action){
		return CreateAction(ID,modelID,parameters);
	}
	ELOG(LOG_ERROR, "创建行为树失败(id:%d,type:%d,modelid:%d)", ID, type, modelID);
	return NULL;
}

CSSBTreeNode*	CSSAI_HeroRobot::CreateCondition(INT32 ID, INT32 modelID, INT32 parameters[16]){
	switch(modelID){
	case 80001:
		return new CSSBTreeCon_GameInStartTime(ID,parameters);
	case 80003:
		return new CSSBTreeCon_HasItemToBuy(ID,parameters);
	case 80004:
		return new CSSBTreeCon_HasCpToBuyItem(ID,parameters);
	case 80005:
		return new CSSBTreeCon_Control(ID,parameters);
	case 80006:
		return new CSSBTreeCon_Dead(ID,parameters);
	case 80007:
		return new CSSBTreeCon_Moving(ID,parameters);
	case 80008:
		return new CSSBTreeCon_Restrain(ID,parameters);
	case 80009:
		return new CSSBTreeCon_Attacking(ID,parameters);
	case 80010:
		return new CSSBTreeCon_TransHome(ID,parameters);
	case 80011:
		return new CSSBTreeCon_HeroNumLower(ID,parameters);
	case 80012:
		return new CSSBTreeCon_SelfHPLower(ID,parameters);
	case 80013:
		return new CSSBTreeCon_BeHurted(ID,parameters);
	case 80014:
		return new CSSBTreeCon_HaveSkillByType(ID,parameters);
	case 80015:
		return new CSSBTreeCon_SkillNotCD(ID,parameters);
	case 80016:
		return new CSSBTreeCon_SkillHasCost(ID,parameters);
	case 80017:
		return new CSSBTreeCon_NotSilence(ID,parameters);
	case 80018:
		return new CSSBTreeCon_SkillInDist(ID,parameters);
	case 80019:
		return new CSSBTreeCon_NoEnemyHero(ID,parameters);
	case 80020:
		return new CSSBTreeCon_FriendHeroHurted(ID,parameters);
	case 80021:
		return new CSSBTreeCon_EnemyHeroAttackFriendHero(ID,parameters);
	case 80022:
		return new CSSBTreeCon_SkillOutDist(ID,parameters);
	case 80023:
		return new CSSBTreeCon_HasEnemyHero(ID,parameters);
	case 80024:
		return new CSSBTreeCon_EnemyHeroHPLower(ID,parameters);
	case 80025:
		return new CSSBTreeCon_HasTarget(ID,parameters);
	case 80026:
		return new CSSBTreeCon_NoEnemy(ID,parameters);
	case 80027:
		return new CSSBTreeCon_HasAbsorbMonster(ID,parameters);
	case 80028:
		return new CSSBTreeCon_HasSelfAltar(ID,parameters);
	case 80029:
		return new CSSBTreeCon_Random(ID,parameters);
	case 80030:
		return new CSSBTreeCon_LevelBigger(ID,parameters);
	case 80031:
		return new CSSBTreeCon_CpBigger(ID,parameters);
	case 80032:
		return new CSSBTreeCon_HasEmptyAbsorbSlot(ID,parameters);
	case 80033:
		return new CSSBTreeCon_Absorbing(ID,parameters);
	case 80034:
		return new CSSBTreeCon_PrepareSkill(ID,parameters);
	case 80035:
		return new CSSBTreeCon_FullFury(ID,parameters);
	case 80036:
		return new CSSBTreeCon_EnemyTargetHpLowerThanHit(ID,parameters);
	case 80037:
		return new CSSBTreeCon_AutoAttacking(ID,parameters);
	case 80038:
		return new CSSBTreeCon_UsingSkill(ID,parameters);
	case 80039:
		return new CSSBTreeCon_HeroInSpring(ID,parameters);
	case 80040:
		return new CSSBTreeCon_NotBeHurt(ID,parameters);
	case 80041:
		return new CSSBTreeCon_SkillTypeCheck(ID,parameters);
	case 80042:
		return new CSSBTreeCon_MonsterType(ID,parameters);
	case 80043:
		return new CSSBTreeCon_CheckAttackTarget(ID,parameters);
	case 80044:
		return new CSSBTreeCon_HasCpForAbsorb(ID,parameters);
	default:
		ELOG(LOG_ERROR, "id:%d,modelid:%d", ID, modelID);
		break;
	}
	return NULL;
}

CSSBTreeNode*	CSSAI_HeroRobot::CreateAction(INT32 ID, INT32 modelID, INT32 parameters[16]){
	switch(modelID){
	case 90001:
		return new CSSBTreeAct_Wait(ID,parameters);
	case 90002:
		return new CSSBTreeAct_BuyItem(ID,parameters);
	case 90003:
		return new CSSBTreeAct_UseSkill(ID,parameters);
	case 90004:
		return new CSSBTreeAct_TransHome(ID,parameters);
	case 90005:
		return new CSSBTreeAct_MoveByPath(ID,parameters);
	case 90006:
		return new CSSBTreeAct_NormalAttack(ID,parameters);
	case 90007:
		return new CSSBTreeAct_FuryExpersion(ID,parameters);
	case 90008:
		return new CSSBTreeAct_PutAltarMonster(ID,parameters);
	case 90009:
		return new CSSBTreeAct_Absorb(ID,parameters);
	default:
		ELOG(LOG_ERROR, "id:%d,modelid:%d", ID, modelID);
		break;
	}
	return NULL;
}

bool	CSSAI_HeroRobot::MoveByPath(INT32 pathIndex, INT32 nodeIndex, bool moveForward){
	m_un8ChoosedPath = pathIndex;
	m_un16PathIndex = nodeIndex;
	m_bIfPathForward = moveForward;
	m_pathPos = m_path[m_un8ChoosedPath][m_un16PathIndex];
	bool rst = eNormal == AskMoveTar(m_pathPos);
	if(rst) m_bIfMoveByPath = rst;
	return rst;
}

void	CSSAI_HeroRobot::OnMoveBlock(){
	CSSAI::OnMoveBlock();
	m_bIfMoveByPath = FALSE;
}

INT32	CSSAI_HeroRobot::HeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if(m_pBTreeRoot == NULL) return eNormal;
	//如果死亡，如果长时间无操作,清除所有动作
	if(m_pcHeroGU->IsDead() || (GetLastActionTime() != 0 && tUTCMilsec - GetLastActionTime() > 8000)){
		//if(m_bIfMoveByPath == TRUE){
			m_bIfMoveByPath = false;
			m_pcHeroGU->GetCurBattle()->AskStopMoveObjectAll(m_pcHeroGU);
		//}
		ClearAction();
		SetLastActionTime(tUTCMilsec);
	}
	UINT8 actionNum = 0;
	while(TRUE){
		actionNum++;
		//为何>=3?
		if(actionNum >= 3) break;
		vector<SBTreeResult> rstVec;//rstVec获取没使用？
		bool ifEndHeartBeat = false;
		CSSBTreeNode* pActionNode = NULL;
		if(TRUE == m_pBTreeRoot->Travel(this,pActionNode,&rstVec) && pActionNode != NULL){
			//找到了可以做的动作
			CSSBTreeAction* pAct = (CSSBTreeAction*)pActionNode;
			if(TRUE == pAct->Action(this)){
				//看看动作是否耗时。如果耗时，就结束本次活动
				if(pAct->IfNeedTime(this)){
					ifEndHeartBeat = true;
				}
			}
		}
		//string str;
		//for(auto iter = rstVec.begin();iter!=rstVec.end();iter++){
		//	char t[256];
		//	sprintf(t, "%d:%d,", iter->id,iter->rst);
		//	str += t;
		//}
		//ELOG(LOG_DBBUG,"AI__ %d:%s",m_pcHeroGU->GetHeroID(),str.c_str());
		if(ifEndHeartBeat){
			break;
		}
	}

	//检查根据路点移动状态
	if(m_bIfMoveByPath){
		do{
			//如果开始自动攻击了，则需要打断自动寻路
			if(IfAutoAttacking()){
				m_bIfMoveByPath = FALSE;
				break;
			}
			if(FALSE == IfMoving()){
				MoveByPath(m_un8ChoosedPath,m_un16PathIndex,m_bIfPathForward);
			}
			else{
				//检查是否到达目标点
				if(m_pcHeroGU->GetCurPos().CanWatch(300,m_pathPos)){
					bool canContinueMove = FALSE;
					//向前走，则路点加一
					if(m_bIfPathForward){
						m_un16PathIndex++;
						if(m_un16PathIndex < m_path[m_un8ChoosedPath].size()){
							canContinueMove = TRUE;
						}
						else{
							m_un16PathIndex--;
						}
					}
					//向后走，路点减1
					else if(m_un16PathIndex > 0){
						m_un16PathIndex--;
						if(m_un16PathIndex >= 0){
							canContinueMove = TRUE;
						}
						else{
							m_un16PathIndex = 0;
						}
					}
					//如果下一个点有效，则向下一个点移动
					if(canContinueMove){
						MoveByPath(m_un8ChoosedPath,m_un16PathIndex,m_bIfPathForward);
					}
					else{
						AskStopMove();
					}
				}
			}
		}
		while(FALSE);
		
	}


	this->CSSAI_Hero::HeartBeat(tUTCMilsec,tTickSpan);
	return eNormal;
}

INT32	CSSAI_HeroRobot::Random(INT32 start, INT32 end){
	std::uniform_int_distribution<int> dis(start,end);  
	return dis(GetHero()->GetCurBattle()->GetRandomEngine());
}


}