using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using System.Collections.Generic;
using GameDefine;
using BlGame.Resource;
using System;

namespace BlGame.Ctrl
{
    public class SocialCtrl : Singleton<SocialCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_SocialEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_SocialExit);
        }
        
        internal void RemoveFriend(UInt64 sGUID, FRIENDTYPE fRIENDTYPE)
        {
            CGLCtrl_GameLogic.Instance.EmsgTocs_AskRemoveFromFriendList(sGUID, (int)fRIENDTYPE);
        }
        internal void AddFriendBlackList(string CurrName, FRIENDTYPE fRIENDTYPE)
        {
            CGLCtrl_GameLogic.Instance.EmsgTocs_AskAddToFriendList(CurrName, (int)fRIENDTYPE);
        }
        internal void AddFriendBlackID(ulong id, FRIENDTYPE fRIENDTYPE)
        {
            CGLCtrl_GameLogic.Instance.EmsgTocs_AskAddToSNSListByID(id, (int)fRIENDTYPE);
        }

        internal void AskBlackList()
        {
            CGLCtrl_GameLogic.Instance.EmsgTocs__AskBlackListOnlineInfo();
        }
    }

}