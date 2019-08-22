using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
public class DailyBonusData  {

    private static DailyBonusData instance = null;
    public static DailyBonusData Instance
    {
        get { 
            if(instance == null)
            {
                instance = new DailyBonusData();
            }
            return instance;
        }
    }
    public MaterDailyType DailType
    {
        private set;
        get;
    }
    public int NumOrID
    {
        private set;
        get;
    }
    public int EverydaySignIn
    {
        private set;
        get;
    }
    public bool IsHaveReward
    {
        private set;
        get;
    }
    //每天签到消息
    public void SetTodaySignIn(int everydaySignIn, MaterDailyType dailType, int numID/**/)
    {
        EverydaySignIn = everydaySignIn;
        DailType = dailType;
        NumOrID = numID;
    }
    public void SetSighIn(int everydaySignIn,bool isHaveReward)
    {
        EverydaySignIn = everydaySignIn;
        IsHaveReward = isHaveReward;
    }
    public void SetHaveReward(bool isVab)
    {
        IsHaveReward = isVab;
    }
}
