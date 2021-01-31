using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;
using GameDefine;
using System.Linq;


public class ReadHeroHeadConfig
{

    XmlDocument xmlDoc = null;

    public ReadHeroHeadConfig(string xmlFilePath)
    {

        //TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("HeadSelect").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("HeadID") == null) continue;
            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("HeadID").InnerText;

            HeadSelectConfigInfo HeadSelectInfo = new HeadSelectConfigInfo();
            HeadSelectInfo.HeroHeadID = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {

                    case "Head_Name":
                        HeadSelectInfo.HeroHeadName = Convert.ToString(xEle.InnerText);
                        break;
                    case "Head_Type":
                        HeadSelectInfo.HeroHeadType = GameMethod.ResolveToIntList(xEle.InnerText);
                        break;

                    case "Atlas":
                        HeadSelectInfo.HeroHeadAtlas = Convert.ToString(xEle.InnerText);
                        break;

                    case "Atlas_Num":
                        HeadSelectInfo.HeroHeadAtlas_Num = xEle.InnerText;
                        break;

                    case "Used_Condition":
                        HeadSelectInfo.HeroUsedCondition = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "Account_Level":
                        HeadSelectInfo.HeroUsedLevel = Convert.ToInt32(xEle.InnerText);
                        break;
                }

                #endregion
            }
            ConfigReader.headSelectXmlInfoDict.Add(HeadSelectInfo.HeroHeadID, HeadSelectInfo);
        }
    }

    //public st
}


public class HeadSelectConfigInfo : System.Object
{
    #region 英雄头像信息
    public string HeroHeadName; //英雄名

    public int HeroUsedLevel;
    public int HeroUsedCondition; //英雄使用条件（等级）
    public int HeroHeadID; //英雄头像编号
    public string HeroHeadAtlas; //英雄头像所在图集
    public string HeroHeadAtlas_Num; //英雄头像所在图集序号
    public List<int> HeroHeadType; //英雄头像类型(性别)
    #endregion
}
