using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Ctrl;
using System.Collections.Generic;
using BlGame.GameEntity;
using BlGame.Resource;
using JT.FWW.GameData;
using System;
using System.Text.RegularExpressions;

namespace BlGame.View{
    public class SocialWindow : BaseWindow {

        public SocialWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadMakeFriendsUI;
            mResident = true;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_SocialEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_SocialExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }


        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SocialEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SocialExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mFriendToggle = mRoot.FindChild("MenuBtn/MyFriends").GetComponent<UIToggle>();
            mBlackToggle = mRoot.FindChild("MenuBtn/BlackList").GetComponent<UIToggle>();
            mRecentlyToggle = mRoot.FindChild("MenuBtn/Recently").GetComponent<UIToggle>();
            mFriendGrid = mRoot.FindChild("MenuOptions/ListView/Grid").GetComponent<UIGrid>();
            mAddFriendToggle = mRoot.FindChild("MenuOptions/FriendsOption/Button/AddFriends").gameObject;

            mDelFriendToggle = mRoot.FindChild("MenuOptions/FriendsOption/Button/DeleteFriends").GetComponent<UIButton>();
            mMoveToBlackTog = mRoot.FindChild("MenuOptions/FriendsOption/Button/MoveToBlack").GetComponent<UIButton>();
            mSendMsgToggle = mRoot.FindChild("MenuOptions/FriendsOption/Button/SendMsg").GetComponent<UIButton>();
            mSendAddFriend = mRoot.FindChild("MenuOptions/AddInterface/Button").gameObject;

            mInputTemp = mRoot.FindChild("MenuOptions/AddInterface/Input/InputArea").GetComponent<UIInput>();
            mSureDel = mRoot.FindChild("MenuOptions/FriendsOption/DeleteInterface/Confirm").gameObject;
            mCanelDel = mRoot.FindChild("MenuOptions/FriendsOption/DeleteInterface/Cancel").gameObject;
            ToggleID = mRoot.FindChild("MenuOptions/AddInterface/ModeSelect/ID").GetComponent<UIToggle>();
            ToggleName = mRoot.FindChild("MenuOptions/AddInterface/ModeSelect/NickName").GetComponent<UIToggle>();

            mSureToBlack = mRoot.FindChild("MenuOptions/FriendsOption/BlackInterface/Confirm").gameObject;
            mCanelToBlack = mRoot.FindChild("MenuOptions/FriendsOption/BlackInterface/Cancel").gameObject;
            mRemoveToBlack = mRoot.FindChild("MenuOptions/BlackListOption/DeleteFriends").GetComponent<UIButton>();
            mAddToBlack = mRoot.FindChild("MenuOptions/BlackListOption/AddToBlack").gameObject;
            mDefault = mInputTemp.transform.FindChild("Num").GetComponent<UILabel>();

            CloseBtn = mRoot.FindChild("MenuOptions/AddInterface/BackButton").gameObject;
            UIEventListener.Get(mRemoveToBlack.gameObject).onClick += RemoveBlack;
            UIEventListener.Get(mAddToBlack).onClick += OpenAddFriend;

            UIEventListener.Get(mSureDel).onClick += SureDelFriend;
            UIEventListener.Get(mCanelDel).onClick += CanelDelFriend;
            UIEventListener.Get(mSureToBlack).onClick += SureFriendToBlack;
            UIEventListener.Get(mCanelToBlack).onClick += CanelFriendToBlack;

            UIEventListener.Get(mAddFriendToggle).onClick += OpenAddFriend;
            UIEventListener.Get(mDelFriendToggle.gameObject).onClick += OpenDelFriend;
            UIEventListener.Get(mMoveToBlackTog.gameObject).onClick += OpenMoveToBlack;
            UIEventListener.Get(mSendMsgToggle.gameObject).onClick += OpenChatTask;

            UIEventListener.Get(CloseBtn).onClick += CloseOnPress;
            UIEventListener.Get(mInputTemp.gameObject).onSelect += ChangeSelect;
            UIEventListener.Get(mSendAddFriend).onClick += SendAddFriendBlack;
            EventDelegate.Add(mFriendToggle.onChange, FriendList);

