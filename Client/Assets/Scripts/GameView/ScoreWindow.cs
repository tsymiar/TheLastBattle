using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using System.Collections.Generic;
using JT.FWW.Tools;
using JT.FWW.GameData;
using System.Linq;
using GameDefine;
using BlGame.Resource;
using BlGame.Ctrl;
using System;
using BlGame.Model;

namespace BlGame.View
{
    public class ScoreWindow : BaseWindow
    {
        public ScoreWindow() 
        {
            mScenesType = EScenesType.EST_Play;
            mResName = GameConstDefine.ScoreUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_ScoreEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_ScoreExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ScoreEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ScoreExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mLossTip = mRoot.Find("results/Lose").GetComponent<UISprite>();
            mWinTip = mRoot.Find("results/Win").GetComponent<UISprite>();
            mLossTip.gameObject.SetActive(false);
            mWinTip.gameObject.SetActive(false);
            Transform tran = mRoot.FindChild("Player");
            int num = tran.childCount;
            mListPlayer.Clear();
            for (int i = 0; i < num; i++)
            {
                Transform temp = tran.FindChild("Player" + (i + 1));
                mListPlayer.Add(new UIShowScoreInfo(temp));
            }
            mReTurnBtn = mRoot.FindChild("BackButton").gameObject;
            mHeadIcon = mRoot.FindChild("PlayerGet/Head").GetComponent<UISprite>();
            mLevel = mRoot.FindChild("PlayerGet/EXP/Level").GetComponent<UILabel>();
            mExpBar = mRoot.FindChild("PlayerGet/EXP/Bar").GetComponent<UISprite>();
            mExpNum = mRoot.FindChild("PlayerGet/EXP/EXPNum").GetComponent<UILabel>();

            mGold = mRoot.FindChild("PlayerGet/Gold/Num").GetComponent<UILabel>();
            mNickName = mRoot.FindChild("PlayerGet/Name").GetComponent<UILabel>();
            UIEventListener.Get(mReTurnBtn).onClick += BackRoom;
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
        }

        void SetShowInfo()
        {
            Iplayer player = PlayerManager.Instance.LocalPlayer;
            if(player == null)
                return;
            foreach(var item in ScoreCtrl.Instance.GetSettlementList())
            {
                if(item.mObjGUID == player.GameObjGUID)
                {
                    int id = (int)player.ObjTypeID;
                    HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo(id);
                    mHeadIcon.spriteName = info.HeroSelectHead.ToString();
                    mGold.text = ScoreCtrl.Instance.mGotGole.ToString();
                    mNickName.text = GameUserModel.Instance.GameUserNick;
                    SetLv();
                }
            }
        }
        void SetLv ()
        {
            int curLv = ScoreCtrl.Instance.mLevel;
            int curExp = ScoreCtrl.Instance.mExp; 
            int gotExp = ScoreCtrl.Instance.mGotExp;
            
            LevelConfigInfo leveinfo = ConfigReader.GetLevelInfo(curLv);
            if (leveinfo == null)
            {
                mLevel.text = curLv.ToString();
                mExpNum.transform.gameObject.SetActive(false);
                Debug.LogError(" LevelConfigInfo = null" + curLv);
                return;
            }
            if (curLv <= 0 || gotExp <= 0)
            {
                SetExpLabel(curExp, leveinfo.LevelUpExp);
                mLevel.text = curLv.ToString();
                mExpBar.fillAmount = (float) curExp / leveinfo.LevelUpExp;
                return;
            }
            if (curLv >= mMaxLv && curExp >= leveinfo.LevelUpExp)
            {
                mLevel.text = (mMaxLv + 1).ToString();
                mExpNum.transform.gameObject.SetActive(false);
                curExp = (int)leveinfo.LevelUpExp;
                SetExpLabel(curExp, leveinfo.LevelUpExp);
                mExpBar.fillAmount = 1f;
                return;
            }
            mStartExpFlash = true;
            mGoalLv = curLv;
            mExpBar.fillAmount = (float)curExp / (float)leveinfo.LevelUpExp;
            mExpValue = curExp + gotExp;
            GetUpExp(ref mGoalLv, ref mExpValue);
            leveinfo = ConfigReader.GetLevelInfo(mGoalLv);
            mExpFlashRunTimes = mGoalLv - curLv;//升几级
            if (mGoalLv >= mMaxLv && mExpValue > leveinfo.LevelUpExp)
            {
                mExpValue = (int)leveinfo.LevelUpExp;
            }
            mExpLast = (float)mExpValue / (float)leveinfo.LevelUpExp; //升级后slider的value    
            mLevel.text = curLv.ToString();
        }

