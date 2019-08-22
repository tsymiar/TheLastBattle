using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using BlGame.Ctrl;
using BlGame.GameEntity;

namespace BlGame.View
{
    public class TeamMatchSearchingWindow : BaseWindow
    {
        public TeamMatchSearchingWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadGameMatchSearchingUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_TeamMatchSearchinEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_TeamMatchSearchinExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_TeamMatchSearchinEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_TeamMatchSearchinExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mTimeLabel = mRoot.FindChild("Time").GetComponent<UILabel>();
            mNumberLabel = mRoot.FindChild("MatchedNum").GetComponent<UILabel>();
            mCancel = mRoot.FindChild("CancelBtn").GetComponent<UIButton>();

            EventDelegate.Add(mCancel.onClick, OnCancel);
        }

        //窗口控件释放
        protected override void RealseWidget()
        {

        }

     
        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<int,int>(EGameEvent.eGameEvent_MatchNumber, ChangeMatchNumber);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<int,int>(EGameEvent.eGameEvent_MatchNumber, ChangeMatchNumber);
        }

        //显示
        public override void OnEnable()
        {
            mIsDownTime = true;
        }

        //隐藏
        public override void OnDisable()
        {
            mIsDownTime = false;
            mStartTime = 0;
        }

        public override void Update(float deltaTime)
        {
            if (mIsDownTime)
            {
                mStartTime += Time.deltaTime;
                ShowCount(mTimeLabel, (int)mStartTime);
            }
        }

        private void ShowCount(UILabel label, int time)
        {
            string timeString = "";
            if (time < 60)
                timeString = "00:" + SecondToStr(time);
            else
                timeString = SecondToStr(time / 60) + ":" + SecondToStr(time % 60);

            label.text = timeString;
        }

        private string SecondToStr(int time)
        {
            string timeString = "";
            if (time < 10)
                timeString = "0" + time.ToString();
            else
                timeString = "" + time.ToString();

            return timeString;
        }

        public void OnCancel()
        {
            TeamMatchCtrl.Instance.AskStopMatch();

            Hide();
        }

        public void ChangeMatchNumber(int cur,int max)
        {
            mNumberLabel.text = cur.ToString() + "/" + max.ToString();
        }

        UILabel mTimeLabel;
        UILabel mNumberLabel;
        UIButton mCancel;

        private float mStartTime = 0;
        private bool mIsDownTime = false;
    }
}

