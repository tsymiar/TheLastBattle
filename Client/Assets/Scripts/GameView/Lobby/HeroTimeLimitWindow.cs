using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;

using GameDefine;
using JT.FWW.Tools;
using JT.FWW.GameData;
using UICommon;
using BlGame.GameData;
using BlGame;
using BlGame.GameEntity;
using BlGame.GuideDate;
using BlGame.Resource;
using BlGame.Ctrl;
using System.Linq;
using BlGame.Model;


namespace BlGame.View
{
    public class HeroTimeLimitWindow : BaseWindow
    {

        /////////////////////////////////////////////////商城英雄列表////////////////////////////////////////////
        /////////////////////////////////////////////////商城英雄列表////////////////////////////////////////////
        /////////////////////////////////////////////////商城英雄列表////////////////////////////////////////////

        public HeroTimeLimitWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadGameHeroTimeLimitUI;
            mResident = true;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroTimeLimitEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroTimeLimitExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_MarketExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
            HeroTimeLimitList.Clear();
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroTimeLimitEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroTimeLimitExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_MarketExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mAllKindBtn = mRoot.FindChild("SelectKind/All").GetComponent<UIToggle>();
            mMagicKindBtn = mRoot.FindChild("SelectKind/Spell").GetComponent<UIToggle>();
            mAtkKindBtn = mRoot.FindChild("SelectKind/Damage").GetComponent<UIToggle>();
            mAssistKindBtn = mRoot.FindChild("SelectKind/Support").GetComponent<UIToggle>();
            mTankKindBtn = mRoot.FindChild("SelectKind/Tank").GetComponent<UIToggle>();
            mScroll = mRoot.FindChild("HeroScrollView").GetComponent<UIScrollView>();
            mGrid = mRoot.FindChild("HeroScrollView/OptionItems").GetComponent<UIGrid>();
            mLeft = mRoot.FindChild("Arrow/Left").gameObject;
            mRight = mRoot.FindChild("Arrow/Right").gameObject;
            EventDelegate.Add(mAllKindBtn.onChange, OnAllKind);
            EventDelegate.Add(mMagicKindBtn.onChange, OnMagicKind);
            EventDelegate.Add(mAtkKindBtn.onChange, OnAtkKind);
            EventDelegate.Add(mAssistKindBtn.onChange, OnAssistKind);
            EventDelegate.Add(mTankKindBtn.onChange, OnTankKind);
            if (!mScroll.gameObject.activeInHierarchy)
            {
                mScroll.gameObject.SetActive(true);
            }
            mScroll.onDragFinished += OnMoveList;
            InitHeroListCards();
            foreach (HeroCard card in HeroTimeLimitList)
            {
                UIEventListener.Get(card.mRoot).onClick += OnSelectHero;
            }
        }

