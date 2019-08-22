using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.GameData;
using BlGame.GameEntity;
using BlGame.GuideDate;
using BlGame.Resource;
using BlGame.Model;
using BlGame.Ctrl;
using BlGame.View;
using System;
using System.Linq;

namespace BlGame.GameState
{
    class PlayState : IGameState
    {
        GameStateType stateTo;

        GameObject mScenesRoot;
        GameObject mUIRoot;

        //地图相机类型
        public int mCameraType = 1;
        public int ShopID = 0;

        public PlayState()
        {
        }

        public GameStateType GetStateType()
        {
            return GameStateType.GS_Play;
        }

        public void SetStateTo(GameStateType gs)
        {
            stateTo = gs;
        }

        public void Enter()
        {
            SetStateTo(GameStateType.GS_Continue);
          
            IGuideMidMatchTip.Instance.RegisterListerner();

            switch (PlayerManager.Instance.LocalAccount.ObType)
            {
                case ObPlayerOrPlayer.PlayerType:
                    GamePlayCtrl.Instance.Enter();
                    break;
                case ObPlayerOrPlayer.PlayerObType:
                    mUIRoot = LoadUiResource.LoadRes(GameMethod.GetUiCamera.transform, GameConstDefine.LoadGameViewer);
                    break;
            }
            
            JxBlGame.Instance.PlayStart();

            CGLCtrl_GameLogic.Instance.AskLoadComplete();
           
            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.GetMapBgAudio((MAPTYPE)GameUserModel.Instance.GameMapID), ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;

            AudioManager.Instance.PlayBgAudio(clip);
            
            //正常流程
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_GameOver, OnGameOver);

            //新手引导流程
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_Loading, OnEvent);

            EventCenter.AddListener(EGameEvent.eGameEvent_ConnectServerFail, OnConnectServerFail);

            EventCenter.AddListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff);

            EventCenter.AddListener(EGameEvent.eGameEvent_ReconnectToBatttle, OnReconnectToBatttle);     
  
            //获取地图信息
            GetMapType();
        }

        public void GetMapType()
        {
            if (UIGuideModel.Instance.GuideType == GCToCS.AskCSCreateGuideBattle.guidetype.first)
            {
                UIGuideCtrl.Instance.Enter();
                GamePlayGuideModel.Instance.NowTaskId = 90004;
                EventCenter.Broadcast(EGameEvent.eGameEvent_PlayTaskModelFinish);
            }
            else if (UIGuideModel.Instance.GuideType == GCToCS.AskCSCreateGuideBattle.guidetype.second)
            {
                AdvancedGuideCtrl.Instance.Enter();
                UIGuideCtrl.Instance.GuideBattleType(GCToCS.AskCSCreateGuideBattle.guidetype.other);
            }
            //获取当前游戏地图数据
            string levelName = Application.loadedLevelName;
            MapInfo mapInfo = MapLoadConfig.Instance.GetMapInfo(levelName);
            if (mapInfo == null)
                Debug.LogError("can not find levelName in MapLoadCfg.xml!");

            mCameraType = mapInfo.mCameraType;
            GamePlayCtrl.Instance.SetShopID(mapInfo.mShopID);
        }

        public void Exit()
        {
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_GameOver, OnGameOver);

            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_Loading, OnEvent);

            EventCenter.RemoveListener(EGameEvent.eGameEvent_ConnectServerFail, OnConnectServerFail);

            EventCenter.RemoveListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff);

            EventCenter.RemoveListener(EGameEvent.eGameEvent_ReconnectToBatttle, OnReconnectToBatttle);     

            LoadUiResource.DestroyLoad(mUIRoot);

            GamePlayCtrl.Instance.Exit();
        }

        public void FixedUpdate(float fixedDeltaTime)
        {

        }

        public GameStateType Update(float fDeltaTime)
        {
            return stateTo;
        }

        private void OnGameOver(UInt64 BaseGuid)
        {
            PlayFinishVedio(BaseGuid);

            GameStateManager.Instance.ChangeGameStateTo(GameStateType.GS_Over);
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

            }
        }

        private void OnConnectServerFail()
        {
            EventCenter.Broadcast<EMessageType>(EGameEvent.eGameEvent_ShowMessage, EMessageType.EMT_Reconnect);
        }

        private void SdkLogOff()
        {
            GameMethod.LogOutToLogin();

            LoadingState lState = GameStateManager.Instance.getState(GameStateType.GS_Loading) as LoadingState;
            lState.SetNextState(GameStateType.GS_Login);
            lState.SetFrontScenes(View.EScenesType.EST_Play);
            SetStateTo(GameStateType.GS_Loading);
        }

        private void OnReconnectToBatttle()
        {
            CGLCtrl_GameLogic.Instance.EmsgToss_AskEnterBattle(GameUserModel.Instance.GameBattleID);
        }

        //播放特效
        void PlayFinishVedio(UInt64 ObjID)
        {
            BlGame.AudioManager.Instance.StopHeroAudio();
            GameTimeData.Instance.EndCountTime();
            ProgressBarInterface.hideProgressBar();
            //UIPlay.OpenUIPlay(false);
            //if (UIDeathInterface.Instance != null)
            //{
            //    GameObject.DestroyImmediate(UIDeathInterface.Instance.gameObject);
            //}
			DeathCtrl.Instance.Exit();
            Iselfplayer player = PlayerManager.Instance.LocalPlayer;
            if (player != null)
            {
                player.RemoveRuinWarning();
            }

            GameMethod.CreateWindow(GameConstDefine.GameOverBoxPath, Vector3.zero, GameMethod.GetUiCamera.transform);
            if (Camera.main != null && Camera.main.gameObject != null)
            {
                Camera.main.gameObject.AddComponent<BaseDaBomb>();
            }
            GameMethod.GetMainCamera.target = null;
            GameMethod.GetMainCamera.enabled = false;

            //ToReview wincamp没用上
            UInt64 sGUID;
            sGUID = ObjID;
            Ientity entity = EntityManager.Instance.GetEntity(sGUID);

            for (int i = EntityManager.AllEntitys.Count - 1; i >= 0; i--)
            {
                var item = EntityManager.AllEntitys.ElementAt(i);
                if (item.Value.RealEntity != null)
                {
                    item.Value.RealEntity.PlayerFreeAnimation();
                    item.Value.RealEntity.SyncEntity = null;
					item.Value.RealEntity.enabled = false;
                }
                if (item.Value.entityType == EntityType.Player || item.Value.entityType == EntityType.Building)
                    continue;
                EntityManager.AllEntitys.Remove(item.Key);
            }

            if (entity != null)
            {
                BaseDaBomb.Instance.SetBaseBomb(true, entity, GameUserModel.Instance.GameMapID);    //ToReview int->uint
            }

            AltarData.Instance.DelAllAltar();
            BattleingData.Instance.ClearAllGoods();
            BattleingData.Instance.ClearAllBattleData();
        }
    }
}


