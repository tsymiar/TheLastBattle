#include "stdafx.h"
#include "SSActive.h"
#include "windows.h"
boost::atomic<int> Active::m_GlobalActiveID(0);

Active::Active(){}

Active::Active(int n32BlockSize, int n32BufferSize): m_pBufferPool(n32BlockSize, n32BufferSize)
	, m_PendingWorkNum(0), m_ActiveID(m_GlobalActiveID++), m_ThreadID(0), m_BeginInThreadCallback(NULL){
		m_IfUpdate = false;
}

Active::~Active() {
	Stop();
}

void Active::Send( Buffer* apBuffer ){
	if (NULL != apBuffer){
		boost::mutex::scoped_lock lock(m_IOMutex);
		bool bNeedSig = m_Queue.empty();
		m_Queue.push(apBuffer);
		lock.unlock();
		++m_PendingWorkNum;
		if (bNeedSig){
			m_ConditionVar.notify_one();
		}
	}
}

void Active::Run() {
	if (m_BeginInThreadCallback){
		m_BeginInThreadCallback();
	}
	while (true){
		Consume();
	}
}


unsigned get_native_thread_id( boost::thread& t)
{
	return GetThreadId( t.native_handle());
}

const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push, 8)
typedef struct THREADNAME_INFO {
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void _SetThreadName(DWORD threadId, const char* threadName) {
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = threadId;
	info.dwFlags = 0;
	__try {
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
	}
}

Active* Active::CreateActive(Callback aCallBack, int n32BlockSize, int n32BufferSize){
	Active* aPtr = new Active(n32BlockSize, n32BufferSize);
	aPtr->m_Callback = aCallBack;
	return aPtr;
}

void Active::setCallBack(Callback aCallBack){
	m_Callback = aCallBack;
}

Buffer* Active::GetBuffer(){
	Buffer* pBuffer = m_pBufferPool.AcquireObject();
	if (!pBuffer){
		return NULL;
	}
	pBuffer->Clear();
	return pBuffer;
}

bool Active::IfEmpty(){
	return m_Queue.empty();  
}

void Active::Consume(){
	boost::mutex::scoped_lock lock(m_IOMutex);
	while(m_Queue.empty()){
		m_ConditionVar.wait(lock);
	}

	m_SwapQueue.swap(m_Queue);
	lock.unlock();

	while(!m_SwapQueue.empty()){
		Buffer* pBuffer = m_SwapQueue.front();
		m_SwapQueue.pop();

		m_Callback(pBuffer);
		--m_PendingWorkNum;
		if (pBuffer){
			m_pBufferPool.ReleaseObejct(pBuffer);
		}
	}
}

void Active::Stop(){
	m_ConditionVar.notify_one();
}

void Active::ReleaseBuffer(Buffer*& pBuffer){
	if (NULL != pBuffer){
		m_pBufferPool.ReleaseObejct(pBuffer);
		pBuffer = NULL;
	}
}

unsigned Active::GetThreadID(){
	return m_ThreadID;
}

void Active::RunWithUpdate(){
	bool ifNotify = false;
	bool ifQueueEmpty = false;
	while(true){
		ifNotify = false;
		ifQueueEmpty = false;
		
		{
			ptime waitTime;
			bool ifNeedWait = m_ThreadTimer.GetNearestWaitTime(waitTime);

			boost::mutex::scoped_lock lock(m_IOMutex);
			while(m_Queue.empty()){
				ifQueueEmpty = true;
				if (!ifNeedWait){
					break;
				}

				ifNotify = m_ConditionVar.timed_wait(lock, waitTime);
				if (!ifNotify){
					break;
				}
			}

			if (ifNotify || !ifQueueEmpty){
				m_SwapQueue.swap(m_Queue);
			}
		}
		
		while(!m_SwapQueue.empty()){
			Buffer* pBuffer = m_SwapQueue.front();
			m_SwapQueue.pop();

			m_Callback(pBuffer);
			--m_PendingWorkNum;
			if (pBuffer){
				m_pBufferPool.ReleaseObejct(pBuffer);
			}
		}

		m_ThreadTimer.Run();
	}
}

void Active::Start(){
	bool ifHvTimer = !m_ThreadTimer.IsEmpty();
	if (ifHvTimer){
		m_Thread = boost::thread(&Active::RunWithUpdate, this);
	}
	else{
		m_Thread = boost::thread(&Active::Run, this);
	}
	m_ThreadID = get_native_thread_id(m_Thread);

	char sThreadName[30];
	sprintf(sThreadName, "%s-%d", "Actor-Run", GetActorID());
	_SetThreadName(m_ThreadID, sThreadName);
}

void Active::SetThreadStartCallback(BeginInThreadCallback pBeginInThreadCallback){
	m_BeginInThreadCallback = pBeginInThreadCallback;
}

int Buffer::GetMaxLength(){
	return m_MaxLength;
}
