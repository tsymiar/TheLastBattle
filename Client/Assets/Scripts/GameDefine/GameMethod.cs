using System;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using UICommon;
using System.Linq;
using AssemblyCSharp;
using BlGame.GameData;
using BlGame.GameEntity;
using BlGame.Network;
using BlGame.GameState;
using System.IO;
using System.Xml;
using System.Text;
using BlGame.Effect;
using BlGame.Resource;
using BlGame.Model;

namespace GameDefine
{
	public static class GameMethod
	{

		public static Camera GetUiCamera
		{
			get{
                if (UICamera.currentCamera == null)
                {
                    UICamera.currentCamera = BlGameUI.Instance.transform.FindChild("Camera").camera;
                }
				return UICamera.currentCamera;
			}
		}

        private static GrayscaleEffect CameraGray;
        public static GrayscaleEffect GetCameraGray
        {
            get
            {
                if (CameraGray == null)
                { 
                    CameraGray = GetMainCamera.GetComponent<GrayscaleEffect>();
                }
                return CameraGray;
            }
        }

		static SmoothFollow mainCamera = null;
		public static SmoothFollow GetMainCamera{
			get{
				if(mainCamera == null){
					mainCamera = Camera.main.GetComponent<SmoothFollow>();
				}
				return mainCamera;
			}
		}

        public static GameObject LoadProgress()
        {
			//DisableAllUI ();
            //GameObject LoadPregress = (GameObject)UnityEngine.Object.Instantiate(Resources.Load(GameConstDefine.LoadProsPath));

            ResourceUnit LoadPregressUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadProsPath, ResourceType.PREFAB);
            GameObject LoadPregress = GameObject.Instantiate(LoadPregressUnit.Asset) as GameObject;

            
            MonoBehaviour.DontDestroyOnLoad (LoadPregress);
			LoadPregress.transform.parent = GetUiCamera.transform;
			LoadPregress.transform.localScale = new Vector3 (1.0f, 1.0f, 1.0f);
            LoadPregress.transform.localPosition = Vector3.zero;
            return LoadPregress;
		}

		public static void DisableAllUI(){
			if (GetUiCamera == null) {
				return;
			}
			for (int i = 0; i < GetUiCamera.transform.childCount; i++) {
//				MonoBehaviour.DestroyImmediate()
                if (GetUiCamera.transform.GetChild(i) != null && GetUiCamera.transform.GetChild(i).gameObject != null)
                {
                    GetUiCamera.transform.GetChild(i).gameObject.SetActive(false);
                }
			}
		}

        public static void DestroyAllUI() {
            for (int i = 0; i < GetUiCamera.transform.childCount; i++)
            { 
                if (GetUiCamera.transform.GetChild(i) != null && GetUiCamera.transform.GetChild(i).gameObject != null)
                {
                    GameObject.DestroyImmediate(GetUiCamera.transform.GetChild(i).gameObject);
                }
            }
        }

        public static void DestroyOtherUI()
        {
            for (int i = 0; i < GetUiCamera.transform.childCount; i++)
            {
                if (GetUiCamera.transform.GetChild(i) != null)
                {
                    GameObject obj = GetUiCamera.transform.GetChild(i).gameObject;
                    if (obj != null && obj.name != "UIGameLogin(Clone)")
                    {
                        GameObject.DestroyImmediate(GetUiCamera.transform.GetChild(i).gameObject);
                    }                     
                }
            }
        }

        public static void RemoveUI(string name)
        {
            if (GetUiCamera == null)
            {
                return;
            }
            for (int i = 0; i < GetUiCamera.transform.childCount; i++)
            {
                //				MonoBehaviour.DestroyImmediate()
                if (GetUiCamera.transform.GetChild(i) != null && GetUiCamera.transform.GetChild(i).gameObject != null && GetUiCamera.transform.GetChild(i).gameObject.name == name)
                {
                    GameObject.DestroyImmediate(GetUiCamera.transform.GetChild(i).gameObject);
                }
            }
        }
        
