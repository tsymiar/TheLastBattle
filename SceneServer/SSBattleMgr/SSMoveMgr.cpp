#include "SSMoveMgr.h"
#include "SSMoveObject.h"
#include "SSMoveTool.h"
#include "SSAStar.h"

#include "SSGameUnit.h"
namespace SceneServer{
	CSSMoveMgr::CSSMoveMgr():
	m_staticBlockArray(NULL),
	m_regionWidth(0),
	m_regionHeight(0),
	m_regionSize(0)
{
	for (INT32 i = 0; i < 1024; ++i){
		m_heartBeatTempArray[i] = nullptr;
	}
}

CSSMoveMgr::~CSSMoveMgr(){
	if(m_staticBlockArray != NULL){
		delete []m_staticBlockArray;
		m_staticBlockArray = NULL;
	}
	DelAllPathCache();
}

void CSSMoveMgr::AddMoveObject(ISSMoveObject* pMObject, UINT8 ui8GrpID)
{
	Assert(ui8GrpID!=SSImpactGruop_None);//它不会和别人碰\别人也不会和它碰//
	RemoveMoveObject(pMObject);
	pMObject->SetGroupID(ui8GrpID);
	pMObject->SetMoveStatus(SSMoveObjectStatus_Stand);
	pMObject->SetStepMoveTarget(pMObject->GetColSphere());
	pMObject->SetBeforeMovePos(pMObject->GetStepMoveTarget().c);
	m_mObjectMap[ui8GrpID].insert(pMObject);
	m_allMObjectSet.insert(pMObject);
}

void CSSMoveMgr::RemoveMoveObject(ISSMoveObject* pMObject)
{
	if (pMObject->GetGroupID() == SSImpactGruop_None) return;
	hash_set<ISSMoveObject*>* pSet = &m_mObjectMap[pMObject->GetGroupID()];
	pSet->erase(pMObject);
	m_allMObjectSet.erase(pMObject);
}

void CSSMoveMgr::AddStaticTriBlockInfo(UINT16 x, UINT16 y, ETriDir dir){
	m_staticTriBlockMap[GetRegionID_INT(x,y)] = SIntTri(x,y,dir);
}


//开始按照方向移动(可以在移动中调用)
bool CSSMoveMgr::AskStartMoveDir(ISSMoveObject* pMObject, ColVector dir){
	if(FALSE == AskStartCheck(pMObject)){
		return FALSE;
	}
	if(pMObject->GetMoveStatus() == SSMoveObjectStatus_Stand){
		pMObject->SetMoveType(SSMoveObjectMoveType_Dir);
		TIME_MILSEC now = GetUTCMiliSecond();
		pMObject->SetMoveStatus(SSMoveObjectStatus_Move);
		pMObject->SetAskMoveDir(dir);
		pMObject->SetDir(dir);
		pMObject->SetStartMoveTime(now);
		//计算下一个100毫秒后的位置
		pMObject->CalculateStepMoveTarget(now + 100);
		//检查是否会直接撞墙
		if(FALSE == TestNextStepMove(pMObject)){
			if(FALSE == TryTurnDir(pMObject)){
				pMObject->Stop(now,FALSE);
				return FALSE;
			}
		}
		pMObject->OnStartMove(pMObject->GetDir());
		return TRUE;
	}
	return FALSE;
}

//请求移动到指定坐标(可以在移动中调用)
bool CSSMoveMgr::AskStartMoveToTar(ISSMoveObject* pMObject, ColVector pos, bool bIfMoveToBlackPoint, bool bIfFliter){
	TIME_MILSEC now = GetUTCMiliSecond();
	if(now - pMObject->GetLastFailStartTime() < 150){
		//同一个单位的最短尝试时间为150;
		return FALSE;
	}
	if(FALSE == AskStartCheck(pMObject)){
		pMObject->SetLastFailStartTime(now);
		return FALSE;
	}
	pMObject->SetMoveType(SSMoveObjectMoveType_Tar);
	if(pMObject->GetMoveStatus() == SSMoveObjectStatus_Stand){
		//获取周围的动态碰撞数据
		UINT32 dynamicBlockSet[c_maxDynamicBlockNum] = {0};
		UINT16 dynBlockLength = 0;
		FindDynamicBlock(pMObject,dynamicBlockSet, dynBlockLength);
		//准备寻路的基础数据
		ColVector nowVec = pMObject->GetColVector();
		UINT16 startX = GetRegionIndexX(nowVec.x);
		UINT16 startY = GetRegionIndexY(nowVec.y);
		UINT16 targetX= GetRegionIndexX(pos.x);
		UINT16 targetY= GetRegionIndexY(pos.y);

		bool bIfNeedFind = FALSE;
		if(dynBlockLength == 0){
			//没有阻挡，尝试读取缓存数据
			SIntPoint* cachePath = GetPathByCache(GetPathID(GetRegionID_INT(startX,startY), GetRegionID_INT(targetX,targetY)));
			if(cachePath != NULL){
				//有缓存的值，直接使用
				SIntPoint* path = &pMObject->GetPathCell()[1];
				memcpy(path,cachePath,sizeof(SIntPoint)*(c_n32MaxOrderPathNode-1));
				pMObject->SetPathLength(1);
				for(int i=0;i<c_n32MaxOrderPathNode-1;i++){
					if(cachePath[i].n32XIdx == 0 || cachePath[i].n32ZIdx == 0){
						pMObject->SetPathLength(i + 1);
						break;
					}
				}
			}
			else{
				//没有缓存的值
				bIfNeedFind = TRUE;
			}
		}

		if(dynBlockLength > 0 || bIfNeedFind){
			//如果有动态阻挡，或者找不到缓存，则还是需要寻找
			SFindPathInfoNew aStartInfo;
			//设置起点
			aStartInfo.psStartIntPoint.n32XIdx = startX;
			aStartInfo.psStartIntPoint.n32ZIdx = startY;
			//设置终点
			aStartInfo.psTargetIntPoint.n32XIdx = targetX;
			aStartInfo.psTargetIntPoint.n32ZIdx = targetY;
			pMObject->GetPathCell()[0] = aStartInfo.psTargetIntPoint;
			//设置阻挡
			aStartInfo.pDynamicBlockArray = dynamicBlockSet;
			//设置其他属性
			aStartInfo.ifToTarget = bIfMoveToBlackPoint;
			aStartInfo.ifFliter = bIfFliter;
			//初始化接收用的数组
			aStartInfo.psPathBuff = &pMObject->GetPathCell()[1];
			aStartInfo.n32PathNodeNum = c_n32MaxOrderPathNode - 1;

			m_Astar.FindPath(aStartInfo);
			//如果寻路失败，返回
			if(aStartInfo.n32PathNodeNum <= 0){
				pMObject->SetLastFailStartTime(now);
				return FALSE;
			}

			pMObject->SetPathLength(aStartInfo.n32PathNodeNum + 1);

			if(dynBlockLength == 0 && aStartInfo.n32PathNodeNum > 0){
				//没有动态阻挡，把本次寻路缓存
				SavePathToCache(GetPathID(GetRegionID_INT(startX,startY),GetRegionID_INT(targetX,targetY)),aStartInfo.psPathBuff,aStartInfo.n32PathNodeNum);
			}
		}

		//设置移动参数
		pMObject->SetStartMoveTime(GetUTCMiliSecond());
		pMObject->SetMoveStep(1);
		SetNextMovePoint(pMObject);

		//判断动态碰撞
		pMObject->IfStart = TRUE;
		TIME_MILSEC now = GetUTCMiliSecond();
		pMObject->SetStartMoveTime(now);
		pMObject->CalculateStepMoveTarget(now + 100);
		//如果下一个100毫秒后会阻挡，返回。正常情况下，这里返回的概率应该很低。如果有游戏对象在地图上卡住，优先检查这里，看是否寻到的路直接被阻挡
		if(FALSE == TestNextStepMove(pMObject,FALSE,TRUE)){
			pMObject->Stop(now,FALSE);
			pMObject->IfStart = FALSE; 
			return FALSE;
		}
		pMObject->IfStart = FALSE;
		pMObject->OnStartMove(pMObject->GetDir());

		return TRUE;
	}

	pMObject->SetLastFailStartTime(now);

	return FALSE;
}

//开始强制位移(可以在移动中调用)
bool CSSMoveMgr::AskStartMoveForced(ISSMoveObject* pMObject, ColVector dir, FLOAT speed, bool bIfImpact){
	if(FALSE == AskStartCheck(pMObject)){
		return FALSE;
	}
	//强制位移可以打断其他所有位移，包括强制位移(这里逻辑还有些问题)
	if(pMObject->GetMoveStatus() == SSMoveObjectStatus_Stand){
		pMObject->SetMoveType(SSMoveObjectMoveType_ForceMove);
		TIME_MILSEC now = GetUTCMiliSecond();
		pMObject->SetDir(dir);
		pMObject->SetStartMoveTime(now);
		pMObject->SetMoveStatus(SSMoveObjectStatus_ForceMove);
		pMObject->SetIfForceMoveImpact(bIfImpact);
		pMObject->SetForceMoveSpeed(speed);
		return TRUE;
	}
	return FALSE;
}

//将移动目标设置为下一个点
bool CSSMoveMgr::SetNextMovePoint(ISSMoveObject* pMObject){
	//获取下一个目标点
	SIntPoint firstIntPoint;
	if(FALSE == pMObject->GetNowMoveIntTar(firstIntPoint))
		return FALSE;
	//获取目标中心点
	ColVector firstPoint = GetRegionCenter(firstIntPoint.n32XIdx, firstIntPoint.n32ZIdx);
	//设置目标方向
	ColVector dir = (firstPoint - pMObject->GetColVector()).Unit();
	pMObject->SetDir(dir);
	pMObject->SetMoveStatus(SSMoveObjectStatus_Move);
	return TRUE;
}

//寻找周围的动态阻挡格子
void CSSMoveMgr::FindDynamicBlock(ISSMoveObject* pMObject, UINT32 *dynamicArray, UINT16 &length){
	ColVector selfSph = pMObject->GetColVector();
	if(pMObject->GetImpactType_Now() == SSMoveObjectImpactType_NoImpact) return;
	if (!pMObject->IfCanImpact()) return;
	UINT16 index = 0;
	SSMoveObjectImpactType impact_type = pMObject->GetImpactType_Now();
	switch (impact_type)
	{
		//如果是组碰撞对象，则需要检查所有的同组成员，以及全碰撞组的成员
	case SSMoveObjectImpactType_Group:
		{
			hash_set<ISSMoveObject*>& group_set = m_mObjectMap[pMObject->GetGroupID()];
			for (auto it = group_set.begin(); it!= group_set.end(); ++it)
			{
				if (*it == pMObject) continue;
				AddDnnamicBlock(pMObject,*it,dynamicArray,length,index);
			}
			hash_set<ISSMoveObject*>& group_all_set = m_mObjectMap[SSImpactGruop_All];
			for (auto it = group_all_set.begin(); it!= group_all_set.end(); ++it)
			{
				if (*it == pMObject) continue;
				AddDnnamicBlock(pMObject,*it,dynamicArray,length,index);
			}
		}
		break;
		//如果是全碰撞对象，则直接检查所有的碰撞对象
	case SSMoveObjectImpactType_ALL:
		{
			for(hash_set<ISSMoveObject*>::iterator iter = m_allMObjectSet.begin(); iter != m_allMObjectSet.end(); ++iter)
			{
				if(*iter == pMObject) continue;
				AddDnnamicBlock(pMObject,*iter,dynamicArray,length,index);
			}
		}
		break;
	}
}

void	CSSMoveMgr::AddDnnamicBlock(ISSMoveObject* pMObject, ISSMoveObject* pTarget, UINT32 *dynamicArray, UINT16 &length, UINT16 &index){
	//检查状态
	if(pTarget->GetMoveStatus() != SSMoveObjectStatus_Stand){
		return;
	}
	//检查阻挡能力
	if(pTarget->IfCanImpact() == FALSE){
		return;
	}
	//检查距离
	ColVector selfSph = pMObject->GetColVector();
	ColSphere sph = pTarget->GetColSphere();
	if(abs(sph.c.x - selfSph.x) > 500 || abs(sph.c.y - selfSph.y) > 500){
		return;
	}

	//检查的方法为： 获取以碰撞者为圆心，碰撞者和移动者半径之和为半径，覆盖的所有格子
	sph.r += pMObject->GetColRadius();
	//获取阻挡的格子
	for(float x=sph.c.x - sph.r; x<= sph.c.x + sph.r + m_regionSize; x+=m_regionSize){
		for(float y=sph.c.y - sph.r; y<= sph.c.y + sph.r + m_regionSize; y+=m_regionSize){
			//判断正方形是否在圆内
			UINT16 uiX = GetRegionIndexX(x);
			UINT16 uiY = GetRegionIndexY(y);
			//创造正方形
			ColAABB aabb;
			aabb.min.x = uiX*m_regionSize; aabb.min.y = uiY*m_regionSize;
			aabb.max.x = (uiX+1)*m_regionSize; aabb.max.y = (uiY+1)*m_regionSize;
			if(CSSMoveTool::IfImpact(sph,aabb)){
				if(index < c_maxDynamicBlockNum)
					dynamicArray[index++] = GetRegionID_INT(uiX,uiY);
			}
		}
	}
	length = index;
}

//请求停止移动
bool	CSSMoveMgr::AskStopMoveObject(ISSMoveObject* pMObject,EnumAskStopMoveType type){
	//判断对象是否存在
	if(m_allMObjectSet.end() == m_allMObjectSet.find(pMObject)){
		return FALSE;
	}
	//根据当前状态进行处理
	if(pMObject->GetMoveStatus() == SSMoveObjectStatus_Stand){
		return FALSE;
	}
	//每种枚举值代表只能停止指定类型的移动。All类型的可以停止所有移动
	if(pMObject->GetMoveStatus() == SSMoveObjectStatus_Move){
		if(type == EnumAskStopMoveType_ALL || 
			(type == EnumAskStopMoveType_DIR && pMObject->GetMoveType() == SSMoveObjectMoveType_Dir)
			|| (type == EnumAskStopMoveType_TAR && pMObject->GetMoveType() == SSMoveObjectMoveType_Tar))
			StopLastStep(pMObject,TRUE);
	}
	if(pMObject->GetMoveStatus() == SSMoveObjectStatus_ForceMove){
		if(type == EnumAskStopMoveType_ALL || type == EnumAskStopMoveType_FORCEMOVE)
		{
			StopLastStep(pMObject,TRUE);
			ResetPos(pMObject,pMObject->GetColVector(),TRUE);
		}
	}

	return TRUE;
}

bool	CSSMoveMgr::AskStartCheck(ISSMoveObject* pMObject){
	//判断对象是否存在
	if(m_allMObjectSet.end() == m_allMObjectSet.find(pMObject)){
		return FALSE;
	}
	//根据当前状态进行处理
	if(pMObject->GetMoveStatus() == SSMoveObjectStatus_ForceMove){
		return FALSE;
	}
	if(pMObject->GetMoveStatus() == SSMoveObjectStatus_Move){
		//如果是移动状态,先将上一次移动终结
		StopLastStep(pMObject,FALSE);
	}
	return TRUE;
}

INT32 CSSMoveMgr::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){

	ProfileInScope lProfileInScope(StaticsType_Move);

	TIME_MILSEC now = GetUTCMiliSecond();

	const UINT16 moveObjectSize = m_allMObjectSet.size();
	Assert(moveObjectSize<=1024);

	for (INT32 i = 0; i < 1024; ++i){
		m_heartBeatTempArray[i] = nullptr;
	}

	//重置所有目标为未移动
	UINT16 index = 0;
	for(auto iter = m_allMObjectSet.begin(); iter != m_allMObjectSet.end(); ++iter){
		ISSMoveObject* pMObject = *iter;
		if(pMObject->GetMoveStatus() == SSMoveObjectStatus_Stand){
			continue;
		}
		pMObject->SetStepMoved(FALSE);
		//如果是按照方向位移的玩家，要定时将其方向搞回来
		if(pMObject->GetMoveType() == SSMoveObjectMoveType_Dir && pMObject->GetDir() != pMObject->GetAskMoveDir()){
			ColVector nowDir = pMObject->GetDir();
			pMObject->SetDir(pMObject->GetAskMoveDir());
			pMObject->CalculateStepMoveTarget(now);
			//检查其静态碰撞
			if (IfStaticImpact(pMObject))
			{
				//如果有静态碰撞，将其方向改回来
				pMObject->SetDir(nowDir);
			}
			else{
				//方向改变了，发送消息
				pMObject->OnStartMove(pMObject->GetDir());
			}
		}
		//预先计算下一步将要移动到的位置
		pMObject->CalculateStepMoveTarget(now);
		m_heartBeatTempArray[index++] = pMObject;
	}

	UINT16 moveTargetNum = 0;
	//反复尝试移动所有对象，直到没有任何一个对象可以移动为止
	do{
		moveTargetNum = 0;
		for(int i=0;i<moveObjectSize;i++){
			ISSMoveObject* pMObject = m_heartBeatTempArray[i];
			if(pMObject == NULL) continue;
			//尝试移动。返回值代表成功与否。在时间间隔足够小的时候，移动成功也可能距离为0。
			float moveDist = 0;
			if(TRUE == TryMove(pMObject,now, moveDist)){
				//位移成功
				++moveTargetNum;
				//检查是否需要更换移动点，或已移动到目标
				CheckTargetMoveStatus(pMObject,moveDist);
				m_heartBeatTempArray[i] = NULL;
			}
		}
	}while(moveTargetNum > 0);

	//剩下的目标都是不能移动的，将其停止
	for(int i=0;i<moveObjectSize;i++){
		ISSMoveObject* pMObject = m_heartBeatTempArray[i];
		if(pMObject == NULL) continue;
		pMObject->Stop(now);
	}

	//重新检查所有目标，看是否可以在下一次心跳移动
	for(auto iter = m_allMObjectSet.begin(); iter != m_allMObjectSet.end(); ++iter){
		ISSMoveObject* pMObject = *iter;
		if(pMObject->GetMoveStatus() == SSMoveObjectStatus_Stand) continue;
		if(pMObject->GetMoveStatus() == SSMoveObjectStatus_ForceMove) continue;
		pMObject->CalculateStepMoveTarget(now + 100);
		if(FALSE == TestNextStepMove(pMObject,pMObject->GetMoveType() == SSMoveObjectMoveType_Dir)){
			//下一次心跳移动会失败，将其停止
			pMObject->Stop(now);
		}
	}

	return eNormal;
}

