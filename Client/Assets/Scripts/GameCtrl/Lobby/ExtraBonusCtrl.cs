using UnityEngine;
using System.Collections;
namespace BlGame.Ctrl
{
    public class ExtraBonusCtrl : Singleton<ExtraBonusCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_ExtraBonusEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_ExtraBonusExit);
        }

        internal void SendMsg(string mTemp)
        {
            CGLCtrl_GameLogic.Instance.EmsgTocs_CDKReq(mTemp);
        }
    }
}