using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using BlGame.GameEntity;
using System;
using System.Linq;
namespace BlGame.GameData
{
    public class PackageData
    {

        private static PackageData instance = null;
        public static PackageData Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new PackageData();
                }
                return instance;
            }
            
        }
        public Iplayer Player
        {
            private set;
            get;
        }
        public int Seat
        {
            private set;
            get;
        }
        public int Id
        {
            private set;
            get;
        }
        public int Num
        {
            private set;
            get;
        }
        public float TotalTime
        {
            private set;
            get;
        }
        public float LastTime
        {
            private set;
            get;
        }
        public void SetPackageData(Iplayer player, int seat, int id, int num, float totalTime, float lastTime)
        {
            Player = player;
            Seat = seat;
            Id = id;
            Num = num;
            TotalTime = totalTime;
            LastTime = lastTime;
        }
        // Use this for initialization
        void Start()
        {

        }
        
        // Update is called once per frame
        void Update()
        {
            
        }
        
    }
}