bool CSSMoveMgr::TryMove(ISSMoveObject* pMObject,TIME_MILSEC now, float &moveDist)
{
	bool bIfForceMoveImpact = pMObject->GetMoveStatus() == SSMoveObjectStatus_ForceMove && pMObject->IfForceMoveImpact();

	//检查静态碰撞,只有按照方向进行移动的目标才需要检查//
	if( (pMObject->GetMoveType() == SSMoveObjectMoveType_Dir || bIfForceMoveImpact))
	{
		if (IfStaticImpact(pMObject))
		{
			//额外多检测一步。如果能出去，则依然予以放行
			pMObject->CalculateStepMoveTarget(now + 100);
			if(IfStaticImpact(pMObject)){
				pMObject->CalculateStepMoveTarget(now);
				if(bIfForceMoveImpact || FALSE == TryTurnDir(pMObject)){
					moveDist = 0;
					return FALSE;
				}
			}
			else{
				pMObject->CalculateStepMoveTarget(now);
			}
		}
	}

	//检查动态碰撞//
	if(pMObject->GetMoveStatus() != SSMoveObjectStatus_ForceMove || bIfForceMoveImpact)
	{
		if (IfDynamicImpact(pMObject,TRUE))
		{
			moveDist = 0;
			return FALSE;
		}
	}

	//移动
	moveDist = pMObject->Move(now);
	return TRUE;
}

