#include "Timer.h"


CBattleTimer::CBattleTimer(void):m_TimerSeq(0)
{
}


CBattleTimer::~CBattleTimer(void)
{
}

int64_t CBattleTimer::AddTimer(HeartbeatCallback pHeartbeatCallback, int64_t interval, bool ifPersist){
	TimeKey nowTime = GetInternalTime();
	int64_t seqID = GetTimerSequence();
	TimeKey nextTime = nowTime + interval;
	ThreadTimer lThreadTimer(nextTime, nowTime, pHeartbeatCallback, interval, seqID, ifPersist);
	m_ToAddTimer.push_back(lThreadTimer);
	return seqID;
}

void CBattleTimer::Run(){
	TimeKey nowTime = GetInternalTime();

	while(!m_ThreadTimerQueue.empty()){
		ThreadTimer& sThreadTimer = m_ThreadTimerQueue.top();
		if (!m_InvalidTimerSet.empty()){
			auto iter = m_InvalidTimerSet.find(sThreadTimer.sequence);
			if (iter != m_InvalidTimerSet.end()){
				m_InvalidTimerSet.erase(iter);
				m_ThreadTimerQueue.pop();
				continue;
			}
		}
		
		if (nowTime >=  sThreadTimer.nextexpiredTime){
			m_PendingTimer.push_back(sThreadTimer);
			m_ThreadTimerQueue.pop();
		}
		else{
			break;
		}
	}

	if (!m_PendingTimer.empty()){
		for (auto iter = m_PendingTimer.begin(); iter != m_PendingTimer.end(); ++iter){
			ThreadTimer& sThreadTimer = *iter;
			nowTime = GetInternalTime();
			int64_t tickSpan = nowTime - sThreadTimer.lastHandleTime;
			sThreadTimer.pHeartbeatCallback(nowTime, tickSpan);

			if (sThreadTimer.ifPersist){
				TimeKey newTime = nowTime + sThreadTimer.interval;
				sThreadTimer.lastHandleTime = nowTime;
				sThreadTimer.nextexpiredTime = newTime;
				m_ThreadTimerQueue.push(sThreadTimer);
			}
		}

		m_PendingTimer.clear();
	}

	if (!m_ToAddTimer.empty()){
		for (auto iter = m_ToAddTimer.begin(); iter != m_ToAddTimer.end(); ++iter){
			m_ThreadTimerQueue.push(*iter);
		}

		m_ToAddTimer.clear();
	}
}

bool CBattleTimer::GetNearestWaitTime(ptime& time){
	if (m_ThreadTimerQueue.empty()){
		return false;
	}

	time = m_InitTime + boost::posix_time::milliseconds(m_ThreadTimerQueue.top().nextexpiredTime);
	return true;
}

TimeKey CBattleTimer::GetInternalTime(){
	return (microsec_clock::universal_time() - m_InitTime).total_milliseconds();
}
