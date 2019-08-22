using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuideKillHeroTaskConfig
{

    XmlDocument xmlDoc = null;

    public ReadGuideKillHeroTaskConfig(string xmlFilePath)
    {
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("killhero").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            GuideKillHeroInfo info = new GuideKillHeroInfo();
            info.TaskId = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "deadnpcid":
                        info.mDeadnpcid = GameMethod.ResolveToIntList(xEle.InnerText);
                        break;
                    case "times":
                        info.mTimes = Convert.ToInt32(xEle.InnerText);
                        break;
                }
                #endregion
            }
            ConfigReader.guideKillHeroXmlInfoDict.Add(info.TaskId, info);
        }
    }
}

public class GuideKillHeroInfo
{
    public int TaskId;
    public List<int> mDeadnpcid;
    public int mTimes;
}

