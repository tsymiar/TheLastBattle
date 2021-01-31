using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

using GameDefine;
using JT.FWW.Tools;
using BlGame.GameData;
using BlGame.Model;
using GameDefine;
using System;

namespace BlGame.GameEntity
{
	public class PlayerManager : EntityManager
	{
		public static new PlayerManager Instance {
			private set;
			get;
		}

        public Dictionary<UInt64, Iplayer> AccountDic = new Dictionary<UInt64, Iplayer>();//UInt64 masterId

		public PlayerManager(){
			Instance = this;
		}

		public Iselfplayer LocalPlayer {
			set;
			get;
		}		 

		public Iplayer LocalAccount{
			set;
			get;
		}
        public SITUATION StateSituationOb
        {
            set;
            get;
        }
		public override Ientity HandleCreateEntity (UInt64 sGUID , EntityCampType campType){//entity id		
            Iplayer player;
            if (GameUserModel.Instance.IsLocalPlayer(sGUID))
            {
                player = new Iselfplayer(sGUID, campType);                
            }
            else
            {
                player =  new Iplayer(sGUID, campType);
            }

            player.GameUserId = sGUID;
            return player;
		}

        public void AddAccount(UInt64 sGUID, Iplayer entity)
		{
			if (AccountDic.ContainsKey (sGUID)) {
				Debug.LogError("Has the same Guid: " + sGUID)	;		
				return;
			}
			AccountDic.Add (sGUID , entity);
		}

        public override void SetCommonProperty(Ientity entity, int id)
        {
            base.SetCommonProperty(entity, id);
            HeroConfigInfo info = ConfigReader.GetHeroInfo(id);
            entity.ColliderRadius = info.HeroCollideRadious / 100;
            Iplayer mpl = (Iplayer)entity;
            if (mpl.GameUserNick == "" || mpl.GameUserNick == null)
            {
                mpl.GameUserNick = RandomNameData.Instance.GetRandName();
            }
        }

		protected override string GetModeName (int id)
		{
			return ConfigReader.GetHeroInfo(id).HeroName;
		}

        //protected override void AddEntityComponent (Ientity entity){
        //    Entity syncEntity = (Entity)entity.realObject.AddComponent ("Entity");
        //    entity.RealEntity = syncEntity;
        //    entity.realObject = syncEntity.gameObject;
        //    syncEntity.SyncEntity = entity;
        //    syncEntity.CampType = entity.entityCamp;
        //}

		public bool IsLocalSameType(Ientity entity){
			if(PlayerManager.Instance.LocalPlayer.EntityCamp != entity.EntityCamp)
				return false;
			return true;
		}

		public void CleanAccount(){
			for (int i = AccountDic.Count - 1; i >= 0; i--) {
				if (GameUserModel.Instance.IsLocalPlayer (AccountDic.ElementAt(i).Value.GameObjGUID))
					continue;	
				AccountDic.Remove (AccountDic.ElementAt(i).Key);
			}					 
		}

		public void RemoveAccountBySeat(uint seat){
			for (int i = AccountDic.Count - 1; i >= 0; i--) {
				if (AccountDic.ElementAt(i).Value.GameUserSeat != seat)
					continue;	
				AccountDic.Remove (AccountDic.ElementAt(i).Key);
				break;
			}					 
		}

        public void CleanPlayerWhenGameOver() {
            foreach (var item in AccountDic.Values) 
            { 
                item.CleanWhenGameOver();
            }
        }

        //public override GameObject CreateEntityModel(Ientity entity, UInt64 sGUID, Vector3 dir, Vector3 pos)
        //{
            //return base.CreateEntityModel(entity, sGUID, dir, pos);
        //}
	}
}