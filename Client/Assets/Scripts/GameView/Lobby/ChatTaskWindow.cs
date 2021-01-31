using UnityEngine;
using System.Collections;
using GameDefine;
using System.Collections.Generic;
using BlGame.Resource;
using BlGame.Ctrl;
using BlGame.GameEntity;
using System.Text;
using System;
using BlGame.GameData;
using BlGame.Model;
using System.Linq;

namespace BlGame.View
{
    public class ChatTaskWindow : BaseWindow
    {
        public ChatTaskWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadChatTaskUI;
            mResident = false;
        }
        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_ChatTaskEnter, ShowFriendTask);
            EventCenter.AddListener(EGameEvent.eGameEvent_ChatTaskExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        private void ShowFriendTask(UInt64 sGUID)
        {
            mLastTempNew = mLastTemp = sGUID;
            Show();
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_ChatTaskEnter, ShowFriendTask);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ChatTaskExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, RemoveTask);
        }

        private void RemoveTask()
        {
            mResident = false;
            Hide();
        }
        
        //窗口控件初始化
        protected override void InitWidget()
        {
            mLastObj = null;
            CloseChat = mRoot.FindChild("CloseBtn").gameObject;
            MsgInfo = mRoot.FindChild("ChatWindow/InputArea").GetComponent<UIInput>();
            SendMsgInfo = mRoot.FindChild("ChatWindow/SendMSg").gameObject;
            mChatTaskNameGrid = mRoot.FindChild("ChatList/Panel/Grid").GetComponent<UIGrid>();
            TalkTable = mRoot.FindChild("ChatWindow/Scroll View/Table").GetComponent<UITable>();
            ScrollView = mRoot.FindChild("ChatWindow/Scroll View").GetComponent<UIScrollView>();
            ScrollBar = mRoot.FindChild("ChatWindow/Control - Simple Vertical Scroll Bar").GetComponent<UIScrollBar>();
            ScrollMemBer = mRoot.FindChild("ChatList/Control - Simple Vertical Scroll Bar").GetComponent<UIScrollBar>();
            UIEventListener.Get(CloseChat).onClick += CloseChatPress;
            UIEventListener.Get(SendMsgInfo).onClick += SendChatPress;
            bool isTure = false;
            if (mLastObj != null || FriendManager.Instance.AllTalkDic.Count < 1)
                CreateMember(mLastTemp);
            else
            {
                foreach (var item in FriendManager.Instance.AllTalkDic.Values)
                {
                    if (item.SGUID == mLastTempNew)
                    {
                        isTure = true;
                        continue;
                    }
                    GameObject gameObj = null;
                    if (item.NickName == null && string.IsNullOrEmpty(item.NickName))
                        continue;
                    if (TalkChatName.TryGetValue(item.SGUID, out gameObj))
                    {
                        if (IsVisable && TalkChatObj.ContainsKey(gameObj))
                            TalkChatObj[gameObj].mTwinkle.SetActive(true);
                        continue;
                    }
                    if (item.TalkState == MsgTalk.UnReadMsg)
                    {
                        isReadInfo = false;
                    }
                    else
                    {
                        isReadInfo = true;
                    }
                    CreateMember(item.SGUID);
                }
                if (mLastTempNew != 0 && (isTure || !FriendManager.Instance.AllTalkDic.ContainsKey(mLastTempNew)))
                {
                    isReadInfo = true;
                    CreateMember(mLastTempNew);
                }
				ChatTaskCtrl.Instance.SetNewChat();
            }
        }

        bool isReadInfo = true;
        //窗口控件释放
        protected override void RealseWidget()
        {
        }
        public void OnScrollBarChange()
        {
            Debug.LogError(ScrollBar.value);
            if (ScrollBar.value != 1)
            {
                Debug.LogError("value:" + ScrollBar.value);
                ScrollBar.value = 1;
            }
            EventDelegate.Remove(ScrollBar.onChange, OnScrollBarChange);
        }
        bool IsVisable = false;
        void CreateMember(UInt64 temp)
        {
            if (mChatTaskNameGrid.transform.childCount <= 0)
                EventDelegate.Add(ScrollBar.onChange, OnScrollBarChange);
            if (temp == 0)
                return;
            Friend friend = null;
            if (FriendManager.Instance.AllFriends.TryGetValue(temp,out friend))
            {
                ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadChatMember, ResourceType.PREFAB);
                GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
                obj.transform.parent = mChatTaskNameGrid.transform;
                obj.transform.localPosition = Vector3.zero;
                obj.transform.localScale = Vector3.one;

                ChatNameMember chat = new ChatNameMember();
                chat.mNickName = obj.transform.FindChild("Name").GetComponent<UILabel>();
                chat.mHighlight = obj.transform.FindChild("Highlight").gameObject;
                chat.mTwinkle = obj.transform.FindChild("Twinkle").gameObject;
                chat.mNickName.text = friend.NiceName;
                chat.sGUID = temp;

                chat.objParent = obj;
                chat.mTwinkle.gameObject.SetActive(!isReadInfo);
                if (mLastObj == null && (mLastTempNew == temp || mLastTempNew == 0))
                {
                    mLastObj = chat;
                    mLastTemp = temp;
                    chat.mHighlight.SetActive(true);
                    chat.mTwinkle.gameObject.SetActive(false);
                    ClearOld();
                }
                if (!TalkChatName.ContainsKey(temp))
                    TalkChatName.Add(temp, obj);
                if (!TalkChatObj.ContainsKey(obj))
                    TalkChatObj.Add(obj, chat);
                UIEventListener.Get(obj).onClick += ChatNameSelect;
                mChatTaskNameGrid.repositionNow = true;
                mChatTaskNameGrid.Reposition();
                ScrollBar.value = 1f;
            }
            
        }

        private void ChatNameSelect(GameObject go)
        {
            if (mLastObj != null && mLastObj.objParent == go)
            {
                return;
            }
            if (mLastObj != null)
            {
                mLastObj.mHighlight.SetActive(false);
            }
            if (TalkChatObj.TryGetValue(go, out mLastObj))
            {
                mLastObj.mHighlight.SetActive(true);
                mLastObj.mTwinkle.SetActive(false);
                mLastTemp = mLastObj.sGUID;
            }
            ClearOld();
            Refresh();
        }

        void ClearOld()
        {
            foreach (var item in TaskChat)
            {
                UnityEngine.GameObject.DestroyImmediate(item);
            }
            
            if (FriendManager.Instance.AllTalkDic.Count < 1)
                return;
            IChat cat = null;
            if (FriendManager.Instance.AllTalkDic.TryGetValue(mLastTemp, out cat))
            {
                cat.SetTalkState(GameDefine.MsgTalk.ReadMsg);
                foreach (var item in cat.MsgInfo)
                {
                    ShowTalk(mLastTemp,item.nickName, item.msg, item.head, item.isLocalPlayer);
                }
            }
        }

        private void SendChatPress(GameObject go)
        {
            //chatta
            if (string.Compare(str, GameUserModel.Instance.GameUserNick) == 0)
            {
                MsgInfoManager.Instance.ShowMsg(10028);
                return;
            }
            if (MsgInfo.label.text.CompareTo(str) == 0 || string.IsNullOrEmpty(MsgInfo.value))
            {
                MsgInfoManager.Instance.ShowMsg(-16);
                return;
            }
            if (mLastTemp == 0)
            {
                //Debug.LogError("接受消息对象不能为空");
                MsgInfoManager.Instance.ShowMsg(10027);
                return;
            }
            Friend friend = null;
            if (FriendManager.Instance.AllFriends.TryGetValue(mLastTemp, out friend))
            {
                if (!friend.isOnline)
                {
                    MsgInfoManager.Instance.ShowMsg(-130846);
                    return;
                }
            }
            if (friend == null)
                return;
            string currStr = GameMethod.GetSplitStr(MsgInfo.value);
            byte[] utf8Bytes = Encoding.UTF8.GetBytes(currStr);
            ViableTalk(currStr);
            MsgInfo.value = "";
            ChatTaskCtrl.Instance.SendMsg(mLastTemp, (UInt32)utf8Bytes.Length, utf8Bytes);
        }

        void ViableTalk(string currStr)
        {
            Friend friend = null;
            IChat cat = null;
            if (FriendManager.Instance.AllFriends.TryGetValue(mLastTemp, out friend))
            {
                if (!FriendManager.Instance.AllTalkDic.TryGetValue(mLastTemp, out cat))
                {
                    cat = new IChat();

                    cat.SetMsgInfo(mLastTemp, friend.NiceName, currStr, MsgTalk.ReadMsg, GameUserModel.Instance.GameUserHead, true);
                    FriendManager.Instance.AllTalkDic.Add(mLastTemp, cat);
                }
                else
                    cat.SetMsgInfo(mLastTemp, friend.NiceName, currStr, MsgTalk.ReadMsg, GameUserModel.Instance.GameUserHead, true);
            }
            ShowTalk(mLastTemp, friend.NiceName, currStr, GameUserModel.Instance.GameUserHead, true);
        }

        void ShowTalk(UInt64 sGUID ,string nickName, string info, int head, bool cat)
        {
            ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadTalk, ResourceType.PREFAB);
            GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
            obj.transform.parent = TalkTable.transform;
            obj.transform.localPosition = Vector3.zero;
            obj.transform.localScale = Vector3.one;
            SetVib(nickName, obj, info, head, !cat);
            TaskChat.Add(obj);
            TalkTable.repositionNow = true;
            TalkTable.Reposition();
            ScrollBar.value = 1;
        }

        void SetVib(string name, GameObject obj, string info, int head, bool isLoc)
        {
            UILabel label1 = obj.transform.FindChild("Conversation1/Show/Label").GetComponent<UILabel>();
            UISprite sprite1 = obj.transform.FindChild("Conversation1/Portrait/Head").GetComponent<UISprite>();
            UISprite sprite2 = obj.transform.FindChild("Conversation1/Show/BG").GetComponent<UISprite>();
            BoxCollider bc = obj.transform.GetComponent<BoxCollider>();
            label1.transform.parent.parent.gameObject.SetActive(true);
            label1.text = info;
            sprite1.spriteName = head.ToString();
            if (label1.localSize.x > 412)
            {
                label1.overflowMethod = UILabel.Overflow.ResizeHeight;
                label1.SetDimensions(420, (int)label1.localSize.x / 412);
                label1.spacingY = 10;
                label1.pivot = UIWidget.Pivot.Left;
            }
            label1.text = info;
            if (isLoc)
            {
                sprite2.spriteName = "248";
            }
            else
            {
                //label1.text = "[ff0bf5]" + info + "[-]";
                sprite2.spriteName = "249";
            }
            bc.size = new Vector3(label1.localSize.x + 50, label1.localSize.y, 0);
            bc.center = new Vector3((label1.localSize.x + 50) / 2, -label1.localSize.y / 2, 0);
        }

        public void ShowTalkInfo(UInt64 sGUID)
        {
            isReadInfo = false;
            if (mLastTemp == sGUID)
            {
                IChat cat = null;
                mLastTemp = sGUID;
                if (FriendManager.Instance.AllTalkDic.TryGetValue(sGUID, out cat))
                {
                    cat.SetTalkState(GameDefine.MsgTalk.ReadMsg);
                    ShowTalk(sGUID,cat.NickName, cat.MsgInfo[cat.MsgInfo.Count - 1].msg, cat.MsgInfo[cat.MsgInfo.Count - 1].head, cat.MsgInfo[cat.MsgInfo.Count - 1].isLocalPlayer);
                }
                ChatTaskCtrl.Instance.SetNewChat();
            }
            else
            {
                IsVisable = true;
                if (isCreate(sGUID))
                    CreateMember(sGUID);
                Refresh();
            }
        }

        void Refresh()
        {
            GameObject obje = null;
            foreach (var item in FriendManager.Instance.AllTalkDic.Values)
            {
                if (TalkChatName.TryGetValue(item.SGUID, out obje))
                {
                    if (item.TalkState == MsgTalk.UnReadMsg)
                    {
                        TalkChatObj[obje].mTwinkle.SetActive(!isReadInfo);
                    }
                    else if (item.TalkState == MsgTalk.ReadMsg)
                    {
                        TalkChatObj[obje].mTwinkle.SetActive(isReadInfo);
                    }
                }
            }
            ChatTaskCtrl.Instance.SetNewChat();
        }

        bool isCreate(UInt64 temp)
        {
            GameObject gameObj = null;
            if (temp == 0)
                return false;
            if (TalkChatName.TryGetValue(temp, out gameObj))
            {
                if (IsVisable && TalkChatObj.ContainsKey(gameObj))
                    TalkChatObj[gameObj].mTwinkle.SetActive(true);
                return false;
            }
            //if (mLastObj != null && IsVisable)
            //{
            //    mLastObj.mHighlight.SetActive(false);
            //    mLastObj = null;
            //}
            return true;
        }

        private void CloseChatPress(GameObject go)
        {
            mLastObj = null;
            TalkChatName.Clear();
            TalkChatObj.Clear();
            ChatTaskCtrl.Instance.Exit();
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_ReadyChatTaskEnter, CreateMember);
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_ReceiveNewMsg, ShowTalkInfo);
            EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_RemoveChatTask, RemovChat);
        }

        private void RemovChat(UInt64 arg1)
        {
            GameObject tempObj = null;
            if (arg1 == 0)
                return;
            ClearOld();
            if (TalkChatName.TryGetValue(arg1, out tempObj))
            {
                if(TalkChatName.Count <= 0)
                {
                    mLastTemp = 0;
                    mLastObj = null;
                }else
                {
                    mLastTemp = TalkChatName.ElementAt(0).Key;
                    mLastObj = TalkChatObj[TalkChatName.ElementAt(0).Value];
					if (TalkChatObj.ContainsKey(tempObj))
					{
						UnityEngine.GameObject.DestroyImmediate(TalkChatObj[tempObj].objParent);
						TalkChatObj[tempObj] = null;
						TalkChatObj.Remove(tempObj);
					}
                }
                Refresh();
                TalkChatName[arg1] = null;
                TalkChatName.Remove(arg1);
                mChatTaskNameGrid.repositionNow = true;
                mChatTaskNameGrid.Reposition();
            }
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_ReadyChatTaskEnter, CreateMember);
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_ReceiveNewMsg, ShowTalkInfo);
            EventCenter.RemoveListener<UInt64>(EGameEvent.eGameEvent_RemoveChatTask, RemovChat);
        }

        //显示
        public override void OnEnable()
        {

        }

        //隐藏
        public override void OnDisable()
        {
            mLastObj = null;
            mLastTemp = 0;
        }

        private ChatNameMember mLastObj = null;
        private UIInput MsgInfo;
        private GameObject SendMsgInfo;
        private UIGrid mChatTaskNameGrid;
        private UITable TalkTable;
        private GameObject CloseChat;
        private UIScrollView ScrollView;
        private UIScrollBar ScrollMemBer;
        private UIScrollBar ScrollBar;
        private Dictionary<GameObject, ChatNameMember> TalkChatObj = new Dictionary<GameObject, ChatNameMember>();
        private Dictionary<UInt64, GameObject> TalkChatName = new Dictionary<UInt64, GameObject>();
        private List<GameObject> TaskChat = new List<GameObject>();
        private UInt64 mLastTemp;
        private UInt64 mLastTempNew;
        string str = "";
        public class ChatNameMember
        {
            public GameObject objParent;
            public UInt64 sGUID;
            public UILabel mNickName;
            public GameObject mHighlight;
            public GameObject mClickChatName;
            public GameObject mTwinkle;
        }
    }
}
