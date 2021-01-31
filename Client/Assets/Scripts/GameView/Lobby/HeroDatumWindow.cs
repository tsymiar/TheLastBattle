using UnityEngine;
using System.Collections;
using GameDefine;
using System.Collections.Generic;
using BlGame.Model;
using BlGame.Resource;
using BlGame.Ctrl;
using System;
using System.Linq;

namespace BlGame.View
{
    public class HeroDatumWindow : BaseWindow
    {
        public HeroDatumWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadHeroDataUI;
            mResident = false;
        }
        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroDatumEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroDatumExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_HomePageExit, Hide);
        }


        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroDatumEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroDatumExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HomePageExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mPopupButton = mRoot.FindChild("KindSelect/PopupList").GetComponent<UIButton>();
            mOpenHeroKind = mRoot.FindChild("KindSelect/HeroKind");
            mGrid = mRoot.FindChild("ScrollView/Grid").GetComponent<UIGrid>();
            mHeroAttributive = mPopupButton.transform.FindChild("Label").GetComponent<UILabel>();

            mHeroAll = mRoot.FindChild("KindSelect/HeroKind/All").GetComponent<UIButton>();
            mHeroAttack = mRoot.FindChild("KindSelect/HeroKind/Attack").GetComponent<UIButton>();
            mHeroSpell = mRoot.FindChild("KindSelect/HeroKind/Spell").GetComponent<UIButton>();
            mHeroDefend = mRoot.FindChild("KindSelect/HeroKind/Defend").GetComponent<UIButton>();
            mHeroAssist = mRoot.FindChild("KindSelect/HeroKind/Assist").GetComponent<UIButton>();
            mClose = mRoot.FindChild("CloseBtn").GetComponent<UIButton>();
            mScroll = mRoot.FindChild("ScrollView").GetComponent<UIScrollView>();

            EventDelegate.Add(mPopupButton.onClick, OpenHeroKindButton);
            EventDelegate.Add(mHeroAll.onClick, OpenAllHeroButton);
            EventDelegate.Add(mHeroAttack.onClick, OpenAttackHeroButton);
            EventDelegate.Add(mHeroSpell.onClick, OpenSpellHeroButton);
            EventDelegate.Add(mHeroDefend.onClick, OpenDefendHeroButton);
            EventDelegate.Add(mHeroAssist.onClick, OpenAssistHeroButton);
            EventDelegate.Add(mClose.onClick, CloseHeroDatum);
            InitHeroHead();
        }

        private void CloseHeroDatum()
        {
            HeroDatumCtrl.Instance.Exit();
        }

        private void OpenAssistHeroButton()
        {
            ShowHeroDatum(HeroType.AssistHero);
            OpenHeroKindButton();
        }

        private void OpenDefendHeroButton()
        {
            ShowHeroDatum(HeroType.DefendHero);
            OpenHeroKindButton();
        }

        private void OpenSpellHeroButton()
        {
            ShowHeroDatum(HeroType.SpellHero);
            OpenHeroKindButton();
        }

        private void OpenAttackHeroButton()
        {
            ShowHeroDatum(HeroType.AttackHero);
            OpenHeroKindButton();
        }

        private void OpenAllHeroButton()
        {
            ShowHeroDatum(HeroType.AllHero);
            OpenHeroKindButton();
        }

        private void OpenHeroKindButton()
        {
            isOpen = !isOpen;
            mOpenHeroKind.gameObject.SetActive(isOpen);
        }

        protected override void RealseWidget()
        {
            HeroDataList.Clear();
            isOpen = false;
        }

        public override void OnEnable()
        {
            ShowHeroDatum(HeroType.AllHero);
        }
        //隐藏
        public override void OnDisable()
        {
            isOpen = false;
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<int>(EGameEvent.eGameEvent_RefreshGoodHero, BuySuccess);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<int>(EGameEvent.eGameEvent_RefreshGoodHero, BuySuccess);
        }

        /// <summary>
        /// 购买英雄成功刷新界面
        /// </summary>
        /// <param name="goods"></param>
        private void BuySuccess(int goods)
        {
            foreach (HeroDataPhoto data in HeroDataList)
            {
                if (data.mGoodsID == goods)
                {
                    data.mOwn.gameObject.SetActive(true);
                    data.mGray.gameObject.SetActive(false);
                    data.mLastTime.gameObject.SetActive(false);
                    break;
                }
            }
        }

        public override void Update(float deltaTime)
        {

            foreach (HeroDataPhoto data in HeroDataList)
            {
                data.FreshHeroLastTime();
            }
        }

        /// <summary>
        /// 获取所要显示的英雄的列表
        /// </summary>
        /// <param name="type"></param>
        /// <returns></returns>
        private List<int> GetShowHerolist(HeroType type)
        {
            List<int> herolist = new List<int>();
            foreach (var item in ConfigReader.HeroBuyXmlInfoDict)
            {
                if (!item.Value.bIsShowInHeroInfo)
                {
                    continue;
                }
                if (HeroType.AllHero == type)
                {
                    herolist.Add(item.Key);
                }
                else if (item.Value.HeroKind.Contains((int)type))
                {
                    herolist.Add(item.Key);
                }
            }
            return herolist;
        }

        /// <summary>
        /// 根据类型显示英雄资料
        /// </summary>
        /// <param name="type"></param>
        private void ShowHeroDatum(HeroType type)
        {
            List<int> HeroList = GetShowHerolist(type);
            if (HeroList.Count == 0)
            {
                return;
            }
            for (int mCount = 0; mCount < HeroDataList.Count; mCount++)
            {
                HeroDataPhoto data = HeroDataList[mCount];
                if (mCount >= HeroList.Count)
                {
                    data.mRoot.SetActive(false);
                    continue;
                }
                data.mRoot.SetActive(true);
                data.ShowHeroData(HeroList[mCount]);
            }
            mGrid.repositionNow = true;
            mGrid.Reposition();
            mScroll.ResetPosition();
        }

        private void InitHeroHead()
        {
            List<int> HeroList = GetShowHerolist(HeroType.AllHero);
            foreach (int hero in HeroList)
            {
                ResourceUnit objHomeBaseUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadHeroDataPhoto, ResourceType.PREFAB);
                GameObject obj = GameObject.Instantiate(objHomeBaseUnit.Asset) as GameObject;
                obj.transform.parent = mGrid.transform;
                obj.transform.localPosition = Vector3.zero;
                obj.transform.localScale = Vector3.one;
                HeroDataPhoto hdp = new HeroDataPhoto();
                hdp.mRoot = obj;
                hdp.mHeroNick = obj.transform.FindChild("Name").GetComponent<UILabel>();
                hdp.mHeroIcon = obj.transform.FindChild("Photo").GetComponent<UISprite>();
                hdp.mOwn = obj.transform.FindChild("IsOwn").gameObject;
                hdp.mLastTime = obj.transform.FindChild("LastTime").GetComponent<UILabel>();
                hdp.mGray = obj.transform.FindChild("Gray").GetComponent<UISprite>();
                UIEventListener.Get(obj).onClick += hdp.OnItemSelect;
                HeroDataList.Add(hdp);
            }
        }

        bool isOpen = false;

        UIButton mPopupButton = null;
        Transform mOpenHeroKind = null;
        UIGrid mGrid = null;

        UIButton mHeroAll = null;
        UIButton mHeroAttack = null;
        UIButton mHeroSpell = null;
        UIButton mHeroDefend = null;
        UIButton mHeroAssist = null;
        UIButton mClose = null;
        UIScrollView mScroll;
        UILabel mHeroAttributive;
        private List<HeroDataPhoto> HeroDataList = new List<HeroDataPhoto>();

        public class HeroDataPhoto
        {
            public GameObject mRoot;
            public int mGoodsID;
            public UILabel mHeroNick;
            public UISprite mHeroIcon;
            public GameObject mOwn;
            public UILabel mLastTime;
            public UISprite mGray;
            public CommodityInfos mDityInfo;

            /// <summary>
            /// 刷新英雄显示信息
            /// </summary>
            /// <param name="goodsId"></param>
            public void ShowHeroData(int goodsId)
            {
                HeroBuyConfigInfo mCfgInfo;
                if (!ConfigReader.heroBuyXmlInfoDict.TryGetValue(goodsId, out mCfgInfo))
                {
                    return;
                }
                mGoodsID = goodsId;
                mHeroNick.text = mCfgInfo.Name;
                mHeroIcon.spriteName = mCfgInfo.DefaultIcon;
                mGray.spriteName = mCfgInfo.DefaultIcon;
                mLastTime.gameObject.SetActive(false);
                if (GameUserModel.Instance.OwnedHeroInfoDict.TryGetValue(mGoodsID, out mDityInfo))
                {
                    if (mDityInfo.Expired_time == -1 || mDityInfo.If_free)
                    {
                        mOwn.gameObject.SetActive(true);
                        mGray.gameObject.SetActive(false);
                        return;
                    }
                    FreshHeroLastTime();
                }
                else
                {
                    mOwn.gameObject.SetActive(false);
                    mGray.gameObject.SetActive(true);
                }
            }

            /// <summary>
            /// 刷新英雄显示时间
            /// </summary>
            public void FreshHeroLastTime()
            {
                if (mDityInfo == null || !mRoot.activeSelf || mOwn.activeSelf)
                {
                    return;
                }
                TimeSpan span = mDityInfo.LastTime - DateTime.Now;
                if (span.Ticks > 0)
                {
                    mLastTime.gameObject.SetActive(true);
                    mGray.gameObject.SetActive(false);
                    if (span.Days == 0 && span.Hours == 0 && span.Minutes == 0)
                    {
                        mLastTime.text = span.Hours + "时" + span.Minutes + "分" + span.Seconds + "秒";
                    }
                    else
                    {
                        mLastTime.text = span.Days + "天" + span.Hours + "时" + span.Minutes + "分";
                    }
                }
                else
                {
                    mLastTime.gameObject.SetActive(false);
                    mGray.gameObject.SetActive(true);
                    //GameUserCtrl.Instance.DeleteHero(mGoodsID);
                }
            }

            public void OnItemSelect(GameObject Gobj)
            {
                MarketHeroListCtrl.Instance.SetSelectGoods(mGoodsID);
                MarketHeroInfoCtrl.Instance.Enter();
            }
        }
    }
}