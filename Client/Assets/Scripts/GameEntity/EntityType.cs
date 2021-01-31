using UnityEngine;
using System.Collections;

namespace BlGame.GameEntity
{
	public enum EntityType{
		Monster = 1,
		Soldier,
		Building,
		Player,
        AltarSoldier,
	}

    public enum ENPCCateChild
    {
        eNPCChild_None = 0,
        eNPCChild_NPC_Per_AtkBuilding,
        eNPCChild_NPC_Per_Bomb,
        eNPCChild_SmallMonster, //Ð¡Ò°¹Ö 3
        eNPCChild_HugeMonster,  //´óÒ°¹Ö 4

        eNPCChild_BUILD_Altar = 10,  //¼ÀÌ³
        eNPCChild_BUILD_Base,        //»ùµØ
        eNPCChild_BUILD_Shop,        //ÉÌµê
        eNPCChild_BUILD_Tower,       //¼ýËþ

        eNPCChild_BUILD_Summon = 20,

        eNPCCateChild_Ohter,
    };

 
}
