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
    public class SecondaryTaskInfo
    {
        private int sTaskId;
        private int sTimes;
        private int sMathches;
        private GuideHelpData data;
        private bool isFinish;
        private SecondaryTaskEffect taskEffect;      
        private SecondaryTaskCheckBase curCheckBase;   
        public const int endTaskBetween = -4;
        


        public SecondaryTaskInfo()
        {
            Clean();
        }

        public void Clean() {
            sTaskId = 0;
            sTimes = 0;
            sMathches = 0;
            data = null;
            isFinish = false;
            if (taskEffect != null)
            {
                taskEffect.OnEnd();                
            }
            taskEffect = null;
            if (curCheckBase != null)
            {
                curCheckBase.OnEnd();
            }
            
            curCheckBase = null; 
        }

        public int GetTaskId()
        {
            return sTaskId;
        }
         

        public void InitTaskInfo(int taskId, int times, int mathches)
        {
            sTaskId = taskId;
            sTimes = times;
            sMathches = mathches;
            data = ConfigReader.GetGuideHelpInfo(sTaskId); 
            switch (taskId) { 
                case 20001://cp enough to buy
                    curCheckBase = new SecondaryTaskCpEnoughCheck();                    
                    break;
                case 20002://hp less back to city
                    curCheckBase = new SecondaryTaskHpLessCheck();
                    break;
                case 20003:
                    curCheckBase = new SecondaryTaskFullFuryCheck();
                    break;
                case 20004:
                    curCheckBase = new SecondaryTaskOpenShopCheck();
                    break;
                default:
                   GameMethod.DebugError("Secondary task not define");
                   break;
            }
            if (curCheckBase != null)
            {
                curCheckBase.OnEnter(this); 
            }
        }

        public void OnEnter()
        { 
            EventCenter.AddListener<CEvent>((EGameEvent)data.helpTriggerEvent, OnEventStart); 
        }

        public void OnUpdate() {
            if (taskEffect == null)
                return;
            taskEffect.OnUpdate();   
        }

        void OnCommit()
        {
            isFinish = true;
            SecondaryGuideManager.Instance.CommitTask(this);
        }

        void OnEnd()
        {          
            if (curCheckBase != null)
            {
                curCheckBase.OnEnd();
            }
            
            SecondaryGuideManager.Instance.FinishTask(this);
        }

        void OnEventStart(CEvent eve)
        {           
            if (isFinish)
                return;
            if (data == null)
                return;
            if (EventCenter.mEventTable != null && EventCenter.mEventTable.ContainsKey((EGameEvent)data.helpTriggerEvent))
            {
                EventCenter.RemoveListener<CEvent>((EGameEvent)data.helpTriggerEvent, OnEventStart);
            }
            taskEffect = new SecondaryTaskEffect(this);
            taskEffect.OnEnter();
            sTimes += 1; 
            if (sTimes >= data.helpTimes)
            {
                OnCommit(); //只是提交任务，但是显示效果可能还在继续
            }
            EventCenter.AddListener<CEvent>((EGameEvent)(data.helpTriggerEvent + endTaskBetween), OnEventEnd); 
        }

        void OnEventEnd(CEvent eve) {
            if (EventCenter.mEventTable.ContainsKey((EGameEvent)(data.helpTriggerEvent + endTaskBetween)))
            {
                EventCenter.RemoveListener<CEvent>((EGameEvent)(data.helpTriggerEvent + endTaskBetween), OnEventEnd); 
            }   
            if (taskEffect != null)
            {
                taskEffect.OnEnd();
            } 
            taskEffect = null;
            if (isFinish)//如果任务已经提交则，真正完成此任务
            {
                OnEnd();
                return;
            }
            EventCenter.AddListener<CEvent>((EGameEvent)data.helpTriggerEvent, OnEventStart); 
        }
         
    }
}
