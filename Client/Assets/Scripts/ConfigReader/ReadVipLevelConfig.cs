using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using GameDefine;
using BlGame.Resource;
using System.Linq;

public class ReadVipLevelConfig  {
    XmlDocument xmlDoc = null;
    public ReadVipLevelConfig(string xmlFilePath)
    {
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("VIPIntegralCfg").ChildNodes;
        string temp = null; string[] value = null;
        for (int i = 0; i < infoNodeList.Count; i++)//XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("VIPLv") == null) continue;

            string level = (infoNodeList[i] as XmlElement).GetAttributeNode("VIPLv").InnerText;

            VipLevelConfigInfo levelInfo = new VipLevelConfigInfo();
            levelInfo.VipUserLevel = Convert.ToInt32(level);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "VIPIntegral":
                        levelInfo.VipIntegral = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "M20001":
                        levelInfo.VipHero = Convert.ToString(xEle.InnerText);
                        break;
                    case "M20002":
                        levelInfo.VipRune = Convert.ToString(xEle.InnerText);
                        break;
                    case "M21001":
                        levelInfo.VipPrivilege1 = Convert.ToString(xEle.InnerText);
                        break;
                    case "M21002":
                        levelInfo.VipPrivilege2 = Convert.ToString(xEle.InnerText);
                        break;
                    case "M21003":
                        levelInfo.VipPrivilege3 = Convert.ToString(xEle.InnerText);
                        break;
                    case "M21004":
                        levelInfo.VipPrivilege4 = Convert.ToString(xEle.InnerText);
                        break;
                    case "M21005":
                        levelInfo.VipPrivilege5 = Convert.ToString(xEle.InnerText);
                        break;
                    case "M21006":
                        levelInfo.VipPrivilege6 = Convert.ToString(xEle.InnerText);
                        break;
                    case "M21007":
                        levelInfo.VipPrivilege7 = Convert.ToString(xEle.InnerText);
                        break;
                    case "M21008":
                        levelInfo.VipPrivilege8 = Convert.ToString(xEle.InnerText);
                        break;
                    case "M21009":
                        levelInfo.VipPrivilege9 = Convert.ToString(xEle.InnerText);
                        break;
                    case "M21010":
                        levelInfo.VipPrivilege10 = Convert.ToString(xEle.InnerText);
                        break;
                }

                #endregion
            }
            //Debug.LogError("levelInfo.UserLevel  " + levelInfo.UserLevel + " levelInfo " + levelInfo.LevelUpExp);
            ConfigReader.ViplevelXmlInfoDict.Add(levelInfo.VipUserLevel, levelInfo);
        }
    }
}
public class VipLevelConfigInfo : System.Object
{
    public int VipUserLevel;
    public int VipIntegral;
    public string VipHero;
    public string VipRune;

    public string VipPrivilege1;
    public string VipPrivilege2;
    public string VipPrivilege3;
    public string VipPrivilege4;
    public string VipPrivilege5;
    public string VipPrivilege6;
    public string VipPrivilege7;
    public string VipPrivilege8;
    public string VipPrivilege9;
    public string VipPrivilege10;
}
