using UnityEngine;
using System.Collections;
using GameDefine;
using JT.FWW.GameData;
using UICommon;
using BlGame.GuideDate;
using System.Collections.Generic;
public class UIStartSelfDefine : MonoBehaviour {

	UIToggle BtnCreateMenu;//创建房间菜单
	UIToggle BtnAddRoomMenu;//加入房间菜单
    ButtonSelectPic BtnCreateRoom;//创建房间
    ButtonSelectPic BtnBack;//返回
	UILabel RoomNumberLabel;//房间号
	UILabel RoomPassWordLabel;//房间密码
	ButtonOnPress []SpriteEasyOrHigh = new ButtonOnPress[3];//简单高级
	Transform RoomTran;
	UIInput InputPassWord;
	UIInput InputRoomNum;
    GameObject FrameTips = null;
	enum STATE_BTN{
		BTN_EASY = 0,
		BTN_MIDDLE,
		BTN_HIGH,
		BTN_CREATE_ROOM,
		BTN_BACK,

	}
    const int TypeStart = 1001;
	string[] Coord = {"Easy","Middle","High"};
	MAPTYPE TypeMap;
	string PassWord = "";
	string AddRoom = "加入房间";
	string CreateRoom = "创建房间";
    Transform posParent;


    void Init()
	{
        posParent = transform.FindChild("Postion");
		TypeMap = MAPTYPE.MIDDLEMAP;
        RoomTran = posParent.FindChild("Mapselect");
		//获取三张图。监听         
		for (int i = 0; i < RoomTran.childCount; i ++) {
			SpriteEasyOrHigh[i] = RoomTran.Find (Coord[i]).GetComponent<ButtonOnPress>();		 			
		}
		//获取菜单按钮。监听
        BtnCreateMenu = posParent.Find("ModeSelect/Create").GetComponent<UIToggle>();
        BtnAddRoomMenu = posParent.Find("ModeSelect/Join").GetComponent<UIToggle>();

		//获取创建和返回按钮。监听
        BtnCreateRoom = posParent.Find("CreateButton").GetComponent<ButtonSelectPic>();
        BtnBack = posParent.Find("BackButton").GetComponent<ButtonSelectPic>();
		//获取房间号和房间密码
        RoomNumberLabel = posParent.Find("RoomID/Label").GetComponent<UILabel>();
		InputRoomNum = RoomNumberLabel.transform.parent.GetComponent<UIInput>(); 
		
        InputPassWord = posParent.Find("Password").GetComponent<UIInput>();
		RoomPassWordLabel = InputPassWord.transform.Find ("Label").GetComponent<UILabel>();
	}


    void Awake() {
        Init();
    }

