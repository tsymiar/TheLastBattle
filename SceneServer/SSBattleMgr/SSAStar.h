 #include <hash_set>


#ifndef SSATAR_H_
#define SSATAR_H_

namespace SceneServer{

//格子状态枚举
enum ENodeState{
	eNodeStae_None = 0,
	eNodeState_Open,//开放，可走
	eNodeState_Close,//关闭，不可走
};

//用于返回寻路结果的数据结构
struct SFindPathInfoNew{
	SIntPoint			psStartIntPoint;//起点
	SIntPoint			psTargetIntPoint;//目标终点
	UINT32				*pDynamicBlockArray;//动态阻挡数组
	bool				ifToTarget;//是否尝试走到目标点，False代表走到目标点隔壁格子即可
	bool				ifFliter;//是否平滑
	SIntPoint*			psPathBuff;//返回的结果点数组
	INT32				n32PathNodeNum;//返回的结果点数组长度

	SFindPathInfoNew():
		psStartIntPoint(0,0),
		psTargetIntPoint(0,0),
		pDynamicBlockArray(NULL),
		ifToTarget(FALSE),
		ifFliter(FALSE),
		psPathBuff(NULL),
		n32PathNodeNum(0)
	{
		
	}
};

//用于保存整个地图的格子状态
struct SAStarRegion{
	UINT16 un16RunTimeID;
	UINT16 un16XRegionIdx;
	UINT16 un16ZRegionIdx;
	UINT16 un16AroundGONum;
	bool	bStaticBlock;
	bool	bDynamicBlock;
	INT8	n8NodeState;
	UINT16	un16GValue;
	UINT16	un16HValue;
	SAStarRegion*	psParentNode;
	SAStarRegion*	psNextOne;
	SAStarRegion():
	un16RunTimeID(0),
	un16XRegionIdx(0),
	un16ZRegionIdx(0),
	un16AroundGONum(0),
	bStaticBlock(false),
	bDynamicBlock(false),
	un16GValue(0),
	un16HValue(0),
	psParentNode(NULL),
	n8NodeState(eNodeStae_None),
	psNextOne(NULL)
	{}

	UINT16	GetFValue(){
		return un16GValue + un16HValue;
	}
};


struct	SAStarRegionBuff{
	INT32		n32CurRegionNum;
	UINT16		un16CurRunTimeID;
	SAStarRegion	*psBeginRegion;

	SAStarRegionBuff():
	un16CurRunTimeID(0),
	n32CurRegionNum(0),
	psBeginRegion(NULL)
	{}

	void	Clear(){
		un16CurRunTimeID = 0;
		n32CurRegionNum = 0;
		psBeginRegion = NULL;
	}

	INT32			Push(SAStarRegion *psRegion){
		psRegion->psNextOne = psBeginRegion;
		psBeginRegion = psRegion;
		n32CurRegionNum++;
		if (0 == un16CurRunTimeID){
			un16CurRunTimeID = psRegion->un16RunTimeID;
		}
		Assert(un16CurRunTimeID == psRegion->un16RunTimeID);
		return eNormal;
	}

	SAStarRegion*	Pop(){
		if (0 >= n32CurRegionNum){
			return NULL;
		}

		SAStarRegion *psTempRegion = psBeginRegion;
		psBeginRegion = psTempRegion->psNextOne;
		n32CurRegionNum--;
		psTempRegion->psNextOne = NULL;
		Assert (un16CurRunTimeID == psTempRegion->un16RunTimeID);
		return psTempRegion;
	}
};

//用二叉堆实现的计算用节点数据
//逻辑比较复杂，请参考文章：http://www.apkbus.com/android-58533-1-1.html
struct	SAStarRegionBuffBinaryTree{
	static const	UINT16 c_maxLength = 100;
	INT32		n32CurRegionNum;//当前节点数量
	SAStarRegion*	data[c_maxLength];//所有节点

	SAStarRegionBuffBinaryTree():
		n32CurRegionNum(0)
	{
		Clear();
	}

	void	Clear(){
		n32CurRegionNum = 0;
		memset(data,0,c_maxLength*sizeof(SAStarRegion*));
	}

