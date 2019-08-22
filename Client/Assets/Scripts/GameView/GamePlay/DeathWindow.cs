using UnityEngine;
using System.Collections;
using GameDefine;
using System;
using BlGame.Ctrl;
using BlGame.Resource;
using BlGame.GameEntity;
using BlGame.Model;

namespace BlGame.View
{
    public class DeathWindow : BaseWindow
    {
        public DeathWindow()
        {
            mScenesType = EScenesType.EST_Play;
            mResName = GameConstDefine.DeathInterfaceUI;
            mResident = false;
        }
        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_NotifyDeathEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_NotifyDeathExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }


        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_NotifyDeathEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_NotifyDeathExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            StratTime = timeRelieve = DateTime.Now;
            BtnStore = mRoot.FindChild("Store").GetComponent<UIButton>();
            TimeRes = mRoot.FindChild("ResurrectionTime/Label").GetComponent<UILabel>();
            MoneyLabel = mRoot.FindChild("RecoveryBtn/Gold/Number").GetComponent<UILabel>();
            RecoveryNums = mRoot.FindChild("RecoveryBtn/Remain").GetComponent<UILabel>();
            NumberSprite = mRoot.FindChild("ResurrectionTime/Minute1").GetComponent<UISprite>();
            DecadeSprite = mRoot.FindChild("ResurrectionTime/Minute2").GetComponent<UISprite>();
            HundredsSprite = mRoot.FindChild("ResurrectionTime/Minute3").GetComponent<UISprite>();
            RecoveryBtn = mRoot.FindChild("RecoveryBtn/Button").GetComponent<UIButton>();
            RecoveryLabel = mRoot.FindChild("RecoveryBtn/Remain_Label").GetComponent<UILabel>();
            NumberSprite.gameObject.SetActive(false);
            DecadeSprite.gameObject.SetActive(false);
            HundredsSprite.gameObject.SetActive(false);
            EventDelegate.Add(RecoveryBtn.onClick,BtnOnPressRecovery);
            EventDelegate.Add(BtnStore.onClick,BtnOnPressStore);
            mMoveMainCamerBtn = mRoot.FindChild("Background").GetComponent<UIDragObCamera>();
            UIEventListener.Get(mMoveMainCamerBtn.gameObject).onDrag += MoveMian;
        }

        private void MoveMian(GameObject go, Vector2 delta)
        {
            Vector3 vec = Camera.main.ScreenToWorldPoint(delta);
            CameraRemotionConfigInfo camInfo = ConfigReader.GetCameraInfo((int)GameUserModel.Instance.GameMapID);
            if (camInfo != null) {
                Vector3 v1 = new Vector3(GameMethod.GetMainCamera.transform.position.x - (delta.x / camInfo.mMovementSpeed), GameMethod.GetMainCamera.transform.position.y, GameMethod.GetMainCamera.transform.position.z - (delta.y / camInfo.mMovementSpeed));
                if (v1.x < camInfo.mLeftEdge.x)
                    v1.x = camInfo.mLeftEdge.x;
                if (v1.x > camInfo.mRightEdge.x)
                    v1.x = camInfo.mRightEdge.x;
                if (v1.z < camInfo.mBottomEdge.z)
                    v1.z = camInfo.mBottomEdge.z;
                if (v1.z > camInfo.mTopEdge.z)
                    v1.z = camInfo.mTopEdge.z;

                GameMethod.GetMainCamera.transform.position = v1;
                GameMethod.GetMainCamera.RefreshFrustumPlanes();
            }
        }

        void ExitDeath()
        {
            //CreateEffect();
            //yield return new WaitForSeconds(0.79f);
            isDownTime = false;
            DeathCtrl.Instance.Exit();
            //DestroyEffect();
        }

        public override void Update(float deltaTime)
        {
            if (isDownTime)
            {
                if (DateTime.Now != timeRelieve)
                {
                    TimeSpan currtime = DateTime.Now - StratTime;
                    ShowTimes((int)(LastTime - currtime.Seconds));
                    if (currtime.Seconds >= LastTime)
                    {
                       ExitDeath();
                    }
                }
                GamePlayCtrl.Instance.showaudiotimeold = System.DateTime.Now;
            }
        }

        private void ShowTimes(int time)
        {
            if (time < 0)
                return;
            int num = time % 10;
            int dest = time / 10 % 10;
            int hund = time / 100 % 10;
            //Debug.LogError(num + "  dest = " + dest + "  hund = " + hund + "   time = " + time);
            NumberSprite.spriteName = num.ToString();
            DecadeSprite.spriteName = dest.ToString();
            HundredsSprite.spriteName = hund.ToString();
            NumberSprite.gameObject.SetActive(time >= 0);
            DecadeSprite.gameObject.SetActive(time >= 10);
            HundredsSprite.gameObject.SetActive(time >= 100);
        }
        private const string path = "effect/ui_effect/Resurrection_effect_fast";
        // Update is called once per frame
        void CreateEffect()
        {
            if (Effect != null)
                return;
            ResourceUnit EffectUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
            Effect = GameObject.Instantiate(EffectUnit.Asset) as GameObject;

            //Effect.gameObject.transform.parent = transform;
            //Effect.transform.localScale = Vector3.one;
            //Effect.transform.localPosition = Vector3.zero;
        }

        private void BtnOnPressStore()
        {
            Iplayer player = PlayerManager.Instance.LocalPlayer;
            if (player != null)
            {
                GamePlayCtrl.Instance.OpenShop();
            }
        }

        private void BtnOnPressRecovery()
        {
            DeathCtrl.Instance.SendBtn();
        }

        protected override void RealseWidget()
        {

        }


        public override void OnEnable()
        {
            RecoveryBtn.isEnabled = (DeathCtrl.Instance.mRecoveryNums != 0);
            long money = DeathCtrl.Instance.mMoney;
            if (money == 0)
            {
                MoneyLabel.transform.parent.gameObject.SetActive(false);
                RecoveryLabel.transform.localPosition = new Vector3(RecoveryLabel.transform.localPosition.x + 30, RecoveryLabel.transform.localPosition.y, RecoveryLabel.transform.localPosition.z);
            }
            MoneyLabel.text = money.ToString();
            LastTime = DeathCtrl.Instance.mLastTime;
            isDownTime = true;
        }
        //隐藏
        public override void OnDisable()
        {

        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroReborn, ExitDeath);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroReborn, ExitDeath);
        }

        UIButton BtnStore;
        UILabel TimeRes;
        UILabel MoneyLabel;
        UILabel RecoveryNums;
        UIButton RecoveryBtn;
        UILabel RecoveryLabel;
        UISprite NumberSprite;
        UISprite DecadeSprite;
        UISprite HundredsSprite;//NumberSprite,DecadeSprite,HundredsSprite
        private GameObject Effect = null;
        DateTime StratTime;

        UIDragObCamera mMoveMainCamerBtn = null;

        private DateTime timeRelieve;
        private float LastTime;
        private bool isDownTime = false;
    }
}
