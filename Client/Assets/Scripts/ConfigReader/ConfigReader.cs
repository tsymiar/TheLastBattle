using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using BlGame.GuideDate;
using System;

public class ConfigReader
{
    public static void Init()
    {
        Dictionary<uint, SkillPassiveConfigInfo> sp = SkillPassiveInfoDic;
        Dictionary<int, SkillMoveConfig> sm = SkillMoveInfoDic;
        Dictionary<int, SkillFlyConfig>  skillFlyCfg = SkillBeatFlyInfoDict;
        Dictionary<int, SkillCameraConfig> skillCameraCfg = SkillCameraInfoDict;        
        Dictionary<int, SkillSummonConfig> ss = SkillSummonInfoDic;
        Dictionary<uint, SkillLeadingonfig> sl = SkillLeadingInfoDic;
        Dictionary<uint, SkillAreaConfig> sa = SkillAreaInfoDic;
        Dictionary<int, SkillManagerConfig> sma = SkillManagerInfoDic;
        Dictionary<uint, SkillAccountConfig> sac = SkillAccountInfoDic;
        Dictionary<uint, SkillEmitConfig> sem = SkillEmitXmlInfoDic;
        Dictionary<int, MsgConfigInfo> mxm = MsgXmlInfoDic;
        Dictionary<int, MsgSettingConfigInfo> mse = MsgSettingXmlInfoDic;
        Dictionary<uint, BuffConfigInfo> bxm = BuffXmlInfoDict;
        Dictionary<uint, SkillConfigInfo> sxm = SkillXmlInfoDict;
        Dictionary<int, HeroConfigInfo> hxml = HeroXmlInfoDict;
        Dictionary<int, NpcConfigInfo> nxm = NpcXmlInfoDict;
        Dictionary<int, HeroSelectConfigInfo> hsx = HeroSelectXmlInfoDict;
        Dictionary<int, HeroBuyConfigInfo> hbc = HeroBuyXmlInfoDict;
        Dictionary<int, HeroSkinConfigInfo> hsc = HeroSkinXmlInfoDict;
        Dictionary<int, BuySkinConfigInfo> bsx = BuySkinXmlInfoDict;
        Dictionary<int, VipLevelConfigInfo> viplevel = VipLevelXmlInfoDict;
        Dictionary<int, HeadSelectConfigInfo> hsci = HeadSelectXmlInfoDict;
        Dictionary<UInt32, RuneConfigInfo> runeCfg = RuneXmlInfoDict;
        Dictionary<uint, DailyBonusConfigInfo> dailyBonusDic = DailyBonusXmlInfoDic;
        Dictionary<uint, STaskConfig> dailyTaskDic = DailyTaskXmlInfoDic;
        Dictionary<uint, STaskConfig> infiniteTaskDic = InfiniteTaskXmlInfoDic;
        Dictionary<uint, OtherItemConfigInfo> otherItemDic = OtherItemXmlInfoDic;
		Dictionary<int, GuideMgrInfo> gmgr = GuideTaskMgrInfoDict;


        Dictionary<int, GuideButtonClickInfo> gtb = GuideButtonClickXmlDict;
        Dictionary<int, CameraMoveInfo> gcm = GuideCameraMoveXmlInfoDict;
        Dictionary<int, GuidePathInfo> gpi = GuidePathInfoDict;
        Dictionary<int, AdvancedGuideInfo> aid = AdvancedGuideInfoDict;
        Dictionary<int, GuideKillHeroInfo> dsi = GuideKillHeroXmlInfoDict;
        ////mail gift
        //Dictionary<int, MailGiftConfig> mailGift = MailGiftXmlInfoDict;
        //Dictionary<int, MailTypeConfig> mailType = MailTypeXmlInfoDict; 
    }

    #region 英雄头像表
    public static Dictionary<int, HeadSelectConfigInfo> headSelectXmlInfoDict = new Dictionary<int, HeadSelectConfigInfo>();
    public static Dictionary<int, HeadSelectConfigInfo> HeadSelectXmlInfoDict
    {
        get
        {
            if (headSelectXmlInfoDict.Count == 0)
            {
                ReadHeroHeadConfig heroHeadConfig = new ReadHeroHeadConfig("Config/HeadSelect");
            }
            return headSelectXmlInfoDict;
        }

    }
    #endregion

    public static Dictionary<int, GuideTaskInfo> guideTaskXmlInfoDict = new Dictionary<int, GuideTaskInfo>();
    public static Dictionary<int, GuideTaskInfo> GuideTaskXmlInfoDict
    {
        get
        {
            if (guideTaskXmlInfoDict.Count == 0)
            {
                ReadGuideTaskConfig heroHeadConfig = new ReadGuideTaskConfig("Config/GuideTask");
            }
            return guideTaskXmlInfoDict;
        }

    }


    #region passiveskill
    public static Dictionary<uint, SkillPassiveConfigInfo> skillPassiveInfoDic = new Dictionary<uint, SkillPassiveConfigInfo>();
    public static Dictionary<uint, SkillPassiveConfigInfo> SkillPassiveInfoDic
    {
        get
        {
            if (skillPassiveInfoDic.Count == 0)
            {
                ReadSkillPassiveConfig spConfig = new ReadSkillPassiveConfig("Config/SkillCfg_passitive");
            }
            return skillPassiveInfoDic;
        }
    }
    public static SkillPassiveConfigInfo GetSkillPassiveConfig(uint id)
    {
        if (SkillPassiveInfoDic.ContainsKey(id))
        {
            return SkillPassiveInfoDic[id];
        }
        return null;
    }
    #endregion
    #region skillmove
    public static Dictionary<int, SkillMoveConfig> skillMoveInfoDic = new Dictionary<int, SkillMoveConfig>();
    public static Dictionary<int, SkillMoveConfig> SkillMoveInfoDic
    {
        get
        {
            if (skillMoveInfoDic.Count == 0)
            {
                ReadSkillMoveConfig smConfig = new ReadSkillMoveConfig("Config/SkillCfg_move");
            }
            return skillMoveInfoDic;
        }
    }
    public static SkillMoveConfig GetSkillMoveConfig(int id)
    {
        if (!SkillMoveInfoDic.ContainsKey(id))
        {
            GameDefine.GameMethod.DebugError("has no SkillMoveConfig:" + id);
            return null;
        }
        return SkillMoveInfoDic[id];
    }
    #endregion

