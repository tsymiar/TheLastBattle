using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

//namespace BlGame.ConfigReader
//{
    class ReadBuffConfig
    {
        XmlDocument xmlDoc = null;
        public ReadBuffConfig()
        {
        }
        public ReadBuffConfig(string xmlFilePath)
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
            XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("SkillCfg_buff ").ChildNodes;
            for (int i = 0; i < infoNodeList.Count; i++)
            {//(XmlNode xNode in infoNodeList)
                if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)
                    continue;

                string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
                //Debug.LogError(typeName);
                BuffConfigInfo buffInfo = new BuffConfigInfo();
                buffInfo.BuffID = Convert.ToUInt32(typeName);
                foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
                {
                    #region 搜索
                    switch (xEle.Name)
                    {
                        case "szName":
                            {
                                buffInfo.BuffName = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "szEffectIcon":
                            {
                                buffInfo.IconRes = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        //case "szEffectDes":
                        //    {
                        //        buffInfo.DescContent = Convert.ToString(xEle.InnerText);
                        //    }
                        //    break;
                        case "n32EffectID":
                            {
                                buffInfo.effectID = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "szEffection":
                            {
                                buffInfo.EffectRes = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "eEffect0LastTick":
                            {
                                buffInfo.TotalTime = Convert.ToInt32(xEle.InnerText) / 1000.0f;
                            }
                            break;
                        case "szEffectSound":
                            {
                                buffInfo.sound = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        //case "n32Effect0ID":
                        //    {
                        //        buffInfo.EffectType = Convert.ToInt32(xEle.InnerText);
                        //    }
                        //    break;
                        case "eFlyEffectID":
                            {
                                buffInfo.eFlyEffectID = Convert.ToInt32(xEle.InnerText);
                            }break;
                    }
                    
                    #endregion
                }
                ConfigReader.buffXmlInfoDict.Add(buffInfo.BuffID, buffInfo);
            }
        }
    }
    public class BuffConfigInfo : System.Object
    {
        public uint BuffID;
        public string BuffName;
        public string IconRes;
        public string EffectRes;
        public int effectID;
        //public string DescContent;
        public float TotalTime;
        public string sound;
       // public int EffectType;

        public int eFlyEffectID;            //buff后续击飞效果id
    }
//}
