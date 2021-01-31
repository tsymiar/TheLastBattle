using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using System.Linq;
using BlGame.Resource;

public class ReadGuideCameraTaskConfig
{

	XmlDocument xmlDoc = null;

    public ReadGuideCameraTaskConfig(string xmlFilePath)
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
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("Vidicon").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            CameraMoveInfo info = new CameraMoveInfo();
            info.mTaskId = Convert.ToInt32(typeName);
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "begin":
                        info.mStartPos = GameMethod.ResolveToVector3AsServer(xEle.InnerText);
                        break;
                    case "end":
                        info.mEndPos = GameMethod.ResolveToVector3AsServer(xEle.InnerText);
                        break;
                    case "aspect":
                        info.mAspect = GameMethod.ResolveToVector3AsServer(xEle.InnerText);
                        break;
                    case "time":
                        info.mDurTime = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "Nexttime":
                        info.mNextTime = Convert.ToSingle(xEle.InnerText);
                        break;
                    case "goon":
                        info.mGoon = Convert.ToInt32(xEle.InnerText);
                        break;
                }
				#endregion
			}
            ConfigReader.guideCameraMoveXmlInfoDict.Add(info.mTaskId, info);
		}
	}
}

public class CameraMoveInfo
{
    public int mTaskId;
    public Vector3 mStartPos;
    public Vector3 mEndPos;
    public Vector3 mAspect;
    public float mDurTime;
    public float mNextTime;
    public int mGoon;

}
 
