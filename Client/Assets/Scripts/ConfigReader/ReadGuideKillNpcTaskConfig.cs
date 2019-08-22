using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuideKillNpcTaskConfig
{

	XmlDocument xmlDoc = null;

    public ReadGuideKillNpcTaskConfig(string xmlFilePath)
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
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("killnpc").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            CKillTask killInfo = new CKillTask();
            killInfo.TaskId = Convert.ToInt32(typeName);
            killInfo.TaskType = GuideTaskType.KillTask;
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "deadtype":
                        killInfo.DeadType = (DeadType)Convert.ToInt32(xEle.InnerText);
                        break;
                    case "deadnpcid":
                        killInfo.DeadId = Convert.ToInt32(xEle.InnerText);          
                        break;      
                    case "times":
                        killInfo.DeadTimes = Convert.ToInt32(xEle.InnerText);            
                        break;
                    case "deadreason":
                       string reason = Convert.ToString(xEle.InnerText);
                       if (reason.IndexOf(',') != -1)
                       {
                           killInfo.KillReason = GameMethod.ResolveToIntList(reason);
                       }
                       else
                       {
                           killInfo.KillReason.Add(Convert.ToInt32(reason));
                       }
                       break;

                }
				#endregion
			}
            CTaskBase.killTaskDic.Add(killInfo.TaskId, killInfo);
		}
	}
}
 
