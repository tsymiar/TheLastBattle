using System;
using System.Collections.Generic;
using UnityEngine; 
using System.Linq; 
namespace BlGame.GuideDate
{
    public class SecondaryTaskCpEnoughCheck : SecondaryTaskCheckBase
    {
        const float goalCp = 1500f; 

        public override void OnEnter(SecondaryTaskInfo parent)
        {
            base.OnEnter(parent);
            AddCheckListener();
        }

        void OnEvent(CEvent eve)
        {             
            int cp = (int)eve.GetParam("Cp");
            if (cp >= goalCp)
            { 
                GuideHelpData data = ConfigReader.GetGuideHelpInfo(parentInfo.GetTaskId());
                SecondaryGuideManager.Instance.SendTaskStartTag(data);
            }
        }

        public override void AddCheckListener()
        {
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_NotifyChangeCp, OnEvent);             
        }

        public override void RemoveAddListener()
        {
            if (EventCenter.mEventTable.ContainsKey(EGameEvent.eGameEvent_NotifyChangeCp))
            {
                EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_NotifyChangeCp, OnEvent);
            }
        }

        public override void OnEnd()
        { 
            base.OnEnd();
            RemoveAddListener();
        }

    }
}
