using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using System.Linq;
using BlGame.Resource;

public class ReadGuideManagerTaskConfig
{

	XmlDocument xmlDoc = null;

    public ReadGuideManagerTaskConfig(string xmlFilePath)
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
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("taskmanager").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            GuideMgrInfo mgrInfo = new GuideMgrInfo();
            mgrInfo.TaskId = Convert.ToInt32(typeName);

			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "childtype":
                        mgrInfo.ChildTaskType = GameMethod.ResolveToIntList(xEle.InnerText);
                        break;  
                    case "childid":
                        mgrInfo.ChildTaskId = GameMethod.ResolveToIntList(xEle.InnerText);
                        break;
                    case "endtype":
                        mgrInfo.TaskEndType = (TaskCheckType)Convert.ToInt32(xEle.InnerText);
                        break;
                    case "endid":
                        mgrInfo.EndTaskChildId = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "nexttaskid":
                        mgrInfo.NextTaskId = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "close":
                        mgrInfo.mToServerType = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "tasktype":
                        mgrInfo.mTasktype = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "moduleend":
                        mgrInfo.moduleend = Convert.ToInt32(xEle.InnerText) == 1 ? true : false;
                        break;
                }
				#endregion
			}
            ConfigReader.guideTaskMgrInfoDict.Add(mgrInfo.TaskId, mgrInfo);
		}


	}
}

public class GuideMgrInfo
{
    public int TaskId;
    public List<int> ChildTaskType;
    public List<int> ChildTaskId;
    public TaskCheckType TaskEndType;
    public int EndTaskChildId;
    public int NextTaskId;
    public int mToServerType;
    public int mTasktype;
    public bool moduleend;

}
 
