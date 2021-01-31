using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuideVoiceTaskConfig
{

	XmlDocument xmlDoc = null;

    public ReadGuideVoiceTaskConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("Audio").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            CVoiceTask voiceInfo = new CVoiceTask();
            voiceInfo.TaskId = Convert.ToInt32(typeName);
            voiceInfo.TaskType = GuideTaskType.VoiceTask;
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "Audio":
                        voiceInfo.VoicePath = Convert.ToString(xEle.InnerText);
                        break;                             
                }
				#endregion
			} 
            CTaskBase.voiceTaskDic.Add(voiceInfo.TaskId, voiceInfo);
		}
	}
}
 
