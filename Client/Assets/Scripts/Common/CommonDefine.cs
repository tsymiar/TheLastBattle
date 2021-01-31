using UnityEngine;
using System;
using System.Collections;
using System.Threading;
using System.IO;
using System.Runtime.InteropServices;
using JT.FWW.Tools;
using JT.FWW.GameData;

#pragma warning disable 414

namespace JT
{
    namespace FWW
    {
        namespace GameData
        {
//          public  class CConstDefine
//	        {
//            //private static CConstDefine s_Instance = null;
//		        private static    UInt32 s_un32MaxProduceBuildingDemandNum = 6;
//              private static    UInt32 s_un32DefaultNameLen = 30;
//              private static    UInt32 s_un32DefaultPasswordLen = 33;
//				private static	  UInt32 s_un32DefaultDeviceKeyLen = 150;
//				private static 	  Int32  s_n32DefaultMaxHP = 100;
//				private static    UInt32 s_un32DefaultUpgradeNeedSecond = 30;
//				private static 	  Int32  s_n32MaxShipInBuilding = 6;
//				private static 	  Int32		s_n32DefaultLaunchCDSecond = 6;
//
//              public static UInt32 MaxProduceBuildingDemandNum{get{return s_un32MaxProduceBuildingDemandNum;}}
//              public static UInt32 DefaultNameLen{get{return s_un32DefaultNameLen;}}
//              public static UInt32 DefaultPasswordLen{get{return s_un32DefaultPasswordLen;}}
//				public static UInt32 DefaultDeviceKeyLen{get{return s_un32DefaultDeviceKeyLen;}}
//				public static Int32 DefaultMaxHP{get{return s_n32DefaultMaxHP;}}
//				public static UInt32 DefaultUpgradeNeedSecond{get{return s_un32DefaultUpgradeNeedSecond;}}
//				public static Int32 MaxShipInBuilding{get{return s_n32MaxShipInBuilding;}}
//				public static Int32 DefaultLaunchCDSecond{get{return s_n32DefaultLaunchCDSecond;}}
//	        };
//			
//			public enum ECamp
//			{
//				eCamp_Friend = 0,
//				eCamp_Enemy,
//			}
//			
//			public	enum EGameState
//			{
//				eGS_Normal,
//				eGS_SearchAttack,
//				eGS_PrepareAttack,
//				eGS_Attacking,
//				eGS_ReturnSelfCity,
//			}
//			
//	        public enum ERuningMode
//			{
//				eRuningMode_None = 0,
//				eRuningMode_Restriction = 1,
//				eRuningMode_Local = 2,
//				eRuningMode_Network = 3,
//			}			
            //public enum EMyConst
            //{
            //    eMyConst_DefaultNameLen = 15,
            //    eMyConst_DefaultPasswordLen = 33,
            //}
			
			public  class CConstDefine
	        {
				private static    UInt32 s_un32DefaultNameLen = 30;
                private static    UInt32 s_un32DefaultPasswordLen = 33;
				private static	  UInt32 s_un32DefaultDeviceKeyLen = 150;
				private static 	  Int32  s_n32MaxShipInBuilding = 6;
				private static	Int32	s_n32MaxAttackProjectileNumInShip = 3;
				private static	Int32	s_n32MaxDefenseProjectileNumInShip = 3;
				private static	Int32	s_n32ProjectileMaxChildProjectileType = 3;
				
