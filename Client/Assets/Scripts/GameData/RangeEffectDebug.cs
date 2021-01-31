using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using UnityEngine;

namespace BlGame
{
    public class RangeEffectDebug
    {
        public List<RangeEffectInfo> rangeEffectList = new List<RangeEffectInfo>();
        public static readonly RangeEffectDebug Instance = new RangeEffectDebug();
        public void AddRangeStart(int id, string time)
        {
            bool bFindData = false;
            foreach (RangeEffectInfo info in rangeEffectList)
            {
                if (id == info.projectID)
                {
                    if (info.startTime == "")
                    {
                        info.startTime = time;
                        bFindData = true;
                    }
                }
            }
            if (bFindData == false)
            {
                RangeEffectInfo info = new RangeEffectInfo();
                info.projectID = id;
                info.startTime = time;
                rangeEffectList.Add(info);
            }
        }
        public void AddRangeEnd(int id, string time)
        {
            bool bFindData = false;
            foreach (RangeEffectInfo info in rangeEffectList)
            {
                if (id == info.projectID)
                {
                    if (info.endTime == "")
                    {
                        info.endTime = time;
                        bFindData = true;
                    }
                }
            }
            if (bFindData == false)
            {
                RangeEffectInfo info = new RangeEffectInfo();
                info.projectID = id;
                info.endTime = time;
                rangeEffectList.Add(info);
            }
        }

        public void SaveFile()
        {
            FileStream fs = File.Create(Application.dataPath + "/rangedata.txt");
            //byte[] str2 = System.Text.Encoding.UTF8.GetBytes("范围技能数据\n");
            //fs.Write(str2, 0, str2.Length);
            //int index = str2.Length;

            StreamWriter w = new StreamWriter(fs);
            foreach (RangeEffectInfo info in rangeEffectList)
            {
                string s = "";
                s += info.projectID;
                s += "\r\n";
                s += "开始时间:" + info.startTime + "\r\n";
                s += "结束时间:" + info.endTime + "\r\n";
                s += "-----------------------------------------------------------------------------------------------------" + "\r\n";
                //byte[] str = System.Text.Encoding.UTF8.GetBytes(s);
                w.Write(s);
                //index += str.Length;
            }
            w.Flush();
            w.Close();
        }

    }
    public class RangeEffectInfo
    {
        public int projectID = 0;
        public string startTime = "";
        public string endTime = "";
    }
}
