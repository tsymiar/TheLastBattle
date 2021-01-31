using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuideBornNpcTaskConfig
{

	XmlDocument xmlDoc = null;

    public ReadGuideBornNpcTaskConfig(string xmlFilePath)
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
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("bornnpc").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            CSendNpcTask sendNpcInfo = new CSendNpcTask();
            sendNpcInfo.TaskId = Convert.ToInt32(typeName);
            sendNpcInfo.TaskType = GuideTaskType.SenderSoldierTask;
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "bornindex":
                        sendNpcInfo.MilitaryId = Convert.ToInt32(xEle.InnerText);
                        break;  
                    case "borntag":
                        sendNpcInfo.Tag = Convert.ToInt32(xEle.InnerText);
                        break;

                }
				#endregion
			}
            CTaskBase.sendNpcTaskDic.Add(sendNpcInfo.TaskId, sendNpcInfo);
		}
	}
}

public class SendNpcInfo
{
    public int mTaskId;

}
 
