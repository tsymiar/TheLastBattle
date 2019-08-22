using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using JT.FWW;
using BlGame.GameData;
using JT.FWW.Tools;
using JT.FWW.GameData;
using BlGame.GameEntity;
using System.Text;
using UICommon;
using GameDefine;
using System.Linq;
using BlGame.GuideDate;

using BlGame.Ctrl;
using BlGame.Model;

namespace BlGame.View
{
    public class RoomWindow : BaseWindow
    {
        public RoomWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadGameRoomUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_RoomEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_RoomExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_RoomEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_RoomExit, Hide);

        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mMidObj = mRoot.FindChild("Animator_Area").gameObject;
            mBallObj = mRoot.FindChild("Ball_effect/thunder_gravity").gameObject;
            mStartObj = mRoot.FindChild("StartBtn_effect/start_stars").gameObject;
            mReadyObj = mRoot.FindChild("StartBtn_effect/start_burst").gameObject;

            //获得队伍名称Label
            for (int i = 0; i < TeamCount; i++)
            {
                mTteamObj[i] = mRoot.FindChild("TeamBackground" + (i + 1).ToString()).gameObject;
                UILabel label = mTteamObj[i].transform.FindChild("TeamPlate/Label").GetComponent<UILabel>();
                mTeamNameLabelList.Add(label);
            }

            //获得房间号Label
            mRoomNumLabel = mRoot.FindChild("Animator_Area/RoomID/Label").GetComponent<UILabel>();

            //获得控制按钮，开始，退出
            mBtnStart = mRoot.FindChild("Animator_Area/CtrlBtn/Start").GetComponent<ButtonOnPress>();
            mBtnQuit = mRoot.FindChild("Animator_Area/CtrlBtn/Quit").GetComponent<ButtonOnPress>();
            InvaitaionBtn = mRoot.FindChild("Animator_Area/CtrlBtn/Mall").GetComponent<UIButton>();
            mQuitGameLabel = mRoot.FindChild("Animator_Area/CtrlBtn/Quit/Label").GetComponent<UILabel>();

            

            //座位UI
            for (int i = 0; i < SeatCount; i++)
            {
                Transform team;
                if (i % 2 == 0)
                {
                    team = mTteamObj[0].transform;
                }
                else
                {
                    team = mTteamObj[1].transform;
                }
                Transform seatParent = team.FindChild("Player" + (i + 1).ToString());
                Transform master = seatParent.FindChild("HostSign");
                Transform name;
                if (i < SeatTeam)
                {
                    Transform head = seatParent.FindChild("Hero").FindChild("Thumbnail");
                    name = seatParent.FindChild("Name");
                    Transform money = seatParent.FindChild("Gold").FindChild("GoldNumber");
                    Transform moneyIcon = seatParent.FindChild("Gold").FindChild("Icon");

                    Transform ready = seatParent.FindChild("Preparation");
                    Transform tip = seatParent.FindChild("Tip");
                    mHeadList.Add(head.GetComponent<UISprite>());
                    mNameList.Add(name.GetComponent<UILabel>());
                    mTipList.Add(tip.GetComponent<UILabel>());
                    mMoneyList.Add(money.GetComponent<UILabel>());
                    mMoneyIconList.Add(moneyIcon.GetComponent<UISprite>());
                    mMasterList.Add(master.GetComponent<UISprite>());
                    mReadyList.Add(ready.GetComponent<UISprite>());
                    mSoulObj[i] = mRoot.FindChild("soul_fly" + (i + 1).ToString()).gameObject;
                }
                else
                {
                    mMasterList.Add(master.GetComponent<UISprite>());
                    name = seatParent.FindChild("Name");
                    Transform tip = seatParent.FindChild("Tip");
                    mNameList.Add(name.GetComponent<UILabel>());
                    mTipList.Add(tip.GetComponent<UILabel>());
                }
                mSeatBtnList.Add(seatParent.GetComponent<ButtonOnPress>());
            }
            mRightDefaultSprite = mHeadList[0].spriteName;
            mLeftDefaultSprite = mHeadList[1].spriteName;

            //聊天
            Transform talkParent = mRoot.FindChild("Animator_Area/TalkArea");
            mBtnSend = mRoot.FindChild("Animator_Area/TalkArea/SendMsg").GetComponent<ButtonOnPress>();
            mInputTalk = mRoot.FindChild("Animator_Area/TalkArea/Input").GetComponent<UIInput>();
            mInputTalkList = mInputTalk.GetComponent<UITextList>();
            mTalkShow = talkParent.FindChild("ShowTalk").GetComponent<UITextList>();

