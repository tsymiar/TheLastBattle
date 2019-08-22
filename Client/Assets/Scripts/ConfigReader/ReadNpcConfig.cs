using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

public class ReadNpcConfig
{

    XmlDocument xmlDoc = null;

    public ReadNpcConfig(string xmlFilePath)
    {

        //TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("NPCCfg ").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;

            NpcConfigInfo NpcSelectInfo = new NpcConfigInfo();
            NpcSelectInfo.NpcId = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "szNOStr":
                        NpcSelectInfo.NpcName = Convert.ToString(xEle.InnerText);
                        break;
                    case "eNpcType":
                        NpcSelectInfo.NpcType = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "eNpcChildType":
                        NpcSelectInfo.ENPCCateChild = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "XuetiaoHeight":
                        NpcSelectInfo.NpcXueTiaoHeight = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "eRace":
                        NpcSelectInfo.NpcRace = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "eAttendantCate":
                        NpcSelectInfo.NpcAttendantCate = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "eMagicCate":
                        NpcSelectInfo.NpcMagicCate = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "n32AttackDist":
                        NpcSelectInfo.NpcAtkDis = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "eAICate":
                        NpcSelectInfo.NpcAiCate = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "un32AITarID":
                        NpcSelectInfo.NpcAiTarId = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "n32PursueDist":
                        NpcSelectInfo.NpcPursueDis = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32GuardDist":
                        NpcSelectInfo.NpcGuardDis = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "eAttackMode":
                        NpcSelectInfo.NpcAtkMode = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "n32AttackPower":
                        NpcSelectInfo.NpcAtkPower = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32MagAttackPower":
                        NpcSelectInfo.NpcMagAtkPower = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32DefenseAbility":
                        NpcSelectInfo.NpcDef = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32MagDefenseAbility":
                        NpcSelectInfo.NpcMagDef = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32MoveSpeed":
                        NpcSelectInfo.NpcMoveSpeed = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32BaseMoveSpeedScaling":
                        NpcSelectInfo.n32BaseMoveSpeedScaling = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32AttackCD":
                        NpcSelectInfo.NpcAtkCd = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32MaxHP":
                        NpcSelectInfo.NpcMaxHp = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32MaxMP":
                        NpcSelectInfo.NpcMaxMp = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32GotExp":
                        NpcSelectInfo.NpcGotExp = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32ConsumeCP":
                        NpcSelectInfo.NpcConsumeCp = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32KillCP":
                        NpcSelectInfo.NpcKillCp = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "bIfCanControl":
                        NpcSelectInfo.NpcCanControl = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "n32HPRecover":
                        NpcSelectInfo.NpcHpRecover = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32MPRecover":
                        NpcSelectInfo.NpcMpRecover = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32CollideRadius":
                        NpcSelectInfo.NpcCollideRadius = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "un32WalkSound":
                        NpcSelectInfo.NpcWalkSound = Convert.ToString(xEle.InnerText);
                        break;
                    case "un32DeathSould":
                        NpcSelectInfo.NpcDeathSould = Convert.ToString(xEle.InnerText);
                        break;
                    case "un32SkillType1":
                        NpcSelectInfo.NpcSkillType1 = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "un32SkillType2":
                        NpcSelectInfo.NpcSkillType2 = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "un32SkillType3":
                        NpcSelectInfo.NpcSkillType3 = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "un32SkillType4":
                        NpcSelectInfo.NpcSkillType4 = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "un32SkillType5":
                        NpcSelectInfo.NpcSkillType5 = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "HeadPhoto":
                        NpcSelectInfo.HeadPhoto = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "eCamp":
                        NpcSelectInfo.NpcCamp = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "un32ShopID":
                        NpcSelectInfo.un32ShopID = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "BuildsDeathEffID":
                        NpcSelectInfo.NpcJianTaDeath = Convert.ToString(xEle.InnerText);
                        break;
                    case "n32LockRadius":
                        NpcSelectInfo.n32LockRadius = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "n32Script1Rate":
                        NpcSelectInfo.n32Script1Rate = Convert.ToString(xEle.InnerText);
                        break;
                    case "un32Script1":
                        NpcSelectInfo.un32Script1 = Convert.ToString(xEle.InnerText);
                        break;
                    case "n32AttackSpeed":
                        NpcSelectInfo.NpcAttackSpeed = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "n32IsLocked":
                        NpcSelectInfo.CanLock = (Convert.ToInt32(xEle.InnerText) == 1) ? true : false;
                        break;
                }

                #endregion
            }
            ConfigReader.npcXmlInfoDict.Add(NpcSelectInfo.NpcId, NpcSelectInfo);
        }
    }
}

public class NpcConfigInfo : System.Object
{
    #region NPC信息
    public string NpcName;
    public int NpcId;
    public int NpcType;
    public int ENPCCateChild;
    public float NpcXueTiaoHeight;
    public int NpcRace;
    public int NpcAttendantCate;
    public int NpcMagicCate;
    public float NpcAtkDis;
    public int NpcAiCate;
    public int NpcAiTarId;
    public float NpcPursueDis;
    public float NpcGuardDis;
    public int NpcAtkMode;
    public float NpcAtkPower;
    public float NpcMagAtkPower;
    public float NpcDef;
    public float NpcMagDef;
    public float NpcMoveSpeed;
    public float n32BaseMoveSpeedScaling;
    public float NpcAtkCd;
    public float NpcMaxHp;
    public float NpcMaxMp;
    public float NpcGotExp;
    public float NpcConsumeCp;
    public float NpcKillCp;
    public int NpcCanControl;
    public float NpcHpRecover;
    public float NpcMpRecover;
    public float NpcCollideRadius;
    public string NpcWalkSound;
    public string NpcDeathSould;
    public int NpcSkillType1;
    public int NpcSkillType2;
    public int NpcSkillType3;
    public int NpcSkillType4;
    public int NpcSkillType5;
    public int HeadPhoto;
    public int NpcCamp;
    public int un32ShopID;
    public string NpcJianTaDeath;
    public float n32LockRadius;
    public string n32Script1Rate;
    public string un32Script1;
    public int NpcAttackSpeed;
    public bool CanLock;
    #endregion

}
