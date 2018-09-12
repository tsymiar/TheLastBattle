#include "stdafx.h"
#include "Task.h"

UINT32 CTask::mMaxTaskGUID = 0;

CTask::CTask(CCSUser* user, const STaskConfig* pTaskConfig)
{
	mTaskConfig = pTaskConfig;
	mTaskGUID = ++mMaxTaskGUID;
	mCurCount = 0;
	mOwner = user;
}

CTask::~CTask()
{

}

void CTask::NotifyCount()
{
	mOwner->GetTaskMgr()->NotifyTaskUpdate(this);
}

void CTask::GiveAwards()
{
	mCurCount = UINT32_MAX;//标记已领取
	mOwner->GetTaskMgr()->NotifyTaskRewards(this);
}
