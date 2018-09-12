#include "SSightMgr.h"
#include "SSight.h"
#include "SSightObject.h"
namespace SceneServer{
CSSightMgr::CSSightMgr():
	m_lastHeartBeatTime(0)
{
	
}

CSSightMgr::~CSSightMgr(){
	for(auto iter = m_SightVec.begin(); iter != m_SightVec.end(); iter++){
		delete (*iter);
	}
}
//创建、添加到视野系统并返回一个视野
CSSight* CSSightMgr::CreateSight(){
	CSSight* pSight = new CSSight;
	m_SightVec.push_back(pSight);
	return pSight;
}
//移除视野
void CSSightMgr::RemoveSight(CSSight* pSight){
	for(auto iter = m_SightVec.begin(); iter!=m_SightVec.end(); ++iter){
		if(*iter == pSight){
			delete pSight;
			m_SightVec.erase(iter);
			return;
		}
	}
	Assert(false);
}

//向一个视野中添加一个观察者
void CSSightMgr::AddWatcherAndCheck(CSSight* pSight, CSSUser* pWatcher){
	if(false == pSight->AddWatcher(pWatcher)){
		return;
	}
	vector<CSSUser*> pWatcherVec;	
	pWatcherVec.push_back(pWatcher);
	//检查所有对象是否在视野中。如果是，需要发送Appear消息给这个新观察者
	for(auto iter = m_SightObjectMap.begin(); iter != m_SightObjectMap.end(); ++iter){
		SSightObjectInfo& pSOInfo = iter->second;
		ISSightObject* pObject = iter->first;
		if(false == pSight->HasWatcher())
			continue;
		if(pObject->GetOwner()==pSight ||
			pSight->IsInSight(pObject)){
			pObject->OnAppearInSight(pWatcherVec);
			for(auto iter = pSOInfo.childSet.begin(); iter != pSOInfo.childSet.end();++iter){
				(*iter)->OnAppearInSight(pWatcherVec);
			}
		}
	}
}

//添加视野对象
void CSSightMgr::AddSightObject(ISSightObject* pObject){
	SSightObjectInfo info;
	//向所有能看到该视野对象的观察者发送Appear消息
	for(auto iter = m_SightVec.begin(); iter != m_SightVec.end(); ++iter){
		CSSight* pSight = *iter;
		if(pObject->GetOwner()==pSight ||
			pSight->IsInSight(pObject)){
			info.AddSight(pSight);
			if(false == pSight->HasWatcher()) 
				continue;
			pObject->OnAppearInSight(pSight->GetWatchers());
		}
	}
	m_SightObjectMap.insert(std::make_pair(pObject, info));
}

//添加子视野对象。子视野对象附属在视野对象上，不单独计算是否在视野中出现。当其父对象调用Appear和Disapear接口时，其需要同步调用
void CSSightMgr::AddChildSightObject(ISSightObject* pFather, ISSightObject* pChild){
	auto iter = m_SightObjectMap.find(pFather);
	if(iter == m_SightObjectMap.end()){
		return;
	}
	SSightObjectInfo* pInfo = &(iter->second);
	pInfo->childSet.insert(pChild);
	for(int i=0;i<pInfo->c_maxSightSize;++i){
		CSSight* pSight = pInfo->sightArray[i];
		if(pSight == NULL) 
			continue;
		if(false == pSight->HasWatcher()) 
			continue;
		pChild->OnAppearInSight(pSight->GetWatchers());
	}
}

//移除视野对象
void CSSightMgr::RemoveSightObject(ISSightObject* pObject){
	auto iter = m_SightObjectMap.find(pObject);
	if(m_SightObjectMap.end() != iter){
		SSightObjectInfo *pInfo = &iter->second;
		//发送对应的Disappear消息给其观察者，并对其下属的子视野对象做同样的发送和移除处理。
		for(int i=0;i<pInfo->c_maxSightSize;i++){
			CSSight* pSight = pInfo->sightArray[i];
			if(pSight == NULL) continue;
			for(auto childIter = pInfo->childSet.begin();childIter != pInfo->childSet.end(); ++childIter){
				(*childIter)->OnDisappearInSight(pSight->GetWatchers());
			}
			pObject->OnDisappearInSight(pSight->GetWatchers());
		}
		m_SightObjectMap.erase(iter);
	}
}

void CSSightMgr::RemoveChildSightObject(ISSightObject* pFather, ISSightObject* pChild){
	auto iter = m_SightObjectMap.find(pFather);
	if(m_SightObjectMap.end() != iter){
		ISSightObject* pObject = iter->first;
		SSightObjectInfo *pInfo = &iter->second;
		for(int i=0;i<pInfo->c_maxSightSize;i++){
			CSSight* pSight = pInfo->sightArray[i];
			if(pSight == NULL) 
				continue;
			pChild->OnDisappearInSight(pSight->GetWatchers());
		}
		pInfo->childSet.erase(pChild);
	}
}

//获取所有能看到指定视野对象的观察者并返回
void CSSightMgr::GetWatcher(ISSightObject* pObject, CSSUser** pWatcherArray, UINT16& length){
	int index = 0;
	auto iter = m_SightObjectMap.find(pObject);
	if(m_SightObjectMap.end() != iter){
		ISSightObject* pObject = iter->first;
		SSightObjectInfo *pInfo = &iter->second;
		for(int i=0;i<pInfo->c_maxSightSize;i++){
			CSSight* pSight = pInfo->sightArray[i];
			if(pSight == NULL) continue;
			vector<CSSUser*>& pTempWatcherVec = pSight->GetWatchers();
			for(auto watcherIter = pTempWatcherVec.begin(); watcherIter != pTempWatcherVec.end(); watcherIter++){
				pWatcherArray[index++] = *watcherIter;
			}
		}
	}
	length = index;
}

//获取整个视野系统中所有的观察者
void CSSightMgr::GetAllWatcher(vector<CSSUser*>& pWatcherVec){
	for(auto sightIter = m_SightVec.begin(); sightIter != m_SightVec.end(); sightIter++){
		CSSight* pSight = *sightIter;
		vector<CSSUser*>& pTempWatcherVec = pSight->GetWatchers();
		for(auto watcherIter = pTempWatcherVec.begin(); watcherIter != pTempWatcherVec.end(); watcherIter++){
			pWatcherVec.push_back(*watcherIter);
		}
	}
}

//获取指定视野类型的所有的观察者
void CSSightMgr::GetWatcherBySightType(ESightType eSightType, vector<CSSUser*>& pWatcherVec){
	for(auto sightIter = m_SightVec.begin(); sightIter != m_SightVec.end(); ++sightIter){
		CSSight* pSight = *sightIter;
		if(pSight->m_sightType == eSightType){
			pWatcherVec = pSight->GetWatchers();
			break;
		}
	}
}
//心跳
INT32 CSSightMgr::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){

	ProfileInScope lProfileInScope(StaticsType_Sight);

	//每500毫秒计算一次非敏感对象的视野
	bool updateAllSight = FALSE;
	if(tUTCMilsec - m_lastHeartBeatTime > 500){
		updateAllSight = TRUE;
		m_lastHeartBeatTime = tUTCMilsec;
	}
	//检查所有的视野对象。当其出现或者消失在视野中的时候，发送对应的消息
	for(auto iter = m_SightObjectMap.begin(); iter != m_SightObjectMap.end(); ++iter){
		for(auto sightIter = m_SightVec.begin(); sightIter != m_SightVec.end(); ++sightIter){
			if(updateAllSight || iter->first->GetSightObjectType() == ESSSightObjectType_Sensitive){
				CheckObjectInSight(*sightIter, iter->first, iter->second);
			}
		}
	}

	return eNormal;
}

void CSSightMgr::CheckObjectInSight(CSSight* pSight, ISSightObject* pObject, SSightObjectInfo& soInfo){
	if(false == pSight->HasWatcher()){
		return;
	}

	// 跳过判断自己在自己视野中//
	if (pObject->GetOwner()==pSight){
		return;
	}

	//如果对象在视野中，然后以前又不在该视野中，发送Appear消息
	if(pSight->IsInSight(pObject)){
		if(false == soInfo.IfHasSight(pSight)){
			soInfo.AddSight(pSight);
			pObject->OnAppearInSight(pSight->GetWatchers());
			for(auto iter = soInfo.childSet.begin(); iter != soInfo.childSet.end();++iter){
				(*iter)->OnAppearInSight(pSight->GetWatchers());
			}
		}
	}
	//如果对象不在视野中，以前却在视野中，发送Disappear消息
	else{
		if(true == soInfo.IfHasSight(pSight)){
			soInfo.RemoveSight(pSight);
			pObject->OnDisappearInSight(pSight->GetWatchers());
			for(auto iter = soInfo.childSet.begin(); iter != soInfo.childSet.end();++iter){
				(*iter)->OnDisappearInSight(pSight->GetWatchers());
			}
		}
	}
}

}