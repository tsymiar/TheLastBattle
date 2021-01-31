using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Resource;
using BlGame.Ctrl;

namespace BlGame.GameState
{
    class UserState : IGameState
    {
        GameStateType stateTo;

        GameObject mScenesRoot;

        public UserState()
        {
        }

        public GameStateType GetStateType()
        {
            return GameStateType.GS_User;
        }

        public void SetStateTo(GameStateType gs)
        {
            stateTo = gs;
        }

        public void Enter()
        {
            SetStateTo(GameStateType.GS_Continue);

            EventCenter.Broadcast(EGameEvent.eGameEvent_UserEnter);

            //AudioClip clip = Resources.Load(AudioDefine.PATH_UIBGSOUND) as AudioClip;
            //AudioManager.Instance.PlayBgAudio(clip);

            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_UIBGSOUND, ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;

            AudioManager.Instance.PlayBgAudio(clip);

            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_IntoLobby, OnEvent);
            EventCenter.AddListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff);
           
        }

        private void SdkLogOff()
        {
            GameMethod.LogOutToLogin();

            SetStateTo(GameStateType.GS_Login);
        }

        public void Exit()
        {
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_IntoLobby, OnEvent);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff);

            EventCenter.Broadcast(EGameEvent.eGameEvent_UserExit);
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
                case EGameEvent.eGameEvent_IntoLobby:
                    GameStateManager.Instance.ChangeGameStateTo(GameStateType.GS_Lobby);
                    break;            
            }
        }
    }
}


