using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame.Resource;


namespace BlGame
{
    public enum EBattleMatchType
    {
        EBMT_None,
        EBMT_Ai,
        EBMT_Normal,
        EBMT_Rank,
        EBMT_Train,
        EBMT_Dungeon,
    }

    public class MapLoadConfig : Singleton<MapLoadConfig>
    {
        public MapLoadConfig()
        {
            ReadConfig("Config/MapLoadCfg");
        }

        //取得MAP信息
        public MapInfo GetMapInfo(uint id)
        {
            if (mMapList.ContainsKey(id))
            {
                return mMapList[id];
            }

            return null;
        }

        //根据地图名称获取Map信息
        public MapInfo GetMapInfo(string name) 
        {
            foreach (MapInfo info in mMapList.Values)
            {
                if (info.mLoadScene == name)
                {
                    return info;
                }
            }
            return null;
        }

        //通过匹配模式找地图
        public List<MapInfo> GetMapList(EBattleMatchType type)
        {
            List<MapInfo> mapList = new List<MapInfo>();

            foreach (MapInfo info in mMapList.Values)
            {
                bool bIs = false;
                switch (type)
                {
                    case EBattleMatchType.EBMT_Normal:
                        bIs = info.mIsNormal;
                        break;
                    case EBattleMatchType.EBMT_Ai:
                        bIs = info.mIsAI;
                        break;
                    case EBattleMatchType.EBMT_Rank:
                        bIs = info.mIsRank;
                        break;
                    case EBattleMatchType.EBMT_Train:
                        bIs = info.mIsTrain;
                        break;
                    case EBattleMatchType.EBMT_Dungeon:
                        bIs = info.mIsDungeon;
                        break;
                    case EBattleMatchType.EBMT_None:
                        bIs = true;
                        break;
                }
                if (bIs)
                {
                    mapList.Add(info);
                }
            }

            return mapList;
        }

        //裁入配置表
        public void ReadConfig(string xmlFilePath)
        {
            ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
            TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

            if (!xmlfile)
            {
                Debug.LogError(" error infos: 没有找到指定的xml文件：" + xmlFilePath);
            }

            mXmlDoc = new XmlDocument();
            mXmlDoc.LoadXml(xmlfile.text);

            XmlNodeList infoNodeList = mXmlDoc.SelectSingleNode("MapLoadCfg").ChildNodes;

            for (int i = 0; i < infoNodeList.Count; i++)//(XmlNode xNode in infoNodeList)
            {
                if ((infoNodeList[i] as XmlElement).GetAttributeNode("MapID") == null) continue;

                string mapId = (infoNodeList[i] as XmlElement).GetAttributeNode("MapID").InnerText;

                MapInfo mapInfo = new MapInfo();
                mapInfo.mId = Convert.ToUInt32(mapId);
                foreach (XmlElement xEle in infoNodeList[i].ChildNodes)
                {
                    #region 搜索
                    switch (xEle.Name)
                    {
                        case "LoadScene":
                            mapInfo.mLoadScene = Convert.ToString(xEle.InnerText);
                            break;
                        case "MiniMap":
                            mapInfo.mMiniMap = Convert.ToString(xEle.InnerText);
                            break;
                        case "NameCn":
                            mapInfo.mName = Convert.ToString(xEle.InnerText);
                            break;
                        case "ACameraPos":
                            mapInfo.mACameraPos = Convert.ToString(xEle.InnerText);
                            break;
                        case "BCameraPos":
                            mapInfo.mBCameraPos = Convert.ToString(xEle.InnerText);
                            break;
                        case "ShowPic":
                            mapInfo.mShowPic = Convert.ToString(xEle.InnerText);
                            break;
                        case "PlayerNum":
                            mapInfo.mPlayerNum = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "PlayerMode":
                            mapInfo.mPlayerMode = Convert.ToString(xEle.InnerText);
                            break;
                        case "CameraType":
                            mapInfo.mCameraType = Convert.ToInt32(xEle.InnerText);
                            break;
                        case "IsAI":
                            mapInfo.mIsAI = Convert.ToBoolean(Convert.ToInt32(xEle.InnerText));
                            break;
                        case "IsNormal":
                            mapInfo.mIsNormal = Convert.ToBoolean(Convert.ToInt32(xEle.InnerText));
                            break;
                        case "IsRank":
                            mapInfo.mIsRank = Convert.ToBoolean(Convert.ToInt32(xEle.InnerText));
                            break;
                        case "IsTrain":
                            mapInfo.mIsTrain = Convert.ToBoolean(Convert.ToInt32(xEle.InnerText));
                            break;
                        case "IsDungeon":
                            mapInfo.mIsDungeon = Convert.ToBoolean(Convert.ToInt32(xEle.InnerText));
                            break;
                        case "ShopID":
                            mapInfo.mShopID = Convert.ToInt32(xEle.InnerText);
                            break;
                    }
                    #endregion
                }
                mMapList.Add(mapInfo.mId, mapInfo);
            }
        }

        private Dictionary<uint, MapInfo> mMapList = new Dictionary<uint, MapInfo>();
        private XmlDocument mXmlDoc = null;
    }

    public class MapInfo
    {
        public uint mId;                 //地图ID
        public string mName;             //地图名
        public string mLoadScene;        //对应场景资源
        public string mMiniMap;        //对应小地图
        public string mACameraPos;       //基地爆炸相机移动目标点B
        public string mBCameraPos;      //基地爆炸相机移动目标点B
        public string mShowPic;         //ui地图
        public int mPlayerNum;       //总人数
        public string mPlayerMode;         //战争模式2v2
        public int  mCameraType;    //相机类型    1:斜45度  2:水平
        public bool mIsAI;            //是否人机
        public bool mIsNormal;        //是否普通
        public bool mIsRank;          //是否天梯
        public bool mIsTrain;         //是否新手
        public bool mIsDungeon;       //是否副本
        public int mShopID;
    }

}
