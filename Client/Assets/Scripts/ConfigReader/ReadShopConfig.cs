using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

//namespace BlGame.ConfigReader
//{
    class ReadShopConfig
    {
        XmlDocument xmlDoc = null;
        public ReadShopConfig(string xmlFilePath)
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
            XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("ShopCfg ").ChildNodes;
            for (int i = 0; i < infoNodeList.Count; i++)
            {
                XmlAttribute xmlAttr = (infoNodeList[i] as XmlElement).GetAttributeNode("n32ShopID");
                if (xmlAttr == null)
                {
                    continue;
                }
                string shopId = xmlAttr.InnerText;
                ShopConfigInfo shopInfo = new ShopConfigInfo();
                //shopInfo.un32ItemIDPage1 = Convert.ToInt32(itemId);
                foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
                {
                    #region 搜索
                    switch (xEle.Name)
                    {
                        case "un32ItemIDPage1":
                            shopInfo.un32ItemIDPage1 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "un8CostTypePage1":
                            shopInfo.un8CostTypePage1 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "n32CPCostPage1":
                            shopInfo.n32CPCostPage1 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "un32ItemIDPage2":
                            shopInfo.un32ItemIDPage2 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "un8CostTypePage2":
                            shopInfo.un8CostTypePage2 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "n32CPCostPage2":
                            shopInfo.n32CPCostPage2 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "un32ItemIDPage3":
                            shopInfo.un32ItemIDPage3 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "un8CostTypePage3":
                            shopInfo.un8CostTypePage3 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "n32CPCostPage3":
                            shopInfo.n32CPCostPage3 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "un32ItemIDPage4":
                            shopInfo.un32ItemIDPage4 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "un8CostTypePage4":
                            shopInfo.un8CostTypePage4 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "n32CPCostPage4":
                            shopInfo.n32CPCostPage4 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "un32ItemIDPage5":
                            shopInfo.un32ItemIDPage5 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "un8CostTypePage5":
                            shopInfo.un8CostTypePage5 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                        case "n32CPCostPage5":
                            shopInfo.n32CPCostPage5 = GameDefine.GameMethod.ResolveToIntList(xEle.InnerText);
                            break;
                    }

                    #endregion
                }
                ConfigReader.shopXmlInfoDict.Add(Convert.ToInt32(shopId), shopInfo);
            }
        }
    }
    public class ShopConfigInfo : System.Object
    {
        public List<int> un32ItemIDPage1;
        public List<int> un8CostTypePage1;
        public List<int> n32CPCostPage1;
        public List<int> un32ItemIDPage2;
        public List<int> un8CostTypePage2;
        public List<int> n32CPCostPage2;
        public List<int> un32ItemIDPage3;
        public List<int> un8CostTypePage3;
        public List<int> n32CPCostPage3;
        public List<int> un32ItemIDPage4;
        public List<int> un8CostTypePage4;
        public List<int> n32CPCostPage4;
        public List<int> un32ItemIDPage5;
        public List<int> un8CostTypePage5;
        public List<int> n32CPCostPage5;
    }
//}