     #region skillBeatFly
    public static Dictionary<int, SkillFlyConfig> skillBeatFlyInfoDict = new Dictionary<int, SkillFlyConfig>();
    public static Dictionary<int, SkillFlyConfig> SkillBeatFlyInfoDict
    {
        get
        {
            if (skillBeatFlyInfoDict.Count == 0)
            {
                ReadSkillFlyConfig smConfig = new ReadSkillFlyConfig("Config/SkillCfg_fly");
            }
            return skillBeatFlyInfoDict;
        }
    }
    public static SkillFlyConfig GetSkillFlyConfig(int id)
    {
        if (!SkillBeatFlyInfoDict.ContainsKey(id))
        {
            GameDefine.GameMethod.DebugError("has no SkillFlyConfig:" + id);
            return null;
        }
        return SkillBeatFlyInfoDict[id];
    }
    #endregion




    #region skillCamera
    public static Dictionary<int, SkillCameraConfig> skillCameraInfoDict = new Dictionary<int, SkillCameraConfig>();
    public static Dictionary<int, SkillCameraConfig> SkillCameraInfoDict
    {
        get
        {
            if (skillCameraInfoDict.Count == 0)
            {
                ReadSkillCameraConfig smConfig = new ReadSkillCameraConfig("Config/SkillCfg_Camera");
            }
            return skillCameraInfoDict;
        }
    }
    public static SkillCameraConfig GetSkillCameraConfig(int id)
    {
        if (!SkillCameraInfoDict.ContainsKey(id))
        {
            GameDefine.GameMethod.DebugError("has no SkillCameraConfig:" + id);
            return null;
        }
        return SkillCameraInfoDict[id];
    }
    #endregion

       

    #region skillsummon
    public static Dictionary<int, SkillSummonConfig> skillSummonInfoDic = new Dictionary<int, SkillSummonConfig>();
    public static Dictionary<int, SkillSummonConfig> SkillSummonInfoDic
    {
        get
        {
            if (skillSummonInfoDic.Count == 0)
            {
                ReadSkillSummonConfig ssConfig = new ReadSkillSummonConfig("Config/SkillCfg_summon");
            }
            return skillSummonInfoDic;
        }
    }
    public static SkillSummonConfig GetSkillSummonConfig(int id)
    {
        if (!SkillSummonInfoDic.ContainsKey(id))
        {
            //GameDefine.GameMethod.DebugError("has no SkillSummonConfig:" + id);
            return null;
        }
        return skillSummonInfoDic[id];
    }
    #endregion
    #region skillleading表
    public static Dictionary<uint, SkillLeadingonfig> skillLeadingInfoDic = new Dictionary<uint, SkillLeadingonfig>();
    public static Dictionary<uint, SkillLeadingonfig> SkillLeadingInfoDic
    {
        get
        {
            if (skillLeadingInfoDic.Count == 0)
            {
                ReadSkillLeadingConfig slConfig = new ReadSkillLeadingConfig("Config/SkillCfg_leading");
            }
            return skillLeadingInfoDic;
        }
  
    }
    public static SkillLeadingonfig GetSkillLeadingConfig(uint id)
    {
        if (!SkillLeadingInfoDic.ContainsKey(id))
        {
            //GameDefine.GameMethod.DebugError("has no SkillLeadingonfig:" + id);
            return null;
        }
        return skillLeadingInfoDic[id];
    }
    #endregion
    #region skillarea表
    public static Dictionary<uint, SkillAreaConfig> skillAreaInfoDic = new Dictionary<uint, SkillAreaConfig>();
    public static Dictionary<uint,SkillAreaConfig> SkillAreaInfoDic
    {
        get
        {
            if (skillAreaInfoDic.Count == 0)
            {
                ReadSkillAreaConfig saConfig = new ReadSkillAreaConfig("Config/SkillCfg_area");
            }
            return skillAreaInfoDic;
        }
    }
    public static SkillAreaConfig GetSkillAreaConfig(uint id)
    {
        if (!SkillAreaInfoDic.ContainsKey(id))
        {
            return null;
        }
        return skillAreaInfoDic[id];
    }
#endregion
    #region skillmanager表
    public static Dictionary<int, SkillManagerConfig> skillManagerInfoDic = new Dictionary<int, SkillManagerConfig>();
    public static Dictionary<int, SkillManagerConfig> SkillManagerInfoDic
    {
        get
        {
            if (skillManagerInfoDic.Count == 0)
            {
                ReadSkillManager smConfig = new ReadSkillManager("Config/SkillCfg_manager");
            }
            return skillManagerInfoDic;
        }
    }
    public static SkillManagerConfig GetSkillManagerCfg(int id)
    {
        if (!SkillManagerInfoDic.ContainsKey(id))
        {
            //GameDefine.GameMethod.DebugError("Has no SkillManagerCfg." + id);
            return null;
        }
        return skillManagerInfoDic[id];
    }
    #endregion

