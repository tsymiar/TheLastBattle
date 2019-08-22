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
    public class MarketRuneListModel : Singleton<MarketRuneListModel>
    {
        Dictionary<int, RuneGoodsInfo> RunecfgListDict = new Dictionary<int, RuneGoodsInfo>();
        Dictionary<uint, OneRuneInfo> buyedRuneInfoDict = new Dictionary<uint, OneRuneInfo>();
        public void Clear()
        {
            RunecfgListDict.Clear();
            buyedRuneInfoDict.Clear();
        }
        public void UpdateBuyedRuneInfo(uint commidityId, int num, long gottime)
        {
            if (num == 0)
            {
                buyedRuneInfoDict.Remove(commidityId);
            }
            else
            {
                OneRuneInfo sRuneInterInfo = null;
                buyedRuneInfoDict.TryGetValue(commidityId, out sRuneInterInfo);
                if (null == sRuneInterInfo)
                {
                    sRuneInterInfo = new OneRuneInfo();
                    sRuneInterInfo.gottime = gottime;
                    sRuneInterInfo.num = num;
                    buyedRuneInfoDict.Add(commidityId, sRuneInterInfo);
                }
                else
                {
                    sRuneInterInfo.gottime = gottime;
                    sRuneInterInfo.num = num;
                }
            }

            EventCenter.Broadcast<uint, int, long>(EGameEvent.eGameEvent_RuneBagUpdate, (uint)commidityId, num, gottime);
        }

        public Dictionary<uint, OneRuneInfo> GetBuyedRuneDict()
        {
            return buyedRuneInfoDict;
        }

        public RuneGoodsInfo GetGoodsIno(int id)
        {
            if (RunecfgListDict.ContainsKey(id))
            {
                return RunecfgListDict[id];
            }

            return null;
        }
        public Dictionary<int, RuneGoodsInfo> GetRuneCfgListDict()
        {
            return RunecfgListDict;
        }

        public OneRuneInfo GetOneRuneInfo(uint id)
        {
            if (buyedRuneInfoDict.ContainsKey(id))
            {
                return buyedRuneInfoDict[id];
            }

            return null;
        }

        public void AddRuneCfgListInfo(GSToGC.GoodsCfgInfo cfgInfo)
        {
            RuneGoodsInfo mGoodInfo;
            int gdid = cfgInfo.goodid;
            if (!RunecfgListDict.TryGetValue(gdid, out mGoodInfo))
            {
                mGoodInfo = new RuneGoodsInfo();
                mGoodInfo.mGoodsId = gdid;
                mGoodInfo.RLDiamondPrice = -1;
                mGoodInfo.RLGoldPrice = -1;

                for (int i = 0; i < cfgInfo.consume.Count; ++i )
                {
                    if (cfgInfo.consume[i].consumetype == 2)
                    {
                        mGoodInfo.RLDiamondPrice = cfgInfo.consume[i].price;
                    }
                    else
                    {
                        mGoodInfo.RLGoldPrice = cfgInfo.consume[i].price;
                    }
                }
                RunecfgListDict.Add(gdid, mGoodInfo);
            }
        }
    }
} 

