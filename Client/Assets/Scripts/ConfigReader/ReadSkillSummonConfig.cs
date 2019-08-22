using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

class ReadSkillSummonConfig
{
    XmlDocument xmlDoc = null;
    public ReadSkillSummonConfig()
    {

    }
    public ReadSkillSummonConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkillCfg_summon ").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)
        {//(XmlNode xNode in infoNodeList)
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)
                continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
            //Debug.LogError(typeName);
            SkillSummonConfig skillinfo = new SkillSummonConfig();
            skillinfo.id = Convert.ToInt32(typeName);

            //SkillConfigInfo.NpcId = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "szName":
                        {
                            skillinfo.name = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    case "summonEffect":
                        {
                            skillinfo.effect = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    case "summonSound":
                        {
                            skillinfo.sound = Convert.ToString(xEle.InnerText);
                        }
                        break;
                #endregion
                }

            }
            ConfigReader.skillSummonInfoDic.Add(skillinfo.id, skillinfo);
        }
    }
}
public class SkillSummonConfig
{
    public int id;//id
    //public int emitType;//发射类型
    //public float flySpeed;//飞行特效
    public string name;//名字
    public string effect;//特效
    public string sound;
    //public string sound;//声音
    //public float lifeTime;//生命周期
    //public int isPenetrate;//是否穿透

}


