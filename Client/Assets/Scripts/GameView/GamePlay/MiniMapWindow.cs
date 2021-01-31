using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using BlGame.GameEntity;
using GameDefine;
using BlGame.GameData;
using JT.FWW.Tools;
using System.Linq;
using BlGame;
using BlGame.GuideDate;
using BlGame.Ctrl;
using BlGame.Model;

namespace BlGame.View
{
    public class MiniMapWindow : BaseWindow
    {
        public MiniMapWindow() 
        {
            mScenesType = EScenesType.EST_Play;
            mResName = GameConstDefine.LoadMiniMapUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
            mInited = false;
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mMapPanel = mRoot.FindChild("Hold/Panel");

            MapInfo map = MapLoadConfig.Instance.GetMapInfo(GameUserModel.Instance.GameMapID);

            if (map != null)
            {
                string path = "Guis/Play/" + map.mMiniMap;
                mMapGround = LoadUiResource.AddChildObject(mMapPanel, path).transform;
                mMapGround.localPosition = new Vector3(-150, -100, 0);
                mMapGround.localEulerAngles = new Vector3(0, 0, 0);
            }
        }

        //删除Login外其他控件，例如
        public static void DestroyOtherUI()
        {
          
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
            mMapElementDic.Clear();
            mMapWarningDic.Clear();
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_UpdateMiniMap, OnUpdateMapEvent);
            EventCenter.AddListener(EGameEvent.eGameEvent_InitMiniMap, InitGameMap);
            EventCenter.AddListener<Ientity>(EGameEvent.eGameEvent_AddMiniMap, OnAddMapEvent);
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_RemoveMiniMap, OnRemoveMapEvent);
            EventCenter.AddListener<UInt64 , uint , UInt64>(EGameEvent.eGameEvent_BroadcastBeAtk, OnWarningEvent);
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_RemoveMiniWarning, RemoveMapWarning);
            
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_UpdateMiniMap, OnUpdateMapEvent);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_InitMiniMap, InitGameMap);
            EventCenter.RemoveListener<Ientity>(EGameEvent.eGameEvent_AddMiniMap, OnAddMapEvent);
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_RemoveMiniMap, OnRemoveMapEvent);
            EventCenter.RemoveListener<UInt64, uint, UInt64>(EGameEvent.eGameEvent_BroadcastBeAtk, OnWarningEvent);
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_RemoveMiniWarning, RemoveMapWarning);
        }

        //显示
        public override void OnEnable()
        {
            mInited = false;
        }

        //隐藏
        public override void OnDisable()
        {
        }

        private void InitGameMap()
        {
            //清除已有对象
            Clear();

            //地图初始化
            MapInfo map = MapLoadConfig.Instance.GetMapInfo(GameUserModel.Instance.GameMapID);

            if (!mInited && map != null)
            {
                Ientity player = PlayerManager.Instance.LocalPlayer;

                if (map.mCameraType == 1)
                {
                    if (player.EntityCamp == EntityCampType.CampTypeA)
                    {
                        mMapPanel.Rotate(new Vector3(0, 0, 45));
                    }
                    else
                    {
                        mMapPanel.Rotate(new Vector3(0, 0, 225));
                    }
                    mMapPanel.localPosition = new Vector3(-120, -120, 0);
                }
                else
                {
                    if (player.EntityCamp == EntityCampType.CampTypeA)
                    {
                        mMapPanel.Rotate(new Vector3(0, 0, 0));
                    }
                    else
                    {
                        mMapPanel.Rotate(new Vector3(0, 0, 180));
                    }
                    mMapPanel.localPosition = new Vector3(-130, -66, 0);
                }
            }

            mInited = true;

            //加入已经存在的对象，比如静态NPC
            foreach (Ientity entity in EntityManager.AllEntitys.Values)
            {
                OnAddMapEvent(entity);
            }
          
        }



        private Transform mMapGround;

        private Transform mMapPanel;

        private Dictionary<UInt64, UIMiniMapElement> mMapElementDic = new Dictionary<UInt64, UIMiniMapElement>();

        private Dictionary<UInt64, UIMiniMapInterfaceWarning> mMapWarningDic = new Dictionary<UInt64, UIMiniMapInterfaceWarning>();

       
        private float mTimeBuildingWarning = 0f;
        private const float mWarningVoice = 20f;
        private const float mWarningSound = 10f;
        private bool mIsFirstAttack = false;
        private bool hasPlayWaringVoice = false;
        private bool hasPlayWarningSound = false;
        private bool mIsFirstWarning = false;

      

        private bool mInited = false;

        /// <summary>
        /// 增加地图元素
        /// </summary>
        /// <param name="entity"></param>
        private void AddMapElement(UInt64 guid, EntityCampType type, float x, float y, float z)
        {
            if (!mInited)
                return;

            int index = (int)type;
            if (index <= (int)EntityCampType.CampTypeNull)
                return;
            UIMiniMapElement element = GetMapElement(guid);
            if (element == null)
            {
                element = CreateMapElement(guid, type, x, y, z);
                if (element == null)
                    return;
                mMapElementDic.Add(guid, element);

                EventCenter.Broadcast(EGameEvent.eGameEvent_ChangeMapState);
            }
        }

        /// <summary>
        /// 删除地图元素
        /// </summary>
        ///</param>
        private void DestroyMapElement(UInt64 guid)
        {
            UIMiniMapElement element = GetMapElement(guid);
            if (element == null)
            {
                return;
            }
            GameObjectPool.Instance.ReleaseGO(element.resPath, element.gameObject, PoolObjectType.POT_MiniMap);

            mMapElementDic.Remove(guid);

            EventCenter.Broadcast(EGameEvent.eGameEvent_ChangeMapState);
        }

        /// <summary>
        /// 更新地图元素
        /// </summary>
        /// <param name="entity"></param>
        private void UpdateMapElement(UInt64 guid, EntityCampType type, float x, float y, float z)
        {
            int index = (int)type;
            if (index <= (int)EntityCampType.CampTypeKind)
                return;
            UIMiniMapElement element = GetMapElement(guid);
            if (element != null)
            {
                element.UpdatePosDirect(x, y, z);
            }
        }

        private void AddMapWarning(UInt64 guid)
        {
            if (!EntityManager.AllEntitys.ContainsKey(guid))
                return;
            Ientity entity = EntityManager.AllEntitys[guid];
            if (entity == null || entity.realObject == null || !entity.realObject.activeInHierarchy)
                return;
            bool isAllow = false;

            if (entity.entityType != EntityType.Player && entity.entityType != EntityType.Soldier && entity.entityType != EntityType.AltarSoldier)
            {
                isAllow = false;
            }
            if (SceneGuideTaskManager.Instance().IsNewsGuide() != SceneGuideTaskManager.SceneGuideType.NoGuide && (entity.NpcGUIDType == 21017
             || entity.NpcGUIDType == 21025 || entity.NpcGUIDType == 21024))
            {
                isAllow = true;
            }
            if (!isAllow)
                return;
            if (entity.FSM != null && entity.FSM.State == BlGame.FSM.FsmState.FSM_STATE_DEAD)
                return;
            UIMiniMapInterfaceWarning element = GetMapWarning(guid);
            Vector3 pos = entity.realObject.transform.position;
            if (element == null)
            {
                element = CreateMapElementWarning(guid, pos.x, pos.y, pos.z);
                mMapWarningDic.Add(guid, element);
            }
            else
            {
                element.UpdatePosDirect(pos.x, pos.y, pos.z);
            }
        }

        private void AddBuildingWarningVoice(UInt64 sGUID)
        {
            if (!EntityManager.AllEntitys.ContainsKey(sGUID))
                return;

            Ientity entity = EntityManager.AllEntitys[sGUID];
            if (!entity.IfNPC())
                return;

            if (entity == null || entity.realObject == null || entity.entityType != EntityType.Building)
                return;
            if (entity.FSM != null && entity.FSM.State == BlGame.FSM.FsmState.FSM_STATE_DEAD)
                return;
            /*
            if (!mIsFirstAttack)
            {
                LoadSound(GameDefine.GameConstDefine.WarningBuildingVoice);
                mIsFirstAttack = true;
                return;
            }
            if (mTimeBuildingWarning == 0)
            {
                mTimeBuildingWarning = Time.time;
            }

            if (Time.time - mTimeBuildingWarning >= mWarningVoice)
            {
                Debug.Log("our base is under attack语音提示");
                LoadSound(GameDefine.GameConstDefine.WarningBuildingVoice);
                mTimeBuildingWarning = Time.time;
            }

            else if (Time.time - mTimeBuildingWarning >= mWarningSound)
            {
                Debug.Log("minimap waring警告声音");
                LoadSound(GameDefine.GameConstDefine.WarningBuildingSound);
                mTimeBuildingWarning = Time.time;
            }
             **/


            if(!mIsFirstAttack)
            {
                LoadSound(GameDefine.GameConstDefine.WarningBuildingVoice);
               
                mIsFirstAttack = true;
                hasPlayWaringVoice = true;
                //Debug.Log("mIsFirstAttack");

                return;
            }

            if (mTimeBuildingWarning == 0)
            {
                mTimeBuildingWarning = Time.time;
            }

            if (Time.time - mTimeBuildingWarning >= mWarningVoice)
            {
                LoadSound(GameDefine.GameConstDefine.WarningBuildingVoice);
                mTimeBuildingWarning = Time.time;
            
                //Debug.Log("mWarningVoice");
                hasPlayWaringVoice = true;
                hasPlayWarningSound = true;
            }


            //二十秒内受到攻击则播放警告音效
             if ((Time.time - mTimeBuildingWarning <= mWarningSound) && (hasPlayWaringVoice) && (!mIsFirstWarning))
            {

                LoadSound(GameDefine.GameConstDefine.WarningBuildingSound);
               
                mTimeBuildingWarning = Time.time;
            
                hasPlayWarningSound = true;
                mIsFirstWarning = true;
                //Debug.Log("WarningBuildSound 二十秒内受到攻击则播放警告音效");
            }

            //持续受到攻击时播放警告音效十秒之后才会再次播放
            if ((Time.time - mTimeBuildingWarning > mWarningSound)  && (hasPlayWarningSound) )
            {

                LoadSound(GameDefine.GameConstDefine.WarningBuildingSound);
                mIsFirstWarning = false;
                mTimeBuildingWarning = Time.time;
             
                //Debug.Log("持续受到攻击时播放警告音效十秒之后才会再次播放");
            }

        }

        private void LoadSound(string audio)
        {
            AudioClip clip = Resources.Load(audio) as AudioClip;
            AudioSource source = AudioManager.Instance.PlayEffectAudio(clip);
            source.volume = 1f;
        }

        /// <summary>
        /// 移出地图元素
        /// </summary>
        /// <param name="entity"></param>
        private void RemoveMapWarning(UInt64 guid)
        {
            UIMiniMapInterfaceWarning element = GetMapWarning(guid);
            if (element == null)
            {
                return;
            }
            GameObjectPool.Instance.ReleaseGO(element.resPath, element.gameObject, PoolObjectType.POT_MiniMap);

            mMapWarningDic.Remove(guid);

            EventCenter.Broadcast(EGameEvent.eGameEvent_ChangeMapState);
        }

        /// <summary>
        /// 获得Ientity所对应的地图元素
        /// </summary>
        /// <param name="entity"></param>
        /// <returns></returns>
        private UIMiniMapElement GetMapElement(UInt64 guid)
        {
            if (mMapElementDic == null || mMapElementDic.Count == 0)
                return null;
            UIMiniMapElement element = null;
            mMapElementDic.TryGetValue(guid, out element);
            return element;

        }

        private UIMiniMapInterfaceWarning GetMapWarning(UInt64 guid)
        {
            if (mMapWarningDic == null || mMapWarningDic.Count == 0)
                return null;
            UIMiniMapInterfaceWarning element = null;
            mMapWarningDic.TryGetValue(guid, out element);
            return element;
        }

        private void Clear()
        {
            for (int i = mMapElementDic.Count - 1; i >= 0; i--)
            {
                if (mMapElementDic.ElementAt(i).Value == null)
                    continue;

                GameObjectPool.Instance.ReleaseGO(mMapElementDic.ElementAt(i).Value.resPath, mMapElementDic.ElementAt(i).Value.gameObject, PoolObjectType.POT_MiniMap);
            }
            mMapElementDic.Clear();
        }

        /// <summary>
        /// 创建地图元素图标
        /// </summary>
        /// <param name="entity"></param>
        /// <returns></returns>
        private UIMiniMapElement CreateMapElement(UInt64 guid, EntityCampType type, float x, float y, float z)
        {
            string path = null;

            bool blueTeam = true;
            if (PlayerManager.Instance.LocalPlayer!=null && type != PlayerManager.Instance.LocalPlayer.EntityCamp)
                blueTeam = false;

            Ientity entity = EntityManager.AllEntitys[guid];

            if (entity.entityType == EntityType.Player)
            {
                path = GetPlayerElementPath(blueTeam);
            }
            else if (entity.IfNPC())
            {
                path = GetNpcElementPath(entity,blueTeam);
            }
            if (string.IsNullOrEmpty(path))
                return null;
            return LoadElementResource(path, guid, x, y, z);
        }

        private string GetPlayerElementPath(bool blueTeam)
        {
            string path;
            if (blueTeam)
                path = GameDefine.GameConstDefine.PlayerMapGameObject_Green;
            else
                path = GameDefine.GameConstDefine.PlayerMapGameObject_Red;
            return path;
        }

        private string GetNpcElementPath(Ientity entity,bool blueTeam)
        {
            string path = null;

            if (entity.entityType == EntityType.Building || entity.entityType == EntityType.Monster)
            {
                if (entity.NPCCateChild == ENPCCateChild.eNPCChild_SmallMonster)
                {
                    path = GameDefine.GameConstDefine.NpcMinimapCreepsIconSmall;
                }
                else if (entity.NPCCateChild == ENPCCateChild.eNPCChild_HugeMonster)
                {
                    path = GameDefine.GameConstDefine.NpcMinimapCreepsIconHuge;
                }
                else if (entity.NPCCateChild == ENPCCateChild.eNPCChild_BUILD_Altar)
                {
                    path = GameDefine.GameConstDefine.NpcMinimapBarracksIcon;
                }
                else if (entity.NPCCateChild == ENPCCateChild.eNPCChild_BUILD_Base)
                {
                    path = GameDefine.GameConstDefine.NpcMinimapBaseIcon;
                }
                else if (entity.NPCCateChild == ENPCCateChild.eNPCChild_BUILD_Shop)
                {
                    path = GameDefine.GameConstDefine.NpcMinimapShopIcon;
                }
                else if (entity.NPCCateChild == ENPCCateChild.eNPCChild_BUILD_Tower)
                {
                    path = GameDefine.GameConstDefine.NpcMinimapTowerIcon;
                }
            }
            else
            {
                if (blueTeam)
                    path = GameDefine.GameConstDefine.SoldierMapGameObject_Green;
                else
                    path = GameDefine.GameConstDefine.SoldierMapGameObject_Red;
            }
          
            return path;
        }


        private UIMiniMapInterfaceWarning CreateMapElementWarning(UInt64 guid, float x, float y, float z)
        {
            string path = null;
            path = GameDefine.GameConstDefine.MapWarning;
            return (UIMiniMapInterfaceWarning)LoadElementResource(path, guid, x, y, z);
        }

        private UIMiniMapElement LoadElementResource(string path, UInt64 guid, float x, float y, float z)
        {
            GameObject obj;
            obj = GameObjectPool.Instance.GetGO(path);
            obj.transform.parent = mMapGround.transform;
            obj.transform.localScale = Vector3.one;
            UIMiniMapElement element = obj.GetComponent<UIMiniMapElement>();
            element.resPath = path;
            element.CreateMiniMapElement(guid, x, y, z);
            obj.SetActive(false);
            obj.SetActive(true);
            return element;
        }

        private void OnWarningEvent(UInt64 ownerId, uint skillID, UInt64 targetID)
        {
            SkillAccountConfig skillAccConfig = ConfigReader.GetSkillAccountCfg(skillID);
            if (skillAccConfig == null || !skillAccConfig.isDmgEffect)
                return;
            if (!EntityManager.AllEntitys.ContainsKey(targetID))
                return;
            Ientity entity = EntityManager.AllEntitys[targetID];
            if (PlayerManager.Instance.LocalPlayer == null)
                return;
            if (!entity.IsSameCamp(PlayerManager.Instance.LocalPlayer.EntityCamp))
                return;
            if (entity == null || entity.realObject == null || !entity.realObject.activeInHierarchy)
                return;
            if (entity.entityType != EntityType.Building && entity.entityType != EntityType.Player)
            {
                if (SceneGuideTaskManager.Instance().IsNewsGuide() == SceneGuideTaskManager.SceneGuideType.NoGuide)
                {
                    return;
                }
                else
                {
                    if (entity.NpcGUIDType != 21017)//新手引导己方精灵女
                        return;
                }
            }
            AddMapWarning(targetID);
            AddBuildingWarningVoice(targetID);
        }

        private void OnUpdateMapEvent()
        {
            for (int i = EntityManager.AllEntitys.Count - 1; i >= 0; i--)
            {
                Ientity entity = EntityManager.AllEntitys.ElementAt(i).Value;
                if (entity == null || entity.realObject == null || !entity.realObject.activeInHierarchy)
                    continue;
                if (entity.entityType != EntityType.Player && entity.entityType != EntityType.Soldier && entity.entityType != EntityType.AltarSoldier)
                    continue;

                //死亡删除
                if (entity.FSM != null && entity.FSM.State == BlGame.FSM.FsmState.FSM_STATE_DEAD)
                {
                    OnRemoveMapEvent(entity.GameObjGUID);
                    continue;
                }

                //复活增加
                if (!mMapElementDic.ContainsKey(entity.GameObjGUID))
                {
                    OnAddMapEvent(entity);
                }

                Vector3 pos = entity.realObject.transform.position;
                UpdateMapElement(EntityManager.AllEntitys.ElementAt(i).Key, entity.EntityCamp, pos.x, pos.y, pos.z);
            }
        }

        private void OnAddMapEvent(Ientity entity)
        {
            if (entity == null || entity.realObject == null || !entity.realObject.activeInHierarchy)
                return;

            if (entity.entityType != EntityType.Player && entity.entityType != EntityType.Soldier && entity.entityType != EntityType.AltarSoldier && entity.entityType != EntityType.Building && entity.entityType != EntityType.Monster)
                return;

            if (entity.FSM != null && entity.FSM.State == BlGame.FSM.FsmState.FSM_STATE_DEAD)
            {
                return;
            }

            Vector3 pos = entity.realObject.transform.position;
            AddMapElement(entity.GameObjGUID, entity.EntityCamp, pos.x, pos.y, pos.z);
        }

        private void OnRemoveMapEvent(UInt64 guid)
        {
            DestroyMapElement(guid);
        }
    }
}