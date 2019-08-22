using UnityEngine;
using System.Collections;
using System.Collections.Generic; 
using System;
using BlGame;
using BlGame.Model;

public class GameTimeData : Singleton<GameTimeData>
{
    int hour = 0;
    int min = 0;
    int sec = 0; 
    float deltaTime = 0;
    long TimeStart = 0;
    bool starCount = false;
    bool gameStart = false;
    public TimeSpan GetGameTime()
    {
        return gametTime;
    }

    public void OnUpdate() {
        if (!starCount)
            return;
        deltaTime += Time.deltaTime; 
        if (deltaTime > 1.0f)
        {
            long spanTime = CGLCtrl_GameLogic.Instance.GetNowTime() - TimeStart;

            long timeAdd = spanTime / 1000;
            hour = (int)timeAdd / 3600;
            min = (int)timeAdd / 60;
            sec = (int)timeAdd % 60;
            gametTime = new TimeSpan(hour, min, sec);  

            deltaTime = 0.0f;
        }
    }


    public void UpdatePlayTime(long time = 0)
    {
        TimeStart = time - GameUserModel.Instance.STCTimeDiff;

        long spanTime = CGLCtrl_GameLogic.Instance.GetNowTime() - TimeStart;

        long timeAdd = spanTime / 1000;
        hour = (int)timeAdd / 3600;
        min = (int)timeAdd / 60;
        sec = (int)timeAdd % 60;
        gametTime = new TimeSpan(hour, min, sec);  

        starCount = true;
        gameStart = true;
    }

    public void  StartCountTime() {
        starCount = true;
    }

    public void EndCountTime() {
        starCount = false;
        gameStart = false;
    }

    public bool GetStartTag() {
        return starCount && gameStart;
    }

    private TimeSpan gametTime;
}