		//不包含自己阵营
		public static IEnumerable<Ientity> GetEnemyItemListByRadius(Ientity self, float radius){
			var items = from item in EntityManager.AllEntitys.Values
				where item.RealEntity != null
                where item.RealEntity.enabled == true
                where item.entityType != EntityType.Monster
                where IsEnemyCamp(self , item)
                where item.FSM != null
                where item.FSM.State != BlGame.FSM.FsmState.FSM_STATE_DEAD
                where item.mCanBeLocked
				let distance = Vector3.Distance(item.realObject.transform.position, self.realObject.transform.position)
				where distance < radius
				orderby distance
				select item;
            return items;
		}

        /// <summary>
        /// 所有怪物按照距离排序 去除野怪和建筑
        /// </summary>
        /// <param name="self"></param>
        /// <param name="radius"></param>
        /// <returns></returns>
        public static List<Ientity> GetEnemyItemListByRadiusWithoutMonsterBuilding(Ientity self, float radius)
        {
            var items = from item in EntityManager.AllEntitys.Values
                where item.RealEntity != null
                where IsEnemyCamp(self, item)
                where item.FSM != null
                where item.FSM.State != BlGame.FSM.FsmState.FSM_STATE_DEAD
                where item.entityType != EntityType.Monster
                where item.entityType != EntityType.Building
                let distance = Vector3.Distance(item.realObject.transform.position, self.realObject.transform.position)
                where distance < radius
                orderby distance
                select item;
            return items.ToList();
        }

        /// <summary>
        /// 根据距离排序所有敌对阵营的小兵
        /// </summary>
        /// <param name="self"></param>
        /// <param name="radius"></param>
        /// <returns></returns>
        public static List<Ientity> GetSoldierEnemyItemListByRadius(Ientity self, float radius)
        {
            var items = from item in EntityManager.AllEntitys.Values
                        where item.RealEntity != null
                        where IsEnemyCamp(self, item)
                        where item.FSM != null
                        where item.FSM.State != BlGame.FSM.FsmState.FSM_STATE_DEAD
                        where item.entityType != EntityType.Building
						where item.entityType != EntityType.Monster
						where item.entityType != EntityType.Player
                        let distance = Vector3.Distance(item.realObject.transform.position, self.realObject.transform.position)
                        where distance < radius
                        orderby distance
                        select item;
            return items.ToList();
        }

        /// <summary>
        /// 根据距离排序所有敌对阵营的野怪
        /// </summary>
        /// <param name="self"></param>
        /// <param name="radius"></param>
        /// <returns></returns>
        public static List<Ientity> GetMonsterEnemyItemListByRadius(Ientity self, float radius)
        {
            var items = from item in EntityManager.AllEntitys.Values
                        where item.RealEntity != null
                        where IsEnemyCamp(self, item)
                        where item.FSM != null
                        where item.FSM.State != BlGame.FSM.FsmState.FSM_STATE_DEAD
                        where item.entityType != EntityType.Building
                        where item.entityType != EntityType.Soldier
                        where item.entityType != EntityType.Player
                        let distance = Vector3.Distance(item.realObject.transform.position, self.realObject.transform.position)
                        where distance < radius
                        orderby distance
                        select item;
            return items.ToList();
        }

        /// <summary>
        /// 根据距离排序所有敌对阵营的英雄
        /// </summary>
        /// <param name="self"></param>
        /// <param name="radius"></param>
        /// <returns></returns>
        public static List<Ientity> GetSPlayerEnemyItemListByRadius(Ientity self, float radius)
        {
            var items = from item in EntityManager.AllEntitys.Values
                        where item.RealEntity != null
                        where IsEnemyCamp(self, item)
                        where item.FSM != null
                        where item.FSM.State != BlGame.FSM.FsmState.FSM_STATE_DEAD
                        where item.entityType == EntityType.Player
                        let distance = Vector3.Distance(item.realObject.transform.position, self.realObject.transform.position)
                        where distance < radius
                        orderby distance
                        select item;
            return items.ToList();
        }

