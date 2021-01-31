using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using System.Linq;
using BlGame.Resource;
public class ReadGuideClickButtonTaskConfig
{

    XmlDocument xmlDoc = null;

    public ReadGuideClickButtonTaskConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("ClickButton").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            GuideButtonClickInfo info = new GuideButtonClickInfo();
            info.mTaskId = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索 
                switch (xEle.Name)
                {
                    case "path":
                        info.mPath = xEle.InnerText;
                        break;
                    case "Effects":
                        info.mEffects = xEle.InnerText;
                        break;
                }
                #endregion
            }
            ConfigReader.guideButtonClickXmlDict.Add(info.mTaskId, info);
        }
    }
}

public class GuideButtonClickInfo
{
    public int mTaskId;
    public string mPath;
    public string mEffects;
}