void CSSMoveMgr::StopLastStep(ISSMoveObject* pMObject, bool bIfCallBack){
	//根据当前时间和最后一次移动的时间差，做最后一小步的移动
	TIME_MILSEC now = GetUTCMiliSecond();
	pMObject->CalculateStepMoveTarget(now);
	float tempFloat;
	TryMove(pMObject,now,tempFloat);
	pMObject->Stop(now,bIfCallBack);
}

void CSSMoveMgr::CheckTargetMoveStatus(ISSMoveObject* pMObject, float movedLength){
	if(pMObject->GetMoveType() != SSMoveObjectMoveType_Tar){
		return;
	}

	if(pMObject->GetMoveStatus() == SSMoveObjectStatus_ForceMove){
		return;
	}

	SIntPoint targetIntPoint;
	if(FALSE == pMObject->GetNowMoveIntTar(targetIntPoint)){
		//如果找不到目标点，返回
		return;
	}
	ColVector targetPoint = GetRegionCenter(targetIntPoint.n32XIdx, targetIntPoint.n32ZIdx);
	ColVector oldDir = targetPoint - pMObject->GetBeforeMovePos();
	ColVector nowDir = targetPoint - pMObject->GetColVector();
	//FLOAT distToPoint = (targetPoint - pMObject->GetBeforeMovePos()).Length();
	//判断是否已经移动到了中途点
	//if((distToPoint - movedLength) < (movedLength/2)){
	if((oldDir.x>=0 && nowDir.x<=0)||(oldDir.x<=0 && nowDir.x>=0)||(oldDir.y>=0 && nowDir.y<=0)||(oldDir.y<=0 && nowDir.y>=0)){
		pMObject->AddMoveStep();
		//寻找下一个移动点
		if(FALSE == SetNextMovePoint(pMObject)){
			//抵达目的地，停止移动
			pMObject->SetMoveStatus(SSMoveObjectStatus_Stand);
			pMObject->OnMoveBlock();
		}
		else{
			pMObject->OnStartMove(pMObject->GetDir());
		}
	}
	else{
		if(oldDir.Length() < nowDir.Length()){
			int abc = 0;
		}
	}
}

