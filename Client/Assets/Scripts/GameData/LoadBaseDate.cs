using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Resource;

namespace BlGame.GameData
{
    public class LoadBaseDate
    {

        public GameObject BaseA
        {
            private set;
            get;
        }
        public GameObject BaseB
        {
            private set;
            get;
        }
        private static LoadBaseDate instance = null;
        public static LoadBaseDate Instance()
        {
            if (instance == null)
            {
                instance = new LoadBaseDate();
            }
            return instance;
        }

        public void LoadBase()
        {
            //BaseA = GameObject.Instantiate(Resources.Load(GameConstDefine.LoadGameBuildingEffectPath + "jidideath_A")) as GameObject;
            //BaseB = GameObject.Instantiate(Resources.Load(GameConstDefine.LoadGameBuildingEffectPath + "jidideath_B")) as GameObject;

            ResourceUnit BaseAUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadGameBuildingEffectPath + "jidideath_A", ResourceType.PREFAB);
            BaseA = GameObject.Instantiate(BaseAUnit.Asset) as GameObject;

            ResourceUnit BaseBUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadGameBuildingEffectPath + "jidideath_B", ResourceType.PREFAB);
            BaseB = GameObject.Instantiate(BaseBUnit.Asset) as GameObject;


            if (BaseA == null)
            {
                Debug.LogError("error  BaseA is null");
                return;
            }
            if (BaseB == null)
            {
                Debug.LogError("error  BaseB is null");
                return;
            }
            BaseB.gameObject.SetActive(false);
            BaseA.gameObject.SetActive(false);
            //Debug.LogError("DDDDDDDDD");
        }
        // Use this for initialization
        void Start()
        {

        }

        // Update is called once per frame
        void Update()
        {

        }
    }
}
