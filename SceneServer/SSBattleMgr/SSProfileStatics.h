#pragma once
#include "stdafx.h"
#include <map>

namespace SceneServer{
enum StaticsType{
	StaticsType_None,
	StaticsType_NPCLookEnemy,
	StaticsType_Sight,
	StaticsType_Move,
};

class ProfileInScope{
public:
	ProfileInScope(StaticsType aType);
	~ProfileInScope();
private:
	StaticsType	m_Type;
};

class CSSProfileStatics
{
private:
	CSSProfileStatics();
public:
	friend class ProfileInScope;
	struct ProfileData{
		TIME_MILSEC tottime;
		TIME_MILSEC beginTime;
		string debugString;
		ProfileData():tottime(0), beginTime(0){}
	};
	static CSSProfileStatics& GetInstance();
	~CSSProfileStatics(void);
	void	GetProfileReport(wstringstream &report);
	void	AddMsg(int Msgid);
private:
	typedef std::map<int, ProfileData> ProfileMap;
	ProfileMap	m_ProfileMap;
	void	Begin(StaticsType aType);
	void	End(StaticsType aType);

private:
	typedef map<int, int>  TotNumForPerMsgMap;
	TotNumForPerMsgMap m_TotNumForPerMsg;
	struct MsgInterval{
		int msgid;
		int msgnum;
		MsgInterval(int msgid, int msgnum):msgid(msgid), msgnum(msgnum){}
		bool operator <(const MsgInterval& p) const{
			return msgnum > p.msgnum;
		}
		bool operator <(MsgInterval& p){
			return msgnum > p.msgnum;
		}
	};
	TIME_MILSEC m_LastMsgShow;
};

}

