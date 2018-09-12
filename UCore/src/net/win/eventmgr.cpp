#include "ucnetconfig.h"
#include "eventmgr.h"
#include "iocpctrl.h"

IMPLEMENT_SINGLETON(CEventMgr)

CEventMgr::CEventMgr()
{
	//m_poRecvQueue	= NULL;
}

CEventMgr::~CEventMgr()
{
}

bool CEventMgr::Init(INT32 nSize)
{
	if(false == m_oEvtQueue.Init(nSize+1))
	{
		return false;
	}

	m_threadBufferPool = CIocpCtrl::Instance()->GetThreadBufferPool();
	return true;
}

void CEventMgr::Uninit()
{
}


