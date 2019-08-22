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
    public class MarketRuneListCtrl : Singleton<MarketRuneListCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_MarketRuneListEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_MarketRuneListExit);
        }

        public enum ConsumeType
        {
            TypeGold = 1,
            TypeDiamond,
        }


        public MarketRuneListCtrl()
        {
        }

        public void SetSelectGoods(int runeId) {
            mGoodsSelect = runeId;
        }

        public int GetGoodsSelect() {
            return mGoodsSelect;
        }

        public void MarketHeroAskBuyRunes(int goodsId , ConsumeType tp)
        {
            CGLCtrl_GameLogic.Instance.EMsgToGSToCSFromGC_AskBuyGoods(goodsId, (int)tp);
        }

        public void UpdateRuneBagInfo(uint runeID, int num, long gottime)
        {
            MarketRuneListModel.Instance.UpdateBuyedRuneInfo(runeID, num, gottime);
        }
        private int mGoodsSelect;
    }
}