        /// <summary>
        /// ScrollView 位移表现
        /// </summary>
        /// <param name="mScroll"></param>
        /// <param name="dis"></param>
        /// 正数为从右到左、负数为从左到右
        public static void ScrollViewTweenPosition(UIScrollView mScroll , float dis)
        {
            Vector3 ls = mScroll.gameObject.transform.localPosition;
            float px = ls.x + dis;
            mScroll.gameObject.transform.localPosition = new Vector3(px, ls.y, ls.z);
            Vector2 v2 = mScroll.GetComponent<UIPanel>().clipOffset;
            v2.x = -px + ls.x + v2.x;
            mScroll.GetComponent<UIPanel>().clipOffset = v2;
            mScroll.RestrictWithinBounds(false);
        }

        public static bool IsEnemyCamp(Ientity self, Ientity target)
        {
            if (target.EntityCamp == EntityCampType.CampTypeKind)
            {
                return false;
            }
            if (target.EntityCamp == EntityCampType.CampTypeBad)
            {
                return true;
            }
            if (self.EntityCamp != target.EntityCamp)
            {
                return true;
            }
            return false;
        }

        /// <summary>
        /// 获取阵营类型
        /// </summary>
        /// <param name="campId"></param>
        /// <returns></returns>
        public static EntityCampType GetEntityCamp(int campId)
        {
            EntityCampType type = (EntityCampType)campId;
            if (campId > 0)
            {
                if (campId % 2 == 0)
                {
                    type = EntityCampType.CampTypeB;
                }
                else
                {
                    type = EntityCampType.CampTypeA;
                }
            }
            return type;
        }

        public static void CreateCharacterController(Ientity entity)
        {
            if(entity.realObject.GetComponent<CharacterController>() == null)
                entity.realObject.AddComponent<CharacterController>();

            entity.RealEntity.Controller = entity.realObject.GetComponent<CharacterController>();
            entity.RealEntity.Controller.center = new Vector3(0f, 1f, 0f);
            entity.RealEntity.Controller.radius = 0.01f;
        }

        /// <summary>
        /// 创建窗体组件
        /// </summary>
        /// <param name="name"></param>
        /// <param name="v3"></param>
        /// <param name="father"></param>
        /// <returns></returns>
		public static GameObject CreateWindow(string name, Vector3 v3 , Transform father)
		{
            //GameObject obj = UnityEngine.Object.Instantiate(Resources.Load(name)) as GameObject;

            ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(name, ResourceType.PREFAB);
            GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
                        			
            obj.transform.parent = father.transform;
			obj.transform.localScale = Vector3.one;
			obj.transform.localPosition = v3;
			return obj;
		}

		//获取距离
		public static float GetDistanceBetweenGameObject(GameObject o1,GameObject o2)
		{
			if(o1 == null || o2 == null)
			{
				return -1.0f;
			}
			Vector3 diff = o1.transform.position - o2.transform.position;
			return Mathf.Sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
		}

		//场景到ui位置的变化
		public static Vector3 SceneToUIPos(Vector3 ScenePos)
		{
			// 3d场景的位置
			Vector3 pos_3d = ScenePos;
			// 对应在屏幕的位置
			Vector2 pos_screen = Camera.main.WorldToScreenPoint(pos_3d);
			// 对应在ui中的位置
			Vector3 pos_ui = GetUiCamera.ScreenToWorldPoint(new Vector3(pos_screen.x, pos_screen.y, 0f));		
			
			return pos_ui;
		}

        /// <summary>
        /// 建筑销毁、替换组件
        /// </summary>
        /// <param name="rp"></param>
        /// 要替换的GameObject
        /// <param name="dt"></param>
        /// 目标GameObject
        public static void BuildReplaceObject(Ientity entity)
        {
            if (!ConfigReader.MapObjXmlInfoDict.ContainsKey(entity.MapObgId))
            {
                return;
            }
            string rp = ConfigReader.MapObjXmlInfoDict[entity.MapObgId].ReplaceID;
            if (rp == "0" || rp == null)
            {
                return;
            }

            //GameObject obj = Resources.Load(GameConstDefine.MapObjectReplaceIDPath + rp) as GameObject;            
//             ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.MapObjectReplaceIDPath + rp, ResourceType.PREFAB);
//             GameObject obj = objUnit.Asset as GameObject;
// 
//             if (obj == null)
//             {
//                 return;
//             }
            //root = GameObject.Instantiate(obj , entity.realObject.transform.position , Quaternion.identity) as GameObject;

            GameObject root = entity.realObject.transform.FindChild(rp).gameObject;
            root.SetActive(true);
            root.transform.parent = null;
            root.transform.rotation = entity.realObject.transform.rotation;
            
        }

