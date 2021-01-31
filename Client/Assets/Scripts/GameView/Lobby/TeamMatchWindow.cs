using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using BlGame.Ctrl;
using BlGame.GameEntity;
using BlGame.Model;
using System;

namespace BlGame.View
{
    

    public class TeamMatchWindow : BaseWindow
    {
        public TeamMatchWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadGameTeamMatchUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_TeamMatchEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_TeamMatchExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_TeamMatchEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_TeamMatchExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            //地图
            mMapNameLabel = mRoot.FindChild("MapInfo/Name").GetComponent<UILabel>();
            mMapTypeLabel = mRoot.FindChild("MapInfo/Type").GetComponent<UILabel>();
            mQuitBtn = mRoot.FindChild("QuitBtn").GetComponent<UIButton>();
            mMatchBtn = mRoot.FindChild("MatchBtn").GetComponent<UIButton>();

            //好友
            mFriendList = mRoot.FindChild("InvitationList/Grid");
            mInvitation = mRoot.FindChild("InvitationBtn").GetComponent<UIButton>();

            //队友
            mTeammateList = mRoot.FindChild("TeamMember");

            EventDelegate.Add(mQuitBtn.onClick, OnQuit);
            EventDelegate.Add(mInvitation.onClick, OnInvitation);
            EventDelegate.Add(mMatchBtn.onClick, OnMatch);
        }

        //窗口控件释放
        protected override void RealseWidget()
        {

        }

     
        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<Teammate>(EGameEvent.eGameEvent_TeamMatchAddTeammate, AddTeammate);
            EventCenter.AddListener<string>(EGameEvent.eGameEvent_TeamMatchDelTeammate, DelTeammate);

