using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using UnityEngine;
using GameDefine;
using BlGame;
using BlGame.GameEntity;
using BlGame.Resource;

namespace BlGame.GuideDate

{
    public class PrimaryGuideTask : SceneGuideTaskBase
    {
        const int HeroVoiceCount = 3;
        const string Jinglingnan_voice = "Jinglingnan_5_line_02";
        const string Jinglingnv_voice = "Jinglingnv_5_line_02";
        const string Renma_voice = "Renma_5_line_01";
        const string ZhiZhu_voice = "Zhizhu_5_line_01";
        const string voicePath = "Audio/HeroSelect/";
        public override void InitSceneTaskData()
        {
            heroVoicDic.Clear();
        }

        public override void SetTaskStart(int step)
        {
            base.SetTaskStart(step);
           
        }

        Dictionary<int, AudioClip> heroVoicDic = new Dictionary<int, AudioClip>();


        public void AddHeroVoice(int npcId) {
            if (heroVoicDic.ContainsKey(npcId))
            {
                return ;
            } 
            string path = "";
            switch (npcId) { 
                case 19999:
                    path = voicePath + Jinglingnan_voice;
                    break;
                case 21017:
                    path = voicePath + Jinglingnv_voice;
                    break;
                case 21025:
                    path = voicePath + Renma_voice;
                    break;
                case 21024:
                    path = voicePath + ZhiZhu_voice;
                    break;
            }
            
            //AudioClip clip = GameObject.Instantiate(Resources.Load(path)) as AudioClip; 
            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;


            heroVoicDic.Add(npcId, clip);
            AudioManager.Instance.PlayLongVoiceAudio(clip);
        }

        private void CheckAddHeroBornVoice() {
            if (heroVoicDic != null && heroVoicDic.Count == 3)
                return;
            IEnumerable<Ientity> Items = GameMethod.GetEnemyItemListByRadius(PlayerManager.Instance.LocalPlayer, GameConstDefine.PlayerLockTargetDis);
            List<Ientity> ItemList = Items.ToList();
            if (ItemList == null || ItemList.Count == 0)
                return;
            for (int i = ItemList.Count - 1; i >= 0; i--) {
                Ientity entity = ItemList[i];
                if (SceneGuideTaskManager.Instance().IsFakeHero(entity.NpcGUIDType)) {
                   AddHeroVoice(entity.NpcGUIDType);
                }
            }
        }
        public override void OnUpdate()
        {
            base.OnUpdate();
            CheckAddHeroBornVoice();
        }
    }


}
