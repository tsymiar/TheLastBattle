using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using System.Linq;
public class ReadBuySkinConfig
{

    XmlDocument xmlDoc = null;

    public ReadBuySkinConfig(string xmlFilePath)
    {

        TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkinsPurchaseCfg").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("CommodityID") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("CommodityID").InnerText;

            BuySkinConfigInfo buySkinInfo = new BuySkinConfigInfo();
            buySkinInfo.skinId = Convert.ToInt32(typeName);
            string cost = "",price = "";
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {                
                #region 搜索
                switch (xEle.Name)
                {
                    case "UnlockHeroID":
                        buySkinInfo.heroId = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "HalfPhotoIcon":
                        buySkinInfo.halfPhotoIcon = Convert.ToString(xEle.InnerText);
                        break;
                    case "DefaultSkin":
                        buySkinInfo.isDefaultSkin = Convert.ToInt32(xEle.InnerText) == 1 ? true : false;
                        break;
                    case "TexturePick":
                        buySkinInfo.skinTextureName = Convert.ToString(xEle.InnerText);
                        break;
                }

                #endregion
            }  
            ConfigReader.buySkinXmlInfoDict.Add(buySkinInfo.skinId, buySkinInfo);
            HeroWithSkinHelp.Instance().LinkHeroWithSkin(buySkinInfo);
        }
    }
}

public class HeroWithSkin
{
    public int heroId;
    public List<int> skinList = new List<int>();
    public int defaultSkinId;
    public void AddSkin(BuySkinConfigInfo info)
    {
        if (skinList.Contains(info.skinId))
        {
            return;
        }
        skinList.Add(info.skinId);
        if (info.isDefaultSkin)
        {            
            defaultSkinId = info.skinId;
        }
    }
}

public class HeroWithSkinHelp
{ 
    private static HeroWithSkinHelp intance = null;

    public static HeroWithSkinHelp Instance() {
        if (intance == null) {
            intance = new HeroWithSkinHelp();
        }
        return intance;
    }

    public Dictionary<int, HeroWithSkin> heroWithSkin = new Dictionary<int, HeroWithSkin>();
    public void LinkHeroWithSkin(BuySkinConfigInfo info)
    {
        HeroWithSkin heroInfo;
        if (heroWithSkin.TryGetValue(info.heroId, out heroInfo))
        {
            heroInfo.AddSkin(info);
            return;    
        }
        heroInfo = new HeroWithSkin();
        heroInfo.heroId = info.heroId;
        heroInfo.AddSkin(info);
        heroWithSkin.Add(heroInfo.heroId, heroInfo);       
    }

    public HeroWithSkin GetHeroWithSkinInfo(int heroId) {
        HeroWithSkin info;

        if (heroWithSkin.TryGetValue(heroId, out info))
        {
            return info;
        }
        return null;
    }
}

public class BuySkinConfigInfo : System.Object
{
    public int skinId;
    public int heroId;
    public string halfPhotoIcon;  
    public string skinTextureName;
    public bool isDefaultSkin;
    
}
