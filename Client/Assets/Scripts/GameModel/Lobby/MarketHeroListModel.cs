using UnityEngine;
using System.Collections.Generic;
using BlGame;
using JT.FWW.GameData;
using BlGame.GameData;
using BlGame.Network;
using LSToGC;
using System.IO;
using System.Linq;
using GameDefine;

namespace BlGame.Model
{
    public class MarketHeroListModel : Singleton<MarketHeroListModel>
    {
        public Dictionary<int, MarketGoodsInfo> HeroListDict = new Dictionary<int, MarketGoodsInfo>();
        public void HeroListInfo(GSToGC.GoodsCfgInfo cfgInfo)
        {
            MarketGoodsInfo mGoodInfo;
            int gdid = cfgInfo.goodid;
            if (!HeroListDict.TryGetValue(gdid, out mGoodInfo))
            {
                mGoodInfo = new MarketGoodsInfo();
                HeroListDict.Add(gdid, mGoodInfo);
            }
            GSToGC.GoodsCfgInfo.CfgType cfg = (GSToGC.GoodsCfgInfo.CfgType)cfgInfo.cfg_type;
            if (cfg == GSToGC.GoodsCfgInfo.CfgType.CfgType_Discount)
            {
                mGoodInfo.DisCountConsume = cfgInfo.consume;
                foreach (var item in cfgInfo.consume)
                {
                    if ((GameDefine.ConsumeType)item.consumetype == GameDefine.ConsumeType.TypeGold)
                    {
                        mGoodInfo.RLGoldPrice = item.price;
                    }
                    else if ((GameDefine.ConsumeType)item.consumetype == GameDefine.ConsumeType.TypeDiamond)
                    {
                        mGoodInfo.RLDiamondPrice = item.price;
                    }
                }
            }
            else if (cfg == GSToGC.GoodsCfgInfo.CfgType.CfgType_Common)
            {
                mGoodInfo.CommonConsume = cfgInfo.consume;
                foreach (var item in cfgInfo.consume)
                {
                    if ((GameDefine.ConsumeType)item.consumetype == GameDefine.ConsumeType.TypeGold)
                    {
                        mGoodInfo.RLGoldPrice = item.price;
                    }
                    else if ((GameDefine.ConsumeType)item.consumetype == GameDefine.ConsumeType.TypeDiamond)
                    {
                        mGoodInfo.RLDiamondPrice = item.price;
                    }
                }
            }
            mGoodInfo.mGoodsId = gdid;
            mGoodInfo.ConfigType = cfg;
            HeroListDict[gdid] = mGoodInfo;
        }

        public void Clear()
        {
            foreach (var item in HeroListDict)
            {
                item.Value.CommonConsume.Clear();
                item.Value.DisCountConsume.Clear();
            }
            HeroListDict.Clear();
        }
        public List<int> GetHeroListTime(HeroType type)
        {
            List<int> HeroListTime = new List<int>();
            foreach (var item in ConfigReader.HeroBuyXmlInfoDict)
            {
                if (item.Value.Time == -1)
                    continue;
                if (type == HeroType.AllHero)
                {
                    HeroListTime.Add(item.Key);
                }else
                if (item.Value.HeroKind.Contains((int)type))
                {
                    HeroListTime.Add(item.Key);
                }
            }
            return HeroListTime;
        }
        /// <summary>
        /// 获取要显示的英雄列表
        /// </summary>
        /// <param name="type"></param>
        /// <returns></returns>
        public List<int> GetHeroList(HeroType type)
        {
            List<int> HeroList = new List<int>();
            foreach (var item in HeroListDict)
            {
                HeroBuyConfigInfo bugConfig;
                if (!ConfigReader.HeroBuyXmlInfoDict.TryGetValue(item.Key , out bugConfig))
                {
                    continue;
                }
                if (bugConfig.Time == -1)
                {
                    if (type == HeroType.AllHero)
                    {
                        HeroList.Add(item.Key);
                    }
                    else if (bugConfig.HeroKind.Contains((int)type))
                    {
                        HeroList.Add(item.Key);
                    }
                }
            }
            return HeroList;
        }

    }

    public class MarketGoodsInfo
    {
        public int mGoodsId;
        public GSToGC.GoodsCfgInfo.CfgType ConfigType;
        public List<GSToGC.GoodsCfgInfo.Consume> DisCountConsume = new List<GSToGC.GoodsCfgInfo.Consume>();
        public List<GSToGC.GoodsCfgInfo.Consume> CommonConsume;

        public int RLGoldPrice;
        public int RLDiamondPrice;
    }

} 

