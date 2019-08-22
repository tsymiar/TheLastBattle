using System;
using System.Collections.Generic;
using UnityEngine; 
using System.Linq;
using GameDefine;
namespace BlGame.GuideDate
{
    public class SecondaryTaskOpenShopCheck : SecondaryTaskCheckBase
    {
         
        public override void OnEnter(SecondaryTaskInfo parent)
        {
            base.OnEnter(parent);
            AddCheckListener();
        } 

        void OnEvent(CEvent eve)
        {
            bool tag = (bool)eve.GetParam("Tag");
            GuideHelpData data = ConfigReader.GetGuideHelpInfo(parentInfo.GetTaskId());       
            if(tag){
                SecondaryGuideManager.Instance.SendTaskStartTag(data);
            }
            else {
                SecondaryGuideManager.Instance.SendTaskEndTag(data); 
            } 
        }

        public override void AddCheckListener()
        {            
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_NotifyOpenShop, OnEvent);             
        }

        public override void RemoveAddListener()
        {
            if (EventCenter.mEventTable.ContainsKey(EGameEvent.eGameEvent_NotifyOpenShop))
            {
                EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_NotifyOpenShop, OnEvent);
            }
        }

        public override void OnEnd()
        { 
            base.OnEnd();
            RemoveAddListener();
        }

    }
}