            //控制按钮
           
            for (int i = 0; i < 2; i++)
            {
                UISprite parentBg = mBtnStart.transform.FindChild("BG/BG" + (i + 1).ToString()).GetComponent<UISprite>();
                mBgSprite.Add(parentBg);
                UISprite parentReady = mBtnStart.transform.FindChild("Ready/Ready" + (i + 1).ToString()).GetComponent<UISprite>();
                mReadySprite.Add(parentReady);
                UISprite parentStart = mBtnStart.transform.FindChild("Start/Start" + (i + 1).ToString()).GetComponent<UISprite>();
                mStartSprite.Add(parentStart);
            }
            mObStart = mBtnStart.transform.FindChild("Start3").GetComponent<UISprite>();

            mBtnStart.GetComponent<ButtonSelectPic>().AddListener(PressStartBtn);

            UIEventListener.Get(InvaitaionBtn.gameObject).onClick += CreateInvitation;
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
            mTeamNameLabelList.Clear();
            mHeadList.Clear();
            mNameList.Clear();
            mTipList.Clear();
            mMoneyList.Clear();
            mMoneyIconList.Clear();
            mSeatBtnList.Clear();
            mMasterList.Clear();
            mReadyList.Clear();
            mBgSprite.Clear();
            mReadySprite.Clear();
            mStartSprite.Clear();
        }

     
        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_RoomEnd, ShowRoomEnd);
            EventCenter.AddListener(EGameEvent.eGameEvent_SeatPosUpdate, RrefreshSeat);
            EventCenter.AddListener<EErrorCode>(EGameEvent.eGameEvent_AskBeginBattleError, BeginBattleError);
            EventCenter.AddListener<uint,string>(EGameEvent.eGameEvent_RecvChatMsg,ShowTalk);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_RoomEnd, ShowRoomEnd);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SeatPosUpdate, RrefreshSeat);
            EventCenter.RemoveListener<EErrorCode>(EGameEvent.eGameEvent_AskBeginBattleError, BeginBattleError);
            EventCenter.RemoveListener<uint, string>(EGameEvent.eGameEvent_RecvChatMsg, ShowTalk);
        }

        //显示
        public override void OnEnable()
        {
            mRoomState = RoomState.InitState;
            mDefaultInput = mInputTalk.value;
            ShowSeat(-1, false, 0);
    
            InitEffect();
            RrefreshSeat();

         }

        //隐藏
        public override void OnDisable()
        {

        }

        enum GameRoomBtn
        {
            SEAT1 = 0,
            SEAT2,
            SEAT3,
            SEAT4,
            SEAT5,
            SEAT6,
            SEAT7,
            SEAT8,
            STARTBATTLE,
            LEAVEBATTLE,
        }

        public enum RoomState
        {
            InitState,
            StartFlashState,
            JoinSeatState,
            AllReadState,
            StartGameState,
        }

        //座位UI
        private List<UISprite> mHeadList = new List<UISprite>();
        private List<UILabel> mNameList = new List<UILabel>();
        private List<UILabel> mTipList = new List<UILabel>();
        private List<UILabel> mMoneyList = new List<UILabel>();
        private List<UISprite> mMoneyIconList = new List<UISprite>();
        private List<ButtonOnPress> mSeatBtnList = new List<ButtonOnPress>();

        private List<UISprite> mMasterList = new List<UISprite>();
        private List<UISprite> mReadyList = new List<UISprite>();
        string mLeftDefaultSprite;
        string mRightDefaultSprite;

        //房间Id
        UILabel mRoomNumLabel;

        //队伍名称
        private List<UILabel> mTeamNameLabelList = new List<UILabel>();

        //控制按钮
        UILabel mQuitGameLabel;
        ButtonOnPress mBtnStart;
        ButtonOnPress mBtnQuit;
        

        List<UISprite> mReadySprite = new List<UISprite>();
        List<UISprite> mStartSprite = new List<UISprite>();
        List<UISprite> mBgSprite = new List<UISprite>();
        UISprite mObStart = null;


        //聊天
        ButtonOnPress mBtnSend;
        public UIButton InvaitaionBtn
        {
            set;
            get;
        }
        UITextList mTalkShow;
        UIInput mInputTalk;
        UITextList mInputTalkList;

        //常量
        const int TeamCount = 2;
        const int SeatCount = 8;
        const int SeatTeam = 6;
        const string SeatTip = "点击换位";
        const string LookViewerTip = "点此处观战";
        const string mCancelReadyText = "取 消";
        string mDefaultInput = "";

        float talkBgAlpha = 0f;

       //效果
        GameObject[] mTteamObj = new GameObject[TeamCount];
        GameObject mMidObj;
        GameObject[] mSoulObj = new GameObject[SeatCount];
        GameObject mStartObj;
        GameObject mReadyObj;
        float mXOffset = 50f;
        float mMoveDuring = 1f;
        float mAlphaDuring = 1.7f;
        UITweener mTweenAlpha;
        GameObject mBallObj;
        UITweener mTweenSoulMove;
        float mTweenTime = 1f;
        Vector3 mSoulTargetPos = new Vector3(-10f, -75f, 0f);

        RoomState mRoomState = RoomState.InitState;


        void PressStartBtn(int ie, bool isPress)
        {
            if (!PlayerManager.Instance.LocalAccount.IsMaster())
                return;
            if (isPress)
            {
                mStartSprite.ElementAt(0).gameObject.SetActive(false);
                mStartSprite.ElementAt(1).gameObject.SetActive(true);
            }
            else
            {
                mStartSprite.ElementAt(0).gameObject.SetActive(true);
                mStartSprite.ElementAt(1).gameObject.SetActive(false);
            }
        }

        void CreateInvitation(GameObject go)
        {
            RoomCtrl.Instance.AskCanInviteFriends();
            
            //if (isPress)
            //    return;
            ////senlin
            //GameMethod.CreateWindow(GameConstDefine.LoadInvitationList, Vector3.zero, mRoot);
            //InvaitaionBtn.GetComponent<BoxCollider>().enabled = false;
        }
        void InitEffect()
        {
            mStartObj.SetActive(false);
            mReadyObj.SetActive(false);
            mTteamObj[0].transform.localPosition = new Vector3(mTteamObj[0].transform.localPosition.x - mXOffset, mTteamObj[0].transform.localPosition.y, mTteamObj[0].transform.localPosition.z);
            TweenPosition.Begin(mTteamObj[0], mMoveDuring, new Vector3(mTteamObj[0].transform.localPosition.x + mXOffset, mTteamObj[0].transform.localPosition.y, mTteamObj[0].transform.localPosition.z));
            mTteamObj[1].transform.localPosition = new Vector3(mTteamObj[1].transform.localPosition.x + mXOffset, mTteamObj[1].transform.localPosition.y, mTteamObj[1].transform.localPosition.z);
            TweenPosition.Begin(mTteamObj[1], mMoveDuring, new Vector3(mTteamObj[1].transform.localPosition.x - mXOffset, mTteamObj[1].transform.localPosition.y, mTteamObj[1].transform.localPosition.z));
            UICommon.UICommonMethod.TweenAlphaBegin(mMidObj, 0f, 0f);
            mTweenAlpha = UICommon.UICommonMethod.TweenAlphaBegin(mMidObj, mAlphaDuring, 1f);
            EventDelegate.Add(mTweenAlpha.onFinished, FinishMove, true);
            for (int i = 0; i < mSeatBtnList.Count; i++)
            {
                mSeatBtnList[i].AddListener(i, BtnEvent);
                if (mSeatBtnList.Count < SeatTeam)
                    mSoulObj[i].gameObject.SetActive(false);
            }
            mBtnStart.AddListener((int)GameRoomBtn.STARTBATTLE, BtnEvent);
            mBtnQuit.AddListener((int)GameRoomBtn.LEAVEBATTLE, BtnEvent);
            mBtnSend.AddListener(SendTalk);
            mRoomState = RoomState.StartFlashState;

            //UIGuide 
            IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideRoomBeginBtnStart, StartIGuideTask);
            IGuideTaskManager.Instance().SendTaskTrigger(EGameEvent.eGameEvent_UIGuideTriggerRoomBeginGame);

            IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideBackLobbyBtnStart, StartIGuideTask);
            IGuideTaskManager.Instance().SendTaskTrigger(EGameEvent.eGameEvent_UIGuideTriggerBackLobby);
        }

        void StartIGuideTask(EGameEvent taskId)
        {
            List<GameObject> objList = new List<GameObject>();
            switch (taskId)
            {
                case EGameEvent.eGameEvent_UIGuideRoomBeginBtnEnd:
                    objList.Add(mBtnStart.gameObject);
                    break;
                case EGameEvent.eGameEvent_UIGuideBackLobbyBtnEnd:
                    objList.Add(mBtnQuit.gameObject);
                    break;
            }

            IGuideTaskManager.Instance().SendTaskMarkObjList(taskId, objList);
        }

        void FinishMove()
        {

            //UIGuide
            IGuideTaskManager.Instance().SendTaskEffectShow(EGameEvent.eGameEvent_UIGuideRoomBeginBtnEnd);
            IGuideTaskManager.Instance().SendTaskEffectShow(EGameEvent.eGameEvent_UIGuideBackLobbyBtnEnd);

            // TweenAlpha.Begin(spriteTalkBg.cachedGameObject,0f,talkBgAlpha);
            mRoomState = RoomState.JoinSeatState;
            EventDelegate.Remove(mTweenAlpha.onFinished, FinishMove);
        }
  
        #region 显示队伍名字
        void ShowTeamName()
        {
            string team1 = GetRoomMapType();
            string team2 = GetRoomMapType();
            mTeamNameLabelList[0].text = team1;
            mTeamNameLabelList[1].text = team2;
        }

        string GetRoomMapType()
        {
            MapInfo info =MapLoadConfig.Instance.GetMapInfo(GameUserModel.Instance.GameMapID);
            if (info != null)
            {
                return info.mName;
            }
            
            return "";
        }

        #endregion

        #region 显示房间Id
        void ShowRoomId()
        {
            mRoomNumLabel.text = "房间号： " + GameUserModel.Instance.GameRoomID.ToString();
        }
        #endregion


        //Ob位并且不是房主的按钮显示
        void SetObTypeBtn()
        {
            mObStart.gameObject.SetActive(true);
            mStartSprite.ElementAt(0).transform.parent.gameObject.SetActive(false);
            mBgSprite.ElementAt(0).transform.parent.gameObject.SetActive(false);
            mBtnStart.GetComponent<BoxCollider>().enabled = false;
        }

        //非Ob位按钮共同设置
        void SetNoObBtnCommon()
        {
            mObStart.gameObject.SetActive(false);
            mBgSprite.ElementAt(0).transform.parent.gameObject.SetActive(true);
            mBtnStart.GetComponent<BoxCollider>().enabled = true;
        }

        //非ob位按钮房主设置
        void SetMasterBtn()
        {
            SetNoObBtnCommon();

            mStartSprite.ElementAt(0).transform.parent.gameObject.SetActive(true);
            mReadySprite.ElementAt(0).transform.parent.gameObject.SetActive(false);

            mStartSprite.ElementAt(0).gameObject.SetActive(true);
            mStartSprite.ElementAt(1).gameObject.SetActive(false);
            mBgSprite.ElementAt(0).gameObject.SetActive(true);
            mBgSprite.ElementAt(1).gameObject.SetActive(false);
            mBtnStart.GetComponent<ButtonSelectPic>().resetAfterPress = true;
        }

        //非ob位按钮非房主设置
        void SetNoMasterBtn()
        {
            SetNoObBtnCommon();
            mStartSprite.ElementAt(0).transform.parent.gameObject.SetActive(false);
            mReadySprite.ElementAt(0).transform.parent.gameObject.SetActive(true);
            mBtnStart.GetComponent<ButtonSelectPic>().resetAfterPress = false;


            if (PlayerManager.Instance.LocalAccount.IsReady())
            {
                mReadySprite.ElementAt(0).gameObject.SetActive(false);
                mReadySprite.ElementAt(1).gameObject.SetActive(true);
                mBgSprite.ElementAt(0).gameObject.SetActive(false);
                mBgSprite.ElementAt(1).gameObject.SetActive(true);
            }
            else
            {
                mReadySprite.ElementAt(0).gameObject.SetActive(true);
                mReadySprite.ElementAt(1).gameObject.SetActive(false);
                mBgSprite.ElementAt(0).gameObject.SetActive(true);
                mBgSprite.ElementAt(1).gameObject.SetActive(false);
            }
        }

        #region  控制按钮处理
        void ShowStartBtn()
        {
            Iplayer entiy;
            if (PlayerManager.Instance.AccountDic.TryGetValue(GameUserModel.Instance.GameUserGUID, out entiy))
            {
                Iplayer player = (Iplayer)entiy;

                if (player.IsMaster())
                {
                    SetMasterBtn();
                }
                else
                {
                    if (player.isGameLookViewer())
                    {
                        SetObTypeBtn();
                        return;
                    }
                    else
                    {
                        SetNoMasterBtn();
                    }
                }
            }
        }

        void BtnStart()
        {
            Iplayer entiy;
            PlayerManager.Instance.AccountDic.TryGetValue(GameUserModel.Instance.GameUserGUID, out entiy);
            Iplayer player = entiy;
            if (player.IsMaster())
            {
                RoomCtrl.Instance.BeginBattle();
                ShowMasterPressEffect();
            }
            else
            {
                if (!player.IsReady())
                {
                    RoomCtrl.Instance.ReadyRoom();
                }
                else
                {
                    RoomCtrl.Instance.CancelReadyRoom();
                }
            }
        }
        #endregion

        void BeginBattleError(EErrorCode m_n32ErrorId)
        {
            MsgInfoManager.Instance.ShowMsg((int)m_n32ErrorId);
        }
        #region 座位处理
        public void RrefreshSeat()
        {
            if (mRoomState == RoomState.AllReadState || mRoomState == RoomState.StartGameState)
                return;
            ShowSeat(-1, false, 0);
            foreach (var item in PlayerManager.Instance.AccountDic)
            {
                SetPlayerSeat((Iplayer)item.Value);
                //Debug.LogError(item.Value.GameUserSeat);
            }

            ShowRoomId();
            ShowStartBtn();
            ShowTeamName();
            ShowButtonEffect();
        }

        void ShowButtonEffect()
        {
            foreach (var item in PlayerManager.Instance.AccountDic)
            {
                Iplayer player = (Iplayer)item.Value;
                if (!player.IsReady() || (PlayerManager.Instance.AccountDic.Count == 1))
                {
                    mStartObj.SetActive(false);
                    return;
                }
            }
            if (PlayerManager.Instance.LocalAccount != null && PlayerManager.Instance.LocalAccount.IsMaster())
            {
                mStartObj.SetActive(true);
            }

        }

        void ShowMasterPressEffect()
        {
            foreach (var item in PlayerManager.Instance.AccountDic)
            {
                Iplayer player = (Iplayer)item.Value;
                if (!player.IsReady())
                {
                    return;
                }
            }
            mStartObj.SetActive(true);
            mReadyObj.SetActive(true);
        }

        void SetPlayerSeat(Iplayer playerU)
        {
            if (playerU.GameUserSeat <= 0)
                return;

            int seat = (int)playerU.GameUserSeat - 1;
            ShowSeat(seat, true, playerU.GameHeadId);
            ShowNickName(seat, playerU);
            ShowMasterIcon(seat, playerU);
            ShowGold(seat, playerU);
        }

        void ShowGold(int index, Iplayer player)
        {
            if (index > 5)
                return;
            mMoneyList.ElementAt(index).text = player.AccountGold.ToString();
        }

        void ShowMasterIcon(int index, Iplayer player)
        {
            if (player.isGameLookViewer())
            {
                if (player.IsMaster())
                    mMasterList[index].gameObject.SetActive(true);
                return;
            }
            if (player.IsMaster())
                mMasterList[index].gameObject.SetActive(true);
            else if (player.IsReady())
                mReadyList[index].gameObject.SetActive(true);
        }

        void ShowNickName(int index, Iplayer player)
        {
            mNameList[index].text = player.GameUserNick;

            if (player.isGameLookViewer())
            {
                mNameList[index].gameObject.SetActive(true);
                mTipList[index].gameObject.SetActive(false);
                return;
            }
        }

        void ShowSeat(int index, bool visiable, int head)
        {
            for (int i = 0; i < SeatCount; i++)
            {
                if (i < SeatTeam)
                {
                    if (!visiable)
                    {
                        mNameList[i].text = "";

                        mReadyList[i].gameObject.SetActive(visiable);
                        mMasterList[i].gameObject.SetActive(visiable);
                        if (i % 2 == 0)
                        {
                            mHeadList[i].spriteName = mRightDefaultSprite;
                        }
                        else
                        {
                            mHeadList[i].spriteName = mLeftDefaultSprite;
                        }
                    }
                    if (i != index && index != -1)
                        continue;
                    mMoneyList[i].gameObject.SetActive(visiable);
                    mMoneyIconList[i].gameObject.SetActive(visiable);
                    mTipList[i].gameObject.SetActive(!visiable);
                    if (visiable)
                    {
                        mTipList[i].text = SeatTip;
                        mHeadList[i].spriteName = head.ToString();
                    }
                }
                else
                {
                    if (!visiable)
                    {
                        //nameList[i].text = LookViewerTip;
                        mNameList[i].text = "";
                        mTipList[i].gameObject.SetActive(!visiable);
                        mMasterList[i].gameObject.SetActive(visiable);
                    }
                }
            }
        }

        #endregion

        void BtnEvent(int ie, bool pressed)
        {
            if (pressed)
            {
                return;
            }
            switch ((GameRoomBtn)ie)
            {
                case GameRoomBtn.SEAT1:
                case GameRoomBtn.SEAT2:
                case GameRoomBtn.SEAT3:
                case GameRoomBtn.SEAT4:
                case GameRoomBtn.SEAT5:
                case GameRoomBtn.SEAT6:
                    if (!PlayerManager.Instance.LocalAccount.IsReady() || PlayerManager.Instance.LocalAccount.IsMaster())
                        RoomCtrl.Instance.ChangeSeat(ie + 1);
                    break;
                case GameRoomBtn.SEAT7:
                case GameRoomBtn.SEAT8:
                        RoomCtrl.Instance.ChangeSeat(ie + 1);
                    break;
                case GameRoomBtn.STARTBATTLE:
                    BtnStart();
                    break;
                case GameRoomBtn.LEAVEBATTLE:
                    IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideBackLobbyBtnEnd);
                    if (!PlayerManager.Instance.LocalAccount.isGameLookViewer() && PlayerManager.Instance.LocalAccount.IsReady() && !PlayerManager.Instance.LocalAccount.IsMaster())
                    {
                        MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_CancelReady);
                    }
                    else
                    {

                        CGLCtrl_GameLogic.Instance.LeaveRoom();
                    }
                    break;
            }
        }


        void SendTalk(int index, bool isPress)
        {
            if (mInputTalk.label.text.CompareTo(mDefaultInput) == 0 || string.IsNullOrEmpty(mInputTalk.value))
            {
                MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_TalkSendNull);
                return;
            }

            RoomCtrl.Instance.SendTalkMessage(mInputTalk.value);

            mInputTalk.value = "";
            mInputTalkList.Clear();
          
        }

        public void ShowTalk(uint index, string msg)
        {
            foreach (var item in PlayerManager.Instance.AccountDic)
            {
                Iplayer player = (Iplayer)item.Value;
                if (index == player.GameUserSeat)
                {
                    string str = "[0bf4f5]" + player.GameUserNick + "[-]:" + msg;
                    mTalkShow.Add(str);
                    mTalkShow.scrollValue = 1f;
                    //                if (spriteTalkBg.fillAmount < 1f)
                    //                {
                    //                    int length = GetStringLines(str);
                    //                    for (int i = 0; i < length; i++)
                    //                    {
                    //                        if(spriteTalkBg.fillAmount < 1f)
                    //                            spriteTalkBg.fillAmount += 0.2f;
                    //                    }
                    //                }
                    break;
                }
            }
        }

        int GetStringLines(string str)
        {
            string final;
            int lines = 0;
            if (NGUIText.WrapText(str, out final))
            {
                string[] linesStr = final.Split('\n');
                lines += linesStr.Length;
            }
            return lines;
        }

        public void ShowRoomEnd()
        {
            mRoomState = RoomState.AllReadState;
            foreach (var item in PlayerManager.Instance.AccountDic.Values)
            {
                if (item.GameUserSeat <= SeatTeam)
                {
                    int index = (int)item.GameUserSeat - 1;
                    mSoulObj[index].gameObject.SetActive(true);
                    UITweener tween = TweenPosition.Begin(mSoulObj[index], mTweenTime, mBallObj.transform.position + mSoulTargetPos);
                    if (mTweenSoulMove == null)
                    {
                        mTweenSoulMove = tween;
                        EventDelegate.Add(mTweenSoulMove.onFinished, FinishSoulMove, true);
                    }
                }
                //else
                //{
                //    JxBlGame.Instance.BattleSelectHero();
                //}
            }
        }

        void FinishSoulMove()
        {
            mRoomState = RoomState.StartGameState;
            EventDelegate.Remove(mTweenSoulMove.onFinished, FinishSoulMove);

            EventCenter.SendEvent(new BlGame.CEvent(EGameEvent.eGameEvent_IntoHero));
        }


    }

}
