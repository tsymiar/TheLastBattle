using System;
using System.Collections.Generic;
using System.Linq; 
using UnityEngine;
using GameDefine;
using BlGame;
using BlGame.Resource;

namespace BlGame.GuideDate

{
    public class GuideVoiceTask : GuideTaskBase
    { 
        private CVoiceTask voiceTask = null;

        public GuideVoiceTask(int task, GuideTaskType type, GameObject mParent)
            : base(task, type, mParent)
        { 
            //读取数据
            voiceTask = ConfigReader.GetVoiceTaskInfo(task);
            if (voiceTask == null) {
                Debug.LogError("GuideVoiceTask = "+task+"id 不存在");
            }
        }        

        public override void EnterTask()
        {
            //AudioClip clip = Resources.Load(voiceTask.VoicePath) as AudioClip;
            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(voiceTask.VoicePath, ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;


            AudioManager.Instance.PlayGuideVoice(clip);           
        }
         
        public override void ExcuseTask()
        {
            
        }

        public override void ClearTask()
        {            
            base.ClearTask();
            AudioManager.Instance.StopGuideVoice();
            voiceTask = null;
        }         
       
    }


}
