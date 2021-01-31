using UnityEngine;
using System.Collections;
using BlGame;
using JT.FWW.GameData;
using BlGame.GameData;
using BlGame.Network;
using LSToGC;
using System.IO;
using System.Linq;

namespace BlGame.Ctrl
{
    public class MarketCtrl : Singleton<MarketCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_MarketEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_MarketExit);
        }

    }
}
