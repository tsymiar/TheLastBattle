using UnityEngine;
using System.Collections;

namespace GameDefine
{
    public class RuneGoodsInfo
    {
        public int mGoodsId;
        public int RLGoldPrice;
        public int RLDiamondPrice;
    }

    public class OneRuneInfo
    {
        public long gottime
        {
            set;
            get;
        }
        public int num
        {
            set;
            get;
        }
    }

    public class EffectInfo
    {
        public double val
        {
            set;
            get;
        }

        public double per
        {
            set;
            get;
        }

        public string des
        {
            set;
            get;
        }
    }

    public class RuneItemInfo
    {
        public RuneConfigInfo sRuneConfigInfo
        {
            get;
            set;
        }

        public int num
        {
            get;
            set;
        }
    }

    public enum ConsumeType
    {
        TypeGold = 1,
        TypeDiamond,
    }

    public enum SystemSet
    {
        SoundButton,
        MusicButton,
        EffectButton,
        RangeButton,
        CloseButton,
    }
    public enum EHeroKillTitle
    {
        eNoneTitel = 10018,
        eKillingSpree,
        eDominating,
        eMegaKill,
        eUnstoppable,
        eWhickedSick,
        eMonsterKill,
        eGodLike,
        eHolyShit,
    }

    public enum HeroType
    {
        AllHero,
        AttackHero = 1,
        AssistHero,
        SpellHero,
        DefendHero
    }

    public enum FRIENDTYPE { 
        FRIENDNULL,
        FRIENDLIST,
        BLACKLIST,
    }
    public enum ONLINEFRIEND { 
        ONLINENULL,
        ONLINE,
    }

    public enum NoticeState { 
        NoticNull = 0,
        NoticeHot,
        NoticeNew,
    }

    public enum NoticeIdentify
    {
        NoticIdentifyNull = 0,
        NoticIdentifyDiscount,
        NoticIdentifyActivity,
        NoticIdentifyNotify,
    }

    public enum MaterDailyType
    {
        nullType = 0,
        moneyType = 1,
        diamonType,
        runeType,
        heroType,
        skinType,
    }

    public enum ObPlayerOrPlayer
    {
        InitType = -1,
        PlayerType,
        PlayerObType,
    }
    public enum SITUATION
    {
        DEUCE,
        WIN,
        LOSE,
    }
	public enum e_BattleState{
        eBS_SelectHero = 0,
        eBS_SelectRune,
        eBS_Loading,
        eBS_Playing,
        eBS_Finished,
	}
	public enum LobbyPvpButtom{
		CUSTOM,
		NEWBIE_GUIDE_ONE,
		NEWBIE_GUIDE_TWO,
		NEWBIE_GUIDE_THREE
	}
    public enum MsgTalk
    {
        ReadNull,
        UnReadMsg,
        ReadMsg,
    }

	public enum MAPTYPE
	{
        NEWS_GUIDE_1 = 1000,
		EASYMAP = 1001,
		MIDDLEMAP,
		HIGHMAP,
        
	}

	public enum SkillType{
		SKILL_NULL = -1,
		SKILL_TYPE1,
		SKILL_TYPE2,
		SKILL_TYPE3,
		SKILL_TYPE4,
        SKILL_TYPE5,
        SKILL_TYPEABSORB1,
        SKILL_TYPEABSORB2,
	}
    public enum SkillStateType
    {
        SKILL_STATE_NORMAL = 0,
        SKILL_STATE_PREPARE = 1,
        SKILL_STATE_PLAY = 2,
        SKILL_STATE_COOLDOWN = 3,
    }

	public enum BattleType{
		BATTLE_TYPE_1,
		BATTLE_TYPE_2,
		BATTLE_TYPE_3,
		BATTLE_TYPE_4,
	}

	public enum OptionLobby
	{
		FIGHTQUICK = 0,
		STORE,
		PERSONALINFO,
		GODROAD,
		ACCOUNTCHANGE,
        DIAMOND,
        GOLD,
	}
    public enum BattleDataType
    {
        HeadIcon,
        NickName,
        Level,
        Kills,
        Deaths,
        Assist,
        LastHit,
        Goods,
        Cp
    }
    public enum EFuryState
    {
        eFuryNullState = 0,
        eFuryFullState,
        eFuryRunState,
    }

