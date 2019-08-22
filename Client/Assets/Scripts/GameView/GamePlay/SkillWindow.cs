using UnityEngine;
using System.Collections;

using GameDefine;
using BlGame.GameEntity;
using System.Collections.Generic;
using System.Linq;
using BlGame.GuideDate;
using BlGame.Resource;
using BlGame.Ctrl;
using BlGame.Model;
namespace BlGame.View
{
    public class SkillWindow : BaseWindow
    {
        public SkillWindow()
        {
            mScenesType = EScenesType.EST_Play;
            mResName = GameConstDefine.LoadSkillUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            //技能按钮初始化
            Transform Adjust = mRoot.FindChild("Adjust");
            mBtnArray = new ButtonOnPress[Adjust.childCount];
            for (int i = 0; i < mBtnArray.Length; i++)
            {
                ButtonOnPress btn = mRoot.FindChild("Adjust/Button_" + i).GetComponent<ButtonOnPress>();
                mBtnArray[i] = btn;

                //事件注册
                switch ((ShortCutBarBtn)i)
                {
                    case ShortCutBarBtn.BTN_SKILL_1:
                        btn.AddListener(i, OnSkillBtnFunc);
                        btn.AddListener(i, OnSkillPress, ButtonOnPress.EventType.PressType);
                        UIGuideCtrl.Instance.AddUiGuideEventBtn(btn.gameObject);
                        break;
                    case ShortCutBarBtn.BTN_SKILL_2:
                    case ShortCutBarBtn.BTN_SKILL_3:
                    case ShortCutBarBtn.BTN_SKILL_4:
                    case ShortCutBarBtn.BTN_SKILL_5:
                    case ShortCutBarBtn.BTN_SKILL_6:
                        btn.AddListener(i, OnSkillBtnFunc);
                        btn.AddListener(i, OnSkillPress, ButtonOnPress.EventType.PressType);
                        break;
                    case ShortCutBarBtn.BTN_AUTOFIGHT:
                    case ShortCutBarBtn.BTN_CHANGELOCK:
                        btn.AddListener(i, OnCutBarBtnFunc, ButtonOnPress.EventType.ClickType);
                        UIGuideCtrl.Instance.AddUiGuideEventBtn(btn.gameObject);
                        break;
                }

                //状态控制组件初始化
                if (i < SkillCount)
                {
                    GameObject obj = btn.transform.FindChild("Light").gameObject;
                    mCanPressEffect.Add((ShortCutBarBtn)i, obj);

                    UISprite sprite = mRoot.FindChild("Adjust/Button_" + i + "/CutBar_" + i).GetComponent<UISprite>();
                    mBtnSprite.Add((ShortCutBarBtn)i, sprite);

                    ButtonSelectPic selectPic = btn.GetComponent<ButtonSelectPic>();
                    selectPic.state = ButtonSelectPic.SelectState.DisableState;
                    mBtnSelectPic.Add((ShortCutBarBtn)i, selectPic);
                }
            }

            //暴气技能初始化
            ResourceUnit effectUnit = ResourcesManager.Instance.loadImmediate(GameDefine.GameConstDefine.FurySkillBtnEffect, ResourceType.PREFAB);

            //技能1暴气
            mEffect3 = GameObject.Instantiate(effectUnit.Asset) as GameObject;
            mEffect3.transform.parent = mBtnArray[(int)ShortCutBarBtn.BTN_SKILL_3].transform;
            mEffect3.transform.localPosition = new Vector3(0f, 0f, -10f);

            //技能2暴气
            mEffect4 = GameObject.Instantiate(effectUnit.Asset) as GameObject;
            mEffect4.transform.parent = mBtnArray[(int)ShortCutBarBtn.BTN_SKILL_4].transform;
            mEffect4.transform.localPosition = new Vector3(0f, 0f, -10f);

            ChangeFuryState(EFuryState.eFuryNullState);

            //初始化技能按钮
            ShowValideUseSkillBtn(ShortCutBarBtn.BTN_SKILL_1, true);
            ShowValideUseSkillBtn(ShortCutBarBtn.BTN_SKILL_2, true);
            ShowValideUseSkillBtn(ShortCutBarBtn.BTN_SKILL_3, true);
            ShowValideUseSkillBtn(ShortCutBarBtn.BTN_SKILL_4, true);

            if (SceneGuideTaskManager.Instance().IsNewsGuide() == SceneGuideTaskManager.SceneGuideType.NoGuide)
            {
                mBtnSelectPic[ShortCutBarBtn.BTN_SKILL_5].gameObject.SetActive(false);
                mBtnSelectPic[ShortCutBarBtn.BTN_SKILL_6].gameObject.SetActive(false);
                ShowValideUseSkillBtn(ShortCutBarBtn.BTN_SKILL_5, false);
                ShowValideUseSkillBtn(ShortCutBarBtn.BTN_SKILL_6, false);
            }

            //CD初始化
            for (int i = 0; i < SkillCount; i++)
            {
                CdCountDown cd = mBtnArray[i].GetComponent<CdCountDown>();
                mCdDownDic.Add((ShortCutBarBtn)i, cd);
            }
            ResetSkill();

            mIsShowDes = false;

            mTimePressStart = 0f;
            mCurSkillPress = -1;

        }

