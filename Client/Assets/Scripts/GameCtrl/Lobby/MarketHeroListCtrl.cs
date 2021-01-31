using UnityEngine;
using System.Collections.Generic;
using BlGame;
using JT.FWW.GameData;
using BlGame.GameData;
using BlGame.Network;
using LSToGC;
using System.IO;
using System.Linq;

using BlGame.Model;

namespace BlGame.Ctrl
{
    public class 
        MarketHeroListCtrl : Singleton<MarketHeroListCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_MarketHeroListEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_MarketHeroListExit);
        }

        /// <summary>
        /// 刷新物品信息
        /// </summary>
        //public void NotifyMarketGoodsRes(GSToGC.NotifyGoodsRes pMsg)
        //{
            //if(pMsg.res)
        //}

        public MarketHeroListCtrl() {
           
        }

        public void SetRootGoods(int goodId) 
        {
            mRootGood = goodId;
        }

        public void SetSelectGoods(int goodId) {
            mGoodsSelect = goodId;
        }

        public int GetRootGoods()
        {
            return mRootGood;
        }
        
        public int GetGoodsSelect() {
            return mGoodsSelect;
        }


        /// <summary>
        /// 请求购买英雄
        /// </summary>
        /// <param name="goodsId"></param>
        /// <param name="tp"></param>
        public void MarketHeroAskBuyGoods(int goodsId, GameDefine.ConsumeType tp)
        {
            CGLCtrl_GameLogic.Instance.EMsgToGSToCSFromGC_AskBuyGoods(goodsId, (int)tp);
        }
         
        //private List<int> HeroList = new List<int>();

        private int mGoodsSelect;
        private int mRootGood;
    }
}
