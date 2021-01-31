using UnityEngine;
using System.Collections;

namespace BlGame.Ctrl
{
    public class SystemNoticeCtrl : Singleton<SystemNoticeCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_SystemNoticeEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_SystemNoticeExit);
        }
    }
}