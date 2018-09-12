#pragma once

#include "Task.h"

/**

数据库结构体：每日任务更新时间,任务1ID:任务1数量,任务2ID:任务2数量,.....

**/

class CTask;

class CTaskMgr
{
public:
	CTaskMgr(CCSUser* user);
	virtual ~CTaskMgr();
public:
	bool AddTask(UINT32 taskId,UINT32 taskCount=0);//领取任务
	bool FinishTask(UINT32 taskGuid);//完成任务
	bool RushDailyTasks();//刷新每日任务
	bool OnEvent(TASK_TYPE tt,UINT32 count,UINT32 param1,UINT32 param2);//任务事件
	bool PackTaskData(string& taskData,bool& isTaskRush);//编码数据
	bool UnpackTaskData(string& taskData);//解码数据
public:
	void NotifyTaskAdd(CTask* pTask);//通知增加任务
	void NotifyTaskUpdate(CTask* pTask);//通知更新任务
	void NotifyTaskRewards(CTask* pTask);//通知任务奖励
	void NotifyTaskDel(CTask* pTask);//通知删除任务
	void NotifyAllTask();//通知所有任务
	void NotifyAllDailyTask();//通知所有日常任务
private:
	CCSUser* mOwner;
	UINT32 mDailyDay;//日常更新时间
	map<UINT32,CTask*> mDailyTasks;//日常任务
	map<UINT32,CTask*> mInfiniteTasks;//永久任务(成就任务)
	vector<CTask*> mAllTasks;//所有任务
	bool mIsForbidAllTaskNotify;//是否通知所有任务
	bool mIsForbidDailyTaskNotify;//是否通知日常任务
};
