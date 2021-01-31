using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuidePathTaskConfig
{

    XmlDocument xmlDoc = null;

    public ReadGuidePathTaskConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("path").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            GuidePathInfo info = new GuidePathInfo();
            //CPathTask pathInfo = new CPathTask();
            info.mTaskId = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "point":
                        info.mDesPos = GameMethod.ResolveToVector3(Convert.ToString(xEle.InnerText));
                        break;
                    case "dis":
                        info.mDistance = Convert.ToSingle(xEle.InnerText);
                        break;
                }
                #endregion
            }
            ConfigReader.guidePathInfoDict.Add(info.mTaskId, info);
        }
    }
}

public class GuidePathInfo
{
    public int mTaskId;
    public Vector3 mDesPos;
    public float mDistance;
}

