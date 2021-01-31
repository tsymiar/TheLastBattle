using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using UnityEngine;
using GameDefine;
using JT.FWW.Tools;
using BlGame.Resource;
using BlGame.Model;

namespace BlGame
{
    public class AudioManager
    {
        private static AudioManager instance = null;
        public static AudioManager Instance
        {
            get {
                if (instance == null) {
                    instance = new AudioManager();
                }
                return instance;
            }
        }

        public AudioSource LoopAudioSource
        {
            private set;
            get;
        }

        public AudioSource HeroMoveAudio
        {
            private set;
            get;
        }

        public AudioSource GuideVoice
        {
            private set;
            get;
        }

        public Queue<AudioSource> EffectAudioSourceQueue
        {
            private set;
            get;
        }

        public Queue<AudioSource> LongVoiceAudioSourceQueue
        {
            private set;
            get;
        }

        /// <summary>
        /// 各英雄台词播放
        /// </summary>
        public Dictionary<UInt64, AudioSource> HeroLinesAudioDict
        {
            private set;
            get;
        }

        /// <summary>
        /// 游戏击杀音效播放列表
        /// </summary>
        private List<AudioClip> GameKillClipList
        {
            set;
            get;
        }

        public AudioSource GameKillAudioSource
        {
            private set;
            get;
        }

        public AudioSource HeroGetMoneySource
        {
            private set;
            get;
        }

        private AudioManager()
        { 
            InitAudio();
        }

        private void InitAudio()
        {
            HeroLinesAudioDict = new Dictionary<UInt64, AudioSource>();
            GameKillClipList = new List<AudioClip>();
            EffectAudioSourceQueue = new Queue<AudioSource>(Settings.MaxEffectAudioSource);
            LongVoiceAudioSourceQueue = new Queue<AudioSource>(Settings.LongVoiceAudioSourceQueue);
            
            for (int im = 0; im < Settings.MaxEffectAudioSource; im++)
            {
                AudioSource ad = JxBlGame.Instance.gameObject.AddComponent<AudioSource>();
                ad.volume = Settings.AudioVolume;
                EffectAudioSourceQueue.Enqueue(ad);
            }

            for (int im = 0; im < Settings.LongVoiceAudioSourceQueue; im++)
            {
                AudioSource ad = JxBlGame.Instance.gameObject.AddComponent<AudioSource>();
                ad.volume = Settings.AudioVolume;
                LongVoiceAudioSourceQueue.Enqueue(ad);
            }
            HeroMoveAudio = JxBlGame.Instance.gameObject.AddComponent<AudioSource>();
            HeroMoveAudio.volume = Settings.AudioVolume;
            HeroMoveAudio.loop = true;
            HeroMoveAudio.playOnAwake = false;

            LoopAudioSource = JxBlGame.Instance.gameObject.AddComponent<AudioSource>();
            LoopAudioSource.loop = true;
            LoopAudioSource.playOnAwake = false;
            LoopAudioSource.volume = Settings.AudioVolume;

            GameKillAudioSource = JxBlGame.Instance.gameObject.AddComponent<AudioSource>();
            GameKillAudioSource.volume = Settings.AudioVolume;

            HeroGetMoneySource = JxBlGame.Instance.gameObject.AddComponent<AudioSource>();
            HeroGetMoneySource.volume = Settings.AudioVolume;


            GuideVoice = JxBlGame.Instance.gameObject.AddComponent<AudioSource>();
            GuideVoice.volume = Settings.AudioVolume;
        }

        public void OnUpdate()
        {
            if (!GameKillAudioSource.isPlaying && GameKillClipList.Count > 0)
            {
                AudioClip adClip = GameKillClipList[0];
                GameKillAudioSource.clip = adClip;
                GameKillAudioSource.Play();
                GameKillClipList.Remove(adClip);
            }
        }