    public enum EEffectCate : byte
    {
        eEffectCate_None = 0,
        eEffectCate_NormalPhyHurt,
        eEffectCate_NormalMagicHurt,
        eEffectCate_ChangeCurHP,
        eEffectCate_ChangeCurMP,
        eEffectCate_ChangePhyAttack,
        eEffectCate_ChangeMagicAttack,
        eEffectCate_ChangePhyDefense,
        eEffectCate_ChangeMagicDefense,
        eEffectCate_ChangeMoveSpeed,
        eEffectCate_ChangeAttackSpeed,
        eEffectCate_ChangeMaxHP,
        eEffectCate_ChangeMaxMP,
        eEffectCate_ChangeHPRecoverRate,
        eEffectCate_ChangeMPRecoverRate,
        eEffectCate_ChangeReliveTime,
        eEffectCate_Dizziness,
        eEffectCate_Silence,
        eEffectCate_PosMove,
        eEffectCate_Rebound,
        eEffectCate_SuckBlood,
        eEffectCate_Disarm,
        eEffectCate_Restrain,
        eEffectCate_CriPersent,
        eEffectCate_CriHarm,
        eEffectCate_CPRecover,
        eEffectCate_MaxMoveSpeed
    };


	public static class GameConstDefine{
        public static int maxRandomNameSegment = 3;
        public static int[] CombineCost = {10,20,100,300,1200}; 
        public static string showWashMsg = "洗练此符文需要";
        public static int MaxUnlockOfOneLevel = 3;
        public static int MaxNewComposeWaitTime = 50000;
        public static int RefreshRuneCardIndex = 0;
        public static int RefreshRuneSlotIndex = 1;
        public static int MaxCombineSlotNum = 4;
        public static int MaxRefreshSlotNum = 2;
        public static int MaxRuneLevel = 7;
        public static int MaxRuneSlot = 10;
        public static int BuyGoodsSuccess = 10031;
		public static string LoadProsPath = "LoadPros/LoadProcess";
		public static string LoadMonsterModels = "Monsters/";
        public static int LackGoldErrorCode = -130902;
        public const string GuisPlay = "Guis/Play/";

        public const int HeroBuyCfgIDBegin = 100001;
        public const int HeroBuyCfgIDEnd = 109999;

        public const int RuneBuyCfgIDBegin = 120001;
        public const int RuneBuyCfgIDEnd = 129999;

        //login
		public static string LoadGameLoginUI = "Guis/UIGameLogin";
        public static string LoadGameUserUI = "Guis/UIRegister";
        public static string LoadGameHeroHead = "Guis/Head1";

        //lobby
        public static string LoadGameLobbyUI = "Guis/UIGameLobby";
        public static string LoadGameBattleUI = "Guis/Lobby/PvpOrGuideBase";
        public static string LoadGameLobbyRoomItemUI = "Guis/Lobby/Room";
        public static string LoadGameLobbyCustomMapItem = "Guis/Lobby/Create_Maps";
        public static string LoadGameLobbyMatchMapItem = "Guis/Lobby/Match_Maps";
        public static string LoadGameTeamMatchUI = "Guis/Lobby/TeamMatch";
        public static string LoadGameMatchFriendsItem = "Guis/Lobby/Match_Friends";
        public static string LoadGameMatchTeammateItem = "Guis/Lobby/Match_Member";
        public static string LoadGameMatchInvitationUI = "Guis/Lobby/InvitationTips";
        public static string LoadGameMatchSearchingUI = "Guis/Lobby/MatchSearching";
        public static string LoadGameHomePageUI = "Guis/Lobby/HomePage";
        public static string LoadPersonalDateOptionUI = "Guis/Lobby/PersonalDateOption";
        public static string LoadVIPPrerogativeUI = "Guis/Lobby/VIPPrerogative";
        public static string LoadVIPInfoUI = "Guis/Lobby/VIPInfo";
        public static string LoadSystemNoticeUI = "Guis/SystemNotice";
        public static string LoadNoticeTitleUI = "Guis/NoticeTitle";
        public static string LoadHeroDataUI = "Guis/Lobby/HeroData";
        public static string LoadHeroDataPhoto = "Guis/Lobby/HeroDataPhoto";
        public static string LoadGameSettingUI = "Guis/Lobby/GameSetting";
        public static string LoadExtraBonusUI = "Guis/Lobby/ExtraBonus";

        //daily
        public static string LoadDailyBonusUI = "DailyBonus/DailyBonus";
        public static string LoadDailyPrizeUI = "DailyBonus/Prize";
        public static string LoadDailyOneBonusUI = "DailyBonus/Bonus";
        public static string LoadDailyHeroIcon = "Guis/TeamHead2";
        public static string LoadDailyTaskUI = "DailyBonus/TaskItems";
        public static string LoadInfiniteTaskUI = "DailyBonus/AchieveItems";
        //room
        public static string LoadGameRoomUI = "Guis/Lobby/UIRoom";
        public static string LoadInvitationList = "Guis/Lobby/InvitationList";
        public static string LoadInvitationUI = "Guis/Lobby/InvitationMembers";
        //hero
        public static string HeroSelectUI = "Guis/Lobby/SelectHero";
        public static string HeroShowBoxUI = "Guis/Lobby/Hero";
        //Social
        public static string LoadMakeFriendsUI = "Guis/Lobby/MakeFriends";
        public static string LoadFriendUI = "Guis/Lobby/MyFriend";
        public static string LoadInvitationTips = "Guis/Lobby/InvitationTips";