    void OnEnable()
    {
        EnableBoxEvent(true);
        AddEvent();
        DoMoveIn();
        //UIGuide
        IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelectPrimaryBtnStart, StartIGuideTask);
        IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideCreateRoomBtnStart, StartIGuideTask);
        IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideTriggerCreateRoom);
    }


    void StartIGuideTask(EGameEvent taskId)
    {
        //List<BoxCollider> box = new List<BoxCollider>();
        List<GameObject> objList = new List<GameObject>();
        switch (taskId)
        {
            case EGameEvent.eGameEvent_UIGuideSelectCreateRoomBtnEnd:
                IGuideTaskManager.Instance().SendTaskEffectShow(EGameEvent.eGameEvent_UIGuideSelectCreateRoomBtnEnd);
                objList.Add(BtnCreateRoom.gameObject);
                break;
            case EGameEvent.eGameEvent_UIGuideSelectPrimaryBtnEnd:
                objList.Add(SpriteEasyOrHigh[1].gameObject);
                break;
        }

        IGuideTaskManager.Instance().SendTaskMarkObjList(taskId, objList);
    }


    UITweener tweenMoveIn = null;
    float moveDurning = 0.8f;
    float moveRightX =  1000f;
    float moveLeftX = -70f;
    float moveBackDurning = 0.6f;
    float moveBackX = 5f;

    void DoMoveIn() {
        MoveMethod(null, moveRightX,0f);
        MoveMethod(MoveInFinish, moveLeftX, moveDurning);               
    }


    void MoveMethod(EventDelegate.Callback callback, float xMove, float durning, UITweener.Method method = UITweener.Method.EaseIn, UITweener.Style style = UITweener.Style.Once)
    {
        tweenMoveIn = TweenPosition.Begin(posParent.gameObject, durning, new Vector3(xMove, 0f, 0f));
        tweenMoveIn.style = style;
        tweenMoveIn.method = method;
        if (callback != null)
            EventDelegate.Add(tweenMoveIn.onFinished, callback,true);
    }

    void MoveInFinish() {
        EventDelegate.Remove(tweenMoveIn.onFinished, MoveInFinish);
        MoveMethod(MoveBackFinish,moveBackX,moveBackDurning,UITweener.Method.EaseOut);
    }

    void MoveBackFinish() { 
		IGuideTaskManager.Instance().SendTaskEffectShow(EGameEvent.eGameEvent_UIGuideSelectPrimaryBtnEnd);
       // EnableBoxEvent(true);
    }

    void EnableBoxEvent(bool enable) {
        //for (int i = 0; i < SpriteEasyOrHigh.Length; i++)
        for (int i = 0; i < SpriteEasyOrHigh.Length -1; i++)
        {
            SpriteEasyOrHigh[i].GetComponent<BoxCollider>().enabled = enable;
        }
        InputPassWord.GetComponent<BoxCollider>().enabled = enable;
        BtnCreateRoom.GetComponent<BoxCollider>().enabled = enable;
        BtnBack.GetComponent<BoxCollider>().enabled = enable;
        BtnCreateMenu.GetComponent<BoxCollider>().enabled = enable;
        BtnAddRoomMenu.GetComponent<BoxCollider>().enabled = enable;    
    }

    void AddEvent() {
        for (int i = 0; i < SpriteEasyOrHigh.Length; i++)
        { 
            SpriteEasyOrHigh[i].AddListener(i, StartBtnOnPress); 
        }
        BtnCreateRoom.AddListener((int)STATE_BTN.BTN_CREATE_ROOM, StartBtnOnPress);
        BtnBack.AddListener((int)STATE_BTN.BTN_BACK, StartBtnOnPress);
        EventCenter.AddListener<EErrorCode>(EGameEvent.eGameEvent_AskAddInBattle, StartSelfResult);
        EventDelegate.Add(BtnCreateMenu.onChange, BtnCreateOrAdd);
        EventDelegate.Add(BtnAddRoomMenu.onChange, BtnCreateOrAdd);
    }

	void OnDisable(){
        for (int i = 0; i < SpriteEasyOrHigh.Length; i++)
        {
            SpriteEasyOrHigh[i].RemoveListener(StartBtnOnPress);
            
        }
		BtnBack.RemoveListener (StartBtnOnPress);
		EventDelegate.Remove (BtnCreateMenu.onChange, BtnCreateOrAdd);
		EventDelegate.Remove (BtnAddRoomMenu.onChange, BtnCreateOrAdd);

		EventCenter.RemoveListener<EErrorCode>(EGameEvent.eGameEvent_AskAddInBattle, StartSelfResult);
		IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelectPrimaryBtnStart, StartIGuideTask);
        IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEvent_UIGuideCreateRoomBtnStart, StartIGuideTask);
	}
	void BtnCreateOrAdd()
	{
		UIToggle toggle =  UIToggle.current;
		if (toggle != null && toggle.value) {
			if(BtnCreateMenu == toggle)
			{
                InputRoomNum.label.text = "-------";
				InputPassWord.label.text = PassWord;
				InputPassWord.value = PassWord;
				RoomNumberLabel.transform.parent.GetComponent<UIInput>().enabled = false;
				RoomNumberLabel.transform.parent.GetComponent<BoxCollider> ().enabled = false;
				BtnCreateRoom.GetComponentInChildren<UILabel>().text = CreateRoom;
			}else
			{
				InputRoomNum.label.text = PassWord;
				InputRoomNum.value = PassWord;
				InputPassWord.label.text = PassWord;
				InputPassWord.value = PassWord;
				RoomNumberLabel.transform.parent.GetComponent<UIInput>().enabled = true;
				RoomNumberLabel.transform.parent.GetComponent<BoxCollider> ().enabled = true;
				BtnCreateRoom.GetComponentInChildren<UILabel>().text = AddRoom;
			}
		}
	}
    UIFrameTips Tips = null;
	void StartSelfResult(EErrorCode m_n32ErrorId)
	{
        switch (m_n32ErrorId)
        {
            case EErrorCode.eEC_AddBattleFailForAllFull://消
                MsgInfoManager.Instance.ShowMsg((int)m_n32ErrorId);
                if (FrameTips != null)
                {
                    DestroyImmediate(FrameTips);
                    FrameTips = null;
                }
                break;
            case EErrorCode.eEC_AddBattleFailForUserFull://弹
                
                if (FrameTips == null)
                {
                    FrameTips = GameDefine.GameMethod.CreateWindow(GameConstDefine.FrameTips, Vector3.zero, transform);
                   Tips = FrameTips.AddComponent<UIFrameTips>();
                }
                if (FrameTips != null)
                    Tips.SetSendMsg(RoomNumberLabel.text, RoomPassWordLabel.text, lenth);
                break;
            default:
                MsgInfoManager.Instance.ShowMsg((int)m_n32ErrorId);
                break;
        }
        
        //MsgConfigInfo msg = ConfigReader.GetMsgInfo((int)m_n32ErrorId);
        //if (msg != null)
        //    MsgInfoManager.Instance.ShowMsg((MsgInfoManager.MsgType)msg.msgType, (int)m_n32ErrorId);
        //switch (m_n32ErrorId) {
        //case EErrorCode.eEC_TheBattleUserFull:
        //    SetTipsName("人满了");
        //    break;
        //case EErrorCode.eEC_NullBattle:
        //    SetTipsName("没有这个房间");
        //    break;
        //case EErrorCode.eEC_BattlePDWNotMatch:
        //    SetTipsName("密码不对");
        //    break;
        //case EErrorCode.eEC_JustInBattle:
        //    SetTipsName("JustInBattle");
        //    break;
        //case EErrorCode.eEC_NullUser:
        //    SetTipsName("NullUser");
        //    break;
        //case  EErrorCode.eEC_InvalidBattleState:
        //    SetTipsName("无效的房间状态"); 
        //    break;
        //case EErrorCode.eEC_InvalidBattlePos:
        //    SetTipsName("InvalidBattlePos");
        //    break;
        //case EErrorCode.eEC_InvalidMapID:
        //    SetTipsName("InvalidMapID");
        //    break;
        //}
	}

	void SetTipsName(string name)
	{
		//MetaMsg.ShowBoxTips(name,2, 0.7f);
	}

	
