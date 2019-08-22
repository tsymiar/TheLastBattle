using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using JT.FWW.Tools;
using BlGame.GameEntity;
using GameDefine;
using System.Linq;
using System;

public class BattleingData {

    private static BattleingData instance = null;
    public static BattleingData Instance
    {
        get {
            if (instance == null)
            {
                instance = new BattleingData();
            }
            return instance;
        }
    }

    public int PlayerSpeed;
    public int AttackInterval;
    public int AttackRange;
    public int ResurgenceTime;
    public int PhysicAttack;
    public int SpellsAttack;
    public int PhysicDef;
    public int SpellsDef;

    public void SetAttributes(int playerSpeed, int attackInterval, int attackRange, int resurgenceTime, int physicAttack, int spellsAttack, int physicDef,int spellsDef)
    {
        PlayerSpeed = playerSpeed;
        AttackInterval = attackInterval;
        AttackRange = attackRange;
        ResurgenceTime = resurgenceTime;
        PhysicAttack = physicAttack;
        SpellsAttack = spellsAttack;
        PhysicDef = physicDef;
        SpellsDef = spellsDef;
    }
    public static Dictionary<UInt64, HeroBattleInfo> AllRedHeroBattle = new Dictionary<UInt64, HeroBattleInfo>();
    public static Dictionary<UInt64, HeroBattleInfo> AllBlueHeroBattle = new Dictionary<UInt64, HeroBattleInfo>();

    public string GetGUIDName(UInt64 icon, EntityCampType camp)
    {
        if (EntityCampType.CampTypeA == (EntityCampType)camp)
        {
            foreach (var battle in AllBlueHeroBattle)
            {
                if (icon == battle.Key)
                    return battle.Value.HeroName;
            }
        }
        else
        {
            foreach (var battle in AllRedHeroBattle)
            {
                if (icon == battle.Key)
                    return battle.Value.HeroName;
            }
        }
        return null;
    }
    public void ClearAllGoods()
    {
        foreach (var item in AllRedHeroBattle)
        {
            item.Value.ClearGoods();
        }
        foreach (var item in AllBlueHeroBattle)
        {
            item.Value.ClearGoods();
        }
    }

    public void ClearAllBattleData()
    {
        AllBlueHeroBattle.Clear();
        AllRedHeroBattle.Clear();
    }

    public void AddPlayer(UInt64 sGUID, int temp , BattleDataType type,int index = 0, int goodsID = 0)
    {
        HeroBattleInfo HeroBattle = null;
        Dictionary<UInt64, HeroBattleInfo> heroBattleDic = GetCamp(sGUID);
        if (heroBattleDic == null)
            return;
        if (!heroBattleDic.TryGetValue(sGUID, out HeroBattle))
        {
            HeroBattle = new HeroBattleInfo();
            SetDic(sGUID, HeroBattle);
        }
        if (!EntityManager.AllEntitys.ContainsKey(sGUID))
        {
            return;
        }
        Ientity sEntity = EntityManager.AllEntitys[sGUID];
        switch (type)
        {
            case BattleDataType.Cp:
                HeroBattle.Cp = temp;
                break;
            case BattleDataType.LastHit:
                HeroBattle.LastHit = temp;
                break;
            case BattleDataType.HeadIcon:
                int id = (int)sEntity.ObjTypeID;
                HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo(id);
                HeroBattle.HeadIcon = info.HeroSelectHead;
                break;
            case BattleDataType.NickName:
                HeroBattle.HeroName = temp.ToString();
                break;
            case BattleDataType.Level:
                HeroBattle.Level = temp;
                break;
            case BattleDataType.Kills:
                HeroBattle.Kills = temp;
                break;
            case BattleDataType.Deaths:
                HeroBattle.Deaths = temp;
                break;
            case BattleDataType.Assist:
                HeroBattle.Assist = temp;
                break;
            case BattleDataType.Goods:
                //if (goodsID != 0)
                    HeroBattle.AddGoodItem(index, goodsID);
                //else
                //    HeroBattle.DelGoodsItem(index);
                break;
        }
        heroBattleDic[sGUID] = HeroBattle;
    }