        //窗口控件释放
        protected override void RealseWidget()
        {
            mCanPressEffect.Clear();
            mBtnSprite.Clear();
            mBtnSelectPic.Clear();
            mCdEndEffect.Clear();
            mCdEndEffectTime.Clear();
            mPressBtnEffect.Clear();
            mPressBtnEffectTime.Clear();

            mCdDownDic.Clear();
            mSkillCdList.Clear();
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<EFuryState>(EGameEvent.eGameEvent_FuryStateInfo, ChangeFuryState);
            EventCenter.AddListener<int, string, bool>(EGameEvent.eGameEvent_AbsorbResult, OnAbsorbResult);
            EventCenter.AddListener<SkillType, float, float>(EGameEvent.eGameEvent_LocalPlayerSkillCD, OnSkillCountDown);
            EventCenter.AddListener(EGameEvent.eGameEvent_LocalPlayerInit, OnLocalPlayerInit);
            EventCenter.AddListener<bool>(EGameEvent.eGameEvent_LocalPlayerSilence, OnLocalPlayerSilence);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<EFuryState>(EGameEvent.eGameEvent_FuryStateInfo, ChangeFuryState);
            EventCenter.RemoveListener<int, string, bool>(EGameEvent.eGameEvent_AbsorbResult, OnAbsorbResult);
            EventCenter.RemoveListener<SkillType, float, float>(EGameEvent.eGameEvent_LocalPlayerSkillCD, OnSkillCountDown);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LocalPlayerInit, OnLocalPlayerInit);
            EventCenter.RemoveListener<bool>(EGameEvent.eGameEvent_LocalPlayerSilence, OnLocalPlayerSilence);
        }

      

        //显示
        public override void OnEnable()
        {

        }

        //隐藏
        public override void OnDisable()
        {
            EventCenter.Broadcast<bool, SkillType, Iplayer>(EGameEvent.eGameEvent_SkillDescribleType, false, SkillType.SKILL_NULL, PlayerManager.Instance.LocalPlayer);
            mIsShowDes = false;
            mCurSkillPress = -1;
            mTimePressStart = 0f;
        }


        public override void Update(float deltaTime)
        {
            //清除技能描述
            CloseSkillDestribe();

            //yaz限帧
            mElapseTime += Time.deltaTime;
            if (mElapseTime > 0.2f)
            {
                mElapseTime = 0.0f;

                CheckToDeletePressEffect();
                CheckToDeleteCdEndEffect();
                ShowAllBtnCanUseEffect();
            }

            if (mTimePressStart == 0 || mCurSkillPress == -1)
                return;

            //显示技能描述
            if (Time.time - mTimePressStart >= mTimeLimit)
            {
                EventCenter.Broadcast<bool, SkillType, Iplayer>(EGameEvent.eGameEvent_SkillDescribleType, true, GetSkillType(mCurSkillPress), PlayerManager.Instance.LocalPlayer);

                    mIsShowDes = true;
                mTimePressStart = 0f;
                mCurSkillPress = -1;
            }
        }

