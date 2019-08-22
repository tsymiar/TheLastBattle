using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using UnityEngine;
using GameDefine;
using BlGame.GameData;
using BlGame.GameEntity;
using JT.FWW.Tools;
using BlGame.Model;

namespace BlGame.GuideDate

{
    public class SceneGuideTaskManager 
    {
       
        private static SceneGuideTaskManager instance = null;

        public SceneGuideTaskBase sceneGuideTask
        { 
            private set;
            get;
        }

        public enum SceneGuideType { 
            PrimaryGuide = 1,
            MiddleGuide,
            NoGuide,
        }


        private const int primaryGuideTaskMap = 1000;
        private const int middleGuideTaskMap = 1005;
        public const string guideUiPath = "Guide/UINewsGuide";
        public static SceneGuideTaskManager Instance() {
            if (instance == null) {
                instance = new SceneGuideTaskManager();
            }
            return instance;
        }
        public SceneGuideType IsNewsGuide(int mapId)
        {
            if (mapId == (int)MAPTYPE.NEWS_GUIDE_1)
                return SceneGuideType.PrimaryGuide;            
            return SceneGuideType.NoGuide;
        }

        public SceneGuideType IsNewsGuide()
        {
            switch ((int)GameUserModel.Instance.GameMapID) 
            {
                case primaryGuideTaskMap:
                    return SceneGuideType.PrimaryGuide;
                case middleGuideTaskMap:
                    return SceneGuideType.MiddleGuide;
                default:
                    return SceneGuideType.NoGuide; 
            } 
        }       
        
        public void InitSenseTaskData() {
            switch ((int)GameUserModel.Instance.GameMapID)//记得改回去-2014-06-26
            //switch (primaryGuideTaskMap)
            {
                case primaryGuideTaskMap:
                    if (sceneGuideTask == null)
                    {
                        sceneGuideTask = new PrimaryGuideTask();
                    }
                    break;
                case middleGuideTaskMap:
                    break;
            }
            if (UINewsGuide.Instance == null)
            { 
                LoadUiResource.LoadRes(GameMethod.GetUiCamera.transform, guideUiPath); 
            }
            sceneGuideTask.InitSceneTaskData();
        }

        public void StartAsignedStep(int taskId)
        {
            IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideNewsGuideEnd);  
            sceneGuideTask.SetTaskStart(taskId);
        }

        public void FinishAll() {           
            sceneGuideTask = null; 
        }

        public void FinishAsignedStep(int taskId) {
            if (taskId == 1002) {
                AddPlayerBornVoice(PlayerManager.Instance.LocalPlayer.NpcGUIDType);
            }
            CGLCtrl_GameLogic.Instance.EmsgToss_GuideFinishStep(taskId);
        }

        public void OnUpdate() {
            if (sceneGuideTask != null) {
                sceneGuideTask.OnUpdate();
            }
        }

        public void ReConnectTask(int step)
        {
            if (sceneGuideTask != null) {
                sceneGuideTask.ReConnectTask(step);
            }
        }

        public void RebornTask() {
            if (sceneGuideTask != null)
            {
                sceneGuideTask.DoRebornTask();
            }
        }

        private SceneGuideTaskManager()
        {
            sceneGuideTask = null;
        }

         public bool CheckObstructTaskCanMove(Vector3 pos) {
             if (IsNewsGuide() == SceneGuideType.NoGuide)
                 return true;
             if (sceneGuideTask == null)
                 return true;
             return sceneGuideTask.CheckObstructTask(pos);             
        }

         private bool hasFinishAllGuide = false;
         public void SetHasFinishedAllGuide(bool finish) {
             hasFinishAllGuide = finish;
         }

         public bool HasFinishAllGuide() {
             return hasFinishAllGuide;
         }

         void AddPlayerBornVoice(int npcId) {
             if (IsNewsGuide() != SceneGuideType.PrimaryGuide || sceneGuideTask == null)
                 return;
             PrimaryGuideTask task = (PrimaryGuideTask)sceneGuideTask;
             task.AddHeroVoice(npcId);
         }
         public Dictionary<UInt64, Ientity> fakeHeroDic = new Dictionary<UInt64, Ientity>();

         //public Dictionary<s, Ientity> fakeHeroDic = new Dictionary<int, Ientity>();

         public void AddFakeHero(Ientity entity)
         {
             if (fakeHeroDic.ContainsKey(entity.GameObjGUID))
             {
                 fakeHeroDic[entity.GameObjGUID] = entity;
             }
             else
             {
                 fakeHeroDic.Add(entity.GameObjGUID, entity);
             }
         }

         public bool IsFakeHero(int npcId) {
             if (21017 == npcId || 21025 == npcId || 21024 == npcId)
                 return true;
             return false;
         }

         public Ientity GetFakeHero(UInt64 guid) { 
             Ientity entity = null;
             if (fakeHeroDic.TryGetValue(guid, out entity))
             {
                 return entity;
             }
             return null;
         }

         public string GetFakeHeroName(int npcId) {
             switch (npcId)
             { 
                 case 21017:
                     return "伊芙蕾";
                 case 21025:
                     return "半人马酋长";
                 case 21024:
                     return "冷面黑寡妇";
             }
             return null;
         }
        
    }
}