	//对调两个节点
	void			Exchange(UINT16 pos1, UINT16 pos2){
		SAStarRegion* pTemp = data[pos1];
		data[pos1] = data[pos2];
		data[pos2] = pTemp;
	}

	//添加一个新节点
	bool			Push(SAStarRegion *psRegion){
		if(n32CurRegionNum >= c_maxLength){
			return FALSE;
		}
		n32CurRegionNum++;
		data[n32CurRegionNum] = psRegion;
		if(n32CurRegionNum <= 1){
			return TRUE;
		}
		//开始循环替换位置
		UINT16	un16Pos = n32CurRegionNum;
		while(TRUE){
			UINT16 un16TempPos = un16Pos/2;
			if(data[un16TempPos]->GetFValue() > data[un16Pos]->GetFValue()){
				Exchange(un16TempPos,un16Pos);
				un16Pos = un16TempPos;
				if(un16Pos <= 1){
					break;
				}
			}
			else{
				break;
			}
		}
		return TRUE;
	}

	//获取最小F值的节点
	SAStarRegion*	PopMiniFValue(){
		if(n32CurRegionNum <= 0){
			return NULL;
		}
		SAStarRegion* returnValue = data[1];
		if(n32CurRegionNum <= 1){
			data[1] = NULL;
			n32CurRegionNum--;
			return returnValue;
		}
		data[1] = data[n32CurRegionNum];
		data[n32CurRegionNum] = NULL;
		n32CurRegionNum--;
		//开始循环替换位置
		UINT16	un16Pos = 1;
		while(TRUE){
			UINT16 un16TempPos1 = un16Pos*2;
			UINT16 un16TempPos2 = un16Pos*2+1;
			bool bIfNeedChange = FALSE;
			UINT16 un16ChangePos = -1;
			if(un16TempPos1 <= n32CurRegionNum && data[un16Pos]->GetFValue() > data[un16TempPos1]->GetFValue()){
				bIfNeedChange = TRUE;
				un16ChangePos = un16TempPos1;
			}
			if(un16TempPos2 <= n32CurRegionNum && data[un16Pos]->GetFValue() > data[un16TempPos2]->GetFValue()){
				if(bIfNeedChange){
					//第一个格子需要替换
					if(data[un16TempPos1]->GetFValue() > data[un16TempPos2]->GetFValue()){
						un16ChangePos = un16TempPos2;
					}
				}
				else{
					bIfNeedChange = TRUE;
					un16ChangePos = un16TempPos2;
				}
			}

			if(bIfNeedChange){
				//需要替换
				Exchange(un16Pos , un16ChangePos);
				un16Pos = un16ChangePos;
			}
			else{
				break;
			}
		}
		return returnValue;
	}
};

class CSSAStar
{
private:
	UINT16 m_un16RunTimeID;
	UINT16 m_un16XRegionNum;//宽度
	UINT16 m_un16ZRegionNum;//高度
	UINT32	m_un32TotalRegionNum;//总数
	SAStarRegion *m_pcRegionList;//格子列表
	bool*	m_staticBlockArray;//静态阻挡列表

	SAStarRegionBuffBinaryTree	m_sOpenRegionBuff;
	SAStarRegionBuff			m_sTempRegionBuff;

	bool			IfAStarImpact(UINT16 x,UINT16 y,UINT32 *pDynamicBlockArray);


public:
	CSSAStar();
	~CSSAStar();
	void			Init(UINT16	width, UINT16 height, bool* staticBlockArray);

	//核心寻路函数
	INT32			FindPath(SFindPathInfoNew &rsFindPathInfo);
	//根据坐标获取格子
	SAStarRegion*	GetAStarRegionByIdx(UINT32 un32Idx);
	SAStarRegion*	GetAStarRegionByIdx(UINT16 un16XIdx, UINT16 un16ZIdx);
	//获取一个格子周围所有格子，最多8个
	INT32			GetAroundRegion(SAStarRegion *pcCenterRegion, SAStarRegion *apsAroundRegion[c_n32AroundAStarRegionNum]);
	//通过两个格子获取相对方向
	EIntDir			GetRegionDir(SAStarRegion *cpsBeginRegion,SAStarRegion *&cpsEndRegion);
};

}

#endif

