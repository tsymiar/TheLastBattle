using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

public class OtherItemConfigInfo : System.Object
{
    public string sName;
    public uint effectid;
    public uint effectvalue;
    public uint consumetype;
    public uint priceseries;
    public bool isShowInShop;
    public int time;
    public string icon;
}

public class ReadOtherItemConfig
{
    XmlDocument xmlDoc = null;
    public ReadOtherItemConfig(string xmlFilePath)
    {
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);

        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;
        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("OtherItemCfg ").ChildNodes;
        for (int i = 0; i < infoNodeList.Count; i++)
        {
            XmlAttribute xmlAttr = (infoNodeList[i] as XmlElement).GetAttributeNode("ID");

            if (xmlAttr == null)
            {
                continue;
            }

            OtherItemConfigInfo otherItemInfo = new OtherItemConfigInfo();

            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                switch (xEle.Name)
                {
                    case "Name":
                        otherItemInfo.sName = xEle.InnerText;
                        break;
                    case "EffectID":
                        otherItemInfo.effectid = Convert.ToUInt32(xEle.InnerText);
                        break;
                    case "EffectValue":
                        otherItemInfo.effectvalue = Convert.ToUInt32(xEle.InnerText);
                        break;
                    case "ConsumeType":
                        otherItemInfo.consumetype = Convert.ToUInt32(xEle.InnerText);
                        break;
                    case "PriceSeries":
                        otherItemInfo.priceseries = Convert.ToUInt32(xEle.InnerText);
                        break;
                    case "bIsShowInShop":
                        otherItemInfo.isShowInShop = Convert.ToUInt32(xEle.InnerText)==1?true:false;
                        break;
                    case "Time":
                        otherItemInfo.time = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "Icon":
                        otherItemInfo.icon = xEle.InnerText;
                        break;
                }
            }

            ConfigReader.otherItemXmlInfoDic.Add(Convert.ToUInt32(xmlAttr.InnerText), otherItemInfo);
        }
    }
}

