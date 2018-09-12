
#ifndef SSOBSEV_H_
#define SSOBSEV_H_
#include "SSUser.h"

namespace SceneServer{

class CSSOBSev
{ 
	typedef map<UINT64, INT8 >	OBUSER_GUID_MAP; 
	 
	typedef map<INT64, CSSUser*>	OBUSER_MAP; 
	typedef map<INT64, SGUID_KEY>   OBALLUSER_GUID;
public:
	CSSOBSev();
	~CSSOBSev();
	void Release(); 

	INT32 AddObUser(  CSSBattle *pBattle, CSSUser* user);

	INT32 AddObGUID(  CSSBattle *pBattle, const SGUID_KEY& guid, INT8 pos); 


	bool  IsAllOBEnter();
	bool  IfOBUser(CSSUser* piUser);

	 
private:  

	OBUSER_MAP	m_OBUser; 

	OBALLUSER_GUID m_UserGUID;

	OBUSER_GUID_MAP m_OBUserGUID; 

	//void SyncBattleHerosSkillsInfo(CSSBattle *pBattle   );

	//void SyncBattleBaseInfo( IMsg::PsMsgHeader  psMsgHeader, CSSBattle *pBattle );

	//bool IfExistGUID(const SGUID_KEY &guid);

	EGameObjectCamp  GetOBGUIDCamp(const SGUID_KEY & guid);

	//INT32 RemoveDeadGUID(const SGUID_KEY & guid); 
};



}

#endif