using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;
using GameDefine;
using System.Linq;
using BlGame.GuideDate;
using GameDefine;

public class ReadGuideTaskConfig
{

    XmlDocument xmlDoc = null;

    public ReadGuideTaskConfig(string xmlFilePath)
    {

        //TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("Trigger").ChildNodes;

        for (int i = 0; i < infoNodeList.Count; i++)//XmlNode xNode in infoNodeList)
        {
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("TaskID") == null) continue;
            string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("TaskID").InnerText;

            GuideTaskInfo guideInfo = new GuideTaskInfo();
            guideInfo.TaskId = Convert.ToInt32(typeName);
            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                #region 搜索
                switch (xEle.Name)
                {
                    case "TaskId":
                        guideInfo.TaskId = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "Text":
                        guideInfo.Text = Convert.ToString(xEle.InnerText);
                        break;
                    case "PrefabName":
                        guideInfo.PrefabName = Convert.ToString(xEle.InnerText);
                        break;
                    case "PosXYZ":
                        guideInfo.PosXYZ = GameMethod.ResolveToVector3(xEle.InnerText);
                        break;
                    case "ModelID":
                        guideInfo.ModelID = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "IsFinishModel":
                        guideInfo.IsFinishModel = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "GuideType":
                        guideInfo.GuideType = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "BtnName":
                        guideInfo.BtnName = xEle.InnerText;
                        break;
                    case "triggertype":
                        guideInfo.mBtnTriggerType = (ButtonTriggerType)Convert.ToInt32(xEle.InnerText);
                        break;
                    case "type":
                        guideInfo.mDragType = (ButtonDragType)Convert.ToInt32(xEle.InnerText);
                        break;
                    case "times":
                        guideInfo.mTaskTimes = Convert.ToInt32(xEle.InnerText);
                        break;
                    case "Btncoord":
                        guideInfo.mPos = GameMethod.ResolveToVector3(xEle.InnerText);
                        break;
                    case "GuideEffect":
                        guideInfo.GuideEffect = xEle.InnerText;
                        break;
                }

                #endregion
            }
            ConfigReader.guideTaskXmlInfoDict.Add(guideInfo.TaskId, guideInfo);
        }
    }

    //public st
}


public class GuideTaskInfo : System.Object
{
    #region 英雄头像信息
    public int TaskId;
    public string Text;
    public string PrefabName;
    public Vector3 PosXYZ;
    public string BtnName;
    public int ModelID;
    public int IsFinishModel;
    public int GuideType;
    public ButtonTriggerType mBtnTriggerType;
    public ButtonDragType mDragType;
    public int mTaskTimes;
    public Vector3 mPos;
    public string GuideEffect;
    #endregion
}
