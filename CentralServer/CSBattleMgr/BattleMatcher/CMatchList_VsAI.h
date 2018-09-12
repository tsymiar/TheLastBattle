#pragma once

#include "IMatchList.h"

class CCSMatchList_VsAI : public ICSMatchList
{
public:
	CCSMatchList_VsAI(UINT32 mapId);
	virtual ~CCSMatchList_VsAI();
public:
	virtual bool AddOneTeam(CCSMatchTeam* pTeam);
	virtual bool RemoveOneTeam(CCSMatchTeam* pTeam);
	virtual void Update();
	void PostMsgToAll(google::protobuf::Message& sMsg,int n32MsgID);
public:
	std::vector<CCSMatchTeam*> mVTeam; 
	UINT32 mUserCount;
};
