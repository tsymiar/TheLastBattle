using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;


class ReadSkillLeadingConfig
{
    XmlDocument xmlDoc = null;
    public ReadSkillLeadingConfig()
    {

    }
    public ReadSkillLeadingConfig(string xmlFilePath)
    {
        //TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        if (!xmlfile)
        {
            //Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkillCfg_leading ").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)
        {//(XmlNode xNode in infoNodeList)
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)
                continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
            //Debug.LogError(typeName);
            SkillLeadingonfig skillinfo = new SkillLeadingonfig();
            skillinfo.id = (uint)Convert.ToUInt32(typeName);

            //SkillConfigInfo.NpcId = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {

                switch (xEle.Name)
                {
                    #region 搜索
                    case "szName":
                        {
                            skillinfo.name = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    case "eLeadingTime":
                        {
                            skillinfo.time = Convert.ToInt32(xEle.InnerText) / 1000.0f;
                        }
                        break;
                    case "eLeadingAction":
                        {
                            skillinfo.action = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    case "eLeadingSound":
                        {
                            skillinfo.sound = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    case "eLeadingEffect":
                        {
                            skillinfo.effect = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    #endregion
                }

            }
            ConfigReader.skillLeadingInfoDic.Add(skillinfo.id, skillinfo);
        }
    }
}
public class SkillLeadingonfig
{
    public uint id;
    public string name;
    public float time;
    public string action;
    public string sound;
    public string effect;

}


