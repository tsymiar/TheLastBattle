using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Ctrl;
using System.Collections.Generic;
using BlGame.GameData;
using BlGame.Resource;
using BlGame.GameEntity;
using BlGame.Model;
using System;

namespace BlGame.View
{
    public class RoomInviteWindow : BaseWindow
    {
        public RoomInviteWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadInvitationList;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_RoomInviteEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_RoomInviteExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_RoomExit, Hide);
        }


        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_RoomInviteEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_RoomInviteExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_RoomExit, Hide);
        }

         //窗口控件初始化
        protected override void InitWidget()
        {
            ToggleFriend = mRoot.FindChild("FriendListBtn").GetComponent<UIToggle>();
            ToggleLobby = mRoot.FindChild("LobbyListBtn").GetComponent<UIToggle>();
            mOnLineGrid = mRoot.FindChild("ShowList/Panel/Grid").GetComponent<UIGrid>();
            InviteBtn = mRoot.FindChild("InviteBtn").GetComponent<UIButton>();
            CloseBtn = mRoot.FindChild("CloseBtn").GetComponent<UIButton>();
            nickName.Clear();
            OnLineNickName.Clear();
            InviteBtn.isEnabled = (bool)(nickName.Count != 0);
            UIEventListener.Get(InviteBtn.gameObject).onClick += InviteList;
            UIEventListener.Get(CloseBtn.gameObject).onClick += CloseButton;
        }

        private void CloseButton(GameObject go)
        {
            RoomCtrl.Instance.ExitInviteList();
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
        }

        private void InviteList(GameObject go)
        {
            if(nickName.Count < 1)
                return;
            foreach (var item in nickName)
            {
                if(item == null)
                    return;
                RoomCtrl.Instance.SendInviteList((int)GameUserModel.Instance.GameRoomID, item);
            }
            RoomCtrl.Instance.ExitInviteList();
        }

        public void ShowFriendOnLine()
        {
            foreach (var item in FriendManager.Instance.RoomInvite)
            {
                OnLineFriend olf = null;
                if (!OnLineNickName.TryGetValue(item.Key, out olf))
                {
                    ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadInvitationUI, ResourceType.PREFAB);
                    GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
                    obj.transform.parent = mOnLineGrid.transform;
                    obj.transform.localPosition = Vector3.zero;
                    obj.transform.localScale = Vector3.one;

                    olf = new OnLineFriend();
                    olf.mHeroHeadIcon = obj.transform.FindChild("Portrait/Head").GetComponent<UISprite>();
                    olf.mHeadName = obj.transform.FindChild("Name/Label").GetComponent<UILabel>();
                    olf.mSelect = obj.GetComponent<UIToggle>();
                    OnLineNickName.Add(item.Key, olf);
                    EventDelegate.Add(olf.mSelect.onChange, OnSelectName);
                }
                olf.SGUID = item.Key;
                olf.mHeadName.text = olf.nickName = item.Value.nickName;
                olf.mHeroHeadIcon.spriteName = item.Value.ToString();
                OnLineNickName[item.Key] = olf;
            }
        }
        void OnSelectName()
        {
            foreach (var item in OnLineNickName.Values)
            {
                if (item.mSelect.value)
                {
                    nickName.Add(item.SGUID);
                }
                else
                {
                    if (nickName.Contains(item.SGUID))
                        nickName.Remove(item.SGUID);
                }
            }
            InviteBtn.isEnabled = (bool)(nickName.Count != 0);
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_InviteList, ShowFriendOnLine);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_InviteList, ShowFriendOnLine);
        }

        //显示
        public override void OnEnable()
        {
            ShowFriendOnLine();
        }

        //隐藏
        public override void OnDisable()
        {

        }
        List<UInt64> nickName = new List<UInt64>();
        Dictionary<UInt64, OnLineFriend> OnLineNickName = new Dictionary<UInt64, OnLineFriend>();
        UIToggle ToggleFriend = null;
        UIToggle ToggleLobby = null;
        UIGrid mOnLineGrid = null;
        UIButton InviteBtn = null;

        UIButton CloseBtn = null;

        class OnLineFriend
        {
            public UInt64 SGUID;
            public string nickName;
            public UISprite mHeroHeadIcon;
            public UILabel mHeadName;
            public  UIToggle mSelect;
        }
    }
}