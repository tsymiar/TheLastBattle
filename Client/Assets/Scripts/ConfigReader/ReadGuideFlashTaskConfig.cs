using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuideFlashTaskConfig  {

	XmlDocument xmlDoc = null;

    public ReadGuideFlashTaskConfig(string xmlFilePath)
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
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("Animine").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;
			
			CFlashTask flashInfo = new CFlashTask();
			flashInfo.TaskId = Convert.ToInt32(typeName);
            flashInfo.TaskType = GuideTaskType.ObjFlashTask;
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "localpath":
                        flashInfo.UiPath = Convert.ToString(xEle.InnerText);
                        break;
                    case "pathtype":
                        flashInfo.PathType = (UIPathType)Convert.ToInt32(xEle.InnerText);
                        break;
                    case "startpos":
                        flashInfo.StartPos = GameMethod.ResolveToVector3(Convert.ToString(xEle.InnerText), ';');    
                        break;
                    case "begin":
                        flashInfo.StartEffect = Convert.ToString(xEle.InnerText);                        
                        break;
                    case "objective":
                        flashInfo.TargetEffect = Convert.ToString(xEle.InnerText);            
                        break;
                    case "frequency":
                        flashInfo.During = Convert.ToSingle(xEle.InnerText);            
                        break;
                    case "form":
                        flashInfo.EffectType = (UITweener.Method)Convert.ToInt32(xEle.InnerText);  
                        break;
                    case "style":
                        flashInfo.Style = (UITweener.Style)Convert.ToInt32(xEle.InnerText);            
                        break;
                    case "animation":
                        flashInfo.Type = (FlashType)Convert.ToInt32(xEle.InnerText);    
                        break;
                    case "finish":
                        flashInfo.OverReset = Convert.ToInt32(xEle.InnerText);  
                        break;
                    case "DefaultLocation":
                        flashInfo.PrefabPath = Convert.ToString(xEle.InnerText); 
                        break;
                }
				#endregion
			}
            CTaskBase.flashTaskDic.Add(flashInfo.TaskId, flashInfo);
		}
	}
}
 