        void GetUpExp(ref int lv, ref int exp)
        {
            LevelConfigInfo leveinfo = ConfigReader.GetLevelInfo(lv);
            if (exp >= (int)leveinfo.LevelUpExp)
            {
                exp -= (int)leveinfo.LevelUpExp;
                lv += 1;
                if (lv >= mMaxLv)
                {
                    lv = mMaxLv;
                    return;
                }
                GetUpExp(ref lv, ref exp);
            }
        }

        public override void Update(float deltaTime)
        {
            if (mStartExpFlash)
            {
                mExpBar.fillAmount += 0.02f;
                int lv = mGoalLv - mExpFlashRunTimes;
                LevelConfigInfo leveinfo = ConfigReader.GetLevelInfo(lv);
                if (leveinfo == null)
                {
                    Debug.LogError("levelinfo is null" + lv);
                    return;
                }
                int exp = (int)(leveinfo.LevelUpExp * mExpBar.fillAmount);
                SetExpLabel(exp, ConfigReader.GetLevelInfo(lv).LevelUpExp);
                if (mExpBar.fillAmount >= 1f)//升一级
                {
                    SetExpLabel(ConfigReader.GetLevelInfo(lv).LevelUpExp, ConfigReader.GetLevelInfo(lv).LevelUpExp);
                    mExpFlashRunTimes -= 1;
                    mExpBar.fillAmount = 0f;
                    mLevel.text = (lv + 1).ToString();
                    if (mGoalLv == mMaxLv)
                    {
                        mLevel.text = (mMaxLv + 1).ToString();
                    }
                }
                if (mExpFlashRunTimes < 0 || (mExpFlashRunTimes == 0 && mExpLast <= mExpBar.fillAmount))
                {
                    mExpBar.fillAmount = mExpLast;
                    SetExpLabel(mExpValue, ConfigReader.GetLevelInfo(mGoalLv).LevelUpExp);
                    mStartExpFlash = false;
                }
            }
        }
        void SetExpLabel(float expIndex, float ExpMax)
        {
            mExpNum.text = expIndex + " / " + ExpMax;
        }

        private void BackRoom(GameObject go)
        {
            ScoreCtrl.Instance.AskBackRoom();
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
          
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
          
        }

        //显示
        public override void OnEnable()
        {
            RefreshData();
        }

        //隐藏
        public override void OnDisable()
        {
            
        }



        UISprite mWinTip;
        UISprite mLossTip;
        GameObject mReTurnBtn = null;
        UISprite mHeadIcon = null;
        UILabel mLevel = null;
        UILabel mMoney = null;
        UISprite mExpBar = null;
        UILabel mExpNum = null;
        UILabel mGold = null;
        UILabel mNickName = null;
        const int mMaxLv = 29;
        private int mGoalLv = 0;
        private int mExpValue = 0;//剩余经验
        private int mExpFlashRunTimes = 0;
        private float mExpLast = 0;//slider value
        bool mStartExpFlash = false;
        private List<UIShowScoreInfo> mListPlayer = new List<UIShowScoreInfo>();

        int i = 0;
        int num = 3;
     
