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
    public class RuneBuyWindow : BaseWindow
    {
        UISprite m_RuneIcon;
        UILabel m_TotConsume;
        UILabel m_NameLabel;

        UIButton m_DecNumBtn;
        UIButton m_AddNumBtn;
        UILabel m_BuyNumLabel;

        UIButton m_BuyBtn;

        UILabel m_DescriptionLabel;
        UIToggle m_GoldBuyToggle;
        UIToggle m_CrystalBuyToggle;
        UIButton m_CloseBtn;
        UInt32 m_RuneID = 0;
        int m_CurNum = 0;

        public RuneBuyWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadRuneBuyUI;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener<GameObject>(EGameEvent.eGameEvent_RuneBuyWindowEnter, InitRuneInfo);
            EventCenter.AddListener(EGameEvent.eGameEvent_RuneBuyWindowExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_HomePageExit, ParentClose);
        }

        void InitRuneInfo(GameObject go)
        {
            m_RuneID = Convert.ToUInt32(go.name);

            Show();
        }

        void ParentClose()
        {
            mResident = false;
            Hide();
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener<GameObject>(EGameEvent.eGameEvent_RuneBuyWindowEnter, InitRuneInfo);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_RuneBuyWindowExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HomePageExit, ParentClose);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            m_RuneIcon = mRoot.FindChild("Icon").GetComponent<UISprite>();
            m_TotConsume = mRoot.Find("TotalLabel").GetComponent<UILabel>();

            m_DecNumBtn = mRoot.Find("SelectNum/MinusButton").GetComponent<UIButton>();
            EventDelegate.Add(m_DecNumBtn.onClick, OnDecNum);

            m_AddNumBtn = mRoot.Find("SelectNum/AddButton").GetComponent<UIButton>();
            EventDelegate.Add(m_AddNumBtn.onClick, AddNum);

            m_BuyNumLabel = mRoot.Find("SelectNum/NumLabel").GetComponent<UILabel>();
            m_BuyNumLabel.text = "1";

            m_BuyBtn = mRoot.Find("BuyBtn").GetComponent<UIButton>();
            EventDelegate.Add(m_BuyBtn.onClick, OnBuy);

            m_DescriptionLabel = mRoot.Find("StatusLabel").GetComponent<UILabel>();

            m_GoldBuyToggle = mRoot.Find("ChooseType/Gold").GetComponent<UIToggle>();
            m_CrystalBuyToggle = mRoot.Find("ChooseType/Crystal").GetComponent<UIToggle>();

            EventDelegate.Add(m_GoldBuyToggle.onChange, SelectBuy);
            EventDelegate.Add(m_CrystalBuyToggle.onChange, SelectBuy);

            m_CloseBtn = mRoot.Find("CloseBtn").GetComponent<UIButton>();
            EventDelegate.Add(m_CloseBtn.onClick, Hide);

            m_NameLabel = mRoot.Find("Icon/NameLabel").GetComponent<UILabel>();
        }

        void OnBuy()
        {
            var cfg = ConfigReader.GetRuneFromID(m_RuneID);
            if (m_BuyType == BuyType.buyDiamond)
            {
                if (m_Consume > (int)GameUserModel.Instance.mGameUserDmd)
                {
                    MsgInfoManager.Instance.ShowMsg(GameDefine.GameConstDefine.LackGoldErrorCode);
                }
                else
                    MarketRuneInfoCtrl.Instance.BuyRune((int)m_RuneID, GameDefine.ConsumeType.TypeDiamond, m_CurNum);
            }
            else
            {
                if (m_Consume > (int)GameUserModel.Instance.mGameUserGold)
                {
                    MsgInfoManager.Instance.ShowMsg(-130902);
                }
                else
                    MarketRuneInfoCtrl.Instance.BuyRune((int)m_RuneID, GameDefine.ConsumeType.TypeGold, m_CurNum);
            }

            Hide();
        }

        private void UpdateStatus()
        {
            RuneConfigInfo sRuneConfigInfo = ConfigReader.GetRuneFromID(m_RuneID);
            var per = sRuneConfigInfo.effectPer * m_CurNum;
            var val = sRuneConfigInfo.effectVal * m_CurNum;
            m_DescriptionLabel.text = RuneConfig.ModifyDes(val, per, sRuneConfigInfo).ToString();
        }

        enum BuyType
        {
            buyGold,
            buyDiamond,
        }

        BuyType m_BuyType;
        void SelectBuy()
        {
            UIToggle currToggle = UIToggle.current;
            currToggle.startsActive = true;
            if (currToggle != null && currToggle.value)
            {
                var cfg = MarketRuneListModel.Instance.GetGoodsIno((int)m_RuneID);
                if (currToggle == m_GoldBuyToggle)
                {
                    m_BuyType = BuyType.buyGold;
                }
                else if (currToggle == m_CrystalBuyToggle)
                {
                    m_BuyType = BuyType.buyDiamond;
                }
            }

            CalculateConsume();
        }

        int m_Consume = 0;
        void CalculateConsume()
        {
            var cfg = MarketRuneListModel.Instance.GetGoodsIno((int)m_RuneID);
            if (cfg == null)
            {
                return;
            }
            if (m_BuyType == BuyType.buyDiamond)
            {
                m_Consume = cfg.RLDiamondPrice * m_CurNum;
                m_TotConsume.text = m_Consume.ToString() + "钻石";
            }
            else
            {
                m_Consume = cfg.RLGoldPrice * m_CurNum;
                m_TotConsume.text = m_Consume.ToString() + "金币";
            }
        }
        void OnDecNum()
        {
            if (m_CurNum <= 1)
            {
                return;
            }
            --m_CurNum;
            m_BuyNumLabel.text = m_CurNum.ToString();
            CalculateConsume();

            UpdateStatus();
        }

        void AddNum()
        {
            ++m_CurNum;
            m_BuyNumLabel.text = m_CurNum.ToString();
            CalculateConsume();

            UpdateStatus();
        }

        public void OnCloseInterface()
        {
            Hide();
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

        public override void Update(float deltaTime)
        {
        }


        //显示
        public override void OnEnable()
        {
            m_CurNum = 1;
            m_Consume = 0;
            if (m_RuneID > 0)
            {
                var cfg = ConfigReader.GetRuneFromID(m_RuneID);

                m_RuneIcon.spriteName = cfg.Icon;
                m_DescriptionLabel.text = cfg.Description;
                m_TotConsume.text = cfg.consumeGold.ToString();
                m_NameLabel.text = cfg.RuneName;

                var priceCfg = MarketRuneListModel.Instance.GetGoodsIno((int)m_RuneID);
                if (priceCfg.RLDiamondPrice < 1)
                {
                    m_CrystalBuyToggle.gameObject.SetActive(false);
                }

                if (priceCfg.RLGoldPrice < 1)
                {
                    m_GoldBuyToggle.gameObject.SetActive(false);
                }

                UpdateStatus();
            }
        }

        //隐藏
        public override void OnDisable()
        {
            m_CurNum = 1;
            m_RuneID = 0;
            m_Consume = 0;
        }

    }
}
