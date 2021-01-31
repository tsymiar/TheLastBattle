using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
public class ReadGuideHelpConfig
{

    XmlDocument xmlDoc = null;

    public ReadGuideHelpConfig(string xmlFilePath)
    {

        TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("HelpTips").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;

            GuideHelpData helpInfo = new GuideHelpData();
            helpInfo.helpId = Convert.ToInt32(typeName); 
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "tiptimes":
                        helpInfo.helpTimes = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "tipmatches":
                        helpInfo.helpMatches = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "tippath":
                        helpInfo.helpObjPathList = GameMethod.ResolveToStrList(Convert.ToString(xEle.InnerText),';');
                        break;
                    case "tipevent":
                        helpInfo.helpTriggerEvent = Convert.ToInt32(xEle.InnerText);
                        break;                       
                    case "tipShowTimes":
                        helpInfo.helpShowTime = Convert.ToSingle(xEle.InnerText);
                        break;
                }
                #endregion
            }
            ConfigReader.guideHelpXmlInfoDict.Add(helpInfo.helpId, helpInfo); 
        }
    }
}
 

public class GuideHelpData
{
    public int helpId;
    public int helpTimes;
    public int helpMatches;
    public List<string> helpObjPathList;
    public int helpTriggerEvent;
    public float helpShowTime;
}