            EventDelegate.Add(mBlackToggle.onChange, BlackList);
            EventDelegate.Add(mRecentlyToggle.onChange, RecentlyList);
            EventDelegate.Add(ToggleID.onChange, ToggleClick);
            EventDelegate.Add(ToggleName.onChange, ToggleClick);
        }

        private void CloseOnPress(GameObject go)
        {
            mDefault.text = PutName;
            mSendAddFriend.transform.parent.gameObject.SetActive(false);
        }

        private void ToggleClick()
        {
            if (ToggleID.value)
            {

            }
            else if (ToggleName.value)
            {
 
            }
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
        }

        private void AddCurrToBlack(GameObject go)
        {
            
        }

        private void RecentlyList()
        {
            if (mRecentlyToggle.value)
            {
                Clear(mFriendGrid.transform);
            }
            mFriendGrid.repositionNow = true;
            mFriendGrid.Reposition();
        }

        private void RemoveBlack(GameObject go)
        {
            if (LastObj != null && FriendManager.Instance.OnClickKey.ContainsKey(LastObj.objParent))
            {
                SGUID = FriendManager.Instance.OnClickKey[LastObj.objParent].SGUID;
                LastTemp = FriendManager.Instance.OnClickKey[LastObj.objParent].NiceName;
                SocialCtrl.Instance.RemoveFriend(SGUID, FRIENDTYPE.BLACKLIST);
                SGUID = 0; LastObj = null;
            }
        }

        private void CanelFriendToBlack(GameObject go)
        {
            mSureToBlack.transform.parent.gameObject.SetActive(false);
        }

        private void SureFriendToBlack(GameObject go)
        {
            if (SGUID != 0)
            {
                SocialCtrl.Instance.AddFriendBlackID(SGUID, FRIENDTYPE.BLACKLIST);
                mSureToBlack.transform.parent.gameObject.SetActive(false);
                SGUID = 0; LastObj = null;
            }
        }

        private void CanelDelFriend(GameObject go)
        {
            mSureDel.transform.parent.gameObject.SetActive(false);
        }

        private void SureDelFriend(GameObject go)
        {
            if (SGUID != 0)
            {
                SocialCtrl.Instance.RemoveFriend(SGUID, FRIENDTYPE.FRIENDLIST);
                mSureDel.transform.parent.gameObject.SetActive(false);
                SGUID = 0; LastObj = null;
            }
        }

        private void ChangeSelect(GameObject go, bool state)
        {
            if (state)
            {
                mInputTemp.value = "";
            }
        }

        private void OpenMoveToBlack(GameObject go)
        {
            //先判断是否选中
            if (!SelectFriend(mSureToBlack.transform.parent)) 
                return;
            mSureToBlack.transform.parent.gameObject.SetActive(SGUID != 0);
        }

        private void OpenChatTask(GameObject go)
        {
            if (!SelectFriend(mSureToBlack.transform.parent))
                return;
            ChatTaskCtrl.Instance.Enter(SGUID);
        }

        private void OpenDelFriend(GameObject go)
        {
            //先判断是否选中
            if (!SelectFriend(mSureDel.transform.parent))
                return;
            mSureDel.transform.parent.gameObject.SetActive(SGUID!= 0);
            
        }
        bool SelectFriend(Transform temp)
        {
            if (LastObj != null && FriendManager.Instance.OnClickKey.ContainsKey(LastObj.objParent))
            {
                SGUID = FriendManager.Instance.OnClickKey[LastObj.objParent].SGUID;
                LastTemp = FriendManager.Instance.OnClickKey[LastObj.objParent].NiceName;
                temp.FindChild("Name").GetComponent<UILabel>().text = LastTemp;
                return true;
            }
            else if (LastObj == null)
            {
                MsgInfoManager.Instance.ShowMsg(10041);
                return false;
            }
            return false;
        }
        
        private void OpenAddFriend(GameObject go)
        {
            mSendAddFriend.transform.parent.gameObject.SetActive(true);
        }

        bool IsAddFriend(string value)
        {
            foreach (var item in FriendManager.Instance.AllFriends)
            {
                if (string.Compare(item.Value.NiceName, value) == 0)
                {
                    MsgInfoManager.Instance.ShowMsg(-130821);
                    return false;
                }
            }
            if (FriendManager.Instance.AllFriends.ContainsKey(SGUID))
            {
                if (string.Compare(FriendManager.Instance.AllFriends[SGUID].NiceName, value) == 0)
                {
                    MsgInfoManager.Instance.ShowMsg(-130821);
                    return false;
                }
            }
            foreach (var item in FriendManager.Instance.AllBlackDic)
            {
                if (string.Compare(item.Value.NiceName, value) == 0)
                {
                    MsgInfoManager.Instance.ShowMsg(-130820);
                    return false;
                }
            }
            if (FriendManager.Instance.AllBlackDic.ContainsKey(SGUID))
            {
                if (string.Compare(FriendManager.Instance.AllBlackDic[SGUID].NiceName, value) == 0)
                {
                    MsgInfoManager.Instance.ShowMsg(-130820);
                    return false;
                }
            }
            return true;
        }
        private void SendAddFriendBlack(GameObject go)
        {//通过ID查找用户
            if (mInputTemp.value != null && !string.IsNullOrEmpty(mInputTemp.value) && (string.Compare(mInputTemp.value, PutName) != 0))
            {
                if (ToggleID.value)
                {
                    string pat = @"[\u4e00-\u9fa5]";
                    Regex rg = new Regex(pat);
                    Match mh = rg.Match(mInputTemp.value);
                    if (mh.Success)
                    {
                        MsgInfoManager.Instance.ShowMsg(10030);
                        return;
                    }
                    if (IsAddFriend(mInputTemp.value))
                    {
                        if (mBlackToggle.value)
                        {
                            SocialCtrl.Instance.AddFriendBlackID(Convert.ToUInt64(mInputTemp.value), FRIENDTYPE.BLACKLIST);
                        }
                        else if (mFriendToggle.value)
                        {
                            SocialCtrl.Instance.AddFriendBlackID(Convert.ToUInt64(mInputTemp.value), FRIENDTYPE.FRIENDLIST);
                        }
                        MsgInfoManager.Instance.ShowMsg(-130775);
                    }
                }//通过昵称查找用户
                else if (ToggleName.value)
                {
                    if (IsAddFriend(mInputTemp.value))
                    {
                        if (mBlackToggle.value)
                        {
                            SocialCtrl.Instance.AddFriendBlackList(mInputTemp.value, FRIENDTYPE.BLACKLIST);
                        }
                        else if (mFriendToggle.value)
                        {
                            SocialCtrl.Instance.AddFriendBlackList(mInputTemp.value, FRIENDTYPE.FRIENDLIST);
                        }
                        MsgInfoManager.Instance.ShowMsg(-130775);
                    }
                }
                //mSendAddFriend.transform.parent.gameObject.SetActive(false);
            }
            else
            {
                MsgInfoManager.Instance.ShowMsg(10029);
            }
            mInputTemp.value = "";
        }
        
        private void BlackList()
        {
            if (mBlackToggle.value)
            {
                Clear(mFriendGrid.transform);
                ShowOnLine(false, FriendManager.Instance.AllBlackDic, false);
                mFriendGrid.repositionNow = true;
                mFriendGrid.Reposition();
            }
        }
        void Clear(Transform temp)
        {
            LastObj = null;
            SGUID = 0;
            foreach (var item in FriendListUI)
            {
                UnityEngine.GameObject.DestroyImmediate(item.Key);
            }
            FriendListUI.Clear();
            FriendManager.Instance.OnClickKey.Clear();
        }
        private void FriendList()
        { 
            if (mFriendToggle.value)
            {
                Clear(mFriendGrid.transform);
                ShowOnLine(true, FriendManager.Instance.AllFriends);
                ShowOnLine(false, FriendManager.Instance.AllFriends);
            }
            mFriendGrid.repositionNow = true;
            mFriendGrid.Reposition();
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_RemoveFriendEnd, RemoveGrid);
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_FriendChangeInfo, ChangeOnLine);
            EventCenter.AddListener<Friend>(EGameEvent.eGameEvent_CreateFriendPrefab, NewFriend);
            EventCenter.AddListener(EGameEvent.eGameEvent_CreateBlackPrefab, NewBlack);
            EventCenter.AddListener<EErrorCode>(EGameEvent.eGameEvent_AskFriendEorr, EventError);
        }
        
        private void NewFriend(Friend friend)
        {
            MsgInfoManager.Instance.ShowMsg(-130774);
            FriendList();
        }
        private void NewBlack()
        {
            MsgInfoManager.Instance.ShowMsg(-130774);
            BlackList();
        }


        //更改在线状态
        public void ChangeOnLine(UInt64 temp)
        {
            GameObject obj = FriendManager.Instance.GetObj(temp);
            if (obj == null)
                return;
            bool isVib = false;
            if (FriendManager.Instance.AllFriends.ContainsKey(temp))
            {
                isVib = FriendManager.Instance.AllFriends[temp].isOnline;
            }
            else if (FriendManager.Instance.AllBlackDic.ContainsKey(temp))
            {
                isVib = FriendManager.Instance.AllBlackDic[temp].isOnline;
            }
            if (FriendListUI.ContainsKey(obj))
            {
                FriendListUI[obj].mOnLine.gameObject.SetActive(isVib);
                FriendListUI[obj].mOffLine.gameObject.SetActive(!isVib);
                FriendListUI[obj].mNickName.text = FriendManager.Instance.AllFriends[temp].NiceName;
                uint vip = FriendManager.Instance.AllFriends[temp].isVip;
                FriendListUI[obj].mVipSign.text = "VIP" + vip.ToString();
                FriendListUI[obj].mVipSign.gameObject.SetActive(vip != 0);
            }
            mFriendGrid.repositionNow = true;
            mFriendGrid.Reposition();
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_RemoveFriendEnd, RemoveGrid);
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_FriendChangeInfo, ChangeOnLine);
            EventCenter.RemoveListener<Friend>(EGameEvent.eGameEvent_CreateFriendPrefab, NewFriend);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_CreateBlackPrefab, NewBlack);
            EventCenter.RemoveListener<EErrorCode>(EGameEvent.eGameEvent_AskFriendEorr, EventError);
        }

        private void EventError(EErrorCode arg1)
        {
            MsgInfoManager.Instance.ShowMsg((int)arg1);
        }


        //显示
        public override void OnEnable()
        {
            
        }

        //隐藏
        public override void OnDisable()
        {

        }
        private void RemoveGrid(UInt64 sGUID)
        {
            UIFriend temp = null;
            GameObject obj = null;
            if (FriendManager.Instance.AllTalkDic.ContainsKey(sGUID))
            {
                FriendManager.Instance.DelAllTalkDic(sGUID);
                ChatTaskCtrl.Instance.SetDestoy(sGUID);
            }
            if (LastObj == null)
            {
                obj = FriendManager.Instance.GetObj(sGUID);
                if (obj == null)
                    return;
                FriendListUI.TryGetValue(obj, out temp);
            }else if ( FriendListUI.TryGetValue(LastObj.objParent, out temp))
            {
                obj = LastObj.objParent;
            }
            ChatTaskCtrl.Instance.SetNewChat();
            FriendManager.Instance.RemoveOnClickKey(obj);
            FriendListUI[obj] = null;
            FriendListUI.Remove(obj);
            UnityEngine.GameObject.DestroyImmediate(temp.objParent);
            LastObj = null; SGUID = 0;
            if (mFriendToggle.value)
                FriendList();
            else if (mBlackToggle.value)
                BlackList();
            mFriendGrid.repositionNow = true;
            mFriendGrid.Reposition();
        }
        void SetBtnVib(bool isV)
        {
            mDelFriendToggle.isEnabled = isV;
            mMoveToBlackTog.isEnabled = isV;
            mSendMsgToggle.isEnabled = isV;
            mRemoveToBlack.isEnabled = isV;
        }

        void ShowOnLine(bool isOn, Dictionary<UInt64, Friend> temp, bool isVib = true)
        {
            SetBtnVib(false);
            foreach (var item in temp.Values)
            {
                SetBtnVib(true);
                GameObject obj = null;
                if (item.isOnline == isOn)
                {
                    ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadFriendUI, ResourceType.PREFAB);
                    obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
                    obj.transform.parent = mFriendGrid.transform;
                    obj.transform.localPosition = Vector3.zero;
                    obj.transform.localScale = Vector3.one;

                    UIFriend friend = new UIFriend();
                    friend.mHeadID = obj.transform.FindChild("HeadPhoto").GetComponent<UISprite>();
                    friend.mNickName = obj.transform.FindChild("Name").GetComponent<UILabel>();
                    friend.mVipSign = obj.transform.FindChild("VipSign").GetComponent<UILabel>();
                    friend.mLadderIntegral = obj.transform.FindChild("Ladder/Score").GetComponent<UILabel>();
                    friend.mHighLight = obj.transform.FindChild("HighLight").GetComponent<UISprite>();
                    friend.mOnLine = obj.transform.FindChild("Online").gameObject;
                    friend.mOffLine = obj.transform.FindChild("Offline").gameObject;
                    friend.objParent = obj;
                    FriendListUI.Add(obj,friend);
                    FriendManager.Instance.AddOnClickKey(obj, item);
                    if (isVib)
                    {
                        friend.mOnLine.gameObject.SetActive(item.isOnline);
                        friend.mOffLine.gameObject.SetActive(!item.isOnline);
                    }
                    else
                    {
                        friend.mOnLine.gameObject.SetActive(false);
                        friend.mOffLine.gameObject.SetActive(false);
                    }
                    friend.mNickName.text = friend.NickName = item.NiceName;
                    friend.mHeadID.spriteName = item.HeadId.ToString();
                    friend.mVipSign.gameObject.SetActive(item.isVip != 0);
                    friend.mVipSign.text = "VIP" + item.isVip.ToString();
                    UIEventListener.Get(obj).onClick += onClickSelect;
                }
            }
        }

        private void onClickSelect(GameObject go)
        {
            if (LastObj != null && LastObj.objParent == go)//the same
                 return;
            if (LastObj == null && FriendListUI.TryGetValue(go, out LastObj))// first time
            {
                LastObj.mHighLight.gameObject.SetActive(true);
                return;
             }
            if (LastObj != null) {
                LastObj.mHighLight.gameObject.SetActive(false);
            }             
             if (FriendListUI.TryGetValue(go, out LastObj))
             {
                 LastObj.mHighLight.gameObject.SetActive(true);
             }
        }
        UIFriend LastObj = null;
        private UInt64 SGUID = 0;
        private string LastTemp = null;
        bool isCheck = false;
        private const string PutID = "输入ID或昵称";
        private const string PutName = "请输入昵称";

        UIToggle mFriendToggle = null;//好友列表按钮
        UIToggle mBlackToggle = null;//黑名单
        UIToggle mRecentlyToggle = null;//
        GameObject CloseBtn = null;

        GameObject mAddFriendToggle = null;
        UIButton mDelFriendToggle = null;
        UIButton mMoveToBlackTog = null;
        UIButton mSendMsgToggle = null;

        /// <summary>
        /// 添加好友
        /// </summary>
        GameObject mSendAddFriend = null;
        UIInput mInputTemp = null;
        UIToggle ToggleID = null;
        UIToggle ToggleName = null;
        /// <summary>
        /// 删除
        /// </summary>

        GameObject mSureDel = null;
        GameObject mCanelDel = null;
        GameObject mSureToBlack = null;
        GameObject mCanelToBlack = null;
        UIButton mRemoveToBlack = null;
        GameObject mAddToBlack = null;

        UILabel mDefault;
        UIGrid mFriendGrid = null;
        GameObject Invitaion = null;

        private Dictionary<GameObject, UIFriend> FriendListUI = new Dictionary<GameObject, UIFriend>();
        
        class UIFriend
        {
            public GameObject objParent;
            public string NickName;
            public UISprite mHeadID;
            public UILabel mVipSign;
            public GameObject mOnLine;
            public GameObject mOffLine;
            public UILabel mNickName;
            public UILabel mLadderIntegral;
            public UISprite mHighLight;
        }
    }
}
