using System;
using System.Collections.Generic;
using UnityEngine;
using GameDefine;
using BlGame.GameEntity;
using JT.FWW.Tools;
using System.Linq;
namespace BlGame.GuideDate
{
    public class IGuideTaskManager
    {
        public static IGuideTaskManager Instance()
        {
            if (instance == null)
            {
                instance = new IGuideTaskManager();
            }
            return instance;
        }
         
        private static IGuideTaskManager instance = null;

        private List<IGuideTaskList> currentCheckTask = new List<IGuideTaskList>();

        private IGuideTaskList curTask = null;

        Dictionary<BoxCollider, bool> orignalBox = new Dictionary<BoxCollider, bool>();

        private List<IGuideTaskList> triggerTaskList = new List<IGuideTaskList>();//current excuese trigger task
        private List<int> triggerTaskIdList = new List<int>();//contain trigger task total task Id and son task Id

        public const int startId = 10001;


        public void SetTaskId(int taskId)
        {
            for (int i = 0; i < currentCheckTask.Count; i++)
            {
                if (currentCheckTask.ElementAt(i).GetTaskId() == taskId)
                {
                    return;
                }
                currentCheckTask.ElementAt(i).Clean();
                IGuideTaskList task = currentCheckTask.ElementAt(i);
                task = null;
            }
            currentCheckTask.Clear();

            IGuideTaskList iTask = new IGuideTaskList(taskId);
            iTask.OnEnter();
            currentCheckTask.Add(iTask);
        }

        public void SetCurTask(IGuideTaskList task)
        {
            curTask = task;
            for (int i = 0; i < currentCheckTask.Count; i++)
            {
                if (currentCheckTask.ElementAt(i) == curTask)
                {
                    return;
                }
                currentCheckTask.ElementAt(i).Clean();
            }
            currentCheckTask.Clear();
        }

        public void NextTask()
        {
            if (curTask == null)
            {
                return;
            }

            IGuideManagerData data = curTask.GetIGuideTaskData();
            curTask = null;
            LoadUiResource.DestroyLoad(GameConstDefine.UIGuideRestPath); 
            if (data.NextTaskId.ElementAt(0) == -1)
            {
                //ask all end
                CGLCtrl_GameLogic.Instance.EmsgTocsAskFinishUIGuideTask(1, data.TaskId, 1);
                data = null;
                StartTriggerTask();
                SetTaskIsFinish(true, true);
                return;
            }
            // ask end
            CGLCtrl_GameLogic.Instance.EmsgTocsAskFinishUIGuideTask(1, data.TaskId, 0);
            for (int i = 0; i < data.NextTaskId.Count; i++)
            {
                SetTaskId(data.NextTaskId.ElementAt(i));
            }
            data = null;
        }


        public void OnUpdate()
        {
            if (curTask != null)
            {
                curTask.OnExecute();
            }

            if (triggerTaskList != null && triggerTaskList.Count != 0) {
                for (int i = triggerTaskList.Count - 1; i >= 0; i--) {
                    if (triggerTaskList.ElementAt(i) != null) {
                        triggerTaskList.ElementAt(i).OnExecute();
                    }
                }
            }
        }
         
        private IGuideTaskManager()
        {
            EventCenter.AddListener(EGameEvent.eGameEent_ScenseChange, ChangeScense);
        }

        ~IGuideTaskManager()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEent_ScenseChange, ChangeScense);
        }

        void ChangeScense()
        {
            LoadUiResource.DestroyLoad(GameConstDefine.UIGuideRestPath);
        }

        public void SendTaskEnd(EGameEvent gameEve)
        {
            CEvent eve = new CEvent(gameEve);
            eve.AddParam("TaskState", IGuideTask.TaskState.TaskEnd);
            EventCenter.SendEvent(eve);
        }

        public void SendTaskTrigger(EGameEvent gameEve)
        {
            SendTaskEnd(gameEve);
        }

        public void SendTaskStart(EGameEvent gameEnd, EGameEvent gameStart)
        {
            EventCenter.Broadcast(gameStart, gameEnd);
        }

        public void AddTaskStartListerner(EGameEvent gameEve, Callback<EGameEvent> back)
        {
            EventCenter.AddListener<EGameEvent>(gameEve, back);
        }

        public void RemoveTaskStartListerner(EGameEvent gameEve, Callback<EGameEvent> back)
        {
            if (EventCenter.mEventTable.ContainsKey(gameEve))
            {
                EventCenter.RemoveListener<EGameEvent>(gameEve, back);
            }
        }

        public void SendTaskMarkObjList(EGameEvent gameEve, List<GameObject> objList)
        {
            CEvent even = new CEvent(gameEve);
            even.AddParam("TaskState", IGuideTask.TaskState.TaskMark);
            even.AddParam("Mark", objList);
            EventCenter.SendEvent(even);
        }

        public void SendTaskEffectShow(EGameEvent gameEve)
        {
            CEvent eve = new CEvent(gameEve);
            eve.AddParam("TaskState", IGuideTask.TaskState.TaskShow);
            EventCenter.SendEvent(eve);
        }

        public void StartTriggerTask()// start trigger task
        {
            triggerTaskList.Clear();
            for (int i = ConfigReader.ITriggerGuideManagerDatalXmlInfoDict.Count - 1; i >= 0; i--)
            {
                IGuideManagerData data = ConfigReader.ITriggerGuideManagerDatalXmlInfoDict.ElementAt(i).Value;
                if (IsTaskTriggered(data.TaskId))
                    continue;
                IGuideTaskList task = new IGuideTriggerTaskList(data.TaskId);
                task.OnEnter();
                triggerTaskList.Add(task);
            }
        }

        public void RemoveTriggerTask(IGuideTaskList task)// if one trigger task finish ,remove update
        {
            triggerTaskList.Remove(task);
            task = null;
        }
        
        public void SetHasTriggerTask(string taskId)// get has trigger task from server
        {
            triggerTaskIdList.Clear(); 
            if (!string.IsNullOrEmpty(taskId)) {
                triggerTaskIdList = GameMethod.ResolveToIntList(taskId, ',');
            } 
        }

        public void AddHasTriggerTask(int taskId) {// client finish trigger task
            triggerTaskIdList.Add(taskId);
        }

        public bool IsTaskTriggered(int id)//check is triggered task
        {
            if (triggerTaskIdList.Contains(id))
            {
                return true;
            }
            return false;
        }

        private bool lineTaskFinish = false;
        private bool triggerTaskFinish = false;
        public void SetTaskIsFinish(bool isLineTask,bool isFinish) {
            if (isLineTask)
            {
                lineTaskFinish = isFinish;
            }
            else {
                triggerTaskFinish = isFinish;
            }
        }

        public bool IsTriggerTaskFinish() {
            return triggerTaskFinish;
        }

        public bool IsLineTaskFinish() {
            return lineTaskFinish;
        }
    }
}