        //关闭技能描述
        void CloseSkillDestribe()
        {
            bool isUp = false;
            if (Application.platform == RuntimePlatform.Android
                     || Application.platform == RuntimePlatform.IPhonePlayer
                     || Application.platform == RuntimePlatform.WP8Player
                     || Application.platform == RuntimePlatform.BlackBerryPlayer)
            {
                if (Input.touchCount <= 0)
                    return;
                Touch touch = Input.GetTouch(0);
                isUp = (touch.phase == TouchPhase.Canceled || touch.phase == TouchPhase.Ended);

            }
            else
            {
                isUp = Input.GetMouseButtonUp(0);
            }
            if (isUp)
            {

                EventCenter.Broadcast<bool, SkillType, Iplayer>(EGameEvent.eGameEvent_SkillDescribleType, false, SkillType.SKILL_NULL, PlayerManager.Instance.LocalPlayer);

                mCurSkillPress = -1;
                mTimePressStart = 0f;
            }

        }

        //设计技能图片
        public void SetSkillBtnPic(ShortCutBarBtn btntype, string spriteName)
        {
            mBtnSprite[btntype].spriteName = spriteName;

            if (btntype == ShortCutBarBtn.BTN_SKILL_5 || btntype == ShortCutBarBtn.BTN_SKILL_6)
            {
                if (spriteName == "")
                {
                    mBtnSprite[btntype].enabled = false;
                    mBtnSelectPic[btntype].gameObject.SetActive(false);
                    RemoveSkillCountDown(btntype);
                }
                else
                {
                    mBtnSelectPic[btntype].gameObject.SetActive(true);
                    mBtnSprite[btntype].enabled = true;
                }
            }
        }

        public void DisableSkillBtn()
        {
            for (int i = 0; i < SkillCount; i++)
            {
                mBtnSelectPic[(ShortCutBarBtn)i].ShowSelectPic(true);
            }
        }

        public void EnableSkillBtn()
        {
            for (int i = 0; i < SkillCount; i++)
            {
                mBtnSelectPic[(ShortCutBarBtn)i].ShowSelectPic(false);
            }
        }

        public void ChangeFuryState(EFuryState state)
        {
            if (state == EFuryState.eFuryRunState)
            {
                mBtnArray[(int)ShortCutBarBtn.BTN_SKILL_1].gameObject.SetActive(false);
                mBtnArray[(int)ShortCutBarBtn.BTN_SKILL_2].gameObject.SetActive(false);
                mBtnArray[(int)ShortCutBarBtn.BTN_SKILL_3].gameObject.SetActive(true);
                mBtnArray[(int)ShortCutBarBtn.BTN_SKILL_4].gameObject.SetActive(true);
            }
            else
            {
                mBtnArray[(int)ShortCutBarBtn.BTN_SKILL_1].gameObject.SetActive(true);
                mBtnArray[(int)ShortCutBarBtn.BTN_SKILL_2].gameObject.SetActive(true);
                mBtnArray[(int)ShortCutBarBtn.BTN_SKILL_3].gameObject.SetActive(false);
                mBtnArray[(int)ShortCutBarBtn.BTN_SKILL_4].gameObject.SetActive(false);
            }
        }

        bool IsHeroSkillBtn(int index)
        {
            if (ShortCutBarBtn.BTN_SKILL_1 == (ShortCutBarBtn)index ||
                ShortCutBarBtn.BTN_SKILL_2 == (ShortCutBarBtn)index ||
                ShortCutBarBtn.BTN_SKILL_3 == (ShortCutBarBtn)index ||
                ShortCutBarBtn.BTN_SKILL_4 == (ShortCutBarBtn)index)
                return true;
            return false;
        }

        private void OnSkillPress(int ie, bool isDown)
        {
            if (isDown)
            {
                mTimePressStart = Time.time;
                mCurSkillPress = ie;
                mIsShowDes = false;
                return;
            }

            EventCenter.Broadcast<bool, SkillType, Iplayer>(EGameEvent.eGameEvent_SkillDescribleType, false, SkillType.SKILL_NULL, PlayerManager.Instance.LocalPlayer);
            
            mCurSkillPress = -1;
            mTimePressStart = 0f;
        }

