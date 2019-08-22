using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BlGame.GameEntity;

using JT.FWW;
using JT.FWW.Tools;
using BlGame.GameData;
using JT.FWW.GameData;
using GameDefine;
using BlGame.FSM;

using BlGame.Effect;
using BlGame;

namespace BlGame.Skill
{
    class BuffManager
    {
        public delegate void OnStopBuffAdd(UInt64 key);
        public enum eBuffEffect
        {
            eBuffEffectXuanYun = 16,
            eBuffEffectShuFu = 22,
        }
        public static readonly BuffManager Instance = new BuffManager();
        public Dictionary<uint, Buff> buffDict = new Dictionary<uint, Buff>();
        private BuffManager()
        {
        }
        //主角是否有不能移动的buff

        public bool isHaveStopBuff(UInt64 key)
        {
            foreach (Buff b in buffDict.Values)
            {

                if (b == null || b.entity == null || b.entity.GameObjGUID != key)
                {
                    continue;
                }
                BuffConfigInfo bi = ConfigReader.GetBuffInfo(b.buffTypeID);
                if (null == bi)
                {
                    continue;
                }

                if (bi.effectID == (int)eBuffEffect.eBuffEffectXuanYun || bi.effectID == (int)eBuffEffect.eBuffEffectShuFu)
                {
                    return true;
                }
            }
            return false;
        }
        //
        public bool isSelfHaveBuffType(int typeID)
        {
            foreach (Buff b in buffDict.Values)
            {
                if (b == null || b.entity == null)
                {
                    continue;
                }
                if (b.buffTypeID == typeID && PlayerManager.Instance.LocalPlayer.ObType == ObPlayerOrPlayer.PlayerType && b.entity.GameObjGUID == PlayerManager.Instance.LocalPlayer.GameObjGUID)
                {
                    return true;
                }
            }
            return false;
        }
        //
        public bool IsHaveBuff(uint instID)
        {
            return buffDict.ContainsKey(instID);
        }
        static public int chenmoID = 1017;
        //

        public Buff AddBuff(uint instID, uint typeID, float remainTime, Ientity entity)
        {
            if (IsHaveBuff(instID))
            {
                return buffDict[instID];
            }
            if (entity == null)
            {
                return null;
            }

            if (isHaveStopBuff(entity.GameObjGUID) == false)
            {
                BuffConfigInfo bi = ConfigReader.GetBuffInfo(typeID);
                if (null != bi)
                {
                    if (bi.effectID == (int)eBuffEffect.eBuffEffectXuanYun || bi.effectID == (int)eBuffEffect.eBuffEffectShuFu)
                    {
                        entity.OnEntityGetAstrictBuff();
                    }
                }
            }
            
            Buff b = new Buff();
            b.buffID = instID;
            b.buffTypeID = typeID;
            b.buffTime = remainTime;
            b.entity = entity;

            //buffDict[instID] = b;
            buffDict.Add(instID, b);

            if (isSelfHaveBuffType(chenmoID) == true)
            {
                EventCenter.Broadcast<bool>(EGameEvent.eGameEvent_LocalPlayerSilence, true);
            }
            //refresh ui
            if (UIBuffUnityInterface.Instance != null)
            {
                UIBuffUnityInterface.Instance.RefreshUIItem();
            }


            return b;
        }
        //
        public void RemoveBuff(uint instID)
        {
            if (buffDict.ContainsKey(instID))
            {
                //if (buffDict[instID].buffTypeID == 1017 && buffDict[instID].entity.GameObjGUID == PlayerManager.Instance.LocalPlayer.GameObjGUID)
                //{
                //    if (isSelfHaveBuffType(1017))
                //    {

                //    }
                //    //to do set skillbtn enable
                //}
                Buff b = buffDict[instID];
                BuffConfigInfo bi = ConfigReader.GetBuffInfo(b.buffTypeID);

                buffDict.Remove(instID);
                if (bi != null)
                {
                    if (bi.effectID == (int)eBuffEffect.eBuffEffectXuanYun || bi.effectID == (int)eBuffEffect.eBuffEffectShuFu)
                    {
                        if (b.entity != null)
                        {
                            if (isHaveStopBuff(b.entity.GameObjGUID) == false)
                            {
                                b.entity.OnEntityRomoveAstrictBuff();
                            }
                        }
                    }
                }

                //如果该Buffer带有BeatFlyMotion效果,立即取消BeatFlyMotion效果
                if (b != null && bi.eFlyEffectID != 0)
                {
                    //获取击飞信息                
                    SkillFlyConfig skillFlycfg = ConfigReader.GetSkillFlyConfig(bi.eFlyEffectID);
                    
                    if (skillFlycfg != null && b.entity != null)
                    {

                        b.entity.BeatFlyFallDown(bi.BuffID);
                    }
                }



                //refresh ui
                if (UIBuffUnityInterface.Instance != null)
                {
                    UIBuffUnityInterface.Instance.RefreshUIItem();
                }

                if (isSelfHaveBuffType(chenmoID) == false)
                {
                    EventCenter.Broadcast<bool>(EGameEvent.eGameEvent_LocalPlayerSilence, false);
                }
            }
        }
        //
        public void Update()
        {
            //to do
            foreach (Buff b in buffDict.Values)
            {
                b.Update();
            }
            if (UIBuffUnityInterface.Instance != null)
            {
                UIBuffUnityInterface.Instance.UpdateUIItem();
            }
        }
    }
}