//	private void BtnMenu(string currBtn,int ie,bool isvel)
//	{
//		InputRoomNum.onValidate += OnInputChanged;
//		RoomNumberLabel.text = InputRoomNum.value;
//		if (isvel == true)
//			RoomNumberLabel.text = RoomNum;
//		else
//			RoomNumberLabel.text = str;
//		RoomNumberLabel.transform.parent.GetComponent<UIInput>().enabled = isvel;
//		RoomNumberLabel.transform.parent.GetComponent<BoxCollider> ().enabled = isvel;
//		BtnCreateRoom.GetComponentInChildren<UILabel>().text = currBtn;
//		for(int i = 0; i < PaneSprite.Length;i++)
//			if(TypeMap == MAPTYPE.EASYMAP && i > (int)MAPTYPE.EASYMAP)
//				PaneSprite[i].gameObject.SetActive(i == ie);
//	}
	private void StartBtnOnPress(int ie, bool press)
	{

		if (press)
			return;
		switch((STATE_BTN)ie)
		{
		case STATE_BTN.BTN_CREATE_ROOM:     
			CreateOrAddRoom();
			break;
		case STATE_BTN.BTN_BACK:
            //senlin
            //UILobby.Instance.OnCancel();
			break;
		case STATE_BTN.BTN_EASY:
		case STATE_BTN.BTN_HIGH:
		case STATE_BTN.BTN_MIDDLE:
            TypeMap = (MAPTYPE)(ie + TypeStart);
			 
			break;
		}
        if ((STATE_BTN)ie == STATE_BTN.BTN_MIDDLE)
        {
            IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideSelectPrimaryBtnEnd);
        }
	}
	public void CreateOrAddRoom(){
		if(string.Equals(BtnCreateRoom.GetComponentInChildren<UILabel>().text,CreateRoom))
		{
            IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideSelectCreateRoomBtnEnd);
			if(string.IsNullOrEmpty(RoomPassWordLabel.text))
				CGLCtrl_GameLogic.Instance.CreateRoom((int)TypeMap,"");
			else
				CGLCtrl_GameLogic.Instance.CreateRoom((int)TypeMap,RoomPassWordLabel.text);
		}else{
            if (string.IsNullOrEmpty(InputRoomNum.label.text) || string.Equals(InputRoomNum.label.text, "-------"))
            {
                MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_NullRoomID);
                //MsgConfigInfo msg = ConfigReader.GetMsgInfo((int)ERROR_TYPE.eT_NullRoomID);
                //if (msg != null)
                //    MsgInfoManager.Instance.ShowMsg((MsgInfoManager.MsgType)msg.msgType, (int)ERROR_TYPE.eT_NullRoomID);
            }
            else
            {
                //				Debug.LogError( string.IsNullOrEmpty(InputRoomNum.label.text) + " " + );
                CGLCtrl_GameLogic.Instance.AddRoom(RoomNumberLabel.text, RoomPassWordLabel.text);
            }
		}

	}
	const int lenth =6;     
}