bool	CSSMoveMgr::IfStaticImpact(ISSMoveObject* pMObject){
	//检查静态碰撞
	ColSphere nextPoint = pMObject->GetStepMoveTarget();
	INT16	colRadius = pMObject->GetColRadius();
	for(int i=-(colRadius/100+1);i<colRadius/100+1;i++){
		for(int j=-(colRadius/100+1);j<colRadius/100+1;j++){
			float x = nextPoint.c.x + i*100;
			if(x < nextPoint.c.x - colRadius) x = nextPoint.c.x - colRadius;
			if(x > nextPoint.c.x + colRadius) x = nextPoint.c.x + colRadius;
			float y = nextPoint.c.y + j*100;
			if(y < nextPoint.c.y - colRadius) y = nextPoint.c.y - colRadius;
			if(y > nextPoint.c.y + colRadius) y = nextPoint.c.y + colRadius;

			//先检查方格子碰撞
			if(TRUE == m_staticBlockArray[GetRegionID_FLOAT(x,y)]){
				return TRUE;
			}
			//再检查三角形碰撞
			auto iter = m_staticTriBlockMap.find(GetRegionID_FLOAT(x,y));
			if(iter != m_staticTriBlockMap.end()){
				SIntTri &tri = iter->second;
				if(tri.eDir == ETriDir_RightBelow){
					if(x-(tri.x*m_regionSize) + y - (tri.y*m_regionSize) < m_regionSize) { return TRUE;}
				}
				else if(tri.eDir == ETriDir_RightUp){
					if(x-(tri.x*m_regionSize) + ((tri.y+1)*m_regionSize) - y < m_regionSize) { return TRUE;}
				}
				else if(tri.eDir == ETriDir_LeftBelow){
					if(((tri.x+1)*m_regionSize) - x + y - (tri.y*m_regionSize) < m_regionSize) { return TRUE;}
				}
				else if(tri.eDir == ETriDir_LeftUp){
					if(((tri.x+1)*m_regionSize) - x + ((tri.y+1)*m_regionSize) - y < m_regionSize) { return TRUE;}
				}
			}
		}
	}
	return FALSE;
}

