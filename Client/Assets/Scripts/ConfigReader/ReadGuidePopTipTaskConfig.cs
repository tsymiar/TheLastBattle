using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuidePopTipTaskConfig
{

	XmlDocument xmlDoc = null;

    public ReadGuidePopTipTaskConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("PopWindow").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            PopTipskInfo info = new PopTipskInfo();
            info.TaskId = Convert.ToInt32(typeName);
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "content":
                        info.mTip = Convert.ToString(xEle.InnerText);
                        break;
                    case "site":
                        info.mResPath = Convert.ToString(xEle.InnerText);
                        break;
                    case "address":
                        info.mSitePos = GameMethod.ResolveToVector3(xEle.InnerText);
                        break;
                    case "rate":
                        info.mRate = GameMethod.ResolveToVector3(xEle.InnerText);
                        break;
                    case "mTime":
                        info.mTime = Convert.ToSingle(xEle.InnerText);
                        break;
                }
				#endregion
			}
            ConfigReader.guidePopTipTaskXmlDict.Add(info.TaskId, info);
		}
	}
}

public class PopTipskInfo
{ 
    public int TaskId;
    public string mTip;
    public string mResPath;
    public Vector3 mSitePos;
    public Vector3 mRate;
    public float mTime;
}
 
