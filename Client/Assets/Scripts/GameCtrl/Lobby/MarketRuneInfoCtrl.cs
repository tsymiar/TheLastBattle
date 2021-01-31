using UnityEngine;
using System.Collections;
using BlGame;
using JT.FWW.GameData;
using BlGame.GameData;
using BlGame.Network;
using LSToGC;
using System.IO;
using System.Linq;
using System;
using BlGame.Model;

namespace BlGame.Ctrl
{
    public class MarketRuneInfoCtrl : Singleton<MarketRuneInfoCtrl>
    {
        public void Enter(GameObject go)
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_RuneBuyWindowEnter, go);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_RuneBuyWindowExit);
        }

        public void BuyRune(int runeid, GameDefine.ConsumeType type, int num)
        {
            CGLCtrl_GameLogic.Instance.EMsgToGSToCSFromGC_AskBuyGoods(runeid, (int)type, num);
        }
    }
}
