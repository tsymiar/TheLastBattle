using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using System.Linq;
using BlGame.Resource;

public class ReadIGuideTaskManagerConfig
{

    XmlDocument xmlDoc = null;

    public ReadIGuideTaskManagerConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("UiTaskmanager").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            IGuideManagerData managerInfo = new IGuideManagerData();
            managerInfo.TaskId = Convert.ToInt32(typeName);
            managerInfo.IsTriggerTask = false;
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "sontaskid":
                        managerInfo.SonTaskList = GameMethod.ResolveToIntList(Convert.ToString(xEle.InnerText), ';');
                        break;
                    case "trigger":
                        managerInfo.TriggerInterface = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "NextID":
                        managerInfo.NextTaskId = GameMethod.ResolveToIntList(Convert.ToString(xEle.InnerText), ';');
                        break;
                    case "istrigger":
                        managerInfo.IsTriggerTask = (Convert.ToInt32(xEle.InnerText) == 1) ? true : false;
                        break;
                }
                #endregion
            }
            if (managerInfo.IsTriggerTask)
            {
                ConfigReader.iTriggerGuideManagerDatalXmlInfoDict.Add(managerInfo.TaskId, managerInfo);
            }
            else {
                ConfigReader.iGuideManagerDatalXmlInfoDict.Add(managerInfo.TaskId, managerInfo);
            }            
        }
    }
   
}

public class IGuideManagerData
{
    public int TaskId;
    public List<int> SonTaskList = new List<int>();
    public int TriggerInterface;
    public List<int> NextTaskId = new List<int>();
    public bool IsTriggerTask = false;

}
 
