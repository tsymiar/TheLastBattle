using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using JT.FWW.Tools;
using GameDefine;
using BlGame.View;

namespace BlGame.Model
{
    public class GameUserModel : Singleton<GameUserModel>
    {
        public UInt64 GameUserGUID
        {
            get;
            private set;
        }

        public UInt64 GameRoomID
        {
            get;
            set;
        }

        public UInt32 GameMapID
        {
            get;
            set;
        }

        public UInt64 GameBattleID
        {
            get;
            set;
        }

        public UInt64 GameScore
        {
            get;
            private set;
        }

        //ss和本地时差
        public Int64 STCTimeDiff
        {
            get;
            set;
        }

        /// <summary>
        /// 帐号当前钻石
        /// </summary>
        public UInt64 mGameUserDmd
        {
            private set;
            get;
        }

        public string mGameUserName
        {
            get;
            private set;
        }

        public string GameUserNick
        {
            get;
            private set;
        }
        public void SetGameUserNick(string nickName)
        {
            GameUserNick = nickName;
        }
        public void SetGameUserHead(int headid)
        {
            GameUserHead = headid;
        }

        public byte GameUserSex
        {
            get;
            private set;
        }

        public Int32 GameUserHead
        {
            get;
            private set;
        }

        public void Clear()
        {
            CanChooseHeroList.Clear();
            OwnedHeroInfoDict.Clear();
        }

        /// <summary>
        /// 帐号当前的金币
        /// </summary>
        public UInt64 mGameUserGold
        {
            private set;
            get;
        }

        public Int16 UserLevel
        {
            private set;
            get;
        }
        public bool IsReconnect
        {
            set;
            get;
        }

        public const char sexBoy = '1';
        public const char sexGirl = '2';

        public Dictionary<int, CommodityInfos> OwnedHeroInfoDict = new Dictionary<int, CommodityInfos>();

        public void OnChangGold(UInt64 curgold)
        {
            mGameUserGold = curgold;
        }
        public void ChangeReconnect(bool isvib)
        {
            IsReconnect = isvib;
        }
        /// <summary>
        /// 帐号当前钻石改变
        /// </summary>
        /// <param name="value"></param>
        public void GameUserCurDiamond(UInt64 value)
        {
            if (mGameUserDmd != 0 && value > mGameUserDmd)
            {

            }
            mGameUserDmd = value;
            EventCenter.Broadcast(EGameEvent.eGameEent_ChangeMoney);
        }

        /// <summary>
        /// 游戏帐号当前金币
        /// </summary>
        /// <param name="num"></param>
        public void GameUserCurGold(UInt64 value)
        {
            mGameUserGold = value;
            EventCenter.Broadcast(EGameEvent.eGameEent_ChangeMoney);
        }

        public List<int> CanChooseHeroList = new List<int>();
        public void SetGameBaseInfo(GSToGC.UserBaseInfo pMsg)
        {
            GameScore = (ulong)pMsg.curscore;
            mGameUserDmd = (UInt64)pMsg.curdiamoand;
            mGameUserGold = (UInt64)pMsg.curgold;
            mGameUserName = pMsg.name;
            GameUserNick = pMsg.nickname;
            IsReconnect = pMsg.ifreconnect;
            GameUserGUID = pMsg.guid;
            GameBattleID = pMsg.battleid;
            GameUserSex = (byte)pMsg.sex;
            GameUserHead = pMsg.headid;
            UserLevel = (short)pMsg.level;
            GameUserExp = pMsg.cur_exp;
            GameUserVipLevel = pMsg.vip_level;
            GameUserVipIntegral = pMsg.vip_score;
            
        }
        public int GameUserExp
        {
            private set;
            get;
        }
        public int GameUserVipLevel
        {
            private set;
            get;
        }
        public int GameUserVipIntegral
        {
            private set;
            get;
        }

        public bool IsLocalPlayer(UInt64 sGUID)
        {
            return sGUID == GameUserGUID;
        }

        public void SetGameUserLv(Int16 lv)
        {
            UserLevel = lv;
            EventCenter.Broadcast(EGameEvent.eGameEvent_ChangeUserLevel);
        }

