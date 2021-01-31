using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;


    class ReadMsgConfig
    {
        XmlDocument xmlDoc = null;
        public ReadMsgConfig()
        {
        }
        public ReadMsgConfig(string xmlFilePath)
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
            XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("MsgCfg ").ChildNodes;
            for (int i = 0; i < infoNodeList.Count; i++)
            {
                if ((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)
                    continue;

                string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;
                //Debug.LogError(typeName);
                MsgConfigInfo msgInfo = new MsgConfigInfo();
                msgInfo.id = Convert.ToInt32(typeName);
                #region 搜索
                foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
                {
                    
                    switch (xEle.Name)
                    {
                        case "content":
                            {
                                msgInfo.content = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "type":
                            {
                                msgInfo.msgType = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "errorcode":
                            {
                                string codes = Convert.ToString(xEle.InnerText);
                                string[] words = codes.Split(new char[] { ','});
                                if (words != null && words.Length > 0)
                                {
                                    foreach (string s in words)
                                    {
                                        //msgInfo.serverErrorCode.Add(Convert.ToInt32(s));
                                    }
                                }
                            }
                            break;
                        case "sound":
                            {
                                msgInfo.audio_name = Convert.ToString(xEle.InnerText);
                            }
                            break;
                        case "effect":
                            msgInfo.effect = Convert.ToInt32(xEle.InnerText);
                            break;
                    }
                }
                #endregion
                ConfigReader.msgXmlInfoDic.Add(msgInfo.id, msgInfo);

            }
        }
    }

    public class MsgConfigInfo : System.Object
    {
        public int id;//id
        public int msgType;
        public string content;//字符串
        public int effect;//效果
        public string audio_name;//音效
        public List<int> serverErrorCode = new List<int>();
        public MsgConfigInfo() { 

        }

        public MsgConfigInfo(MsgConfigInfo info) {
            id = info.id;
            msgType = info.msgType;
            content = info.content;
            effect = info.effect;
            audio_name = info.audio_name;
            serverErrorCode.AddRange(info.serverErrorCode);
        }
    }