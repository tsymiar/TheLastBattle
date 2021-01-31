using UnityEngine;
using System.Collections;
using GameDefine;
using System.Collections.Generic;
using BlGame.GameEntity;
using BlGame;
using BlGame.Ctrl;

namespace BlGame.View
{
    //游戏中战斗信息界面
    public class BattleInfoWindow : BaseWindow
    {
        public BattleInfoWindow() {
            mScenesType = EScenesType.EST_Play;
            mResName = GameConstDefine.LoadBattleInfoUI;
            mResident = false;
        }
        
        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_BattleInfoEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_BattleInfoExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_BattleInfoEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_BattleInfoExit, Hide);

#if UNITY_STANDALONE_WIN || UNITY_EDITOR || SKIP_SDK
#else
                    SdkConector.HideToolBar();
#endif
        }


        //窗口控件初始化
        protected override void InitWidget()
        {
            RecordToggle = mRoot.FindChild("BattleBtn").GetComponent<UIToggle>();
            AttributeToggle = mRoot.FindChild("StatusBtn").GetComponent<UIToggle>();
            SystemSetToggle = mRoot.FindChild("SettingBtn").GetComponent<UIToggle>();
            BattleState = BattleCurrInfo.BattleRecord;

            Transform temp = mRoot.FindChild("BattleInfo/Team/BlueTeam");
            int index = temp.childCount;
            for (int i = 0; i < index; i++)
            {
                AllBlueTeam.Add(new BattleRealInfo(temp.FindChild("Player" + (i + 1))));
            }
            temp = mRoot.FindChild("BattleInfo/Team/RedTeam");
            index = temp.childCount;
            for (int i = 0; i < index; i++)
            {
                AllRedTeam.Add(new BattleRealInfo(temp.FindChild("Player" + (i + 1))));
            }
            PlayerSpeed = mRoot.FindChild("StatusInfo/Speed/Num").GetComponent<UILabel>();
            AttackInterval = mRoot.FindChild("StatusInfo/AtkInterval/Num").GetComponent<UILabel>();

            AttackRange = mRoot.FindChild("StatusInfo/AtkRange/Num").GetComponent<UILabel>();
            ResurgenceTime = mRoot.FindChild("StatusInfo/ResuTime/Num").GetComponent<UILabel>();
            PhysicAttack = mRoot.FindChild("StatusInfo/PhysicAtk/Num").GetComponent<UILabel>();
            SpellsAttack = mRoot.FindChild("StatusInfo/SpellsAtk/Num").GetComponent<UILabel>();

            PhysicDef = mRoot.FindChild("StatusInfo/PhysicDef/Num").GetComponent<UILabel>();
            SpellsDef = mRoot.FindChild("StatusInfo/SpellsDef/Num").GetComponent<UILabel>();
            CloseBtn = mRoot.FindChild("CloseBtn").gameObject;
            tran = mRoot.FindChild("SettingInfo");

            PlayerSpeed.text = "";
            AttackInterval.text = "";
            AttackRange.text = "";
            ResurgenceTime.text = "";

            PhysicAttack.text = "";
            SpellsAttack.text = "";
            PhysicDef.text = "";
            SpellsDef.text = "";

            SoundBtn = tran.FindChild("SoundSwitch").gameObject;
            MusicBtn = tran.FindChild("MusicSwitch").gameObject;
            EffectBtn = tran.FindChild("EffectSwitch").gameObject;
            RangeBtn = tran.FindChild("RangeSwitch").gameObject;

            objOn[0] = SoundBtn.transform.FindChild("On").gameObject;
            ojbOff[0] = SoundBtn.transform.FindChild("Off").gameObject;
            objOn[1] = MusicBtn.transform.FindChild("On").gameObject;
            ojbOff[1] = MusicBtn.transform.FindChild("Off").gameObject;

            objOn[2] = EffectBtn.transform.FindChild("On").gameObject;
            ojbOff[2] = EffectBtn.transform.FindChild("Off").gameObject;
            objOn[3] = RangeBtn.transform.FindChild("On").gameObject;
            ojbOff[3] = RangeBtn.transform.FindChild("Off").gameObject;

            UIEventListener.Get(CloseBtn).onClick += ClosePress;
            UIEventListener.Get(SoundBtn).onClick += SoundPress;
            UIEventListener.Get(MusicBtn).onClick += MusicPress;
            UIEventListener.Get(EffectBtn).onClick += EffectPress;

            UIEventListener.Get(RangeBtn).onClick += RangePress;
            EventDelegate.Add(RecordToggle.onChange, ShowRecordAttrSet);
            EventDelegate.Add(AttributeToggle.onChange, ShowRecordAttrSet);
            EventDelegate.Add(SystemSetToggle.onChange, ShowRecordAttrSet);
            
            SetSave(ref SoundOpenState, UIGameSetting.soundKey, SystemSet.SoundButton);
            SetSave(ref MusicOpenState, UIGameSetting.voiceKey, SystemSet.MusicButton);
            SetSave(ref EffectOpenState, GameMethod.EffectVoc, SystemSet.EffectButton);
            SetSave(ref RangeOpenState, GameMethod.RangeVoc, SystemSet.RangeButton);

        }

        protected override void RealseWidget()
        {

        }

        private void RangePress(GameObject go)
        {
            RangeOpenState = !RangeOpenState;
            Save(ref RangeOpenState, GameMethod.RangeVoc, SystemSet.RangeButton);
        }

        private void EffectPress(GameObject go)
        {
            EffectOpenState = !EffectOpenState;
            Save(ref EffectOpenState, GameMethod.EffectVoc, SystemSet.EffectButton);
        }

        private void MusicPress(GameObject go)
        {
            MusicOpenState = !MusicOpenState;
            Save(ref MusicOpenState, UIGameSetting.voiceKey, SystemSet.MusicButton);
        }

        private void SoundPress(GameObject go)
        {
            SoundOpenState = !SoundOpenState;
            Save(ref SoundOpenState, UIGameSetting.soundKey, SystemSet.SoundButton);
        }

        private void ClosePress(GameObject go)
        {
            BattleInfoCtrl.Instance.Exit();
        }

        void ShowRecordAttrSet()
        {
            UIToggle currToggle = UIToggle.current;
            currToggle.startsActive = true;
            if (currToggle != null && currToggle.value)
            {
                if (currToggle == RecordToggle)
                {
                    Record();
                    BattleState = BattleCurrInfo.BattleRecord;
                }
                else if (currToggle == AttributeToggle)
                {
                    BattleAttr();
                }
                else if (currToggle == SystemSetToggle)
                {
                    BattleState = BattleCurrInfo.BattleSystemSet;
                }
            }
        }

        void SetSave(ref bool saveState, string key, SystemSet systemSet)
        {
            int state = 0;
            if (PlayerPrefs.HasKey(key))
            {
                state = PlayerPrefs.GetInt(key);
                saveState = (state == 1) ? true : false;
            }
            else
            {
                state = saveState ? 1 : 0;
                PlayerPrefs.SetInt(key, state);
            }
            Save(ref saveState, key, systemSet);
        }

        void Save(ref bool saveState, string key, SystemSet systemSet)
        {
            objOn[(int)systemSet].SetActive(saveState);
            ojbOff[(int)systemSet].SetActive(!saveState);
            int state = saveState ? 1 : 0;
            PlayerPrefs.SetInt(key, state);

            switch (systemSet)
            {
                case SystemSet.SoundButton:
                    AudioManager.Instance.EnableSound(saveState);
                    break;
                case SystemSet.MusicButton:
                    AudioManager.Instance.EnableVoice(saveState);
                    break;
                case SystemSet.EffectButton:
                    BattleInfoCtrl.Instance.SetEffect(saveState);
                    break;
                case SystemSet.RangeButton:
                    GameMethod.SetRange(saveState);
                    break;
            }
        }

        void ShowInfo()
        {
            switch(BattleState)
            {
                case BattleCurrInfo.BattleRecord:
                    Record();
                    break;
                case BattleCurrInfo.BattleSystemSet:
                    break;
                case BattleCurrInfo.BattleAttribute:
                    BattleAttr();
                    break;
            }
        }

        public override void OnEnable()
        {
            
        }
        //隐藏
        public override void OnDisable()
        {
            AllBlueTeam.Clear();
            AllRedTeam.Clear();
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerGoods, Record);
            EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerAssist, Record);
            EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerKills, Record);
            EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerDeaths, Record);
            EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerLevel, Record);
            EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerLastHit, Record);
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroAttributesInfo,ShowMine);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerGoods, Record);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerAssist, Record);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerKills, Record);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerDeaths, Record);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerLevel, Record);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroAttributesInfo, ShowMine);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerLastHit, Record);
        }

        void Record()
        {
            int i = 0;
            foreach (var item in BattleingData.AllBlueHeroBattle.Values)
            {
                if (item != null)
                    AllBlueTeam[i++].ShowBattleInfo(item);
            }
            i = 0;
            foreach (var item in BattleingData.AllRedHeroBattle.Values)
            {
                if (item != null)
                    AllRedTeam[i++].ShowBattleInfo(item);
            }
        }
        void BattleAttr()
        {
            BattleInfoCtrl.Instance.AskBattleMine();
            BattleState = BattleCurrInfo.BattleAttribute;
        }
        void ShowMine()
        {
            PlayerSpeed.text = BattleingData.Instance.PlayerSpeed.ToString();
            AttackInterval.text = (BattleingData.Instance.AttackInterval / 1000f).ToString();
            AttackRange.text = BattleingData.Instance.AttackRange.ToString();
            ResurgenceTime.text = (BattleingData.Instance.ResurgenceTime / 1000f).ToString();
            PhysicAttack.text = BattleingData.Instance.PhysicAttack.ToString();
            SpellsAttack.text = BattleingData.Instance.SpellsAttack.ToString();
            PhysicDef.text = BattleingData.Instance.PhysicDef.ToString();
            SpellsDef.text = BattleingData.Instance.SpellsDef.ToString();
        }

        GameObject CloseBtn;
        UIToggle RecordToggle;
        UIToggle AttributeToggle;
        UIToggle SystemSetToggle;
        enum BattleCurrInfo
        {
            BattleNull,
            BattleRecord,
            BattleAttribute,
            BattleSystemSet,
        }

        /// <summary>
        /// 实时战斗信息
        /// </summary>
        List<BattleRealInfo> AllBlueTeam = new List<BattleRealInfo>();//date
        List<BattleRealInfo> AllRedTeam = new List<BattleRealInfo>();//date
        BattleCurrInfo BattleState = BattleCurrInfo.BattleNull;
        /// <summary>
        /// 个人战斗属性
        /// </summary>
        UILabel PlayerSpeed;
        UILabel AttackInterval;
        UILabel AttackRange;
        UILabel ResurgenceTime;
        UILabel PhysicAttack;
        UILabel SpellsAttack;
        UILabel PhysicDef;
        UILabel SpellsDef;
        /// <summary>
        /// 按键
        /// </summary>


        private bool SoundOpenState = true;
        private bool MusicOpenState = true;
        private bool EffectOpenState = true;
        private bool RangeOpenState = true;
        GameObject SoundBtn;
        GameObject MusicBtn;
        GameObject EffectBtn;
        GameObject RangeBtn;
        GameObject[] objOn = new GameObject[4];
        GameObject[] ojbOff = new GameObject[4];
        Transform tran = null;
    }
    public class BattleRealInfo
    {
        private Transform temp;
        public UISprite PlayerIcon;
        public UILabel PlayerName;
        public UILabel PlayerLevel;
        public UILabel PlayerKills;
        public UILabel PlayerDeath;
        public UILabel PlayerPlains;
        public UILabel Assist;
        public Dictionary<int, UISprite> PlayerGoodsDic = new Dictionary<int, UISprite>();
        public BattleRealInfo(Transform temp)
        {
            // TODO: Complete member initialization
            this.temp = temp;
            PlayerName = temp.FindChild("Name/Label").GetComponent<UILabel>();
            PlayerKills = temp.FindChild("Kill").GetComponent<UILabel>();
            PlayerLevel = temp.FindChild("HeadPhoto/Level").GetComponent<UILabel>();
            PlayerDeath = temp.FindChild("Death").GetComponent<UILabel>();
            PlayerPlains = temp.FindChild("Plains").GetComponent<UILabel>();
            PlayerIcon = temp.FindChild("HeadPhoto/Head").GetComponent<UISprite>();
            Assist = temp.FindChild("Assists").GetComponent<UILabel>();
            PlayerName.text = "";
            PlayerKills.text = "";
            PlayerLevel.text = "";
            PlayerDeath.text = "";
            PlayerPlains.text = "";
            PlayerIcon.spriteName = "";
            Assist.text = "";
            Transform tran = temp.FindChild("Items");
            int index = tran.childCount;
            for (int i = 0; i < index; i++)
            {
                string str = "Item" + (i + 1) + "/icon";
                UISprite sp = tran.FindChild(str).GetComponent<UISprite>();
                sp.spriteName = "";
                PlayerGoodsDic.Add(i , sp);
            }
        }

        //void SetVib(bool isVib)
        //{
        //    PlayerName.gameObject.SetActive(isVib);
        //    PlayerKills.gameObject.SetActive(isVib);
        //    PlayerLevel.gameObject.SetActive(isVib);
        //    PlayerDeath.gameObject.SetActive(isVib);
        //    PlayerPlains.gameObject.SetActive(isVib);
        //    PlayerIcon.gameObject.SetActive(isVib);
        //    Assist.gameObject.SetActive(isVib);
        //}
        internal void ShowBattleInfo(HeroBattleInfo heroBattleInfo)
        {
            PlayerName.text = heroBattleInfo.HeroName;
            PlayerLevel.text = heroBattleInfo.Level.ToString();
            PlayerKills.text = heroBattleInfo.Kills.ToString();
            PlayerDeath.text = heroBattleInfo.Deaths.ToString();
            PlayerIcon.spriteName = heroBattleInfo.HeadIcon.ToString();
            PlayerPlains.text = heroBattleInfo.LastHit.ToString();
            Assist.text = heroBattleInfo.Assist.ToString();
            int i = 0;
            if (heroBattleInfo.GetItemsInfo() != null)
            {
                foreach (var item in PlayerGoodsDic)
                {
                    int id = heroBattleInfo.GetId(i++);
                    item.Value.spriteName = "";
                    if (id != 0 && ConfigReader.ItemXmlInfoDict.ContainsKey(id))
                    {
                        item.Value.spriteName = ConfigReader.ItemXmlInfoDict[id].sIcon;
                    }
                }
            }
            else
            {
                foreach (var item in PlayerGoodsDic)
                {
                    item.Value.spriteName = "";
                }
            }
        }
    }
}