        /// <summary>
        /// 转换成Vector3
        /// </summary>
        /// <param name="values"></param>
        /// <returns></returns>
        public static Vector3 ResolveToVector3(string values,char sp = ',')
        {
            string[] value = values.Split(sp);
            if (value.Length != 3)
            {
                Debug.LogError("Vector3 Convert Error: " + values);
            }
            return new Vector3(Convert.ToSingle(value[0]), Convert.ToSingle(value[1]), Convert.ToSingle(value[2]));
        }

        /// <summary>
        /// 转换成Vector3
        /// </summary>
        /// <param name="values"></param>
        /// <returns></returns>
        public static Vector3 ResolveToVector3AsServer(string values, char sp = ',')
        {
            string[] value = values.Split(sp);
            if (value.Length != 3)
            {
                Debug.LogError("Vector3 Convert Error: " + values);
            }
            return new Vector3(Convert.ToSingle(value[0]) / 100f, Convert.ToSingle(value[1]) / 100f, Convert.ToSingle(value[2]) / 100f);
        }

        public static Color ResolveToColor(string values)
        {
            string[] value = values.Split(',');
            if (value.Length != 3)
            {
                Debug.LogError("Vector3 Convert Error: " + values);
            }
            return new Color(Convert.ToSingle(value[0]) / 255f, Convert.ToSingle(value[1]) / 255f, Convert.ToSingle(value[2]) / 255f);
        }

        public static Vector2 ResolveToVector2(string values)
        {
            string[] value = values.Split(',');
            if (value.Length != 2)
            {
                Debug.LogError("Vector3 Convert Error: " + values);
            }
            return new Vector2(Convert.ToSingle(value[0]), Convert.ToSingle(value[1]));
        }

        public static List<int> ResolveToIntList(string values, char sp = ',')
        {
            List<int> ItList = new List<int>();
            if (values == "0")
            {
                return ItList;
            }
            string[] value = values.Split(sp);
            foreach(string it in value)
            {
                ItList.Add(Convert.ToInt32(it));
            }
            return ItList;
        }

        public static List<float> ResolveToFloatList(string values,char sp = ',')
        {
            string[] value = values.Split(sp);
            List<float> ItList = new List<float>();
            foreach (string it in value)
            {
                ItList.Add(Convert.ToSingle(it));
            }
            return ItList;
        }

        /// <summary>
        /// 转换成String List
        /// </summary>
        /// <param name="values"></param>
        /// <returns></returns>
        public static List<string> ResolveToStrList(string values, char sp = ',')
        {
            string[] value = values.Split(sp);
            List<string> StrList = new List<string>();
            foreach (string it in value)
            {
                StrList.Add(it);
            }
            return StrList;
        }

        public static Dictionary<int , int>ResolveToIntDict(string keys, string values, char sp = ',')
        {
            string[] key = keys.Split(sp);
            string[] value = values.Split(sp);
            Dictionary<int, int> Dict = new Dictionary<int, int>();
            for (int i = 0; i < key.Length; i++)
            {
                Dict.Add(Convert.ToInt32(key[i]), Convert.ToInt32(value[i]));
            }
            return Dict;
        }

