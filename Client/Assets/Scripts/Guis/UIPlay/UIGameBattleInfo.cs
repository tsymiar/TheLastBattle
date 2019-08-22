using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using JT.FWW.Tools;
using BlGame.GameEntity;
using BlGame.GameData;
using System.Linq;
using System;

public class UIGameBattleInfo : MonoBehaviour {

	public static UIGameBattleInfo Instance {
		set;
		get;
	}
	List<BattleInfo> AllBlueTeam = new List<BattleInfo>();//date
	List<BattleInfo> AllRedTeam = new List<BattleInfo>();//date
	List<UIBattleInfo> BattleInfoList = new List<UIBattleInfo>();
	public Transform ObjTran {
				set;
				get;
	}
	ButtonOnPress BtnOnPress;
	void Awake()
	{
		Instance = this;
		ObjTran = transform.FindChild("temp");
		Transform blue = transform.FindChild ("temp/Team/BlueTeam");
		Transform red = transform.FindChild("temp/Team/RedTeam");
		BtnOnPress = transform.FindChild("BottomBar/Arrow").GetComponent<ButtonOnPress>();
		for (int i = 0; i < blue.childCount; i++) {
			Transform temp = blue.FindChild("Player" + (i+1));
			BattleInfoList.Add(new UIBattleInfo(temp));
		}
		for (int i = 0; i < blue.childCount; i++) {
			Transform temp = red.FindChild("Player" + (i+1));
			BattleInfoList.Add(new UIBattleInfo(temp));
		}
        ObjTran.gameObject.SetActive(false);
	}

	int indexRed = 3;
	public void SetDeadInfo(UInt64 SGUID ,int dead)
	{
                for (int i = 0; i < AllBlueTeam.Count; i++)
                {
                    if (AllBlueTeam[i].PlayerIcon == SGUID)
                    {
                        AllBlueTeam[i].PlayerDeath = dead;
                    }
                    SetLevelDeadKill(AllBlueTeam[i], i);
                }
                indexRed = 3;
                for (int i = 0; i < AllRedTeam.Count; i++)
                {
                    if (AllRedTeam[i].PlayerIcon == SGUID)
                    {
                        AllRedTeam[i].PlayerDeath = dead;
                    }
                    SetLevelDeadKill(AllRedTeam[i], indexRed);
                    indexRed++;
                }
	}
	void OnEnable() {
        BtnOnPress.collider.enabled = false;
	}
	public void SetKillInfo(UInt64 SGUID ,int kill)
	{
                for (int i = 0; i < AllBlueTeam.Count; i++)
                {
                    if (AllBlueTeam[i].PlayerIcon == SGUID)
                    {
                        AllBlueTeam[i].PlayerKills = kill;
                    }
                    SetLevelDeadKill(AllBlueTeam[i], i);
                }
                indexRed = 3;
                for (int i = 0; i < AllRedTeam.Count; i++)
                {
                    if (AllRedTeam[i].PlayerIcon == SGUID)
                    {
                        AllRedTeam[i].PlayerKills = kill;
                    }
                    SetLevelDeadKill(AllRedTeam[i], indexRed);
                    indexRed++;
                }
	}
	void SetLevelDeadKill(BattleInfo item,int i)
	{
		BattleInfoList [i].PlayerLevelLabel.text = item.PlayerLevel.ToString ();
		BattleInfoList [i].PlayerNameLabel.text = item.PlayerName;

        if (!EntityManager.AllEntitys.ContainsKey(item.PlayerIcon))
        {
            return;
        }
        Ientity sEntity = EntityManager.AllEntitys[item.PlayerIcon];

		int id = (int)sEntity.ObjTypeID;
		BattleInfoList [i].PlayerDeathLabel.text = item.PlayerDeath.ToString ();
		HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo (id);
		BattleInfoList [i].PlayerKillsLabel.text = item.PlayerKills.ToString ();
		if(info != null)
			BattleInfoList [i].PlayerIcon.spriteName = info.HeroSelectHead.ToString ();
	}
	public void SetLevelInfo(UInt64 SGUID ,int level)
	{
		for (int i = 0; i < AllBlueTeam.Count; i++)
		{
		    if (AllBlueTeam[i].PlayerIcon == SGUID)
		    {
		        AllBlueTeam[i].PlayerLevel = level;
		    }
		    SetLevelDeadKill(AllBlueTeam[i], i);
		}
		indexRed = 3;
		for (int i = 0; i < AllRedTeam.Count; i++)
		{
		    if (AllRedTeam[i].PlayerIcon == SGUID)
		    {
		        AllRedTeam[i].PlayerLevel = level;
		    }
		    SetLevelDeadKill(AllRedTeam[i], indexRed);
		    indexRed++;
		}
	}

