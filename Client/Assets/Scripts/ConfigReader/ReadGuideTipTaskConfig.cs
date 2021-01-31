using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuideTipTaskConfig
{

    XmlDocument xmlDoc = null;

    public ReadGuideTipTaskConfig(string xmlFilePath)
    {
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("Title").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            GuideTitleInfo info = new GuideTitleInfo();
            info.TaskId = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "content":
                        info.mContent = Convert.ToString(xEle.InnerText);
                        break;
                    case "site":
                        info.mSite = Convert.ToString(xEle.InnerText);
                        break;
                    case "place":
                        info.LabelPos = GameMethod.ResolveToVector3(xEle.InnerText);
                        break;
                }
                #endregion
            }
            ConfigReader.guideTitleInfoXmlInfoDict.Add(info.TaskId, info);
        }
    }
}

public class GuideTitleInfo
{
    public int TaskId;
    public string mContent;
    public string mSite;
    public Vector3 LabelPos;
}