    #region skillaccount表
    public static Dictionary<uint, SkillAccountConfig> skillAccountInfoDic = new Dictionary<uint, SkillAccountConfig>();
    public static Dictionary<uint, SkillAccountConfig> SkillAccountInfoDic
    {
        get
        {
            if (skillAccountInfoDic.Count == 0)
            {
                ReadSkillAccountConfig resConfig = new ReadSkillAccountConfig("Config/SkillCfg_account");
            }
            return skillAccountInfoDic;
        }
    }
    public static SkillAccountConfig GetSkillAccountCfg(uint id)
    {
        if (!SkillAccountInfoDic.ContainsKey(id))
        {
            //GameDefine.GameMethod.DebugError("Has no skillAccountCfg:" + id);
            return null;
        }
        return SkillAccountInfoDic[id];
    }
    #endregion
    #region skillemit表
    public static Dictionary<uint, SkillEmitConfig> skillEmitXmlInfoDic = new Dictionary<uint, SkillEmitConfig>();
    public static Dictionary<uint, SkillEmitConfig> SkillEmitXmlInfoDic
    {
        get
        {
            if (skillEmitXmlInfoDic.Count == 0)
            {
                ReadSkillEmitConfig skillEmitConfig = new ReadSkillEmitConfig("Config/SkillCfg_emit");
            }
            return skillEmitXmlInfoDic;
        }
    }
    public static SkillEmitConfig GetSkillEmitCfg(uint id)
    {
        if (!SkillEmitXmlInfoDic.ContainsKey(id))
        {
            return null;
        }
        return SkillEmitXmlInfoDic[id];
    }
    #endregion
    #region res表
    public static Dictionary<string, ResConfigInfo> resXmlInfoDic = new Dictionary<string, ResConfigInfo>();
    public static Dictionary<string, ResConfigInfo> ResXmlInfoDic
    {
        get
        {
            if (resXmlInfoDic.Count < 1)
            {
                ReadResConfig resConfig = new ReadResConfig("Config/ResCfg");
            }
            return resXmlInfoDic;
        }
    }
    public static ResConfigInfo GetResInfo(string path)
    {
        if (!ResXmlInfoDic.ContainsKey(path))
        {
            //GameDefine.GameMethod.DebugError("Has no Res:" + path);
            return null;
        }
        return ResXmlInfoDic[path];
    }
    #endregion
    #region msg表
    public static Dictionary<int, MsgConfigInfo> msgXmlInfoDic = new Dictionary<int, MsgConfigInfo>();
    public static Dictionary<int, MsgConfigInfo> MsgXmlInfoDic
    {
        get
        {
            if (msgXmlInfoDic.Count < 1)
            {
                ReadMsgConfig msgConfig = new ReadMsgConfig("Config/MsgCfg");
            }
            return msgXmlInfoDic;
        }

    }
    public static MsgConfigInfo GetMsgInfoByServerErrorCode(int code)
    {
        if (MsgXmlInfoDic.ContainsKey(code))
        {
            return MsgXmlInfoDic[code];
        }
        return null;
    }
    public static MsgConfigInfo GetMsgInfo(int MsgID)
    {
        if (!MsgXmlInfoDic.ContainsKey(MsgID))
        {
            //GameDefine.GameMethod.DebugError("Has no MsgID Key: " + MsgID);
            return null;
        }
        return MsgXmlInfoDic[MsgID];
    }
    #endregion
    #region msgsetting表
    public static Dictionary<int, MsgSettingConfigInfo> msgSettingXmlInfoDic = new Dictionary<int, MsgSettingConfigInfo>();
    public static Dictionary<int, MsgSettingConfigInfo> MsgSettingXmlInfoDic
    {
        get
        {
            if(msgSettingXmlInfoDic.Count == 0)
            {
                ReadMsgSettingConfig msgConfig = new ReadMsgSettingConfig("Config/MsgSettingCfg");
            }
            return msgSettingXmlInfoDic;
        }
        
    }
    public static MsgSettingConfigInfo GetMsgSettingInfo(int MsgSettingID)
    {
        if (!MsgSettingXmlInfoDic.ContainsKey(MsgSettingID))
        {
            //GameDefine.GameMethod.DebugError("Has no MsgSettingID Key: " + MsgSettingID);
            return null;
        }
        return MsgSettingXmlInfoDic[MsgSettingID];
    }
    #endregion
    #region buff表
    public static Dictionary<uint, BuffConfigInfo> buffXmlInfoDict = new Dictionary<uint, BuffConfigInfo>();
    public static Dictionary<uint, BuffConfigInfo> BuffXmlInfoDict
    {
        get
        {
            if (buffXmlInfoDict.Count == 0)
            {
                ReadBuffConfig buffConfig = new ReadBuffConfig("Config/SkillCfg_buff");
            }
            return buffXmlInfoDict;
        }
    }
    public static BuffConfigInfo GetBuffInfo(uint BuffID)
    {
        if (!BuffXmlInfoDict.ContainsKey(BuffID))
        {
            //Debug.LogError("Has no BuffID Key: " + BuffID);
            return null;
        }
        return BuffXmlInfoDict[BuffID];
    }
    #endregion
    #region 技能表
    public static Dictionary<uint,SkillConfigInfo> skillXmlInfoDict = new Dictionary<uint,SkillConfigInfo>();
	public static Dictionary<uint,SkillConfigInfo> SkillXmlInfoDict 
	{
		get
		{
			if(skillXmlInfoDict.Count == 0)
			{
				ReadSkillConfig skillConfig = new ReadSkillConfig("Config/SkillCfg");
			}
			return skillXmlInfoDict;
		}
	}
	public static SkillConfigInfo GetSkillInfo(uint SkillID)
	{
		if(!SkillXmlInfoDict.ContainsKey(SkillID))
		{
			return null;
		}
		return SkillXmlInfoDict[SkillID];
	}
	#endregion

	#region 英雄表
	public static Dictionary<int, HeroConfigInfo> heroXmlInfoDict = new Dictionary<int, HeroConfigInfo>();
	public static Dictionary<int, HeroConfigInfo> HeroXmlInfoDict
	{
		get
		{
			if(heroXmlInfoDict.Count == 0)
			{
				ReadHeroConfig heroXmlReader = new ReadHeroConfig("Config/HeroCfg");
			}
			return heroXmlInfoDict;
		}
	}
	
	public static HeroConfigInfo GetHeroInfo(int heroId)
	{
        if (!HeroXmlInfoDict.ContainsKey(heroId))
		{
            Debug.LogError("Has no Hero Key: " + heroId);
			return null;
		}
        return HeroXmlInfoDict[heroId];
	}
	#endregion

	#region NPC表
	public static Dictionary<int, NpcConfigInfo> npcXmlInfoDict = new Dictionary<int, NpcConfigInfo>();
	public static Dictionary<int, NpcConfigInfo> NpcXmlInfoDict
	{
		get
		{
			if(npcXmlInfoDict.Count == 0)
			{
				ReadNpcConfig npcXmlReader = new ReadNpcConfig("Config/NPCCfg");
			}
			return npcXmlInfoDict;
		}
	}

