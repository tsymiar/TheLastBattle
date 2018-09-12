/*
* file name			:SSight.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:July 15 2014
* summary			:
*
*/

#ifndef __SSight_h__
#define __SSight_h__

#include "stdafx.h"
#include <hash_set>
#include "SSObjectPool.h"

namespace SceneServer{

enum ESightType{
	ESightType_Normal,
	ESightType_OB,
};


class CSSightLight;
class ISSightLight;
class ISSightObject;

class CSSight
{
private:
	hash_set<ISSightLight*>					m_sightLishtMap;//视野提供者树
	vector<CSSUser*>						m_watcherVec;//观察者

public:
	static const UINT16						c_MaxWaterNumInSight = 5;//最大观察者数量
	ESightType								m_sightType;//视野类型(普通或OB)

public:
	CSSight();
	~CSSight();

	void						Clear();
	bool						HasWatcher(){return !m_watcherVec.empty();}
	bool						IsInSight(ISSightObject* pSObject);
	void						AddSightLight(ISSightLight* pISightLight, ISSightObject* pAsSightObj);
	void						RemoveSightLight(ISSightLight* pISightLight, ISSightObject* pAsSightObj);
	bool						AddWatcher(CSSUser* pWatcher);
	void						RemoveWatcher(CSSUser* pWatcher){for(vector<CSSUser*>::iterator iter = m_watcherVec.begin(); iter != m_watcherVec.end(); ++iter){if(*iter == pWatcher){m_watcherVec.erase(iter); return;}}}
	vector<CSSUser*>&			GetWatchers(){return m_watcherVec;}
};

}

#endif