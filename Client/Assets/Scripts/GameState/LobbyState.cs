using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Resource;
using BlGame.Ctrl;
using System;
using BlGame.View;

namespace BlGame.GameState
{
    class LobbyState : IGameState
    {
        GameStateType stateTo;

        GameObject mScenesRoot;

        GameObject mUIRoot;

        public LobbyState()
        {
        }

        public GameStateType GetStateType()
        {
            return GameStateType.GS_Lobby;
        }

        public void SetStateTo(GameStateType gs)
        {
            stateTo = gs;
        }

        public void Enter()
        {
            SetStateTo(GameStateType.GS_Continue);

            LobbyCtrl.Instance.Enter();

            //AudioClip clip = Resources.Load(AudioDefine.PATH_UIBGSOUND) as AudioClip;
            //AudioManager.Instance.PlayBgAudio(clip);

            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_UIBGSOUND, ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;
            
            AudioManager.Instance.PlayBgAudio(clip);
            
            if (!AudioManager.Instance.LoopAudioSource.isPlaying)
            {
                AudioManager.Instance.LoopAudioSource.Play();
            }

             EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_IntoRoom, OnEvent);
             EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_Loading, OnEvent);
             EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_IntoHero, OnEvent);
             EventCenter.AddListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff); 
             EventCenter.AddListener<UInt64,string>(EGameEvent.eGameEvent_InviteCreate, InviteAddFriend);
             EventCenter.AddListener<string>(EGameEvent.eGameEvent_InviteAddRoom, InviteAddRoom);
             EventCenter.AddListener(EGameEvent.eGameEvent_ReconnectToBatttle, ReconnectToBatttle);    
            
        }

        private void SdkLogOff()
        {
            GameMethod.LogOutToLogin();
            SetStateTo(GameStateType.GS_Login);
        }

        private void InviteAddRoom(string arg1)
        {
            InviteRoomCtrl.Instance.Enter();
        }

        private void InviteAddFriend(UInt64 sGUID ,string temp)
        {
            if (InviteCtrl.Instance.AddDic(sGUID, temp) && InviteCtrl.Instance.InvatiDic.Count > 1)
            {
                InviteCtrl.Instance.ChangeInvite(sGUID, temp);
            }else
                InviteCtrl.Instance.Enter(sGUID,temp);
        }

        public void Exit()
        {
            LobbyCtrl.Instance.Exit();

            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_IntoRoom, OnEvent);
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_Loading, OnEvent);
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_IntoHero, OnEvent);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff);            
            EventCenter.RemoveListener<UInt64,string>(EGameEvent.eGameEvent_InviteCreate, InviteAddFriend);
            EventCenter.RemoveListener<string>(EGameEvent.eGameEvent_InviteAddRoom, InviteAddRoom);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ReconnectToBatttle, ReconnectToBatttle);    

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
                case EGameEvent.eGameEvent_IntoRoom:
                    SetStateTo(GameStateType.GS_Room);
                    break;
                case EGameEvent.eGameEvent_IntoHero:
                    GameStateManager.Instance.ChangeGameStateTo(GameStateType.GS_Hero);
                    break;
                case EGameEvent.eGameEvent_Loading:
                    GameStateType stateType = (GameStateType)evt.GetParam("NextState");
                    LoadingState lState = GameStateManager.Instance.getState(GameStateType.GS_Loading) as LoadingState;
                    lState.SetNextState(stateType);
                    lState.SetFrontScenes(View.EScenesType.EST_Login);
                    SetStateTo(GameStateType.GS_Loading);
                    break;              
            }
            
        }

        public void ReconnectToBatttle()
        {
            EventCenter.Broadcast<EMessageType>(EGameEvent.eGameEvent_ShowMessage, EMessageType.EMT_ReEnter);
        }
    }
}


