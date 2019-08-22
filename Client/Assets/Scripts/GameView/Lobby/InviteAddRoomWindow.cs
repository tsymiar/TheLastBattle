using UnityEngine;
using System.Collections;
using GameDefine;
using System;
using BlGame.GameEntity;
using BlGame.Ctrl;

namespace BlGame.View
{
    public class InviteAddRoomWindow : BaseWindow
    {
        public InviteAddRoomWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadInvitationTips;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_InviteRoomEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_InviteRoomExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_InviteRoomEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_InviteRoomExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }


        //窗口控件初始化
        protected override void InitWidget()
        {
            StratTime = timeRelieve = DateTime.Now;
            isTimeStart = true;
            BtnAccept = mRoot.FindChild("Accept").gameObject;
            BtnCanel = mRoot.FindChild("Cancel").gameObject;
            nickName = mRoot.FindChild("Tips").GetComponent<UILabel>();
            UIEventListener.Get(BtnAccept).onClick += AcceptInvite;
            UIEventListener.Get(BtnCanel).onClick += CanelInvite;
        }

        //窗口控件释放
        protected override void RealseWidget()
        {

        }

        private void CanelInvite(GameObject go)
        {
            InviteRoomCtrl.Instance.Exit();
        }

        private void AcceptInvite(GameObject go)
        {
            InviteRoomCtrl.Instance.AcceptAddFriend(InviteOtherPlayer.Instance.RoomID.ToString(),
                InviteOtherPlayer.Instance.PassWord);
            InviteRoomCtrl.Instance.Exit();
        }


        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_NewInviteRoom, ChangeInvite);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_NewInviteRoom, ChangeInvite);
        }

        void ChangeInvite()
        {
            tempName = InviteOtherPlayer.Instance.NiceName;
            if (tempName == null && string.IsNullOrEmpty(tempName))
                Debug.LogError("名字为空");
            nickName.text = "玩家 " + tempName + "  邀请你加入到房间" + InviteOtherPlayer.Instance.RoomID;
        }

        //显示
        public override void OnEnable()
        {
            ChangeInvite();
        }

        //隐藏
        public override void OnDisable()
        {

        }

        public override void Update(float deltaTime)
        {
            if (isTimeStart)
            {
                if (DateTime.Now != timeRelieve)
                {
                    timeRelieve = DateTime.Now;
                    TimeSpan currtime = timeRelieve - StratTime;
                    if (currtime.Seconds >= LastTime)
                    {
                        isTimeStart = false;
                        InviteRoomCtrl.Instance.Exit();
                    }
                }
            }
        }

        DateTime StratTime;
        private float LastTime = 10;
        bool isTimeStart = false;
        string tempName;
        private GameObject BtnAccept;
        private GameObject BtnCanel;
        private UILabel nickName;
        private DateTime timeRelieve
        {
            set;
            get;
        }
        
    }

}