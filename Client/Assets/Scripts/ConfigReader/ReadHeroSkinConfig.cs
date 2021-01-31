using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;
using GameDefine;

public class ReadHeroSkinConfig
{

	XmlDocument xmlDoc = null;
	
	public ReadHeroSkinConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkinCfg ").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//XmlNode xNode in infoNodeList)
		{
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;

            HeroSkinConfigInfo HeroSkininfo = new HeroSkinConfigInfo();
            HeroSkininfo.GoodsId = Convert.ToInt32(typeName);
            string un8ConsumeType = "";
            string n32Price = "";
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{
				#region 搜索
				switch(xEle.Name)
				{
                case "SkinName":
                    HeroSkininfo.SkinName = Convert.ToString(xEle.InnerText);
					break;
                case "un8ConsumeType":
                    un8ConsumeType = Convert.ToString(xEle.InnerText);
					break;
                case "n32Price":
                    n32Price = Convert.ToString(xEle.InnerText);   
					break;
                case "Description":
                    HeroSkininfo.Description = Convert.ToString(xEle.InnerText);
					break;
                case "Time":
                    HeroSkininfo.Time = Convert.ToInt32(xEle.InnerText);
					break;
                case "Icon":
                    HeroSkininfo.Icon = Convert.ToString(xEle.InnerText);
                    break;
                case "ResourcesID":
                    HeroSkininfo.ResourcesID = Convert.ToString(xEle.InnerText); 
					break;
                case "Skill1Effect":
                    HeroSkininfo.Skill1Effect = Convert.ToString(xEle.InnerText);
                    break;
                case "Skill2Effect":
                    HeroSkininfo.Skill2Effect = Convert.ToString(xEle.InnerText);
                    break;		
				}
				#endregion
			}
            HeroSkininfo.Consume = GameMethod.ResolveToIntDict(un8ConsumeType, n32Price);
            ConfigReader.heroSkinXmlInfoDict.Add(HeroSkininfo.GoodsId, HeroSkininfo);
		}
	}
}

public class HeroSkinConfigInfo:System.Object
{
	#region 道具信息
    public int GoodsId;
    public string SkinName;
    public Dictionary<int, int> Consume;
    public int Time;
    public string Description;
    public string Icon;
    public string ResourcesID;
    public string Skill1Effect;
    public string Skill2Effect;
	#endregion
	
}
