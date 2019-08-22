using System;
using System.Collections.Generic;
using UnityEngine;  
using System.Linq; 
namespace BlGame.GuideDate
{
    public class SecondaryTaskCheckBase
    {

        protected SecondaryTaskInfo parentInfo; 

        public virtual void OnEnter(SecondaryTaskInfo parent)
        {
            parentInfo = parent; 
        }
 
        public virtual void AddCheckListener() { 

        }

        public virtual void RemoveAddListener() { 

        }
 
        public virtual void OnExcuese() {
          
        }

        public virtual void OnEnd() {
            
        }

        protected virtual void TaskTriggerCheck() { 

        }

    }
}