        /// <summary>
        /// 播放英雄台词 
        /// </summary>
        /// <param name="sGUID"></param>
        /// 对应英雄GUID
        public void PlayHeroLinesAudio(UInt64 sGUID , AudioClip adClip)
        {
            if (!HeroLinesAudioDict.ContainsKey(sGUID))
            {
                AudioSource ad = JxBlGame.Instance.gameObject.AddComponent<AudioSource>();
                HeroLinesAudioDict.Add(sGUID, ad);
            }
            AudioSource adSource = HeroLinesAudioDict[sGUID];
            if (adSource.isPlaying)
            {
                return;
            }
            adSource.clip = adClip;
            adSource.Play();
        }

        /// <summary>
        /// 播放击杀音效
        /// </summary>
        /// <param name="adClip"></param>
        public void PlayGameKillAudio(AudioClip adClip)
        {
            GameKillClipList.Add(adClip);
        }


        private AudioClip GetMoney;
        /// <summary>
        /// 英雄获得金币音效
        /// </summary>
        /// <param name="adClip"></param>
        public void GetMoneyAudio()
        {
            if (GetMoney == null)
            {
                //GetMoney = GameObject.Instantiate(Resources.Load(AudioDefine.PATH_GETMONEY)) as AudioClip;

                ResourceUnit GetMoneyUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_GETMONEY, ResourceType.ASSET);
                GetMoney = GameObject.Instantiate(GetMoneyUnit.Asset) as AudioClip;                                    
            }
            HeroGetMoneySource.clip = GetMoney;
            if (HeroGetMoneySource.isPlaying)
            {
                return;
            }
            
            HeroGetMoneySource.Play();
        }

