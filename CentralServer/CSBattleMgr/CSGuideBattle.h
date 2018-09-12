#include "stdafx.h"
#include "CSBattle.h"
#include "CSUser.h"

namespace CentralServer{

	 
class CCSGuideBattle : public CCSBattle
{
public:
	CCSGuideBattle(){}
	CCSGuideBattle(EBattleMatchType match_type, BattleType type,UINT32 ssId,UINT64 battleId,UINT32 mapId, CCSUserListMap& pCCSUserList, map<UINT32,UINT32>* aiRobots=NULL);
	~CCSGuideBattle(void); 
public:
	void				ReCreateRoom();
	 void				ReCreateMatch();
	void				CaculateResult(SSToCS::FinishBattle* pFinishMsg);

	void				AddBattleHeroAndOtherList(CCSUser* pUser,CSToSS::CreateBattle_CreateBattleUserInfo* pUserInfo) ;

};

}
