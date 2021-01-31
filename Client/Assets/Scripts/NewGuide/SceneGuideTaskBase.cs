using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using UnityEngine;
using GameDefine;

namespace BlGame.GuideDate

{
    public class SceneGuideTaskBase 
    {
        protected const int startTaskId = 1001;
        protected const int endTaskId = -1;

        protected int currentTaskId = 0;
        protected int reConnectTaskId = 0;
        public GuideTaskManager currentTask = null;
        public GuideTaskManager reConnectTask = null;

        public virtual void InitSceneTaskData() { 

        }

        public virtual void SetTaskStart(int taskId) {
            CTaskManagerData data = ConfigReader.GetManagerTaskInfo(taskId);
            if (data == null)
                return;
            currentTaskId = taskId; 
            currentTask = new GuideTaskManager(data);
            currentTask.SetTaskState(GuideTaskManager.TaskState.EnterState);
            currentTask.SetTaskState(GuideTaskManager.TaskState.CheckState);
            currentTask.FinishEvent += FinishCurrentTask; 
        }

        public void DoRebornTask() {
            CleanReStartTask();
            ReStartTask(currentTaskId);
        }

        public void ReConnectTask(int taskId){//上一个完成的任务
            CleanReStartTask();
            //CTaskManagerData finishTask = ConfigReader.GetManagerTaskInfo(taskId);
            //if (finishTask == null)
            //    return;
            //ReStartTask(finishTask.NextTaskId);
        }

        void CleanReStartTask()
        {
            if (currentTask != null)
            {
                currentTask.CleanAllTask();
                currentTask.FinishEvent -= FinishCurrentTask;
                currentTask = null;
            }
            if (reConnectTask != null)
            {
                reConnectTask.CleanAllTask();
                reConnectTask.FinishEvent -= FinishReConnect;
                reConnectTask = null;
            }
        }

        void ReStartTask(int curTaskId)
        {
            CTaskManagerData curTask = ConfigReader.GetManagerTaskInfo(curTaskId);
            if (curTask == null)
                return;
            GuideTaskReConnectDeal.Instance.StartReConnectTask(curTaskId);
            GuideTaskReConnectDeal.Instance.DealReConnectTask();
           // return;
            //reConnectTaskId = curTask.ReConnectId;           
            //CTaskManagerData newTask = new CTaskManagerData();
            //newTask.TaskTypeSet = new List<GuideTaskType>();
            //newTask.TaskTypeSet.Add(GuideTaskType.PathTask);
            //newTask.SonTaskIdSet = new List<int>();
            //newTask.SonTaskIdSet.Add(reConnectTaskId);
            //newTask.MeetCheck = TaskCheckType.PartMeetCheck;
            //newTask.EndTaskSet = new List<int>();
            //newTask.EndTaskSet.Add(0);
            //currentTaskId = curTask.TaskId;
            //reConnectTask = new GuideTaskManager(newTask);
            //reConnectTask.SetTaskState(GuideTaskManager.TaskState.EnterState);
            //reConnectTask.SetTaskState(GuideTaskManager.TaskState.CheckState);
            //reConnectTask.FinishEvent += FinishReConnect;
        }

        public virtual void OnUpdate() {
            if (currentTask != null && currentTask.taskState == GuideTaskManager.TaskState.CheckState) {
                currentTask.OnUpdate();
            }
            if (reConnectTask != null && reConnectTask.taskState == GuideTaskManager.TaskState.CheckState)
            {
                reConnectTask.OnUpdate();
            }
        } 

        protected virtual void FinishCurrentTask() {
            //currentTask.FinishEvent -= FinishCurrentTask;
            //CTaskManagerData taskInfo = ConfigReader.GetManagerTaskInfo(currentTaskId);
            //if (taskInfo.NextTaskId == -1)
            //{ 
            //    SceneGuideTaskManager.Instance().FinishAll();
            //    currentTaskId = 0;
            //    currentTask = null;
            //    return;
            //}
            //SceneGuideTaskManager.Instance().FinishAsignedStep(currentTaskId);
            ////通知服务器完成了当前的任务
            //currentTaskId = taskInfo.NextTaskId;
            //SetTaskStart(currentTaskId);
        }

        protected virtual void FinishReConnect() {
            reConnectTask.FinishEvent -= FinishReConnect;
            reConnectTask = null;
            SetTaskStart(currentTaskId);
            reConnectTaskId = 0;
        }

        public bool CheckObstructTask(Vector3 pos) {
            if (currentTask == null || reConnectTask != null)
                return true;
            List<GuideObstructTask> obstructTaskList = currentTask.GetObstructTask();
            if (obstructTaskList == null || obstructTaskList.Count == 0)
                return true;
            foreach (var item in obstructTaskList) {
                if (!item.MoveCheck(pos))
                    return false;
            }
            return true;
        }
    }
}