        private void OnSkillBtnFunc(int ie, bool isDown)//按技能
        {
            if (mIsShowDes)
            {
                mIsShowDes = false;
                return;
            }
            mIsShowDes = false;

            GamePlayCtrl.Instance.showaudiotimeold = System.DateTime.Now;
            if (PlayerManager.Instance.LocalPlayer.FSM == null ||
                PlayerManager.Instance.LocalPlayer.FSM.State == BlGame.FSM.FsmState.FSM_STATE_DEAD ||
                BlGame.Skill.BuffManager.Instance.isSelfHaveBuffType(1017))
            {
                return;
            }
            SendSkill(ie);
        }

        //按功能键
        private void OnCutBarBtnFunc(int ie, bool isDown)
        {
            if (isDown)
            {
                return;
            }
            switch ((ShortCutBarBtn)ie)
            {
                case ShortCutBarBtn.BTN_AUTOFIGHT:
                    CGLCtrl_GameLogic.Instance.GameAutoFight();
                    break;
                case ShortCutBarBtn.BTN_CHANGELOCK:
                    OnLockEnemySoldier();
                    break;
            }
        }

        //设置技能状态，是普通技能还是怒气技能
        private void SetSkillState(int ie)
        {
            if (!IsHeroSkillBtn(ie))
            {
                mSkillState = SkillState.NormalSkill;
                return;
            }

            if (PlayerManager.Instance.LocalPlayer.FuryState == EFuryState.eFuryRunState)
            {
                mSkillState = SkillState.FurySkill;
            }
            else
            {
                mSkillState = SkillState.NormalSkill;
            }
        }


        //使用技能
        private void SendSkill(int btn)
        {
            if (PlayerManager.Instance.LocalPlayer.FSM.State == BlGame.FSM.FsmState.FSM_STATE_DEAD)
                return;
            SkillType type = GetSkillType(btn);
            if (type == SkillType.SKILL_NULL)
            {
                return;
            }
            PlayerManager.Instance.LocalPlayer.SendPreparePlaySkill(type);
        }

        //快键到技能类型转换
        SkillType GetSkillType(int ie)
        {
            SetSkillState(ie);
            SkillType type = SkillType.SKILL_NULL;
            switch ((ShortCutBarBtn)ie)
            {
                case ShortCutBarBtn.BTN_SKILL_1:
                    type = SkillType.SKILL_TYPE1;
                    break;
                case ShortCutBarBtn.BTN_SKILL_2:
                    type = SkillType.SKILL_TYPE2;
                    break;
                case ShortCutBarBtn.BTN_SKILL_3:
                    type = SkillType.SKILL_TYPE3;
                    break;
                case ShortCutBarBtn.BTN_SKILL_4:
                    type = SkillType.SKILL_TYPE4;
                    break;
                case ShortCutBarBtn.BTN_SKILL_5:
                    type = SkillType.SKILL_TYPEABSORB1;
                    break;
                case ShortCutBarBtn.BTN_SKILL_6:
                    type = SkillType.SKILL_TYPEABSORB2;
                    break;
            }
            return type;
        }

        //快键到技能类型转换
        private ShortCutBarBtn GetBtnType(SkillType type)
        {
            ShortCutBarBtn btnType = ShortCutBarBtn.BTN_END;
            switch (type)
            {
                case SkillType.SKILL_TYPE1:
                    btnType = ShortCutBarBtn.BTN_SKILL_1;
                    break;
                case SkillType.SKILL_TYPE2:
                    btnType = ShortCutBarBtn.BTN_SKILL_2;
                    break;
                case SkillType.SKILL_TYPE3:
                    btnType = ShortCutBarBtn.BTN_SKILL_3;
                    break;
                case SkillType.SKILL_TYPE4:
                    btnType = ShortCutBarBtn.BTN_SKILL_4;
                    break;
                case SkillType.SKILL_TYPEABSORB1:
                    btnType = ShortCutBarBtn.BTN_SKILL_5;
                    break;
                case SkillType.SKILL_TYPEABSORB2:
                    btnType = ShortCutBarBtn.BTN_SKILL_6;
                    break;
            }
            return btnType;
        }


