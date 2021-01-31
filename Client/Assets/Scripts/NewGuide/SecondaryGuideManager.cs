using System;
using System.Collections.Generic;
using UnityEngine;
using GameDefine;
using BlGame.GameEntity;
using JT.FWW.Tools;
using System.Linq;
using BlGame.GameData;
namespace BlGame.GuideDate
{
    public class SecondaryGuideManager : Singleton<SecondaryGuideManager>
    { 
        private Dictionary<int, SecondaryTaskInfo> taskDic = new Dictionary<int, SecondaryTaskInfo>(); 

        public void InitFinishTask(int task,int matches) {
            GuideHelpData data = ConfigReader.GetGuideHelpInfo(task);
            if (data == null || data.helpMatches <= matches)
                return;
            if (taskDic.ContainsKey(task))
            {
                GameMethod.DebugError("secondary guide repeat");
                return;
            }
            SecondaryTaskInfo taskInfo = new SecondaryTaskInfo(); 
            taskInfo.InitTaskInfo(task, 0, matches);
            taskDic.Add(task, taskInfo);  
        }

        public void InitTask() {
            foreach (var item in ConfigReader.GuideHelpXmlInfoDict) {
                if (taskDic.ContainsKey(item.Key))
                    continue;
                InitFinishTask(item.Key,0);
            }
        }

        public void CleanAll() {
            for (int i = taskDic.Count - 1; i >= 0; i--) {
                SecondaryTaskInfo info = taskDic.ElementAt(i).Value;
                if (info == null)
                    continue;
                FinishTask(info);
            }
            //if (EventCenter.mEventTable.ContainsKey(EGameEvent.eGameEvent_GameOver))
            //{
            //    EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_GameOver, OnGameOver);
            //}
        }

        public void StartAllTask() {
            //EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_GameOver, OnGameOver);
            if (taskDic == null || taskDic.Count == 0)
                return;
            for (int i = taskDic.Count - 1; i >= 0; i--) {
                SecondaryTaskInfo taskInfo = taskDic.ElementAt(i).Value;
                taskInfo.OnEnter();
            } 
        }

        public void OnUpdate() {
            for (int i = taskDic.Count - 1; i >= 0; i--) { 
                SecondaryTaskInfo info = taskDic.ElementAt(i).Value;
                if (info == null)
                    continue;
                info.OnUpdate();
            }
        }

        public void CommitTask(SecondaryTaskInfo task) { //某个任务已经触发
            CGLCtrl_GameLogic.Instance.EmsgToss_GuideFinishStep(task.GetTaskId(), 3);
        }

        public void FinishTask(SecondaryTaskInfo task)//某个任务真正显示完成
        {            
            taskDic.Remove(task.GetTaskId());
            task.Clean();
            task = null;
        }

        public void SendTaskStartTag(GuideHelpData data) {
            if (data == null)
                return;
            CEvent cEve = new CEvent((EGameEvent)data.helpTriggerEvent); 
            EventCenter.SendEvent(cEve);
        }

        public void SendTaskEndTag(GuideHelpData data) 
        {
            CEvent cEve = new CEvent((EGameEvent)(data.helpTriggerEvent + SecondaryTaskInfo.endTaskBetween));
            EventCenter.SendEvent(cEve);
        }

        public bool ContainTask(int taskId) {
            return taskDic.ContainsKey(taskId);
        }

        public static string parentPath = "Guide/SecondaryBoot";
        public GameObject CreateParentPrefab()
        {
            return LoadUiResource.LoadRes(GameMethod.GetUiCamera.transform, parentPath);
        }

        void OnGameOver(CEvent eve)
        {
            LoadUiResource.DestroyLoad(parentPath);
            //if (EventCenter.mEventTable.ContainsKey(EGameEvent.eGameEvent_GameOver))
            //{
            //    EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_GameOver, OnGameOver);
            //}
        }

        
        
    }
}

