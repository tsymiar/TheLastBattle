
#include "stdafx.h"
#include "SSGuide.h"

namespace SceneServer{

CSSGuide::CSSGuide(){
	m_ObjCompTask.clear();
}

CSSGuide::~CSSGuide(){
 
}

 
INT32 CSSGuide::AddCompGuide(INT64 objIdx, INT32 dtype, INT32 childType, INT32 stepId)
{  
	SGuideBattleCompTask ct;
	ct.stepId = stepId;
	ct.childType = (EChildTaskType)childType;

	//std::map<INT64/*objid*/,std::vector<SGuideBattleCompTask> >  m_ObjCompTask; 
	auto itr = m_ObjCompTask.find(objIdx);
	if (itr != m_ObjCompTask.end())
	{  
		itr->second.push_back(ct);
	}
	else
	{ 
		std::vector<SGuideBattleCompTask> vec;
		vec.push_back(ct);
		m_ObjCompTask[objIdx] = vec;
	} 
	return eNormal;
}
 
void	CSSGuide::CheckBornSolderOrHero(INT32 dtype, INT32 childType)
{

}
}