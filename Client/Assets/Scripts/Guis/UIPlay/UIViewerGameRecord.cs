using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using System;
using GameDefine;
public class UIViewerGameRecord : MonoBehaviour {

    public static UIViewerGameRecord Instance
    {
        private set;
        get;
    }
    private UILabel AllRedKills;
    private UILabel AllBuleKills;
    private UILabel TimeLabel;
    private UITweener uit;   
    float deltaTime = 0;

    public ButtonOnPress BtnOnPress
    {
        set;
        get;
    }

    private UISprite greenHp;
    private UISprite redHp;

    void Awake()
    {
        Instance = this;
        Init();
    }
    void Init()
    {
        BtnOnPress = transform.FindChild("Arrow").GetComponent<ButtonOnPress>();
        BtnOnPress.AddListener(0, OnButtonPress);
        AllRedKills = this.transform.FindChild("Panel/Kill/Spirit/KillLabel").GetComponent<UILabel>();
        AllBuleKills = this.transform.FindChild("Panel/Kill/Undeath/KillLabel").GetComponent<UILabel>();
        TimeLabel = this.transform.FindChild("Panel/Time/TimeLabel").GetComponent<UILabel>();
        AllRedKills.text = "0";
        AllBuleKills.text = "0";
        greenHp = transform.FindChild("Panel/zcxt_s/Sprite").GetComponent<UISprite>();
        redHp = transform.FindChild("Panel/zcxt_d/Sprite").GetComponent<UISprite>();
    } 

    void OnButtonPress(int ie , bool isPress)
    {
        if (isPress)
            return;
        //UIGameBattleInfo.Instance.SetOnPress(true);
        UIViewerBattleInfo.Instance.BtnOnPress.collider.enabled = true;
        BtnOnPress.collider.enabled = false;
        uit = TweenPosition.Begin(UIViewerBattleInfo.Instance.gameObject, 0.4f, new Vector3(-41, -190, 0));
        uit.method = UITweener.Method.EaseInOut;
        EventDelegate.Add(uit.onFinished, Finish, true);
    }

    void Finish()
    {
        EventDelegate.Remove(uit.onFinished, Finish);
        uit = TweenPosition.Begin(UIViewerBattleInfo.Instance.gameObject, 0.3f, new Vector3(-41, -126, 0));
        uit.method = UITweener.Method.EaseOut;
        EventDelegate.Add(uit.onFinished, FinishEnd, true);
    }

    void FinishEnd()
    {
        EventDelegate.Remove(uit.onFinished, FinishEnd);
        uit = TweenPosition.Begin(UIViewerBattleInfo.Instance.gameObject, 0.3f, new Vector3(-41, -136, 0));
        uit.method = UITweener.Method.EaseOut;
        //EventDelegate.Add(uit.onFinished, FinishInEnd, true);
    }
    //void FinishInEnd()
    //{
    //    EventDelegate.Remove(uit.onFinished, FinishInEnd);
    //    SetGearState();
    //}

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
    void Update()
    {
        UpdateHomeHp();

        deltaTime += Time.deltaTime;
        if (GameTimeData.Instance.GetStartTag() && deltaTime > 1.0f)
        {
            TimeSpan span = GameTimeData.Instance.GetGameTime();
            int sHour = span.Hours;
            int sMin = span.Minutes;
            int sSec = span.Seconds;
            TimeLabel.text = GameMethod.GetCurrSystemTime(sHour, sMin, sSec); 
            deltaTime = 0.0f;
        }
    }

    private void UpdateHomeHp()
    {
        Ientity homeGreen = EntityManager.GetHomeBase(EntityCampType.CampTypeA);
        Ientity homeRed = EntityManager.GetHomeBase(EntityCampType.CampTypeB);
        if (homeGreen != null && homeGreen.HpMax != 0)
        {
            greenHp.fillAmount = homeGreen.Hp / homeGreen.HpMax;
        }
        if (homeRed != null && homeRed.HpMax != 0)
        {
            redHp.fillAmount = homeRed.Hp / homeRed.HpMax;
        }
         
    }
    
    public void SetShowAllKills(string red,string bule)
    {
       AllRedKills.text = bule;
       AllBuleKills.text = red;
    }
    
}
