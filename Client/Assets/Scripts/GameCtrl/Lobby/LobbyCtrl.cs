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
    public class LobbyCtrl : Singleton<LobbyCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_LobbyEnter);

            //请求队伍信息
            CGLCtrl_GameLogic.Instance.EmsgToss_RequestMatchTeamList();

            InitLobbyGuideUIInfo();
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_LobbyExit);
        }

        public void AskNewGMCmd(string cmd)
        {
            CGLCtrl_GameLogic.Instance.EmsgTocs_AddNewGMCmd(cmd);
        }

        /// <summary>
        /// 进入大厅的时候初始化引导的信息
        /// </summary>
        private void InitLobbyGuideUIInfo()
        {
            UIGuideCtrl.Instance.InitLobbyGuideInfo();
        }

        internal void AskPersonInfo()
        {
            CGLCtrl_GameLogic.Instance.PersonInfo();
        }


        internal void InviteInfo(ulong sGUID, string nickName)
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_InviteCreate, sGUID, nickName);
        }
    }
}
