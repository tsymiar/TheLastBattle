using UnityEngine;
using System.Collections;
using System;
using BlGame.GameEntity;
using BlGame.GameState;
namespace BlGame.Ctrl
{
    public class GamePlayCtrl : Singleton<GamePlayCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_GamePlayEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_GamePlayExit);
        }

        public void AskBattleInfo()
        {
            CGLCtrl_GameLogic.Instance.EmsgToss_AskBattleInfo();
        }

        public DateTime showaudiotimeold = System.DateTime.Now;
        private int ShopID = 0;

        internal void HpChange(GameEntity.Ientity entity)
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_EntityHpChange, entity);
        }

        internal void MpChange(GameEntity.Ientity entity)
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_EntityMpChange, entity);
        }

        internal void AskUseSkill(uint p)
        {
            CGLCtrl_GameLogic.Instance.EmsgToss_AskUseSkill((UInt32)p);
        }

        internal void UpdateSkillPriv(int skillId)
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_LocalPlayerPassiveSkills, skillId);
        }

        internal void OpenShop()
        {
            if (ShopID != 0)
                PlayerManager.Instance.LocalPlayer.OpenShop(ShopID);
        }

        internal void SetShopID(int p)
        {
            ShopID = p;
        }

        public void BattleDelayTimeBegin(long time)
        {
            EventCenter.Broadcast<long>(EGameEvent.eGameEvent_BattleTimeDownEnter, time);
        }
    }
}