using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

public class ReadLevelConfig
{

    XmlDocument xmlDoc = null;

    public ReadLevelConfig(string xmlFilePath)
    {

        //TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        if (!xmlfile)
        {
            //Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("WildMonsterCfg").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("UserLevel") == null) continue;

            string level =  (infoNodeList[i] as XmlElement).GetAttributeNode("UserLevel").InnerText;

            LevelConfigInfo levelInfo = new LevelConfigInfo();
            levelInfo.UserLevel = Convert.ToInt32(level);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {                    
                    case "LevelUpExp":
                        levelInfo.LevelUpExp = Convert.ToSingle(xEle.InnerText);
                        break;
                }

                #endregion
            }
            //Debug.LogError("levelInfo.UserLevel  " + levelInfo.UserLevel + " levelInfo " + levelInfo.LevelUpExp);
            ConfigReader.levelXmlInfoDict.Add(levelInfo.UserLevel, levelInfo);
        }
    }
}

public class LevelConfigInfo : System.Object
{
    public int UserLevel;
    public float LevelUpExp;
}
