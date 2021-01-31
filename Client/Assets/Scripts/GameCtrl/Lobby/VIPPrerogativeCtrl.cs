using UnityEngine;
using System.Collections;

namespace BlGame.Ctrl
{
    public class VIPPrerogativeCtrl : Singleton<VIPPrerogativeCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_VIPPrerogativeEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_VIPPrerogativeExit);
        }
    }
}
