using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;

/* 枚举值必须和服务器保持一一对应*/
public enum TASK_TIME_TYPE
{
	TTT_Infinite,
	TTT_Daily,
};

/* 枚举值必须和服务器保持一一对应*/
public enum TASK_TYPE
{
    TT_None,
    TT_KillPlayer,//击杀玩家
    TT_KillXiaobin,//击杀小兵
    TT_KillMonster,//击杀野怪
    TT_KillBuild,//击杀建筑
    TT_GamePlay,//进行一场游戏
    TT_GameWin,//胜利一场游戏
    TT_GameGetCP,//游戏获得CP
    TT_GameUseCP,//游戏使用CP
};

public class STaskConfig
{
	public uint taskID;//当前任务ID
	public uint nextTaskID;//下一任务ID
    public uint taskLevel;//任务等级
    public string taskDesc;//任务描述
	public TASK_TIME_TYPE taskTimeType;//任务时间类型
	public TASK_TYPE taskType;//任务类型
	public uint taskMaxCount;//需求总量
	public string[] taskParams;//格外参数
    public string[] n32ItemID;//奖励ID
    public string[] n32ItemNum;//奖励数量
};

public class ReadTaskConfig
{
    XmlDocument xmlDoc = null;
    public ReadTaskConfig(string xmlFilePath, TASK_TIME_TYPE timeType)
    {
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);

        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;
        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
        }

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = null;
        if (timeType==TASK_TIME_TYPE.TTT_Daily)
        {
            infoNodeList = xmlDoc.SelectSingleNode("DailyQuest ").ChildNodes;
        }
        else if (timeType==TASK_TIME_TYPE.TTT_Infinite)
        {
            infoNodeList = xmlDoc.SelectSingleNode("AchievementTask ").ChildNodes;
        }
        
        for (int i = 0; i < infoNodeList.Count; i++)
        {
            XmlAttribute xmlAttr = (infoNodeList[i] as XmlElement).GetAttributeNode("id");

            if (xmlAttr == null)
            {
                continue;
            }

            STaskConfig oneTask = new STaskConfig();

            foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
            {
                switch (xEle.Name)
                {
                    case "type":
                        oneTask.taskTimeType = (TASK_TIME_TYPE)Convert.ToUInt32(xEle.InnerText);
                        break;
                    case "grade":
                        oneTask.taskLevel = Convert.ToUInt32(xEle.InnerText);
                        break;
                    case "describe":
                        oneTask.taskDesc = xEle.InnerText;
                        break;
                    case "task":
                        oneTask.taskType = (TASK_TYPE)Convert.ToUInt32(xEle.InnerText);
                        break;
                    case "amount":
                        oneTask.taskMaxCount = Convert.ToUInt32(xEle.InnerText);
                        break;
                    case "award":
                        oneTask.n32ItemID = xEle.InnerText.Split(new char[] { ',' });
                        break;
                    case "number":
                        oneTask.n32ItemNum = xEle.InnerText.Split(new char[] { ',' });
                        break;
                    case "params":
                        oneTask.taskParams = xEle.InnerText.Split(new char[] { ',' });
                        break;
                    case "NextID":
                        oneTask.nextTaskID = Convert.ToUInt32(xEle.InnerText);
                        break;
                }
            }

            oneTask.taskID = Convert.ToUInt32(xmlAttr.InnerText);

            if (timeType == TASK_TIME_TYPE.TTT_Daily)
            {
                ConfigReader.dailyTaskXmlInfoDic.Add(oneTask.taskID, oneTask);
            }
            else if (timeType == TASK_TIME_TYPE.TTT_Infinite)
            {
                ConfigReader.infiniteTaskXmlInfoDic.Add(oneTask.taskID, oneTask);
            }
        }
    }
}
