using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.GuideDate;
using GameDefine;
using BlGame;

public class RandomNameData : Singleton<RandomNameData>
{
    List<string> randomNameList1 = new List<string>();
    List<string> randomNameList2 = new List<string>();
    List<string> randomNameList3 = new List<string>();

    System.Random random = new System.Random(unchecked((int)DateTime.Now.Ticks));

    public RandomNameData()
    {
        TextAsset xmlfile = Resources.Load("Config/RandomName") as TextAsset;

        if (!xmlfile)
        {
            Debug.LogError(" error infos: 没有找到指定的xml文件：");
        }

        XmlDocument xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("RandomName").ChildNodes;
        for (int i = 0; i < infoNodeList.Count; i++)
        {
            var key = infoNodeList[i] as XmlElement;

            for (int j = 1; j <= GameDefine.GameConstDefine.maxRandomNameSegment; ++j)
            {
                var xmlEle = infoNodeList[i] as XmlElement;
                string nodeName= "Name" + j.ToString();
                if (xmlEle.GetAttributeNode(nodeName) != null)
                {
                    var name1 = xmlEle.GetAttributeNode(nodeName).InnerText;
                    AddRandomName(name1, j);
                }
            }
        }
    }

    public void Clean() {
    }

    public void AddRandomName(string name, int i)
    {
        if (i == 1)
        {
            randomNameList1.Add(name);
        }
        else if (i == 2)
        {
            randomNameList2.Add(name);

        }
        else if (i == 3){
            randomNameList3.Add(name);
        }
    }

    string getRandNameInternal(int i)
    {
        List<string> nameList = null;
        if (i == 1)
        {
            nameList = randomNameList1;
        }
        else if (i == 2)
        {
            nameList = randomNameList2;
        }
        else if (i == 3)
        {
            nameList = randomNameList3;
        }

        if (nameList == null || nameList.Count < 1)
        {
            Debug.LogError("");
            return "";
        }
        int ran = random.Next(0, nameList.Count);
        int beginIndex = 0;
        foreach (var key in nameList)
        {
            if (beginIndex == ran)
            {
                return key;
            }
            ++beginIndex;
        }

        return "";
    }
    public string GetRandName()
    {
        string randNickNameRes = "";
        for (int i = 1; i <= GameDefine.GameConstDefine.maxRandomNameSegment; ++i)
        {
            randNickNameRes += getRandNameInternal(i);
        }

        return randNickNameRes;
    }
}