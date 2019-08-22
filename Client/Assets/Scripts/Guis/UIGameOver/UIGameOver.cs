using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using System.Collections.Generic;
using JT.FWW.Tools;
using JT.FWW.GameData;
using System.Linq;
using GameDefine;
using BlGame.Resource;
using System;

public class UIGameOver : MonoBehaviour {


	UILabel WinLabel = null;
	ButtonOnPress ExitButton;
	//ButtonOnPress AddFriendBtn = null;//添加好友按钮，预留
	UILabel CountDown;
	UISprite winTip;
	UISprite lossTip;

	bool isStartTime = false;
	float StartTime = 10.0f;
	private List<UIShowScoreInfo> ListPlayer = new List<UIShowScoreInfo> ();

	private List<ShowScoreDate> ListDate = new List<ShowScoreDate>();
	enum GAMEOVER
	{
		EXIT,
		CANCEL,
	}
	public static UIGameOver Instance {
		private set;
		get;
	}
	// Use this for initialization
	void Start () {
	
	}
	// Update is called once per frame
	void Update () {
        //if (isStartTime) {
        //    StartTime -= Time.deltaTime;
        //    ShowCount(CountDown,(int)StartTime);
        //    if(StartTime <= 0){
        //        isStartTime = false;
        //    }
        //    UIGameRecord.Instance.showaudiotimeold = System.DateTime.Now;
        //}
	}
	void InitExit()
	{
        lossTip = this.transform.Find("results/Lose").GetComponent<UISprite>();
        winTip = this.transform.Find("results/Win").GetComponent<UISprite>();
        lossTip.gameObject.SetActive(false);
        winTip.gameObject.SetActive(false);
		ExitButton = this.transform.Find("Button").GetComponent<ButtonOnPress>();
		Transform tran = this.transform.FindChild ("Player");
		int num = tran.childCount;
		for (int i = 0; i < num; i++) {
			Transform temp = tran.FindChild("Player" + (i + 1));
//			AddFriendBtn = temp.FindChild("AddFriends").GetComponent<ButtonOnPress>();
//			AddFriendBtn.AddListener(i + i,BtnPress);
			ListPlayer.Add(new UIShowScoreInfo(temp));
		}
		CountDown = this.transform.FindChild("CountDown/Label").GetComponent<UILabel>();
	}


	void OnEnable(){
		Instance = this;
		InitExit ();
//		SetWin ();
		ExitButton.AddListener (0,BtnPress);
	}
	void GameOverError(EErrorCode errorCode)
	{
	}
	public void RemoveList()
	{
		ListDate.Clear ();
	}
	void OnDisble()
	{
		Instance = null;
		ExitButton.RemoveListener (BtnPress);
        //DestroyImmediate(this.gameObject);
	}
	int i = 0;
	int num = 3;
	public void SetScoreInfo(UInt64 key,string pszNickName,uint level,uint heroKills,uint deadTimes,EntityCampType Type)
	{
		isStartTime = true;
		ShowScoreDate ssd = new ShowScoreDate ( key, pszNickName, (int)level, (int)heroKills, (int)deadTimes, (int)Type);
		ListDate.Add (ssd);
	}
    public void Show()
    {
        i = 0;
        num = 3;
        foreach (var item in ListDate)
        {
            if ((EntityCampType)item.HeroCampID == EntityCampType.CampTypeA)
            {
                if(i <= 2)
                    ListPlayer[i++].SetShowLabel(item.HeadIcon, item.HeadName, item.HeroLevel, item.HeroKills, item.HeroDeadTimes, item.HeroCampID);
                ShowBattleResult();
            }
            else if ((EntityCampType)item.HeroCampID == EntityCampType.CampTypeB)
            {
                if(num <=5 )
                    ListPlayer[num++].SetShowLabel(item.HeadIcon, item.HeadName, item.HeroLevel, item.HeroKills, item.HeroDeadTimes, item.HeroCampID);
                ShowBattleResult();
            }
        }
    }

