using UnityEngine;
using System.Collections;
using BlGame.Model;
using System;

namespace BlGame.Ctrl
{
    public class PresonInfoCtrl : Singleton<PresonInfoCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_PresonInfoEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_PresonInfoExit);
        }

        public void ChangeNickName()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_ChangeNickName);
        }
        public void ChangeHeadID()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_ChangeHeadID);
        }
        public void SetHeadID(ulong sGUID, uint headID)
        {
            GameUserModel.Instance.SetGameUserGUID(sGUID); mSGUID = sGUID;
            GameUserModel.Instance.SetGameUserHead((int)headID);
        }

        public void ChangeHead(uint headID)
        {
            CGLCtrl_GameLogic.Instance.EmsgToss_AskChangeheaderId(headID);
        }

        public void ChangeNickName(string nickName)
        {
            CGLCtrl_GameLogic.Instance.EmsgToss_AskChangeNickName(nickName);
        }

        public void SetNickName(ulong sGUID, string nickName)
        {
            GameUserModel.Instance.SetGameUserGUID(sGUID); mSGUID = sGUID;
            GameUserModel.Instance.SetGameUserNick(nickName);
        }

        public void SetCurrentDate(int headID, string pszNickName, int UserLv, int UserTotalExp, int UserCurLvExp,
                              int TotalGameInns, int TotalWinInns, int HeroKills, int BuidingDemolishNum,
                              int DeadTimes, int MaxAchNum, int AchNum, int assist, int integral,   ulong expTime, ulong goldTime)
        {
            GameUserModel.Instance.SetGameUserHead(headID);
            mGameInning = TotalGameInns;
            GameUserModel.Instance.SetGameUserLv((short)UserLv);
            mWinInning = TotalWinInns;
            mKillAll = HeroKills;
            mDestrucBuild = BuidingDemolishNum;
            mDeathAll = DeadTimes;
            mAchievement = AchNum.ToString();
            mMaxAchievement = MaxAchNum.ToString();
            AchNum = 0;
            if (AchNum <= 0)
            {
                mAchievement = "N";
                mMaxAchievement = "A";
            }
            mAssistAll = assist;
            mIntegral = integral;
            isTimeExtRun = (expTime != 0);
            isTimeGoldRun = (goldTime != 0);
            mExpTime = expTime;
            mGoleTime = goldTime;
            mExpHours = Mathf.Abs(GetHour(expTime));
            mExpMinute = Mathf.Abs(GetMinute(expTime));
            mExpSecond = Mathf.Abs(GetSecond(expTime));

            mGoldHours = Mathf.Abs(GetHour(goldTime));
            mGoldMinute = Mathf.Abs(GetMinute(goldTime));
            mGoldSecond = Mathf.Abs(GetSecond(goldTime));
        }

        int GetSecond(ulong time)
        {
            return (int)time % 60;
        }

        public int GetMinute(ulong time)
        {
            return (int)time / 60 % 60;
        }

        int GetHour(ulong time)
        {
            return (int)time / 3600;
        }

        ulong mSGUID;
        public string mBelongGuild;//所属公会

        public int mExp;
        public int mGameInning;//游戏总局数
        public int mWinInning;// 胜利总局数
        public int mKillAll;//杀人总数
        public int mDestrucBuild;//破坏建筑
        public int mDeathAll;// 死亡总数
        public string mAchievement;//成就数
        public string mMaxAchievement;
        public int mAssistAll;//助攻
        public int mIntegral;//积分
        public int mVipLevel;
        public int mExpHours;
        public int mExpMinute;
        public int mExpSecond;

        public int mGoldHours;
        public int mGoldMinute;
        public int mGoldSecond;

        bool isTimeExtRun;
        bool isTimeGoldRun;
        ulong mExpTime;
        ulong mGoleTime;
        int CurrExpMinute;
        int CurrGoldMinute;
        public bool GetExpLastMin()
        {
            if (CurrExpMinute == GetMinute(mExpTime))
                return false;
            return true;
        }

        public bool GetGoldLastMin()
        {
            if (CurrGoldMinute == GetMinute(mGoleTime))
                return false;
            return true;
        }


        public bool GetTimeExp()
        {
            return isTimeExtRun;
        }
        public bool GetTimeGold()
        {
            return isTimeGoldRun;
        }
        public void SetTimeExp(bool isTrue)
        {
             isTimeExtRun = isTrue ;
        }
        public void SetTimeGold(bool isTrue)
        {
            isTimeGoldRun = isTrue;
        }
    }
}
