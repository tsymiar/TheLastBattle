using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using GameDefine;
using BlGame.Resource;

    class ReadMsgSettingConfig
    {
        XmlDocument xmlDoc = null;
        public ReadMsgSettingConfig()
        {
        }
        public ReadMsgSettingConfig(string xmlFilePath)
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
            XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("MsgSettingConfig ").ChildNodes;
            for (int i = 0; i < infoNodeList.Count; i++)
            {
                if ((infoNodeList[i] as XmlElement).GetAttributeNode("id") == null)
                    continue;

                string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("id").InnerText;
                //Debug.LogError(typeName);
                MsgSettingConfigInfo msgSettingInfo = new MsgSettingConfigInfo();
                msgSettingInfo.id = Convert.ToInt32(typeName);
                #region 搜索
                foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
                {

                    switch (xEle.Name)
                    {
                        case "color":
                            {
                                msgSettingInfo.color = GameMethod.ResolveToColor(xEle.InnerText);
                            }
                            break;
                        case "font_size":
                            {
                                msgSettingInfo.font_size = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "effect":
                            {
                                msgSettingInfo.font_effect = Convert.ToInt32(xEle.InnerText);
                            }
                            break;
                        case "effect_color":
                            {
                                msgSettingInfo.fontEffect_color = GameMethod.ResolveToColor(xEle.InnerText);
                            }
                            break;
                        case "effect_distance":
                            {
                                msgSettingInfo.fontEffect_distance = GameMethod.ResolveToVector2(xEle.InnerText);
                            }
                            break;
                        case "position_start_in":
                            {
                                msgSettingInfo.position_start_in = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "position_end_in":
                            {
                                msgSettingInfo.position_end_in = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "position_time_in":
                            msgSettingInfo.position_time_in = Convert.ToSingle(xEle.InnerText);
                            break;
                        case "position_start_out":
                            {
                                msgSettingInfo.position_start_out = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "position_end_out":
                            {
                                msgSettingInfo.position_end_out = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "position_time_out":
                            msgSettingInfo.position_time_out = Convert.ToSingle(xEle.InnerText);
                            break;
                        case "rotation_start_in":
                            {
                                msgSettingInfo.rotation_start_in = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "rotation_end_in":
                            {
                                msgSettingInfo.rotation_end_in = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "rotation_time_in":
                            msgSettingInfo.rotation_time_in = Convert.ToSingle(xEle.InnerText);
                            break;
                        case "rotation_start_out":
                            {
                                msgSettingInfo.rotation_start_out = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "rotation_end_out":
                            {
                                msgSettingInfo.rotation_end_out = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "rotation_time_out":
                            msgSettingInfo.rotation_time_out = Convert.ToSingle(xEle.InnerText);
                            break;
                        case "scale_start_in":
                            {
                                msgSettingInfo.scale_start_in = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "scale_end_in":
                            {
                                msgSettingInfo.scale_end_in = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "scale_time_in":
                            msgSettingInfo.scale_time_in = Convert.ToSingle(xEle.InnerText);
                            break;
                        case "scale_start_out":
                            {
                                msgSettingInfo.scale_start_out = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "scale_end_out":
                            {
                                msgSettingInfo.scale_end_out = GameMethod.ResolveToVector3(xEle.InnerText);
                            }
                            break;
                        case "scale_time_out":
                            msgSettingInfo.scale_time_out = Convert.ToSingle(xEle.InnerText);
                            break;
                        case "alpha_start_in":
                            {
                                msgSettingInfo.alpha_start_in = Convert.ToSingle(xEle.InnerText);
                            }
                            break;
                        case "alpha_end_in":
                            {
                                msgSettingInfo.alpha_end_in = Convert.ToSingle(xEle.InnerText);
                            }
                            break;
                        case "alpha_time_in":
                            msgSettingInfo.alpha_time_in = Convert.ToSingle(xEle.InnerText);
                            break;
                        case "alpha_start_out":
                            {
                                msgSettingInfo.alpha_start_out = Convert.ToSingle(xEle.InnerText);
                            }
                            break;
                        case "alpha_end_out":
                            {
                                msgSettingInfo.alpha_end_out = Convert.ToSingle(xEle.InnerText);
                            }
                            break;
                        case "alpha_time_out":
                            msgSettingInfo.alpha_time_out = Convert.ToSingle(xEle.InnerText);
                            break;
                        case "if_frame":
                            msgSettingInfo.if_frame = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "staytime":
                            msgSettingInfo.stayTime = Convert.ToInt32(xEle.InnerText);
                            break;
                    }
                }
                #endregion
                ConfigReader.msgSettingXmlInfoDic.Add(msgSettingInfo.id, msgSettingInfo);
            }
            
        }
    }
    public class MsgSettingConfigInfo : System.Object
    {
        public int id;//id
        public Color color;//字体颜色
        public int font_size;//字体尺寸
        public int font_effect;//特效  如：加字体边框
        public Color fontEffect_color;//字体描边颜色
        public Vector2 fontEffect_distance;//字体描边
        public Vector3 position_start_in;//淡入起始坐标
        public Vector3 position_end_in;//淡入终点坐标
        public float position_time_in;//淡入时间
        public Vector3 position_start_out;//淡出起始坐标
        public Vector3 position_end_out;//淡出终点坐标
        public float position_time_out;//淡出时间
        public Vector3 rotation_start_in;//入旋转起始坐标
        public Vector3 rotation_end_in;//入旋转终点坐标
        public float rotation_time_in;//旋转时间
        public Vector3 rotation_start_out;//出旋转起始坐标
        public Vector3 rotation_end_out;//出旋转终点坐标
        public float rotation_time_out;//出旋转时间
        public Vector3 scale_start_in;//入场缩放起始坐标
        public Vector3 scale_end_in;//入场缩放终点坐标
        public float scale_time_in;//缩放时间
        public Vector3 scale_start_out;//出场缩放起始坐标
        public Vector3 scale_end_out;//出场缩放终点坐标
        public float scale_time_out;//出场缩放时间
        public float alpha_start_in;//入场透明度起始值
        public float alpha_end_in;//入场透明度终点值
        public float alpha_time_in;//入场时间
        public float alpha_start_out;//出场透明度起始值
        public float alpha_end_out;//出场透明度起始值
        public float alpha_time_out;//出场透明度时间
        public int if_frame;
        public int stayTime;

        ////int moveDist;//移动距离
        //public int time1;//停留显示时间
        //public int time2;//渐隐时间
        ////int maxMsg;//最大消息数量
        //public int colorR;//颜色r
        //public int colorG;//颜色g
        //public int  colorB;//颜色b
        //public int fontsize;//字体大小

        //string effect;//特效
    }