		        private static    UInt32 s_un32MaxProduceBuildingDemandNum = 6;
				private static 	  Int32  s_n32DefaultMaxHP = 100;
				private static    UInt32 s_un32DefaultUpgradeNeedSecond = 30;				
				private static 	  Int32	 s_n32DefaultLaunchCDSecond = 6;
				private static EObjectType s_eDefaultProjectileTypeID = (EObjectType)52001;				
				private static Int64	s_n64DefaultFoodCostForBuilding = 1000;
				private static Int64	s_n64DefaultSteelCostForBuilding = 1000;
				private static Int64	s_n64DefaultOilCostForBuilding = 1000;				
				private static Int64	s_n64DefaultFoodCostForShip = 600;
				private static Int64	s_n64DefaultSteelCostForShip = 600;
				private static Int64	s_n64DefaultOilCostForShip = 600;
				private static Int32	s_n32PrepareAttackSecond = 30;
				private static Int32	s_n32AttackSecond = 600;
				private static Int32	s_n32DefaultShipZDist = 4000;
				private static Int32	s_n32DefaultMissileMiniYAltitude = 5;
				private static Int32	s_n32DefaultMissileYAltitude = 200;
				private static Int32	s_n32DefaultCannonBallAltitude = 300;
				private static Int32	s_n32DefaultShipXRange = 200;
				private static Int32	s_n32DefaultAircraftFighterYAltitude = 1000;
				private static Int32	s_n32DefaultAircraftBomberYAltitude = 1000;
				private static Int32	s_n32DefaultAircraftAsistantYAltitude = 1000;
				private static Int32	s_n32DefaultAircraftRotateRadius = 400;
				private static Int32	s_n32DefaultMissileRotateRadius = 400;
				private static float	s_fDefaultNearAttackDistance = 2500;
				private static float	s_fDefaultModerateAttackDistance = 3000;
				private static float	s_fDefaultFarAttackDistance = 3500;
				private static Int32	s_n32DefaultRecoverHPEverySec = 10;
				private static Int32	s_n32MiniDistanceToTargetCity = 18520;				
//				private static Int32	s_n32EObjLevel1Inter = 10000;
//				private static Int32	s_n32EObjLevel2Inter = 1000;
//				private static Int32	s_n32EObjLevel3Inter = 1000;
				
				public static UInt32 DefaultNameLen{get{return s_un32DefaultNameLen;}}
                public static UInt32 DefaultPasswordLen{get{return s_un32DefaultPasswordLen;}}
				public static UInt32 DefaultDeviceKeyLen{get{return s_un32DefaultDeviceKeyLen;}}
				public static Int32 MaxShipInBuilding{get{return s_n32MaxShipInBuilding;}}
				
                public static UInt32 MaxProduceBuildingDemandNum{get{return s_un32MaxProduceBuildingDemandNum;}}               
				public static Int32 DefaultMaxHP{get{return s_n32DefaultMaxHP;}}
				public static UInt32 DefaultUpgradeNeedSecond{get{return s_un32DefaultUpgradeNeedSecond;}}
				public static Int32 DefaultLaunchCDSecond{get{return s_n32DefaultLaunchCDSecond;}}
				public static EObjectType DefaultProjectileTypeID{get{return s_eDefaultProjectileTypeID;}}
				public static Int32	ProjectileMaxChildProjectileType{get{return s_n32ProjectileMaxChildProjectileType;}}				
				public static Int64 DefaultFoodCostForBuilding{get{return s_n64DefaultFoodCostForBuilding;}}
				public static Int64 DefaultSteelCostForBuilding{get{return s_n64DefaultSteelCostForBuilding;}}
				public static Int64 DefaultOilCostForBuilding{get{return s_n64DefaultOilCostForBuilding;}}
				public static Int64 DefaultFoodCostForShip{get{return s_n64DefaultFoodCostForShip;}}
				public static Int64 DefaultSteelCostForShip{get{return s_n64DefaultSteelCostForShip;}}
				public static Int64 DefaultOilCostForShip{get{return s_n64DefaultOilCostForShip;}}
				public static Int32 PrepareAttackSecond{get{return s_n32PrepareAttackSecond;}}
				public static Int32 AttackSecond{get{return s_n32AttackSecond;}}
				public static Int32 DefaultShipZDist{get{return s_n32DefaultShipZDist;}}
				public static Int32 DefaultMissileMiniYAltitude{get{return s_n32DefaultMissileMiniYAltitude;}}
				public static Int32 DefaultMissileYAltitude{get{return s_n32DefaultMissileYAltitude;}}
				public static Int32 DefaultCannonBallAltitude{get{return s_n32DefaultCannonBallAltitude;}}
				public static Int32 DefaultShipXRange{get{return s_n32DefaultShipXRange;}}
				public static Int32 DefaultAircraftFighterYAltitude{get{return s_n32DefaultAircraftFighterYAltitude;}}
				public static Int32 DefaultAircraftBomberYAltitude{get{return s_n32DefaultAircraftBomberYAltitude;}}
				public static Int32 DefaultAircraftAsistantYAltitude{get{return s_n32DefaultAircraftAsistantYAltitude;}}				
				public static Int32 DefaultAircraftRotateRadius{get{return s_n32DefaultAircraftRotateRadius;}}
				public static Int32 DefaultMissileRotateRadius{get{return s_n32DefaultMissileRotateRadius;}}
				public static float DefaultNearAttackDistance{get{return s_fDefaultNearAttackDistance;}}				
				public static float DefaultModerateAttackDistance{get{return s_fDefaultModerateAttackDistance;}}
				public static float DefaultFarAttackDistance{get{return s_fDefaultFarAttackDistance;}}				
				public static Int32 DefaultRecoverHPEverySec{get{return s_n32DefaultRecoverHPEverySec;}}
				public static Int32 MiniDistanceToTargetCity{get {return s_n32MiniDistanceToTargetCity;}}
				
//				public const Int32 EObjLevel1Inter{get{return s_n32EObjLevel1Inter;}}
//				public static Int32 EObjLevel2Inter{get{return s_n32EObjLevel2Inter;}}
//				public static Int32 EObjLevel3Inter{get{return s_n32EObjLevel3Inter;}}
	        };
			