        /// <summary>
        /// 爆炸特效
        /// </summary>
        /// <param name="entity"></param>
        public static void SetLoadJianTaBaoZha(Ientity entity, bool Replace = true)
        {
            if (entity.NPCCateChild != ENPCCateChild.eNPCChild_BUILD_Summon && Replace)
            {
                BuildReplaceObject(entity);
            }
            GameObject root = null;
            //Vector3 pos = entity.EntityFSMPosition;
            Vector3 pos = entity.realObject.transform.position;
            //if (entity.NpcGUIDType != Ientity.JIDI_A && entity.NpcGUIDType != Ientity.JIDI_B)
            if (!EntityManager.GetHomeBaseList().Contains(entity))
            {
                string jianTaBaoZha = ConfigReader.GetNpcInfo(entity.NpcGUIDType).NpcJianTaDeath;
                if (jianTaBaoZha == "0" || jianTaBaoZha == null || entity.deadSpot == false)
                {
                    return;
                }

                //GameObject CreateEffect = Resources.Load(GameConstDefine.LoadGameBuildingEffectPath + jianTaBaoZha) as GameObject;
                //ResourceUnit CreateEffectUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadGameBuildingEffectPath + jianTaBaoZha, ResourceType.PREFAB);
                //GameObject CreateEffect = CreateEffectUnit.Asset as GameObject;
               

                NormalEffect normalEffect = EffectManager.Instance.CreateNormalEffect(GameConstDefine.LoadGameBuildingEffectPath + jianTaBaoZha,null);
                if (normalEffect == null)
                {
                    return;
                }
                //root = GameObject.Instantiate(CreateEffect) as GameObject;
                //if (root != null)
                //    root.transform.position = new Vector3(pos.x, pos.y + 6, pos.z);
                normalEffect.obj.transform.position = new Vector3(pos.x, pos.y + 6, pos.z);
            }
            else
            {
                if (entity.IsSameCamp(EntityCampType.CampTypeA))
                {
                    pos.y += 6f;
                    LoadBaseDate.Instance().BaseA.gameObject.SetActive(true);
                    LoadBaseDate.Instance().BaseA.transform.position = new Vector3(pos.x, pos.y, pos.z);
                    return;
                }
                LoadBaseDate.Instance().BaseB.gameObject.SetActive(true);
                LoadBaseDate.Instance().BaseB.transform.position = new Vector3(pos.x, pos.y, pos.z);
            } 
        }

        public static void DebugError(object message)
        {
            //return;
            Debug.LogError(message);
        }

        public static void DebugWaring(object message)
        {
            return;
        }

        public static void DebugLog(object message)
        {
            return;
        }

        //传的是概率集合 获得概率集合下表
        public static int RandProbablityIndex(List<float> probabilityList)
        {
            double tempNum = 0f;
            System.Random r = new System.Random();
            double rNum = r.NextDouble();
            //获得概率集合从低到高的排序后的下标
            List<int> probabilityIndex = SortListReturnIndex(probabilityList);
            for (int i = 0; i < probabilityIndex.Count; i++)
            {
                //tempNum从0开始与概率从低到高的概率做加法
                int index = probabilityIndex[i];
                tempNum += probabilityList[index];
                //如果加成后的大于随机数
                if (rNum < tempNum)
                    return i;
            }
            return 0;
        }

        ////获得概率集合从低到高的排序后的下标
        private static List<int> SortListReturnIndex(List<float> item)
        {
            List<float> itemTemp = new List<float>();
            for (int i = 0; i < item.Count; i++)
                itemTemp.Add(item[i]);
            List<int> listIndex = new List<int>();
            while (itemTemp.Count != 0)
            {
                float min = itemTemp[0];
                int k = 0;
                for (int i = 0; i < itemTemp.Count; i++)
                {
                    if (itemTemp[i] <= min)
                    {
                        min = itemTemp[i];
                        k = i;
                    }
                }
                listIndex.Add(k);
                itemTemp.Remove(min);
            }
            return listIndex;
        }
        private const int TIMEBUCKET = 60; 
        public static string GetCurrSystemTime(int hour,int minutes,int seconds) {
            int currTime = 0;
            if (hour != 0)
            {
                currTime += hour * TIMEBUCKET * TIMEBUCKET;
            }
            if(minutes != 0)
            {
                currTime += minutes * TIMEBUCKET;
            }
            currTime += seconds;
            seconds = currTime % 60;
            minutes = currTime / 60;
            string timeSeconds = seconds.ToString();
            string timeMinutes = minutes.ToString();
            if (seconds < 10)
                timeSeconds = "0" + seconds.ToString();
            if (minutes < 10)
                timeMinutes = "0" + minutes;
            return timeMinutes + ":" + timeSeconds;             
        }

