using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

class ReadSkillManager
{
            XmlDocument xmlDoc = null;
        public ReadSkillManager()
        {

        }
        public ReadSkillManager(string xmlFilePath)
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

            XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkillCfg_manager ").ChildNodes;

            for (int i = 0; i < infoNodeList.Count; i++)
            {//(XmlNode xNode in infoNodeList)
                if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)
                    continue;

                string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
                //Debug.LogError(typeName);
                SkillManagerConfig skillinfo = new SkillManagerConfig();
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
                        case "bIfNormalAttack":
                            {
                                skillinfo.isNormalAttack = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "n32PrepareAction":
                            {
                                skillinfo.yAnimation = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "n32PrepareTime":
                            {
                                skillinfo.yTime = Convert.ToInt32(xEle.InnerText) / 1000.0f;
                            }
                            break;
                        case "n32PrepareEffect":
                            {
                                skillinfo.yEffect = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "n32ReleaseSound":
                            {
                                skillinfo.rSound = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "n32ReleaseAction":
                            {
                                skillinfo.rAnimation = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "n32ReleaseEffect":
                            {
                                skillinfo.rEffect = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "n32PrepareSound":
                            {
                                skillinfo.ySound = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "eUseWay":
                            {
                                skillinfo.useWay = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "eTargetType":
                            {
                                skillinfo.targetType = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "SkillIcon":
                            {
                                skillinfo.skillIcon = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "info":
                            {
                                skillinfo.info = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "n32CoolDown":
                            {
                                skillinfo.coolDown = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "n32ReleaseDistance":
                            {
                                skillinfo.range = Convert.ToInt32(xEle.InnerText) / 100.0f;
                            }
                            break;
                        case "eSummonEffect":
                            {
                                skillinfo.absorbRes = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "bIsConsumeSkill":
                            {
                                skillinfo.isAbsorb = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "n32UpgradeLevel":
                            {
                                skillinfo.n32UpgradeLevel = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "n32UseMP":
                            {
                                skillinfo.mpUse = Convert.ToSingle(xEle.InnerText);
                            }
                            break;
                        case "n32UseHP":
                            {
                                skillinfo.hpUse = Convert.ToSingle(xEle.InnerText);
                            }
                            break;
                        case "n32UseCP":
                            {
                                skillinfo.cpUse = Convert.ToSingle(xEle.InnerText);
                            }
                            break;
                    #endregion
                    }
                    
                }
                //Debug.LogError("add skillmanagerconfig:" + skillinfo.id);
                ConfigReader.skillManagerInfoDic.Add(skillinfo.id, skillinfo);
            }
        }
}

public class SkillManagerConfig
{
    public int id;//id
    public string name;//名字
    //public int  releaseWay;//释放方法
    public int  isNormalAttack;//是否普通攻击
    public string yAnimation;//吟唱动画
    public string ySound;//吟唱声音
    public string  yEffect;//吟唱效果
    public string rAnimation;//释放动画
    public string rSound;//释放声音
    public string    rEffect;//吟唱效果
    public int  useWay;//使用方法
    public int  targetType;//目标类型
    public string  skillIcon;//
    public string info;//
    public int  coolDown;//
    public float range;
    public string absorbRes;
    public int isAbsorb;
    public int n32UpgradeLevel;
    public float yTime;//吟唱时间 
    public float mpUse;
    public float hpUse;
    public float cpUse;
}