			public enum EUserPlatform
			{
				eUP_None,
				eUP_Apple,
				eUP_QQ,
				eUP_360,
			};
						
			public enum EClientOS
			{
				eCOS_IOS,
				eCOS_Android,
			};
			
			public enum EUserLoginWay
			{
				eULW_None = 0,
				eULW_QuickLogin,
			};
			
			public enum EUserCate
            {
	            eUserCate_Admin = 0,
	            eUserCate_GM,
	            eUserCate_Robot,
	            eUserCate_DiamondVIP,
	            eUserCate_GloldVIP,
	            eUserCate_SilverVIP,
	            eUserCate_CopperVIP,
	            eUserCate_Normal,
            };
			
			public enum EEquipCate
            {
				eEquipCate_None = 0,
				eEquipCate_Engine = 1,
				eEquipCate_ElectricGenerator = 2,
	            eEquipCate_FoodCollector = 3,
				eEquipCate_SteelCollector = 4,
	            eEquipCate_OilCollector = 5,
				eEquipCate_FireExtinguisher = 6,
	            eEquipCate_Radar = 7,
	            eEquipCate_Sonar = 8,
				eEquipCate_Communication = 9,
	            eEquipCate_ElectronicWarfare = 10,
	            eEquipCate_MachineGun = 11,
	            eEquipCate_Cannon = 12,
	            eEquipCate_TorpedoLauncher = 13,
	            eEquipCate_BombLauncher = 14,
	            eEquipCate_MissileLauncher = 15,
				eEquipCate_Armour = 16,				
				eEquipCate_HelicopterApron = 17,				
				eEquipCate_FighterApron = 18,
				eEquipCate_BombersApron = 19,
				eEquipCate_AssistantAircraftApron = 20,				
            };
			
			public enum EEquipAssembleCate
			{
				eEquipAssembleCate_Building = 0,
				eEquipAssembleCate_Ship = 1,
				eEquipAssembleCate_Boath = 2,
			}
			
			public enum EShipCate
            {
				eShipCate_Null = 0,
	            eShipCate_TransportShip = 1,
	            eShipCate_PatrolBoat = 2,
	            eShipCate_EscortVessel = 3,
	            eShipCate_Destroyer = 4,
	            eShipCate_Cruiser = 5,
				eShipCate_BattleShip = 6,
	            eShipCate_AircraftCarrier = 7,
            };
			