        private void OnMoveList()
        {
            Vector3 constraint = mScroll.panel.CalculateConstrainOffset(mScroll.bounds.min, mScroll.bounds.max);
            if (constraint.magnitude > 0.5f)
            {
                if (constraint.x > 0f)//右到头
                {
                    mLeft.gameObject.SetActive(true);
                    mRight.gameObject.SetActive(false);
                }
                else//左到头
                {
                    mLeft.gameObject.SetActive(false);
                    mRight.gameObject.SetActive(true);
                }
            }
            else
            {
                mLeft.gameObject.SetActive(true);
                mRight.gameObject.SetActive(true);
            }
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<int>(EGameEvent.eGameEvent_RefreshTimeLimitHero, RefreshMarketHeroList);
            EventCenter.AddListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff);
            EventCenter.AddListener<GuideTaskType, int>(EGameEvent.eGameEvent_PlayChildTaskFinish, OnFinishGuideTask);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<int>(EGameEvent.eGameEvent_RefreshTimeLimitHero, RefreshMarketHeroList);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SdkLogOff, SdkLogOff);
            EventCenter.RemoveListener<GuideTaskType, int>(EGameEvent.eGameEvent_PlayChildTaskFinish, OnFinishGuideTask);
        }

        //显示
        public override void OnEnable()
        {
            if (!mAllKindBtn.value)
            {
                mAllKindBtn.value = true;
            }
            ShowHeroList(HeroType.AllHero);
        }

        //隐藏
        public override void OnDisable()
        {

        }

        /// <summary>
        /// 当完成引导任务时，卡片要刷新位置
        /// </summary>
        /// <param name="type"></param>
        /// <param name="taskId"></param>
        private void OnFinishGuideTask(GuideTaskType type, int taskId)
        {
            FreshGridPosition();
        }

        private void SdkLogOff()
        {
            HeroTimeLimitCtrl.Instance.Exit();
        }

        void OnSelectHero(GameObject obj)
        {
            foreach (HeroCard card in HeroTimeLimitList)
            {
                if (card.mRoot == obj)
                {
                    MarketHeroListCtrl.Instance.SetSelectGoods(card.mGoodsIdChild);
                    MarketHeroListCtrl.Instance.SetRootGoods(card.mGoodsId);
                    MarketHeroInfoCtrl.Instance.Enter();
                    break;
                }
            }
        }

        protected void OnAllKind()
        {
            if (mAllKindBtn.value)
            {
                ShowHeroList(HeroType.AllHero);
            }
        }

        protected void OnMagicKind()
        {
            if (mMagicKindBtn.value)
            {
                ShowHeroList(HeroType.SpellHero);
            }
        }

        protected void OnAtkKind()
        {
            if (mAtkKindBtn.value)
            {
                ShowHeroList(HeroType.AttackHero);
            }
        }

        protected void OnTankKind()
        {
            if (mTankKindBtn.value)
            {
                ShowHeroList(HeroType.DefendHero);
            }
        }

        protected void OnAssistKind()
        {
            if (mAssistKindBtn.value)
            {
                ShowHeroList(HeroType.AssistHero);
            }
        }

        /// <summary>
        /// 刷新商城的英雄显示信息
        /// </summary>
        /// <param name="goodsId"></param>
        private void RefreshMarketHeroList(int goodsId)
        {
            HeroBuyConfigInfo buyInfo = ConfigReader.GetHeroBuyInfo(goodsId);
            if (buyInfo == null)
            {
                return;
            }
            foreach (HeroCard card in HeroTimeLimitList)
            {
                if (card.mHeroId == buyInfo.UnlockHeroID)
                {
                    card.mGoodsIdChild = goodsId;
                    this.DeltCardInfo(card.mGoodsId, card);
                }
            }
        }

        int GetGoodsId(int goodsId)
        {
            HeroBuyConfigInfo buyInfo = ConfigReader.GetHeroBuyInfo(goodsId);
            if (buyInfo == null)
                return 0;
            foreach (var item in ConfigReader.HeroBuyXmlInfoDict.Values)
            {
                if (item.Time == -1)
                {
                    if (item.UnlockHeroID == buyInfo.UnlockHeroID)
                    {
                        return item.GoodsId;
                    }
                }
            }
            return 0;
        }

        /// <summary>
        /// 通过商品Id获取当前这个商品英雄所在的卡片
        /// </summary>
        /// <param name="goodsId"></param>
        /// <returns></returns>
        private HeroCard GetCardByGoodsId(int mGoodId)
        {
            foreach (HeroCard card in HeroTimeLimitList)
            {
                if (card.mGoodsId == mGoodId)
                {
                    return card;
                }
            }
            return null;
        }

        /// <summary>
        /// 初始化所有的可出售卡片信息
        /// </summary>
        void InitHeroListCards()
        {
            List<int> mHeroList = MarketHeroListModel.Instance.GetHeroListTime(HeroType.AllHero);
            if (HeroTimeLimitList.Count != 0)
            {
                HeroTimeLimitList.Clear();
            }

            foreach (int hero in mHeroList)
            {
                ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadGameMarketHeroTag, ResourceType.PREFAB);
                GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
                obj.transform.parent = mGrid.transform;
                obj.transform.localScale = Vector3.one;
                obj.name = (HeroTimeLimitList.Count + 1).ToString();
                HeroCard card = new HeroCard();
                card.mRoot = obj;
                card.mGoodsId = hero;
                card.mSpriteIcon = obj.transform.FindChild("Portrait").GetComponent<UISprite>();
                Transform cost = obj.transform.FindChild("Cost");
                card.mDiamond_Common = cost.FindChild("Crystal1").gameObject;
                card.mDiamond_DisCount = cost.FindChild("Crystal2").gameObject;
                card.mGold_Common = cost.FindChild("Gold1").gameObject;
                card.mGold_DisCount = cost.FindChild("Gold2").gameObject;
                card.mGoldLine = cost.FindChild("Gold/Line");
                card.mCrystalLine = cost.FindChild("Crystal/Line");
                card.IsOwn = obj.transform.FindChild("IsOwn");
                card.mName = obj.transform.FindChild("NamePlate/Name").GetComponent<UILabel>();
                card.TagList.Add(obj.transform.FindChild("Discount").gameObject);
                card.TagList.Add(obj.transform.FindChild("Hot").gameObject);
                card.TagList.Add(obj.transform.FindChild("New").gameObject);
                HeroTimeLimitList.Add(card);
            }
            if (HeroTimeLimitList.Count != 0)
            {
                UIGuideCtrl.Instance.AddUiGuideEventBtn(HeroTimeLimitList[0].mRoot);
            }
        }

        /// <summary>
        /// 显示英雄列表
        /// </summary>
        /// <param name="type"></param>
        /// 要显示的英雄类型
        protected void ShowHeroList(HeroType type)
        {
            List<int> heros = MarketHeroListModel.Instance.GetHeroListTime(type);
            if (heros == null || heros.Count == 0)
            {
                return;
            }
            int index = 0;
            for (int i = 0; i < HeroTimeLimitList.Count; i++)
            {
                HeroCard mCard = HeroTimeLimitList[i];
                if (i >= heros.Count)
                {
                    mCard.mRoot.SetActive(false);
                    continue;
                }
                mCard.mRoot.SetActive(true);
                mCard.mGoodsId = GetGoodsId(heros[i]);
                mCard.mGoodsIdChild = heros[i];
                DeltCardInfo(heros[i], mCard);
                index++;
            }
            mLeft.gameObject.SetActive(false);
            mRight.gameObject.SetActive(index > 3);
            GameMethod.ScrollViewTweenPosition(mScroll, 729f);
            FreshGridPosition();
        }

        /// <summary>
        /// 刷新格子的坐标
        /// </summary>
        private void FreshGridPosition()
        {
            mGrid.enabled = true;
            mGrid.repositionNow = true;
            mGrid.Reposition();
            mScroll.restrictWithinPanel = true;
            mScroll.ResetPosition();
        }

        /// <summary>
        /// 处理商城的英雄卡片信息
        /// </summary>
        /// <param name="goodsId"></param>
        /// 商品Id
        /// <param name="card"></param>
        /// 卡片
        private void DeltCardInfo(int goodsId, HeroCard card)
        {
            HeroBuyConfigInfo info = ConfigReader.GetHeroBuyInfo(goodsId);
            int heroId = info.UnlockHeroID;
            card.mHeroId = heroId;
            card.mSpriteIcon.spriteName = info.DefaultIcon;
            card.mName.text = info.Name;
            bool isShow = GameUserModel.Instance.OwnedHeroInfoDict.ContainsKey(goodsId);
            bool isVib = GameUserModel.Instance.OwnedHeroInfoDict.ContainsKey(card.mGoodsId);
            card.IsOwn.gameObject.SetActive(isShow || isVib);
            card.ReFreshTimeCardTag();
        }

        UIToggle mAllKindBtn;
        UIToggle mAtkKindBtn;
        UIToggle mMagicKindBtn;
        UIToggle mTankKindBtn;
        UIToggle mAssistKindBtn;
        UIScrollView mScroll;
        UIGrid mGrid;

        GameObject mLeft = null;
        GameObject mRight = null;
        public List<HeroCard> HeroTimeLimitList = new List<HeroCard>();          //  所有的英雄卡片
        //public List<GameObject> HeroListObjects = new List<GameObject>();//创建出来所有的英雄列表
        //Dictionary<GameObject, HeroCard> HeroListShowDict = new Dictionary<GameObject, HeroCard>();//根据类型显示的英雄列表

        public class HeroCard
        {
            public GameObject mRoot;
            public UISprite mSpriteIcon;
            public List<GameObject> TagList = new List<GameObject>();

            public GameObject mGold_Common;
            public GameObject mDiamond_Common;
            public GameObject mGold_DisCount;
            public GameObject mDiamond_DisCount;
            public UILabel mName;
            public Transform mGoldLine;
            public Transform mCrystalLine;
            public Transform IsOwn;
            public int mHeroId;
            public int mGoodsId;
            public int mGoodsIdChild;

            /// <summary>
            /// 显示卡片Tag
            /// </summary>
            public void ReFreshCardTag()
            {
                MarketGoodsInfo mGoodInfo;
                if (!MarketHeroListModel.Instance.HeroListDict.TryGetValue(this.mGoodsId, out mGoodInfo))
                {
                    return;
                }
                int tag = -1;
                if (mGoodInfo.ConfigType == GSToGC.GoodsCfgInfo.CfgType.CfgType_Discount)
                {
                    tag = 0;
                }
                else if (mGoodInfo.ConfigType == GSToGC.GoodsCfgInfo.CfgType.CfgType_hot)
                {
                    tag = 1;
                }
                else if (mGoodInfo.ConfigType == GSToGC.GoodsCfgInfo.CfgType.CfgType_New)
                {
                    tag = 2;
                }
                for (int i = 0; i < this.TagList.Count; i++)
                {
                    if (tag == i)
                    {
                        TagList[i].SetActive(true);
                        continue;
                    }
                    TagList[i].SetActive(false);
                }
            }

            /// <summary>
            /// 刷新卡片上显示的价格
            /// </summary>
            /// 
            public void ReFreshTimeCardTag()
            {
                HeroBuyConfigInfo conInfo = ConfigReader.GetHeroBuyInfo(this.mGoodsIdChild);
                if (conInfo == null)
                    return;
                mGold_DisCount.SetActive(false);
                mDiamond_DisCount.SetActive(false);
                if (IsOwn.gameObject.activeSelf)
                {
                    mGold_Common.SetActive(false);
                    mDiamond_Common.SetActive(false);
                    return;
                }
                if (conInfo.Consume != null && conInfo.Consume.Count > 1)
                {
                    mGold_Common.transform.FindChild("LabelMoney1").GetComponent<UILabel>().text = conInfo.Consume.ElementAt(0).Value.ToString();
                    mDiamond_Common.transform.FindChild("LabelMoney1").GetComponent<UILabel>().text = conInfo.Consume.ElementAt(1).Value.ToString();
                    mGold_Common.SetActive(true);
                    mDiamond_Common.SetActive(true);
                }
            }
        }
    }

}
