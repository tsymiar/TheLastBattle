using System;
using System.Collections.Generic;
using UnityEngine;
using GameDefine;
using BlGame.GameEntity;
using JT.FWW.Tools;
using System.Linq;
using BlGame.Model;

namespace BlGame.GuideDate
{
    public class IGuideMidMatchTip : Singleton<IGuideMidMatchTip>
    {
        private const string path = "Guide/TPS1"; 
        private const float timeLimit = 5f;
        GameObject objTip = null; 
        int count = 0;
        public void RegisterListerner()
        {
            if ((MAPTYPE)GameUserModel.Instance.GameMapID == MAPTYPE.MIDDLEMAP)
            {
                EventCenter.AddListener<long>(EGameEvent.eGameEvent_BattleTimeDownEnter, OnEvent);
                EventCenter.AddListener(EGameEvent.eGameEent_ScenseChange, ChangeScense);
            }
        }   

        public void OnUpdate() {


           if (objTip != null) {
                TimeSpan span = GameTimeData.Instance.GetGameTime(); 
                if (span.TotalSeconds >= timeLimit) { 
                    LoadUiResource.DestroyLoad(path);
                    objTip = null;
                }
            }
        }

        void OnEvent(long i) {
            EventCenter.RemoveListener<long>(EGameEvent.eGameEvent_BattleTimeDownEnter, OnEvent);    
            TimeSpan span = GameTimeData.Instance.GetGameTime();

            if (span.TotalSeconds < timeLimit)
            {
                objTip = LoadUiResource.LoadRes(GameMethod.GetUiCamera.transform, path);
            }
        }

        void ChangeScense() {
            EventCenter.RemoveListener(EGameEvent.eGameEent_ScenseChange, ChangeScense);
            if (objTip != null)
            {
                LoadUiResource.DestroyLoad(path);
            }
        }
    }
}
