using UnityEngine;
using System.Collections.Generic;
using BlGame;
using JT.FWW.GameData;
using BlGame.GameData;
using BlGame.Network;
using LSToGC;
using System.IO;
using System.Linq;

namespace BlGame.Ctrl
{
    public class MarketRecommendCtrl: Singleton<MarketRecommendCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_MarketHeroListEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_MarketHeroListExit);
        }

        public MarketRecommendCtrl()
        { 
            // sort by hot buy,discounts ,new kinds
            //for (int i = 0; i < HeroWithSkinHelp.Instance().heroWithSkin.Count; i++)  
            //{
            //    int heroId = HeroWithSkinHelp.Instance().heroWithSkin.ElementAt(i).Key;
            //    heroList.Add(heroId);
            //    HeroConfigInfo info = ConfigReader.GetHeroInfo(heroId);
            //    if (info.heroKind.Contains(HeroConfigInfo.HeroSortType.AtkType))
            //    {
            //        heroSortByAtk.Add(heroId);
            //    }
            //    if (info.heroKind.Contains(HeroConfigInfo.HeroSortType.AssistType))
            //    {
            //        heroSortByAssist.Add(heroId);
            //    }
            //    if (info.heroKind.Contains(HeroConfigInfo.HeroSortType.MagicType))
            //    {
            //        heroSortByMagic.Add(heroId);
            //    }
            //    if (info.heroKind.Contains(HeroConfigInfo.HeroSortType.TankType))
            //    {
            //        heroSortByTank.Add(heroId);
            //    }
            //} 
        }

        public List<int> GetHeroList(RecommendType type)
        {  
            switch (type)
            { 
                case RecommendType.NewKinds:
                    return heroSortByNewKind;
                case RecommendType.HotBuy:
                    return heroSortByHotKind;
                case RecommendType.Discounts:
                    return heroSortByDiscounts;
            }
            return null;
        }

        public void SetSelectHero(int heroId) {
            mHeroSelect = heroId;
        }

        public int GetHeroSelect() {
            return mHeroSelect;
        }
          
        private List<int> heroSortByNewKind = new List<int>();
        private List<int> heroSortByHotKind = new List<int>();
        private List<int> heroSortByDiscounts = new List<int>();        
        private int mHeroSelect;

        public enum RecommendType
        { 
            NewKinds = 0,
            Discounts,
            HotBuy,
        }

    }
}
