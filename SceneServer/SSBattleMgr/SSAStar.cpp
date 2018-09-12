#include "stdafx.h"
#include "SSAStar.h"
#include "SSBattle.h"
#include "SSGameObject.h" 
#include "SSNPC.h"

namespace SceneServer{
CSSAStar::CSSAStar():
	m_un16RunTimeID(0),
	m_un16XRegionNum(0),
	m_un16ZRegionNum(0),
	m_un32TotalRegionNum(0),
	m_pcRegionList(NULL),
	m_staticBlockArray(NULL)
{
}

CSSAStar::~CSSAStar(){
	if (NULL != m_pcRegionList){
		delete[] m_pcRegionList;
	}
}

void CSSAStar::Init(UINT16	width, UINT16 height, bool* staticBlockArray){
	m_un16XRegionNum = width;
	m_un16ZRegionNum = height;
	m_un32TotalRegionNum = m_un16ZRegionNum * m_un16XRegionNum;
	m_staticBlockArray = staticBlockArray;
	if(m_pcRegionList != NULL){
		delete m_pcRegionList;
		m_pcRegionList = NULL;
	}
	m_pcRegionList = new SAStarRegion[m_un32TotalRegionNum];
	//set region list.
	for (UINT16 i = 0; i < m_un16ZRegionNum; i++){
		for (UINT16 j = 0; j < m_un16XRegionNum; j++){
			UINT32 un32Idx = j + i * m_un16XRegionNum;
			m_pcRegionList[un32Idx].un16XRegionIdx = j;
			m_pcRegionList[un32Idx].un16ZRegionIdx = i;
		}
	}
}


SAStarRegion*	CSSAStar::GetAStarRegionByIdx(UINT32 un32Idx){
	if (un32Idx >= m_un32TotalRegionNum){
		return NULL;
	}
	SAStarRegion *psTempRegion = &m_pcRegionList[un32Idx];
	if (psTempRegion->un16RunTimeID != m_un16RunTimeID){
		psTempRegion->psParentNode = NULL;
		psTempRegion->un16GValue = 0;
		psTempRegion->un16HValue = 0;
		psTempRegion->n8NodeState = eNodeStae_None;
		psTempRegion->un16RunTimeID = m_un16RunTimeID;
		psTempRegion->un16AroundGONum = 0;
	}
	return psTempRegion;
}

SAStarRegion*	CSSAStar::GetAStarRegionByIdx(UINT16 un16XIdx, UINT16 un16ZIdx){
	UINT32 un32Idx = un16XIdx + un16ZIdx * m_un16XRegionNum;
	return GetAStarRegionByIdx(un32Idx);
}

INT32	CSSAStar::GetAroundRegion(SAStarRegion *pcCenterRegion, SAStarRegion *apsAroundRegion[c_n32AroundAStarRegionNum]){
	if (NULL == pcCenterRegion){
		return eEC_NullPointer;
	}
	apsAroundRegion[0] = GetAStarRegionByIdx(pcCenterRegion->un16XRegionIdx - 1, pcCenterRegion->un16ZRegionIdx);
	apsAroundRegion[1] = GetAStarRegionByIdx(pcCenterRegion->un16XRegionIdx, pcCenterRegion->un16ZRegionIdx - 1);
	apsAroundRegion[2] = GetAStarRegionByIdx(pcCenterRegion->un16XRegionIdx + 1, pcCenterRegion->un16ZRegionIdx);
	apsAroundRegion[3] = GetAStarRegionByIdx(pcCenterRegion->un16XRegionIdx, pcCenterRegion->un16ZRegionIdx + 1);
	apsAroundRegion[4] = GetAStarRegionByIdx(pcCenterRegion->un16XRegionIdx - 1, pcCenterRegion->un16ZRegionIdx - 1);
	apsAroundRegion[5] = GetAStarRegionByIdx(pcCenterRegion->un16XRegionIdx + 1, pcCenterRegion->un16ZRegionIdx - 1);
	apsAroundRegion[6] = GetAStarRegionByIdx(pcCenterRegion->un16XRegionIdx - 1, pcCenterRegion->un16ZRegionIdx + 1);
	apsAroundRegion[7] = GetAStarRegionByIdx(pcCenterRegion->un16XRegionIdx + 1, pcCenterRegion->un16ZRegionIdx + 1);
	return eNormal;
}

EIntDir	CSSAStar::GetRegionDir(SAStarRegion *cpsBeginRegion,SAStarRegion *&cpsEndRegion){
	return CFunction::GetIntDirByIndx(cpsBeginRegion->un16XRegionIdx, cpsBeginRegion->un16ZRegionIdx, cpsEndRegion->un16XRegionIdx, cpsEndRegion->un16ZRegionIdx);
}

INT32	CSSAStar::FindPath(SFindPathInfoNew &rsFindPathInfo){
	//check parameters.
	if (NULL == rsFindPathInfo.psPathBuff || 0 >= rsFindPathInfo.n32PathNodeNum){
		return eEC_NullPointer;
	}
	m_un16RunTimeID++;
	//get begin region and target region.
	SAStarRegion *psBeginRegion = GetAStarRegionByIdx(rsFindPathInfo.psStartIntPoint.n32XIdx,rsFindPathInfo.psStartIntPoint.n32ZIdx);
	if (NULL == psBeginRegion){
		return eEC_InvalidRegionID;
	}

	SAStarRegion *psTargetRegion = GetAStarRegionByIdx(rsFindPathInfo.psTargetIntPoint.n32XIdx,rsFindPathInfo.psTargetIntPoint.n32ZIdx);
	if (psTargetRegion == NULL || psBeginRegion == psTargetRegion){
		rsFindPathInfo.n32PathNodeNum = 0;
		return eNormal;
	}

	TIME_MILSEC tBeginMilsec = GetUTCMiliSecond();

	m_sOpenRegionBuff.Clear();
	m_sTempRegionBuff.Clear();

	psBeginRegion->un16GValue = 0;
	psBeginRegion->un16HValue = 0;
	psBeginRegion->n8NodeState = eNodeState_Open;
	m_sOpenRegionBuff.Push(psBeginRegion);

	SAStarRegion *psAroundRegion = NULL;
	SAStarRegion *psFindEndRegion = NULL;
	INT32 n32TestRegionTimes = 0;
	INT32 n32MaxOpenNode = 0;
	INT32 n32MinDist = -1;
	SAStarRegion* psNearestRegion = NULL;
	SAStarRegion *apsAroundRegionList[c_n32AroundAStarRegionNum] = {NULL};
	UINT32 un32StepSize = 10;
	EIntDir eIntDirToCurRegion;


	UINT16 un16CurFValue = 0;			
	UINT16 un16NewGValue = 0;
	UINT16 un16TarHValueInX = 0;
	UINT16 un16TarHValueInZ = 0;
	UINT16 un16Mini = 0;
	UINT16 un16Max = 0;
	UINT16 un16NewHValue = 0;
	UINT16 un16NewFValue = 0;


	bool bIfHasDynBlock = FALSE;

	//begin find path loop.
	while (true){
		SAStarRegion *psCurRegion = m_sOpenRegionBuff.PopMiniFValue();
		if (NULL == psCurRegion){
			break;
		}
		//at max test about 10000 regions.
		if (10000 < n32TestRegionTimes){
			break;
		}
		Assert (eNodeState_Open == psCurRegion->n8NodeState);

		GetAroundRegion(psCurRegion, apsAroundRegionList);

		for (INT32 i = 0; i < c_n32AroundAStarRegionNum; i++){
			n32TestRegionTimes++;
			psAroundRegion = apsAroundRegionList[i];
			if (NULL == psAroundRegion){
				continue;
			}
			//如果到达目标点，寻路结束
			if (psAroundRegion == psTargetRegion){
				psAroundRegion->psParentNode = psCurRegion;
				psAroundRegion->n8NodeState = eNodeState_Close;
				psFindEndRegion = psAroundRegion;
				break;
			}
			//如果该格子已经被关闭了(走过且证明走不通)，略过
			if (eNodeState_Close == psAroundRegion->n8NodeState){
				continue;
			}

			//检查碰撞
			bIfHasDynBlock = IfAStarImpact(psAroundRegion->un16XRegionIdx,psAroundRegion->un16ZRegionIdx,rsFindPathInfo.pDynamicBlockArray);
			if(bIfHasDynBlock){
				continue;
			}

			//检查是否与目标格子相邻
			UINT32 dist = abs(psAroundRegion->un16XRegionIdx - rsFindPathInfo.psTargetIntPoint.n32XIdx) + abs(psAroundRegion->un16ZRegionIdx - rsFindPathInfo.psTargetIntPoint.n32ZIdx);
			if(n32MinDist < 0 || n32MinDist > dist){
				n32MinDist = dist;
				psNearestRegion = psAroundRegion;
			}
			if(FALSE == rsFindPathInfo.ifToTarget){
				//不是必须走到目标格子上,则检查是否相邻
				if( dist <= 2){
					psAroundRegion->psParentNode = psCurRegion;
					psAroundRegion->n8NodeState = eNodeState_Close;
					psFindEndRegion = psAroundRegion;
					break;
				}
			}

			//开始计算F值

			//相邻格子的G增加值为10，斜线的为14(其实应该是根号2，14.1423，简化为整数加快速度，但是会导致寻路倾向于走斜线)
			un32StepSize = 10;
			eIntDirToCurRegion = GetRegionDir(psCurRegion, psAroundRegion);

			if (eIntDirToCurRegion % 2 == 1){
				un32StepSize = 14;
			}

			un16CurFValue = psAroundRegion->un16GValue + psAroundRegion->un16HValue;			
			un16NewGValue = psCurRegion->un16GValue + un32StepSize;
			un16TarHValueInX = 0;
			if (psTargetRegion->un16XRegionIdx > psAroundRegion->un16XRegionIdx){
				un16TarHValueInX = psTargetRegion->un16XRegionIdx - psAroundRegion->un16XRegionIdx;
			}else{
				un16TarHValueInX = psAroundRegion->un16XRegionIdx - psTargetRegion->un16XRegionIdx;
			}
			un16TarHValueInZ = 0;
			if (psTargetRegion->un16ZRegionIdx > psAroundRegion->un16ZRegionIdx){
				un16TarHValueInZ = psTargetRegion->un16ZRegionIdx - psAroundRegion->un16ZRegionIdx;
			}else{
				un16TarHValueInZ = psAroundRegion->un16ZRegionIdx - psTargetRegion->un16ZRegionIdx;
			}
			//将最新的路点加入到所有可行走格子中
			un16Mini = MINI(un16TarHValueInX, un16TarHValueInZ);
			un16Max = MAX(un16TarHValueInX, un16TarHValueInZ);
			un16NewHValue = un16Mini * 14 + (un16Max - un16Mini) * 10;
			un16NewFValue = un16NewGValue + un16NewHValue;
			if (0 == un16CurFValue || un16CurFValue > un16NewFValue){
				psAroundRegion->un16GValue = un16NewGValue;
				psAroundRegion->un16HValue = un16NewHValue;
				psAroundRegion->psParentNode = psCurRegion;
				if (eNodeState_Open != psAroundRegion->n8NodeState){
					psAroundRegion->n8NodeState = eNodeState_Open;

					psAroundRegion->un16AroundGONum = 0;

					if (0 < psAroundRegion->un16AroundGONum){
						psAroundRegion->un16HValue += psAroundRegion->un16AroundGONum * un32StepSize;
					}
					m_sOpenRegionBuff.Push(psAroundRegion);
					if (n32MaxOpenNode < m_sOpenRegionBuff.n32CurRegionNum){
						n32MaxOpenNode = m_sOpenRegionBuff.n32CurRegionNum;
					}
				}
			}
		}

		//set current region to close.
		psCurRegion->n8NodeState = eNodeState_Close;		
		if (NULL != psFindEndRegion){
			break;
		}
	}

	if (NULL == psFindEndRegion){
		if(psNearestRegion != NULL){
			//返回最接近的一个点,如果最终没有到达目标点的话
			psFindEndRegion = psNearestRegion;
		}
		else{
			rsFindPathInfo.n32PathNodeNum = 0;
			return eEC_CannotFindFullPathNode;
		}
	}

	m_sTempRegionBuff.Clear();
	bool bBeginPushed = false;
	psAroundRegion = psFindEndRegion;
	while (NULL != psAroundRegion){
		if (psAroundRegion == psBeginRegion){
			bBeginPushed = true;
		}
		m_sTempRegionBuff.Push(psAroundRegion);
		psAroundRegion = psAroundRegion->psParentNode;
		if (m_sTempRegionBuff.n32CurRegionNum >= n32TestRegionTimes){
			break;
		}
	}

	//将起点也加入到结果序列中
	if (!bBeginPushed){
		m_sTempRegionBuff.Push(psBeginRegion);
	}

	//整理节点，将多个连续的方向相同的节点整合为最后一个。
	EIntDir eCurDir = (EIntDir)-1;
	INT32 n32FillNodeNum = 0;
	SAStarRegion *psCurRegionNode = m_sTempRegionBuff.Pop();
	SAStarRegion *psNextRegionNode = NULL;
	while (NULL != psCurRegionNode){
		psNextRegionNode = m_sTempRegionBuff.Pop();
		if (NULL == psNextRegionNode){
			rsFindPathInfo.psPathBuff[n32FillNodeNum].n32XIdx = psCurRegionNode->un16XRegionIdx;
			rsFindPathInfo.psPathBuff[n32FillNodeNum].n32ZIdx = psCurRegionNode->un16ZRegionIdx;
			n32FillNodeNum++;
			break;
		}else{
			EIntDir eTempDir = GetRegionDir(psCurRegionNode, psNextRegionNode);
			if (-1 == eCurDir){
				eCurDir = eTempDir;
			}
			if (eCurDir != eTempDir){
				eCurDir = eTempDir;
				rsFindPathInfo.psPathBuff[n32FillNodeNum].n32XIdx = psCurRegionNode->un16XRegionIdx;
				rsFindPathInfo.psPathBuff[n32FillNodeNum].n32ZIdx = psCurRegionNode->un16ZRegionIdx;
				n32FillNodeNum++;
			}else if(psNextRegionNode == psFindEndRegion){
				rsFindPathInfo.psPathBuff[n32FillNodeNum].n32XIdx = psNextRegionNode->un16XRegionIdx;
				rsFindPathInfo.psPathBuff[n32FillNodeNum].n32ZIdx = psNextRegionNode->un16ZRegionIdx;
				n32FillNodeNum++;
				break;
			}
		}
		psCurRegionNode = psNextRegionNode;
		if (n32FillNodeNum >= rsFindPathInfo.n32PathNodeNum){
			break;
		}
		
	}

	rsFindPathInfo.n32PathNodeNum = n32FillNodeNum;

	if (0 >= rsFindPathInfo.n32PathNodeNum){
		Assert (0 >= m_sOpenRegionBuff.n32CurRegionNum);
		return eEC_CannotFindFullPathNode;
	}

	//增加终点
	INT32 finalIndex = rsFindPathInfo.n32PathNodeNum - 1;
	if(rsFindPathInfo.psPathBuff[finalIndex].n32XIdx != rsFindPathInfo.psTargetIntPoint.n32XIdx || rsFindPathInfo.psPathBuff[finalIndex].n32ZIdx != rsFindPathInfo.psTargetIntPoint.n32ZIdx){
		//finalIndex++;
		rsFindPathInfo.n32PathNodeNum = finalIndex+1;
		Assert(rsFindPathInfo.n32PathNodeNum<=c_n32MaxOrderPathNode-1);
		rsFindPathInfo.psPathBuff[finalIndex].n32XIdx = rsFindPathInfo.psTargetIntPoint.n32XIdx;
		rsFindPathInfo.psPathBuff[finalIndex].n32ZIdx = rsFindPathInfo.psTargetIntPoint.n32ZIdx;
	}

	//平滑算法
	//算法的主要思想是，不断从两头检查两个点中间经过的所有节点，判断其是否有经过阻挡点。如果没有，则删除这两个点中间的所有节点
	if(rsFindPathInfo.ifFliter && rsFindPathInfo.n32PathNodeNum > 1){
		SIntPoint* fliterArray[32];
		memset(fliterArray,0,32*sizeof(SIntPoint*));
		
		INT8 len = rsFindPathInfo.n32PathNodeNum + 1;
		fliterArray[0] = &rsFindPathInfo.psStartIntPoint;
		for(int i=1;i<len;i++){
			fliterArray[i] = &rsFindPathInfo.psPathBuff[i-1];
		}

		INT32 i = 0;
		INT32 j = len -1;

		while (j-i>1)
		{
			while (j-i>1)
			{
				bool bIfConnect = TRUE;
				//检查阻挡
				INT32 diffX = fliterArray[j]->n32XIdx - fliterArray[i]->n32XIdx;
				INT32 diffY = fliterArray[j]->n32ZIdx - fliterArray[i]->n32ZIdx;
				INT32 stepNum = pow(diffX * diffX + diffY * diffY,0.5f);
				float stepX = (float)diffX / stepNum;
				float stepY = (float)diffY / stepNum;
				for(int stepIndex = 1;stepIndex<stepNum-1;stepIndex++){
					UINT16 tempX = fliterArray[i]->n32XIdx + stepIndex * stepX;
					UINT16 tempY = fliterArray[i]->n32ZIdx + stepIndex * stepY;
					if(IfAStarImpact(tempX,tempY,rsFindPathInfo.pDynamicBlockArray)
						|| IfAStarImpact(tempX,tempY+1,rsFindPathInfo.pDynamicBlockArray)
						|| IfAStarImpact(tempX+1,tempY,rsFindPathInfo.pDynamicBlockArray)
						|| IfAStarImpact(tempX+1,tempY+1,rsFindPathInfo.pDynamicBlockArray)
						){
						bIfConnect = FALSE;
						break;
					}
				}


				if (bIfConnect)
				{
					INT8 removeLen = j-i-1;
					memset(fliterArray + i + 1,0,removeLen*sizeof(SIntPoint*));
					memcpy(fliterArray + i + 1,fliterArray+j,(len-j)*sizeof(SIntPoint*));
					len -= removeLen;
					break;
				}
				else{
					j--;
				}
			}
			i++;
			j=len-1;  
		}
		//返回结果
		
		for(int i=1;i<len;i++){
			rsFindPathInfo.psPathBuff[i-1].n32XIdx = fliterArray[i]->n32XIdx;
			rsFindPathInfo.psPathBuff[i-1].n32ZIdx = fliterArray[i]->n32ZIdx;
		}
		rsFindPathInfo.n32PathNodeNum = len-1;
		Assert(rsFindPathInfo.n32PathNodeNum<=c_n32MaxOrderPathNode-1);
	}
	

	return eNormal;
}

bool CSSAStar::IfAStarImpact(UINT16 x,UINT16 y,UINT32 *pDynamicBlockArray){
	UINT32 un32Idx = x + y * m_un16XRegionNum;
	if(m_staticBlockArray[un32Idx]){
		return TRUE;
	}
	for(int dynIdx=0;dynIdx<100;dynIdx++){
		UINT32 tempIdx = pDynamicBlockArray[dynIdx];
		if(tempIdx == 0){
			return FALSE;
		}
		if(un32Idx == tempIdx){
			return TRUE;
		}
	}
	return FALSE;
}
}