        public void PlatUnloadRuneAudio()
        {
            ResourceUnit res = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_UnloadRune, ResourceType.ASSET);
            AudioClip clip = res.Asset as AudioClip;
            AudioSource Audio = AudioManager.Instance.PlayEffectAudio(clip);
        }

        /// <summary>
        /// 播放背景音
        /// </summary>
        /// <param name="adClip"></param>
        public void PlayBgAudio(AudioClip adClip)
        {
            if (LoopAudioSource == null || adClip == null || (LoopAudioSource.clip != null && LoopAudioSource.clip.name == adClip.name))
            {
                return;
            }
            LoopAudioSource.clip = adClip;
            LoopAudioSource.Play();
        }

        /// <summary>
        /// 停止播放音频
        /// </summary>
        public void StopBgAudio()
        {
            if (LoopAudioSource == null)
            {
                return;
            }
            LoopAudioSource.Stop();
        }

        /// <summary>
        /// 播放效果音
        /// </summary>
        /// <param name="adClip"></param>
        /// <returns></returns>
        public AudioSource PlayEffectAudio(AudioClip adClip)
        {
            if (adClip == null)
            {
                return null;
            }
            AudioSource adio = EffectAudioSourceQueue.Dequeue();
            adio.clip = adClip;
            adio.volume = 1.0f;
            adio.Play();
            EffectAudioSourceQueue.Enqueue(adio);
            return adio;
        }

        public AudioSource PlayLongVoiceAudio(AudioClip adClip)
        {
            if (adClip == null)
            {
                return null;
            }
            AudioSource adio = LongVoiceAudioSourceQueue.Dequeue();
            adio.clip = adClip;
            adio.Play();
            LongVoiceAudioSourceQueue.Enqueue(adio);
            return adio;
        }
        
        /// <summary>
        /// 更改声音音量
        /// </summary>
        /// <param name="source"></param>
        /// <param name="value"></param>
        public void ChangeAudioVolume(AudioSource source, float value)
        {
            if (source == null || source.clip == null)
            {
                return;
            }
            source.volume = value / Settings.AudioVolume;
        }

        /// <summary>
        /// 播放主角声效
        /// </summary>
        /// <param name="adClip"></param>
        public void PlayHeroAudio(AudioClip adClip)
        {
            if (adClip == null || (HeroMoveAudio.clip != null && HeroMoveAudio.clip.name == adClip.name))
            {
                return;
            }
            HeroMoveAudio.clip = adClip;
            HeroMoveAudio.Play();
        }

        /// <summary>
        /// 停止音效
        /// </summary>
        public void StopHeroAudio()
        {
            if (HeroMoveAudio.clip != null)
            {
                HeroMoveAudio.Stop();
                HeroMoveAudio.clip = null;
            }
        }

        public void ChangeAudioVolume(float value)
        {
            Settings.AudioVolume = value;
            LoopAudioSource.volume = Settings.AudioVolume;
            foreach (AudioSource adio in EffectAudioSourceQueue)
            {
                adio.volume = Settings.AudioVolume;
            } 

            foreach (AudioSource adio in LongVoiceAudioSourceQueue)
            {
                adio.volume = Settings.AudioVolume;
            }
            
        }


        public void PlayGuideVoice(AudioClip clip) { 
          //  GuideVoice.clip
            if (clip == null)
                return;
            
            //if (GuideVoice.isPlaying) {
            //    GuideVoice.clip = null;
            //} 
            GuideVoice.clip = clip;
            GuideVoice.Play();
        }

        public void StopGuideVoice() {
            //if (GuideVoice == null || !GuideVoice.isPlaying)
            //    return;
            //GuideVoice.clip = null; 
        }

        public void CleanAudio() {
            HeroLinesAudioDict.Clear();
        }

        
        public void EnableVoice(bool enable) {
            if (LoopAudioSource.enabled == enable)
                return;
            LoopAudioSource.enabled = enable;
            if (enable) {
                ResourceUnit audioClipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.GetMapBgAudio((MAPTYPE)GameUserModel.Instance.GameMapID), ResourceType.ASSET);
                AudioClip clip = audioClipUnit.Asset as AudioClip; 
                AudioManager.Instance.PlayBgAudio(clip);
                LoopAudioSource.Play();
            }
        }

        public void EnableSound(bool enable) {
            if (HeroMoveAudio.enabled != enable)
            {
                if (enable)
                {
                    HeroMoveAudio.clip = null;
                }
                HeroMoveAudio.enabled = enable; 
            }

            if (GuideVoice.enabled != enable)
            {
                if (enable)
                {
                    GuideVoice.clip = null;
                }
                GuideVoice.enabled = enable; 
            }
            for(int i = 0;i < EffectAudioSourceQueue.Count;i++){
                if (EffectAudioSourceQueue.ElementAt(i).enabled != enable)
                {
                    if (enable)
                    {
                        EffectAudioSourceQueue.ElementAt(i).clip = null;
                    }
                    EffectAudioSourceQueue.ElementAt(i).enabled = enable; 
                }
            } 
            for(int i = 0;i < LongVoiceAudioSourceQueue.Count;i++){
                if (LongVoiceAudioSourceQueue.ElementAt(i).enabled != enable)
                {
                    if (enable)
                    {
                        LongVoiceAudioSourceQueue.ElementAt(i).clip = null;
                    }
                    LongVoiceAudioSourceQueue.ElementAt(i).enabled = enable; 
                }
            }
            
            for(int i = HeroLinesAudioDict.Count - 1;i >= 0;i--){
                if (HeroLinesAudioDict.ElementAt(i).Value.enabled != enable)
                {
                    if (enable)
                    {
                        HeroLinesAudioDict.ElementAt(i).Value.clip = null;
                    }
                    HeroLinesAudioDict.ElementAt(i).Value.enabled = enable; 
                }
            }
            if (GameKillAudioSource.enabled != enable)
            {
                if (enable)
                {
                    GameKillAudioSource.clip = null;
                }
                GameKillAudioSource.enabled = enable; 
            }

            if (HeroGetMoneySource.enabled != enable)
            {
                if (enable)
                {
                    HeroGetMoneySource.clip = null;
                }
                HeroGetMoneySource.enabled = enable; 
            }

            if (NGUITools.mAudioSource != null && NGUITools.mAudioSource.enabled != enable)
            {
                if (enable)
                {
                    NGUITools.mAudioSource.clip = null;
                }

                NGUITools.mAudioSource.enabled = enable;
            }
        }
    }
}
