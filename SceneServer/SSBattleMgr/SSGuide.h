
#ifndef SSGUIDE_H_
#define SSGUIDE_H_
#include <map>
#include <vector>

namespace SceneServer{

class CSSGuide
{
	std::map<INT64/*objid*/,std::vector<SGuideBattleCompTask> >  m_ObjCompTask; 
public:
	CSSGuide();
	~CSSGuide(); 

	INT32		AddCompGuide(INT64 objIdx, INT32 dtype, INT32 childType, INT32 stepId);

	void		CheckBornSolderOrHero(INT32 dtype, INT32 childType);
 
};
 
}

#endif