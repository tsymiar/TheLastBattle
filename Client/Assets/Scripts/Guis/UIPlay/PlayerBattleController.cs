using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using BlGame.GameEntity;
using BlGame;
public class PlayerBattleController : MonoBehaviour {
    ButtonOnPress toggle;
    public static PlayerBattleController Instance
    {
        set;
        get;
    }
    ButtonOnPress CloseBtn;

    UIToggle RecordToggle;
    UIToggle AttributeToggle;
    UIToggle SystemSetToggle;
    enum BattleCurrInfo {
        BattleNull,
        BattleRecord,
        BattleAttribute,
        BattleSystemSet,
    }
    enum SystemSet
    {
        SoundButton,
        MusicButton,
        EffectButton,
        RangeButton,
        CloseButton,
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
    ButtonOnPress SoundBtn;
    ButtonOnPress MusicBtn;
    ButtonOnPress EffectBtn;
    ButtonOnPress RangeBtn;
    GameObject[] objOn = new GameObject[4];
    GameObject[] ojbOff = new GameObject[4];
    void Awake()
    {
        Instance = this;
        Init();
    }
    Transform tran = null;
    void Init()
    {
        toggle = transform.FindChild("InfoBtn").GetComponent<ButtonOnPress>();
        tran = transform.FindChild("Information");
        RecordToggle = tran.FindChild("BattleBtn").GetComponent<UIToggle>();
        AttributeToggle = tran.FindChild("StatusBtn").GetComponent<UIToggle>();
        SystemSetToggle = tran.FindChild("SettingBtn").GetComponent<UIToggle>();
        BattleState = BattleCurrInfo.BattleRecord;

        Transform temp = tran.FindChild("BattleInfo/Team/BlueTeam");
        int index = temp.childCount;
        for (int i = 0; i < index; i++)
        {
            AllBlueTeam.Add(new BattleRealInfo(temp.FindChild("Player" + (i + 1))));
        }
        temp = tran.FindChild("BattleInfo/Team/RedTeam");
        index = temp.childCount;
        for (int i = 0; i < index; i++)
        {
            AllRedTeam.Add(new BattleRealInfo(temp.FindChild("Player" + (i + 1))));
        }
        PlayerSpeed = tran.FindChild("StatusInfo/Speed/Num").GetComponent<UILabel>();
        AttackInterval = tran.FindChild("StatusInfo/AtkInterval/Num").GetComponent<UILabel>();
        AttackRange = tran.FindChild("StatusInfo/AtkRange/Num").GetComponent<UILabel>();
        ResurgenceTime = tran.FindChild("StatusInfo/ResuTime/Num").GetComponent<UILabel>();
        PhysicAttack = tran.FindChild("StatusInfo/PhysicAtk/Num").GetComponent<UILabel>();
        SpellsAttack = tran.FindChild("StatusInfo/SpellsAtk/Num").GetComponent<UILabel>();
        PhysicDef = tran.FindChild("StatusInfo/PhysicDef/Num").GetComponent<UILabel>();
        SpellsDef = tran.FindChild("StatusInfo/SpellsDef/Num").GetComponent<UILabel>();

        CloseBtn = tran.FindChild("CloseBtn").GetComponent<ButtonOnPress>();
        tran = tran.FindChild("SettingInfo");

        SoundBtn = tran.FindChild("SoundSwitch").GetComponent<ButtonOnPress>();
        MusicBtn = tran.FindChild("MusicSwitch").GetComponent<ButtonOnPress>();
        EffectBtn = tran.FindChild("EffectSwitch").GetComponent<ButtonOnPress>();
        RangeBtn = tran.FindChild("RangeSwitch").GetComponent<ButtonOnPress>();
        objOn[0] = SoundBtn.transform.FindChild("On").gameObject;
        ojbOff[0] = SoundBtn.transform.FindChild("Off").gameObject;
        objOn[1] = MusicBtn.transform.FindChild("On").gameObject;
        ojbOff[1] = MusicBtn.transform.FindChild("Off").gameObject;

        objOn[2] = EffectBtn.transform.FindChild("On").gameObject;
        ojbOff[2] = EffectBtn.transform.FindChild("Off").gameObject;
        objOn[3] = RangeBtn.transform.FindChild("On").gameObject;
        ojbOff[3] = RangeBtn.transform.FindChild("Off").gameObject;

        SoundBtn.AddListener((int)SystemSet.SoundButton,SoundButtonPress);
        MusicBtn.AddListener((int)SystemSet.MusicButton, SoundButtonPress);
        EffectBtn.AddListener((int)SystemSet.EffectButton, SoundButtonPress);
        RangeBtn.AddListener((int)SystemSet.RangeButton, SoundButtonPress);
        CloseBtn.AddListener((int)SystemSet.CloseButton, ClosePress);
        toggle.AddListener(OnPress);
    }

