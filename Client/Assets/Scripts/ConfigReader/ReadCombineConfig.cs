using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

//namespace BlGame.ConfigReader
//{
    class ReadCombineConfig
    {
        XmlDocument xmlDoc = null;
        public ReadCombineConfig(string xmlFilePath)
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
            XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("CombineCfg ").ChildNodes;
            for (int i = 0; i < infoNodeList.Count; i++)
            {
                XmlAttribute xmlAttr = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ItemID");
                if (xmlAttr == null)
                {
                    continue;
                }
                string itemId = xmlAttr.InnerText;
                CombineConfigInfo combineInfo = new CombineConfigInfo();
                //itemInfo.GoodsId = Convert.ToInt32(itemId);
                foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
                {
                    #region 搜索
                    switch (xEle.Name)
                    {
                        case "un32ChildID1":
                            combineInfo.un32ChildID1 = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "un32ChildID2":
                            combineInfo.un32ChildID2 = Convert.ToInt32(xEle.InnerText);
                            break;
                    }
                    #endregion
                }
                ConfigReader.combineXmlInfoDict.Add(Convert.ToInt32(itemId), combineInfo);
            }
        }
    }
    public class CombineConfigInfo : System.Object
    {
        public int un32ChildID1;
        public int un32ChildID2;
    }
//}
