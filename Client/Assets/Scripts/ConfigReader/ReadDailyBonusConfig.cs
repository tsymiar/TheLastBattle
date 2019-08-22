using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

public class DailyBonusConfigInfo : System.Object
{
    public string[] n32Type;
    public string[] n32ItemID;
    public string[] n32ItemNum;
}

public class ReadDailyBonusConfig
{
    XmlDocument xmlDoc = null;
    public ReadDailyBonusConfig(string xmlFilePath)
    {
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);

        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;
        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("DailyBonus ").ChildNodes;
        for (int i = 0; i < infoNodeList.Count; i++)
        {
            XmlAttribute xmlAttr = (infoNodeList[i] as XmlElement).GetAttributeNode("n32Days");

            if (xmlAttr == null)
            {
                continue;
            }

            DailyBonusConfigInfo dailBonusInfo = new DailyBonusConfigInfo();

            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                switch (xEle.Name)
                {
                    case "n32Type":
                        dailBonusInfo.n32Type = xEle.InnerText.Split(new char[] { ',' });
                        break;
                    case "n32ItemID":
                        dailBonusInfo.n32ItemID = xEle.InnerText.Split(new char[] { ',' });
                        break;
                    case "n32ItemNum":
                        dailBonusInfo.n32ItemNum = xEle.InnerText.Split(new char[] { ',' });
                        break;
                }
            }

            ConfigReader.dailyBonusXmlInfoDic.Add(Convert.ToUInt32(xmlAttr.InnerText), dailBonusInfo);
        }
    }
}

