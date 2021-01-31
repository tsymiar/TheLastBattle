using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame.Resource;

public class ReadGuideRewardTaskConfig
{

	XmlDocument xmlDoc = null;

    public ReadGuideRewardTaskConfig(string xmlFilePath)
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
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("Award").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            CRewardTask rewardInfo = new CRewardTask();
            rewardInfo.TaskId = Convert.ToInt32(typeName);
            rewardInfo.TaskType = GuideTaskType.RewardTipTask;
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
                switch (xEle.Name)
                {
                    case "award":
                        rewardInfo.RewardType = (RewardTaskType) Convert.ToInt32(xEle.InnerText);
                        break;
                    case "quantity":
                        rewardInfo.RewardResult = Convert.ToInt32(xEle.InnerText);          
                        break;      
                    case "pos":
                        string pos = Convert.ToString(xEle.InnerText);
                        if (pos.Length == 1) {
                            rewardInfo.EffectPos = Vector3.zero;
                        }
                        else {
                            List<float> posList = GameMethod.ResolveToFloatList(Convert.ToString(xEle.InnerText), ';');
                            rewardInfo.EffectPos = new Vector3(posList[0], posList[1], posList[2]);
                        }                
                        break;
                    case "path":
                        rewardInfo.EffectPath = Convert.ToString(xEle.InnerText);
                        break;

                }
				#endregion
			}
            CTaskBase.rewardTaskDic.Add(rewardInfo.TaskId, rewardInfo);
		}
	}
}
 
