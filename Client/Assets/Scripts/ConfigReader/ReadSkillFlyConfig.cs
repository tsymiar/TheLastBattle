using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

class ReadSkillFlyConfig
{
    XmlDocument xmlDoc = null;
    public ReadSkillFlyConfig()
    {

    }
    public ReadSkillFlyConfig(string xmlFilePath)
    {        
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkillCfg_fly ").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)
                continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
            SkillFlyConfig skillinfo = new SkillFlyConfig();
            skillinfo.id = Convert.ToInt32(typeName);

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
                    case "n32RaiseSpeed":
                        {
                            skillinfo.raiseSpeed = Convert.ToSingle(xEle.InnerText);
                        }
                        break;
                    case "n32AccSpeed":
                        {
                            skillinfo.raiseAccSpeed = Convert.ToSingle(xEle.InnerText);
                        }
                        break;
                    case "n32FallSpeed":
                        {
                            skillinfo.fallSpeed = Convert.ToSingle(xEle.InnerText);
                        }
                        break;
                    case "n32FallAccSpeed":
                        {
                            skillinfo.fallAccSpeed = Convert.ToSingle(xEle.InnerText);
                        }
                        break;

                    case "n32StayTime":
                        {
                            skillinfo.stayTime = Convert.ToSingle(xEle.InnerText)/1000.0f;
                        }break;
                    case "n32MoveAction":
                        {
                            skillinfo.moveAction = Convert.ToString(xEle.InnerText);
                        }break;

                    case "bIsCanBeRecover":
                        {
                            skillinfo.bIsCanBeRecover = Convert.ToInt32(xEle.InnerText);
                        }break;
                    #endregion
                }

            }
            ConfigReader.skillBeatFlyInfoDict.Add(skillinfo.id, skillinfo);
        }
    }
}
public class SkillFlyConfig
{
    public int id;//id
    public string name;           //名字
    public float raiseSpeed;        //上升速度
    public float fallSpeed;         //下降速度
    public float raiseAccSpeed;     //向上加速度             (加速为正，减速为负)
    public float fallAccSpeed;      //向下加速度
    public float stayTime;          //停留时间
    public string moveAction;       //动作
    public int bIsCanBeRecover;     // 0:击飞不会叠加  1:击飞可以被叠加  
}