        //task
        public static string LoadChatTaskUI = "Guis/Lobby/ChatBox";
        public static string LoadChatMember = "Guis/Lobby/ChatMember";
        public static string LoadTalk = "Guis/Lobby/Talks";
        //gameplay
        public static string LoadBattleInfoUI = "Guis/Play/Information";
        public static string LoadMiniMapUI = "Guis/Play/MiniMapWindow";
        public static string LoadEquipmentUI = "Guis/Play/EquipmentWindow";
        public static string LoadEnemyUI = "Guis/Play/EnemyWindow";
        public static string LoadTimeDownUI = "Guis/Countdown";
        public static string LoadSkillUI = "Guis/Play/SkillWindow";

        //overgame
        public static string ScoreUI = "Guis/Over/ShowScore";
        public static string SettlementUI = "Guis/Over/GoldSettlement";

        public static string LoadGameServerUI = "Guis/UIGameServer";
        public static string LoadServerItems = "Guis/Server";
        public static string LoadGameMainUI = "Guis/UIMainWindow";
        public static string LoadGameViewer = "Guis/UIViewer";
		public static string LoadGameMapUI = "Map/SelfPrefeb";
        public static string pathAltarHead = "Guis/Play/AltarSoldierHead";
        

        public static string LoadFriendListUI = "Guis/Lobby/BuddySystem";
        
        
        public static string LoadInputSend = "Guis/Lobby/InputArea";
        public static string LoadDailyBonus = "DailyBonus/DailyBonus";

        public static string LoadGameMarketRuneListUI = "Guis/Lobby/RuneOption";
        public static string LoadGameMarketRuneTag = "Guis/Lobby/RuneTag";
        public static string LoadGameMarketRuneInfoUI = "Guis/Lobby/SureToBuyRune";
        public static string LoadGameMarketRuneItem = "Guis/Lobby/RuneItem";
        public static string LoadRuneBuyUI = "Guis/Lobby/PurchaseInterface";
        
        public static string LoadSearchResult = "Guis/Lobby/SearchResults";
        
		//lobby
		public static string LoadLobby = "Lobby/";
        public static string LoadGameMarketUI = "Guis/Lobby/MallMenuOption";
        public static string LoadHeadPhotoUi = "Guis/Lobby/HeadPhoto";
        
        public static string LoadGameMarketHeroInfoUI = "Guis/Lobby/SureToBuyHero";
        public static string LoadGameMarketHeroTag = "Guis/Lobby/HeroTag";
        public static string LoadGameMarketHeroListUI = "Guis/Lobby/HeroOption";
        public static string LoadGameHeroTimeLimitUI = "Guis/Lobby/HeroTimeLimit";

        public static string LoadRuneEquipWindowRes = "Guis/Lobby/RuneEquip";
        public static string LoadRuneCombineWindowRes = "Guis/Lobby/RuneCombine";
        public static string LoadRuneRefreshWindowRes = "Guis/Lobby/RuneRefresh";
        public static string LoadRuneRefreshCardRes = "Guis/Lobby/ScrollItem";
        public static string PurchaseSuccessTipRes = "Guis/Lobby/PurchaseSuccess";

        public static string RuneSlotAddFlash = "effect/ui_effect/slot_flash";
        public static string RuneSlotEquipLight = "effect/ui_effect/slot_light";
        public static string RuneCombineFlash = "effect/ui_effect/slot_combine";
        public static string RuneDisappearFlash = "effect/ui_effect/slot_disappear";
        public static string RuneFreshFlash = "effect/ui_effect/slot_refresh";
		
		public static string StartSelfDefineUI = "Guis/Lobby/StartSelfDefine";
		public static string PersonalDateUI = "Guis/Lobby/PersonalDateOption";
        public static string GameSettingUI = "Guis/Lobby/GameSetting";
		public static string LoadGameEffectPath = "effect/skill/";
        //mail
        public static string LoadMailGUI = "Guis/Lobby/Mails";
        public static string LoadMailTitleGUI = "Guis/Lobby/MailTitle";
        public static string LoadMailAttachGUI = "Guis/Lobby/MailAttach";

		public static string MessageTipUI = "Guis/MessageTips";
        public static string WaitingUI = "Guis/WaitingInterface";
        public static string GameConnectMsg = "Guis/Play/ConnectionPanel";
		
        public static string FrameTips = "Guis/Lobby/FrameTips";

