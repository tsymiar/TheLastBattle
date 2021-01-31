using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using JT.FWW.Tools;
using BlGame.GameEntity;
using BlGame.GameData;
using System.Linq;

public class UIViewerPlayerPackage : MonoBehaviour {

    public static UIViewerPlayerPackage Instance
    {
        private set;
        get;
    }
    private Dictionary<int, PlayerPackageInfo> PlayerPackage = new Dictionary<int, PlayerPackageInfo>();
    Dictionary<Iplayer, PlayerPackageDate> PackageDate = new Dictionary<Iplayer, PlayerPackageDate>();
    
    void Awake()
    {
       
    }
    void Init()
    {
        Transform temp = transform.FindChild("Item");
        for (int i = 0; i < temp.childCount; i++)
        {
            Transform tran = temp.FindChild("Item" + (i + 1));
            PlayerPackage.Add(i,new PlayerPackageInfo(tran));
            ButtonOnPress button = tran.GetComponent<ButtonOnPress>();
            button.AddListener(i, OnPress);
        }
    }
    void OnEnable()
    {
        Instance = this;
        Init();
        EventCenter.AddListener<Iplayer>(EGameEvent.eGameEvent_PackageBuyInfo, SetPlayerage);
    }
    void OnDisable()
    {
        Instance = null;
        EventCenter.RemoveListener<Iplayer>(EGameEvent.eGameEvent_PackageBuyInfo, SetPlayerage);
    }
    public void OnUpdate()
    {
        foreach (PlayerPackageDate pd in PackageDate.Values)
        {
            pd.Update(Time.deltaTime);
        }
    }
    public void SetPlayerage(Iplayer player)
    {
        int seat = PackageData.Instance.Seat;
        int id = PackageData.Instance.Id; 
        int num = PackageData.Instance.Num; 
        float totalTime = PackageData.Instance.TotalTime;
        float lastTime = PackageData.Instance.LastTime;
        PlayerPackageDate pack;
        if (!PackageDate.TryGetValue(player, out pack))
        {
            pack = new PlayerPackageDate(seat, id, num, totalTime, lastTime);
            PackageDate.Add(player, pack);
        }
        else
        {
            pack.ChangeDate(seat, id, num, totalTime, lastTime);
        }
        if (UIViewerPersonInfo.Instance.SetCurrClickPlayer == player)
        {
            InfoPlayerPackage(player, seat);
        }
    }
    public void InfoPlayerPackage(Iplayer player, int seat)
    {
        PlayerPackageDate pack;

        UIViewerPlayerPackage.Instance.ResetAllSlot();
        if (PackageDate.TryGetValue(player, out pack))
        {
            int id;
            int num;
            float time;
            float end;
            for (int i = 0; i < pack.SeatId.Count; i++)
            {
                id = pack.SeatId.ElementAt(i).Value;
                num = pack.SeatNum.ElementAt(i).Value;
                time = pack.SeatTime.ElementAt(i).Value;
                end = pack.EndTime.ElementAt(i).Value;
                UIViewerPlayerPackage.Instance.SetCurrPackInfo(i, id, num, time, end);
            }
        }
    }
    class PlayerPackageDate
    {
        public Dictionary<int, int> SeatId = new Dictionary<int, int>();//位置、ID号
        public Dictionary<int, int> SeatNum = new Dictionary<int, int>();//位置、物品个数
        public Dictionary<int, float> SeatTime = new Dictionary<int, float>();//位置。冷却最大时间
        public Dictionary<int, float> EndTime = new Dictionary<int, float>();//位置。冷却最大时间
        public PlayerPackageDate(int seat, int id, int num, float totalTime, float lastTime)
        {
            SeatId[seat] = id;
            SeatNum[seat] = num;
            SeatTime[seat] = totalTime;
            EndTime[seat] = lastTime;
        }
        public void ChangeDate(int seat, int id, int num, float totalTime, float lastTime)
        {
            SeatId[seat] = id;
            SeatNum[seat] = num;
            SeatTime[seat] = totalTime;
            EndTime[seat] = lastTime;
        }
        public void Update(float deltaTime)
        {
            for (int i = 0; i < EndTime.Count; i++)
            {
                EndTime[i] -= deltaTime;

                if (EndTime[i] < 0f)
                {
                    EndTime[i] = 0;
                }
            }
        }
    }
    
    public void SetCurrPackInfo(int i,int id, int num, float time,float end)
    {
        PlayerPackage[i].ShowPlayerPackageInfo(id, num, time, end);
    }
    class PlayerPackageInfo
    {
        public int itemId;
        public UISprite PackageIcon;
        public UILabel PackageNum;
        public UISprite PackageSelect;
        public CdCountDown cdDown;
        public PlayerPackageInfo(Transform tran){
            itemId = 0;
            PackageIcon = tran.FindChild("icon").GetComponent<UISprite>();
            PackageNum = tran.FindChild("LabelCount").GetComponent<UILabel>();
            PackageSelect = tran.FindChild("SpriteSelect").GetComponent<UISprite>();
            cdDown = tran.GetComponent<CdCountDown>();
            PackageIcon.spriteName = "";
            PackageIcon.gameObject.SetActive(false);
            //Debug.LogError("-------------------------------------------------");
        }

		public void Reset()
		{
            itemId = 0;
			PackageIcon.gameObject.SetActive (false);
			PackageNum.gameObject.SetActive (false);
			cdDown.EndCdCountDown ();
		}
        public void ShowPlayerPackageInfo(int id, int num, float totalTime, float lastTime)
        {
            itemId = id;
            string infoName = ConfigReader.ItemXmlInfoDict[id].sIcon;
            PackageIcon.spriteName = infoName;
            PackageIcon.gameObject.SetActive(num!=0);
            PackageNum.text = num.ToString();
            PackageNum.gameObject.SetActive(num > 1);
			if (num!=0 && totalTime > 0f && lastTime > 0f) 
			{
				cdDown.StartCdCountDown(totalTime, lastTime);
			}
		}
    }
	// Use this for initialization
	void Start () {
	
	}
	

	public void ResetAllSlot()
	{
		foreach(PlayerPackageInfo pack in PlayerPackage.Values)
		{
			pack.Reset();
		}
	}


    private float startTime = 0f;
    private int itemId = -1;
    private const float timeLimit = 0.5f;

    void OnPress(int ie, bool isPress)
    {
        if (isPress)
        {
            startTime = Time.time;
            itemId = PlayerPackage[ie].itemId;
            return;
        }
        itemId = -1;
        startTime = 0f;
        if (UIItemDestribe.Instance != null && UIItemDestribe.Instance.desObj != null)
        {
            UIItemDestribe.Instance.OpenItemDestribe(false, 0);

            return;
        }
    }

    // Update is called once per frame
    void Update()
    {
        OnUpdate();

        if (startTime == 0 || itemId == -1)
            return;


        if (Time.time - startTime >= timeLimit)
        {
            if (UIItemDestribe.Instance != null)
            {
                UIItemDestribe.Instance.OpenItemDestribe(true, itemId);
            }

            startTime = 0f;
            itemId = -1;
        }
    }

}
