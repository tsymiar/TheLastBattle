using System;
using System.Collections.Generic;
using UnityEngine; 
using System.Linq; 
namespace BlGame.GuideDate
{
    public class SecondaryTaskHpLessCheck : SecondaryTaskCheckBase
    { 
        public override void OnEnter(SecondaryTaskInfo parent)
        {
            base.OnEnter(parent);
            AddCheckListener();
        } 

        void OnEvent(CEvent eve)
        {  
            GuideHelpData data = ConfigReader.GetGuideHelpInfo(parentInfo.GetTaskId());
            bool tag = (bool)eve.GetParam("Tag");
            if (tag)
            {  
                SecondaryGuideManager.Instance.SendTaskStartTag(data);
            }
            else { 
                SecondaryGuideManager.Instance.SendTaskEndTag(data); 
            }
        }

        public override void AddCheckListener()
        {
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_NotifyHpLessWarning, OnEvent);
        }

        public override void RemoveAddListener()
        {
            if (EventCenter.mEventTable.ContainsKey(EGameEvent.eGameEvent_NotifyHpLessWarning))
            {
                EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_NotifyHpLessWarning, OnEvent);
            }
        }

        public override void OnEnd()
        {
            base.OnEnd();
            RemoveAddListener();
        }

    }
}