//检查下一次心跳是否会与别的物体碰撞
bool CSSMoveMgr::TestNextStepMove(ISSMoveObject* pMObject, bool bIfCheckStaticBlock, bool bIfCheckDynamicBlock)
{
	if(bIfCheckStaticBlock && TRUE == IfStaticImpact(pMObject))
	{
		if(pMObject->GetMoveType() == SSMoveObjectMoveType_Dir){
			if(TryTurnDir(pMObject)){
				return TRUE;
			}
		}
		return FALSE;
	}
	if(bIfCheckDynamicBlock && TRUE == IfDynamicImpact(pMObject,TRUE))
	{
		return FALSE;
	}
	return TRUE;
}

bool	CSSMoveMgr::IfDynamicImpact(ISSMoveObject* pMObject,bool pIfCheckNextStep)
{
	if (!pMObject->IfCanImpact()) return FALSE;
	if (pMObject->GetColRadius() == 0) 
		return FALSE;

	//根据分组和碰撞类型，检查所有可能和自己碰撞的对象
	SSMoveObjectImpactType impact_type = pMObject->GetImpactType_Now();
	switch (impact_type)
	{
	case SSMoveObjectImpactType_Group:
		{
			hash_set<ISSMoveObject*>& group_set = m_mObjectMap[pMObject->GetGroupID()];
			for (auto it = group_set.begin(); it!= group_set.end(); ++it)
			{
				if (*it == pMObject) continue;
				if (IfDynamicImpact(pMObject,*it,pIfCheckNextStep))
				{
					return TRUE;
				}
			}
			hash_set<ISSMoveObject*>& group_all_set = m_mObjectMap[SSImpactGruop_All];
			for (auto it = group_all_set.begin(); it!= group_all_set.end(); ++it)
			{
				if (*it == pMObject) continue;
				if (IfDynamicImpact(pMObject,*it,pIfCheckNextStep))
				{
					return TRUE;
				}
			}
		}
		break;
	case SSMoveObjectImpactType_ALL:
		{
			for(hash_set<ISSMoveObject*>::iterator iter = m_allMObjectSet.begin(); iter != m_allMObjectSet.end(); ++iter)
			{
				if(*iter == pMObject) continue;
				if(TRUE == IfDynamicImpact(pMObject,*iter,pIfCheckNextStep))
				{
					return TRUE;
				}
			}
		}
		break;
	}
	return FALSE;
}