			public enum EShipEquipSlotIdx
			{
				eShipEquipSlotIdx_Invalid = -1,
				eShipEquipSlotIdx_Engine = 0,
				eShipEquipSlotIdx_Communication = 1,
				eShipEquipSlotIdx_Radar = 2,
				eShipEquipSlotIdx_Sonar = 3,
				eShipEquipSlotIdx_Torpedo = 4,
				eShipEquipSlotIdx_ElectronicWarfare = 5,
				eShipEquipSlotIdx_BowMainCannon = 6,
				eShipEquipSlotIdx_LeftBoardCannon = 7,
				eShipEquipSlotIdx_RightBoardCannon = 8,
				eShipEquipSlotIdx_SternCannon = 9,
				eShipEquipSlotIdx_HelicopterApron = 10,
				eShipEquipSlotIdx_FighterApron = 11,
				eShipEquipSlotIdx_BombersApron = 12,
				eShipEquipSlotIdx_EarlyWarningAircraftApron = 13,
				eShipEquipSlotIdx_AntiShipMissile = 14,
				eShipEquipSlotIdx_AntiAircraftMissile = 15,
				eShipEquipSlotIdx_ballisticMissile = 16,
				eShipEquipSlotIdx_End,
			};
			
			public enum EBuildingCate
            {
				eBuildingCate_None = 0,
	            eBuildingCate_CivilAdmin = 1,
	            eBuildingCate_Education = 2,
	            eBuildingCate_Ecnomic = 3,
	            eBuildingCate_Religion = 4,
	            eBuildingCate_Health = 5,
	            eBuildingCate_BaseIndustry = 6,
	            eBuildingCate_ResourceCollector = 7,
	            eBuildingCate_ProductFactory = 8,
	            eBuildingCate_Weapon = 9,
	            eBuildingCate_Miracle = 10,
				eBuildingCate_Security = 11,
				eBuildingCate_Communication = 12,
            };
			
			public enum EBuildingEquipSlotIdx
			{
				eBuildingEquipSlotIdx_Invalid = -1,
				eBuildingEquipSlotIdx_Armour = 0,
				eBuildingEquipSlotIdx_First,
				eBuildingEquipSlotIdx_Second,
				eBuildingEquipSlotIdx_Third,
				eBuildingEquipSlotIdx_End,
			};			
			
//			[Serializable]
//    		[StructLayout(LayoutKind.Sequential, Pack = 1)]
//			public class CMyTestClass
//			{
//				[MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
//				public char[] m_myTestCharArray;
//				public int m_n32TestInt;
//			};			
			
			enum EConstEnum
			{
				eObjLevel1Inter = 10000,		
			}
			
            public enum EObjectType
            {
	            eObjectType_None = 0,
	            eObjectType_Guild,
	            eObjectType_User,

				eObjectType_HeroBegin = EConstEnum.eObjLevel1Inter * 1,

				eObjectType_NPCBegin = EConstEnum.eObjLevel1Inter * 2,

				eObjectType_GoodsBegin = EConstEnum.eObjLevel1Inter * 3,

                eObjectType_AiRobotBegin = EConstEnum.eObjLevel1Inter * 4,

            };

			public enum EGameObjectServerState{
				eGOSS_None = -1,
				eGOSS_Free = 0,
				eGOSS_Running,
				eGOSS_Flying,
				eeGOSS_Swimming,
				eGOSS_PreparingSkill,
				eGOSS_ReleasingSkill,
				eGOSS_PassiveState,
				eGOSS_PassiveMove,
				eGOSS_Dead,
				eGOSSS_End,
			}

			public class CGameObjectSyncInfo{
				public float fServerSpeed;
				public Vector3 sServerBeginPos;
				public Vector3 sServerSyncPos;
				public Vector3 sServerDir;
				public Vector3 sLocalSyncDir;
                public float fLastSyncSecond;		
				public float fBeginTime;
				public float fDistMoved;
                public float fForceMoveSpeed;
			};
        }
    }
}