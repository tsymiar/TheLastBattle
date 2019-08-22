using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
public class ReadLoadingTipConfig
{

    XmlDocument xmlDoc = null;

    public ReadLoadingTipConfig(string xmlFilePath)
    {

        TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("LoadingTips").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("Num") == null) continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("Num").InnerText;

            LoadingTipData stipInfo = new LoadingTipData();
            stipInfo.Num = Convert.ToInt32(typeName); 
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "Content":
                        stipInfo.Tip = Convert.ToString(xEle.InnerText);
                        break; 
                }
                #endregion
            }
            ConfigReader.loadingTipXmlInfoDic.Add(stipInfo.Num, stipInfo);
            LoadingTipData.AddIdList(stipInfo.Num);
        }
    }
}
 

public class LoadingTipData
{
    public int Num;
    public string Tip;

    private static List<int> IdList = new List<int>();
    public static void AddIdList(int id) {
        IdList.Add(id);
    }
    public static List<int> GetIdList() {
        if (IdList.Count == 0) {
            ReadLoadingTipConfig tipConfig = new ReadLoadingTipConfig("Config/LoadingTips");
        }
        return IdList;
    }
    
}