    public void DelPlayer(Dictionary<UInt64, HeroBattleInfo> heroBattleDic, UInt64 sGUID)
    {
        if (heroBattleDic.ContainsKey(sGUID))
        {
            heroBattleDic[sGUID] = null;
            heroBattleDic.Remove(sGUID);
        }
    }
    public void AddInitPlayer(UInt64 sGUID, string name, int kills, int death, int Assist, int level, int lastHit, EntityCampType type, int heroid)    {
        HeroBattleInfo HeroBattle = null;
        Dictionary<UInt64, HeroBattleInfo> heroBattleDic = GetCamp(type);
        int id = heroid;
        HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo(id);
        if (info == null)
        {
            Debug.LogError("HeroSeletCfg not Find heroId");
            return;
        }
        if (!heroBattleDic.TryGetValue(sGUID, out HeroBattle))
        {
            HeroBattle = new HeroBattleInfo();
            HeroBattle.SGUID = sGUID;
            HeroBattle.HeroName = name;
            HeroBattle.Level = level;
            HeroBattle.Kills = kills;
            HeroBattle.Deaths = death;
            HeroBattle.Assist = Assist;
            HeroBattle.HeadIcon = info.HeroSelectHead;
            HeroBattle.LastHit = lastHit;
            HeroBattle.campType = type;
            SetDic(sGUID, HeroBattle);
            return;
        }
        HeroBattle.SGUID = sGUID;
        HeroBattle.HeroName = name;
        HeroBattle.Level = level;
        HeroBattle.Kills = kills;
        HeroBattle.Deaths = death;
        HeroBattle.Assist = Assist;
        HeroBattle.HeadIcon = info.HeroSelectHead;
        HeroBattle.LastHit = lastHit;
        HeroBattle.campType = type;
        SetDic(sGUID, HeroBattle);
    }

    Dictionary<UInt64, HeroBattleInfo> GetCamp(UInt64 sGUID)
    {
        if (AllBlueHeroBattle.ContainsKey(sGUID))
        {
            return AllBlueHeroBattle;
        }
        else if (AllRedHeroBattle.ContainsKey(sGUID))
        {
            return AllRedHeroBattle;
        }
        return null;
    }

    public Dictionary<UInt64, HeroBattleInfo> GetCamp(EntityCampType type)
    {
        if (type == EntityCampType.CampTypeA)
        {
            return AllBlueHeroBattle;
        }
        else if (type == EntityCampType.CampTypeB)
        {
            return AllRedHeroBattle;
        }
        return null;
    }
    void SetDic(UInt64 sGUID, HeroBattleInfo info)
    {
        if (info.campType == EntityCampType.CampTypeA)
        {
            AllBlueHeroBattle[sGUID] = info;
        }
        else if (info.campType == EntityCampType.CampTypeB)
        {
            AllRedHeroBattle[sGUID] = info;
        }
    }
    public void DelGood(UInt64 sGUID,int index)
    {
        Dictionary<UInt64, HeroBattleInfo> heroBattleDic = GetCamp(sGUID);
        if (heroBattleDic.ContainsKey(sGUID))
        {
            heroBattleDic[sGUID].DelGoodsItem(index);
        }
    }
}


public class HeroBattleInfo
{
    public UInt64 SGUID;
    public string HeroName;
    public int Level;
    public int Kills;
    public int Deaths;
    public int Assist;
    public int HeadIcon;
    public int LastHit;//
    public int Cp;
    public EntityCampType campType;
    public Dictionary<int, int> GoodsItemsInfo = new Dictionary<int, int>();

    public Dictionary<int, int> GetItemsInfo()
    {
        return GoodsItemsInfo;
    }
    public void AddGoodItem(int index, int id)
    {
        if (!GoodsItemsInfo.ContainsKey(index))
        {
            GoodsItemsInfo.Add(index, id);
            return;
        }
        GoodsItemsInfo[index] = id;
    }
    public void DelGoodsItem(int index)
    {
        if (GoodsItemsInfo.ContainsKey(index))
        {
            GoodsItemsInfo[index] = 0;
        }
    }
    public int GetId(int index)
    {
        for (int i = 0; i < GoodsItemsInfo.Count; i++)
        {
            int id = GoodsItemsInfo.ElementAt(i).Value;
            int temp = GoodsItemsInfo.ElementAt(i).Key;
            if ((index) == temp)
            {
                index = id;
                return index;
            }
        }
        return 0;
    }
    public void ClearGoods()
    {
        GoodsItemsInfo.Clear();
    }
}