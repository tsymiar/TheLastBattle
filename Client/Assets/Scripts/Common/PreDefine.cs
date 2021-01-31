using UnityEngine;
using System;
using System.Collections;
using System.Threading;
using JT.FWW.Tools;
using JT.FWW.GameData;

namespace JT
{
    namespace FWW
    {
        namespace GameData
        {
            public enum EConst
            {
                eConst_ErrorCodeInterval = 0x00010000,
				eConst_Four_Bytes = 4,
				eConst_DefaultNameLen = 30,
                eConst_DefaultNickNameLen = 18,
				eConst_DefaultPasswordLen = 33,
			}

            public enum EMissionType
            {
                eMissionType_Attack,
            }
            public enum EErrorCode
            { 
                eEC_NullMsgHandler = -65492,
                eEC_InvalidUserName = -65476,
				//Server model error code.
				eEC_FWW_Begin = -(EConst.eConst_ErrorCodeInterval * 2),
                eEC_NullGateServer = -131062,
                eEC_InvalidUserPwd = -131060,
                eEC_NullUser = -131055,
                eEC_JustInBattle = -131036,
                eEC_InvalidBattlePos = -131035,

                eEC_TheBattleUserFull = -131034,
                eEC_JustNotInBattle = -131032,
                eEC_NotAllUserReady = -131021,
                eEC_YouAreNotBattleManager = -131020,
                eEC_NullBattle = -131013,
                eEC_InvalidMapID = -131006, 

                eEC_ReEnterRoomFail = -130772,
                eEC_UserInfoUnComplete = -130998,
                eEC_BattlePDWNotMatch = -130979,

                eEC_NULLPassWord =   -130788,
                eEC_AccountTheGame = -130756,
               
                eEC_NULLNickName = -130977,
                eEC_HeroNotDead = -130937,
                eEC_NotEnoughGold = -130936,
                eEC_NoRebornTimes = -130935,
                eEC_BattleIsPlaying = -130934,
                eEC_AskBuyGoodsFailForBagFull = -130916,
                eEC_NickNameCollision = -130909,
                eEC_BattleHaveStart = -130903,
                eEC_AddBattleFailForLackOfGold = -130902,
                eEC_CampNotBalance = -130901,
                eEC_AddBattleFailForAllFull = -130872,
                eEC_AddBattleFailForUserFull = -130871,
                eEC_WarningToSelectHero = -130870,
                eEc_NickNameContainInvalideChar = -130863,
                eEC_AskTooFrequently = -130832,

                eEC_CannotBuygoodsWhenDeath = -130812,
                eEc_CannotSelGoodsWhenDeath = -130813,
                eEC_AskHaveSend = -130814,//请求已发送
                eEC_UserInYourBlackList = -130815,//玩家在你的黑名单中
                eEC_CounterpartFriendListFull = -130816,//对方好友已满
                eEC_UserInBlackList = -130817,//在对方黑名单中
                eEC_JustInBlackList = -130820,//在黑名单中
                eEC_JustInFriendsList = -130821,//在好友名单中
                eEC_BlackListFull = -130822,// 黑名单已满
                eEC_FriendsListFull = -130823,// 好友名单已满
                eEC_BattleClosing = -130790, //重连时提示战斗已经结束
                eEC_InvalidBattleID = -131030,//无效的战斗ID
                eEC_MatchLinkInvalid =-130767,      //游戏已经开始
                eEC_FuryNotEnough = -130943,   //怒气不足
                eEC_FuryRuning = -130944,   //正在释放怒气
                

				eEC_FWWClient_Begin = -(EConst.eConst_ErrorCodeInterval * 3),
				eEC_Base_LostNetConnection,
				eEC_InvalidGameSate,
				eEC_NumLimit,
				eEC_CanNotFindSpecifiedGUID,
				eEC_InvlaidGameState,
				eEC_InvlaidTroops,
				eEC_InvlaidWeaponTypeID,
				eEC_InvlaidDetonatorType,
				eEC_InvalidNum,
				eEC_InvlaidMissileTypeID,
				eEC_InvlaidTxtResource,
				eEC_CanNotFindTheShip,
				eEC_CanNotFindTheBuilding,
				eEC_CanNotFindTheCity,
				eEC_InvalidObjTypeID2,
				eEC_InvalidEquipSlotIdx,
				eEC_NullRunTimeForceInfo,
				eEC_ProjectileEmpty,
				eEC_CannotAttackBuilding,
				eEC_InvalidProjectileTypeID,
				eEC_LaunchCD,
				eEC_NullPointer,
				eEC_CannotFindTarget,
				eEC_NullSpecifiedComponent,
				eEC_InvalidTypeID,
				eEC_AbsentDollar,
				eEC_InvalidGUFS,
				eEC_InvalidGameUnitType,
				eEC_LoadConfigFileFail,

                eEC_ConstInvalid = -2,
                eEC_OperatePending = -1,
                eNormal = 0,
            };
            public enum ERROR_TYPE
            {
                eT_FillInNiceName = 10000,
                eT_NullRoomID = 10001,
                eT_CancelReady = 10002,
                eT_ObtainRandomHero = 10003,
                eT_NeedLockTarget = 10004,
                eT_ManagerChange = 10010,
                eT_AbsentSkillNULL = 10011,
                eT_NeedAItermID = 10012,
                eT_NeedABagID = 10013,
                eT_BagIsEmpty = 10014,
                eT_WarnningToSelectHero = 10015,
                eT_GoodsIsCoolingDown = 10016,
                eT_GoodsNULLUseTimes = 10017,

               
                eT_TalkSendNull = -16,
            }

            public enum ReturnRet
            {
                eT_Normal,
                eT_DelEntityFailed,


            }
        }
    }
}