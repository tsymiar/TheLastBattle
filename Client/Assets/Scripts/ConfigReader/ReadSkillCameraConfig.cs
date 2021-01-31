using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

class ReadSkillCameraConfig
{
    XmlDocument xmlDoc = null;
    public ReadSkillCameraConfig()
    {

    }
    public ReadSkillCameraConfig(string xmlFilePath)
    {
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkillCfg_Camera").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)
                continue;

            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
            SkillCameraConfig skillinfo = new SkillCameraConfig();
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
                    case "mPeriod":
                        {
                            skillinfo.mPeriod = Convert.ToSingle(xEle.InnerText);
                        } break;
                    case "mOffPeriod":
                        {
                            skillinfo.mOffPeriod = Convert.ToSingle(xEle.InnerText);
                        } break;
                    case "mShakeOrient":
                        {
                            skillinfo.mShakeOrient = Convert.ToInt32(xEle.InnerText);
                        } break;
                    case "mShakeTime":
                        {
                            skillinfo.mShakeTime = Convert.ToInt32(xEle.InnerText)/1000.0f;
                        } break;
                    case "mMaxWave":
                        {
                            skillinfo.mMaxWave = Convert.ToSingle(xEle.InnerText);
                        } break;
                    case "mMinWave":
                        {
                            skillinfo.mMinWave = Convert.ToSingle(xEle.InnerText);
                        } break;

                    #endregion
                }
            }
            ConfigReader.skillCameraInfoDict.Add(skillinfo.id, skillinfo);
        }
    }
}
public class SkillCameraConfig
{
    public int id;//id
    public string name;           //名字
    public float mPeriod;         //振动周期
    public float mOffPeriod;      //偏移振动周期
    public int mShakeOrient;      //振动类型        0：水平 1:垂直 2:前后
    public float mShakeTime;      //振动时间      
    public float mMaxWave;        //最大振幅
    public float mMinWave;        //最小振幅
}

