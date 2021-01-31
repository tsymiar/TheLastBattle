using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using JT.FWW.Tools;
using BlGame.GameEntity;
using System;


public class UIViewerBattleInfo : MonoBehaviour {
    public static UIViewerBattleInfo Instance
    {
        set;
        get;
    }
    List<BattleInfo> AllBlueTeam = new List<BattleInfo>();//date
    List<UIBattleInfo> BattleInfoList = new List<UIBattleInfo>();
    public Transform ObjTran
    {
        set;
        get;
    }
    public ButtonOnPress BtnOnPress
    {
        set;
        get;
    }
    void Awake()
    {
        Instance = this;
        ObjTran = transform.FindChild("temp");
        Transform blue = transform.FindChild("temp/Team/BlueTeam");
        BtnOnPress = transform.FindChild("Arrow").GetComponent<ButtonOnPress>();
        for (int i = 0; i < blue.childCount; i++)
        {
            Transform temp = blue.FindChild("Player" + (i + 1));
            BattleInfoList.Add(new UIBattleInfo(temp));
        }
    }

    int indexRed = 3;
    public void SetDeadInfo(UInt64 SGUID, int dead)
    {
        for (int i = 0; i < AllBlueTeam.Count; i++)
        {
            if (AllBlueTeam[i].PlayerIcon == SGUID)
            {
                AllBlueTeam[i].PlayerDeath = dead;
            }
            SetLevelDeadKill(AllBlueTeam[i],AllBlueTeam[i].CampID - 1);
        }
        UIViewerGameRecord.Instance.SetShowAllKills(BlueAllKillsEnemy(1).ToString(), BlueAllKillsEnemy(0).ToString());
    }
    public int BlueAllKillsEnemy(int i)
    {
        int allKills = 0;
        int kills = 0;
        foreach (var item in AllBlueTeam)
        {
            if (item != null && (item.CampID -1) % 2 == 0)
            {
                allKills += Convert.ToInt32(item.PlayerKills);
            }else if((item.CampID -1) % 2 == 1)
                kills += Convert.ToInt32(item.PlayerKills);
        }
        if (i == 0)
            return allKills;
        return kills;
    }

    void OnEnable()
    {
        BtnOnPress.AddListener(0, OnPress);
    }
    UITweener uit;
    void OnPress(int i, bool Press)
    {
        if (Press)
            return;
        //UIGameRecord.Instance.GearRotation(360, -360);
        UIViewerGameRecord.Instance.BtnOnPress.collider.enabled = true;
        BtnOnPress.collider.enabled = false;
        uit = TweenPosition.Begin(this.gameObject, 0.7f, new Vector3(-41, 380, 0));
        uit.method = UITweener.Method.Linear;
        EventDelegate.Add(uit.onFinished, Finished, true);
    }
    void Finished()
    {
        EventDelegate.Remove(uit.onFinished, Finished);
        ObjTran.gameObject.SetActive(true);
        //UIGameRecord.Instance.SetGearState();
    }

