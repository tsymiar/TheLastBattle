using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.GameData;
using System.Threading;
using BlGame.Resource;
using BlGame.View;
using BlGame.Ctrl;
using BlGame.Model;

namespace BlGame.GameState
{
    class LoadingState : IGameState
    {
        GameStateType stateTo;

        GameObject mScenesRoot;

        GameObject mUIRoot;

        public GameStateType mNextState;

        public EScenesType mFrontScenes;

        public LoadingState()
        {
            mNextState = GameStateType.GS_Continue;
        }

        public GameStateType GetStateType()
        {
            return GameStateType.GS_Loading;
        }

        public void SetNextState(GameStateType next)
        {
            mNextState = next;
        }

        public void SetFrontScenes(EScenesType front)
        {
            mFrontScenes = front;
        }

        public void SetStateTo(GameStateType gs)
        {
            stateTo = gs;
        }

        private string GetLoadMapName()
        {
            MapInfo map = MapLoadConfig.Instance.GetMapInfo(GameUserModel.Instance.GameMapID);

            if (map == null)
            {
                Debug.LogError("GetLoadMapName map find fail !!!");
                return "";
            }

            return map.mLoadScene;
        }

        private void LoadFinish(GameStateType state)
        {
            //Timer t = new Timer(this.CallBack, state, 5000, 0);
            SetStateTo(state);
        }

//         public void CallBack(object obj)
//         {
//             GameStateType state = (GameStateType)obj;
//             SetStateTo(state);
//         }

        public void Enter()
        {
            if (mNextState == GameStateType.GS_Continue)
                return;

            SetStateTo(GameStateType.GS_Continue);

            //加载UI
            mUIRoot = GameMethod.LoadProgress();
            LoadScene.Instance.GState = mNextState;
            LoadScene.Instance.OnLoadFinish += LoadFinish;


            //加载场景之前需要进行清除操作
            BlGame.Effect.EffectManager.Instance.DestroyAllEffect();
            //清除GameObjectPool数据
            GameObjectPool.Instance.Clear();

            //加载场景
            if (mNextState == BlGame.GameState.GameStateType.GS_Play)
            {
                LoadScene.Instance.isCloseBySelf = false;
                string name = GetLoadMapName();
                LoadScene.Instance.LoadAsignedSene("Scenes/"+name);

                WindowManager.Instance.ChangeScenseToPlay(mFrontScenes);
            }
            else
            {
                //返回Pvp_Login选人界面需要清除预加载信息                
                ReadPreLoadConfig.Instance.Clear();

                LoadScene.Instance.isCloseBySelf = true;
                LoadScene.Instance.LoadAsignedSene("Scenes/Pvp_Login");

                WindowManager.Instance.ChangeScenseToLogin(mFrontScenes);

            }
            //AudioClip clip = Resources.Load(AudioDefine.PATH_UIBGSOUND) as AudioClip;
            //AudioManager.Instance.PlayBgAudio(clip);

            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_UIBGSOUND, ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;
            AudioManager.Instance.PlayBgAudio(clip);

            EventCenter.AddListener(EGameEvent.eGameEvent_ConnectServerFail, OnConnectServerFail);
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_IntoRoom, OnEvent);
        }

        public void Exit()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ConnectServerFail, OnConnectServerFail);
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_IntoRoom, OnEvent);
        }

        public void FixedUpdate(float fixedDeltaTime)
        {

        }

        public GameStateType Update(float fDeltaTime)
        {
            return stateTo;
        }

        public void OnConnectServerFail()
        {
            EventCenter.Broadcast<EMessageType>(EGameEvent.eGameEvent_ShowMessage, EMessageType.EMT_Reconnect);
        }

        public void OnEvent(CEvent evt)
        {
            switch (evt.GetEventId())
            {
                case EGameEvent.eGameEvent_IntoRoom:
                    {
                        LoadingState lState = GameStateManager.Instance.getState(GameStateType.GS_Loading) as LoadingState;
                        lState.SetNextState(GameStateType.GS_Room);
                        lState.SetFrontScenes(View.EScenesType.EST_Play);
                        SetStateTo(GameStateType.GS_Loading);
                    }
                    break;
            }
        }
    }
}


