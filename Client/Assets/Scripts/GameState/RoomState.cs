using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.GameData;
using BlGame.Resource;
using BlGame.Ctrl;
using System;
using BlGame.View;

namespace BlGame.GameState
{
    class RoomState : IGameState
    {
        GameStateType stateTo;

        GameObject mScenesRoot;

        GameObject mUIRoot;

        public RoomState()
        {
        }

        public GameStateType GetStateType()
        {
            return GameStateType.GS_Room;
        }

        public void SetStateTo(GameStateType gs)
        {
            stateTo = gs;
        }

        public void Enter()
        {
            SetStateTo(GameStateType.GS_Continue);

            RoomCtrl.Instance.Enter();

            //AudioClip clip = Resources.Load(AudioDefine.PATH_UIBGSOUND) as AudioClip;
            //AudioManager.Instance.PlayBgAudio(clip);
            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_UIBGSOUND, ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;

            AudioManager.Instance.PlayBgAudio(clip);
            
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_RoomBack, OnEvent);
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_IntoHero, OnEvent);
            EventCenter.AddListener<UInt64,string>(EGameEvent.eGameEvent_InviteCreate, InviteAddFriend);
            EventCenter.AddListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff);
            EventCenter.AddListener(EGameEvent.eGameEvent_ConnectServerFail, OnConnectServerFail);
        }

        private void InviteAddFriend(UInt64 sGUID,string temp)
        {
            if (InviteCtrl.Instance.AddDic(sGUID, temp) && InviteCtrl.Instance.InvatiDic.Count > 1)
            {
                InviteCtrl.Instance.ChangeInvite(sGUID, temp);
            }
            else
                InviteCtrl.Instance.Enter(sGUID, temp);
        }

        public void Exit()
        {

            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_RoomBack, OnEvent);
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_IntoHero, OnEvent);
            EventCenter.RemoveListener<UInt64, string>(EGameEvent.eGameEvent_InviteCreate, InviteAddFriend);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ConnectServerFail, OnConnectServerFail);
            RoomCtrl.Instance.Exit();
        }

        public void FixedUpdate(float fixedDeltaTime)
        {

        }

        public GameStateType Update(float fDeltaTime)
        {
            return stateTo;
        }

        public void OnEvent(CEvent evt)
        {
            switch (evt.GetEventId())
            {
                case EGameEvent.eGameEvent_RoomBack:
                    SetStateTo(GameStateType.GS_Lobby);
                    break;
                case EGameEvent.eGameEvent_IntoHero:
                    SetStateTo(GameStateType.GS_Hero);
                    break;
            }
        }

        private void SdkLogOff()
        {
            GameMethod.LogOutToLogin();

            SetStateTo(GameStateType.GS_Login);
        }

        private void OnConnectServerFail()
        {
            EventCenter.Broadcast<EMessageType>(EGameEvent.eGameEvent_ShowMessage, EMessageType.EMT_Disconnect);
        }
    }
}