    public void SetKillInfo(UInt64 SGUID, int kill)
    {
        for (int i = 0; i < AllBlueTeam.Count; i++)
        {
            if (AllBlueTeam[i].PlayerIcon == SGUID)
            {
                AllBlueTeam[i].PlayerKills = kill;
            }
            SetLevelDeadKill(AllBlueTeam[i],AllBlueTeam[i].CampID - 1);
        }
    }
    void SetLevelDeadKill(BattleInfo item, int i)// 3 4
    {
        BattleInfoList[i].PlayerLevelLabel.text = item.PlayerLevel.ToString();
        BattleInfoList[i].PlayerNameLabel.text = item.PlayerName;
        if (!EntityManager.AllEntitys.ContainsKey(item.PlayerIcon))
        {
            return;
        }
        Ientity sEntity = EntityManager.AllEntitys[item.PlayerIcon];
        int id = (int)sEntity.ObjTypeID;
        BattleInfoList[i].PlayerDeathLabel.text = item.PlayerDeath.ToString();
        HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo(id);
        BattleInfoList[i].PlayerKillsLabel.text = item.PlayerKills.ToString();
        BattleInfoList[i].PlayerCp.text = item.PlayerCP.ToString();
        BattleInfoList[i].PlayerFarm.text = item.PlayerFarm.ToString();
        if (info != null)
            BattleInfoList[i].PlayerIcon.spriteName = info.HeroSelectHead.ToString();
        //		BattleInfoList [i].PlayerIcon.spriteName = ConfigReader.GetHeroSelectInfo (()).HeroSelectHead.ToString ();
    }
    /************************************************************************/
    /*设置Level                                                                     */
    /************************************************************************/
    public void SetLevelInfo(UInt64 SGUID, int level)
    {
        for (int i = 0; i < AllBlueTeam.Count; i++)
        {
            if (AllBlueTeam[i].PlayerIcon == SGUID)
            {
                AllBlueTeam[i].PlayerLevel = level;
            }
            SetLevelDeadKill(AllBlueTeam[i],AllBlueTeam[i].CampID - 1);
        }
    }
    /************************************************************************/
    /* 设置CP                                                                     */
    /************************************************************************/
    public void SetCpInfo(UInt64 SGUID, int cp)
    {
        for (int i = 0; i < AllBlueTeam.Count; i++)
        {
            if (AllBlueTeam[i].PlayerIcon == SGUID)
            {
                AllBlueTeam[i].PlayerCP = cp;
            }
            SetLevelDeadKill(AllBlueTeam[i],AllBlueTeam[i].CampID - 1);
        }
    }
    /************************************************************************/
    /*   设置FARM                                                                   */
    /************************************************************************/
    public void SetFarmInfo(UInt64 SGUID, int farm)
    {
        for (int i = 0; i < AllBlueTeam.Count; i++)
        {
            if (AllBlueTeam[i].PlayerIcon == SGUID)
            {
                AllBlueTeam[i].PlayerFarm = farm;
            }
            SetLevelDeadKill(AllBlueTeam[i],AllBlueTeam[i].CampID - 1);
            
        }
    }
    public void SetInfoInit(UInt64 icon, string name, int level, int kills, int death, EntityCampType camp,int campID)
    {
        if (GetBlueGuid(icon))
        {
            BattleInfo battle = new BattleInfo(icon, name, level, kills, death, EntityCampType.CampTypeA, campID);
            AllBlueTeam.Add(battle);
            SetLevelInfo(icon, level);
        }
        
    }
    public bool GetBlueGuid(UInt64 icon)
    {
        if (icon == null)
            return false;
        foreach (BattleInfo battle in AllBlueTeam)
        {
            if (icon != battle.PlayerIcon)
                continue;
            else
                return false;
        }
        return true;
    }

    public string GetGUIDName(UInt64 icon, EntityCampType camp)
    {
        foreach (BattleInfo battle in AllBlueTeam)
        {
            if (icon == battle.PlayerIcon)
                return battle.PlayerName;
        }
        return null;
    }

    void Start()
    {

    }
    // Update is called once per frame
    void Update()
    {

    }

    public class BattleInfo
    {
        public string PlayerName;
        public int PlayerLevel;
        public int PlayerKills;
        public int PlayerDeath;
        public int PlayerCP;
        public int PlayerFarm;
        public EntityCampType PlayerCamp;
        public UInt64 PlayerIcon;
        public int CampID;
        public BattleInfo(UInt64 icon, string name, int level, int kills, int death,EntityCampType camp,int campID)
        {
            PlayerIcon = icon;
            PlayerName = name;
            PlayerLevel = level;
            PlayerKills = kills;
            PlayerDeath = death;
            PlayerCamp = camp;
            CampID = campID;
        }
    }
    public class UIBattleInfo
    {
        public UILabel PlayerNameLabel;
        public UILabel PlayerLevelLabel;
        public UILabel PlayerKillsLabel;
        public UILabel PlayerDeathLabel;
        public UISprite PlayerIcon;
        public UILabel PlayerCp;
        public UILabel PlayerFarm;
        public UIBattleInfo(Transform tran)
        {
            PlayerNameLabel = tran.FindChild("Name/Label").GetComponent<UILabel>();
            PlayerLevelLabel = tran.FindChild("Level").GetComponent<UILabel>();
            PlayerKillsLabel = tran.FindChild("Kill").GetComponent<UILabel>();
            PlayerDeathLabel = tran.FindChild("Death").GetComponent<UILabel>();
            PlayerCp = tran.FindChild("CP").GetComponent<UILabel>();
            PlayerFarm = tran.FindChild("Fram").GetComponent<UILabel>();
            PlayerIcon = tran.FindChild("HeadPhoto/Head").GetComponent<UISprite>();
            PlayerIcon.spriteName = "";
            PlayerNameLabel.text = "";
            PlayerLevelLabel.text = "";
            PlayerKillsLabel.text = "";
            PlayerDeathLabel.text = "";
            PlayerCp.text = "";
            PlayerFarm.text = "";
        }
    }
}
