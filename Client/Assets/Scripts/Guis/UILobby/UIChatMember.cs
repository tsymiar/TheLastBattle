using UnityEngine;
using System.Collections;

public class UIChatMember : MonoBehaviour {

    public static UIChatMember Instance
    {
        private set;
        get;
    }
    UILabel NickNameLabel;
    UISprite CheckSprite;
    ButtonOnPress CheckButton;
    public string NickName
    {
        set;
        get;
    }
    public string HeadID
    { set; get; }
    void Init()
    {
        NickNameLabel = this.transform.FindChild("Name").GetComponent<UILabel>();
        CheckSprite = this.transform.FindChild("Highlight").GetComponent<UISprite>();
        CheckButton = this.transform.GetComponent<ButtonOnPress>();
    }
    public void SetCheck(bool isCheck)
    {
        CheckSprite.gameObject.SetActive(isCheck);
    }

    void OnEnable()
    {
        Instance = this;
        Init();
        CheckButton.AddListener(OnButtonPress);
        
    }
    public void ShowInfo(string nickName,bool isCheck,string headid)
    {
        NickName = nickName;
        HeadID = headid;
        NickNameLabel.text = nickName;
        SetCheck(isCheck);
    }

    void OnDisable()
    {
    }

    void OnButtonPress(int ie, bool isPress)
    {
        if (isPress)
            return;
        //UIFriendChatInfo.Instance.SetTalkChatBox(NickName, HeadID);
    }


	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