        public static int[] GetRandIntArrayFromList(List<int> list, int num) { 
           int []ret = new int[num];
           System.Random random = new System.Random();
           for (int i = 0; i < num; i++) {
               if (list != null && list.Count != 0) {
                   int ran = random.Next(list.Count);
                   ret[i] = list.ElementAt(ran);
                   list.RemoveAt(ran);
               }
           }
           return ret;
        }        

        public static string[] GetRandStringArrayFromList(List<string> list, int num) {
            string[] ret = new string[num];
            var seed = (int)GameUserModel.Instance.GameUserGUID;
            System.Random random = new System.Random(seed);
            for (int i = 0; i < num; i++)
            {
                if (list != null && list.Count != 0)
                {
                    int ran = random.Next(list.Count);
                    ret[i] = list.ElementAt(ran);
                    list.RemoveAt(ran);
                }
            }
            return ret;
        }

        public static void LogOutToLogin()
        {   
            if (PlayerManager.Instance != null)
            {
                if (PlayerManager.Instance.AccountDic != null)
                {
                    PlayerManager.Instance.AccountDic.Clear();
                }
                Iselfplayer player = PlayerManager.Instance.LocalPlayer;
                if(player != null){
                    player.UserGameItemsCount.Clear();
                    player.UserGameItemsCoolDown.Clear();
                    player.UserGameItems.Clear();
                    Debug.LogError("player");
                }
                MsgInfoManager.Instance.RemoveKillMsg();
                AltarData.Instance.DelAllAltar();
                FriendManager.Instance.DelAllList();
                PlayerManager.Instance.DestoryAllEntity();
                GameUserModel.Instance.Clear();
                //GoodsModel.Instance.Clear();
                MailModel.Instance.Clear();
                SystemNoticeData.Instance.Clear();
                MarketHeroListModel.Instance.Clear();
                MarketRuneListModel.Instance.Clear();
                RuneEquipModel.Instance.Clear();
                PlayerManager.Instance.LocalAccount = PlayerManager.Instance.LocalPlayer = null;
            }

            NetworkManager.Instance.canReconnect = false;
            NetworkManager.Instance.Close();
            
#if UNITY_STANDALONE_WIN || UNITY_EDITOR || SKIP_SDK
#else
            SdkConector.ShowToolBar(1);
#endif
        }
        public static void FileRead()
        {
            split = new List<string>();
            ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate("Config/filter", ResourceType.ASSET);
            TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;
            string strLine = xmlfile.text;
            string[] value = strLine.Split(',');
            foreach (string item in value)
            {
                split.Add(item.Trim());
            }

            Console.ReadLine(); 
        }

        private static List<string> split;

        public static string GetSplitStr(string currStr)
        {
            foreach (string item in split)
            {
                int currPos = currStr.IndexOf(item, StringComparison.OrdinalIgnoreCase);
                while(currPos > -1)
                {
                    string str1 = new string('*', item.Length);
                    string s = currStr.Replace(currStr, str1);
                    currStr = currStr.Substring(0, currPos) + s + currStr.Substring(currPos + s.Length);
                    currPos = currStr.IndexOf(item, StringComparison.OrdinalIgnoreCase);
                }
            }
            return currStr;
        }
        public static List<string> GetSplit()
        {
            return split;
        }

        public const string EffectVoc = "EffectButton";
        public const string RangeVoc = "RangeButton";
        public static void SetRange(bool state)
        {
            if (PlayerManager.Instance.LocalPlayer != null && state)
            {
                PlayerManager.Instance.LocalPlayer.initAreaCircle();
            }else if (PlayerManager.Instance.LocalPlayer != null && !state)
            {
                PlayerManager.Instance.LocalPlayer.RemoveAreaCircle();
            }
        }
        public static void SetEffect(bool state)
        {
            PlayerPrefs.SetInt(GameMethod.EffectVoc, state ? 1 : 0);

            if (state)
            {
                EffectManager.Instance.SetEffectLodLevel(EffectLodLevel.High);
            }
            else
            {
                EffectManager.Instance.SetEffectLodLevel(EffectLodLevel.Low);
            }
        }
    }
}