    public static Dictionary<int, MapObjConfigInfo> mapObjXmlInfoDict = new Dictionary<int, MapObjConfigInfo>();
    public static Dictionary<int, MapObjConfigInfo> MapObjXmlInfoDict
    {
        get
        {
            if (mapObjXmlInfoDict.Count == 0)
            {
                ReadMapObjConfig mapXmlReader = new ReadMapObjConfig("Config/MapObjCfg");
            }
            return mapObjXmlInfoDict;
        }
    }
	
	public static NpcConfigInfo GetNpcInfo(int npcId)
	{
		if(!NpcXmlInfoDict.ContainsKey(npcId))
		{
			return null;
		}
		return NpcXmlInfoDict[npcId];
	}
	#endregion

	#region 英雄选择表
	public static Dictionary<int, HeroSelectConfigInfo> heroSelectXmlInfoDict = new Dictionary<int, HeroSelectConfigInfo>();
	public static Dictionary<int, HeroSelectConfigInfo> HeroSelectXmlInfoDict
	{
		get
		{
			if(heroSelectXmlInfoDict.Count == 0)
			{
				ReadSelectHeroConfig heroXmlReader = new ReadSelectHeroConfig("Config/HeroChooseCfg");
			}
			return heroSelectXmlInfoDict;
		}
	}

    public static Dictionary<int, HeroBuyConfigInfo> heroBuyXmlInfoDict = new Dictionary<int, HeroBuyConfigInfo>();
    public static Dictionary<int, HeroBuyConfigInfo> HeroBuyXmlInfoDict
    {
        get
        {
            if (heroBuyXmlInfoDict.Count == 0)
            {
                ReadHeroBuyConfig heroBuyyXmlReader = new ReadHeroBuyConfig("Config/HeroBuyCfg");
            }
            return heroBuyXmlInfoDict;
        }
    }

    public static HeroBuyConfigInfo GetHeroBuyInfo(int goodId)
    {
        if (!HeroBuyXmlInfoDict.ContainsKey(goodId))
        {
            Debug.LogError("Has no HeroBuy Key: " + goodId);
            return null;
        }
        return HeroBuyXmlInfoDict[goodId];
    }
    /// <summary>
    /// cameraMove
    /// </summary>

    public static Dictionary<int, CameraRemotionConfigInfo> cameraXmlInfoDict = new Dictionary<int, CameraRemotionConfigInfo>();
    public static Dictionary<int, CameraRemotionConfigInfo> CameraXmlInfoDict
    {
        get
        {
            if (cameraXmlInfoDict.Count == 0)
            {
                ReadMoveCameraConfig cameraXmlReader = new ReadMoveCameraConfig("Config/CameraRemotion");
            }
            return cameraXmlInfoDict;
        }
    }

    public static CameraRemotionConfigInfo GetCameraInfo(int mapID)
    {
        if (!CameraXmlInfoDict.ContainsKey(mapID))
        {
            Debug.LogError("Has no HeroBuy Key: " + mapID);
            return null;
        }
        return CameraXmlInfoDict[mapID];
    }



    public static Dictionary<int, HeroSkinConfigInfo> heroSkinXmlInfoDict = new Dictionary<int, HeroSkinConfigInfo>();
    public static Dictionary<int, HeroSkinConfigInfo> HeroSkinXmlInfoDict
    {
        get
        {
            if (heroSkinXmlInfoDict.Count == 0)
            {
                ReadHeroSkinConfig heroSkinyXmlReader = new ReadHeroSkinConfig("Config/SkinCfg");
            }
            return heroSkinXmlInfoDict;
        }
    }

    public static HeroSkinConfigInfo GetHeroSkinInfo(int goodId)
    {
        if (!HeroSkinXmlInfoDict.ContainsKey(goodId))
        {
            Debug.LogError("Has no HeroSkin Key: " + goodId);
            return null;
        }
        return HeroSkinXmlInfoDict[goodId];
    }

	
	public static HeroSelectConfigInfo GetHeroSelectInfo(int heroId)
	{
		if(!HeroSelectXmlInfoDict.ContainsKey(heroId))
		{
			Debug.LogError("Has no Hero Select Key: " + heroId);
			return null;
		}
		return HeroSelectXmlInfoDict[heroId];
	}

    public static Dictionary<int, ItemConfigInfo> itemXmlInfoDict = new Dictionary<int, ItemConfigInfo>();
    public static Dictionary<int, ItemConfigInfo> ItemXmlInfoDict
    {
        get
        {
            if (itemXmlInfoDict.Count == 0)
            {
                ReadItemConfig itemConfig = new ReadItemConfig("Config/ItemCfg");
            }
            return itemXmlInfoDict;
        }
    }

    public static Dictionary<int, CombineConfigInfo> combineXmlInfoDict = new Dictionary<int, CombineConfigInfo>();
    public static Dictionary<int, CombineConfigInfo> CombineXmlInfoDict
    {
        get
        {
            if (combineXmlInfoDict.Count == 0)
            {
                ReadCombineConfig combineConfig = new ReadCombineConfig("Config/CombineCfg");
            }
            return combineXmlInfoDict;
        }
    }

    public static Dictionary<int, ShopConfigInfo> shopXmlInfoDict = new Dictionary<int, ShopConfigInfo>();
    public static Dictionary<int, ShopConfigInfo> ShopXmlInfoDict
    {
        get
        {
            if (shopXmlInfoDict.Count == 0)
            {
                ReadShopConfig itemConfig = new ReadShopConfig("Config/ShopCfg");
            }
            return shopXmlInfoDict;
        }
    }
	#endregion


    #region 英雄表
    public static Dictionary<int, LevelConfigInfo> levelXmlInfoDict = new Dictionary<int, LevelConfigInfo>();
    public static Dictionary<int, LevelConfigInfo> LevelXmlInfoDict
    {
        get
        {
            if (levelXmlInfoDict.Count == 0)
            {
                ReadLevelConfig levelXmlReader = new ReadLevelConfig("Config/UserAccountCfg");
            }
            return levelXmlInfoDict;
        }
    }

