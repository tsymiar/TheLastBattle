using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Resource;
using BlGame.Ctrl;

namespace BlGame.GameState
{
    class LoginState : IGameState
    {
        GameStateType stateTo;

        GameObject mScenesRoot;

       // GameObject mUIRoot;
	
		public LoginState()
		{
		}

        public GameStateType GetStateType()
        {
            return GameStateType.GS_Login;
        }

        public void SetStateTo(GameStateType gs)
        {
            stateTo = gs;
        }

        public void Enter()
        {
            SetStateTo(GameStateType.GS_Continue);
         
            //yaz修改            
            //mScenesRoot = GameObject.Instantiate(Resources.Load(GameConstDefine.GameLogin)) as GameObject;
            //mUIRoot = LoadUiResource.LoadRes(GameMethod.GetUiCamera.transform, GameConstDefine.LoadGameLoginUI);
            //AudioClip clip = Resources.Load(AudioDefine.PATH_UIBGSOUND) as AudioClip;

            ResourceUnit sceneRootUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.GameLogin, ResourceType.PREFAB);
            mScenesRoot = GameObject.Instantiate(sceneRootUnit.Asset) as GameObject;

            //mUIRoot = LoadUiResource.LoadRes(GameMethod.GetUiCamera.transform, GameConstDefine.LoadGameLoginUI);

            LoginCtrl.Instance.Enter();
        
            ResourceUnit audioClipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_UIBGSOUND, ResourceType.ASSET);
            AudioClip clip = audioClipUnit.Asset as AudioClip;       

                                
            AudioManager.Instance.PlayBgAudio(clip);

            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_InputUserData, OnEvent);
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
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_InputUserData, OnEvent);
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_IntoLobby, OnEvent);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff);       

            //LoadUiResource.DestroyLoad(mUIRoot);
            LoginCtrl.Instance.Exit();
            GameObject.DestroyImmediate(mScenesRoot);            
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
            UIPlayMovie.PlayMovie("cg.mp4", Color.black, 2/* FullScreenMovieControlMode.Hidden*/, 3/*FullScreenMovieScalingMode.Fill*/);
            switch (evt.GetEventId())
            {
                case EGameEvent.eGameEvent_InputUserData:
                    SetStateTo(GameStateType.GS_User);
                    break;
                case EGameEvent.eGameEvent_IntoLobby:
                    GameStateManager.Instance.ChangeGameStateTo(GameStateType.GS_Lobby);
                    break;
            }
        }
    }
}


