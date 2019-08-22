using System;
using System.Collections.Generic;
using UnityEngine;
using GameDefine;
using BlGame.GameEntity;
using JT.FWW.Tools;
using System.Linq;
using BlGame.View;

namespace BlGame.GuideDate
{
    public class IGuideTask
    {

        protected int taskId = -1;

        protected IGuideTaskList iParent = null;

        protected IGuideData taskData = null;

        protected IGuideTaskEffect effect = null;

        const int newsGuideTaskId = 1004;

        public enum SelectPlayerType
        { 
            Newer = 0,
            Old
        }

        SelectPlayerType playeType = SelectPlayerType.Newer;

        public void OnEnter()
        {
            taskData = ConfigReader.GetIGuideInfo(taskId);
            if (iParent.GetIGuideTaskData().IsTriggerTask)
            {
                effect = new IGuideTriggerTaskEffect(iParent.GetIGuideTaskData( ), taskId);
            }
            else {
                effect = new IGuideTaskEffect(iParent.GetIGuideTaskData(), taskId);
            }
            
            EventCenter.AddListener<CEvent>((EGameEvent)taskData.EndTaskEvent, OnTrigger); 
            IGuideTaskManager.Instance().SendTaskStart((EGameEvent)taskData.EndTaskEvent, (EGameEvent)taskData.StartTaskEvent);
        } 
        public void OnExecute()
        {
            if (effect != null) {
                effect.OnExecute();
            }
        }

        public enum TaskState
        {
            TaskMark = 0,
            TaskShow,
            TaskEnd,
        }

        GameObject objSkipNewsGuide = null; 
        
        void OnTrigger(CEvent eve)
        {
            TaskState tag = (TaskState)eve.GetParam("TaskState");
            switch (tag)
            {
                case TaskState.TaskEnd:
                    effect.OnEnd();
                    EventCenter.RemoveListener<CEvent>((EGameEvent)taskData.EndTaskEvent, OnTrigger);
                    if (taskData.IsMask)
                    { 
                        effect.ShowMark(false, null);
                    }
                    iParent.CheckNextTask();
                    SkipNewsGuideEnd();
                    effect = null;
                    break;
                case TaskState.TaskShow:
                    //if (ConnectMsg.Instance == null)
                    {
                        if (taskData.TaskId == newsGuideTaskId && objSkipNewsGuide != null)
                            return;
                        effect.OnEnter();
                    }
                    break;
                case TaskState.TaskMark: 
                    List<GameObject> objList = (List<GameObject>)eve.GetParam("Mark");
                    //objList = (ConnectMsg.Instance != null && ConnectMsg.Instance.gameObject.activeInHierarchy) ? null : objList;
                    if (taskData.IsMask)
                    {
                        effect.ShowMark(true, objList);
                    }
                    SkipNewsGuideMark(objList);
                    break;
            }

        }

        void SkipNewsGuideMark(List<GameObject> objList)
        {
            if (taskData.TaskId == newsGuideTaskId)
            {
                //senln
                objSkipNewsGuide = LoadUiResource.LoadRes(/*LobbyWindow.Instance.GetRoot()*/null, GameConstDefine.SkipNewsGuidePath);
                UIWidget wid = objList.ElementAt(0).GetComponent<UIWidget>();
                ButtonOnPress btn;
                for (int i = 0; i < 2; i++)
                {
                    btn = objSkipNewsGuide.transform.FindChild("Btn" + (i + 1).ToString()).GetComponent<ButtonOnPress>();
                    btn.AddListener(i, SelectSkillNewsGuide);
                    if (btn.GetComponent<UIWidget>() == null)
                    {
                        btn.gameObject.AddComponent<UIWidget>();
                    }
                    btn.GetComponent<UIWidget>().depth = wid.raycastDepth + 2000;
                }
                if (objSkipNewsGuide.GetComponent<UIWidget>() == null)
                {
                    objSkipNewsGuide.gameObject.AddComponent<UIWidget>();
                }
                objSkipNewsGuide.GetComponent<UIWidget>().depth = wid.raycastDepth + 1000;
            }
        }

        void DestroySkillNewsGuide() {
            if (taskData.TaskId == newsGuideTaskId)
            {
                LoadUiResource.DestroyLoad(GameConstDefine.SkipNewsGuidePath);
                objSkipNewsGuide = null;
            }
        }

        void SelectSkillNewsGuide(int ie, bool isPress) {           
            DestroySkillNewsGuide();
            playeType = (SelectPlayerType)ie;
            if (ie == 1)
            {
                IGuideTaskManager.Instance().SendTaskEnd((EGameEvent)taskData.EndTaskEvent);                
            }
            else
            {
                IGuideTaskManager.Instance().SendTaskEffectShow((EGameEvent)taskData.EndTaskEvent);
            }
        }

        void SkipNewsGuideEnd() {
            if (taskData.TaskId == newsGuideTaskId && playeType == SelectPlayerType.Old)
            {
                IGuideTaskManager.Instance().SendTaskTrigger(EGameEvent.eGameEvent_UIGuideTriggerMatchGame);
                IGuideTaskManager.Instance().SendTaskEffectShow(EGameEvent.eGameEvent_UIGuideMatchBtnEnd);
            }
        }

        public IGuideTask(IGuideTaskList parent, int id)
        {
            taskId = id;
            iParent = parent;
        }

        public int GetTaskId()
        {
            return taskId;
        }


    }
}
