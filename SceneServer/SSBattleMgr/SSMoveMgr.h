/*
* file name			:SSMoveMgr.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:August 7 2014
* summary			:
*
*/

#ifndef __SSMoveMgr_h__
#define __SSMoveMgr_h__

#include "stdafx.h"
#include <hash_map>
#include <set>
#include <hash_set>
#include "SSIntersectObject.h"
#include "SSAstar.h"

namespace SceneServer{


const UINT8 c_maxISSMoveObjectNum = 128;
const UINT16	c_maxDynamicBlockNum = 256;

class ISSMoveObject;

//停止移动使用的停止类型，只能停止指定类型的移动,避免移动类型切换时导致的错误停止。
enum EnumAskStopMoveType{
	EnumAskStopMoveType_ALL = 0,
	EnumAskStopMoveType_DIR,
	EnumAskStopMoveType_TAR,
	EnumAskStopMoveType_FORCEMOVE,
};

enum ETriDir{
	ETriDir_LeftUp = 0,
	ETriDir_RightUp,
	ETriDir_RightBelow,
	ETriDir_LeftBelow,
};

struct SIntTri{
	UINT16 x;
	UINT16 y;
	ETriDir eDir;
	SIntTri(UINT16 x,UINT16 y,ETriDir dir): x(x), y(y),eDir(dir){}
	SIntTri(){}
};

class CSSMoveMgr
{
private:
	CSSAStar								m_Astar;//A星寻路类
	hash_set<ISSMoveObject*>				m_mObjectMap[6];//碰撞对象分组树，key为组ID
	hash_set<ISSMoveObject*>				m_allMObjectSet;//所有碰撞对象
	ISSMoveObject*							m_heartBeatTempArray[1024];//心跳时用临时数组
	bool*									m_staticBlockArray;//静态碰撞点信息
	map<UINT64,SIntTri>						m_staticTriBlockMap;//三角形静态碰撞map
	hash_map<UINT64,SIntPoint*>				m_MapCacheMap;//缓存的寻路节点树。保存所有无静态阻挡的寻路数据

	UINT16								m_regionWidth;//地图宽度，单位格子
	UINT16								m_regionHeight;//地图高度，单位格子
	UINT16								m_regionSize;//格子尺寸，正方形

	UINT16								GetRegionIndexX(float x){UINT16 uiX = x/m_regionSize; if(uiX < 0) uiX = 0;if(uiX > m_regionWidth) uiX = m_regionWidth; return uiX;}//普通坐标转格子坐标
	UINT16								GetRegionIndexY(float y){UINT16 uiY = y/m_regionSize; if(uiY < 0) uiY = 0;if(uiY > m_regionHeight) uiY = m_regionHeight; return uiY;}//普通坐标转格子坐标
	UINT32								GetRegionID_INT(UINT16 x, UINT16 y){return x + y * m_regionWidth;}
	UINT32								GetRegionID_FLOAT(float x, float y){return GetRegionID_INT(GetRegionIndexX(x),GetRegionIndexY(y));}

	UINT64								GetPathID(UINT32 startRegionID, UINT32 targetRegionID){return ((UINT64)startRegionID) << 32 | (UINT64)targetRegionID;}//获取路径的key，用于缓存路径
	SIntPoint*							GetPathByCache(UINT64 ID){auto iter = m_MapCacheMap.find(ID);return iter==m_MapCacheMap.end()?NULL:iter->second;}
	void								SavePathToCache(UINT64 ID, SIntPoint* path, const UINT16 length);
	void								DelAllPathCache();

