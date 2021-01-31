using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuideObjShowTaskConfig
{

	XmlDocument xmlDoc = null;

    public ReadGuideObjShowTaskConfig(string xmlFilePath)
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
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("gameobjctshow").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            CGameObjectShowTask showInfo = new CGameObjectShowTask();
            showInfo.TaskId = Convert.ToInt32(typeName); ;
            showInfo.TaskType = GuideTaskType.ObjShowTask; 
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "pathtype":
                        showInfo.PathType = (UIPathType)Convert.ToInt32(xEle.InnerText);
                        break;  
                    case "path":
                        showInfo.Path = Convert.ToString(xEle.InnerText);
                        break;
                    case "startshow":
                        showInfo.StartShow = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "endshow":
                        showInfo.EndShow = Convert.ToInt32(xEle.InnerText);
                        break;
                }
				#endregion
			}
            CTaskBase.objShowTaskDic.Add(showInfo.TaskId, showInfo);
		}
	}
}
 
