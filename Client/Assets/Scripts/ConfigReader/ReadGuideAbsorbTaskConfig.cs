using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuideAbsorbTaskConfig
{

	XmlDocument xmlDoc = null;

    public ReadGuideAbsorbTaskConfig(string xmlFilePath)
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
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("absorb").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            CAbsorbTask absorbInfo = new CAbsorbTask();
            absorbInfo.TaskId = Convert.ToInt32(typeName);
            absorbInfo.TaskType = GuideTaskType.AbsorbTask;
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "npcid":
                        absorbInfo.NpcId = Convert.ToInt32(xEle.InnerText);
                        break;  
                }
				#endregion
			}
            CTaskBase.absorbTaskDic.Add(absorbInfo.TaskId, absorbInfo);
		}
	}
}
 
