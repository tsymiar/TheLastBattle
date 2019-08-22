using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;

using GameDefine;
using JT.FWW.Tools;
using JT.FWW.GameData;
using UICommon;
using BlGame.GameData;
using BlGame.Model;
using BlGame.GameEntity;
using BlGame.GuideDate;
using BlGame.Resource;
using BlGame.Ctrl;


namespace BlGame.View
{
    public class MarketRuneInfoWindow : BaseWindow
    {
        public MarketRuneInfoWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadGameMarketRuneInfoUI;
        }

        UIButton mBtnClose;

        UIButton mBtnGold;
        UIButton mBtnDiamond;

        UIButton m_SingleBuyBtn;

        UILabel m_NameLabel;
        UILabel m_DesptionLabel;
        RuneGoodsInfo m_MarketGoodsInfo;

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener<GameObject>(EGameEvent.eGameEvent_MarketRuneInfoEnter, InitRuneInfo);
            EventCenter.AddListener(EGameEvent.eGameEvent_MarketRuneInfoExit, Hide);
        }

        void InitRuneInfo(GameObject go)
        {
            int runeID = Convert.ToInt32(go.name);
            Dictionary<int, RuneGoodsInfo> runesDict = MarketRuneListModel.Instance.GetRuneCfgListDict();
            if (!runesDict.TryGetValue(runeID, out m_MarketGoodsInfo))
            {
                Debug.LogError("");
                return;
            }
            Show();
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener<GameObject>(EGameEvent.eGameEvent_MarketRuneInfoEnter, ShowWhenRelease);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_MarketRuneInfoExit, Hide);
        }

        void ShowWhenRelease(GameObject go)
        {
            Show();
        }
        //窗口控件初始化
        protected override void InitWidget()
        {
            mBtnClose = mRoot.FindChild("CloseBtn").GetComponent<UIButton>();

            RuneConfigInfo sRuneConfigInfo = ConfigReader.GetRuneFromID((UInt32)m_MarketGoodsInfo.mGoodsId);
            if (null == sRuneConfigInfo)
            {
                Debug.LogError("null cfg with runeid " + m_MarketGoodsInfo.mGoodsId);
                return;
            }

            UISprite icon = mRoot.FindChild("Icon").GetComponent<UISprite>();
            icon.spriteName = sRuneConfigInfo.Icon;

            //如果只有一个金钱需要居中
            bool ifNeedCentral = m_MarketGoodsInfo.RLGoldPrice < 0 || m_MarketGoodsInfo.RLDiamondPrice < 0;
            if (ifNeedCentral)
            {
                m_SingleBuyBtn = mRoot.FindChild("BuyBtn/Single").GetComponent<UIButton>();
                if (m_MarketGoodsInfo.RLGoldPrice >= 0)
                {
                    UILabel label = m_SingleBuyBtn.gameObject.transform.FindChild("Gold/label").GetComponent<UILabel>();
                    label.text = Convert.ToString(m_MarketGoodsInfo.RLGoldPrice);
                    UIEventListener.Get(m_SingleBuyBtn.gameObject).onClick += OnBuyByGold;
                }
                else
                {
                    UILabel label = m_SingleBuyBtn.gameObject.transform.FindChild("Crystal/label").GetComponent<UILabel>();
                    label.text = Convert.ToString(m_MarketGoodsInfo.RLGoldPrice);
                    UIEventListener.Get(m_SingleBuyBtn.gameObject).onClick += OnBuyByDiamond;
                }

                m_SingleBuyBtn.gameObject.name = Convert.ToString(m_MarketGoodsInfo.mGoodsId);
            }
            else
            {
                GameObject bothGo = mRoot.FindChild("BuyBtn/Both").gameObject;
                bothGo.SetActive(true);

                GameObject singleGo = mRoot.FindChild("BuyBtn/Single").gameObject;
                singleGo.SetActive(false);

                mBtnGold = mRoot.FindChild("BuyBtn/Both/Gold").GetComponent<UIButton>();
                mBtnGold.gameObject.SetActive(true);

                UILabel label = mBtnGold.gameObject.transform.FindChild("label").GetComponent<UILabel>();
                label.text = Convert.ToString(m_MarketGoodsInfo.RLGoldPrice);

                mBtnGold.gameObject.name = Convert.ToString(m_MarketGoodsInfo.mGoodsId);

                mBtnDiamond = mRoot.FindChild("BuyBtn/BuyBtn/Crystal").GetComponent<UIButton>();
                mBtnDiamond.gameObject.SetActive(true);
                UILabel diamondLabel = mBtnDiamond.gameObject.transform.FindChild("label").GetComponent<UILabel>();
                diamondLabel.text = Convert.ToString(m_MarketGoodsInfo.RLDiamondPrice);

                mBtnDiamond.gameObject.name = Convert.ToString(m_MarketGoodsInfo.mGoodsId);

                UIEventListener.Get(mBtnGold.gameObject).onClick += OnBuyByGold;
                UIEventListener.Get(mBtnDiamond.gameObject).onClick += OnBuyByDiamond;
            }

            m_NameLabel = mRoot.FindChild("Name").GetComponent<UILabel>();
            m_NameLabel.text = sRuneConfigInfo.RuneName;

            m_DesptionLabel = mRoot.FindChild("RuneStatus").GetComponent<UILabel>();
            m_DesptionLabel.text = "属性：" + sRuneConfigInfo.Description;

            mRoot.gameObject.SetActive(true);

            InitWindowEvent();
        }

        /// <summary>
        /// 增加界面事件
        /// </summary>
        private void InitWindowEvent()
        {
            EventDelegate.Add(mBtnClose.onClick, OnCloseInterface);
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide); 
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide); 
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
        }

        //显示
        public override void OnEnable()
        {
        }

        //隐藏
        public override void OnDisable()
        {

        }

        /// <summary>
        /// yingx购买的按钮是否可用
        /// </summary>
        /// <param name="isEnable"></param>
        /// 是否可用
        private void DeltGoodBuyBtnEnabled(bool isEnable)
        {
            mBtnDiamond.gameObject.SetActive(isEnable);
            mBtnGold.gameObject.SetActive(isEnable);
        }

        void OnBuyByGold(GameObject go)
        {
            var runeID = Convert.ToInt32(go.name);
            var cfg = ConfigReader.GetRuneFromID((uint)runeID);
            ulong consume = (ulong)MarketRuneListModel.Instance.GetGoodsIno(runeID).RLGoldPrice;
            //if (consume > GameUserModel.Instance.mGameUserGold)
            //{
            //    MsgInfoManager.Instance.ShowMsg(GameDefine.GameConstDefine.LackGoldErrorCode);
            //}
            //else
            //   MarketRuneInfoCtrl.Instance.BuyRune(runeID, GameDefine.ConsumeType.TypeGold, m_C);
            Hide();
        }

        void OnBuyByDiamond(GameObject go)
        {
            var runeID = Convert.ToInt32(go.name);
            var cfg = ConfigReader.GetRuneFromID((uint)runeID);
            ulong consume = (ulong)MarketRuneListModel.Instance.GetGoodsIno(runeID).RLDiamondPrice;
            //if (consume > GameUserModel.Instance.mGameUserGold)
            //{
            //    MsgInfoManager.Instance.ShowMsg(-130902);
            //}
            //else
            //    MarketRuneInfoCtrl.Instance.BuyRune(runeID, GameDefine.ConsumeType.TypeDiamond);
            Hide();
        }

        public void OnRuneSelect()
        {
        }

        public void OnCloseInterface()
        {
            MarketRuneInfoCtrl.Instance.Exit();
        }
    }
}

