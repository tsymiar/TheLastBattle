using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using BlGame;
 
public class TalkGame  : Singleton<TalkGame>
{
      public bool TalkGameSwitch = true;   //talkgame开关

      public TDGAAccount account;

      public string appid = "291CD0D31B7B4470F35147075E2090C7";

      public void InitTalkGame(string sdk, string cdkey,   string serverIp)
    {
        if (TalkGameSwitch)
        {
            sdk = sdk.Replace("ePlatform_", "");

            TalkingDataGA.OnStart(appid, sdk);
            account = TDGAAccount.SetAccount(cdkey);

            if (account != null)
            {
                account.SetGameServer(serverIp);
            }
        }
    }

      public void UnInitTalkGame()
    {
        if (TalkGameSwitch)
        {
            TalkingDataGA.OnEnd();
        }
    }
}

 
