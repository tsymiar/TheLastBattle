using UnityEngine;
using System.Collections;
using BlGame.GameEntity;

namespace BlGame.GameData
{
    public class PlayerPersonData
    {
        private static PlayerPersonData instance = null;
        public static PlayerPersonData Instance
        {
            get {
                if (instance == null)
                {
                    instance = new PlayerPersonData();
                }
                return instance;
            }
        }
        public Iplayer Player
        {
            private set;
            get;
        }
        public int DeathTime
        {
            private set;
            get;
        }
        public int CampID
        {
            private set;
            get;
        }

        public void SetDeathTime(Iplayer player, int deathTime)
        {
            Player = player;
            DeathTime = deathTime;
        }
        public void SetPersonInfo(Iplayer player, int campId)
        {
            Player = player;
            CampID = campId;
        }
      
    }
}