        /// <summary>
        /// 刷新帐号拥有的英雄信息数据
        /// </summary>
        /// <param name="heroid"></param>
        /// <param name="info"></param>
        public void DeltGetHeroInfoData(CommodityInfos info)
        {
            if (info.Expired_time > 0)
            {
                int sDay = Mathf.Abs(GetDay(info.Expired_time));
                int sHour =Mathf.Abs(GetHour(info.Expired_time));
                int sMin = Mathf.Abs(GetMinute(info.Expired_time));
                int sSec = Mathf.Abs(GetSecond(info.Expired_time));
                info.gametTime = new TimeSpan(sDay, sHour, sMin, sSec);
                info.LastTime = DateTime.Now;
                info.LastTime += info.gametTime;
            }
            if (this.OwnedHeroInfoDict.ContainsKey(info.goodsId))
            {
                OwnedHeroInfoDict[info.goodsId] = info;
            }
            else
            {
                OwnedHeroInfoDict.Add(info.goodsId, info);
                EventCenter.Broadcast<int>(EGameEvent.eGameEvent_CsGetNewHero, info.goodsId);
            }
            
            EventCenter.Broadcast<int>(EGameEvent.eGameEvent_RefreshGoodHero, info.goodsId);
            EventCenter.Broadcast<int>(EGameEvent.eGameEvent_RefreshTimeLimitHero, info.goodsId);
        }

        /// <summary>
        /// 英雄到期或者销毁
        /// </summary>
        /// <param name="goosId"></param>
        public void DeleteHero(int goosId)
        {
            if (OwnedHeroInfoDict.ContainsKey(goosId))
            {
                OwnedHeroInfoDict.Remove(goosId);
            }
        }

        int GetSecond(long time)
        {
            return (int)time % 60;
        }

        int GetMinute(long time)
        {
            return (int)time / 60 % 60;
        }

        int GetHour(long time)
        {
            return (int)time  / 3600 % 24;
        }

        int GetDay(long time)
        {
            return (int)time / 3600  / 24;
        }

        /// <summary>
        /// 当前获得的商品的类型
        /// </summary>
        public MarketGoodsType NewCommodityType
        {
            private set;
            get;
        }

        /// <summary>
        /// 当前获得的商品的Id
        /// </summary>
        public int NewCommodityId
        {
            private set;
            get;
        }


        /// <summary>
        /// 帐号获得新的物品
        /// </summary>
        /// <param name="Cmdityid"></param>
        public void GameUserGetNewCommodity(int Cmdityid)
        {
            Int64 expiredTime = -1;
            if (GameDefine.GameConstDefine.IfHeroBuyCfg(Cmdityid))
            {
                if (ConfigReader.HeroBuyXmlInfoDict.ContainsKey(Cmdityid))
                {
                    NewCommodityType = MarketGoodsType.GoodsTypeHero;
                    expiredTime = (Int64)ConfigReader.HeroBuyXmlInfoDict[Cmdityid].Time;
                }
            }
            else if (GameDefine.GameConstDefine.IfRuneBuyCfg(Cmdityid))
            {
                //EventCenter.Broadcast<EMessageType>(EGameEvent.eGameEvent_ShowMessage, EMessageType.EMT_BuyGoodsSuccess);
                //MarketRuneListModel.Instance.UpdateBuyedRuneInfo(Cmdityid, 0);
            }

            CommodityInfos info = new CommodityInfos()
            {
                goodsId = Cmdityid,
                GoodsType = NewCommodityType,
                Expired_time = expiredTime
            };

            switch (NewCommodityType)
            {
                case MarketGoodsType.GoodsTypeHero: 
                    this.DeltGetHeroInfoData(info); 
                    break;
            }
            //OwnedHeroInfoDict
            NewCommodityId = Cmdityid; 
            //EventCenter
        }

        public void SetGameUserGUID(ulong sGUID)
        {
            GameUserGUID = sGUID;
        }

        public void SetGameUserExp(int mExpValue)
        {
            GameUserExp = mExpValue;
        }
    }

    public class CommodityInfos : System.Object
    {
        public DateTime LastTime;
        public int goodsId;
        public Int64 Expired_time;
        public bool If_free;
        public MarketGoodsType GoodsType;
        public TimeSpan gametTime;
    }

    public class RuneBuyedInfo :System.Object
    {
        public int commidityID;
        public int num;
    }

    public enum MarketGoodsType
    {
        GoodsTypeHero = 1,
        GoodsTypeGoods,
    }
}