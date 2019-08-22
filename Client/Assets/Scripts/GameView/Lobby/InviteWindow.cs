using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.GameEntity;
using BlGame.Ctrl;
using System;

namespace BlGame.View
{
    public class InviteWindow : BaseWindow
    {
        public InviteWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadInvitationTips;
            mResident = false;
        }

                ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener<UInt64,string>(EGameEvent.eGameEvent_InviteEnter, InvateEnter);
            EventCenter.AddListener(EGameEvent.eGameEvent_InviteExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        private void InvateEnter(ulong sguid, string temp)
        {
            sGUID = sguid;
            tempName = temp;
            Show();
        }


        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener<UInt64, string>(EGameEvent.eGameEvent_InviteEnter, InvateEnter);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_InviteExit, Hide);
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

        void UpdataNew(ulong sguid, string temp)
        {
            sGUID = sguid;
            tempName = temp;
            isTimeStart = true;
            timeRelieve = DateTime.Now;
            if (tempName == null && string.IsNullOrEmpty(tempName))
                Debug.LogError("名字为空");
            nickName.text = tempName + "  申请添加你为好友"; 
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
        }

        private void CanelInvite(GameObject go)
        {
            InviteCtrl.Instance.CanelAddFriend(sGUID);
            InviteCtrl.Instance.RemoveDic(sGUID);
        }

        private void AcceptInvite(GameObject go)
        {
            InviteCtrl.Instance.AcceptAddFriend(sGUID);
            InviteCtrl.Instance.RemoveDic(sGUID);
        }
        
        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<UInt64, string>(EGameEvent.eGameEvent_InviteChange, UpdataNew);
        }

         //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<UInt64, string>(EGameEvent.eGameEvent_InviteChange, UpdataNew);
        }


        //显示
        public override void OnEnable()
        {
            UpdataNew(sGUID, tempName);
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
                    TimeSpan currtime = DateTime.Now - timeRelieve;
                    if (currtime.Seconds >= LastTime)
                    {
                        isTimeStart = false;
                        InviteCtrl.Instance.RemoveDic(sGUID);
                    }
                }
            }
        }

        DateTime StratTime;
        private float LastTime = 10;
        bool isTimeStart = false;
        UInt64 sGUID;
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