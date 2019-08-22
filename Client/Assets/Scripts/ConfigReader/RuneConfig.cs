using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using GameDefine;
using BlGame.Resource;
using System.Linq;
using System.Globalization;
using System.Text;

public class RuneConfigInfo : System.Object
{
    public UInt32 runeid;
    public string RuneName;
    public Int16 level;
    public UInt32 effectID;
    public double effectVal;
    public double effectPer;
    public Int64 consumeDiamond;
    public Int64 consumeGold;
    public Int32 bIsCanRandomComposed;
    public Int32 n32ComposedSubID;
    public string Description;
    public string oriDescription;
    public string Icon;
}

public class RuneConfig
{
    XmlDocument xmlDoc = null;

    public  RuneConfig(string xmlFilePath)
	{

        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;
		xmlDoc = new XmlDocument();
		xmlDoc.LoadXml(xmlfile.text);
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("RuneCfg").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count; ++i)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)	
                continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
			
			RuneConfigInfo RuneSelectInfo = new RuneConfigInfo();
			RuneSelectInfo.runeid = Convert.ToUInt32(typeName);
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				switch(xEle.Name)
				{
				case "RuneName":	
					RuneSelectInfo.RuneName = xEle.InnerText;
					break;					
				case "un8Level":	
					RuneSelectInfo.level = Convert.ToInt16(xEle.InnerText);
					break;
				case "bIsCanRandomComposed": 
					RuneSelectInfo.bIsCanRandomComposed = Convert.ToInt32(xEle.InnerText);
					break;
                case "Description":
                    RuneSelectInfo.oriDescription = xEle.InnerText;
					RuneSelectInfo.Description = xEle.InnerText;
					break;
                 case "Icon":
                    RuneSelectInfo.Icon = xEle.InnerText;
					break;
                case "n32EffectVal":
                    RuneSelectInfo.effectVal = Convert.ToDouble(xEle.InnerText);
                    break;
                case "fEffectPer":
                    RuneSelectInfo.effectPer = Convert.ToDouble(xEle.InnerText);
                    break;
                case "un8EffectID":
                    RuneSelectInfo.effectID = Convert.ToUInt32(xEle.InnerText);
                    break;
				}
			}
            //进行描述 替换
            RuneSelectInfo.Description = (ModifyDes(RuneSelectInfo.effectVal, RuneSelectInfo.effectPer, RuneSelectInfo)).ToString();
			ConfigReader.runeXmlInfoDict[RuneSelectInfo.runeid] = RuneSelectInfo;
		}
    }

    static public StringBuilder ModifyDes(double val, double per, RuneConfigInfo cfg)
    {
        string des = cfg.oriDescription;
        var aStringBuilder = new StringBuilder(des);
        if (des.Contains("{1}"))
        {
            var index = des.IndexOf("{");
            aStringBuilder.Remove(index, 3);
            double tempVal = per / 10;
            tempVal = Math.Round(tempVal, 2);
            aStringBuilder.Insert(index, tempVal);
        }
        else
        {
            var index = des.IndexOf("{");
            aStringBuilder.Remove(index, 3);
            var tempVal = val;
            if (cfg.effectID == 13 || cfg.effectID == 14)
            {
                tempVal /= 1000;
            }
            else if (cfg.effectID == 23 || cfg.effectID == 24)
            {
                tempVal /= 10;
            }
            tempVal = Math.Round(tempVal,2);
            aStringBuilder.Insert(index, tempVal);
        }

        return aStringBuilder;
    }
}
