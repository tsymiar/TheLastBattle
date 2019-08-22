using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BlGame.Resource
{
    public class Archive
    {
        //private string mName;
        //private bool mIsRecursive;
        //private string mPath;
        //private bool mUseSuffix;
        //private string mSuffix;
        private Dictionary<string, string> mAllFiles; //name-type

        public Archive()
        {
            mAllFiles = new Dictionary<string, string>();
        }

        //public string Name
        //{
        //    get
        //    {
        //        return mName;
        //    }
        //    set
        //    {
        //        mName = value;
        //    }
        //}

        public Dictionary<string, string> AllFiles
        {
            get
            {
                return mAllFiles;
            }
        }

        //public void parse(string info)
        //{
        //    //1.名称
        //    int firstEmptyIndex = info.IndexOf(" ");
        //    mName = info.Substring(0, firstEmptyIndex);

        //    //2.是否递归
        //    int secondEmptyIndex = info.IndexOf(" ", firstEmptyIndex + 1);
        //    string temp = info.Substring(firstEmptyIndex + 1, secondEmptyIndex - firstEmptyIndex - 1);
        //    mIsRecursive = temp.Contains("*");

        //    //3.路径
        //    int thirdlyEmptyIndex = info.IndexOf(" ", secondEmptyIndex + 1);
        //    if (-1 == thirdlyEmptyIndex)
        //    {
        //        mPath = info.Substring(secondEmptyIndex + 1).Trim();
        //        mUseSuffix = false;
        //    }
        //    else
        //    {
        //        mPath = info.Substring(secondEmptyIndex + 1, thirdlyEmptyIndex - secondEmptyIndex - 1);
        //        //4.是否用指定后缀
        //        mSuffix = info.Substring(thirdlyEmptyIndex + 1).Trim();
        //        mUseSuffix = true;
        //    }
        //    //5.解析所有文件
        //    ResourceCommon.getFiles(mPath, mIsRecursive, mAllFiles, mUseSuffix, mSuffix);
        //}

        //public void add(Archive other)
        //{
        //    foreach (KeyValuePair<string, List<string>> p in other.mAllFiles)
        //    {
        //        if (mAllFiles.ContainsKey(p.Key))
        //        {
        //            List<string> list = new List<string>();
        //            foreach (string a in mAllFiles[p.Key])
        //                list.Add(a);
        //            foreach (string a in other.mAllFiles[p.Key])
        //                list.Add(a);
        //            mAllFiles.Remove(p.Key);
        //            mAllFiles.Add(p.Key, list);
        //        }
        //        else
        //            mAllFiles.Add(p.Key, p.Value);
        //    }
        //}

        public void add(string filename, string type)
        {
            if (!mAllFiles.ContainsKey(filename))
            {
                mAllFiles.Add(filename, type);
            }
        }

        public string getPath(string fileName)
        {
            if (mAllFiles.ContainsKey(fileName))
                return fileName + "." + mAllFiles[fileName];           //name+type
            else
                DebugEx.LogError("can not find " + fileName, ResourceCommon.DEBUGTYPENAME);
            return null;
        }

        //public void dumpAllFiles()
        //{
        //    string info = "";
        //    foreach (KeyValuePair<string, List<string>> path in mAllFiles)
        //    {
        //        foreach (string p in path.Value)
        //        {
        //            info += "the file is : " + path.Key + "\n the path is : " + p + "\n";
        //        }
        //    }
        //    DebugEx.Log(info, ResourceCommon.DEBUGTYPENAME);
        //}
    }
}