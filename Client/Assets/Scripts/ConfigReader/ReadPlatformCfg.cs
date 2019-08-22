using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

class ReadPlatformCfg// : MonoBehaviour
{
    public ReadPlatformCfg()
    { }

	XmlDocument xmlDoc = null;
    public   ReadPlatformCfg(string xmlFilePath)
    {
        //TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;
       // ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
       // TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        //if(xmlfile == null)
        //{
        //    Debug.LogError("error info: 没有平台相关XML文件配置");
        //}
        //xmlDoc = new XmlDocument();
        //xmlDoc.LoadXml(xmlfile.text);

        //XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("platform").ChildNodes ;
        //for (int i = 0; i < infoNodeList.Count; i++)
        //{
           
        //    if ((infoNodeList[i] as XmlElement).GetAttributeNode("ID") == null)
        //    {
        //        continue;
        //    }

        //    PlatformCfg cfg = new PlatformCfg();
        //    cfg.szPlatfromId = (infoNodeList[i] as XmlElement).GetAttribute("ID"); 
        //    foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
        //    { 
        //        switch(xEle.Name)
        //        {
        //            case "szName":
        //                {
        //                    cfg.szName = Convert.ToString(xEle.InnerText);
        //                }break;
        //            default: break;
        //        }
        //    }
        //    ConfigReader.platfromCfgDic.Add(cfg.szName, cfg); 
        //} 
    }
     
}
 
public class PlatformCfg
{ 
    public string szPlatfromId;//名字 
    public string szName;//名字 
}