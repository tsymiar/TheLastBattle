using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

class ReadSkillAreaConfig
{
    XmlDocument xmlDoc = null;
    public ReadSkillAreaConfig()
    {

    }
    public ReadSkillAreaConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkillCfg_area ").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)
        {//(XmlNode xNode in infoNodeList)
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)
                continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
            //Debug.LogError(typeName);
            SkillAreaConfig skillinfo = new SkillAreaConfig();
            skillinfo.id = Convert.ToUInt32(typeName);

            //SkillConfigInfo.NpcId = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                
                switch (xEle.Name)
                {
                #region 搜索
                    case "szName:":
                        {
                            skillinfo.name = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    case "eLifeTime":
                        {
                            skillinfo.lifeTime = Convert.ToInt32(xEle.InnerText);
                        }
                        break;
                    case "attackEffect":
                        {
                            skillinfo.effect = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    case "FlySound":
                        {
                            skillinfo.sound = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    case "eAoeType":
                        {
                            skillinfo.aoeType = Convert.ToInt32(xEle.InnerText);
                        }
                        break;
                #endregion
                }

            }
            ConfigReader.skillAreaInfoDic.Add(skillinfo.id, skillinfo);
        }
    }
}
public class SkillAreaConfig
{
    public uint id;//id
    public int aoeType;//
    public string name;//名字
    public string effect;//特效
    public string sound;//声音
    public float lifeTime;//生命周期

}