	bool								AskStartCheck(ISSMoveObject* pMObject);//请求位移前置检查
	bool								TestNextStepMove(ISSMoveObject* pMObject, bool bIfCheckStaticBlock = TRUE, bool bIfCheckDynamicBlock = TRUE);//碰撞组检查置于外层(modify)//
	bool								IfDynamicImpact(ISSMoveObject* pMObject,bool pIfCheckNextStep);//检查动态碰撞
	bool								IfDynamicImpact(ISSMoveObject* pMObjectMove,ISSMoveObject* pMObjectBlock, bool pIfCheckNextStep);//检查动态碰撞
	//bool								IfStaticImpact(float x, float y){if(x<0||y<0||(x/100)>=m_regionWidth||(y/100)>=m_regionHeight) return TRUE; return m_staticBlockArray[GetRegionID_FLOAT(x,y)];}//检查静态碰撞
	bool								IfStaticImpact(ISSMoveObject* pMObject);//检查静态碰撞
	void								StopLastStep(ISSMoveObject* pMObject, bool bIfCallBack);//停止移动中对象前，根据时间移动最后那一小步。
	bool								TryMove(ISSMoveObject* pMObject,TIME_MILSEC now, float &moveDist);//尝试移动，返回移动距离。移动失败返回0
	void								FindDynamicBlock(ISSMoveObject* pMObject, UINT32 *dynamicArray, UINT16 &length);//获取一个对象身边的动态阻挡
	void								AddDnnamicBlock(ISSMoveObject* pMObject, ISSMoveObject* pTarget, UINT32 *dynamicArray, UINT16 &length, UINT16 &index);
	ColVector							GetRegionCenter(UINT16 x, UINT16 y){ColVector vec; vec.x = x*m_regionSize+ m_regionSize/2;vec.y = y*m_regionSize+ m_regionSize/2;return vec;}//获取格子中心点坐标
	bool								SetNextMovePoint(ISSMoveObject* pMObject);//将下一步准备移动的点先行计算
	void								CheckTargetMoveStatus(ISSMoveObject* pMObject, float movedLength);//检查是否已经移动到目标点了
	BOOLEAN								TryTurnDir(ISSMoveObject* pMObject);//尝试改变方向来继续移动

public:
	CSSMoveMgr();
	~CSSMoveMgr();

	void								AddStaticBlockInfo(UINT16 x, UINT16 y){m_staticBlockArray[GetRegionID_INT(x,y)] = TRUE;}
	void								AddStaticTriBlockInfo(UINT16 x, UINT16 y, ETriDir dir);
	void								SetMapInfo(UINT16 width, UINT16 height, UINT16 size){m_regionWidth = width;m_regionHeight=height;m_regionSize = size; if(m_staticBlockArray == NULL) m_staticBlockArray = new bool[m_regionWidth*m_regionHeight](); m_Astar.Init(m_regionWidth,m_regionHeight,m_staticBlockArray);}

	INT32								OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);//心跳

	void								AddMoveObject(ISSMoveObject* pMObject, UINT8 ui8GrpID);//添加碰撞体
	void								RemoveMoveObject(ISSMoveObject* pMObject);//移除碰撞体

	bool								GetAroundFreePos(ColVector pos, ColVector &outPos, ISSMoveObject *pGU, UINT8 ui8MoveGrpID);//寻找周围一个可以安放对象的空白空间
	bool								ResetPos(ISSMoveObject *pGU, ColVector pos, bool bIfImpact);//将对象安全的放在一个点周围
	bool								IfPosImpact(ISSMoveObject* pMObject, float x,float y, float r, UINT8 groupID);//检查一个点是否可碰撞

	bool								AskStartMoveDir(ISSMoveObject* pMObject, ColVector dir);//请求朝制定方向移动
	bool								AskStartMoveToTar(ISSMoveObject* pMObject, ColVector pos, bool bIfMoveToBlackPoint, bool bIfFliter);//请求朝制定位置移动
	bool								AskStartMoveForced(ISSMoveObject* pMObject, ColVector dir, FLOAT speed, bool bIfImpact);//请求强制移动
	bool								AskStopMoveObject(ISSMoveObject* pMObject,EnumAskStopMoveType type);//请求停止移动。根据枚举值不同，只能停止指定类型的移动。
};

}

#endif