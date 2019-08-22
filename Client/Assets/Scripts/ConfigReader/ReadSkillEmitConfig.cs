using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

class ReadSkillEmitConfig
{
        XmlDocument xmlDoc = null;
        public ReadSkillEmitConfig()
        {

        }
        public ReadSkillEmitConfig(string xmlFilePath)
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

            XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkillCfg_emit ").ChildNodes;

            for (int i = 0; i < infoNodeList.Count; i++)
            {//(XmlNode xNode in infoNodeList)
                if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)
                    continue;

                string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
                //Debug.LogError(typeName);
                SkillEmitConfig skillinfo = new SkillEmitConfig();
                skillinfo.id = (uint)Convert.ToUInt32(typeName);

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
                        case "EmitType":
                            {
                                skillinfo.emitType = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "FlySpeed":
                            {
                                skillinfo.flySpeed = Convert.ToInt32(xEle.InnerText) / 100.0f;
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
                        case "eLifeTime":
                            {
                                skillinfo.lifeTime = Convert.ToInt32(xEle.InnerText) / 1000.0f;
                            }
                            break;
                        case "bIsPenetrate":
                            {
                                skillinfo.isPenetrate = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                    #endregion
                    }
                   
                }
                ConfigReader.skillEmitXmlInfoDic.Add(skillinfo.id, skillinfo);
            }
        }
}
public class SkillEmitConfig
{
    public uint id;//id
    public int emitType;//发射类型
    public float flySpeed;//飞行特效
    public string name;//名字
    public string effect;//特效
    public string sound;//声音
    public float lifeTime;//生命周期
    public int isPenetrate;//是否穿透

}


