using UnityEngine;
using System.Collections;

using JT.FWW.Tools;
using System;

namespace BlGame.GameEntity
{
    public class INpc : Ientity
    {

        public INpc(UInt64 sGUID, EntityCampType campType)
            : base(sGUID, campType)
        { 

        }

    }
}
