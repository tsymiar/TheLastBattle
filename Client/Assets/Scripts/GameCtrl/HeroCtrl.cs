using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using BlGame.GameEntity;
using System.Linq;
using BlGame;
using JT.FWW.GameData;
using BlGame.Resource;
using BlGame.Model;

namespace BlGame.Ctrl
{
    public class HeroCtrl : Singleton<HeroCtrl>
    {
        public void Enter()
        {
            HeroCtrl.Instance.SetSelectState(HeroCtrl.HeroSelectState.EnterSelect);
            EventCenter.Broadcast(EGameEvent.eGameEvent_HeroEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_HeroExit);

            if (mAudioSource != null)
                mAudioSource.Stop();
        }

		public void SetSelectState(HeroSelectState state){
			selectState = state;
			switch(state){
			case HeroSelectState.InitSelect:
				Init();
				break;
			case HeroSelectState.EnterSelect:
				EnterSelectHero();
				break;
			case HeroSelectState.RandomSelect:
				EnterRandomHero();
				break;
			case HeroSelectState.OutSelect:
				//Exit();
                Debug.LogError("HeroSelectState.OutSelect");
				break;
			}
		}

		public void AddPreSelectHero(uint index,int heroId){//玩家预选择英雄
			foreach(var item in PlayerManager.Instance.AccountDic.Values){
				if(index == item.GameUserSeat){
					if(heroSelectDic.ContainsKey(item)){
						heroSelectDic.Remove(item);
					}
					heroSelectDic.Add(item,heroId);
					break;
				}
			}

            EventCenter.Broadcast(EGameEvent.eGameEvent_HeroPreSelect);
			
        }

		public void AddRealSelectHero(uint index,int heroId){//玩家确定英雄
			foreach(var item in PlayerManager.Instance.AccountDic.Values){
                if (index != item.GameUserSeat){
                    continue;
                }
				if(heroSelectDic.ContainsKey(item)){
					heroSelectDic.Remove(item);
				}
                if (index == PlayerManager.Instance.LocalAccount.GameUserSeat)
                {
                    if (selectState == HeroSelectState.RandomSelect) {
                        EventCenter.Broadcast<int>(EGameEvent.eGameEvent_HeroRealSelect, heroId);
                    }
                    else {
                        PlayLocalSelectSound(heroId);
                    }
                }
				heroSureSelectDic.Add(item,heroId);
				heroSelectDic.Add(item,heroId);

				break;				
			}

            EventCenter.Broadcast(EGameEvent.eGameEvent_HeroAddSelect);
		}

        public void ReconnectPreSelect(int heroId)
        {
            EventCenter.Broadcast<int>(EGameEvent.eGameEvent_HeroReconnectPreSelect,heroId);
        }

       private void Init(){
			heroSelectDic = new Dictionary<Iplayer, int>();			
			heroSureSelectDic = new Dictionary<Iplayer, int>();	
			heroInfoList = new List<HeroSelectConfigInfo>();
			for(int i= 0 ;i < ConfigReader.HeroSelectXmlInfoDict.Count;i++)
			{
				#region 获得所有英雄信息
				HeroSelectConfigInfo item = ConfigReader.HeroSelectXmlInfoDict.ElementAt(i).Value;
                if (item.IsGuideHero == 1)
                    continue;
				heroInfoList.Add(item);
				#endregion
			}
            SortByBuyed();            
		}

        void SortByBuyed() {
            for (int i = 0; i < heroInfoList.Count; i++)
            {
                for (int j = 0; j < heroInfoList.Count - 1 - i; j++)
                {
                    HeroSelectConfigInfo infoA = heroInfoList[j];
                    HeroSelectConfigInfo infoB = heroInfoList[j + 1];
                    if (!GameUserModel.Instance.CanChooseHeroList.Contains(infoA.HeroSelectNum) && GameUserModel.Instance.CanChooseHeroList.Contains(infoB.HeroSelectNum))
                    {
                        HeroSelectConfigInfo temp = heroInfoList[j];
                        heroInfoList[j] = heroInfoList[j + 1];
                        heroInfoList[j + 1] = temp;
                    }
                }
            }
        }

		private void EnterSelectHero(){
			heroSelectDic.Clear();
			heroSureSelectDic.Clear(); 
		}
		
        
		private void EnterRandomHero()
        {
            //AudioClip clip = Resources.Load(AudioDefine.PATH_HERO_SELECT_COUNTDOWN) as AudioClip;
            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_HERO_SELECT_COUNTDOWN, ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;
            mAudioSource = AudioManager.Instance.PlayEffectAudio(clip);

            EventCenter.Broadcast(EGameEvent.eGameEvent_HeroEnterRandom);
		}
		
		//private void Exit(){
            //senlin
			//if(UISelectHero.Instance != null){
			//	UISelectHero.Instance.Clean();
			//}
            //LoadUiResource.LoadRes(GameMethod.GetUiCamera.transform, GameConstDefine.HeroSelectUI);
		//}


        private void PlayLocalSelectSound(int heroId) {
            HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo(heroId);
            string path = "Audio/HeroSelect/";
            path = path + info.HeroSelectSound;
            

            //AudioClip clip = (AudioClip) Resources.Load(path) as AudioClip;
            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;

            AudioSource source = AudioManager.Instance.PlayEffectAudio(clip);
        }


		public HeroCtrl(){
			SetSelectState(HeroSelectState.InitSelect);
		} 


        
		#region public
		//临时选择的英雄
		public Dictionary<Iplayer,int> heroSelectDic{
			get;
			private set;
		}

		public Dictionary<Iplayer,int> heroSureSelectDic{
			get;
			private set;
		}
		//所有英雄信息
		public List<HeroSelectConfigInfo> heroInfoList{
			get;
			private set;
		}

		public enum HeroSelectState{
			InitSelect,
			EnterSelect,
			RandomSelect,
			OutSelect
		}
		#endregion

		#region private
		HeroSelectState selectState = HeroSelectState.InitSelect ;
        AudioSource mAudioSource = null;
		#endregion
	}
}
