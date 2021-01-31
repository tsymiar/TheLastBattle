using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Ctrl;
using BlGame.Model;
using System.Collections.Generic;
using System.Linq;
using BlGame.Resource;
using System;

namespace BlGame.View
{
    public class PresonInfoWindow : BaseWindow
    {
        public PresonInfoWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadPersonalDateOptionUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化

        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_PresonInfoEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_PresonInfoExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_PresonInfoEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_PresonInfoExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }
          //窗口控件初始化
        protected override void InitWidget()
        {
            mHeadIcon = mRoot.FindChild("OptionItems/OptionPersionalData/player/HeadPhoto").GetComponent<UISprite>();
            mNickName = mRoot.FindChild("OptionItems/OptionPersionalData/player/Name").GetComponent <UILabel>();
            mLevel = mRoot.FindChild("OptionItems/OptionPersionalData/player/Level/Number").GetComponent<UILabel>();
            mExp = mRoot.FindChild("OptionItems/OptionPersionalData/player/EXP/Number").GetComponent<UILabel>();

            mGameInning = mRoot.FindChild("OptionItems/OptionPersionalData/Items/Item1/Number").GetComponent<UILabel>();
            mWinInning = mRoot.FindChild("OptionItems/OptionPersionalData/Items/Item2/Number").GetComponent<UILabel>();
            mKillAll = mRoot.FindChild("OptionItems/OptionPersionalData/Items/Item3/Number").GetComponent<UILabel>();
            mDestrucBuild = mRoot.FindChild("OptionItems/OptionPersionalData/Items/Item4/Number").GetComponent<UILabel>();

            mDeathAll = mRoot.FindChild("OptionItems/OptionPersionalData/Items/Item5/Number").GetComponent<UILabel>();
            mAchievement = mRoot.FindChild("OptionItems/OptionPersionalData/Items/Item6/Number").GetComponent<UILabel>();
            mAssistAll = mRoot.FindChild("OptionItems/OptionPersionalData/Items/Item7/Number").GetComponent<UILabel>();
            mBelongGuild = mRoot.FindChild("OptionItems/OptionPersionalData/Items/Item8/Number").GetComponent<UILabel>();

            mIntegral = mRoot.FindChild("OptionItems/OptionPersionalData/Items/Item9/Number").GetComponent<UILabel>();
            mShowInterName = mRoot.FindChild("OptionItems/OptionPersionalData/player/ChangeName").gameObject;
            mShowInterHeadIncon = mRoot.FindChild("OptionItems/OptionPersionalData/player/ChangeHead").gameObject;
            mTempName = mRoot.FindChild("OptionItems/OptionPersionalData/NameChange");
            mTempHeadIcon = mRoot.FindChild("OptionItems/OptionPersionalData/HeadPick");

            mLogOutBtn = mRoot.FindChild("OptionItems/OptionPersionalData/LogoutBtn").gameObject;

            mCloseHeadBtn = mTempHeadIcon.FindChild("Background").gameObject;
            mCloseBtn = mRoot.FindChild("CloseBtn").gameObject;
            mNewTemp = mTempName.FindChild("Name").GetComponent<UIInput>();
            mSureBtn = mTempName.FindChild("ConfirmBtn").GetComponent<UIButton>();
            mVipLevel = mRoot.FindChild("OptionItems/OptionPersionalData/player/VIPLevelUp/Level").GetComponent<UISprite>();
            mVipPriv = mRoot.FindChild("OptionItems/OptionPersionalData/player/VIPInfo").gameObject;
            VIPLevelUp = mRoot.FindChild("OptionItems/OptionPersionalData/player/VIPLevelUp").GetComponent<UILabel>();
            mDoubleExp = mRoot.FindChild("OptionItems/OptionPersionalData/player/EXPCardTime/Time").GetComponent<UILabel>();
            mDoubleGold = mRoot.FindChild("OptionItems/OptionPersionalData/player/GoldCardTime/Time").GetComponent<UILabel>();
            mCloseChangeNameBtn = mTempName.FindChild("CancelBtn").gameObject;
            mGrid = mTempHeadIcon.FindChild("ScrollView/Grid").GetComponent<UIGrid>();
            heroInfoList = new List<HeadSelectConfigInfo>();

            for (int i = 0; i < ConfigReader.HeadSelectXmlInfoDict.Count; i++)
            {
                #region 获得所有英雄信息
                HeadSelectConfigInfo item = ConfigReader.HeadSelectXmlInfoDict.ElementAt(i).Value;
                if (item.HeroUsedCondition == 1 && item.HeroUsedLevel > 1)
                    continue;
                heroInfoList.Add(item);
                #endregion
            }
            UIEventListener.Get(mShowInterName).onClick += ShowChangeName;
            UIEventListener.Get(mShowInterHeadIncon).onClick += ShowChangeHeadIcon;
            UIEventListener.Get(mSureBtn.gameObject).onClick += SureClickBtn;
            UIEventListener.Get(mNewTemp.gameObject).onSelect += ChangeNameSelect;
            UIEventListener.Get(mCloseChangeNameBtn).onClick += CloseChangeNameBtn;

            UIEventListener.Get(mLogOutBtn).onClick += LogOutBtn;
            UIEventListener.Get(mCloseHeadBtn).onClick += CloseHeadBtn;
            UIEventListener.Get(mVipPriv).onClick += ShowVipPriv;
            UIEventListener.Get(mCloseBtn).onClick += ClosePresonInfo;
        }