        public static string LoadGameBuildingEffectPath = "effect/building/";
		public static string LoadGameSkillEffectPath = "effect/skill/";
		public static string LoadGameOtherEffectPath = "effect/other/";
        public static string LoadGameBuffEffectPath = "effect/skill/buff/";
		public static string LoadGameLoginEffectPath = "effect/ui_effect/play_burst";
        public static string LoadGameShadow = "effect/shadow/shadow";
        public static string GameOverBoxPath = "Guis/Over/superwan";

        public static string LoadGameSoundPath = "sound/";

        public const float PlayerLockTargetDis = 20f;
        public const float MonsterAtkDistThresold = 1.2f;



        public static string MapObjectReplaceIDPath = "Build/";

        /// /////新手引导///////////////
        public static string UIGuideRestPath = "Guide/UIGuideMask";
        public static string AdVancedGuideRestPath = "Guide/UIGuideInfo";
        public static string SkipNewsGuidePath = "Guide/xuanzhi";
		public static string DeathInterfaceUI = "Guis/Play/DeathInterface";
        public static string DeathInterfaceUIForGuide = "Guis/Play/DeathInterfaceForGuide";
        
		public static string TeamMateInfoUI = "Guis/Play/TeamMateInfo";
        public static string AltarSoldierSelect = "Guis/Play/UIAltarSoldierSelect";
        public static string UISoulsReplace = "Guis/Play/UISoulsReplace";

        

        public static string FuryStartLongPressEffect = "effect/other/power_xuli";
        public static string FurySkillBtnEffect = "effect/ui_effect/power_skill";
        public static string FuryHeadInfoEffect = "effect/ui_effect/power_head";
        public static string FuryFullFuryInfoEffect = "effect/ui_effect/anger_full";
        public static string FuryGasExplosionEffect = "effect/other/baoqi"; 


        public static string PlayerMapGameObject_Red = "Guis/Play/PlayerIconRed";
        public static string SoldierMapGameObject_Red = "Guis/Play/SoldierIconRed";

        public static string PlayerMapGameObject_Green = "Guis/Play/PlayerIconGreen";
        public static string SoldierMapGameObject_Green = "Guis/Play/SoldierIconGreen";

        public static string NpcMinimapBaseIcon = "Guis/Play/BaseIcon";
        public static string NpcMinimapTowerIcon = "Guis/Play/TowerIcon";
        public static string NpcMinimapBarracksIcon = "Guis/Play/BarracksIcon";
        public static string NpcMinimapCreepsIconSmall = "Guis/Play/CreepsIconSmall";
        public static string NpcMinimapCreepsIconHuge = "Guis/Play/CreepsIconHuge";
        public static string NpcMinimapShopIcon = "Guis/Play/ShopIcon";


        public static string MapWarning = "Guis/Play/MapWarning";

        public static string WarningBuildingVoice = "Audio/AudioKill/our_base_is_under_attack";
        public static string WarningBuildingSound = "Audio/UIAudio/xiaoditutishii";

        public static string WarningRuin = "Guis/Play/RuinWarning";
        public static string GameShop = "Guis/Play/Shop";

        public static string AfterPackItemPath = "Guis/Play/Update";
        public static string BuyPackItemPath = "Guis/Play/SelectPanel";

        

        public static string SkillDestribe = "Guis/Play/SkillInfoPanel";

        public static string ItemDestribe = "Guis/Play/ItemsInfoPanel";

        public static string LockEffect = "effect/ui_effect/lock_effect";

        public static string CdEndEffect = "effect/ui_effect/Remove_cooling_effect";

        public static string PressBtnEffect = "effect/ui_effect/Cooldown_Click";

        public static string HomeBaseBeAtkEffect = "effect/ui_effect/majorcity_hit";

        public static string GuideOverUi = "Guide/GuideCompeled";

        public static string GameLogin = "Game/GameLogin";



        public static string ResRootPath
        {
            get
            {
                //Debug.LogError(Application.platform);
                switch (Application.platform)
                {
                    case RuntimePlatform.WindowsPlayer:
                    case RuntimePlatform.WindowsEditor:
                         //return Application.dataPath + "/StreamingAssets/Windows/";
                         return "file:///" + Application.dataPath + "/StreamingAssets/Windows/";
                    case RuntimePlatform.OSXEditor:
                    case RuntimePlatform.OSXPlayer:
                        return "file:///" + Application.dataPath + "/StreamingAssets/Ios/";
                    case RuntimePlatform.IPhonePlayer:
                        return "file:///" + Application.dataPath + "/Raw/Ios/";
                    case RuntimePlatform.Android:
                        return "jar:file://" + Application.dataPath + "!/assets/Android/";
                }
                return "";
            }
        }
        public static bool IfHeroBuyCfg(int id)
        {
            return id >= HeroBuyCfgIDBegin && id <= HeroBuyCfgIDEnd;
        }

        public static bool IfRuneBuyCfg(int id)
        {
            return id >= RuneBuyCfgIDBegin && id <= RuneBuyCfgIDEnd;
        }
	}

}


