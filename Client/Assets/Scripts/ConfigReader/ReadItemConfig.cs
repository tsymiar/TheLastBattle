using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

//namespace BlGame.ConfigReader
//{
    class ReadItemConfig
    {
        XmlDocument xmlDoc = null;
        public ReadItemConfig(string xmlFilePath)
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
            XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("ItemCfg ").ChildNodes;
            for (int i = 0; i < infoNodeList.Count; i++)
            {
                XmlAttribute xmlAttr = (infoNodeList[i] as XmlElement).GetAttributeNode("un32GoodsID");
                if (xmlAttr == null)
                {
                    continue;
                }
                string itemId = xmlAttr.InnerText;
                ItemConfigInfo itemInfo = new ItemConfigInfo();
                itemInfo.GoodsId = Convert.ToInt32(itemId);
                foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
                {
                    #region 搜索
                    switch (xEle.Name)
                    {
                        case "sNameID":
                            itemInfo.sNameID = xEle.InnerText;
                            break;
                        case "sNameCh":
                            itemInfo.sNameCh = xEle.InnerText;
                            break;
                        case "un8TypeID":
                            itemInfo.un8TypeID = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "attributeKeyList":
                            itemInfo.attributeKeyList = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "attributeValList":
                            itemInfo.attributeValList = GameDefine.GameMethod.ResolveToFloatList(xEle.InnerText);
                            break;
                        case "un8UseTimes":
                            itemInfo.un8UseTimes = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "bDestroyAfterUsed":
                            itemInfo.bDestroyAfterUsed = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "un32SkillID":
                            itemInfo.un32SkillID = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "bUsedAfterBuyed":
                            itemInfo.bUsedAfterBuyed = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "sIcon":
                            itemInfo.sIcon = xEle.InnerText;
                            break;
                        case "un32CdTime":
                            itemInfo.un32CdTime = Convert.ToSingle (xEle.InnerText);
                            break;
                        case "bUniqueID":
                            itemInfo.bUniqueID = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "un8OverlapTimes":
                            itemInfo.un8OverlapTimes = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "un8CPCostType":
                            itemInfo.un8CPCostType = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "n32CPCost":
                            itemInfo.n32CPCost = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "un8FunctionType":
                            itemInfo.un8FunctionType = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "un32UniqueTypeID":
                            itemInfo.un32UniqueTypeID = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "sDescribe":
                            itemInfo.sDescribe = xEle.InnerText;
                            break;
                        case "un32UseSoundID":
                            itemInfo.un32UseSoundID = xEle.InnerText;
                            break;


                    }
                    #endregion
                }
                ConfigReader.itemXmlInfoDict.Add(itemInfo.GoodsId, itemInfo);
            }
        }
    }
    public class ItemConfigInfo : System.Object
    {
        public int GoodsId;
        public string sNameID;
        public string sNameCh;
        public int un8TypeID;
        public List<int> attributeKeyList;
        public List<float> attributeValList;
        public int un8UseTimes;
        public int bDestroyAfterUsed;
        public int un32SkillID;
        public int bUsedAfterBuyed;
        public string sIcon;
        public int bUniqueID;
        public int un8OverlapTimes;
        public int un8CPCostType;
        public int n32CPCost;
        public int un8FunctionType;
        public int un32UniqueTypeID;
        public string sDescribe;
        public string un32UseSoundID;
        public float un32CdTime;
    }
//}
