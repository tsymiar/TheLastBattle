using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

class ReadSkillMoveConfig
{
    XmlDocument xmlDoc = null;
    public ReadSkillMoveConfig()
    {

    }
    public ReadSkillMoveConfig(string xmlFilePath)
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

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkillCfg_move ").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)
        {//(XmlNode xNode in infoNodeList)
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)
                continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
            //Debug.LogError(typeName);
            SkillMoveConfig skillinfo = new SkillMoveConfig();
            skillinfo.id = Convert.ToInt32(typeName);

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
                    case "MoveBeginEffect":
                        {
                            skillinfo.effectStart = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    case "MoveEndEffect":
                        {
                            skillinfo.effectEnd = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    case "n32Speed":
                        {
                            skillinfo.speed = Convert.ToInt32(xEle.InnerText) / 100.0f;
                        }
                        break;
                    case "MoveAction":
                        {
                            skillinfo.action = Convert.ToString(xEle.InnerText);
                        }
                        break;
                    #endregion
                }

            }
            ConfigReader.skillMoveInfoDic.Add(skillinfo.id, skillinfo);
        }
    }
}
public class SkillMoveConfig
{
    public int id;//id
    public string name;//名字
    public string effectStart;//开始特效
    public string effectEnd;//结束特效
    public float speed;
    public string action;
    //public float lifeTime;//生命周期

}


