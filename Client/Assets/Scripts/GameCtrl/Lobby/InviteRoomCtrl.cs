using UnityEngine;
using System.Collections;

namespace BlGame.Ctrl
{
    public class InviteRoomCtrl : Singleton<InviteRoomCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_InviteRoomEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_InviteRoomExit);
        }

        public void AcceptAddFriend(string roomID, string password)
        {
            CGLCtrl_GameLogic.Instance.AddRoom(roomID, password);
        }

    }
}