        /// <summary>
        /// 锁定目标
        /// </summary>
        /// <param name="ItemList"></param>
        private void OnLockEnemySoldier()
        {
            GameObject mGameObjectLock = mBtnArray[(int)ShortCutBarBtn.BTN_CHANGELOCK].gameObject;
            List<Ientity> itemList = new List<Ientity>();
            if (GamePlayGuideModel.Instance.IsGuideTrigger(ButtonTriggerType.mTypeClick, mGameObjectLock))
            {
                itemList = GameMethod.GetMonsterEnemyItemListByRadius(PlayerManager.Instance.LocalPlayer, GameConstDefine.PlayerLockTargetDis);
            }
            else
            {
                itemList = GameMethod.GetSoldierEnemyItemListByRadius(PlayerManager.Instance.LocalPlayer, GameConstDefine.PlayerLockTargetDis);
            }

            if (itemList == null || itemList.Count == 0)
            {
                return;
            }
            if (mSyncLockSelIndex >= itemList.Count)
            {
                mSyncLockSelIndex = 0;
            }
            PlayerManager.Instance.LocalPlayer.SetSyncLockTarget(itemList[mSyncLockSelIndex]);
            mSyncLockSelIndex++;
        }

        public void CreateEffect(Transform tran, string pathName)
        {
            ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate("effect/ui_effect/" + pathName, ResourceType.PREFAB);
            GameObject obj = objUnit.Asset as GameObject;

            if (obj == null)
            {
                return;
            }

            for (int i = 0; i < tran.childCount; i++)
                GameObject.DestroyImmediate(tran.GetChild(i).gameObject);

            GameObject ShowEffect = GameObject.Instantiate(obj) as GameObject;
            ShowEffect.transform.parent = tran;
            ShowEffect.transform.localPosition = Vector3.zero;
            ShowEffect.transform.localScale = Vector3.one;
        }

        public void ShowValideUseSkillBtn(ShortCutBarBtn btnType, bool visiable)
        {
            Iselfplayer player = PlayerManager.Instance.LocalPlayer;
            if (null == player)
            {
                return;
            }
            int index = (int)btnType;
            int skillId = 0;
            SkillManagerConfig info = null;
            switch (btnType)
            {
                case ShortCutBarBtn.BTN_SKILL_1:
                case ShortCutBarBtn.BTN_SKILL_2:
                case ShortCutBarBtn.BTN_SKILL_3:
                case ShortCutBarBtn.BTN_SKILL_4:
                    break;
                case ShortCutBarBtn.BTN_SKILL_5:
                    if (player != null && player.BaseSkillIdDic.ContainsKey(SkillType.SKILL_TYPEABSORB1))
                    {
                        skillId = player.BaseSkillIdDic[SkillType.SKILL_TYPEABSORB1];
                    }
                    info = ConfigReader.GetSkillManagerCfg(skillId);
                    if (skillId == 0 || info == null)
                    {
                        visiable = false;
                    }
                    break;
                case ShortCutBarBtn.BTN_SKILL_6:
                    if (player != null && player.BaseSkillIdDic.ContainsKey(SkillType.SKILL_TYPEABSORB2))
                    {
                        skillId = player.BaseSkillIdDic[SkillType.SKILL_TYPEABSORB2];
                    }
                    info = ConfigReader.GetSkillManagerCfg(skillId);
                    if (skillId == 0 || info == null)
                    {
                        visiable = false;
                    }
                    break;
                default:
                    return;
            }
            if (!visiable)
            {
                ShowCdEndEffect(btnType, false);
            }
        }

        public void ShowPressEffect(ShortCutBarBtn btnType)
        {
            if (IsSkillInCd(btnType) || mPressBtnEffect.ContainsKey(btnType))
            {
                return;
            }

            ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.PressBtnEffect, ResourceType.PREFAB);
            GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;

