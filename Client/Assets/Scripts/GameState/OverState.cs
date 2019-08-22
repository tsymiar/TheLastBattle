using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Effect;
using BlGame.Resource;
using BlGame.Ctrl;
using BlGame.View;
using BlGame.GameEntity;
using BlGame.Model;

namespace BlGame.GameState
{
    class OverState : IGameState
    {
        GameStateType stateTo;

        GameObject mScenesRoot;

        GameObject mUIRoot;

        float mTime;
        bool mNeedUpdate;
        bool mNeedScore;

        public OverState()
        {
        }

        public GameStateType GetStateType()
        {
            return GameStateType.GS_Over;
        }

        public void SetStateTo(GameStateType gs)
        {
            stateTo = gs;
        }

        public void Enter()
        {
            SetStateTo(GameStateType.GS_Continue);

            mTime = 0;

            mNeedUpdate = true;
            mNeedScore = true;

            //AudioClip clip = Resources.Load(AudioDefine.PATH_UIBGSOUND) as AudioClip;
            //AudioManager.Instance.PlayBgAudio(clip);

            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.GetMapBgAudio((MAPTYPE)GameUserModel.Instance.GameMapID), ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;

            AudioManager.Instance.PlayBgAudio(clip);

            AdvancedGuideCtrl.Instance.Exit();


            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_Loading, OnEvent);
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_IntoRoom, OnEvent);
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_IntoLobby, OnEvent);
            EventCenter.AddListener(EGameEvent.eGameEvent_ConnectServerFail, OnConnectServerFail);
        }

        public void Exit()
        {
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_Loading, OnEvent);
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_IntoRoom, OnEvent);
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_IntoLobby, OnEvent);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ConnectServerFail, OnConnectServerFail);

            //GameMethod.DisableAllUI();

            GameMethod.RemoveUI("superwan(Clone)");
            PlayerManager.Instance.LocalPlayer.CleanWhenGameOver();
            JxBlGame.Instance.OpenConnectUI();
            GameObjectPool.Instance.Clear();
        }

        public void FixedUpdate(float fixedDeltaTime)
        {
        }

        public GameStateType Update(float fDeltaTime)
        {
            if (!mNeedUpdate)
                return stateTo;

            mTime += fDeltaTime;
            if (UIGuideModel.Instance.GuideType == GCToCS.AskCSCreateGuideBattle.guidetype.first)
            {
                if (mTime > 6)
                {
                    CGLCtrl_GameLogic.Instance.EmsgToss_AskReEnterRoom();
                    mNeedUpdate = false;
                    UIGuideCtrl.Instance.GuideBattleType(GCToCS.AskCSCreateGuideBattle.guidetype.other);
                }
            }
            else
            {
                if (mTime > 18)
                {
                    ScoreCtrl.Instance.Exit();
                    CGLCtrl_GameLogic.Instance.EmsgToss_AskReEnterRoom();
                    mNeedUpdate = false;
                }
                else if (mNeedScore && mTime > 6)
                {
                    ScoreCtrl.Instance.Enter();
                    mNeedScore = false;
                }
            }
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
                        lState.SetFrontScenes(View.EScenesType.EST_Play);
                        SetStateTo(GameStateType.GS_Loading);
                    }
                    break;
                case EGameEvent.eGameEvent_IntoRoom:
                    {
                        LoadingState lState = GameStateManager.Instance.getState(GameStateType.GS_Loading) as LoadingState;
                        lState.SetNextState(GameStateType.GS_Room);
                        lState.SetFrontScenes(View.EScenesType.EST_Play);
                        SetStateTo(GameStateType.GS_Loading);
                    }
                    break;
                case EGameEvent.eGameEvent_IntoLobby:
                    {
                        LoadingState lState = GameStateManager.Instance.getState(GameStateType.GS_Loading) as LoadingState;
                        lState.SetNextState(GameStateType.GS_Lobby);
                        lState.SetFrontScenes(View.EScenesType.EST_Play);
                        SetStateTo(GameStateType.GS_Loading);
                    }
                    break;
            }
        }

        private void OnConnectServerFail()
        {
            EventCenter.Broadcast<EMessageType>(EGameEvent.eGameEvent_ShowMessage, EMessageType.EMT_Disconnect);
        }
    }
}


