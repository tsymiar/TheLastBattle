using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.GameData;
using BlGame.Resource;
using BlGame.Ctrl;
using BlGame.View;

namespace BlGame.GameState
{
    class HeroState : IGameState
    {
        GameStateType stateTo;

        GameObject mScenesRoot;

        GameObject mUIRoot;

        public HeroState()
        {
        }

        public GameStateType GetStateType()
        {
            return GameStateType.GS_Hero;
        }

        public void SetStateTo(GameStateType gs)
        {
            stateTo = gs;
        }

        public void Enter()
        {
            SetStateTo(GameStateType.GS_Continue);
           

            HeroCtrl.Instance.Enter();

            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_UIBGSOUND, ResourceType.ASSET);
          
            AudioClip clip = clipUnit.Asset as AudioClip;
            AudioManager.Instance.PlayBgAudio(clip);
                                                
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_Loading, OnEvent);
            EventCenter.AddListener(EGameEvent.eGameEvent_ConnectServerFail, OnConnectServerFail);
            EventCenter.AddListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff); 
        }

        public void Exit()
        {
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_Loading, OnEvent);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ConnectServerFail, OnConnectServerFail);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff); 

            HeroCtrl.Instance.Exit();
        }

        public void OnConnectServerFail()
        {
            EventCenter.Broadcast<EMessageType>(EGameEvent.eGameEvent_ShowMessage, EMessageType.EMT_Reconnect);
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
                case EGameEvent.eGameEvent_Loading:
                    {
                        GameStateType stateType = (GameStateType)evt.GetParam("NextState");
                        LoadingState lState = GameStateManager.Instance.getState(GameStateType.GS_Loading) as LoadingState;
                        lState.SetNextState(stateType);
                        lState.SetFrontScenes(View.EScenesType.EST_Login);
                        SetStateTo(GameStateType.GS_Loading);
                    }
                   
                    break;
            }
        }

        private void SdkLogOff()
        {
            GameMethod.LogOutToLogin();
            SetStateTo(GameStateType.GS_Login);
        }
    }
}


