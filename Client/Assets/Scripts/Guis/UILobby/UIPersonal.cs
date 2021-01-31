using UnityEngine;
using BlGame.GameData;
using System;
using System.Collections;
using System.Collections.Generic;
public class UIPersonal : MonoBehaviour {

    public static UIPersonal Instance
    {
        private set;
        get;
    }

	UILabel GameInnsLabel;//游戏总局数
	UILabel WinInnsLabel;//游戏胜利局数
	UILabel KillNumLabel;//杀人总数
	UILabel BuildingDemoliLabel;//破坏建筑数
	UILabel DeadNumLabel;//死亡次数
	UILabel AchievNumLabel;//成就完成数
	UILabel LevelLabel;//等级

	UISlider ExpSlier;
	UISprite BarSprite;
	UILabel ExpLabel;//经验

	UISprite HeadIcon;
	UILabel VipLabel;
	UILabel NameLabel; 

    enum ShowState { 
        NullState,
        InitState,
        HeadInState,
        InfoInState,
        NormalState,
    }

    ShowState showState = ShowState.NullState;

    UITweener tweenHeadIn;
    UITweener tweenInfoIn;
    private const float xMove = 50f;
    private const float moveDuring = 0.4f;
    private const float infoDuring = 0.5f;
    private float infoDelay = 0.05f;
    private float infoIncreaset = 0.05f;
    private float infoMoveX = -1000f;
    private float infoMoveInX = 10f;

    List<Transform> infoList = new List<Transform>();

    public void SetCurrentDate()
    {
        NameLabel.text = PersonalInfoData.Instance().gameNick;
        GameInnsLabel.text = PersonalInfoData.Instance().gameCount;
        LevelLabel.text = PersonalInfoData.Instance().gameLevel;
        WinInnsLabel.text = PersonalInfoData.Instance().gameWinCount;
        KillNumLabel.text = PersonalInfoData.Instance().gameKillCount;
        BuildingDemoliLabel.text = PersonalInfoData.Instance().gameBuildingDestroyCount;
        DeadNumLabel.text = PersonalInfoData.Instance().gameDeadCount;
        AchievNumLabel.text = PersonalInfoData.Instance().gameAchieveCount + "/" + PersonalInfoData.Instance().gameMaxAchieve;
        int Exp = Convert.ToInt32(PersonalInfoData.Instance().gameExp);
        int MaxExp = Convert.ToInt32(PersonalInfoData.Instance().gameMaxExp);
        float val = (float)Exp / (float)MaxExp;
        ExpSlier.value = val;
        ExpLabel.text = Exp + "/" + MaxExp;
        HeadIcon.spriteName = PersonalInfoData.Instance().headId.ToString();
    }

	void Init(){
		GameInnsLabel = transform.FindChild("Items/Item1/Number").GetComponent<UILabel>();
        WinInnsLabel = transform.FindChild("Items/Item2/Number").GetComponent<UILabel>();
        KillNumLabel = transform.FindChild("Items/Item3/Number").GetComponent<UILabel>();
        BuildingDemoliLabel = transform.Find("Items/Item4/Number").GetComponent<UILabel>();
        DeadNumLabel = transform.FindChild("Items/Item5/Number").GetComponent<UILabel>();
        AchievNumLabel = transform.FindChild("Items/Item6/Number").GetComponent<UILabel>();
        LevelLabel = transform.FindChild("player/Level/Number").GetComponent<UILabel>();
        ExpSlier = transform.FindChild("player/EXP").GetComponent<UISlider>();
		ExpLabel = ExpSlier.transform.FindChild("Number").GetComponent<UILabel>();
		BarSprite = ExpSlier.transform.FindChild("Bar").GetComponent<UISprite>();
        HeadIcon = transform.FindChild("player/HeadPhoto/Portrait").GetComponent<UISprite>();
        VipLabel = transform.FindChild("player/VIP/TimeRemaining").GetComponent<UILabel>();
        NameLabel = transform.FindChild("player/Name/NickName").GetComponent<UILabel>();
        Transform itemParent = transform.FindChild("Items");
        for (int i = 0; i < itemParent.childCount; i++) {
            Transform add = itemParent.FindChild("Item"+(i+1).ToString());
            infoList.Add(add);
        }
	}

    void Awake() {
        Init();
    }


    void OnEnable() {
        Instance = this;
      
        DoInfoInInit();
        //TweenPosition.Begin(gameObject, 0.5f, new Vector3(-46, 0, 0));
        SetShowState(ShowState.HeadInState);
        EventCenter.AddListener(EGameEvent.eGameEvent_PersonInitInfo, SetCurrentDate);
    }
    void OnDisable()
    {
        Instance = null;
        EventCenter.RemoveListener(EGameEvent.eGameEvent_PersonInitInfo, SetCurrentDate);
    }

    void SetShowState(ShowState state) {
        if (showState == state) {
            return;
        }
        showState = state;
        switch (showState)
        {
            case ShowState.InitState:
                
                break;
            case ShowState.HeadInState: 
                DoHeadIn();
                break;
            case ShowState.InfoInState:
                DoInfoIn();
                break;
        }
    }

    void DoHeadIn() { 
        GameObject obj = transform.FindChild("player").gameObject;
        TweenPosition.Begin(obj,0f,new Vector3(-xMove,0f,0f));
        tweenHeadIn = TweenPosition.Begin(obj, moveDuring, new Vector3(xMove, 0f, 0f));
        tweenHeadIn.method = UITweener.Method.EaseIn;
        tweenHeadIn.style = UITweener.Style.Once;
        EventDelegate.Add(tweenHeadIn.onFinished, OnFinishHeadIn);        
    }


    void OnFinishHeadIn() {
        SetShowState(ShowState.InfoInState);
        EventDelegate.Remove(tweenHeadIn.onFinished, OnFinishHeadIn);
    }

    void DoInfoInInit()
    {
        for (int i = 0; i < infoList.Count; i++)
        {
            UITweener tween = TweenPosition.Begin(infoList[i].gameObject, 0f, new Vector3(-infoMoveX, infoList[i].transform.localPosition.y, infoList[i].transform.localPosition.z));
        }
    }

    void DoInfoIn() {
        for (int i = 0; i < infoList.Count; i++) {
            UITweener tween = TweenPosition.Begin(infoList[i].gameObject, infoDuring, new Vector3(infoMoveInX, infoList[i].transform.localPosition.y, infoList[i].transform.localPosition.z));
            tween.method = UITweener.Method.Linear;
            tween.style = UITweener.Style.Once;
            infoDelay = infoDelay + infoIncreaset;
            tween.delay = infoDelay;
            if (i == 9) {
                tweenInfoIn = tween;
                EventDelegate.Add(tweenInfoIn.onFinished, OnFinishInfoIn);        
            }
        }
    }

    void OnFinishInfoIn() {
        SetShowState(ShowState.NormalState);
        EventDelegate.Remove(tweenInfoIn.onFinished, OnFinishInfoIn);
    }
}
