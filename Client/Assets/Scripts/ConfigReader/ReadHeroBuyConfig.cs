using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;
using GameDefine;

public class ReadHeroBuyConfig
{

	XmlDocument xmlDoc = null;

    public ReadHeroBuyConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("HeroBuyCfg ").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//XmlNode xNode in infoNodeList)
		{
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;

            HeroBuyConfigInfo HeroBuyInfo = new HeroBuyConfigInfo();
			HeroBuyInfo.GoodsId = Convert.ToInt32(typeName);
            string un8ConsumeType = "";
            string n32Price = "";
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{
				#region 搜索
				switch(xEle.Name)
				{
                case "Name":	
					HeroBuyInfo.Name = Convert.ToString(xEle.InnerText);
					break;
                case "HeroKind":
                    HeroBuyInfo.HeroKind = GameMethod.ResolveToIntList(xEle.InnerText);
					break;
                case "un8ConsumeType":
                    un8ConsumeType = Convert.ToString(xEle.InnerText);
					break;
                case "n32Price":
                    n32Price = Convert.ToString(xEle.InnerText);   
					break;
                case "UnlockHeroID":
                    HeroBuyInfo.UnlockHeroID = Convert.ToInt32(xEle.InnerText);
					break;
                case "Time":
                    HeroBuyInfo.Time = Convert.ToInt32(xEle.InnerText);
					break;
                case "DefaultSkin":
                    HeroBuyInfo.DefaultSkin = Convert.ToInt32(xEle.InnerText);
                    break;
                case "DefaultIcon":
                    HeroBuyInfo.DefaultIcon = Convert.ToString(xEle.InnerText);
                    break;
                case "OnSaleSkins":
                    HeroBuyInfo.OnSaleSkins = GameMethod.ResolveToStrList(xEle.InnerText); 
					break;
                case "bIsShowInShop":
                    HeroBuyInfo.IsShowShop = Convert.ToBoolean(Convert.ToInt32(xEle.InnerText));
                    break;
                    case "bIsShowInHeroInfo":
                    HeroBuyInfo.bIsShowInHeroInfo = Convert.ToBoolean(Convert.ToInt32(xEle.InnerText));
                    break;

				}
				#endregion
			}
            HeroBuyInfo.Consume = GameMethod.ResolveToIntDict(un8ConsumeType, n32Price);
            ConfigReader.heroBuyXmlInfoDict.Add(HeroBuyInfo.GoodsId, HeroBuyInfo);
		}
	}
}

public class HeroBuyConfigInfo:System.Object
{
	#region 道具信息
	public int    GoodsId;
    public string Name;
    public List<int> HeroKind;
    public Dictionary<int , int>Consume;
    public int UnlockHeroID;
    public int Time;
    public int DefaultSkin;
    public string DefaultIcon;
    public List<string> OnSaleSkins;
    public bool IsShowShop;
    public bool bIsShowInHeroInfo;
	#endregion
	
}
