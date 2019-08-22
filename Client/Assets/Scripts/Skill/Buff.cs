using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using BlGame.GameEntity;
namespace BlGame.Skill
{
    public class Buff
    {
        //buff的实例id
        public uint buffID
        {
            get;
            set;
        }
        //buff的typeid
        public uint buffTypeID
        {
            get;
            set;
        }
        //buff剩余时间
        public float buffTime
        {
            get;
            set;
        }

        public Ientity entity
        {
            get;
            set;
        }
        //buff总的时间
        //public float totalTime
        //{
        //    get;
        //    set;
        //}
        public Buff()
        {
        }
        public string getSpriteName()
        {

            BuffConfigInfo buffInfo = ConfigReader.GetBuffInfo(buffTypeID);
            return buffInfo == null ? "" : ConfigReader.GetBuffInfo(buffTypeID).IconRes;
        }
        public float getTotalTime()
        {
            BuffConfigInfo buffInfo = ConfigReader.GetBuffInfo(buffTypeID);
            return buffInfo == null ? 1000000 : ConfigReader.GetBuffInfo(buffTypeID).TotalTime;
        }
        public void Update()
        {
            buffTime -= Time.deltaTime;
        }
    }
}
