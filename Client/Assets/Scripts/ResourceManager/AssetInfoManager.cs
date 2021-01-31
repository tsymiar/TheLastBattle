using System;
using System.Xml;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace BlGame.Resource
{

    public class AssetInfo
    {
        public string mName;                    //asset名称
        public int mIndex;                      //asset索引
        public int mLevel;                      //asset层级
        public int mSize;                    //assetbundle所占大小
        public List<int> mDependencys = new List<int>();    //asset关联的asset索引

        public AssetInfo()
        {
            mSize = 0;
        }

        //解析AssetInfo信息
        public void import(XmlElement element)
        {
            mName = element.GetAttribute("name");
            mIndex = Int32.Parse(element.GetAttribute("index"));
            mLevel = Int32.Parse(element.GetAttribute("level"));

            string depencyStr = element.GetAttribute("depency");
            if (depencyStr != "")
            {
                string[] depencyStrList = depencyStr.Split(',');
                for (int i = 0; i < depencyStrList.Length; i++)
                {
                    int depencyIndex = Int32.Parse(depencyStrList[i]);
                    mDependencys.Add(depencyIndex);
                }
            }

            mSize = Int32.Parse(element.GetAttribute("bundlesize"));
        }
    }


    public class AssetInfoManager
    {
        //保存名称对应的AssetInfo
        public Dictionary<string, AssetInfo> mNameAssetInfos = new Dictionary<string, AssetInfo>();
        //保存索引对应的AssetInfo
        public Dictionary<int, AssetInfo> mIndexAssetInfos = new Dictionary<int, AssetInfo>();

        public AssetInfoManager()
        {

        }

        //加载assetInfo信息
        public void LoadAssetInfo()
        {
            StreamReader sr = ResourcesManager.OpenText("AssetInfo");
            XmlDocument doc = new XmlDocument();
            doc.LoadXml(sr.ReadToEnd());
            XmlElement root = doc.DocumentElement;
            IEnumerator iter = root.GetEnumerator();
            while (iter.MoveNext())
            {
                XmlElement child_root = iter.Current as XmlElement;

                //解析AssetInfo信息
                AssetInfo assetInfo = new AssetInfo();
                assetInfo.import(child_root);

                //保存到asset信息列表o
                mNameAssetInfos.Add(assetInfo.mName, assetInfo);
                mIndexAssetInfos.Add(assetInfo.mIndex, assetInfo);
            }
            sr.Close();
        }


        //根据名称获取Asset信息 
        public AssetInfo GetAssetInfo(string name)
        {
            if (mNameAssetInfos.ContainsKey(name))
                return mNameAssetInfos[name];
            else
                return null;
        }

        //根据id获取Asset信息
        public AssetInfo GetAssetInfo(int index)
        {
            if (mIndexAssetInfos.ContainsKey(index))
                return mIndexAssetInfos[index];
            else
                return null;
        }

        //获取该Asset包含depencyAsset的总大小
        public int GetAllAssetSize(AssetInfo assetInfo)
        {
            int totalSize = 0;

            foreach (int index in assetInfo.mDependencys)
            {
                AssetInfo info = GetAssetInfo(index);
                totalSize = totalSize + info.mSize;
            }
            //加上本包大小
            totalSize = totalSize + assetInfo.mSize;

            return totalSize;
        }
    }
}