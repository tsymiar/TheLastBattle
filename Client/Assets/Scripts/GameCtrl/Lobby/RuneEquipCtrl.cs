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
    public class RuneEquipCtrl : Singleton<RuneEquipCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_RuneEquipWindowEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_RuneEquipWindowExit);
        }

        public RuneEquipCtrl()
        {
        }

        public void UnloadRune(int page, int pos)
        {
            RuneEquipModel.Instance.RemoveRune(page, pos);
        }
    }
}
