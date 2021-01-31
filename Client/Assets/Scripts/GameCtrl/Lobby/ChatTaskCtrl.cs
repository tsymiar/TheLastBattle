using UnityEngine;
using System.Collections;
using System;

namespace BlGame.Ctrl
{
    public class ChatTaskCtrl : Singleton<ChatTaskCtrl>
    {
        public bool isOpen = false;
        public void Enter(UInt64 sGUID)
        {
            if (!isOpen){
                isOpen = true;
                EventCenter.Broadcast(EGameEvent.eGameEvent_ChatTaskEnter, sGUID);
            }
            else
            {
                EventCenter.Broadcast(EGameEvent.eGameEvent_ReadyChatTaskEnter, sGUID);
            }

        }

        public void Exit()
        {
            isOpen = false;
            EventCenter.Broadcast(EGameEvent.eGameEvent_ChatTaskExit);
        }
        public void SendMsg(UInt64 sGUID, uint length, byte[] talkMsg)
        {
            CGLCtrl_GameLogic.Instance.EmsgTocs_AskSendMsgToUser(sGUID, length, talkMsg);
        }
        public void ShowChatTask()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_ShowChatTaskFriend);
        }
        public void SetNewChat()
        {
            EventCenter.Broadcast<bool>(EGameEvent.eGameEvent_ReceiveLobbyMsg,false);
        }

        internal void SetDestoy(UInt64 sGUID)
        {
            EventCenter.Broadcast<UInt64>(EGameEvent.eGameEvent_RemoveChatTask, sGUID);
        }
    }
}