//检查一个对象是否会和别的对象碰撞//
bool	CSSMoveMgr::IfDynamicImpact(ISSMoveObject* pMObjectMove,ISSMoveObject* pMObjectBlock, bool pIfCheckNextStep)
{
	if (!pMObjectBlock->IfCanImpact()) return FALSE;
	if (pMObjectBlock->GetColRadius() == 0) 
		return FALSE;
	//根据参数检查是否和阻挡目标的当前和目标位置碰撞//
	return CSSMoveTool::IfImpact(pMObjectMove->GetStepMoveTarget(),pMObjectBlock->GetColSphere()) ||
		(pIfCheckNextStep && pMObjectBlock->GetMoveStatus() ==  SSMoveObjectStatus_Move &&  CSSMoveTool::IfImpact(pMObjectMove->GetStepMoveTarget(),pMObjectBlock->GetStepMoveTarget()) );
}
//尝试转向来避免碰撞
BOOLEAN CSSMoveMgr::TryTurnDir(ISSMoveObject* pMObject){
	if(pMObject->GetMoveType() != SSMoveObjectMoveType_Dir){
		return FALSE;
	}
	
	ColSphere stepTarget = pMObject->GetStepMoveTarget();
	ColVector nowDir = pMObject->GetDir();
	float nowAngle = atan2f(nowDir.y,nowDir.x);
	if(nowAngle < 0) nowAngle += 3.1415926f*2;
	int nowAngleIndex = nowAngle / 3.1415926f * 4;
	float newAngleArr[4];
	newAngleArr[0] = nowAngleIndex * 3.1415926f / 4;
	newAngleArr[1] = (nowAngleIndex+1) * 3.1415926f / 4;
	newAngleArr[2] = (nowAngleIndex-1) * 3.1415926f / 4;
	newAngleArr[3] = (nowAngleIndex+2) * 3.1415926f / 4;

	TIME_MILSEC now = GetUTCMiliSecond();
	for(int i=0;i<4;i++){
		ColVector newDir(cosf(newAngleArr[i]),sinf(newAngleArr[i]));
		pMObject->SetDir(newDir);
		//尝试新方向是否会碰撞
		pMObject->CalculateStepMoveTarget(now+100);
		if (FALSE == IfStaticImpact(pMObject) && FALSE == IfDynamicImpact(pMObject,FALSE)){
			pMObject->OnStartMove(pMObject->GetDir());
			pMObject->SetRedirectTime(now);
			return TRUE;
		}
	}
	pMObject->SetDir(nowDir);
	return FALSE;
}