	private void ShowBattleResult()
	{
        //BlGame.AudioManager.Instance.StopBgAudio();
        Iplayer player = PlayerManager.Instance.LocalPlayer;
        if (player == null)
            player = PlayerManager.Instance.LocalAccount;
        if (player.StateSituation == SITUATION.WIN)
		{
            //BlGame.AudioManager.Instance.PlayLongVoiceAudio(Resources.Load(AudioDefine.PATH_HERO_KILLSOUND + "Victory") as AudioClip);

            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_HERO_KILLSOUND + "Victory", ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;
                
            BlGame.AudioManager.Instance.PlayLongVoiceAudio(clip);
            
			winTip.gameObject.SetActive(true);			
        }
        else if (player.StateSituation == SITUATION.LOSE)
        {
            lossTip.gameObject.SetActive(true);
            //BlGame.AudioManager.Instance.PlayLongVoiceAudio(Resources.Load(AudioDefine.PATH_HERO_KILLSOUND + "Failed") as AudioClip);

            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_HERO_KILLSOUND + "Failed", ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;
                
            BlGame.AudioManager.Instance.PlayLongVoiceAudio(clip);
        }
        else
        {
            //WinLabel.text = "平局";
            Debug.LogError("平局");
        }
	}
	void BtnPress(int ie , bool pressed)
	{
		if (pressed)
			return;
		switch ((GAMEOVER)ie) {
		case GAMEOVER.EXIT:
//			Application.Quit();  //返回结算界面
//			if(UIGameOver.Instance != null)
//				DestroyImmate(UIGameOver.Instance.gameObject);
//			DestroyImmediate(GamedieMethod.GetUiCamera.transform.FindChild("UIGamePlay(Clone)").);
//			JxBlGame.Instance.SetGameState (GameDefine.GameState.STATE_LOBBY);

			break;
		default:
			//添加好友事件触发
			break;
		
		}

	}

	private void ShowCount(UILabel label ,int time)
	{
		string timeString = "";
		if(time < 10)
			timeString = "00:0"+time.ToString();
		else if(time < 60)
			timeString = "00:"+time.ToString();
		else if((time / 60) >= 10  )
			timeString = (time % 60).ToString()+":"+ (time / 60).ToString();
		else if((time / 60) < 10)
			timeString = "0"+(time / 60).ToString()+":"+ (time %60).ToString();
		label.text = timeString;
	}

	public class ShowScoreDate
	{
		public UInt64 HeadIcon;
		public string HeadName;
		public int HeroLevel;
		public int HeroKills;
		public int HeroDeadTimes;
		public int HeroCampID;
		public ShowScoreDate(UInt64 headIcon,string name, int level,int kill,int times,int type)
		{
			HeadIcon = headIcon;
			HeadName = name;
			HeroLevel = level;
			HeroKills = kill;
			HeroDeadTimes = times;
			HeroCampID = type;
		}
	}
	public class UIShowScoreInfo
	{		
		public UISprite HeadIcon;
		public UILabel HeadNameLabel;
		public UILabel HeroLevelLabel;
		public UILabel HeroKillsLabel;
		public UILabel HeroDeadTimesLabel;
		public UILabel HeroCampIDLabel;
		Transform tran;
		public UIShowScoreInfo(Transform temp)
		{
			tran = temp;
			HeadNameLabel = temp.FindChild("Name").GetComponent<UILabel>();
			HeroLevelLabel = temp.FindChild("Level/Label").GetComponent<UILabel>();
			HeroKillsLabel = temp.FindChild("Kills/Label").GetComponent<UILabel>();
            HeroDeadTimesLabel = temp.FindChild("Death/Label").GetComponent<UILabel>();
			HeadIcon = temp.FindChild("Head/thumbnail").GetComponent<UISprite>();
            HeadNameLabel.text = "";
            HeroLevelLabel.text = "";
            HeroKillsLabel.text = "";
            HeroDeadTimesLabel.text = "";
		}
		public void SetShowLabel(UInt64 headIcon,string name, int level,int kill,int times,int campId)
		{ 
			tran.gameObject.SetActive (true);
            if (!EntityManager.AllEntitys.ContainsKey(headIcon))
            {
                return;
            }
            Ientity sEntity = EntityManager.AllEntitys[headIcon];
            int id = (int)sEntity.ObjTypeID;
			HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo (id);
			HeadIcon.spriteName = info.HeroSelectHead.ToString ();
			HeadNameLabel.text = name;
			HeroLevelLabel.text = level.ToString ();
			HeroKillsLabel.text = kill.ToString ();
			HeroDeadTimesLabel.text = times.ToString ();
		}
	}
}
