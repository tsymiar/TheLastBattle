using UnityEngine;
using System.Collections;
using GameDefine;
using UICommon;
using BlGame.GuideDate;
using System.Collections.Generic;
public class UIPvpOrGuideBase : MonoBehaviour
{
	// Use this for initialization
	ButtonOnPress CustomButton;// 自定义
	Transform BtnModeSelect;
	Transform BtnGuide;
	UILabel[] MoneyLabel;//金币Label
	UISprite[] SpriteBg;
    
    ButtonOnPress FriendButton;
	UILabel CountDownTime; // 倒计时
    UILabel MatchNumber;   //匹配数
	ButtonOnPress BtnCancel;

	UISlider GradualChange;
	UISprite ChangeDate;
	UISprite MatteBg;
	private GameObject EffectLine;
	private float startTime = 0;  
	
	const float second = 15 ;

	//public bool isKeyPress = false;///是否可以按
	enum BUTTON_STATE
	{
		BTNCUSTOM,//自定义
		BTNMODE_EASY,//初级
		BTNMODE_HIGH,//高级
		BTNMODE_MIDDLE,//中级
		BTN_GUIDE_ONE,//新手训练
		BTN_GUIDE_TWO,//xin shou 2
		BTN_GUIDE_THREE,//xin shou 3
		BTN_CANCEL,//取消
        BTN_FRIENDLIST
	}
	private float[] duringTime = {0.35f,0.55f,0.75f};
	string[] modeName = {"Easy","High","Middle"};
	MAPTYPE type;
	bool isDownTime = false;
	ButtonOnPress[] BtnMove;
	float fectorTime =  2.3f;

	public static UIPvpOrGuideBase Instance{
		set;
		get;
	}

    void Awake() {
        Instance = this;
    }

	void init()
	{
		//isKeyPress = false;
		fectorTime = 1.5f;
		CustomButton = this.transform.Find ("CustomButton").GetComponent<ButtonOnPress>();
		CustomButton.AddListener ( (int)BUTTON_STATE.BTNCUSTOM , PvpBtnOnPress);
		CustomButton.transform.localScale = new Vector3(0.8f,0.8f,0.8f);
//		TweenColor.Begin (CustomButton.gameObject,0f,Color.black);
//		TweenColor.Begin (CustomButton.gameObject,1f,Color.white);
//		UICommonMethod.TweenAlphaBegin (CustomButton.gameObject,0.2f,0,UITweener.Style.Once);
//		MoveSprite scaleAlpha = new MoveSprite ();
		//scaleAlpha.CustonScaleAlpha (CustomButton.gameObject,0.5f,1f,0.6f);
		BtnModeSelect = this.transform.Find ("ModeSelect");
		MoneyLabel = new UILabel[BtnModeSelect.childCount];
		BtnMove = new ButtonOnPress[BtnModeSelect.childCount]; 
		SpriteBg = new UISprite[BtnModeSelect.childCount];
		for (int i = 0; i < BtnModeSelect.childCount; i ++) {
			BtnMove[i] = BtnModeSelect.FindChild(modeName[i]).GetComponent<ButtonOnPress>();
			BtnMove[i].AddListener ( i + 1, PvpBtnOnPress);
			MoneyLabel[i] = BtnMove[i].transform.Find("bBar/num").GetComponent<UILabel>();
			MoneyLabel[i].text = "55";
			SpriteBg[i] = BtnMove[i].transform.Find("pane").GetComponent<UISprite>();
			SpriteBg[i].gameObject.SetActive(false);
		}
		ButtonOnPress tep = BtnMove [2];
		BtnMove [2] = BtnMove [1] ;
		BtnMove [1] = tep;
		int index = -1210;//X偏移值
		float time = 0.08f;
		for (int i = 0; i < BtnModeSelect.childCount; i++,index += Interval) {
			MoveSprite ms = new MoveSprite();
			ms.SetMoveState(time,BtnMove[i].gameObject,duringTime[i],new Vector3(index - 20 + (i*10) ,0,0));
		}
        //FriendButton = this.transform.FindChild("Training/FriendsListBtn").GetComponent<ButtonOnPress>();
		MatteBg = this.transform.Find("Matte/Sprite").GetComponent<UISprite>();
		EffectLine = this.transform.Find ("pvp_line/flash").gameObject;
        GradualChange = this.transform.Find("Training").GetComponent<UISlider>();
        ChangeDate = GradualChange.transform.Find("BottomBar").GetComponent<UISprite>();
		BtnGuide = this.transform.Find ("Training/Button");
		for(int i = 0; i < BtnGuide.childCount; i ++)
		{
			ButtonOnPress btn = BtnGuide.FindChild("Guide" + (i + 1)).GetComponent<ButtonOnPress>();
			btn.AddListener(i + 4,PvpBtnOnPress);
		}
		BtnCancel = this.transform.Find("Time/button").GetComponent<ButtonOnPress>();
		CountDownTime = this.transform.Find ("Time/Time").GetComponent<UILabel>();
        MatchNumber = this.transform.Find("Time/Num").GetComponent<UILabel>();
        MatchNumber.text = "(1/6)";
		BtnCancel.AddListener ((int)BUTTON_STATE.BTN_CANCEL, PvpBtnOnPress);
		BtnCancel.transform.parent.gameObject.SetActive (false);
        //FriendButton.AddListener((int)BUTTON_STATE.BTN_FRIENDLIST, PvpBtnOnPress);
	}

