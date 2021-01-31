using System;
using System.Collections.Generic;
using UnityEngine;
using GameDefine;
using BlGame.GameEntity;
using JT.FWW.Tools;
using System.Linq;
namespace BlGame.GuideDate
{
    public class IGuideTriggerTaskEffect : IGuideTaskEffect
    {

        private const double ShowTime = 10f;
        private DateTime startTime;
        private bool canCheckTriggerEnd = false;
        
        protected override void ShowTextTip()
        {
            base.ShowTextTip();
            startTime = DateTime.Now;
            canCheckTriggerEnd = true; 
        }

        public IGuideTriggerTaskEffect(IGuideManagerData parentData, int id) : base(parentData,id)
        {
            pData = parentData;
            taskId = id;
            taskData = ConfigReader.GetIGuideInfo(taskId);
        }

        public override void OnExecute()
        {            
            if (pData == null || !pData.IsTriggerTask || !canCheckTriggerEnd)
                return;
            bool endTask = false;
            if (Application.platform == RuntimePlatform.Android
                || Application.platform == RuntimePlatform.IPhonePlayer
                || Application.platform == RuntimePlatform.WP8Player
                || Application.platform == RuntimePlatform.BB10Player)
            {
                Touch touch = Input.GetTouch(0);
                endTask = (touch.phase == TouchPhase.Began);

            }
            else
            {
                endTask = Input.GetMouseButtonDown(0);
            }

            if (!endTask)
            {
                TimeSpan span = DateTime.Now - startTime;
                if (span.TotalSeconds >= ShowTime)
                {
                    endTask = true;
                }
            }

            if (endTask)
            {
                canCheckTriggerEnd = false;
                IGuideTaskManager.Instance().SendTaskEnd((EGameEvent)taskData.EndTaskEvent);
                
            }
        }

    }
}
