using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

public class ReadSelectHeroConfig  {

	XmlDocument xmlDoc = null;
	
	public ReadSelectHeroConfig(string xmlFilePath)
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
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("herochoose ").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("number") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("number").InnerText;
			
			HeroSelectConfigInfo HeroSelectInfo = new HeroSelectConfigInfo();
			HeroSelectInfo.HeroSelectNum = Convert.ToInt32(typeName);
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
				switch(xEle.Name)
				{
				case "name_ID":	
					HeroSelectInfo.HeroSelectName = Convert.ToString(xEle.InnerText);
					break;					
				case "name_ch":	
					HeroSelectInfo.HeroSelectNameCh = Convert.ToString(xEle.InnerText);
					break;
				case "pic_thumb":
					HeroSelectInfo.HeroSelectThumb = Convert.ToString(xEle.InnerText);
					break;
				case "un8costtype":
					HeroSelectInfo.HeroSelectCostType = Convert.ToInt32(xEle.InnerText);   
					break;
				case "n32cost":
					HeroSelectInfo.HeroSelectCost = Convert.ToSingle(xEle.InnerText);
					break;	
				case "hero_des":
					HeroSelectInfo.HeroSelectDes =  Convert.ToString(xEle.InnerText);
					break;
                case "hero_des2":
                    HeroSelectInfo.HeroSelectDes2 = Convert.ToString(xEle.InnerText);
                    break;
                case "buy_des":
                    HeroSelectInfo.HeroSelectDesSkill = Convert.ToString(xEle.InnerText); 
					break;		
				case "pic_icon":
					HeroSelectInfo.HeroSelectHead =  Convert.ToInt32(xEle.InnerText); 
					break;	
				case "sound_select":
					HeroSelectInfo.HeroSelectSound =  Convert.ToString(xEle.InnerText); 
					break;	
				case "skill_icon1":
					HeroSelectInfo.HeroSelectSkill1 = Convert.ToString(xEle.InnerText);
					break;
				case "skill_icon2":
					HeroSelectInfo.HeroSelectSkill2 = Convert.ToString(xEle.InnerText);
					break;
                case "newsguidehero":                    
                    HeroSelectInfo.IsGuideHero = Convert.ToInt32(xEle.InnerText); 
                    break;
                case "hero_desPassitive":
                    HeroSelectInfo.HeroPassDes = Convert.ToString(xEle.InnerText); 
                    break;
                case "skill_iconPassitive":
                    HeroSelectInfo.HeroSkillPass = Convert.ToString(xEle.InnerText); 
                    break;
                case "bac_des":
                    HeroSelectInfo.HeroSelectBuyDes = Convert.ToString(xEle.InnerText); ;
                    break;
				}
				
				#endregion
			}		
			ConfigReader.heroSelectXmlInfoDict.Add(HeroSelectInfo.HeroSelectNum,HeroSelectInfo);
		}
	}
}

public class HeroSelectConfigInfo:System.Object
{
	#region 道具信息
	public int    HeroSelectNum;
	public string HeroSelectName;       
	public string HeroSelectNameCh;    
	public string HeroSelectThumb;
	public int    HeroSelectCostType;
	public float  HeroSelectCost;
	public string HeroSelectDes;
    public string HeroSelectDes2;
	public string HeroSelectBuyDes;
	public int HeroSelectHead;
	public string HeroSelectSound;
	public string HeroSelectSkill1;
	public string HeroSelectSkill2;
    public int IsGuideHero;
    public string HeroPassDes;
    public string HeroSkillPass;
    public string HeroSelectDesSkill;
	#endregion
	
}
