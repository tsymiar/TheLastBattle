using UnityEngine;
using System.Collections;
namespace BlGame.Ctrl
{
    public class GameSettingCtrl : Singleton<GameSettingCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_GameSettingEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_GameSettingExit);
        }
    }
}