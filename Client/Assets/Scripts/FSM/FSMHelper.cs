using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using BlGame.GameEntity;
using UnityEngine;

namespace BlGame.FSM
{
    public static class FSMHelper
    {
        public static bool ExecuteDeviation(Ientity entity)
        {
            if (entity.realObject == null)
            {
                return true;
            }
            float disToFsmPos = Vector3.Distance(entity.realObject.transform.position, entity.EntityFSMPosition);
            if (disToFsmPos <= FSMVariable.IgnoreDistance)
            {
                return true;
            }
            //else if (disToFsmPos > FSMVariable.IgnoreDistance && disToFsmPos < FSMVariable.MoveDistance)
            //{
            //    Debug.Log("Middle " + disToFsmPos);
            //    entity.RealEntity.transform.LookAt(entity.EntityFSMPosition);
            //    entity.RealEntity.PlayerRunAnimation();
            //    entity.RealEntity.Controller.Move(entity.EntityFSMMoveSpeed * Time.deltaTime * entity.RealEntity.transform.forward);
            //    return false;
            //}
            else {
//                entity.RealEntity.transform.position = entity.EntityFSMPosition;
                //entity.RealEntity.transform.position = entity.EntityFSMPosition;
            }
            return true;
        }
    }
}
