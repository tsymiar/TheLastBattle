using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using BlGame.Ctrl;
using BlGame.GameEntity;

namespace BlGame.View
{
    public class TeamMatchInvitationWindow : BaseWindow
    {
        public TeamMatchInvitationWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadGameMatchInvitationUI;
            mResident = false;
            mTime = 0f;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener<string>(EGameEvent.eGameEvent_TeamMatchInvitationEnter, ShowUI);
            EventCenter.AddListener(EGameEvent.eGameEvent_TeamMatchInvitationExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_TeamMatchSearchinEnter, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener<string>(EGameEvent.eGameEvent_TeamMatchInvitationEnter, ShowUI);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_TeamMatchInvitationExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_TeamMatchSearchinEnter, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            //地图
            mTipLabel = mRoot.FindChild("Tips").GetComponent<UILabel>();
            mAccept = mRoot.FindChild("Accept").GetComponent<UIButton>();
            mCancel = mRoot.FindChild("Cancel").GetComponent<UIButton>();

            EventDelegate.Add(mAccept.onClick, OnAccept);
            EventDelegate.Add(mCancel.onClick, OnmCancel);
        }

        //窗口控件释放
        protected override void RealseWidget()
        {

        }

     
        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_TeamMatchEnter, Hide);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_TeamMatchEnter, Hide);
        }

        //显示
        public override void OnEnable()
        {
           

        }

        //隐藏
        public override void OnDisable()
        {

        }

        public override void Update(float deltaTime) 
        {
            mTime += deltaTime;
            if (mTime > 10)
            {
                Hide();
            }
        }

        public void ShowUI(string name)
        {
            Show();
            mTipLabel.text = name + "\n" + ConfigReader.GetMsgInfo(40036).content;
            mName = name;
            mTime = 0f;
        }

        public void OnAccept()
        {
            TeamMatchCtrl.Instance.AccpetInvitation(mName);
            Hide();
        }

        public void OnmCancel()
        {
            Hide();
        }

        UILabel mTipLabel;
        UIButton mAccept;
        UIButton mCancel;

        string mName;
        float mTime;

    }
}

