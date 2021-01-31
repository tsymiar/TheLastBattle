using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuideObstructTaskConfig
{

	XmlDocument xmlDoc = null;

    public ReadGuideObstructTaskConfig(string xmlFilePath)
	{
		//TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

		if(!xmlfile)
		{
			Debug.LogError(" error infos: 没有找到指定的xml文件："+xmlFilePath);
		}		

		xmlDoc = new XmlDocument();
		xmlDoc.LoadXml(xmlfile.text);
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("obstruct").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            CObstructTask obstructInfo = new CObstructTask();
            obstructInfo.TaskId = Convert.ToInt32(typeName);
            obstructInfo.TaskType = GuideTaskType.ObstructTask;
            Vector3 center = new Vector3();
            Vector3 target = new Vector3();
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "centerpoint":
                        center = GameMethod.ResolveToVector3(Convert.ToString(xEle.InnerText),';');
                        break;  
                    case "targetpoint":
                        target = GameMethod.ResolveToVector3(Convert.ToString(xEle.InnerText), ';'); 
                        break;
                    case "text":
                        obstructInfo.ObsTip = Convert.ToString(xEle.InnerText); 
                        break;
                    case "time":
                        obstructInfo.ObsShowTime = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "path":
                        obstructInfo.ObsPath = Convert.ToString(xEle.InnerText);
                        break;
                    case "count":
                        obstructInfo.ObsCount = Convert.ToInt32(xEle.InnerText);
                        break;
                }
				#endregion
			}
            obstructInfo.ObsCenter = center;
            obstructInfo.ObsDisance = Vector3.Distance(center,target);
            CTaskBase.obstructTaskDic.Add(obstructInfo.TaskId, obstructInfo);
		}
	}
}
 