	public void SetInfoInit(UInt64 icon, string name, int level, int kills, int death, EntityCampType camp)
	{
        if (EntityCampType.CampTypeA == (EntityCampType)camp && GetBlueGuid(icon))
        {
			BattleInfo battle = new BattleInfo (icon, name, level, kills, death, EntityCampType.CampTypeA);
			AllBlueTeam.Add (battle);
        }
        else if (EntityCampType.CampTypeB == (EntityCampType)camp && GetRedGuid(icon))
        {
			BattleInfo battle = new BattleInfo (icon, name, level, kills, death, EntityCampType.CampTypeB);
			AllRedTeam.Add (battle);
		}
        SetLevelInfo(icon, level);
	}
    public bool GetBlueGuid(UInt64 icon)
    {
        foreach (BattleInfo battle in AllBlueTeam)
        {
            if (icon != battle.PlayerIcon)
                continue;
            else
                return false;
        }       
        return true;
    }
    public bool GetRedGuid(UInt64 icon)
    {
        foreach (BattleInfo battle in AllRedTeam)
        {
            if (icon != battle.PlayerIcon)
                continue;
            else
                return false;
        }
        return true;
    }

    public string GetGUIDName(UInt64 icon,EntityCampType camp)
    {
        if (EntityCampType.CampTypeA == (EntityCampType)camp)
        {
            foreach (BattleInfo battle in AllBlueTeam)
            {
                if (icon == battle.PlayerIcon)
                    return battle.PlayerName;
            }
        }
        else 
        {
            foreach (BattleInfo battle in AllRedTeam)
            {
                if (icon == battle.PlayerIcon)
                    return battle.PlayerName;
            }
        }
        return null;
    }

	void Start () {
		
	}
	// Update is called once per frame
	void Update () {
	
	}

	public class BattleInfo
	{
		public string PlayerName;
		public int PlayerLevel;
		public int PlayerKills;
		public int PlayerDeath;
        public EntityCampType PlayerCamp;
		public UInt64 PlayerIcon;
        public BattleInfo(UInt64 icon, string name, int level, int kills, int death, EntityCampType camp)
		{
			PlayerIcon = icon;
			PlayerName = name;
			PlayerLevel = level;
			PlayerKills = kills;
			PlayerDeath = death;
			PlayerCamp = camp;
		}
	}
	public class UIBattleInfo
	{
		public UILabel PlayerNameLabel;
		public UILabel PlayerLevelLabel;
		public UILabel PlayerKillsLabel;
		public UILabel PlayerDeathLabel;
		public UISprite PlayerIcon;
		public UIBattleInfo(Transform tran)
		{
			PlayerNameLabel = tran.FindChild("Name/Label").GetComponent<UILabel>();
			PlayerLevelLabel = tran.FindChild("Level").GetComponent<UILabel>();
			PlayerKillsLabel = tran.FindChild("Kill").GetComponent<UILabel>();
			PlayerDeathLabel = tran.FindChild("Death").GetComponent<UILabel>();
			PlayerIcon = tran.FindChild("HeadPhoto/Head").GetComponent<UISprite>();
			PlayerIcon.spriteName = "";
			PlayerNameLabel.text = "";
			PlayerLevelLabel.text = "";
			PlayerKillsLabel.text = "";
			PlayerDeathLabel.text = "";
		}
	}
}
