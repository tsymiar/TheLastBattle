using System;
using System.Collections.Generic;
using UnityEngine;
using GameDefine;
using BlGame.GameEntity;
using JT.FWW.Tools;
using System.Linq;
namespace BlGame.GuideDate
{
    public class IGuideTriggerTaskList:IGuideTaskList
    {

        public IGuideTriggerTaskList(int id) : base(id) {
           
        }

        protected override void SetTaskData(int id)
        {
            taskId = id; 
            taskData = ConfigReader.GetITriggerGuideManagerInfo(taskId);
        }

        protected override void OnEnd()
        {
            IGuideTaskManager.Instance().RemoveTriggerTask(this);
            // total task end;     
            IGuideTaskManager.Instance().AddHasTriggerTask(taskId);

            CGLCtrl_GameLogic.Instance.EmsgTocsAskFinishUIGuideTask(2, taskId, 1); 
        }

        protected override void OnInterfaceTrigger(CEvent eve)
        {
            if (curState != TaskState.InActiveState)
                return; 
            count = GetFirstNoTrigerTaskId(); 
            StartTask(count);
            SetTaskState(TaskState.OpenState);
        }

        protected override void StartTask(int index) {
            base.StartTask(index); 
            IGuideData data = ConfigReader.GetIGuideInfo(currentTask.GetTaskId());
            IGuideTaskManager.Instance().SendTaskEffectShow((EGameEvent)data.EndTaskEvent);
            IGuideTaskManager.Instance().AddHasTriggerTask(data.TaskId); 
            CGLCtrl_GameLogic.Instance.EmsgTocsAskFinishUIGuideTask(2, taskId, 0);
        }

        private int GetFirstNoTrigerTaskId() {
            for (int i = 0; i < taskData.SonTaskList.Count; i++) {
                int id = taskData.SonTaskList.ElementAt(i);
                if (IGuideTaskManager.Instance().IsTaskTriggered(id)) {
                    continue;
                }
                return i;
            }
            return -1;
        }
    }
}