	void OnEnable()
	{
		init ();
		GradualChange.value = 0;
		y = 270f;
        isDownTime = false;
        MoveSprite.count = 0;
        //UIGuide
        IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideNewsGuideStart, StartIGuideTask);
        IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelfDefBtnStart, StartIGuideTask);
        IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideMatchBtnStart, StartIGuideTask); 
        IGuideTaskManager.Instance().SendTaskTrigger(EGameEvent.eGameEvent_UIGuideTriggerNewsGuide);
        IGuideTaskManager.Instance().SendTaskTrigger(EGameEvent.eGameEvent_UIGuideTriggerSelfDefGame);
        IGuideTaskManager.Instance().SendTaskTrigger(EGameEvent.eGameEvent_UIGuideTriggerMatchGame); 
        TweenScale.Begin(CustomButton.gameObject, 0f, tweenScaleBtnVector);
        CustomButton.gameObject.SetActive(false);

	}
    void StartIGuideTask(EGameEvent taskId)
    {
        //List<BoxCollider> box = new List<BoxCollider>();
        List<GameObject> objList = new List<GameObject>();
        switch (taskId)
        {
            case EGameEvent.eGameEvent_UIGuideNewsGuideEnd:
                objList.Add(BtnGuide.FindChild("Guide1").gameObject);
                break;
            case EGameEvent.eGameEvent_UIGuideSelfDefBtnEnd:
                objList.Add(CustomButton.gameObject);
                break; 
            case EGameEvent.eGameEvent_UIGuideMatchBtnEnd:
                objList.Add(BtnModeSelect.FindChild(modeName[0]).gameObject);
                break;
        }


        IGuideTaskManager.Instance().SendTaskMarkObjList(taskId, objList);
    }


	private const int Interval = 310;//间距
	private const float speed = 0.023f; 
	static float y = 270f; 
	void Start () {
	}
	// Update is called once per frame
	void Update () {
		if (isDownTime) {
			startTime += Time.deltaTime;
			ShowCount(CountDownTime,(int)startTime);
		}
		if (ChangeDate.fillAmount < 1 && ChangeDate.fillAmount >= 0) {
			y = y + (speed * 100);
			ChangeDate.fillAmount += speed;
			MatteBg.transform.localPosition = new Vector3(MatteBg.transform.position.x - 1000,-y,MatteBg.transform.position.z);
		}
		fectorTime -= Time.deltaTime;
        //if (fectorTime < 0) {
        //    isKeyPress = true;	
        //}
	}
	public bool SetMoneyToEnter(int money,int allMoney)
	{
		//if(allMoney > money)用户所有金币 > 当前要进入的副本金币，如果大才能进，否则进不去。
		if (money > allMoney)
			return false;
		allMoney -= money;
		return true;
	}
	//判断FB要求等级是否大于玩家等级。如果是就FALSE。如果否就进。
	public bool SetLevelToEnTer(int currLevel,int levelHigh)
	{
		if (currLevel >= levelHigh)
			return true;
		return false;
	}

	void OnDisable()
	{
		IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEvent_UIGuideNewsGuideStart, StartIGuideTask);
        IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelfDefBtnStart, StartIGuideTask);
        IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEvent_UIGuideMatchBtnStart, StartIGuideTask); 
		if (CustomButton != null)
			CustomButton.RemoveListener (PvpBtnOnPress);
		if(BtnModeSelect != null)
			for (int i = 0; i < BtnModeSelect.childCount; i ++) {
				ButtonOnPress btn = BtnModeSelect.FindChild(modeName[i]).GetComponent<ButtonOnPress>();
				btn.RemoveListener (PvpBtnOnPress);
			}
		if(BtnGuide != null)
			for(int i = 0; i < BtnGuide.childCount; i ++)
			{
				ButtonOnPress btn = BtnGuide.FindChild("Guide" + (i + 1)).GetComponent<ButtonOnPress>();
				btn.RemoveListener(PvpBtnOnPress);
			}
		if (BtnCancel != null)
			BtnCancel.RemoveListener (PvpBtnOnPress);
        //FriendButton.RemoveListener(PvpBtnOnPress);

	}

    private void ChangeMatchNumber(int num)
    {
        if (num <= 0 || num > 6)
            return;

        MatchNumber.text = "(" + num + "/6)";
    }

	private void PvpBtnOnPress(int ie, bool press)
	{
		//if (isKeyPress) {
			if (press)
					return; 

			switch ((BUTTON_STATE)ie) {
			 case BUTTON_STATE.BTNCUSTOM:
                IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideSelfDefBtnEnd);

                //senlin
                //UILobby.Instance.SetLobbyPvpButtom(LobbyPvpButtom.CUSTOM);
                break;
            case BUTTON_STATE.BTNMODE_EASY:
                SetTimeLabel(MAPTYPE.MIDDLEMAP, ie);//中级场。
                break;
			case BUTTON_STATE.BTNMODE_HIGH:	
					SetTimeLabel (MAPTYPE.HIGHMAP, ie);
					break;
			case BUTTON_STATE.BTNMODE_MIDDLE:
					SetTimeLabel (MAPTYPE.MIDDLEMAP, ie);
					break;
			case BUTTON_STATE.BTN_GUIDE_ONE: 
                    //CGLCtrl_GameLogic.Instance.AskEnterNewsGuideBattle((int)MAPTYPE.NEWS_GUIDE_1);
					break;
			case BUTTON_STATE.BTN_GUIDE_TWO:

					break;
			case BUTTON_STATE.BTN_GUIDE_THREE:

					break;
			case BUTTON_STATE.BTN_CANCEL:
					BtnCancel.transform.parent.gameObject.SetActive (false);
					isDownTime = false;
					CGLCtrl_GameLogic.Instance.LeaveRoom();

                    GCToCS.AskStopQuickBattle pMsg = new GCToCS.AskStopQuickBattle();
					BlGame.Network.NetworkManager.Instance.SendMsg(pMsg, (int)pMsg.msgnum);
					break;
                case BUTTON_STATE.BTN_FRIENDLIST:
                    break;
			}
	//	}
	}

	private void ShowCount(UILabel label ,int time)
	{
		string timeString = "";
        if (time < 60)
            timeString = "00:" + SecondToStr(time);
		else
            timeString  = SecondToStr(time / 60) + ":" + SecondToStr(time % 60);
		
		label.text = timeString;
	}

    private string SecondToStr(int time)
    {
        string timeString = "";
		if(time < 10)
			timeString = "0"+time.ToString();
		else 
			timeString = ""+time.ToString();

        return timeString;
    }

	void SetTimeLabel(MAPTYPE _type,int ie)
	{
		BtnCancel.transform.parent.gameObject.SetActive (true);
		for(int i = 0; i < SpriteBg.Length;i++)
			SpriteBg[i].gameObject.SetActive((i+1) == ie);
		type = _type;
		isDownTime = true;
		if(CountDownTime.gameObject.activeSelf == true)
		{
			startTime = 0;
			ShowCount(CountDownTime ,(int) startTime);
		}
        if (type == MAPTYPE.MIDDLEMAP)
        {
            IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideMatchBtnEnd);            
        }
        CGLCtrl_GameLogic.Instance.QuickBattle((int)type);
	}

    UITweener tweenBtnScale;
    float duringBtn = 0.25f;
    Vector3 tweenScaleBtnVector = new Vector3(1.2f, 1.2f, 1f);//倒计时缩放
    public void DoSelfBtnMoveIn() {
        CustomButton.gameObject.SetActive(true);
        tweenBtnScale = TweenScale.Begin(CustomButton.gameObject, duringBtn, Vector3.one);
        tweenBtnScale.method = UITweener.Method.EaseIn;
        tweenBtnScale.style = UITweener.Style.Once;        
        EventDelegate.Add(tweenBtnScale.onFinished,SelfBtnMoveInEnd);
    }


    void SelfBtnMoveInEnd()
    {
        IGuideTaskManager.Instance().SendTaskEffectShow(EGameEvent.eGameEvent_UIGuideNewsGuideEnd);
        IGuideTaskManager.Instance().SendTaskEffectShow(EGameEvent.eGameEvent_UIGuideSelfDefBtnEnd);
        IGuideTaskManager.Instance().SendTaskEffectShow(EGameEvent.eGameEvent_UIGuideMatchBtnEnd); 
        EventDelegate.Remove(tweenBtnScale.onFinished, SelfBtnMoveInEnd);
    }
}
