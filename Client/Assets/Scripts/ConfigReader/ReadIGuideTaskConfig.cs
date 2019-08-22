using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using System.Linq;
using BlGame.Resource;

public class ReadIGuideTaskConfig
{

    XmlDocument xmlDoc = null;

    public ReadIGuideTaskConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("UiTaskEvent").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            IGuideData taskInfo = new IGuideData();
            taskInfo.TaskId = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "startEvent":
                        taskInfo.StartTaskEvent = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "endEvent":
                        taskInfo.EndTaskEvent = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "textpath":
                        taskInfo.TextPath = Convert.ToString(xEle.InnerText);
                        break;
                    case "textpos":
                        taskInfo.TextPos = GameMethod.ResolveToVector3(Convert.ToString(xEle.InnerText),';');
                        break;
                    case "content":
                        taskInfo.TextContent = GameMethod.ResolveToStrList(Convert.ToString(xEle.InnerText),';');
                        break;
                    case "shakepath":
                        taskInfo.ShakePath = Convert.ToString(xEle.InnerText);
                        break;
                    case "shakepos":
                        taskInfo.ShakePos = GameMethod.ResolveToVector3(Convert.ToString(xEle.InnerText), ';');
                        break;
                    case "flashpath":
                        taskInfo.FlashPath = Convert.ToString(xEle.InnerText);
                        break;
                    case "flashpos":
                        taskInfo.FlashPos = GameMethod.ResolveToVector3(Convert.ToString(xEle.InnerText), ';'); 
                        break;
                    case "destpos":
                        taskInfo.FlashDstPos = GameMethod.ResolveToVector3(Convert.ToString(xEle.InnerText), ';');
                        break;
                    case "isshield":
                        taskInfo.IsMask = (Convert.ToInt32(xEle.InnerText) == 1) ? true : false;
                        break;
                }
                #endregion
            }
            ConfigReader.iGuideDatalXmlInfoDict.Add(taskInfo.TaskId, taskInfo);
        }
    }
   
}

public class IGuideData
{
    public int TaskId;
    public int StartTaskEvent;
    public int EndTaskEvent;
    public string TextPath;
    public Vector3 TextPos;
    public List<string> TextContent = new List<string>();
    public string ShakePath;
    public Vector3 ShakePos;
    public string FlashPath;
    public Vector3 FlashPos;
    public Vector3 FlashDstPos;
    public bool IsMask;

}
 
