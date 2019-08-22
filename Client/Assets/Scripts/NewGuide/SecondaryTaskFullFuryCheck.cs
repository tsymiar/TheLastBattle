using System;
using System.Collections.Generic;
using UnityEngine; 
using System.Linq;
using GameDefine;
namespace BlGame.GuideDate
{
    public class SecondaryTaskFullFuryCheck : SecondaryTaskCheckBase
    {
        EFuryState curState = EFuryState.eFuryNullState;
        
        
        public override void OnEnter(SecondaryTaskInfo parent)
        {
            base.OnEnter(parent);
            AddCheckListener();
        } 

        void OnEvent(CEvent eve)
        {
            EFuryState state = (EFuryState)eve.GetParam("State");
            GuideHelpData data = ConfigReader.GetGuideHelpInfo(parentInfo.GetTaskId()); 
            if (state == EFuryState.eFuryFullState && curState != EFuryState.eFuryFullState)
            {              
                SecondaryGuideManager.Instance.SendTaskStartTag(data);
            }
            else
            { 
                SecondaryGuideManager.Instance.SendTaskEndTag(data); 
            }
            curState = state;
        }

        public override void AddCheckListener()
        {            
            //EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_NotifySelfPlayerFuryStateChange, OnEvent);             
        }

        public override void RemoveAddListener()
        {
            //if (EventCenter.mEventTable.ContainsKey(EGameEvent.eGameEvent_NotifySelfPlayerFuryStateChange))
            //{
            //    EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_NotifySelfPlayerFuryStateChange, OnEvent);
            //}
        }

        public override void OnEnd()
        { 
            base.OnEnd();
            RemoveAddListener();
        }

    }
}