            EventCenter.AddListener<Friend>(EGameEvent.eGameEvent_CreateFriendPrefab, AddFriend);
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_RemoveFriend, DelFriend);
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_FriendChangeInfo, ChangeOnLine);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<Teammate>(EGameEvent.eGameEvent_TeamMatchAddTeammate, AddTeammate);
            EventCenter.RemoveListener<string>(EGameEvent.eGameEvent_TeamMatchDelTeammate, DelTeammate);

            EventCenter.RemoveListener<Friend>(EGameEvent.eGameEvent_CreateFriendPrefab, AddFriend);
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_RemoveFriend, DelFriend);
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_FriendChangeInfo, ChangeOnLine);
            
        }

        //显示
        public override void OnEnable()
        {
            //地图信息填充
            MapInfo info = MapLoadConfig.Instance.GetMapInfo(TeamMatchCtrl.Instance.GetMapId());

            if (info != null)
            {
                mMapNameLabel.text = info.mName;
                switch (TeamMatchCtrl.Instance.GetMatchType())
                {
                    case EBattleMatchType.EBMT_Normal:
                         mMapTypeLabel.text = "(" + ConfigReader.GetMsgInfo(40038).content + ")";
                         break;
                    case EBattleMatchType.EBMT_Rank:
                         mMapTypeLabel.text = "(" + ConfigReader.GetMsgInfo(40040).content + ")";
                        break;
                    case EBattleMatchType.EBMT_Ai:
                        mMapTypeLabel.text = "(" + ConfigReader.GetMsgInfo(40039).content + ")";
                        break;

                }
            }

            //更新好友列表
            RefreshFrienList();

            //清空队友
            LoadUiResource.ClearAllChild(mTeammateList);
        }

        //隐藏
        public override void OnDisable()
        {

        }

        //玩家好友列表更新
        private void RefreshFrienList(string name = "")
        {
            LoadUiResource.ClearAllChild(mFriendList);

            foreach (Friend friend in FriendManager.Instance.AllFriends.Values)
            {
                if (TeamMatchCtrl.Instance.GetTeammateList().ContainsKey(friend.NiceName) || (friend.isOnline == false))
                    continue;

                AddFriendItem(friend);
            }

            mFriendList.GetComponent<UIGrid>().Reposition();

            mInvitation.isEnabled = false;
        }

         //增加一个好友
        private void AddFriend(Friend friend)
        {
            if (friend.isOnline == true)
            {
                AddFriendItem(friend);
                mFriendList.GetComponent<UIGrid>().Reposition();
            }
        }

        
        //更新好友属性改变(在线否)
        private void ChangeOnLine(UInt64 temp)
        {
            if (FriendManager.Instance.AllFriends.ContainsKey(temp))
            {
                Friend friend = FriendManager.Instance.AllFriends[temp];
                if (friend.isOnline == false)
                {
                    LoadUiResource.ClearOneChild(mFriendList,friend.NiceName);
                    mFriendList.GetComponent<UIGrid>().Reposition();

                    OnFriendSelect(null);
                }
                else
                {
                    AddFriend(friend);
                }
            }
        }

        //删除一个好友
        private void DelFriend(UInt64 temp)
        {
            if (FriendManager.Instance.AllFriends.ContainsKey(temp))
            {
                LoadUiResource.ClearOneChild(mFriendList, FriendManager.Instance.AllFriends[temp].NiceName);

                mFriendList.GetComponent<UIGrid>().Reposition();

                OnFriendSelect(null);
            }
        }

        //增加一个好友选项
        private void AddFriendItem(Friend info)
        {
            GameObject obj = LoadUiResource.AddChildObject(mFriendList, GameConstDefine.LoadGameMatchFriendsItem);
            if (obj != null)
            {
                obj.name = info.NiceName;
                obj.transform.FindChild("Name").GetComponent<UILabel>().text = info.NiceName;
                UIEventListener.Get(obj).onClick += OnFriendSelect;
            }
        }

        private void OnFriendSelect(GameObject obj)
        {
            if (mIsLeader)
            {
                int count = 0;
                for (int i = 0; i < mFriendList.childCount; i++)
                {
                    if (mFriendList.GetChild(i).GetComponent<UIToggle>().value)
                    {
                        ++count;
                    }
                }

                if (count > 0 && !TeamMatchCtrl.Instance.IsTeamFull())
                {
                    mInvitation.isEnabled = true;
                }
                else
                {
                    mInvitation.isEnabled = false;
                }
            }
        }

        //邀请
        private void OnInvitation()
        {
            int count = 0;
            for (int i = 0; i < mFriendList.childCount; i++)
            {
                if (mFriendList.GetChild(i).GetComponent<UIToggle>().isChecked)
                {
                    TeamMatchCtrl.Instance.InvitationFriend(mFriendList.GetChild(i).gameObject.name);
                    ++count;
                }
            }

            if (count > 0)
            {
                MsgInfoManager.Instance.ShowMsg(-130775);
            }
            else
            {
                MsgInfoManager.Instance.ShowMsg(40049);
            }
            
        }

        //匹配
        private void OnMatch()
        {
            TeamMatchCtrl.Instance.AskStartMatch();
        }

        //更新队友列表
        private void RefreshTeammateList()
        {
            //更新功能按钮状态
            if (GameUserModel.Instance.GameUserNick == mMasterName)
            {
                mIsLeader = true;
                mMatchBtn.isEnabled = true;
            }
            else
            {
                mIsLeader = false;
                mMatchBtn.isEnabled = false;
            }

            mTeammateList.GetComponent<UIGrid>().Reposition();
        }

        //新增一个队友
        private void AddTeammate(Teammate team)
        {
            GameObject obj = LoadUiResource.AddChildObject(mTeammateList, GameConstDefine.LoadGameMatchTeammateItem);
            if (obj != null)
            {
                obj.name = team.mName;
                obj.transform.FindChild("Name").GetComponent<UILabel>().text = team.mName;
                obj.transform.FindChild("Head").GetComponent<UISprite>().spriteName = team.mPic;
                obj.transform.FindChild("Level").GetComponent<UILabel>().text = "LV" + team.mLv;

                //找出队长名称
                if (team.mPostion == 0)
                {
                    mMasterName = team.mName;
                }

                //删除好友
                foreach (Friend friend in FriendManager.Instance.AllFriends.Values)
                {
                    if (friend.NiceName == team.mName)
                    {
                        DelFriend(friend.SGUID);
                    }
                }

                RefreshTeammateList();
            }
        }

        //删除一个队友
        private void DelTeammate(string name)
        {
            LoadUiResource.ClearOneChild(mTeammateList,name);

            //增加好友
            foreach (Friend friend in FriendManager.Instance.AllFriends.Values)
            {
                if (friend.NiceName == name && friend.isOnline == true)
                {
                    AddFriend(friend);
                }
            }

            RefreshTeammateList();

            OnFriendSelect(null);
        }


        //退出
        public void OnQuit()
        {
            TeamMatchCtrl.Instance.QuitTeam();
        }

        UILabel mMapNameLabel;
        UILabel mMapTypeLabel;

        Transform mFriendList;
        Transform mTeammateList;
        UIButton mInvitation;
        UIButton mMatchBtn;
        UIButton mQuitBtn;

        string mMasterName;
       string mCurFriend;
       bool mIsLeader;
    }
}