            int index = (int)btnType;
            obj.transform.parent = mBtnArray[index].transform;
            obj.transform.localPosition = Vector3.zero;
            obj.transform.localScale = Vector3.one;
            mPressBtnEffect.Add(btnType, obj);
            mPressBtnEffectTime.Add(btnType, 1.5f);
        }

        private void DeletePressEffect(ShortCutBarBtn btnType)
        {
            GameObject obj = null;
            if (mPressBtnEffect.TryGetValue(btnType, out obj))
            {
                GameObject.DestroyObject(obj);
                mPressBtnEffect.Remove(btnType);
            }
        }

        private float timePressCount = 0f;
        private void CheckToDeletePressEffect()
        {
            if (mPressBtnEffectTime == null || mPressBtnEffectTime.Count == 0) return;
            if (timePressCount == 0)
                timePressCount = Time.time;
            if (Time.time - timePressCount < 0.5f)
            {
                return;
            }
            timePressCount = Time.time;
            for (int i = mPressBtnEffect.Count - 1; i >= 0; i--)
            {
                ShortCutBarBtn type = mPressBtnEffectTime.ElementAt(i).Key;
                mPressBtnEffectTime[type] = mPressBtnEffectTime[type] - 0.5f;
                if (mPressBtnEffectTime[type] <= 0)
                {
                    mPressBtnEffectTime.Remove(type);
                    DeletePressEffect(type);
                }
            }
        }

        public void ShowCdEndEffect(ShortCutBarBtn btnType, bool show)
        {
            GameObject obj = null;
            if (show)
            {
                if (mCdEndEffect.ContainsKey(btnType))
                {
                    return;
                }

                ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.CdEndEffect, ResourceType.PREFAB);
                obj = GameObject.Instantiate(objUnit.Asset) as GameObject;


                int index = (int)btnType;
                obj.transform.parent = mBtnArray[index].transform;
                obj.transform.localPosition = Vector3.zero;
                obj.transform.localScale = Vector3.one;
                mCdEndEffect.Add(btnType, obj);
                mCdEndEffectTime.Add(btnType, 1.5f);
            }
            else
            {
                if (!mCdEndEffect.ContainsKey(btnType))
                {
                    return;
                }
                obj = null;
                if (mCdEndEffect.TryGetValue(btnType, out obj))
                {
                    GameObject.DestroyObject(obj);
                }
                mCdEndEffect.Remove(btnType);
                mCdEndEffectTime.Remove(btnType);
            }
        }

        private float timeCount = 0f;
        private void CheckToDeleteCdEndEffect()
        {
            if (mCdEndEffect == null || mCdEndEffect.Count == 0) return;
            if (timeCount == 0)
                timeCount = Time.time;
            if (Time.time - timeCount < 0.5f)
            {
                return;
            }
            timeCount = Time.time;
            for (int i = mCdEndEffect.Count - 1; i >= 0; i--)
            {
                ShortCutBarBtn type = mCdEndEffectTime.ElementAt(i).Key;
                mCdEndEffectTime[type] = mCdEndEffectTime[type] - 0.5f;
                if (mCdEndEffectTime[type] <= 0)
                {
                    ShowCdEndEffect(type, false);
                }
            }
        }

        private void ShowBtnCanUseEffect(ShortCutBarBtn type)
        {
            if (PlayerManager.Instance == null || PlayerManager.Instance.LocalPlayer == null)
                return;
            if (PlayerManager.Instance.LocalPlayer.SkillIdDic == null || PlayerManager.Instance.LocalPlayer.SkillIdDic.Count == 0)
                return;
            Iselfplayer player = PlayerManager.Instance.LocalPlayer;
            SkillType skillType = GetSkillType((int)type);
            int skillId = PlayerManager.Instance.LocalPlayer.SkillIdDic[skillType];
            SkillManagerConfig info = ConfigReader.GetSkillManagerCfg(skillId);
            if (info == null)
                return;
            GameObject sprite = mCanPressEffect[type];
            bool isInCd = IsSkillInCd(type);
            if (info.mpUse > player.Mp || info.cpUse > player.Cp || info.hpUse > player.Hp || isInCd)
            {
                if (sprite.activeInHierarchy)
                {
                    sprite.SetActive(false);
                }
            }
            else
            {
                if (!sprite.activeInHierarchy)
                {
                    sprite.SetActive(true);
                }
            }
        }

        private void ShowAllBtnCanUseEffect()
        {
            for (int i = 0; i < SkillCount; i++)
            {
                ShowBtnCanUseEffect((ShortCutBarBtn)i);
            }
        }

        // 开始技能CD显示，参数：按钮槽类型，总CD时间，CD时间；返回true 成功，否则 技能正在CD中
        public bool StartSkillCountDown(ShortCutBarBtn btnType, float totalTime, float lastTime)
        {
            CdCountDown cd;
            if (IsSkillInCd(btnType))
            {
                return false;
            }
            if (mCdDownDic.TryGetValue(btnType, out cd) && lastTime > 0)
            {
                ShowValideUseSkillBtn(btnType, false);
                ShowPressEffect(btnType);
                cd.StartCdCountDown(totalTime, lastTime);
                cd.CdCountDownEvent += SkillCdEnd;
                mSkillCdList.Add(btnType);
            }
            return true;
        }

        //清除某个技能CD
        public void RemoveSkillCountDown(ShortCutBarBtn type)
        {
            CdCountDown cd;
            if (mCdDownDic.TryGetValue(type, out cd) && IsSkillInCd(type))
            {
                cd.EndCdCountDown();
                RemoveCdList(type);
                ShowValideUseSkillBtn(type, true);
            }
        }


        //清除所有技能CD
        public void ResetSkill()
        {
            foreach (var item in mCdDownDic)
            {
                item.Value.EndCdCountDown();
                ShowValideUseSkillBtn(item.Key, true);
            }
            for (int i = mSkillCdList.Count - 1; i >= 0; i--)
            {
                RemoveCdList(mSkillCdList.ElementAt(i));
            }
        }

        public void ResetSkillAsignedSkill(ShortCutBarBtn btnType)
        {
            CdCountDown cdDown = null;
            if (mCdDownDic.TryGetValue(btnType, out cdDown) && IsSkillInCd(btnType))
            {
                cdDown.EndCdCountDown();
                RemoveCdList(btnType);
                ShowValideUseSkillBtn(btnType, true);
            }
        }

        // 监听某个技能CD结束
        private void SkillCdEnd(CdCountDown cd)
        {
            for (int i = mCdDownDic.Count - 1; i >= 0; i--)
            {
                if (mCdDownDic.ElementAt(i).Value == cd && IsSkillInCd(mCdDownDic.ElementAt(i).Key))
                {
                    ShowValideUseSkillBtn(mCdDownDic.ElementAt(i).Key, true);
                    RemoveCdList(mCdDownDic.ElementAt(i).Key);
                    break;
                }
            }
        }

        //判断某个技能是否在CD中
        public bool IsSkillInCd(ShortCutBarBtn type)
        {
            return mSkillCdList.Contains(type);
        }

        private void RemoveCdList(ShortCutBarBtn type)
        {
            if (!IsSkillInCd(type))
                return;
            ShowCdEndEffect(type, true);
            mSkillCdList.Remove(type);
        }

        private void OnAbsorbResult(int slot, string spriteName, bool remove)
        {
            if (remove == false)
            {
                if (slot == 0)
                {
                    CreateEffect(mBtnSprite[ShortCutBarBtn.BTN_SKILL_5].transform, "soul_01_burst");
                }
                else
                {
                    CreateEffect(mBtnSprite[ShortCutBarBtn.BTN_SKILL_6].transform, "soul_02_burst");
                }
            }

            SetSkillBtnPic((ShortCutBarBtn.BTN_SKILL_5 + slot), spriteName);//show absorb btn sprite
            ShowValideUseSkillBtn((ShortCutBarBtn.BTN_SKILL_5 + slot), !remove);//show absorb btn
        }

        private void OnSkillCountDown(SkillType type, float CDTime, float time)
        {
            if (time > 0)
            {
                StartSkillCountDown(GetBtnType(type), CDTime, time);
            }
            else
            {
                ResetSkillAsignedSkill(GetBtnType(type));
            }
        }

        private void OnLocalPlayerInit()
        {
            int id = (int)PlayerManager.Instance.LocalPlayer.ObjTypeID;
            HeroConfigInfo heroInfo = ConfigReader.GetHeroInfo(id);

            if (ConfigReader.GetSkillManagerCfg(heroInfo.HeroSkillType1) != null)
            {
                SetSkillBtnPic(ShortCutBarBtn.BTN_SKILL_1, ConfigReader.GetSkillManagerCfg(heroInfo.HeroSkillType1).skillIcon);
            }
            if (ConfigReader.GetSkillManagerCfg(heroInfo.HeroSkillType2) != null)
            {
                SetSkillBtnPic(ShortCutBarBtn.BTN_SKILL_2, ConfigReader.GetSkillManagerCfg(heroInfo.HeroSkillType2).skillIcon);
            }
            if (ConfigReader.GetSkillManagerCfg(heroInfo.HeroSkillType3) != null)
            {
                SetSkillBtnPic(ShortCutBarBtn.BTN_SKILL_3, ConfigReader.GetSkillManagerCfg(heroInfo.HeroSkillType3).skillIcon);
            }
            if (ConfigReader.GetSkillManagerCfg(heroInfo.HeroSkillType4) != null)
            {
                SetSkillBtnPic(ShortCutBarBtn.BTN_SKILL_4, ConfigReader.GetSkillManagerCfg(heroInfo.HeroSkillType4).skillIcon);
            }
            SetSkillBtnPic(ShortCutBarBtn.BTN_SKILL_5, "");
            SetSkillBtnPic(ShortCutBarBtn.BTN_SKILL_6, "");
        }

        private void OnLocalPlayerSilence(bool val)
        {
            if (val)
            {
                DisableSkillBtn();
            }
            else
            {
                EnableSkillBtn();
            }
        }


        //技能按钮索引
        public enum ShortCutBarBtn
        {
            BTN_SKILL_1 = 0,    //技能1
            BTN_SKILL_2,        //技能2
            BTN_SKILL_3,        //技能1暴气
            BTN_SKILL_4,        //技能2暴气
            BTN_SKILL_5,        //吸附技能1
            BTN_SKILL_6,        //吸附技能2
            BTN_AUTOFIGHT,
            BTN_CHANGELOCK,
            BTN_END,
        }

        private enum SkillState
        {
            NormalSkill,
            FurySkill,
        }

        private ButtonOnPress[] mBtnArray;       //技能按钮列表
        private Dictionary<ShortCutBarBtn, GameObject> mCanPressEffect = new Dictionary<ShortCutBarBtn, GameObject>();   //可点击特效列表
        private Dictionary<ShortCutBarBtn, UISprite> mBtnSprite = new Dictionary<ShortCutBarBtn, UISprite>();            //技能按钮图片列表
        private Dictionary<ShortCutBarBtn, ButtonSelectPic> mBtnSelectPic = new Dictionary<ShortCutBarBtn, ButtonSelectPic>();  //技能选择图片列表
        private Dictionary<ShortCutBarBtn, GameObject> mCdEndEffect = new Dictionary<ShortCutBarBtn, GameObject>();
        private Dictionary<ShortCutBarBtn, float> mCdEndEffectTime = new Dictionary<ShortCutBarBtn, float>();
        private Dictionary<ShortCutBarBtn, GameObject> mPressBtnEffect = new Dictionary<ShortCutBarBtn, GameObject>();
        private Dictionary<ShortCutBarBtn, float> mPressBtnEffectTime = new Dictionary<ShortCutBarBtn, float>();
        private Dictionary<ShortCutBarBtn, CdCountDown> mCdDownDic = new Dictionary<ShortCutBarBtn, CdCountDown>();
        private List<ShortCutBarBtn> mSkillCdList = new List<ShortCutBarBtn>();

        private GameObject mEffect3;
        private GameObject mEffect4;

        private float mTimePressStart = 0f;
        private const float mTimeLimit = 1.2f;
        private int mCurSkillPress = -1;
        private int mSyncLockSelIndex;
        private const int SkillCount = 6;
        private float mElapseTime = 0.0f;
        private bool mIsShowDes = false;

        SkillState mSkillState = SkillState.NormalSkill;
    }
}