        private void LogOutBtn(GameObject go)
        {
            LoginCtrl.Instance.SdkLogOff();
            //talkgame
        #if UNITY_STANDALONE_WIN || UNITY_EDITOR || SKIP_SDK
        #else
               TalkGame.Instance.UnInitTalkGame();       
        #endif   
        }

        private void CloseHeadBtn(GameObject go)
        {
            mTempHeadIcon.gameObject.SetActive(false);
        }

        private void CloseChangeNameBtn(GameObject go)
        {
            mNewTemp.value = "输入新的昵称";
            mTempName.gameObject.SetActive(false);
        }

        private void ClosePresonInfo(GameObject go)
        {
            PresonInfoCtrl.Instance.Exit();
        }
        //显示VIP特权介绍
        private void ShowVipPriv(GameObject go)
        {
            VIPPrerogativeCtrl.Instance.Enter();
        }

        private void ChangeNameSelect(GameObject go, bool state)
        {
            
            if (state)
            {
                mNewTemp.value = "";
            }
        }

        private void SureClickBtn(GameObject go)
        {
            if (string.Compare(mNewTemp.value.Trim(), "请输入要更换的昵称") == 0 || string.Compare(mNewTemp.value.Trim(), "输入新的昵称") == 0)
                return;
            if (mNewTemp == null || mNewTemp.value.Trim() == "" || string.IsNullOrEmpty(mNewTemp.value.Trim()))
            {
                MsgInfoManager.Instance.ShowMsg(-130977);
                return;
            }
            if (string.Compare(mNewTemp.value, GameUserModel.Instance.GameUserNick) == 0)
            {
                MsgInfoManager.Instance.ShowMsg(10032);
                return;
            }
            List<string> split = GameMethod.GetSplit();
            string currStr = mNewTemp.value;
            foreach (string item in split)
            {
                int currPos = currStr.IndexOf(item, StringComparison.OrdinalIgnoreCase);
                if (currPos > -1)
                {
                    MsgInfoManager.Instance.ShowMsg(-130861);
                    return;
                }
            }
            PresonInfoCtrl.Instance.ChangeNickName(mNewTemp.value);
            mNewTemp.value = "输入新的昵称";
        }
        