//寻找一个不会碰撞的空白点
bool CSSMoveMgr::GetAroundFreePos(ColVector pos, ColVector &outPos, ISSMoveObject *pGU, UINT8 ui8MoveGrpID){
	//默认步进大小为50
	//float step = 50;
	//float r = pGU->GetColRadius();
	//之前的算法：会先遍历y，导致优先竖着找
	//以自己为中心，向周围扩散，寻找一个可以放下的安全点
	//for(int i=0;i<100;i++){
	//	for(int j=0;j<100;j++){
	//		float x = pos.x + step*i * (i%2==0?1:-1);
	//		float y = pos.y + step*j * (j%2==0?1:-1);
	//		if(FALSE == IfPosImpact(pGU,x,y,r,ui8MoveGrpID)){
	//			outPos.x = x;
	//			outPos.y = y;
	//			return TRUE;
	//		}
	//	}
	//}

	//新算法：以r为半径，从内向外找ByCH20140920
	float radius=pGU->GetColRadius();	//碰撞路径
	int	maxR=100;						//寻找半径默认100
	float step=50;						//寻找步长默认50
	float stepI;						//X步长（用于跳过上轮循环已经判断过的格子）
	for(int r=0;r<maxR;r++)				//从近到远找
	{
		for(int j=-r;j<=r;j++)			//第j行
		{
			//调整stepX跳过中间空格
			if(j==-r||j==r)
			{
				stepI=1;
			}else
			{
				stepI=r*2;
			}
			for(int i=-r;i<=r;i=i+stepI)	//第i列
			{
				float x=pos.x+i*step;
				float y=pos.y+j*step;
				if(FALSE == IfPosImpact(pGU,x,y,radius,ui8MoveGrpID)){
					outPos.x = x;
					outPos.y = y;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

//检查一个点是否会碰撞
bool CSSMoveMgr::IfPosImpact(ISSMoveObject* pMObject, float x,float y, float r, UINT8 groupID){
	ColSphere cs;
	cs.c.x = x;
	cs.c.y = y;
	cs.r = r;
	pMObject->SetStepMoveTarget(cs);
	if(IfStaticImpact(pMObject)){
		return TRUE;
	}
	
	/*
	for(hash_set<ISSMoveObject*>::iterator iter = m_allMObjectSet.begin(); iter != m_allMObjectSet.end(); ++iter){
	ISSMoveObject *pTarget = *iter;
	if(pTarget->GetImpactType_Now() == SSMoveObjectImpactType_NoImpact){
	continue;
	}
	if(pTarget->GetImpactType_Now() == SSMoveObjectImpactType_Group && pTarget->GetGroupID() != groupID){
	continue;
	}
	if(CSSMoveTool::IfImpact(pTarget->GetStepMoveTarget(),cs)){
	return TRUE;
	}
	}
	*/

	if (!pMObject->IfCanImpact()) return FALSE;

	SSMoveObjectImpactType impact_type = pMObject->GetImpactType_Now();
	switch (impact_type)
	{
	case SSMoveObjectImpactType_Group:
		{
			hash_set<ISSMoveObject*>& group_set = m_mObjectMap[groupID];
			for (auto it = group_set.begin(); it!= group_set.end(); ++it)
			{
				if (*it == pMObject) continue;
				if(CSSMoveTool::IfImpact((*it)->GetStepMoveTarget(),cs)){
					return TRUE;
				}
			}
			hash_set<ISSMoveObject*>& group_all_set = m_mObjectMap[SSImpactGruop_All];
			for (auto it = group_all_set.begin(); it!= group_all_set.end(); ++it)
			{
				if (*it == pMObject) continue;
				if(CSSMoveTool::IfImpact((*it)->GetStepMoveTarget(),cs)){
					return TRUE;
				}
			}
		}
		break;
	case SSMoveObjectImpactType_ALL:
		{
			for(hash_set<ISSMoveObject*>::iterator iter = m_allMObjectSet.begin(); iter != m_allMObjectSet.end(); ++iter)
			{
				if(*iter == pMObject) continue;
				if(CSSMoveTool::IfImpact((*iter)->GetStepMoveTarget(),cs)){
					return TRUE;
				}
			}
		}
		break;
	}


	return FALSE;
}

bool CSSMoveMgr::ResetPos(ISSMoveObject *pGU, ColVector pos, bool bIfImpact){
	//以给定坐标为中心，寻找一个可以放下的点，然后将其移动到该位置。
	ColVector outPos;
	if(bIfImpact){
		if(FALSE == GetAroundFreePos(pos,outPos,pGU,pGU->GetGroupID())){
			return FALSE;
		}
	}
	else{
		outPos = pos;
	}
	ColSphere sph = pGU->GetColSphere();
	sph.c = outPos;
	pGU->SetStepMoveTarget(sph);
	pGU->Teleport(GetUTCMiliSecond());
	return TRUE;
}

void CSSMoveMgr::SavePathToCache(UINT64 ID, SIntPoint* path, const UINT16 length)
{
	Assert(m_MapCacheMap.find(ID)==m_MapCacheMap.end());
	Assert(length<=c_n32MaxOrderPathNode-1);
	SIntPoint* newPath = new SIntPoint[c_n32MaxOrderPathNode-1]();
	memcpy(newPath,path,length*sizeof(SIntPoint));
	m_MapCacheMap[ID] = newPath;
}

void CSSMoveMgr::DelAllPathCache()
{
	for (auto it=m_MapCacheMap.begin(); it!=m_MapCacheMap.end(); ++it)
	{
		delete [](it->second);
	}
	m_MapCacheMap.clear();
}

}