    private void ClosePress(int ie, bool pressed)
    {
        tran.parent.gameObject.SetActive(false);
    }

    private void OnPress(int ie, bool pressed)
    {
        tran.parent.gameObject.SetActive(true);
        ShowBattle();
    }    
    void OnEnable()
    {
        EventDelegate.Add(RecordToggle.onChange, ShowRecordAttrSet);
        EventDelegate.Add(AttributeToggle.onChange, ShowRecordAttrSet);
        EventDelegate.Add(SystemSetToggle.onChange, ShowRecordAttrSet);
        SetSave(ref SoundOpenState, UIGameSetting.soundKey,SystemSet.SoundButton);
        SetSave(ref MusicOpenState, UIGameSetting.voiceKey,SystemSet.MusicButton);
        SetSave(ref EffectOpenState, EffectVoc,SystemSet.EffectButton);
        SetSave(ref RangeOpenState, RangeVoc, SystemSet.RangeButton);
        EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerGoods, Record);
        EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerAssist, Record);
        EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerKills, Record);
        EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerDeaths, Record);
        EventCenter.AddListener(EGameEvent.eGameEvent_AllPlayerLevel, Record);
    }
    void ShowRecordAttrSet()
    {
        UIToggle currToggle = UIToggle.current;
        if (currToggle != null && currToggle.value)
        {
            if (currToggle == RecordToggle)
            {
                Record();
            }
            else if (currToggle == AttributeToggle)
            {
                BattleAttr();
            }else if (currToggle == SystemSetToggle)
            {
                
            }
        }
    }

    private const string EffectVoc = "EffectButton";
    private const string RangeVoc = "RangeButton";

    private bool SoundOpenState = true;
    private bool MusicOpenState = true;
    private bool EffectOpenState = true;
    private bool RangeOpenState = true;

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

    void Save(ref bool saveState, string key,SystemSet systemSet)
    {
        objOn[(int)systemSet].SetActive(saveState);
        ojbOff[(int)systemSet].SetActive(!saveState);
        //saveState = !saveState;
        int state = saveState ? 1 : 0;
        PlayerPrefs.SetInt(key, state);
        Debug.LogError("save " + key + " state = " + saveState);
        
        switch (systemSet)
        {
            case SystemSet.SoundButton:
                AudioManager.Instance.EnableSound(saveState);
                break;
            case SystemSet.MusicButton:
                AudioManager.Instance.EnableVoice(saveState);
                break;
            case SystemSet.EffectButton:
                
                break;
            case SystemSet.RangeButton:
                if (PlayerManager.Instance.LocalPlayer != null && saveState)
                    PlayerManager.Instance.LocalPlayer.initAreaCircle();
                else if (PlayerManager.Instance.LocalPlayer != null && !saveState)
                    PlayerManager.Instance.LocalPlayer.RemoveAreaCircle();
                break;
        }
        
    }

    private void SoundButtonPress(int ie, bool pressed)
    {
        switch ((SystemSet)ie)
        {
            case SystemSet.SoundButton:
                SoundOpenState = !SoundOpenState;
                Save(ref SoundOpenState, UIGameSetting.soundKey, (SystemSet)ie);
                break;
            case SystemSet.MusicButton:
                MusicOpenState = !MusicOpenState;
                Save(ref MusicOpenState, UIGameSetting.voiceKey, (SystemSet)ie);
                break;
            case SystemSet.EffectButton:
                EffectOpenState = !EffectOpenState;
                Save(ref EffectOpenState, EffectVoc, (SystemSet)ie);
                break;
            case SystemSet.RangeButton:
                RangeOpenState = !RangeOpenState;
                Save(ref RangeOpenState, RangeVoc, (SystemSet)ie);
                break;
        }
    }
    void Record()
    {
        int i = 0;
        foreach (var item in BattleingData.AllBlueHeroBattle)
        {
            AllBlueTeam[i++].ShowBattleInfo(item.Value);
        }
        i = 0;
        foreach (var item in BattleingData.AllRedHeroBattle)
        {
            AllRedTeam[i++].ShowBattleInfo(item.Value);
        }
    }
    void BattleAttr()
    {
        BattleState = BattleCurrInfo.BattleAttribute;
        PlayerSpeed.text = BattleingData.Instance.PlayerSpeed.ToString();
        AttackInterval.text = BattleingData.Instance.AttackInterval.ToString();
        AttackRange.text = BattleingData.Instance.AttackRange.ToString();
        ResurgenceTime.text = BattleingData.Instance.ResurgenceTime.ToString();
        PhysicAttack.text = BattleingData.Instance.PhysicAttack.ToString();
        SpellsAttack.text = BattleingData.Instance.SpellsAttack.ToString();
        PhysicDef.text = BattleingData.Instance.PhysicDef.ToString();
        SpellsDef.text = BattleingData.Instance.SpellsDef.ToString();
    }
    void ShowBattle()
    {
        if (BattleState == BattleCurrInfo.BattleRecord)
        {
            Record();
        }
        else if (BattleState == BattleCurrInfo.BattleAttribute)
        {
            BattleAttr();
        }
        else if (BattleState == BattleCurrInfo.BattleSystemSet)
        {
            BattleState = BattleCurrInfo.BattleSystemSet;
        }
    }

    void OnDisable()
    {
        EventDelegate.Remove(RecordToggle.onChange, ShowRecordAttrSet);
        EventDelegate.Remove(AttributeToggle.onChange, ShowRecordAttrSet);
        EventDelegate.Remove(SystemSetToggle.onChange, ShowRecordAttrSet);
        EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerGoods, Record);
        EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerAssist, Record);
        EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerKills, Record);
        EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerDeaths, Record);
        EventCenter.RemoveListener(EGameEvent.eGameEvent_AllPlayerLevel, Record);
    }
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
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

        public void ShowBattleInfo()
        {
 
        }
        
        public BattleRealInfo(Transform temp)
        {
            // TODO: Complete member initialization
            this.temp = temp;
            PlayerName= temp.FindChild("Name/Label").GetComponent<UILabel>();
            PlayerKills = temp.FindChild("Kill").GetComponent<UILabel>();
            PlayerLevel = temp.FindChild("HeadPhoto/Level").GetComponent<UILabel>();
            PlayerDeath = temp.FindChild("Death").GetComponent<UILabel>();
            PlayerPlains = temp.FindChild("Plains").GetComponent<UILabel>();
            PlayerIcon = temp.FindChild("HeadPhoto/Head").GetComponent<UISprite>();
            Assist = temp.FindChild("Assists").GetComponent<UILabel>();
            SetVib(false);
            Transform tran = temp.FindChild("Items");
            int index = tran.childCount;
            for (int i = 0; i < index; i++)
            {
                string str = "Item" + (i + 1) + "/icon";
                UISprite sp = tran.FindChild(str).GetComponent<UISprite>();
                sp.gameObject.SetActive(false);
                PlayerGoodsDic.Add((i + 1), sp);
            }
        }
        void SetVib(bool isVib)
        {
            PlayerName.gameObject.SetActive(isVib);
            PlayerKills.gameObject.SetActive(isVib);
            PlayerLevel.gameObject.SetActive(isVib);
            PlayerDeath.gameObject.SetActive(isVib);
            PlayerPlains.gameObject.SetActive(isVib);
            PlayerIcon.gameObject.SetActive(isVib);
            Assist.gameObject.SetActive(isVib);
        }
        internal void ShowBattleInfo(HeroBattleInfo heroBattleInfo)
        {
            SetVib(true);
            PlayerName.text = heroBattleInfo.HeroName;
            PlayerLevel.text = heroBattleInfo.Level.ToString();
            PlayerKills.text = heroBattleInfo.Kills.ToString();
            PlayerDeath.text = heroBattleInfo.Deaths.ToString();
            PlayerIcon.spriteName = heroBattleInfo.HeadIcon.ToString();
            PlayerPlains.text = heroBattleInfo.LastHit.ToString();
            Assist.text = heroBattleInfo.Assist.ToString();
            int i = 1;
            if (heroBattleInfo.GetItemsInfo() != null)
            {
                foreach (var item in PlayerGoodsDic)
                {
                    item.Value.gameObject.SetActive(true);
                    int id = heroBattleInfo.GetId( i++);
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
                    //item.Value.gameObject.SetActive(false);
                    item.Value.spriteName = "";
                }
            }
        }
    }
}
