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
    public class RuneCombineCtrl : Singleton<RuneCombineCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_RuneCombineWindowEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_RuneCombineWindowExit);
        }

        public RuneCombineCtrl()
        {
        }
    }
}
