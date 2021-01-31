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
    public class RuneRefreshWindow : BaseWindow
    {
        UIButton m_WashBtn;
        UISprite[] m_Slot_SpriteArray;
        UIButton m_Close_Btn;
        UIButton mPopupListBtn;
        UILabel m_PopList_Label;
        GameObject m_RuneLevelGo;
        GameObject[] m_LevelLabelArray;
        UIScrollView m_BagScroll;
        UIGrid m_BagGrid;
        UIScrollView m_RefreshScroll;
        UIGrid m_RefreshGrid;
        GameObject m_BackGroundGO;
        float m_SuccessTime = -1;
        uint m_NewID = 0;
        GameObject m_EffectGO;
        const int RuneWashCfgBegin = 130005;
        const int RuneWashCfgEnd = 130010;

        public class RefreshCardInfo
        {
            public GameObject go
            {
                set;
                get;
            }
            public OtherItemConfigInfo cfg
            {
                set;
                get;
            }
        }

        public SlotInfo m_RuneSlotInfo = null;
        public SlotInfo m_RefreshCardSlotInfo = null;
        public int m_Curlevel = 0;

        public Dictionary<uint, RefreshCardInfo> refreshCardListID2GoDict = new Dictionary<uint, RefreshCardInfo>();//所有符文！
        public Dictionary<uint, nowAllRuneGOCache> nowAllRuneGO = new Dictionary<uint, nowAllRuneGOCache>();//当前等级存在的所有符文!等级不

        public RuneRefreshWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadRuneRefreshWindowRes;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_RuneRefeshWindowEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_RuneRefeshWindowExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_HomePageExit, ParentClose);
        }

        void ParentClose()
        {
            mResident = false;
            Hide();
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
            m_Curlevel = 0;
            m_RuneSlotInfo = null;
            m_RefreshCardSlotInfo = null;
            if (m_EffectGO != null)
            {
                MonoBehaviour.DestroyImmediate(m_EffectGO);
                m_EffectGO = null;
            }
            refreshCardListID2GoDict.Clear();
            nowAllRuneGO.Clear();
            m_SuccessTime = -1;
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_RuneRefeshWindowEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_RuneRefeshWindowExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HomePageExit, ParentClose);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            m_Slot_SpriteArray = new UISprite[GameDefine.GameConstDefine.MaxRefreshSlotNum];
            string slotPerStr = "CombineSlot/Slot";
            for (int i = 0; i < GameDefine.GameConstDefine.MaxRefreshSlotNum; ++i)
            {
                string index = Convert.ToString(i + 1);
                string fileStr = slotPerStr + index;
                m_Slot_SpriteArray[i] = mRoot.FindChild(fileStr).GetComponent<UISprite>();
                m_Slot_SpriteArray[i].name = Convert.ToString(i);
                m_Slot_SpriteArray[i].spriteName = "";
            }

            UIEventListener.Get(m_Slot_SpriteArray[GameDefine.GameConstDefine.RefreshRuneSlotIndex].gameObject).onClick += onClickSlot;
            UIEventListener.Get(m_Slot_SpriteArray[GameDefine.GameConstDefine.RefreshRuneCardIndex].gameObject).onClick += onClickRefreshSlot;

            m_Close_Btn = mRoot.FindChild("CloseBtn").GetComponent<UIButton>();
            EventDelegate.Add(m_Close_Btn.onClick, Hide);

            mPopupListBtn = mRoot.FindChild("RuneSelect/PopupList").GetComponent<UIButton>();
            EventDelegate.Add(mPopupListBtn.onClick, OnShowLevel);

            m_RuneLevelGo = mRoot.FindChild("RuneSelect/RuneLevel").gameObject;

            m_LevelLabelArray = new GameObject[GameDefine.GameConstDefine.MaxRuneLevel];
            string resFile = "RuneSelect/RuneLevel/Level";
            for (int i = 0; i < m_LevelLabelArray.GetLength(0); ++i)
            {
                string indexStr = Convert.ToString(i);
                string filename = resFile + indexStr;

                GameObject levelGo = mRoot.FindChild(filename).gameObject;
                levelGo.name = indexStr;
                UIEventListener.Get(levelGo).onClick += onClickLevel;
                m_LevelLabelArray[i] = levelGo;
            }

            m_WashBtn = mRoot.FindChild("CombineBtn").GetComponent<UIButton>();
            EventDelegate.Add(m_WashBtn.onClick, onClickWash);
            m_WashBtn.isEnabled = false;

            m_PopList_Label = mRoot.FindChild("RuneSelect/PopupList/Label").GetComponent<UILabel>();

            m_BagScroll = mRoot.FindChild("Package").GetComponent<UIScrollView>();
            m_BagGrid = mRoot.FindChild("Package/Grid").GetComponent<UIGrid>();

            m_RefreshScroll = mRoot.FindChild("ScrollPackage/List").GetComponent<UIScrollView>();
            m_RefreshGrid = mRoot.FindChild("ScrollPackage/List/Grid").GetComponent<UIGrid>();

            m_BackGroundGO = mRoot.FindChild("Background").gameObject;
            LoadRunes();

            LoadRefresh();
        }

        private void LoadRefresh()
        {
            var refreshMap = GoodsModel.Instance.GetRuneBaptzeMap();
            foreach (var kv in refreshMap)
            {
                uint refreshID = (uint)kv.Key;
                if (refreshID < RuneWashCfgBegin || refreshID > RuneWashCfgEnd)
                {
                    continue;
                }
                LoadOneRefreshCard(refreshID, kv.Value.mNum);
            }

            m_RefreshGrid.Reposition();
            m_RefreshScroll.ResetPosition();
        }

        private void LoadOneRefreshCard(uint refreshID, int num)
        {
            var cfg = ConfigReader.GetOtherItemInfo(refreshID);
            if (null == cfg)
            {
                return;
            }

            ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadRuneRefreshCardRes, ResourceType.PREFAB);
            GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;

            UIEventListener.Get(obj).onClick += onClickRefreshCard;

            obj.transform.parent = m_RefreshGrid.transform;
            obj.transform.localScale = Vector3.one;
            obj.transform.localPosition = Vector3.zero;
            obj.name = (refreshID).ToString();

            var icon = obj.transform.FindChild("Icon").GetComponent<UISprite>();
            icon.spriteName = cfg.icon;

            var numLabel = obj.transform.FindChild("NumLabel").GetComponent<UILabel>();
            numLabel.text = num.ToString();

            var levelLabel = obj.transform.FindChild("LevelLabel").GetComponent<UILabel>();
            levelLabel.text = cfg.effectvalue.ToString();

            RefreshCardInfo sInfo = new RefreshCardInfo();
            sInfo.go = obj;
            sInfo.cfg = cfg;
            refreshCardListID2GoDict.Add(refreshID, sInfo);
        }
        private void onClickRefreshCard(GameObject go)
        {
            if (m_SuccessTime > 0)
            {
                return;
            }

            if (m_RuneSlotInfo == null)
            {
                MsgInfoManager.Instance.ShowMsg(10037);
                return;
            }

            RuneConfigInfo sRuneConfigInfo = ConfigReader.GetRuneFromID(m_RuneSlotInfo.runeID);

            var nowLevel = sRuneConfigInfo.level;

            var refreshID = Convert.ToUInt32(go.name);
            var sInfo = refreshCardListID2GoDict[refreshID];
            if (sInfo.cfg.effectvalue < nowLevel)
            {
                MsgInfoManager.Instance.ShowMsg(10038);
                return;
            }

            var nowInfo = GoodsModel.Instance.GetRuneBaptze((int)refreshID);
            int num = nowInfo.mNum - 1;
            if (num == 0)
            {
                go.transform.parent = null;
                m_RefreshGrid.RemoveChild(go.transform);
                MonoBehaviour.DestroyImmediate(go);
                m_RefreshGrid.Reposition();
                m_RefreshScroll.ResetPosition();
                refreshCardListID2GoDict.Remove(refreshID);
            }
            else
            {
                var numLabel = go.transform.FindChild("NumLabel").GetComponent<UILabel>();
                numLabel.text = num.ToString();
            }

            if (m_RefreshCardSlotInfo == null)
            {
                m_RefreshCardSlotInfo = new SlotInfo();
            }
            else
            {
                //原来有洗练券，现在还原
                var oriInfo = GoodsModel.Instance.GetRuneBaptze((int)m_RefreshCardSlotInfo.runeID);
                var freshCardInfo = refreshCardListID2GoDict[m_RefreshCardSlotInfo.runeID];
                if (refreshID != m_RefreshCardSlotInfo.runeID)
                {
                    if (oriInfo.mNum == 1)
                    {
                        //记载洗练券
                        LoadOneRefreshCard(m_RefreshCardSlotInfo.runeID, oriInfo.mNum);
                        m_RefreshGrid.Reposition();
                        m_RefreshScroll.ResetPosition();
                    }
                    else
                    {
                        var numLabel = freshCardInfo.go.transform.FindChild("NumLabel").GetComponent<UILabel>();
                        numLabel.text = oriInfo.mNum.ToString();
                    }
                }
            }

            m_Slot_SpriteArray[GameDefine.GameConstDefine.RefreshRuneCardIndex].gameObject.SetActive(true);
            m_Slot_SpriteArray[GameDefine.GameConstDefine.RefreshRuneCardIndex].spriteName = sInfo.cfg.icon;
            m_RefreshCardSlotInfo.runeInfo = go;
            m_RefreshCardSlotInfo.sprite = m_Slot_SpriteArray[GameDefine.GameConstDefine.RefreshRuneCardIndex];
            m_RefreshCardSlotInfo.runeID = refreshID;
        }

        public void onClickSlot(GameObject go)
        {
            if (m_RuneSlotInfo == null)
            {
                return;
            }
            m_RuneSlotInfo.sprite.spriteName = "";
            AudioManager.Instance.PlatUnloadRuneAudio();

            var oneRuneInfo = MarketRuneListModel.Instance.GetOneRuneInfo(m_RuneSlotInfo.runeID);
            LoadOneRune(m_RuneSlotInfo.runeID, oneRuneInfo.num);
            if (oneRuneInfo.num == 1)
            {
                m_BagGrid.Reposition();
                m_BagScroll.ResetPosition();
            }

            m_RuneSlotInfo.sprite.gameObject.SetActive(false);

            ClearFreshSlot();

            m_RuneSlotInfo = null;
            m_WashBtn.isEnabled = false;
        }
        private void ClearFreshSlot()
        {
            if (m_RefreshCardSlotInfo == null)
                return;
            m_RefreshCardSlotInfo.sprite.spriteName = "";
            var oriInfo = GoodsModel.Instance.GetRuneBaptze((int)m_RefreshCardSlotInfo.runeID);
            if (oriInfo.mNum == 1)
            {
                LoadOneRefreshCard(m_RefreshCardSlotInfo.runeID, oriInfo.mNum);
                m_RefreshGrid.Reposition();
            }
            else
            {
                var freshCardInfo = refreshCardListID2GoDict[m_RefreshCardSlotInfo.runeID];

                var numLabel = freshCardInfo.go.transform.FindChild("NumLabel").GetComponent<UILabel>();
                numLabel.text = oriInfo.mNum.ToString();
                freshCardInfo.go.SetActive(true);
            }

            m_RefreshCardSlotInfo.sprite.gameObject.SetActive(false);
            m_RefreshCardSlotInfo = null;
        }

        public void onClickRefreshSlot(GameObject go)
        {
            if (m_RefreshCardSlotInfo == null)
            {
                return;
            }

            AudioManager.Instance.PlatUnloadRuneAudio();

            ClearFreshSlot();
        }

        private int GetGoldConsume(uint runeID)
        {
            int needgold = 0;
            RuneConfigInfo sRuneConfigInfo = ConfigReader.GetRuneFromID(m_RuneSlotInfo.runeID);
            switch (sRuneConfigInfo.level)
            {
                case 1:
                    needgold = 3;
                    break;
                case 2:
                    needgold = 10;
                    break;
                case 3:
                    needgold = 30;
                    break;
                case 4:
                    needgold = 70;
                    break;
                case 5:
                    needgold = 200;
                    break;
                case 6:
                    needgold = 480;
                    break;
            }

            return needgold;
        }
        //点击合成符文
        public void onClickWash()
        {
            if (m_SuccessTime > 0)
            {
                return;
            }

            if (m_RefreshCardSlotInfo != null)
            {
                CGLCtrl_GameLogic.Instance.AskRefresh(m_RuneSlotInfo.runeID, m_RefreshCardSlotInfo.runeID);
                return;
            }

            int needgold = GetGoldConsume(m_RuneSlotInfo.runeID);
            string showStr = GameDefine.GameConstDefine.showWashMsg + needgold.ToString() + "金币";
            EventCenter.Broadcast<EMessageType, string, Callback<bool>>(EGameEvent.eGameEvent_ShowLogicMessage, EMessageType.EMT_SureToWash, showStr, windowsCallBack);
        }

        public void windowsCallBack(bool flag)
        {
            if (flag)
            {
                int needgold = GetGoldConsume(m_RuneSlotInfo.runeID);
                if (needgold > (int)GameUserModel.Instance.mGameUserGold)
                {
                    MsgInfoManager.Instance.ShowMsg(GameDefine.GameConstDefine.LackGoldErrorCode);
                    return;
                }
                
                CGLCtrl_GameLogic.Instance.AskRefresh(m_RuneSlotInfo.runeID, 0);
            }
        }

        private int getLeftShowNum(uint id)
        {
            var oneRuneInfo = MarketRuneListModel.Instance.GetOneRuneInfo(id);
            int leftNum = oneRuneInfo.num;

            if (m_RuneSlotInfo != null && m_RuneSlotInfo.runeID == id)
            {
                return leftNum - 1;
            }

            return leftNum;
        }
        //符文等级过滤
        public void onClickLevel(GameObject go)
        {
            m_Curlevel = Convert.ToInt32(go.name);
            var allRuneList = MarketRuneListModel.Instance.GetBuyedRuneDict();
            if (m_Curlevel == 0)
            {
                m_PopList_Label.text = "全部";
                foreach (var val in allRuneList)
                {
                    if (!nowAllRuneGO.ContainsKey(val.Key))
                    {
                        int num = getLeftShowNum(val.Key);
                        if (num > 0)
                        {
                            LoadOneRune(val.Key, num);
                        }
                    }
                }
            }
            else
            {
                m_PopList_Label.text = "等级" + go.name;
                foreach (var kv in allRuneList)
                {
                    bool ifContine = nowAllRuneGO.ContainsKey(kv.Key);
                    var cfg = ConfigReader.GetRuneFromID(kv.Key);
                    if (cfg.level == m_Curlevel)
                    {
                        if (!ifContine)
                        {
                            int num = getLeftShowNum(kv.Key);
                            if (num > 0)
                            {
                                LoadOneRune(kv.Key, num);
                            }
                        }
                    }
                    else
                    {
                        if (ifContine)
                        {
                            var info = nowAllRuneGO[kv.Key];
                            info.go.transform.parent = null;
                            m_BagGrid.RemoveChild(info.go.transform);
                            nowAllRuneGO.Remove(kv.Key);
                            MonoBehaviour.DestroyImmediate(info.go);
                        }
                    }
                }
            }

            m_BagGrid.Reposition();

            m_BagScroll.ResetPosition();

            m_RuneLevelGo.SetActive(false);
        }

        protected void OnShowLevel()
        {
            if (m_RuneLevelGo.active)
                m_RuneLevelGo.SetActive(false);
            else
                m_RuneLevelGo.SetActive(true);
        }

        public void OnCloseInterface()
        {
            Hide();
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<uint, int, long>(EGameEvent.eGameEvent_RuneBagUpdate, RefeshRuneBagInfo);
            EventCenter.AddListener<uint, int, long>(EGameEvent.eGameEvent_RuneRefreshCardUpdate, RefeshCardInfo);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide); 
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<uint, int, long>(EGameEvent.eGameEvent_RuneBagUpdate, RefeshRuneBagInfo);
            EventCenter.RemoveListener<uint, int, long>(EGameEvent.eGameEvent_RuneRefreshCardUpdate, RefeshCardInfo);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide); 
        }

        public void RefeshCardInfo(uint cardid, int num, long lefttime)
        {
            if (cardid < RuneWashCfgBegin || cardid > RuneWashCfgEnd)
            {
                return;
            }
            m_RefreshCardSlotInfo.sprite.spriteName = "";
            AudioManager.Instance.PlatUnloadRuneAudio();

            if (m_RefreshCardSlotInfo != null && m_RefreshCardSlotInfo.runeID == cardid && num == 0)
            {
                refreshCardListID2GoDict.Remove(cardid);
            }
            else
            {
                var info = refreshCardListID2GoDict[cardid];
                info.go.transform.FindChild("NumLabel").GetComponent<UILabel>().text = num.ToString(); 
            }

            m_RefreshCardSlotInfo = null;
        }

        public override void Update(float deltaTime)
        {
            if (m_SuccessTime > 0)
            {
                if (Time.time - m_SuccessTime >= 0.8)
                {
                    m_EffectGO.transform.parent = null;
                    MonoBehaviour.DestroyImmediate(m_EffectGO);
                    m_EffectGO = null;

                    var oneRuneInfo = MarketRuneListModel.Instance.GetOneRuneInfo(m_NewID);
                    RuneConfigInfo runeCfg = ConfigReader.GetRuneFromID(m_NewID);
                    if (runeCfg.level == m_Curlevel || m_Curlevel == 0)
                    {
                        LoadOneRune(m_NewID, oneRuneInfo.num);
                        m_BagGrid.Reposition();
                    }

                    EventCenter.Broadcast(EGameEvent.eGameEvent_PurchaseSuccessWindowEnter);
                    EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin, EPurchaseType.EPT_Rune, runeCfg.RuneName, runeCfg.Icon, 1);
                    m_SuccessTime = -1;
                }
            }
        }

        string effectName = "FirstEffect";
        public void RefeshRuneBagInfo(uint runeId, int num, long gottime)
        {
            if (m_RuneSlotInfo != null)
            {
                m_RuneSlotInfo.sprite.spriteName = "";
                AudioManager.Instance.PlatUnloadRuneAudio();
                m_RuneSlotInfo = null;
                m_WashBtn.isEnabled = false;
            }

            if (num > 0)
            {
                RuneConfigInfo sRuneConfigInfo = ConfigReader.GetRuneFromID(runeId);
                if (m_EffectGO != null)
                {
                    MonoBehaviour.DestroyImmediate(m_EffectGO);
                }
                m_EffectGO = LoadUiResource.AddChildObject(m_BackGroundGO.transform, GameConstDefine.RuneFreshFlash);
                m_EffectGO.name = effectName;

                m_NewID = runeId;
                m_SuccessTime = Time.time;
            }
        }

        //显示
        public override void OnEnable()
        {
        }

        //隐藏
        public override void OnDisable()
        {
        }

        public void LoadRunes()
        {
            var runeList = MarketRuneListModel.Instance.GetBuyedRuneDict();
            foreach (var kv in runeList)
            {
                if (kv.Value.num == 0)
                {
                    continue;
                }
                LoadOneRune(kv.Key, kv.Value.num);
            }

            m_BagGrid.Reposition();

            m_BagScroll.ResetPosition();
        }

        public void onClickRuneCard(GameObject go)
        {
            if (m_SuccessTime > 0)
            {
                return;
            }
            uint runeID = Convert.ToUInt32(go.name);
            RuneConfigInfo sRuneConfigInfo = ConfigReader.GetRuneFromID(runeID);
            var oneRuneInfo = MarketRuneListModel.Instance.GetOneRuneInfo(runeID);

            if (null == m_RuneSlotInfo)
            {
                m_Slot_SpriteArray[GameDefine.GameConstDefine.RefreshRuneSlotIndex].gameObject.SetActive(true);
                m_Slot_SpriteArray[GameDefine.GameConstDefine.RefreshRuneSlotIndex].spriteName = sRuneConfigInfo.Icon;

                m_RuneSlotInfo = new SlotInfo();

                int num = oneRuneInfo.num - 1;
                if (num == 0)
                {
                    go.transform.parent = null;
                    m_BagGrid.RemoveChild(go.transform);
                    MonoBehaviour.DestroyImmediate(go);
                    nowAllRuneGO.Remove(runeID);
                }
                else
                {
                    UILabel levelLabel = go.transform.FindChild("NumLabel").GetComponent<UILabel>();

                    levelLabel.text = Convert.ToString(num);
                }

                m_WashBtn.isEnabled = true;
            }
            else
            {
                //如果替换不一致的符文，则把原来的换回去，新的减少1
                var runeOnSlotID = m_RuneSlotInfo.runeID;
                if (runeOnSlotID != runeID)
                {
                    RuneConfigInfo runeCfg = ConfigReader.GetRuneFromID(runeOnSlotID);
                    if (runeCfg.level == m_Curlevel || m_Curlevel == 0)
                    {
                        var slotRuneInfo = MarketRuneListModel.Instance.GetOneRuneInfo(runeOnSlotID);
                        LoadOneRune(runeOnSlotID, slotRuneInfo.num);
                        if (slotRuneInfo.num == 1)
                        {
                            m_BagGrid.Reposition();
                        }
                    }

                    int num = oneRuneInfo.num - 1;
                    if (num == 0)
                    {
                        go.transform.parent = null;
                        m_BagGrid.RemoveChild(go.transform);
                        MonoBehaviour.DestroyImmediate(go);
                        nowAllRuneGO.Remove(runeID);
                    }
                    else
                    {
                        UILabel levelLabel = go.transform.FindChild("NumLabel").GetComponent<UILabel>();

                        levelLabel.text = Convert.ToString(num);
                    }
                }
            }

            m_Slot_SpriteArray[GameDefine.GameConstDefine.RefreshRuneSlotIndex].spriteName = sRuneConfigInfo.Icon;
            m_RuneSlotInfo.runeInfo = go;
            m_RuneSlotInfo.sprite = m_Slot_SpriteArray[GameDefine.GameConstDefine.RefreshRuneSlotIndex];
            m_RuneSlotInfo.runeID = runeID;

            ClearFreshSlot();

            m_BagGrid.Reposition();

            m_BagScroll.ResetPosition();
        }

        private void LoadOneRune(uint runeid, int num)
        {
            RuneConfigInfo sRuneConfigInfo = ConfigReader.GetRuneFromID(runeid);

            bool isNewRune = !nowAllRuneGO.ContainsKey(runeid);
            if (isNewRune)
            {
                ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadGameMarketRuneItem, ResourceType.PREFAB);
                GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;

                UIEventListener.Get(obj).onClick += onClickRuneCard;

                obj.transform.parent = m_BagGrid.transform;
                obj.transform.localScale = Vector3.one;
                obj.transform.localPosition = Vector3.zero;

                obj.name = (runeid).ToString();

                UISprite icon = obj.transform.FindChild("Icon").GetComponent<UISprite>();
                icon.spriteName = sRuneConfigInfo.Icon;

                UILabel levelLabel = obj.transform.FindChild("NumLabel").GetComponent<UILabel>();
                levelLabel.text = Convert.ToString(num);

                UILabel desptionLabel = obj.transform.FindChild("StausLabel").GetComponent<UILabel>();
                desptionLabel.text = sRuneConfigInfo.Description;

                nowAllRuneGOCache snowAllRuneGOCache = new nowAllRuneGOCache();
                snowAllRuneGOCache.go = obj;
                snowAllRuneGOCache.id = runeid;
                snowAllRuneGOCache.level = sRuneConfigInfo.level;

                nowAllRuneGO.Add(runeid, snowAllRuneGOCache);
            }
            else
            {
                var obj = nowAllRuneGO[runeid];

                UILabel levelLabel = obj.go.transform.FindChild("NumLabel").GetComponent<UILabel>();
                levelLabel.text = Convert.ToString(num);
            }
        }

    }
}
