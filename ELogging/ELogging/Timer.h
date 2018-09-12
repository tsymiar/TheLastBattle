#pragma once
#include <queue>
#include <functional>
#include <vector>
#include <set>

#include <boost/date_time/posix_time/posix_time.hpp> 
#include "boost/date_time/gregorian/gregorian.hpp"

using boost::gregorian::date;
using boost::posix_time::ptime;
using boost::posix_time::microsec_clock;
using namespace std;

typedef std::function<void(int64_t, int64_t)> HeartbeatCallback;

typedef int64_t TimeKey;
class CBattleTimer
{
public:
	CBattleTimer(void);
	~CBattleTimer(void);
	//必须先初始化这个时间，因为之后Timer函数返回的当前时间都是相对时间
	void			SetInitTime(ptime& time){m_InitTime = time;}

	int64_t			AddTimer(HeartbeatCallback pHeartbeatCallback, int64_t interval, bool ifPersist);
	void			RemoveTimer(int64_t timerID){m_InvalidTimerSet.insert(timerID);}
	bool			GetNearestWaitTime(ptime& time);

	void			Run();

	bool			IsEmpty()const{return m_ToAddTimer.empty() && m_ThreadTimerQueue.empty();}

private:
	int64_t			GetTimerSequence(){return ++m_TimerSeq;}

	struct ThreadTimer{
		TimeKey nextexpiredTime;
		TimeKey lastHandleTime;
		int64_t	sequence;
		HeartbeatCallback pHeartbeatCallback;
		int64_t interval;
		bool ifPersist;
		ThreadTimer(TimeKey& nextexpiredTime, TimeKey& lastHandleTime, HeartbeatCallback pHeartbeatCallback, int64_t interval, int64_t sequence, bool ifPersist)
			:nextexpiredTime(nextexpiredTime), lastHandleTime(lastHandleTime), pHeartbeatCallback(pHeartbeatCallback), interval(interval), sequence(sequence), ifPersist(ifPersist){
		}
		bool operator <(const ThreadTimer& sThreadTimer) const{
			if (nextexpiredTime != sThreadTimer.nextexpiredTime){
				return nextexpiredTime > sThreadTimer.nextexpiredTime;
			}
			return  sequence > sThreadTimer.sequence;
		}
	};

	TimeKey GetInternalTime();

	std::priority_queue<ThreadTimer>		m_ThreadTimerQueue;
	vector<ThreadTimer>			m_ToAddTimer;
	vector<ThreadTimer>			m_PendingTimer;
	set<int64_t>	m_InvalidTimerSet;
	int64_t	m_TimerSeq;
	ptime	m_InitTime;
};