        public void  RefreshData()
        {
            i = 0;
            num = 3;
            foreach(var item in ScoreCtrl.Instance.GetSettlementList())
            {
                if (item.mCamp == EntityCampType.CampTypeA)
                {
                    mListPlayer[i++].SetShowLabel((UInt64)item.mHeadID, item.mNickName, item.mLevel, item.mKills, item.mDeath,item.mAsstimes,item.mTotalcp,item.mLasthit);
                     ShowBattleResult();
                }
                else if (item.mCamp == EntityCampType.CampTypeB)
                {
                    if (num <= 5)
                        mListPlayer[num++].SetShowLabel((UInt64)item.mHeadID, item.mNickName, item.mLevel, item.mKills, item.mDeath, item.mAsstimes, item.mTotalcp, item.mLasthit);
                    ShowBattleResult();
                }
            }
            SetShowInfo();
        }

        private void ShowBattleResult()
        {
            //BlGame.AudioManager.Instance.StopBgAudio();
            Iplayer player = PlayerManager.Instance.LocalPlayer;
            if (player == null)
                player = PlayerManager.Instance.LocalAccount;
            if (player.StateSituation == SITUATION.WIN)
            {
                //BlGame.AudioManager.Instance.PlayLongVoiceAudio(Resources.Load(AudioDefine.PATH_HERO_KILLSOUND + "Victory") as AudioClip);

                ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_HERO_KILLSOUND + "Victory", ResourceType.ASSET);
                AudioClip clip = clipUnit.Asset as AudioClip;

                BlGame.AudioManager.Instance.PlayLongVoiceAudio(clip);

                mWinTip.gameObject.SetActive(true);
            }
            else if (player.StateSituation == SITUATION.LOSE)
            {
                mLossTip.gameObject.SetActive(true);
                //BlGame.AudioManager.Instance.PlayLongVoiceAudio(Resources.Load(AudioDefine.PATH_HERO_KILLSOUND + "Failed") as AudioClip);

                ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_HERO_KILLSOUND + "Failed", ResourceType.ASSET);
                AudioClip clip = clipUnit.Asset as AudioClip;

                BlGame.AudioManager.Instance.PlayLongVoiceAudio(clip);
            }
            else
            {
                //WinLabel.text = "平局";
                Debug.LogError("平局");
            }
        }
 


        
        public class UIShowScoreInfo
        {
            public UISprite HeadIcon;
            public UILabel HeadNameLabel;
            public UILabel HeroLevelLabel;
            public UILabel HeroKillsLabel;
            public UILabel HeroDeadTimesLabel;
            public UILabel HeroAssLabel;
            public UILabel HeroTotalLabel;
            public UILabel HeroLastHitLabel;
            public UILabel HeroCampIDLabel;
            Transform tran;
            public UIShowScoreInfo(Transform temp)
            {
                tran = temp;
                HeadNameLabel = temp.FindChild("Name").GetComponent<UILabel>();
                HeroLevelLabel = temp.FindChild("Level").GetComponent<UILabel>();
                HeroKillsLabel = temp.FindChild("Kills").GetComponent<UILabel>();
                HeroDeadTimesLabel = temp.FindChild("Death").GetComponent<UILabel>();
                HeadIcon = temp.FindChild("Head").GetComponent<UISprite>();
                HeroAssLabel = temp.FindChild("Assists").GetComponent<UILabel>();
                HeroTotalLabel = temp.FindChild("CP").GetComponent<UILabel>();
                HeroLastHitLabel = temp.FindChild("LastHits").GetComponent<UILabel>();

                HeadNameLabel.text = "";
                HeroLevelLabel.text = "";
                HeroKillsLabel.text = "";
                HeroDeadTimesLabel.text = "";
                HeroAssLabel.text = "";
                HeroTotalLabel.text = "";
                HeroLastHitLabel.text = "";
            }
            public void SetShowLabel(UInt64 headIcon, string name, int level, int kill, int deathtimes,int asstime,int totalcp,int lasthit)
            {
                HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo((Int32)headIcon);
                HeadIcon.spriteName = info.HeroSelectHead.ToString();
                HeadNameLabel.text = name;
                HeroLevelLabel.text = level.ToString();
                HeroKillsLabel.text = kill.ToString();
                HeroDeadTimesLabel.text = deathtimes.ToString();
                HeroAssLabel.text = asstime.ToString();
                HeroTotalLabel.text = totalcp.ToString();
                HeroLastHitLabel.text = lasthit.ToString();
            }
        }
    }

}