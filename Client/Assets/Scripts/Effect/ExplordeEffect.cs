using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using UnityEngine;
using System.Collections;

using JT.FWW.Tools;
using BlGame.GameData;
using BlGame.GameEntity;
using BlGame.Effect;
using BlGame;
using BlGame.Skill;

//技能爆炸效果，通常应对范围技能
namespace BlGame.Effect
{
    class ExplordeEffect : IEffect
    {
        public ExplordeEffect()
        {

        }
        
        public override void OnLoadComplete()
        {
            Ientity enOwner;
            EntityManager.AllEntitys.TryGetValue(enOwnerKey, out enOwner);

            if (obj != null && enOwner != null)
            {
                SkillConfigInfo skillinfo = ConfigReader.GetSkillInfo(skillID);
                Transform point = enOwner.RealEntity.objAttackPoint;

                if (point != null)
                {
                    if (skillinfo.SkillType == (int)ISkill.ESkillType.eST_FixDistanceArea)
                    {
                        Vector3 temp = fixPosition;
                        temp.y = point.transform.position.y;
                        fixPosition = temp;
                        GetTransform().position = fixPosition;
                    }
                    else
                    {
                        GetTransform().position = point.transform.position;
                    }


                    Ientity enTarget;
                    EntityManager.AllEntitys.TryGetValue(enTargetKey, out enTarget);
                    
                    //Debug.LogError("fly effect init pos:" + root.transform.position.x + ":" + root.transform.position.y + ":" + root.transform.position.z);
                    if (mType == IEffect.ESkillEffectType.eET_FlyEffect && enTarget != null)
                    {
                        Quaternion rt = Quaternion.LookRotation(enTarget.RealEntity.transform.position - GetTransform().position);
                        GetTransform().rotation = rt;
                    }
                    else
                    {
                        if (dir == Vector3.zero)
                        {
                            dir = Vector3.one;
                        }
                        Quaternion rt = Quaternion.LookRotation(dir);
                        GetTransform().rotation = rt;
                    }
                }
            }
        }        
        public override void Update()
        {
            if (isDead)
                return;

            GetTransform().position = fixPosition;

            base.Update();              
        }
    }
}
