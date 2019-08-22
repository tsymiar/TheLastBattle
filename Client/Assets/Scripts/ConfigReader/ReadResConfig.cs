using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

public class ReadResConfig
    
{       
    XmlDocument xmlDoc = null;
    public ReadResConfig(string xmlFilePath)        
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("RESCfg ").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("path") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("path").InnerText;
             
            ResConfigInfo resInfo = new ResConfigInfo();
            resInfo.resPath = typeName;
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    //case "szNOStr":
                    //    NpcSelectInfo.NpcName = Convert.ToString(xEle.InnerText);
                    //    break;
                    case "":
                        break;
                }
                #endregion
            }
            ConfigReader.resXmlInfoDic.Add(resInfo.resPath, resInfo);
        }
    }   
}
public class ResConfigInfo : System.Object
{
    public string resPath;
    public string md5;
    public string type;
    public int    size;
}