    public static LevelConfigInfo GetLevelInfo(int level)
    {
        if (!LevelXmlInfoDict.ContainsKey(level))
        {
            Debug.LogError("Has no Level Key: " + level);
            return null;
        }
        return LevelXmlInfoDict[level];
    }
    #endregion

    public static CFlashTask GetFlashTaskInfo(int taskId)
    {
        if (CTaskBase.flashTaskDic == null || CTaskBase.flashTaskDic.Count == 0)
        {
            ReadGuideFlashTaskConfig guidTask = new ReadGuideFlashTaskConfig("Config/Animine");
        }
        CFlashTask task = null;
        if (!CTaskBase.flashTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导动画表中不存在任务id " + taskId);
        return task;
    }

    public static CRewardTask GetRewardTaskInfo(int taskId)
    {
        if (CTaskBase.rewardTaskDic == null || CTaskBase.rewardTaskDic.Count == 0)
        {
            ReadGuideRewardTaskConfig guidTask = new ReadGuideRewardTaskConfig("Config/Award");
        }
        CRewardTask task = null;
        if (!CTaskBase.rewardTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导奖励表中不存在任务id " + taskId);
        return task;
    }

    public static CCameraTask GetCameraTaskInfo(int taskId)
    {
        if (CTaskBase.cameraTaskDic == null || CTaskBase.cameraTaskDic.Count == 0)
        {
            ReadGuideCameraTaskConfig guidTask = new ReadGuideCameraTaskConfig("Config/Vidicon");
        }
        CCameraTask task = null;
        if (!CTaskBase.cameraTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导摄像机任务表中不存在任务id " + taskId);
        return task;
    }

    public static Dictionary<int, CameraMoveInfo> guideCameraMoveXmlInfoDict = new Dictionary<int, CameraMoveInfo>();
    public static Dictionary<int, CameraMoveInfo> GuideCameraMoveXmlInfoDict
    {
        get
        {
            if (guideCameraMoveXmlInfoDict.Count == 0)
            {
                ReadGuideCameraTaskConfig guidTask = new ReadGuideCameraTaskConfig("Config/Vidicon");
            }
            return guideCameraMoveXmlInfoDict;
        }
    }

    //public static CPopTipTask GetPopTipTaskInfo(int taskId)
    //{
    //    if (CTaskBase.popTipTaskDic == null || CTaskBase.popTipTaskDic.Count == 0)
    //    {
    //        ReadGuidePopTipTaskConfig guidTask = new ReadGuidePopTipTaskConfig("Config/PopWindow");
    //    }
    //    CPopTipTask task;
    //    if (!CTaskBase.popTipTaskDic.TryGetValue(taskId, out task))
    //        Debug.LogError("新手引导弹窗提示任务表中不存在任务id " + taskId);
    //    return task;
    //}

    public static Dictionary<int, PopTipskInfo> guidePopTipTaskXmlDict = new Dictionary<int, PopTipskInfo>();
    public static Dictionary<int, PopTipskInfo> GuidePopTipTaskXmlDict
    {
        get
        {
            if (guidePopTipTaskXmlDict.Count == 0)
            {
                ReadGuidePopTipTaskConfig guidTask = new ReadGuidePopTipTaskConfig("Config/PopWindow");
            }
            return guidePopTipTaskXmlDict;
        }
    }

    public static CTipTask GetTipTaskInfo(int taskId)
    {
        return null;
        if (CTaskBase.tipTaskDic == null || CTaskBase.tipTaskDic.Count == 0)
        {
            ReadGuideTipTaskConfig guidTask = new ReadGuideTipTaskConfig("Config/Title");
        }
        CTipTask task = null;
        if (!CTaskBase.tipTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导字幕提示任务表中不存在任务id " + taskId);
        return task;
    }

    /// <summary>
    /// 战斗引导文字提示
    /// </summary>
    public static Dictionary<int, GuideTitleInfo> guideTitleInfoXmlInfoDict = new Dictionary<int, GuideTitleInfo>();
    public static Dictionary<int, GuideTitleInfo> GuideTitleInfoXmlInfoDict
    {
        get
        {
            if (guideTitleInfoXmlInfoDict.Count == 0)
            {
                ReadGuideTipTaskConfig info = new ReadGuideTipTaskConfig("Config/Title");
            }
            return guideTitleInfoXmlInfoDict;
        }
    }

    public static Dictionary<int, GuideKillHeroInfo> guideKillHeroXmlInfoDict = new Dictionary<int, GuideKillHeroInfo>();
    public static Dictionary<int, GuideKillHeroInfo> GuideKillHeroXmlInfoDict
    {
        get
        { 
            if (guideTitleInfoXmlInfoDict.Count == 0)
            {
                ReadGuideKillHeroTaskConfig info = new ReadGuideKillHeroTaskConfig("Config/killhero");
            }
            return guideKillHeroXmlInfoDict;
        }
    }

    public static CVoiceTask GetVoiceTaskInfo(int taskId)
    {
        if (CTaskBase.voiceTaskDic == null || CTaskBase.voiceTaskDic.Count == 0)
        {
            ReadGuideVoiceTaskConfig guidTask = new ReadGuideVoiceTaskConfig("Config/Audio");
        }
        CVoiceTask task = null;
        if (!CTaskBase.voiceTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导语音提示任务表中不存在任务id " + taskId);
        return task;
    }

    public static CSendNpcTask GetBornNpcTaskInfo(int taskId)
    {
        if (CTaskBase.sendNpcTaskDic == null || CTaskBase.sendNpcTaskDic.Count == 0)
        {
            ReadGuideBornNpcTaskConfig guidTask = new ReadGuideBornNpcTaskConfig("Config/bornnpc");
        }
        CSendNpcTask task = null;
        if (!CTaskBase.sendNpcTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导生成npc任务表中不存在任务id " + taskId);
        return task;
    }

    public static CAbsorbTask GetAbsorbTaskInfo(int taskId)
    {
        if (CTaskBase.absorbTaskDic == null || CTaskBase.absorbTaskDic.Count == 0)
        {
            ReadGuideAbsorbTaskConfig guidTask = new ReadGuideAbsorbTaskConfig("Config/absorb");
        }
        CAbsorbTask task = null;
        if (!CTaskBase.absorbTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导吸附任务表中不存在任务id " + taskId);
        return task;
    }

    public static CGameObjectShowTask GetObjShowTaskInfo(int taskId)
    {
        if (CTaskBase.objShowTaskDic == null || CTaskBase.objShowTaskDic.Count == 0)
        {
            ReadGuideObjShowTaskConfig guidTask = new ReadGuideObjShowTaskConfig("Config/gameobjctshow");
        }
        CGameObjectShowTask task = null;
        if (!CTaskBase.objShowTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导物体显示任务表中不存在任务id " + taskId);
        return task;
    }



    public static CKillTask GetKillTaskInfo(int taskId)
    {
        if (CTaskBase.killTaskDic == null || CTaskBase.killTaskDic.Count == 0)
        {
            ReadGuideKillNpcTaskConfig guidTask = new ReadGuideKillNpcTaskConfig("Config/killnpc");
        }
        CKillTask task = null;
        if (!CTaskBase.killTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导击杀任务表中不存在任务id " + taskId);
        return task;
    }

    public static CObstructTask GetObstructTaskInfo(int taskId)
    {
        if (CTaskBase.obstructTaskDic == null || CTaskBase.obstructTaskDic.Count == 0)
        {
            ReadGuideObstructTaskConfig guidTask = new ReadGuideObstructTaskConfig("Config/obstruct");
        }
        CObstructTask task = null;
        if (!CTaskBase.obstructTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导阻挡任务表中不存在任务id " + taskId);
        return task;
    }

    public static CPathTask GetPathTaskInfo(int taskId)
    {
        if (CTaskBase.pathTaskDic == null || CTaskBase.pathTaskDic.Count == 0)
        {
            ReadGuidePathTaskConfig guidTask = new ReadGuidePathTaskConfig("Config/path");
        }
        CPathTask task = null;
        if (!CTaskBase.pathTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导移动任务表中不存在任务id " + taskId);
        return task;
    }

    public static Dictionary<int, GuidePathInfo> guidePathInfoDict = new Dictionary<int, GuidePathInfo>();
    public static Dictionary<int, GuidePathInfo> GuidePathInfoDict
    {
        get
        {
            if (guidePathInfoDict.Count == 0)
            {
                ReadGuidePathTaskConfig rdTaskMgr = new ReadGuidePathTaskConfig("Config/path");
            }
            return guidePathInfoDict;
        }
    }

    public static Dictionary<int, AdvancedGuideInfo> advancedGuideInfoDict = new Dictionary<int,AdvancedGuideInfo>();
    public static Dictionary<int, AdvancedGuideInfo> AdvancedGuideInfoDict
    {
        get
        {
            if (advancedGuideInfoDict.Count == 0)
            {
                ReadAdvancedGuideConfig rdTaskMgr = new ReadAdvancedGuideConfig("Config/LowerGuide");
            }
            return advancedGuideInfoDict;
        }
    }

    public static Dictionary<int, GuideMgrInfo> guideTaskMgrInfoDict = new Dictionary<int, GuideMgrInfo>();
    public static Dictionary<int, GuideMgrInfo> GuideTaskMgrInfoDict
    {
        get
        {
            if (guideTaskMgrInfoDict.Count == 0)
            {
                ReadGuideManagerTaskConfig rdTaskMgr = new ReadGuideManagerTaskConfig("Config/taskmanager");
            }
            return guideTaskMgrInfoDict;
        }
    }

    public static CTaskManagerData GetManagerTaskInfo(int taskId)
    {
        return null;
        //if (CTaskBase.managerTaskDic.Count == 0)
        //{
        //    ReadGuideManagerTaskConfig guidTask = new ReadGuideManagerTaskConfig("Config/taskmanager");
        //}
        //CTaskManagerData task = null;
        //if (!CTaskBase.managerTaskDic.TryGetValue(taskId, out task))
        //    Debug.LogError("新手引导管理任务表中不存在任务id " + taskId);
        //return task;
    }

    public static CTimeCtrlTask GetTimeCtrlTaskInfo(int taskId)
    {
        if (CTaskBase.timeCtrlTaskDic == null || CTaskBase.timeCtrlTaskDic.Count == 0)
        {
            ReadGuideTimeCtrlTaskConfig guidTask = new ReadGuideTimeCtrlTaskConfig("Config/timectel");
        }
        CTimeCtrlTask task = null;
        if (!CTaskBase.timeCtrlTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导管理任务表中不存在任务id " + taskId);
        return task;
    }

    public static Dictionary<int, GuideButtonClickInfo> guideButtonClickXmlDict = new Dictionary<int, GuideButtonClickInfo>();
    public static Dictionary<int, GuideButtonClickInfo> GuideButtonClickXmlDict
    {
        get
        {
            if (guideButtonClickXmlDict.Count == 0)
            {
                ReadGuideClickButtonTaskConfig rdTaskMgr = new ReadGuideClickButtonTaskConfig("Config/ClickButton");
            }
            return guideButtonClickXmlDict;
        }
    }

    public static CClickButtonTask GetClickButtonTaskInfo(int taskId)
    {
        if (CTaskBase.clickBtnTaskDic == null || CTaskBase.clickBtnTaskDic.Count == 0)
        {
            ReadGuideClickButtonTaskConfig guidTask = new ReadGuideClickButtonTaskConfig("Config/ClickButton");
        }
        CClickButtonTask task = null;
        if (!CTaskBase.clickBtnTaskDic.TryGetValue(taskId, out task))
            Debug.LogError("新手引导点击按钮任务表中不存在任务id " + taskId);
        return task;
    }

    #region VipLevelList
    public static Dictionary<int, VipLevelConfigInfo> ViplevelXmlInfoDict = new Dictionary<int, VipLevelConfigInfo>();
    public static Dictionary<int, VipLevelConfigInfo> VipLevelXmlInfoDict
    {
        get
        {
            if (ViplevelXmlInfoDict.Count == 0)
            {
                ReadVipLevelConfig levelXmlReader = new ReadVipLevelConfig("Config/VIPIntegralCfg");
            }
            return ViplevelXmlInfoDict;
        }
    }

    public static VipLevelConfigInfo GetVipLevelInfo(int level)
    {
        if (!VipLevelXmlInfoDict.ContainsKey(level))
        {
            Debug.LogError("Has no Level Key: " + level);
            return null;
        }
        return VipLevelXmlInfoDict[level];
    }
    #endregion


    //#region 平台配置开始
    /////talkingdata 
    //public static TalkingDataConfig talkingDataXmlInfoDict = new TalkingDataConfig();
    //public static TalkingDataConfig talkingDataXmlInfocfg
    //{ 
    //    get
    //    { 
    //        if (talkingDataXmlInfoDict.appID == null || talkingDataXmlInfoDict.appID.Equals(""))
    //        {
    //            ReadTalkingdataConfig cfig = new ReadTalkingdataConfig("Config/Platform/talkingdata"); 
    //        } 
    //        return talkingDataXmlInfoDict;
    //    } 
    //}

    //public static string GetTalkingDataAppId()
    //{ 
    //    return talkingDataXmlInfocfg.appID; 
    //}
    //public static string GetTalkingDataSwitch()
    //{
    //    return talkingDataXmlInfocfg.appSwitch ;
    //}
    
    ///// platfrom 
    //public static Dictionary<string, PlatformCfg> platfromCfgDic = new Dictionary<string, PlatformCfg>();
    //public static Dictionary<string, PlatformCfg> platfromDic
    //{
    //    get
    //    {
    //        if (platfromCfgDic.Count == 0)
    //        {
    //            ReadPlatformCfg spConfig = new ReadPlatformCfg("Config/platform/platform");
    //        }
    //        return platfromCfgDic;
    //    }
    //}
    //public static int GetPlatfromId(string szName)
    //{
    //    if (platfromDic.ContainsKey(szName)) 
    //    {
    //        return int.Parse(platfromCfgDic[szName].szPlatfromId);
    //    }
    //    return 0;
    //}
    
    //#endregion

    #region 界面引导管理表
    public static Dictionary<int, IGuideManagerData> iGuideManagerDatalXmlInfoDict = new Dictionary<int, IGuideManagerData>();
    public static Dictionary<int, IGuideManagerData> IGuideManagerDatalXmlInfoDict
    {
        get
        {
            if (iGuideManagerDatalXmlInfoDict.Count == 0)
            {
                ReadIGuideTaskManagerConfig config = new ReadIGuideTaskManagerConfig("Config/UiTaskmanager");
            }
            return iGuideManagerDatalXmlInfoDict;
        }
    }

    public static IGuideManagerData GetIGuideManagerInfo(int id)
    {
        if (!IGuideManagerDatalXmlInfoDict.ContainsKey(id))
        { 
            return null;
        }
        return IGuideManagerDatalXmlInfoDict[id];
    }
    #endregion


    #region 界面引触发任务管理表
    public static Dictionary<int, IGuideManagerData> iTriggerGuideManagerDatalXmlInfoDict = new Dictionary<int, IGuideManagerData>();
    public static Dictionary<int, IGuideManagerData> ITriggerGuideManagerDatalXmlInfoDict
    {
        get
        {
            if (iTriggerGuideManagerDatalXmlInfoDict.Count == 0 && iGuideManagerDatalXmlInfoDict.Count == 0)
            {
                ReadIGuideTaskManagerConfig config = new ReadIGuideTaskManagerConfig("Config/UiTaskmanager");
            }
            return iTriggerGuideManagerDatalXmlInfoDict;
        }
    }

    public static IGuideManagerData GetITriggerGuideManagerInfo(int id)
    {
        if (!ITriggerGuideManagerDatalXmlInfoDict.ContainsKey(id))
        {
            return null;
        }
        return ITriggerGuideManagerDatalXmlInfoDict[id];
    }
    #endregion

    #region 界面引导任务表
    public static Dictionary<int, IGuideData> iGuideDatalXmlInfoDict = new Dictionary<int, IGuideData>();
    public static Dictionary<int, IGuideData> IGuideDatalXmlInfoDict
    {
        get
        {
            if (iGuideDatalXmlInfoDict.Count == 0)
            {
                ReadIGuideTaskConfig config = new ReadIGuideTaskConfig("Config/UiTaskEvent");
            }
            return iGuideDatalXmlInfoDict;
        }
    }

    public static IGuideData GetIGuideInfo(int id)
    {
        if (!IGuideDatalXmlInfoDict.ContainsKey(id))
        {
            return null;
        }
        return IGuideDatalXmlInfoDict[id];
    }
    #endregion

    #region 次级引导引导
    public static Dictionary<int, GuideHelpData> guideHelpXmlInfoDict = new Dictionary<int, GuideHelpData>();
    public static Dictionary<int, GuideHelpData> GuideHelpXmlInfoDict
    {
        get
        {
            if (guideHelpXmlInfoDict.Count == 0)
            {
                ReadGuideHelpConfig config = new ReadGuideHelpConfig("Config/HelpTips");
            }
            return guideHelpXmlInfoDict;
        }
    }

    public static GuideHelpData GetGuideHelpInfo(int id)
    {
        if (!GuideHelpXmlInfoDict.ContainsKey(id))
        {
            return null;
        }
        return GuideHelpXmlInfoDict[id];
    }
    #endregion

    #region 每日登陆表
    public static Dictionary<uint, DailyBonusConfigInfo> dailyBonusXmlInfoDic = new Dictionary<uint, DailyBonusConfigInfo>();
    public static Dictionary<uint, DailyBonusConfigInfo> DailyBonusXmlInfoDic
    {
        get
        {
            if (dailyBonusXmlInfoDic.Count < 1)
            {
                ReadDailyBonusConfig msgConfig = new ReadDailyBonusConfig("Config/dailybonus");
            }
            return dailyBonusXmlInfoDic;
        }

    }
    public static DailyBonusConfigInfo GetDailyBonusInfo(uint id)
    {
        if (!DailyBonusXmlInfoDic.ContainsKey(id))
        {
            return null;
        }
        return DailyBonusXmlInfoDic[id];
    }

    #endregion

    #region 每日任务表
    public static Dictionary<uint, STaskConfig> dailyTaskXmlInfoDic = new Dictionary<uint, STaskConfig>();
    public static Dictionary<uint, STaskConfig> DailyTaskXmlInfoDic
    {
        get
        {
            if (dailyTaskXmlInfoDic.Count < 1)
            {
                ReadTaskConfig msgConfig = new ReadTaskConfig("Config/DailyQuest",TASK_TIME_TYPE.TTT_Daily);
            }
            return dailyTaskXmlInfoDic;
        }

    }
    public static STaskConfig GetDailyTaskInfo(uint id)
    {
        if (!DailyTaskXmlInfoDic.ContainsKey(id))
        {
            return null;
        }
        return DailyTaskXmlInfoDic[id];
    }

    #endregion

    #region 永久任务表
    public static Dictionary<uint, STaskConfig> infiniteTaskXmlInfoDic = new Dictionary<uint, STaskConfig>();
    public static Dictionary<uint, STaskConfig> InfiniteTaskXmlInfoDic
    {
        get
        {
            if (infiniteTaskXmlInfoDic.Count < 1)
            {
                ReadTaskConfig msgConfig = new ReadTaskConfig("Config/AchievementTask",TASK_TIME_TYPE.TTT_Infinite);
            }
            return infiniteTaskXmlInfoDic;
        }

    }
    public static STaskConfig GetInfiniteTaskInfo(uint id)
    {
        if (!InfiniteTaskXmlInfoDic.ContainsKey(id))
        {
            return null;
        }
        return InfiniteTaskXmlInfoDic[id];
    }

    #endregion

    #region 其它物品
    public static Dictionary<uint, OtherItemConfigInfo> otherItemXmlInfoDic = new Dictionary<uint, OtherItemConfigInfo>();
    public static Dictionary<uint, OtherItemConfigInfo> OtherItemXmlInfoDic
    {
        get
        {
            if (otherItemXmlInfoDic.Count < 1)
            {
                ReadOtherItemConfig msgConfig = new ReadOtherItemConfig("Config/OtherItemCfg");
            }
            return otherItemXmlInfoDic;
        }
    }
    public static OtherItemConfigInfo GetOtherItemInfo(uint id)
    {
        if (!otherItemXmlInfoDic.ContainsKey(id))
        {
            return null;
        }
        return otherItemXmlInfoDic[id];
    }
    #endregion

    #region LoadTips
    public static Dictionary<int, LoadingTipData> loadingTipXmlInfoDic = new Dictionary<int, LoadingTipData>();
    public static Dictionary<int, LoadingTipData> LoadingTipXmlInfoDic
    {
        get
        {
            if (loadingTipXmlInfoDic.Count < 1)
            {
                ReadLoadingTipConfig tipConfig = new ReadLoadingTipConfig("Config/LoadingTips");
            }
            return loadingTipXmlInfoDic;
        }

    }
    public static LoadingTipData GetLoadingTipInfo(int id)
    {
        if (!LoadingTipXmlInfoDic.ContainsKey(id))
        {
            return null;
        }
        return LoadingTipXmlInfoDic[id];
    }
    #endregion

    #region buy skin
    public static Dictionary<int, BuySkinConfigInfo> buySkinXmlInfoDict = new Dictionary<int, BuySkinConfigInfo>();
    public static Dictionary<int, BuySkinConfigInfo> BuySkinXmlInfoDict
    {
        get
        {
            if (buySkinXmlInfoDict.Count < 1)
            {
                ReadBuySkinConfig skinConfig = new ReadBuySkinConfig("Config/SkinsPurchaseCfg");
            }
            return buySkinXmlInfoDict;
        }

    }
    public static BuySkinConfigInfo GetSkinInfo(int id)
    {
        if (!BuySkinXmlInfoDict.ContainsKey(id))
        {
            return null;
        }
        return BuySkinXmlInfoDict[id];
    }
    #endregion

    #region 符文表
    public static Dictionary<UInt32, RuneConfigInfo> runeXmlInfoDict = new Dictionary<UInt32, RuneConfigInfo>();
    public static Dictionary<UInt32, RuneConfigInfo> RuneXmlInfoDict
    {
        get
        {
            if (runeXmlInfoDict.Count == 0)
            {
                RuneConfig runeXmlInfoDictList = new RuneConfig("Config/RuneCfg");
            }
            return runeXmlInfoDict;
        }
    }

    public static RuneConfigInfo GetRuneFromID(UInt32 id)
    {
        if (!RuneXmlInfoDict.ContainsKey(id))
        {
            return null;
        }
        return RuneXmlInfoDict[id];
    }
    #endregion

    //#region mail gift config
    //public static Dictionary<int, MailGiftConfig> mailGiftCfgList = new Dictionary<int, MailGiftConfig>();
    //public static Dictionary<int, MailGiftConfig> MailGiftXmlInfoDict
    //{
    //    get
    //    {
    //        if (mailGiftCfgList.Count < 1)
    //        {
    //            ReadMailGiftConfig gift = new ReadMailGiftConfig("Config/MailGiftTypeCfg");
    //        }
    //        return mailGiftCfgList;
    //    }
    //}
    //public static MailGiftConfig GetMailGiftInfo(int id)
    //{
    //    if (!mailGiftCfgList.ContainsKey(id))
    //    {
    //        return null;
    //    }
    //    return mailGiftCfgList[id];
    //}
    //#endregion
    //#region mail type
    //public static Dictionary<int, MailTypeConfig> mailTypeList = new Dictionary<int, MailTypeConfig>();
    //public static Dictionary<int, MailTypeConfig> MailTypeXmlInfoDict
    //{
    //    get
    //    {
    //        if (mailTypeList.Count < 1)
    //        {
    //            ReadMailType gift = new ReadMailType("Config/MailTypeCfg");
    //        }
    //        return mailTypeList;
    //    }
    //}
    //public static MailTypeConfig GetMailTypeInfo(int id)
    //{
    //    if (!mailTypeList.ContainsKey(id))
    //    {
    //        return null;
    //    }
    //    return mailTypeList[id];
    //}
    //#endregion
}