        private void ShowChangeHeadIcon(GameObject go)
        {
            mTempHeadIcon.gameObject.SetActive(go == mShowInterHeadIncon);
            if (LastObj != null)
            {
                LastObj = null;
            }
            if (mHeadIconList.Count != 0)
            {
                for (int i = mHeadIconList.Count - 1; i >= 0  ; i --)
                {
                    UnityEngine.GameObject.DestroyImmediate(mHeadIconList[i].LastObj);
                    mHeroShowList.Remove(mHeadIconList[i].LastObj);
                    mHeadIconList.Remove(mHeadIconList[i]);
                }
                mHeroShowList.Clear();
                mHeadIconList.Clear();
            }
            
            foreach (var item in heroInfoList)
            {
                foreach (var mTemp in item.HeroHeadType)
                {
                    if (mTemp != GameUserModel.Instance.GameUserSex)
                    {
                        continue;
                    }
                    #region  加载选择英雄显示预设
                    GameObject objLoad = null;
                    ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadHeadPhotoUi, ResourceType.PREFAB);
                    objLoad = GameObject.Instantiate(objUnit.Asset) as GameObject;

                    objLoad.transform.parent = mGrid.transform;
                    objLoad.transform.localScale = Vector3.one;
                    objLoad.transform.localPosition = Vector3.zero;
                    HeroHeadIcon icon = new HeroHeadIcon();
                    mHeroShowList.Add(objLoad);
                    icon.mHeroIcon = objLoad.GetComponent<UISprite>();
                    icon.mVipLevel = objLoad.transform.FindChild("VIPSign/Label").GetComponent<UILabel>();
                    icon.mHeroIcon.spriteName = icon.mIconName = item.HeroHeadAtlas_Num.ToString();
                    icon.LastObj = objLoad;
                    mHeadIconList.Add(icon);
                    #endregion
                    UIEventListener.Get(objLoad).onClick += OnSelectHero;
                }
            }
            mGrid.repositionNow = true;
            mGrid.Reposition();
        }

        private void OnSelectHero(GameObject go)
        {
            foreach (var item in mHeadIconList)
            {
                if (LastObj == item)
                {
                    mTempHeadIcon.gameObject.SetActive(false);
                    return;
                }
                if (go == item.LastObj)
                {
                    LastObj = item;
                    mTempHeadIcon.gameObject.SetActive(false);
                    PresonInfoCtrl.Instance.ChangeHead(Convert.ToUInt32(item.mIconName));
                    //Debug.LogError(item.mIconName);
                }
            }
        }

        private const ulong needDiamond = 20;
        private void ShowChangeName(GameObject go)
        {
            mSureBtn.isEnabled = GameUserModel.Instance.mGameUserDmd > needDiamond;
            mTempName.gameObject.SetActive(go == mShowInterName);
        }

        public override void Update(float deltaTime)
        {
            if (BlGame.Ctrl.PresonInfoCtrl.Instance.GetTimeExp() && PresonInfoCtrl.Instance.GetExpLastMin())
                ChangeExpTime();
            if (BlGame.Ctrl.PresonInfoCtrl.Instance.GetTimeGold() && PresonInfoCtrl.Instance.GetGoldLastMin())
                ChangeGoldTime();
        }

        //窗口控件释放
        protected override void RealseWidget()
        {

        }

         //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_ChangeNickName, ChangeNickName);
            EventCenter.AddListener(EGameEvent.eGameEvent_ChangeHeadID, ChangeHeadID);
        }

        private void ChangeHeadID()
        {
            mHeadIcon.spriteName = GameUserModel.Instance.GameUserHead.ToString();
        }

        private void ChangeNickName()
        {
            mNickName.text = GameUserModel.Instance.GameUserNick;
        }

        private void ChangeGoldTime()
        {
            if (PresonInfoCtrl.Instance.mGoldHours != 0 || PresonInfoCtrl.Instance.mGoldMinute != 0 || PresonInfoCtrl.Instance.mGoldSecond != 0)
            {
                mDoubleExp.transform.parent.gameObject.SetActive(true);
                mDoubleExp.text = GetString(PresonInfoCtrl.Instance.mGoldHours, PresonInfoCtrl.Instance.mGoldMinute);
            }
            else
            {
                mDoubleExp.transform.parent.gameObject.SetActive(false);
                PresonInfoCtrl.Instance.SetTimeExp(false);
            }
        }

        private void ChangeExpTime()
        {
            if (PresonInfoCtrl.Instance.mExpHours != 0 || PresonInfoCtrl.Instance.mExpMinute != 0 || PresonInfoCtrl.Instance.mExpSecond != 0)
            {
                mDoubleGold.text = GetString(PresonInfoCtrl.Instance.mExpHours, PresonInfoCtrl.Instance.mExpMinute);
                mDoubleGold.transform.parent.gameObject.SetActive(true);
            }
            else
            {
                mDoubleGold.transform.parent.gameObject.SetActive(false);
                PresonInfoCtrl.Instance.SetTimeGold(false);
            }
            
        }
        string GetString(int hours,int minutes)
        {
            string temp = null;
            temp = hours.ToString();
            if (hours < 10)
                temp = "0" + hours;
            if (minutes < 10)
                return temp += ("时0" + minutes + "分");
            return temp + ("时" + minutes + "分");
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ChangeNickName, ChangeNickName);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ChangeHeadID, ChangeHeadID);
        }

        //显示
        public override void OnEnable()
        {
            mHeadIcon.spriteName = GameUserModel.Instance.GameUserHead.ToString();
            mNickName.text = GameUserModel.Instance.GameUserNick;
            mGameInning.text = PresonInfoCtrl.Instance.mGameInning.ToString();
            mWinInning.text = PresonInfoCtrl.Instance.mWinInning.ToString();
            mKillAll.text = PresonInfoCtrl.Instance.mKillAll.ToString();
            mDestrucBuild.text = PresonInfoCtrl.Instance.mDestrucBuild.ToString();
            mDeathAll.text = PresonInfoCtrl.Instance.mDeathAll.ToString();
            mAchievement.text = PresonInfoCtrl.Instance.mAchievement;
            mAssistAll.text = PresonInfoCtrl.Instance.mAssistAll.ToString();
            mBelongGuild.text = PresonInfoCtrl.Instance.mBelongGuild;
            int vipintegral = GameUserModel.Instance.GameUserVipIntegral;
            mIntegral.text = vipintegral.ToString();
            int vipLevel = GameUserModel.Instance.GameUserVipLevel;
            if (vipLevel < 0 || vipLevel > 10)
            {
                Debug.LogError("Vip异常");
                return;
            }
            mVipLevel.spriteName = vipLevel.ToString();
            VipLevelConfigInfo vipLevelInfo = ConfigReader.GetVipLevelInfo(++vipLevel);
            VIPLevelUp.text = /*"再累积" + (vipLevelInfo.VipIntegral - vipintegral) + "积分VIP" +vipLevel*/"";
            short level = GameUserModel.Instance.UserLevel;
            mLevel.text = level.ToString();
            if (BlGame.Ctrl.PresonInfoCtrl.Instance.GetTimeExp())
                ChangeExpTime();
            if (BlGame.Ctrl.PresonInfoCtrl.Instance.GetTimeGold())
                ChangeGoldTime();
            LevelConfigInfo leveinfo = ConfigReader.GetLevelInfo(level);
            if (leveinfo != null)
            {
                mExp.text = GameUserModel.Instance.GameUserExp + "/" + leveinfo.LevelUpExp;
                mExp.transform.parent.GetComponent<UISprite>().fillAmount = PresonInfoCtrl.Instance.mExp / leveinfo.LevelUpExp;
            }
            if (level >= 30)
            {
                mExp.transform.parent.gameObject.SetActive(false);
                mExp.transform.parent.GetComponent<UISprite>().fillAmount = 1;
            }
            
        }

        //隐藏
        public override void OnDisable()
        {

        }

        private DateTime CurrTime;

        private int mSelectIndex = -1;
        List<HeadSelectConfigInfo> heroInfoList = null;
        private List<GameObject> mHeroShowList = new List<GameObject>(); //点击选择框
        private List<HeroHeadIcon> mHeadIconList = new List<HeroHeadIcon>();

        public UISprite mHeadIcon;
        public UILabel mNickName;
        public UILabel mLevel;
        public UILabel mExp;
        public UILabel mGameInning;//游戏总局数
        public UILabel mWinInning;// 胜利总局数
        public UILabel mKillAll;//杀人总数
        public UILabel mDestrucBuild;//破坏建筑
        public UILabel mDeathAll;// 死亡总数
        public UILabel mAchievement;//成就数
        public UILabel mAssistAll;//助攻
        public UILabel mBelongGuild;//所属公会
        public UILabel mIntegral;//积分
        public UISprite mVipLevel;
        public GameObject mVipPriv;
        public UILabel VIPLevelUp;
        public UILabel mDoubleExp;
        public UILabel mDoubleGold;

        public Transform mTempName;
        public Transform mTempHeadIcon;

        public GameObject mShowInterName;
        public GameObject mShowInterHeadIncon;
        public GameObject mCloseBtn;

        public UIInput mNewTemp;
        public UIButton mSureBtn;
        public GameObject mLogOutBtn;
        public GameObject mCloseChangeNameBtn;
        public GameObject mCloseHeadBtn;
        UIGrid mGrid = null;

        HeroHeadIcon LastObj = null;
        UISprite LastSp = null;

        class HeroHeadIcon
        {
            public GameObject LastObj = null;
            public UILabel mVipLevel = null;
            public UISprite mHeroIcon = null;
            public string mIconName = null;
        }
    }
}
