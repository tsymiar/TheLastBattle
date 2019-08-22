using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Ctrl;
using System;
using BlGame.GameEntity;
using BlGame.Resource;
using JT.FWW.Tools;
using System.Collections.Generic;
using JT.FWW.GameData;
namespace BlGame.View
{
    public class GamePlayWindow : BaseWindow
    {
        public GamePlayWindow()
        {
            mScenesType = EScenesType.EST_Play;
            mResName = GameConstDefine.LoadGameMainUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化

        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayExit, Exit);
        }

        void Exit() {
            mResident = false;
            Hide();
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayExit, Exit);
        }

         //窗口控件初始化
        protected override void InitWidget()
        {
            GamePlayCtrl.Instance.showaudiotimeold = System.DateTime.Now;
            CPLabel = mRoot.FindChild("GameRecord/Panel/CP/CpLabel").GetComponent<UILabel>();
            TimeLabel = mRoot.FindChild("GameRecord/Panel/Time/TimeLabel").GetComponent<UILabel>();
            DeadLabel = mRoot.FindChild("GameRecord/Panel/Dead/DeadLabel").GetComponent<UILabel>();
            KillLabel = mRoot.FindChild("GameRecord/Panel/Kill/KillLabel").GetComponent<UILabel>();
            AssistLabel = mRoot.FindChild("GameRecord/Panel/Assists/AssistsLabel").GetComponent<UILabel>();
            AssistLabel.text = "0";
            DeadLabel.text = "0";
            KillLabel.text = "0";
            spriteHomeBaseA = mRoot.FindChild("GameRecord/Panel/ElfLifeBar/xts").GetComponent<UISprite>();
            spriteHomeBaseB = mRoot.FindChild("GameRecord/Panel/UndeadLifeBar/xts").GetComponent<UISprite>();
            BattleInfo = mRoot.FindChild("ExtraButton/Adjust/InfoBtn").gameObject;
            ShopInfo = mRoot.FindChild("ExtraButton/Adjust/ShopBtn").gameObject;
            UIGuideCtrl.Instance.AddUiGuideEventBtn(ShopInfo);
            UIEventListener.Get(BattleInfo).onClick += BattlePlay;
            UIEventListener.Get(ShopInfo).onClick += ShopBtn;

            //LockHead
            mLockRoot = mRoot.FindChild("LockPhoto/Adjust/Head");
            spriteLock = mRoot.FindChild("LockPhoto/Adjust/Head").GetComponent<UISprite>();
            spriteLockBg1 = spriteLock.transform.FindChild("BG1").GetComponent<UISprite>();
            spriteLockBg2 = spriteLock.transform.FindChild("BG2").GetComponent<UISprite>();
            spriteLockHp = mRoot.FindChild("LockPhoto/Adjust/Head/HP").GetComponent<UISprite>();
            spriteLockMp = mRoot.FindChild("LockPhoto/Adjust/Head/MP").GetComponent<UISprite>();
            mLockPos = mLockRoot.localPosition;
            mLockRoot.localPosition = new Vector3(5000, 0, 0);
            UIGuideCtrl.Instance.AddUiGuideEventBtn(spriteLock.gameObject);
            UIEventListener.Get(spriteLock.gameObject).onClick += AbsorbLockHead;

            BackToCity = mRoot.FindChild("BackToCity/Adjust").gameObject;
            UIEventListener.Get(BackToCity).onClick += BackToCityBtn;

            //
            sliderHp = mRoot.FindChild("UI_AvatarInfos/Adjust/Avatar/bar red/Foreground").GetComponent<UISprite>();
            sliderMp = mRoot.FindChild("UI_AvatarInfos/Adjust/Avatar/bar blue/Foreground").GetComponent<UISprite>();
            sliderExp = mRoot.FindChild("UI_AvatarInfos/Adjust/Avatar/bar yellow/Foreground").GetComponent<UISprite>();

            labelHp = sliderHp.transform.parent.FindChild("Label").GetComponent<UILabel>();
            labelMp = sliderMp.transform.parent.FindChild("Label").GetComponent<UILabel>();
            labelExp = sliderExp.transform.parent.FindChild("Label").GetComponent<UILabel>();
            labelLevel = mRoot.FindChild("UI_AvatarInfos/Adjust/Avatar/level_Label").GetComponent<UILabel>();

            spriteHead = mRoot.FindChild("UI_AvatarInfos/Adjust/Avatar/touxiang_Sprite").GetComponent<UISprite>();
            spritePing = new UISprite[3];

            spritePing[(int)PingEnum.GreenTag] = mRoot.FindChild("UI_AvatarInfos/Adjust/Avatar/PingIcon/GreenIcon").GetComponent<UISprite>();
            spritePing[(int)PingEnum.YellowTag] = mRoot.FindChild("UI_AvatarInfos/Adjust/Avatar/PingIcon/YellowIcon").GetComponent<UISprite>();
            spritePing[(int)PingEnum.RedTag] = mRoot.FindChild("UI_AvatarInfos/Adjust/Avatar/PingIcon/RedIcon").GetComponent<UISprite>();
            for (int i = 0; i < spritePing.Length; i++)
            {
                spritePing[i].gameObject.SetActive(false);
            }
            isPress = false;
            sliderExp.fillAmount = 0f;
            mPassiveSkill = mRoot.FindChild("UI_AvatarInfos/Adjust/Button_7/CutBar_7").GetComponent<UISprite>();
            mSpritePassiveSkill = mRoot.FindChild("UI_AvatarInfos/Adjust/Button_7/Foreground").GetComponent<UISprite>();
            mSpritePassiveInfo = mRoot.FindChild("UI_AvatarInfos/Adjust/Button_7/Overlay").GetComponent<UILabel>();
            UIEventListener.Get(mPassiveSkill.transform.parent.gameObject).onPress += mPassivePress;

            mAltarSoldierHeadPoint = mRoot.FindChild("AltarManager");

            sliderFury = mRoot.FindChild("UI_AvatarInfos/Adjust/EnergyPool/FurySlider").GetComponent<UISprite>();
            btnFury = sliderFury.transform.parent.gameObject;
            UIEventListener.Get(btnFury).onClick += OnFuryClick;
            UIGuideCtrl.Instance.AddUiGuideEventBtn(btnFury.gameObject); 
        }

        private void OnFuryClick(GameObject go)
        {
            if (PlayerManager.Instance.LocalPlayer.FuryState == EFuryState.eFuryNullState)
            {
                MsgInfoManager.Instance.ShowMsg((int)EErrorCode.eEC_FuryNotEnough);
                return;
            }

            if (PlayerManager.Instance.LocalPlayer.FuryState == EFuryState.eFuryRunState)
            {
                MsgInfoManager.Instance.ShowMsg((int)EErrorCode.eEC_FuryRuning);
                return;
            }

            if (PlayerManager.Instance.LocalPlayer.FSM == null ||
               PlayerManager.Instance.LocalPlayer.FSM.State == BlGame.FSM.FsmState.FSM_STATE_DEAD)
            {
                return;
            }
            CGLCtrl_GameLogic.Instance.AskGasExplosion();
        }

        DateTime statePress;
        private void mPassivePress(GameObject go, bool state)
        {
            statePress = DateTime.Now;
            if (state)
            {
                isPress = true;
            }
            else
            {
                isPress = false;
                UnityEngine.GameObject.DestroyImmediate(desObj);
                desObj = null;
            }
        }

        private void ShopBtn(GameObject go)
        {
            GamePlayCtrl.Instance.OpenShop();
        }

        private void BackToCityBtn(GameObject go)
        {
            int backSk = ConfigReader.HeroXmlInfoDict[PlayerManager.Instance.LocalPlayer.NpcGUIDType].HeroSkillType6;
            GamePlayCtrl.Instance.AskUseSkill((UInt32)backSk);
           
        }

        private void AbsorbLockHead(GameObject go)
        {
            //是否打开吸附界面
            PlayerManager.Instance.LocalPlayer.SendAbsortMonster();
        }

        //窗口控件释放
        protected override void RealseWidget()
        {

        }

        private void BattlePlay(GameObject go)
        {
            GamePlayCtrl.Instance.AskBattleInfo();
        }

        GameObject desObj = null;
        void ShowDes()
        {
            if (statePress != DateTime.Now)
            {
                if (desObj == null)
                {
                    Iplayer player = PlayerManager.Instance.LocalPlayer;
                    if(player == null)
                        return;
                    int skillId = GetPassSkill(0, player);
                    SkillPassiveConfigInfo skillInfo = ConfigReader.GetSkillPassiveConfig((uint)skillId);
                    if (skillInfo == null)
                    {
                        Debug.LogError("skillPassive is null");
                        return;
                    }
                    ResourceUnit desObjUnit = ResourcesManager.Instance.loadImmediate(GameDefine.GameConstDefine.SkillDestribe, ResourceType.PREFAB);
                    desObj = GameObject.Instantiate(desObjUnit.Asset) as GameObject;
                    desObj.transform.parent = mPassiveSkill.transform;
                    desObj.transform.localScale = Vector3.one;
                    desObj.transform.localPosition = new Vector3(550, -450, 0);
                    UnityEngine.GameObject.DestroyImmediate(desObj.GetComponent<UIAnchor>());
                    UILabel skillCd = desObj.transform.FindChild("Skill_Cooldown").GetComponent<UILabel>();
                    UILabel skillDes = desObj.transform.FindChild("Skill_Describe").GetComponent<UILabel>();
                    UILabel skillLv = desObj.transform.FindChild("Skill_Level").GetComponent<UILabel>();
                    UILabel skillName = desObj.transform.FindChild("Skill_Name").GetComponent<UILabel>();
                    UILabel skillMpCost = desObj.transform.FindChild("Skill_HP").GetComponent<UILabel>();
                    if (skillInfo.isShowCoolTime)
                        skillCd.text = (skillInfo.coolTime / 1000f).ToString();
                    else
                        skillCd.text = "0";
                    skillDes.text = DestribeWithAttribue(skillInfo.info, player);
                    int bet = (int)skillInfo.id % 10;
                    if (bet == 0)
                        bet = 1;
                    skillLv.text = bet.ToString();
                    skillName.text = skillInfo.name;
                    if (skillInfo.Mp != 0)
                        skillMpCost.text = skillInfo.Mp.ToString();
                    skillMpCost.transform.FindChild("Label").GetComponent<UILabel>().gameObject.SetActive(skillInfo.Mp != 0);
                }
            }
        }

        string DestribeWithAttribue(string str, Iplayer player)
        {
            string tempStr = "";
            tempStr = str;
            if (!(str.Contains("mag") || str.Contains("phy")))
                return str;
            for (int i = 0; i < tempStr.Length; i++)
            {
                if (tempStr[i] != ']')
                {
                    continue;
                }
                int index = tempStr.LastIndexOf('[', i);
                string addStr = tempStr.Substring(index, i - index + 1);
                string[] strArray;
                if (addStr.Contains("mag") || addStr.Contains("phy"))
                {
                    strArray = addStr.Split(',');
                    strArray[1] = strArray[1].Remove(strArray[1].Length - 1, 1);
                    float attr = float.Parse(strArray[1]);
                    if (player == null)
                    {
                        return null;
                    }
                    float phyAttr = player.PhyAtk;
                    float magAttr = player.MagAtk;
                    attr = addStr.Contains("mag") ? (attr * magAttr) : (attr * phyAttr);
                    string attrAdd = attr >= 0 ? ("+" + attr.ToString()) : ("-" + attr.ToString());
                    attrAdd = "[00FF00]" + attrAdd + "[-]";
                    tempStr = tempStr.Replace(addStr, attrAdd);
                }

            }
            return tempStr;
        }

        public override void Update(float deltaTime)
        {
            UpdateHomeHp();
            CheckHomeBaseEffectEnd();
            if (isPress)
            {
                ShowDes();
            }
            deltaTime += Time.deltaTime;
            if (runSlowDown && sliderFury != null && sliderFury.fillAmount != 0)
            {
                float cut = 1f / slowDownLimit;
                if (Time.time - startTime >= 1f)
                {
                    if (sliderFury.fillAmount - cut >= 0f)
                        sliderFury.fillAmount -= cut;
                    else
                        sliderFury.fillAmount = 0f;
                    startTime = Time.time;
                }
            }
            else
            {
                runSlowDown = false;
            }
            if (GameTimeData.Instance.GetStartTag())
            {
                TimeSpan span = GameTimeData.Instance.GetGameTime();
                int sHour = span.Hours;
                int sMin = span.Minutes;
                int sSec = span.Seconds;
                TimeLabel.text = GameMethod.GetCurrSystemTime(sHour, sMin, sSec);
                deltaTime = 0.0f;
            }
            if (isDownTime)
            {
                LastTime = DateTime.Now;
                TimeSpan mSpan = LastTime - StartTime;
                int currTime = 0;
                if (mSpan.Hours != 0)
                {
                    currTime += mSpan.Hours * 3600;
                }
                if (mSpan.Minutes != 0)
                {
                    currTime += mSpan.Minutes * 60;
                }
                currTime += mSpan.Seconds;

                if (currTime <= skillEndTime)
                {
                    mSpritePassiveSkill.fillAmount = 1 - (float)currTime/(float)skillEndTime;
                }
                else
                {
                    isDownTime = false;
                }
            }
        }

        private void CheckHomeBaseEffectEnd()
        {
            if (objHomeBase == null)
                return;
            if (Time.time - timeCheck >= homeCheckTime)
            {
                GameObject.DestroyObject(objHomeBase);
            }
        }

        private void UpdateHomeHp()
        {
            //更新A阵营
            Ientity entity = EntityManager.GetHomeBase(EntityCampType.CampTypeA);
            if (entity != null)
            {
                spriteHomeBaseA.fillAmount = entity.Hp / entity.HpMax;
            }

            //更新B阵营
            entity = EntityManager.GetHomeBase(EntityCampType.CampTypeB);
            if (entity != null)
            {
                spriteHomeBaseB.fillAmount = entity.Hp / entity.HpMax;
            }
        }

        public void ShowHomeBaseBeAtkEffect()
        {
            timeCheck = Time.time;
            if (objHomeBase != null)
                return;

            //objHomeBase = GameObject.Instantiate(Resources.Load(GameConstDefine.HomeBaseBeAtkEffect)) as GameObject;
            ResourceUnit objHomeBaseUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.HomeBaseBeAtkEffect, ResourceType.PREFAB);
            objHomeBase = GameObject.Instantiate(objHomeBaseUnit.Asset) as GameObject;

            objHomeBase.transform.parent = mRoot.FindChild("GameRecord/Panel/Bk/Frame");
            objHomeBase.transform.localPosition = Vector3.zero;
            objHomeBase.transform.localScale = Vector3.one;
        }

        public void PlayAudioShow()
        {
            DateTime nowtime = System.DateTime.Now;
            TimeSpan timecha = nowtime - GamePlayCtrl.Instance.showaudiotimeold;
            if (timecha.Seconds > 20)
            {
                Iplayer player = PlayerManager.Instance.LocalPlayer;
                if (player == null)
                {
                    Debug.LogError("error player is null");
                    return;
                }
                UInt64 myself_guid = player.GameObjGUID;
                MsgInfoManager.Instance.SetAudioPlay(myself_guid, MsgInfoManager.AudioPlayType.TwentySconde);
            }
        }

        void SetCp()
        {
            Iplayer player = PlayerManager.Instance.LocalPlayer;
            if (player == null)
                return;
            Dictionary<UInt64, HeroBattleInfo> CpDic = BattleingData.Instance.GetCamp(player.EntityCamp);
            if (CpDic.ContainsKey(player.GameObjGUID))
            {
                CPLabel.text = CpDic[player.GameObjGUID].Cp.ToString();
            }
        }

        void SetDead()
        {
            Iplayer player = PlayerManager.Instance.LocalPlayer;
            if (player == null)
                return;
            Dictionary<UInt64, HeroBattleInfo> DeathsDic = BattleingData.Instance.GetCamp(player.EntityCamp);
            if (DeathsDic.ContainsKey(player.GameObjGUID))
            {
                DeadLabel.text = DeathsDic[player.GameObjGUID].Deaths.ToString();
            }
        }


        void SetKill()
        {
            Iplayer player = PlayerManager.Instance.LocalPlayer;
            if (player == null)
                return;
            Dictionary<UInt64, HeroBattleInfo> KillsDic = BattleingData.Instance.GetCamp(player.EntityCamp);
            if (KillsDic.ContainsKey(player.GameObjGUID))
            {
                KillLabel.text = KillsDic[player.GameObjGUID].Kills.ToString();
            }
        }

        void SetAssist()
        {
            Iplayer palyer = PlayerManager.Instance.LocalPlayer;
            if (palyer == null)
                return;
            Dictionary<UInt64, HeroBattleInfo> AssistDic = BattleingData.Instance.GetCamp(palyer.EntityCamp);
            if (AssistDic.ContainsKey(palyer.GameObjGUID))
            {
                AssistLabel.text = AssistDic[palyer.GameObjGUID].Assist.ToString();
            }
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_NotifyChangeKills, SetKill);
            EventCenter.AddListener(EGameEvent.eGameEvent_LocalPlayerCp, SetCp);
            EventCenter.AddListener(EGameEvent.eGameEvent_NotifyChangeDeaths, SetDead);
            EventCenter.AddListener(EGameEvent.eGameEvent_LocalPlayerAssist, SetAssist);

            EventCenter.AddListener<Ientity>(EGameEvent.eGameEvent_LockTarget, UpdateLockHead);
            EventCenter.AddListener<Ientity>(EGameEvent.eGameEvent_EntityHpChange, UpdateHpSlider);
            EventCenter.AddListener<Ientity>(EGameEvent.eGameEvent_EntityMpChange, UpdateMpSlider);
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_SSPingInfo, OnEvent);

            EventCenter.AddListener(EGameEvent.eGameEvent_LocalPlayerHpChange, UpdateHpSlider);
            EventCenter.AddListener(EGameEvent.eGameEvent_LocalPlayerMpChange, UpdateMpSlider);
            EventCenter.AddListener(EGameEvent.eGameEvent_LocalPlayerExpChange, UpdateExp);

            EventCenter.AddListener<int>(EGameEvent.eGameEvent_LocalPlayerLevelChange, UpdateLevel);

            EventCenter.AddListener(EGameEvent.eGameEvent_LocalPlayerInit, InitHead);
            EventCenter.AddListener<int, int>(EGameEvent.eGameEvent_LocalPlayerPassiveSkillsUpLv, UpdatePassiveSkills);
            EventCenter.AddListener<int>(EGameEvent.eGameEvent_LocalPlayerPassiveSkills, SetPassSkill);
            EventCenter.AddListener(EGameEvent.eGameEvent_LocalPlayerUpdateFuryValue, UpdateFuryValue);
            EventCenter.AddListener(EGameEvent.eGameEvent_LocalPlayerUpdateFuryEffect, UpdateFuryEffect);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_NotifyChangeKills, SetKill);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LocalPlayerCp, SetCp);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_NotifyChangeDeaths, SetDead);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LocalPlayerAssist, SetAssist);

            EventCenter.RemoveListener<Ientity>(EGameEvent.eGameEvent_LockTarget, UpdateLockHead);
            EventCenter.RemoveListener<Ientity>(EGameEvent.eGameEvent_EntityHpChange, UpdateHpSlider);
            EventCenter.RemoveListener<Ientity>(EGameEvent.eGameEvent_EntityMpChange, UpdateMpSlider);
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_SSPingInfo, OnEvent);

            EventCenter.RemoveListener(EGameEvent.eGameEvent_LocalPlayerHpChange, UpdateHpSlider);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LocalPlayerMpChange, UpdateMpSlider);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LocalPlayerExpChange, UpdateExp);
            EventCenter.RemoveListener<int>(EGameEvent.eGameEvent_LocalPlayerLevelChange, UpdateLevel);

            EventCenter.RemoveListener(EGameEvent.eGameEvent_LocalPlayerInit, InitHead);
            EventCenter.RemoveListener<int,int>(EGameEvent.eGameEvent_LocalPlayerPassiveSkillsUpLv, UpdatePassiveSkills);
            EventCenter.RemoveListener<int>(EGameEvent.eGameEvent_LocalPlayerPassiveSkills, SetPassSkill);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LocalPlayerUpdateFuryValue, UpdateFuryValue);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LocalPlayerUpdateFuryEffect, UpdateFuryEffect);
        }

        private void UpdatePassiveSkills(int skillID,int time)
        {
            Iplayer player = PlayerManager.Instance.LocalPlayer;
            if (player == null)
                return;
            int skillId = GetPassSkill(0, player);
            if (skillId == skillID)
            {
                isDownTime = true;
                StartTime = System.DateTime.Now;
                skillEndTime = time / 1000f;
                if (!ShowPassSkill(skillId))
                    return;
                mSpritePassiveSkill.gameObject.SetActive(true);
            }
        }

        bool ShowPassSkill(int skillId)
        {
            SkillPassiveConfigInfo skillInfo = ConfigReader.GetSkillPassiveConfig((uint)skillId);
            if (skillInfo == null)
            {
                Debug.LogError(" ID = " + skillId);
                return false;
            }
            mPassiveSkill.spriteName = skillInfo.icon;
            return true;
        }

        void SetPassSkill(int skillId)
        {
            Iplayer player  = PlayerManager.Instance.LocalPlayer;
            if(player == null)
                return;
            int id = GetPassSkill(skillId, player);
            if (!ShowPassSkill(id))
                return;
        }

        private int GetPassSkill(int skillId,Iplayer player)
        {
            HeroConfigInfo heroInfo = null;
            int id = (int)player.ObjTypeID;
            if (skillId == 0)
            {
                heroInfo = ConfigReader.GetHeroInfo(id);
                PassiveTemp = heroInfo.HeroSkillTypeP;
                if (SkillLevel3 <= player.Level)
                    return PassiveTemp + 2;
                else if (SkillLevel2 <= player.Level)
                    return PassiveTemp + 1;
                else if (player.Level < SkillLevel2)
                    return PassiveTemp;
            }
            heroInfo = ConfigReader.GetHeroInfo(skillId);
            return heroInfo.HeroSkillTypeP;
        }
        
        void ShowPingIcon(float ping)
        {
            PingEnum tag = PingEnum.RedTag;
            if (ping >= 0f && ping < 150f)
            {
                tag = PingEnum.GreenTag;
            }
            else if (ping < 300f)
            {
                tag = PingEnum.YellowTag;
            }
            for (int i = 0; i < spritePing.Length; i++)
            {
                if ((PingEnum)i == tag)
                {
                    spritePing[i].gameObject.SetActive(true);
                }
                else
                {
                    spritePing[i].gameObject.SetActive(false);
                }
            }
        }

        public void UpdateHpSlider()
        {
            sliderHp.fillAmount = PlayerManager.Instance.LocalPlayer.Hp / PlayerManager.Instance.LocalPlayer.HpMax;
            int hp = (int)PlayerManager.Instance.LocalPlayer.Hp;
            int hpMax = (int)PlayerManager.Instance.LocalPlayer.HpMax;
            labelHp.text = hp.ToString() + "/" + hpMax.ToString();
        }

        public void UpdateMpSlider()
        {
            sliderMp.fillAmount = PlayerManager.Instance.LocalPlayer.Mp / PlayerManager.Instance.LocalPlayer.MpMax;
            int mp = (int)PlayerManager.Instance.LocalPlayer.Mp;
            int mpMax = (int)PlayerManager.Instance.LocalPlayer.MpMax;
            labelMp.text = mp.ToString() + "/" + mpMax.ToString();
        }

        public void UpdateLevel(int level)
        {
            labelLevel.text = level.ToString();
            if (level == SkillLevel3)
            {
                UpdatePassiveSkills(PassiveTemp + 2, 0);
            }
            else if (level == SkillLevel2)
            {
                UpdatePassiveSkills(PassiveTemp + 1, 0);
            }
            UpdateExp();
        }

        const int SkillLevel3 = 11;
        const int SkillLevel2 = 6;
        public void UpdateExp()
        {
            HeroConfigInfo info = ConfigReader.GetHeroInfo(PlayerManager.Instance.LocalPlayer.NpcGUIDType);
            if (PlayerManager.Instance.LocalPlayer.Level >= CommonDefine.MAX_LEVEL)
            {
                sliderExp.fillAmount = 1.0f;
                labelExp.text = "";
                return;
            }
            int exp = (int)PlayerManager.Instance.LocalPlayer.Exp;
            float expMax = info.HeroBaseExp + (PlayerManager.Instance.LocalPlayer.Level - 1) * info.HeroExpGrowth;

            sliderExp.fillAmount = PlayerManager.Instance.LocalPlayer.Exp / expMax;
            labelExp.text = exp.ToString() + "/" + Convert.ToInt32(expMax).ToString();
        }


        void OnEvent(CEvent eve)
        {
            switch (eve.GetEventId())
            {
                case EGameEvent.eGameEvent_SSPingInfo:
                    float ping = (float)eve.GetParam("ping");
                    ShowPingIcon(ping);
                    break;
            }
        }
        private GameObject lockEffect = null;
        private void ShowLockEffect(bool show)
        {
            if (lockEffect == null)
            {
                LoadLockEffect();
            }

            lockEffect.SetActive(show);

        }
        public void InitHead()
        {
            HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo(PlayerManager.Instance.LocalPlayer.NpcGUIDType);

            string head = info.HeroSelectHead.ToString();

            spriteHead.spriteName = head;
        }	
        private void LoadLockEffect()
        {
            if (null == lockEffect)
            {
                lockEffect = LoadUiResource.LoadRes(spriteLock.transform.parent, GameDefine.GameConstDefine.LockEffect);
                lockEffect.transform.localPosition = spriteLock.transform.localPosition;
            }
        }
        private void UpdateLockHead(Ientity entity)
        {
            bool mCanAbsorb = false;
            if (entity == null)
            {
                CurrLockEntity = null;
                mLockRoot.localPosition = new Vector3(5000, 0, 0);
                ShowLockEffect(false);
                EventCenter.Broadcast<string>(EGameEvent.eGameEvent_ResetLockHead, null);
            }
            else
            {
                string head = "";
                CurrLockEntity = entity;
                spriteLockBg2.gameObject.SetActive(false);
                if (entity.entityType == EntityType.Player)
                {
                    HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo(entity.NpcGUIDType);
                    head = info.HeroSelectHead.ToString();
                }
                else
                {
                    NpcConfigInfo info = ConfigReader.GetNpcInfo(entity.NpcGUIDType);
                    head = info.HeadPhoto.ToString();
                    if (entity.entityType == EntityType.Monster || entity.entityType == EntityType.Soldier || entity.entityType == EntityType.AltarSoldier && !PlayerManager.Instance.IsLocalSameType(entity))
                    {
                        if (entity.XueTiao != null)
                        {
                            entity.XueTiao.IsXueTiaoCpVib(true);
                        }
                        Iselfplayer player = PlayerManager.Instance.LocalPlayer;
                        if (player != null && player.AbsorbMonsterType[0] != entity.NpcGUIDType && player.AbsorbMonsterType[1] != entity.NpcGUIDType)
                        {
                            if (entity.NPCCateChild == ENPCCateChild.eNPCChild_SmallMonster || entity.NPCCateChild == ENPCCateChild.eNPCChild_HugeMonster
                                || entity.entityType == EntityType.AltarSoldier && !PlayerManager.Instance.IsLocalSameType(entity))
                            {
                                spriteLockBg2.gameObject.SetActive(true);
                                mCanAbsorb = true;
                            }
                        } 
                    }
                }
                spriteLockHp.fillAmount = entity.Hp / entity.HpMax;
                spriteLockMp.fillAmount = entity.Mp / entity.MpMax;
                EventCenter.Broadcast<string>(EGameEvent.eGameEvent_ResetLockHead, head);
                spriteLock.spriteName = head;
                mLockRoot.localPosition = mLockPos;
                ShowLockEffect(true);
                EventCenter.Broadcast<GameObject>((mCanAbsorb) ? EGameEvent.eGameEvent_GuideLockTargetCanAbsorb : EGameEvent.eGameEvent_GuideLockTargetCanNotAbsorb, spriteLockBg2.gameObject);
            }

        }
        //显示
        public override void OnEnable()
        {
            GameTimeData.Instance.StartCountTime();
            PlayAudioShow();
            UpdateFuryValue();
        }

        //隐藏
        public override void OnDisable()
        {
            
        }

        public void UpdateHpSlider(Ientity entity)
        {
            if (CurrLockEntity != null && entity != null && entity == CurrLockEntity)
                spriteLockHp.fillAmount = entity.Hp / entity.HpMax;
        }

        public void UpdateMpSlider(Ientity entity)
        {
            if (CurrLockEntity != null && entity != null && entity == CurrLockEntity)
                spriteLockMp.fillAmount = entity.Mp / entity.MpMax;
        }

        public UIAltarInHead CreateAltarUIPrefab(AltarInHead target, int type)
        {
            GameObject obj = GameObject.Instantiate(Resources.Load(GameConstDefine.pathAltarHead)) as GameObject;
            obj.gameObject.transform.parent = mAltarSoldierHeadPoint;
            obj.transform.localScale = Vector3.one;
            obj.transform.localPosition = Vector3.zero;

            UIAltarInHead altar = obj.GetComponent<UIAltarInHead>();
            altar.SetCurrHeadIcon(type, target.transform);
            return altar;
        }

        /// <summary>
        /// 爆气
        /// </summary>

        public void UpdateFuryValue()
        {
            if (PlayerManager.Instance == null || PlayerManager.Instance.LocalPlayer == null)
                return;
            int furyValue = PlayerManager.Instance.LocalPlayer.FuryValue;
            if (PlayerManager.Instance.LocalPlayer.FuryState != EFuryState.eFuryRunState)
            {
                int fury = furyValue % Iselfplayer.PerFuryValue;
                if (furyValue != Iselfplayer.PerFuryValue)
                    sliderFury.fillAmount = (float)fury / (float)Iselfplayer.PerFuryValue;
                else
                    sliderFury.fillAmount = 1f;
            }

        }

        public void UpdateFuryEffect()
        {
            if (PlayerManager.Instance.LocalPlayer == null)
            {
                return;
            }

            EFuryState state = PlayerManager.Instance.LocalPlayer.FuryState;
            switch (state)
            {
                case EFuryState.eFuryRunState:
                    LoadRunFuryEffect();
                    DestroyFullFuryEffect();
                    break;
                case EFuryState.eFuryNullState:
                    DestroyFullFuryEffect();
                    DestroyRunFuryEffect();
                    break;
                case EFuryState.eFuryFullState:
                    LoadFullFuryEffect();
                    DestroyRunFuryEffect();
                    break;
            }
        }


        private void LoadRunFuryEffect()
        {
            if (null == furyRunEffect)
            {
                furyRunEffect = LoadUiResource.LoadRes(btnFury.transform, GameDefine.GameConstDefine.FuryHeadInfoEffect);
                furyRunEffect.transform.localPosition = new Vector3(sliderFury.transform.localPosition.x, sliderFury.transform.localPosition.y, sliderFury.transform.localPosition.z - 10f);
                runSlowDown = true;
                startTime = Time.time;
            }
        }

        private void DestroyRunFuryEffect()
        {
            if (null != furyRunEffect)
            {
                LoadUiResource.DestroyLoad(GameDefine.GameConstDefine.FuryHeadInfoEffect);
                furyRunEffect = null;
            }
        }

        private void LoadFullFuryEffect()
        {
            if (null == furyFullEffect)
            {
                furyFullEffect = LoadUiResource.LoadRes(btnFury.transform, GameDefine.GameConstDefine.FuryFullFuryInfoEffect);
                furyFullEffect.transform.localPosition = new Vector3(sliderFury.transform.localPosition.x, sliderFury.transform.localPosition.y, sliderFury.transform.localPosition.z - 10f);
                startTime = Time.time;
            }
        }

        private void DestroyFullFuryEffect()
        {
            if (null != furyFullEffect)
            {
                LoadUiResource.DestroyLoad(GameDefine.GameConstDefine.FuryFullFuryInfoEffect);
                furyFullEffect = null;
            }
        }

        UILabel mSpritePassiveInfo = null;
        DateTime LastTime;
        bool isDownTime = false;
        DateTime StartTime;//触发被动技能开始时间
        float skillEndTime = 0;
        UISprite mPassiveSkill = null;
        UISprite mSpritePassiveSkill = null;
        int PassiveTemp = 0;

        Ientity CurrLockEntity = null;

        GameObject BattleInfo = null;
        GameObject ShopInfo = null;
        //实时显示
        float timeCheck = 0f;
        const float homeCheckTime = 3f;
        UILabel CPLabel;
        UILabel TimeLabel;
        UILabel DeadLabel;
        UILabel KillLabel;
        UILabel AssistLabel;
        UISprite spriteHomeBaseA;
        UISprite spriteHomeBaseB;
        GameObject objHomeBase = null;

        GameObject BackToCity;

        private Transform mLockRoot;
        private UISprite spriteLock;
        private UISprite spriteLockBg1;
        private UISprite spriteLockBg2;
        private UISprite spriteLockHp;
        private UISprite spriteLockMp;

        //英雄信息
        private UISprite spriteHead;

        private UILabel labelLevel;

        private UISprite sliderHp;
        private UILabel labelHp;

        private UISprite sliderMp;
        private UILabel labelMp;

        private UISprite sliderExp;
        private UILabel labelExp;

        private UISprite[] spritePing;
        private bool isPress;

        private Transform mAltarSoldierHeadPoint;

        private UISprite sliderFury = null;

        private GameObject furyRunEffect = null;

        private GameObject furyFullEffect = null;

        private GameObject btnFury = null;

        private bool runSlowDown = false;

        private float startTime;

        private const float slowDownLimit = 18f;

        private Vector3 mLockPos;

        private enum PingEnum
        {
            GreenTag = 0,
            YellowTag,
            RedTag,
        }
    }
}
