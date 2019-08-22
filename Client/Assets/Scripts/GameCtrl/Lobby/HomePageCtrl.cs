using UnityEngine;
using System.Collections;
using BlGame;
using JT.FWW.GameData;
using BlGame.GameData;
using BlGame.Network;
using LSToGC;
using System.IO;
using System.Linq;
using GameDefine;
using System;
using System.Collections.Generic;

namespace BlGame.Ctrl
{
    public class HomePageCtrl : Singleton<HomePageCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_HomePageEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_HomePageExit);
        }

        public void AskQuickPlay(int id, EBattleMatchType type)
        {
            CGLCtrl_GameLogic.Instance.AskMatchBattle(id, type);
            CGLCtrl_GameLogic.Instance.AskStartTeamMatch();
        }
    }
}
