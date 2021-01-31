using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using JT.FWW;
using BlGame.GameData;
using JT.FWW.Tools;
using JT.FWW.GameData;
using BlGame.GameEntity;
using GameDefine;
using UICommon;
using BlGame.GuideDate;
using BlGame.Resource;
using BlGame.Ctrl;
using BlGame.Model;

namespace BlGame.View
{

    public class HeroWindow : BaseWindow
    {

        public HeroWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.HeroSelectUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroExit, Hide);

            EventCenter.AddListener<int>(EGameEvent.eGameEvent_HeroFirstTime, SetFirstTime);
            EventCenter.AddListener<int>(EGameEvent.eGameEvent_HeroSecondTime, SetSecondTime);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroExit, Hide);

            EventCenter.RemoveListener<int>(EGameEvent.eGameEvent_HeroFirstTime, SetFirstTime);
            EventCenter.RemoveListener<int>(EGameEvent.eGameEvent_HeroSecondTime, SetSecondTime);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mMidShow = mRoot.FindChild("Animator_Area");
            mLockList.Clear();
            mHeroShowList.Clear();
            mSpriteBlackList.Clear();
            for (int i = 0; i < HeroCtrl.Instance.heroInfoList.Count; i++)
            {
                #region  加载选择英雄显示预设
                GameObject objLoad = null;
                //objLoad = GameObject.Instantiate(Resources.Load(GameConstDefine.HeroShowBoxUI)) as GameObject;

                ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.HeroShowBoxUI, ResourceType.PREFAB);
                objLoad = GameObject.Instantiate(objUnit.Asset) as GameObject;

                objLoad.transform.parent = mMidShow.FindChild("HeroBox").FindChild("Grid");
                objLoad.transform.localScale = Vector3.one;
                objLoad.transform.localPosition = Vector3.zero;
                objLoad.name = "HeroBox" + (i + 1).ToString();

                UIToggle toggle = objLoad.GetComponent<UIToggle>();
                toggle.group = mHeroShowGroup;
                mHeroShowList.Add(toggle);
                #endregion

                #region 显示英雄选择ICON图片
                //black
                mShowAlpha = mRoot.FindChild("Animator_Area");
                Transform selecParent = objLoad.transform.FindChild("HeroThumbnails");
                mSpriteBlackList.Add(selecParent.FindChild("Clip").GetComponent<UISprite>());
                //head
                UISprite sprite = selecParent.FindChild("Head").GetComponent<UISprite>();
                sprite.spriteName = HeroCtrl.Instance.heroInfoList[i].HeroSelectHead.ToString();
                //lock
                Transform lockT = objLoad.transform.FindChild("Lock");
                lockT.gameObject.SetActive(false);
                mLockList.Add(lockT);
                #endregion

            }
            #region 获得描述预设
            Transform desParent = mMidShow.FindChild("HeroDestribe");
            mHeroName = desParent.FindChild("Name").FindChild("Label").GetComponent<UILabel>();
            Transform skillTran = desParent.FindChild("Skill");
            mSkillDes = skillTran.FindChild("Instruction").GetComponent<UILabel>();
            mSkillDes2 = skillTran.FindChild("Instruction2").GetComponent<UILabel>();
            mSkillDes3 = skillTran.FindChild("Instruction3").GetComponent<UILabel>();
            mSpriteDes = desParent.FindChild("Portrait").FindChild("Sprite").GetComponent<UISprite>();
            mSkilltex = skillTran.FindChild("Icon1").GetComponent<UISprite>();
            mSkilltex1 = skillTran.FindChild("Icon2").GetComponent<UISprite>();
            mSkilltex2 = skillTran.FindChild("Icon3").GetComponent<UISprite>();
            #endregion

            #region 获得英雄头像预设
            mTeamLeft = mRoot.FindChild("TeamSelectInfo");
            mTeamRight = mRoot.FindChild("EnemySelectInfo");
            int index = 1;
            for (int j = 0; j < mHeroCount; j++)
            {

                Transform teamParent = null;
                if (j % 2 == 0)
                {  //左边三个 j = 0,2,4 , i = 1,2,3			 
                    teamParent = mTeamLeft;
                }
                else
                {//右边三个 j = 1,3,5, i = 1,2,3				 
                    teamParent = mTeamRight;
                }

                Transform parent = teamParent.FindChild("Player" + index.ToString());
                mSpriteDic[j] = parent.FindChild("Thumbnail").GetComponent<UISprite>();
                mLabelName[j] = parent.FindChild("Name").GetComponent<UILabel>();
                mLabelName[j].text = "";
                mSpriteSelectDic[j] = parent.FindChild("Frame").GetComponent<UISprite>();
                mLabelDic[j] = parent.FindChild("ConfirmBar");

                if (j % 2 != 0)
                {
                    index += 1;
                }
            }
            #endregion

            #region 确定
            mBtnSure = mMidShow.FindChild("ConfirmButton").GetComponent<ButtonOnPress>();
            mObjHightLight = mBtnSure.transform.FindChild("Highlight").gameObject;
            mObjCanPress = mBtnSure.transform.FindChild("Show").gameObject;
            #endregion


            #region 购买界面
            mBuyParent = mRoot.FindChild("SureToBuyHero");
            Transform buyParentHero = mBuyParent.FindChild("Hero");
            mSpriteBuyDes = buyParentHero.FindChild("Portrait").GetComponent<UISprite>();
            mLabelBuyName = buyParentHero.FindChild("Name").GetComponent<UILabel>(); ;
            mLabelBuySkill = buyParentHero.FindChild("Skill").FindChild("Instruction").GetComponent<UILabel>();
            mLabelBuyDes = buyParentHero.FindChild("Story").GetComponent<UILabel>();
            mBtnCloseBuy = mBuyParent.FindChild("Background").GetComponent<ButtonOnPress>();
            mBtnBuy = mBuyParent.FindChild("Buy").GetComponent<ButtonOnPress>();

            //符文
            mSkinsToggle = mRoot.FindChild("TurnPage/SelectSkins").GetComponent<UIToggle>();
            mSkinsDisable = mRoot.FindChild("TurnPage/SelectSkins/Disable").gameObject;
            mSkinsSubmit = mRoot.FindChild("SkinsInterface/ConfirmButton").GetComponent<UIButton>();
            mRunePages = mRoot.FindChild("SkinsInterface/RunePages").GetComponent<UIPopupList>();

            mSkinsHeroName = mRoot.FindChild("SkinsInterface/DefaultSkin/NamePlate/Name").GetComponent<UILabel>();
            mSkinsHeroIcon = mRoot.FindChild("SkinsInterface/DefaultSkin/Portrait").GetComponent<UISprite>();

            EventDelegate.Add(mRunePages.onChange, OnRunePageChange);
            #endregion


            #region 倒计时
            for (int i = 0; i < 2; i++)
            {
                mSpriteTens[i] = mRoot.FindChild("TopBar" + (i + 1).ToString()).FindChild("Minute1").GetComponent<UISprite>();
                mSpriteUnits[i] = mRoot.FindChild("TopBar" + (i + 1).ToString()).FindChild("Minute2").GetComponent<UISprite>();
            }

            SetTime(0, mFirstCountTime);
            SetTime(1, mSecondCountTime);

            #endregion


            mArrowUpDown[0] = mMidShow.transform.FindChild("Arrow/Left");
            mArrowUpDown[1] = mMidShow.transform.FindChild("Arrow/Right");
            mScrollView = mMidShow.transform.FindChild("HeroBox").GetComponent<UIScrollView>();

            //这里出发发送服务器记录的ui点击事件消息
            SendUIEventMsg();
        }

        //窗口控件释放
        protected override void RealseWidget()
        {

        }


        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroPreSelect, UpdateHeroListSelect);
            EventCenter.AddListener<int>(EGameEvent.eGameEvent_HeroRealSelect, HeroSelect);
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroAddSelect, HeroAddSelect);
            EventCenter.AddListener(EGameEvent.eGameEvent_HeroEnterRandom, RandomSelectHero);
            EventCenter.AddListener<int>(EGameEvent.eGameEvent_HeroReconnectPreSelect, UpdateReconnectPreSelect);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroPreSelect, UpdateHeroListSelect);
            EventCenter.RemoveListener<int>(EGameEvent.eGameEvent_HeroRealSelect, HeroSelect);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroAddSelect, HeroAddSelect);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_HeroEnterRandom, RandomSelectHero);
            EventCenter.RemoveListener<int>(EGameEvent.eGameEvent_HeroReconnectPreSelect, UpdateReconnectPreSelect);
        }

        //显示
        public override void OnEnable()
        {
            SetSelectHeroState(SelectHeroState.InitSelect);
            ShowPlayerNickName();
            mArrowUpDown[0].gameObject.SetActive(false);
            mArrowUpDown[1].gameObject.SetActive(true);
            mScrollView.onDragFinished += OnDragFinished;
            IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideRoomBeginBtnEnd);
        }

        //隐藏
        public override void OnDisable()
        {
            //UIGuide
            IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelectHeroHeadStart, StartIGuideTask);
            IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelectHeroCommitStart, StartIGuideTask);

            mScrollView.onDragFinished -= OnDragFinished;
            mFirstCountTime = 60;
            mSecondCountTime = 10;
        }


        public enum SelectHeroState
        {
            InitSelect,
            EnterSelect,
            InSelect,
            RandomSelect,
            OutSelect,
        }

        private SelectHeroState mSeletState = SelectHeroState.InitSelect;

        #region 可供选择
        private List<UIToggle> mHeroShowList = new List<UIToggle>(); //点击选择框
        private List<UISprite> mSpriteBlackList = new List<UISprite>();
        private List<Transform> mLockList = new List<Transform>();
        private int mSelectIndex = -1;
        #endregion

        #region 英雄头像
        Transform mTeamLeft;
        Transform mTeamRight;
        private UISprite[] mSpriteDic = new UISprite[mHeroCount]; //玩家英雄头像显示
        private UISprite[] mSpriteSelectDic = new UISprite[mHeroCount]; //玩家英雄头像显示
        private Transform[] mLabelDic = new Transform[mHeroCount];//玩家英雄头像显示
        private UILabel[] mLabelName = new UILabel[mHeroCount];

        UITweener mTween1;
        UITweener mTween2;
        UITweener mTweenAlpha;

        private UISprite mSkilltex;
        private UISprite mSkilltex1;
        private UISprite mSkilltex2;
        #endregion

        #region 描述
        private UISprite mSpriteDes;
        private UILabel mHeroName;
        private UILabel mSkillDes;
        private UILabel mSkillDes2;
        private UILabel mSkillDes3;
        #endregion

        #region 按钮
        private ButtonOnPress mBtnSure;
        private ButtonOnPress mBtnCloseBuy;
        private ButtonOnPress mBtnBuy;
        private GameObject mObjHightLight;
        private GameObject mObjCanPress;

        private UIToggle mSkinsToggle;
        private GameObject mSkinsDisable;
        private UIButton mSkinsSubmit;
        private UIPopupList mRunePages;

        private UILabel mSkinsHeroName;
        private UISprite mSkinsHeroIcon;

        #endregion

        #region 购买英雄界面
        UISprite mSpriteBuyDes;
        UILabel mLabelBuyName;
        UILabel mLabelBuySkill;
        UILabel mLabelBuyDes;
        Transform mBuyParent;
        #endregion

        #region Top
        UISprite[] mSpriteTens = new UISprite[2];
        UISprite[] mSpriteUnits = new UISprite[2];
        #endregion


        private Iplayer mLocalPlayer;
        private bool mStartCount = false;

        #region 倒计时的时间
        public static int mFirstCountTime = 60;
        public static int mSecondCountTime = 10;
        private Vector3 mTweenScaleVector = new Vector3(1.5f, 1.5f, 1f);//倒计时缩放
        private DateTime mStartTime;
        #endregion

        #region const
        private const int mTopBarTotalCount = 2;
        private const int mHeroCount = 6;
        private const int mHeroShowGroup = 2;
        private const float mMove = 50f;
        #endregion


        #region 中路顯示
        Transform mMidShow;
        Transform mShowAlpha;
        #endregion


        Transform[] mArrowUpDown = new Transform[2];
        UIScrollView mScrollView;
        void SetBtnSure(bool isVlb)
        {
            mBtnSure.gameObject.SetActive(isVlb);
            mBtnSure.enabled = isVlb;
        }
        #region 设置英雄选择界面状态
        public void SetSelectHeroState(SelectHeroState state)
        {
            mSeletState = state;
            switch (mSeletState)
            {
                case SelectHeroState.InitSelect:
                    for (int j = 0; j < mLabelDic.Length; j++)
                    {
                        mLabelDic[j].gameObject.SetActive(false);
                    }
                    EnabledSurePress(false);
                    for (int i = 0; i < mHeroShowList.Count; i++)
                    {
                        EventDelegate.Add(mHeroShowList[i].onChange, OnSelectHero);
                    }
                    mBtnSure.AddListener(OnCommitHero);
                    mBtnCloseBuy.AddListener(CloseBuy);
                    mBtnBuy.AddListener(BuyHero);
                    mLocalPlayer = PlayerManager.Instance.LocalAccount;
                    InitFlash();
                    InitRunePageList();
                    break;
                case SelectHeroState.EnterSelect:
                    EnterSelectHero();
                    break;
                case SelectHeroState.RandomSelect:
                    RandomSelectHero();
                    break;
                case SelectHeroState.OutSelect:
                    OutSelectHero();
                    break;
            }
        }
        #endregion

        #region 预选择英雄显示
        public void UpdateHeroListSelect()
        {//刷新选择列表

            foreach (var pic in mSpriteBlackList)
            {
                pic.gameObject.SetActive(false);
            }

            foreach (var item in HeroCtrl.Instance.heroSelectDic.Keys)
            {
                int heroId = 0;
                if (!HeroCtrl.Instance.heroSelectDic.TryGetValue(item, out heroId))
                    continue;
                if (item.GameUserSeat != mLocalPlayer.GameUserSeat && mLocalPlayer.IsSameCamp(item))
                {//找到队友所选择的英雄
                    int index = GetSelectIndexBuyHeroId(heroId);
                    mSpriteBlackList[index].gameObject.SetActive(true);
                }

                mSpriteDic[(int)item.GameUserSeat - 1].spriteName = ConfigReader.GetHeroSelectInfo(heroId).HeroSelectHead.ToString();
                if (item == PlayerManager.Instance.LocalAccount)
                {
                    mSpriteSelectDic[(int)item.GameUserSeat - 1].spriteName = "329";
                }
                if (!mSpriteSelectDic[(int)item.GameUserSeat - 1].gameObject.activeInHierarchy)
                {
                    mSpriteSelectDic[(int)item.GameUserSeat - 1].gameObject.SetActive(true);
                }
            }
        }

        void ShowPlayerNickName()
        {
            foreach (var item in PlayerManager.Instance.AccountDic.Values)
            {
                int index = (int)item.GameUserSeat - 1;
                if (index >= mLabelName.Length)
                    continue;
                mLabelName[index].text = item.GameUserNick;
            }
        }
        #endregion

        #region 更新左右两边队伍是否选择英雄的显示
        public void UpdateSureSelect()
        {//刷新左右两边队伍是否已经选择
            for (int i = 0; i < mHeroCount; i++)
            {
                mLabelDic[i].gameObject.SetActive(false);
            }
            foreach (var item in PlayerManager.Instance.AccountDic.Values)
            {
                if (HeroCtrl.Instance.heroSureSelectDic.ContainsKey(item))
                {
                    mLabelDic[(int)item.GameUserSeat - 1].gameObject.SetActive(true);
                    if (!mSpriteSelectDic[(int)item.GameUserSeat - 1].gameObject.activeInHierarchy)
                    {
                        mSpriteSelectDic[(int)item.GameUserSeat - 1].gameObject.SetActive(true);
                    }
                }
            }
            if (HeroCtrl.Instance.heroSureSelectDic.ContainsKey(PlayerManager.Instance.LocalAccount))
            {
                //更新英雄头像和名称，皮夫选择里面
                int id = HeroCtrl.Instance.heroSureSelectDic[PlayerManager.Instance.LocalAccount];

                HeroSelectConfigInfo config = new HeroSelectConfigInfo();
                if (ConfigReader.HeroSelectXmlInfoDict.TryGetValue(id, out config))
                {
                    mSkinsHeroName.text = config.HeroSelectNameCh;
                    mSkinsHeroIcon.spriteName = config.HeroSelectHead.ToString();
                }

                //按钮控制
                if (!mBtnSure.enabled)
                    return;
                EnabledSurePress(false);
            }           
        }
        #endregion

        #region 更新英雄选择栏的英雄锁定显示
        private void UpdateLock()
        {//显示锁定框
            for (int i = 0; i < HeroCtrl.Instance.heroInfoList.Count; i++)
            {
                if (!GameUserModel.Instance.CanChooseHeroList.Contains(HeroCtrl.Instance.heroInfoList[i].HeroSelectNum))
                {
                    mLockList[i].gameObject.SetActive(true);
                }
                else
                {
                    mLockList[i].gameObject.SetActive(false);
                }
            }
        }
        #endregion



        void OnDragFinished()
        {
            Vector3 constraint = mScrollView.panel.CalculateConstrainOffset(mScrollView.bounds.min, mScrollView.bounds.max);
            if (mScrollView.RestrictWithinBounds(false, false, true))
            {
                if (constraint.y > 0f)//上到头
                {
                    mArrowUpDown[0].gameObject.SetActive(false);
                    mArrowUpDown[1].gameObject.SetActive(true);
                }
                else//左到头
                {
                    mArrowUpDown[0].gameObject.SetActive(true);
                    mArrowUpDown[1].gameObject.SetActive(false);
                }
            }
            else
            {
                mArrowUpDown[0].gameObject.SetActive(true);
                mArrowUpDown[1].gameObject.SetActive(true);
            }
        }
        #region InitFlash
        void InitFlash()
        {
            UpdateLock();
            Vector3 orginalLeft = mTeamLeft.localPosition;
            Vector3 orginalRight = mTeamRight.localPosition;
            mTeamLeft.localPosition = new Vector3(orginalLeft.x - mMove, orginalLeft.y, orginalLeft.z);
            mTeamRight.localPosition = new Vector3(orginalRight.x + mMove, orginalRight.y, orginalRight.z);

            mTween1 = TweenPosition.Begin(mTeamLeft.gameObject, 0.5f, orginalLeft);
            mTween2 = TweenPosition.Begin(mTeamRight.gameObject, 0.5f, orginalRight);
            UICommonMethod.TweenAlphaBegin(mShowAlpha.gameObject, 0f, 0f);
            mTweenAlpha = UICommonMethod.TweenAlphaBegin(mShowAlpha.gameObject, 1.7f, 1f);
            mTween1.method = UITweener.Method.EaseIn;
            mTween2.method = UITweener.Method.EaseIn;
            mSelectIndex = -1;
            //  EventDelegate.Add(tweenAlpha.onFinished, FinishTween);
            SetSelectHeroState(SelectHeroState.EnterSelect);
            //  EnableToggle(false);
        }
        #endregion


        #region
        void EnableToggle(bool enable)
        {
            foreach (var item in mHeroShowList)
            {
                item.enabled = enable;
            }
        }
        #endregion



        #region 进入选择英雄阶段
        private void EnterSelectHero()
        {
            MsgInfoManager.Instance.ShowMsg(10);
            SetBtnSure(true);
            EnableToggle(true);
            FirstCountDown();
            SetSelectHeroState(SelectHeroState.InSelect);

            //UIGuide
            IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelectHeroHeadStart, StartIGuideTask);
            IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelectHeroCommitStart, StartIGuideTask);
            IGuideTaskManager.Instance().SendTaskTrigger(EGameEvent.eGameEvent_UIGuideTriggerSelectHero);
        }
        #endregion

        void StartIGuideTask(EGameEvent taskId)
        {
            IGuideTaskManager.Instance().SendTaskEffectShow(taskId);
        }



        #region 进入最后随机阶段
        private void RandomSelectHero()
        {
            IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideSelectHeroHeadEnd);
            IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideSelectHeroCommitEnd);
            EnabledSurePress(false);
            TweenRotation obj = mSpriteUnits[0].transform.parent.GetComponent<TweenRotation>();
            obj.enabled = true;
            //UITweener tween = TweenRotation.Begin(obj, 1f, Quaternion.EulerRotation(180f, 0f, 0f));
            obj.method = UITweener.Method.EaseOut;
            EventDelegate.Add(obj.onFinished, LastCountDown);

            mSkinsDisable.gameObject.SetActive(false);
            mSkinsToggle.value = true;
        }
        #endregion

        #region 倒计时
        /*private void ShowCount(UILabel label ,int time)
	{
		string timeString = "";
		if(time < 10)
			timeString = "0"+time.ToString();
		else if(time < 60)
			timeString = ""+time.ToString();
		else if((time / 60) >= 10  )
			timeString = (time % 60).ToString()+":"+ (time / 60).ToString();
		else if((time / 60) < 10)
			timeString = (time / 60).ToString()+":"+ (time %60).ToString();
		label.text = timeString;
	}*/


        private void SetTime(int step, int time)
        {
            int ten = time / 10;
            int unit = time % 10;
            if (step == 0)
            {
                unit = unit + 10;
            }
            if (ten == 0)
            {
                mSpriteTens[step].gameObject.SetActive(false);
            }
            else
            {
                mSpriteTens[step].gameObject.SetActive(true);
                if (step == 0)
                {
                    ten = ten + 10;
                }
                mSpriteTens[step].spriteName = ten.ToString();
            }

            mSpriteUnits[step].spriteName = unit.ToString();

        }

        private void FirstCountDown()
        {
            if (mFirstCountTime == 0)
            {
                return;
            }
            SetTime(0, mFirstCountTime);
            mStartTime = DateTime.Now;
            mStartCount = true;
            mSpriteTens[1].transform.parent.gameObject.SetActive(false);
        }

        private void LastCountDown()
        {
            mStartTime = DateTime.Now;
            mStartCount = true;
            mSpriteTens[0].transform.parent.gameObject.SetActive(false);
            mSpriteTens[1].transform.parent.gameObject.SetActive(true);

            SetSelectHeroState(SelectHeroState.RandomSelect);
        }

        UITweener scaleTween;
        private void TweenCountDownLabelScale(GameObject obj)
        {
            TweenAlpha.Begin(obj.gameObject, 0f, 1f);
            TweenScale.Begin(obj, 0f, mTweenScaleVector);
            scaleTween = TweenScale.Begin(obj, 0.5f, new Vector3(1f, 1f, 1f));
            scaleTween.method = UITweener.Method.EaseInOut;
            EventDelegate.Add(scaleTween.onFinished, FinshTweenScale, true);
        }

        void FinshTweenScale()
        {
            EventDelegate.Remove(scaleTween.onFinished, FinshTweenScale);
            //   TweenAlpha.Begin(spriteUnits[1].gameObject, 0.1f, 0f).delay = 0.4f; 
        }
        void SetSecondTimeDown()
        {
            SetTime(1, mSecondCountTime);
            TweenCountDownLabelScale(mSpriteUnits[1].gameObject);
        }
        public override void Update(float deltaTime)
        {
            if (mStartCount)
            {
                TimeSpan span = DateTime.Now - mStartTime;
                if (span.TotalSeconds <= 1f)
                    return;
                mStartTime = DateTime.Now;
                if (mSeletState == SelectHeroState.RandomSelect)
                {
                    mSecondCountTime = mSecondCountTime - (int)span.TotalSeconds;
                    if (mSecondCountTime < 0)
                    {
                        mStartCount = false;
                        return;
                    }
                    SetSecondTimeDown();
                }
                else
                {
                    mFirstCountTime = mFirstCountTime - (int)span.TotalSeconds;
                    if (mFirstCountTime < 0)
                    {
                        mStartCount = false;
                        return;
                    }
                    SetTime(0, mFirstCountTime);
                }
            }
        }
        #endregion

        #region 结束选择英雄
        private void OutSelectHero()
        {
            for (int i = 0; i < mHeroShowList.Count; i++)
            {
                EventDelegate.Add(mHeroShowList[i].onChange, OnSelectHero);
            }
            mBtnSure.RemoveListener(OnCommitHero);
            mBtnCloseBuy.RemoveListener(CloseBuy);
            mBtnBuy.RemoveListener(BuyHero);
            mStartCount = false;
        }
        #endregion

        #region 结束入场动画
        private void FinishTween()
        {
            //  SetSelectHeroState(SelectHeroState.EnterSelect);		
        }
        #endregion


        #region 点击购买英雄
        private void BuyHero(int ie, bool press)
        {
            if (press || mSeletState == SelectHeroState.InitSelect)
            {
                return;
            }
        }
        #endregion

        #region 关闭购买英雄
        private void CloseBuy(int ie, bool press)
        {
            if (press)
            {
                return;
            }
            //buyParent.gameObject.SetActive(false);
        }
        #endregion

        #region 点击确定选择英雄按钮事件
        private void OnCommitHero(int ie, bool press)
        {

            //if (press)
            //{
            //    objHightLight.gameObject.SetActive(true);
            //}
            //else
            //{
            EnabledSurePress(false);
            // }
            if (press || mSeletState != SelectHeroState.InSelect)
            {
                return;
            }
            if (mSelectIndex != -1)
            {
                IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideSelectHeroCommitEnd);
                CGLCtrl_GameLogic.Instance.EmsgToss_AskSelectHero((UInt32)HeroCtrl.Instance.heroInfoList.ElementAt(mSelectIndex).HeroSelectNum);

                mSkinsDisable.gameObject.SetActive(false);
                mSkinsToggle.value = true;
            }
        }
        #endregion

        #region 点击英雄栏的选择事件
        void OnSelectHero()
        {
            //if (seletState == SelectHeroState.InitSelect)
            //    return;
            UIToggle toggle = UIToggle.current;
            if (toggle != null && toggle.group == mHeroShowGroup && toggle.value)
            {
                for (int i = 0; i < mHeroShowList.Count; i++)
                {
                    if (mHeroShowList[i] != toggle)
                        continue;
                    mSelectIndex = i;
                    ShowDestribe(i);//描述 
                    if (UpdateSureBtn(i))
                    {//是否可点击
                        IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideSelectHeroHeadEnd);
                        NotifyServerMyPreSelect(i);
                    }

                    break;
                }
            }
        }

        public void UpdateReconnectPreSelect(int heroId)
        {
            int index = GetSelectIndexBuyHeroId(heroId);
            mSelectIndex = index;
            ShowDestribe(index);//描述 
            SetSelectShow(index);
        }


        #endregion

        #region 更新按钮状态并判断是否可点击选择按钮
        bool UpdateSureBtn(int index)
        {
            bool canPress = true;
            int id = HeroCtrl.Instance.heroInfoList[index].HeroSelectNum;
            if (!TeamMateHasSelectHero(id) && GameUserModel.Instance.CanChooseHeroList.Contains(id)
               && mSeletState == SelectHeroState.InSelect
               && !HeroCtrl.Instance.heroSureSelectDic.ContainsKey(PlayerManager.Instance.LocalAccount))
            {
                canPress = true;
            }
            else
            {
                canPress = false;
            }
            EnabledSurePress(canPress);
            return canPress;
        }
        #endregion

        #region 设置按钮可不可选择显示
        void EnabledSurePress(bool enable)
        {
            mBtnSure.GetComponent<BoxCollider>().enabled = enable;
            mObjHightLight.gameObject.SetActive(!enable);
            mObjCanPress.gameObject.SetActive(enable);
        }
        #endregion

        #region 通知客户端点击的英雄
        void NotifyServerMyPreSelect(int index)
        {
            int id = HeroCtrl.Instance.heroInfoList[index].HeroSelectNum;
            CGLCtrl_GameLogic.Instance.EmsgTocs_TryToSelectHero((uint)id);
        }
        #endregion

        #region 通过点击的是第几个英雄显示英雄信息
        void ShowDestribe(int index)
        {
            ShowHasDes(index);
            if (!GameUserModel.Instance.CanChooseHeroList.Contains(HeroCtrl.Instance.heroInfoList[index].HeroSelectNum))
            {
                ShowBuyeDes(index);
            }
        }
        #endregion

        #region 通过点击的是第几个英雄显示已购买英雄信息
        public void ShowHasDes(int index)
        {
            mHeroName.text = HeroCtrl.Instance.heroInfoList[index].HeroSelectNameCh;
            mSpriteDes.spriteName = HeroCtrl.Instance.heroInfoList[index].HeroSelectThumb;
            mSkillDes.text = HeroCtrl.Instance.heroInfoList[index].HeroSelectDes;
            mSkillDes2.text = HeroCtrl.Instance.heroInfoList[index].HeroSelectDes2;
            mSkillDes3.text = HeroCtrl.Instance.heroInfoList[index].HeroPassDes;
            mSkilltex.spriteName = HeroCtrl.Instance.heroInfoList[index].HeroSelectSkill1;
            mSkilltex1.spriteName = HeroCtrl.Instance.heroInfoList[index].HeroSelectSkill2;
            mSkilltex2.spriteName = HeroCtrl.Instance.heroInfoList[index].HeroSkillPass;
        }
        #endregion

        #region 通过点击的是第几个英雄显示购买描述
        void ShowBuyeDes(int index)
        {
            return;
            mLabelBuyName.text = HeroCtrl.Instance.heroInfoList[index].HeroSelectNameCh;
            mSpriteBuyDes.spriteName = HeroCtrl.Instance.heroInfoList[index].HeroSelectThumb;
            mLabelBuySkill.text = HeroCtrl.Instance.heroInfoList[index].HeroSelectDes;
            mLabelBuyDes.text = HeroCtrl.Instance.heroInfoList[index].HeroSelectBuyDes;
            mBuyParent.gameObject.SetActive(true);
        }
        #endregion

        #region 通过英雄ID号获得选择的是第几个英雄
        public int GetSelectIndexBuyHeroId(int heroId)
        {
            for (int i = 0; i < HeroCtrl.Instance.heroInfoList.Count; i++)
            {
                if (HeroCtrl.Instance.heroInfoList[i].HeroSelectNum != heroId)
                    continue;
                return i;
            }
            return -1;
        }
        #endregion


        public void SetSelectShow(int index)
        {
            mHeroShowList[index].value = true;
        }

        #region 通过英雄ID判断队友是否有选择此英雄
        private bool TeamMateHasSelectHero(int heroId)
        {
            foreach (var item in HeroCtrl.Instance.heroSelectDic.Keys)
            {
                if (item.GameUserSeat == mLocalPlayer.GameUserSeat || !mLocalPlayer.IsSameCamp(item))//找到队友所选择的英雄
                    continue;
                int heroSelect = 0;
                if (!HeroCtrl.Instance.heroSelectDic.TryGetValue(item, out heroSelect))
                    continue;
                if (heroSelect == heroId)
                    return true;
            }
            return false;
        }
        #endregion

        //英雄选择
        public void HeroSelect(int heroid)
        {
            int itemPos = GetSelectIndexBuyHeroId(heroid);
            ShowHasDes(itemPos);
            SetSelectShow(itemPos);
            MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_ObtainRandomHero);
        }

        public void HeroAddSelect()
        {
            UpdateHeroListSelect();
            UpdateSureSelect();
        }

        public void SetFirstTime(int time)
        {
            mFirstCountTime = time;
        }

        public void SetSecondTime(int time)
        {
            mSecondCountTime = time;
        }

        public void OnRunePageChange()
        {
            int index = mRunePages.data == null ? 0 : (int)mRunePages.data;

            CGLCtrl_GameLogic.Instance.EmsgToss_AskChangeRunePage(index);
        }

        public void InitRunePageList()
        {
            mRunePages.items.Clear();
            mRunePages.itemData.Clear();
            int page = RuneEquipModel.Instance.pagenum + 1;

            for (int i = 0; i < page; i++)
            {
                mRunePages.items.Add(ConfigReader.GetMsgInfo(40050).content + (i + 1));
                mRunePages.itemData.Add(i);
            }
        }
        //这里出发发送服务器记录的ui点击事件消息
        public void SendUIEventMsg()
        {
            CGLCtrl_GameLogic.Instance.EmsgTocs_ReportUIEvent();
        }
    } 
}