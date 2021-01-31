using UnityEngine;
using System.Collections;

public class UIFrameTips : MonoBehaviour {

    //public static UIFrameTips Instance
    //{
    //    private set;
    //    get;
    //}
    ButtonOnPress LeftButton;
    ButtonOnPress RightButton;
    private string RoomID;
    private string PassWord;
    private int PassWordLength;
    enum ButtonBtn
    {
        LeftButton,
        RightButton
    }
	// Use this for initialization
	void Start () {
	
	}
    void OnEnable()
    {
        Init();
        LeftButton.AddListener((int)ButtonBtn.LeftButton, ButtonPress);
        RightButton.AddListener((int)ButtonBtn.RightButton, ButtonPress);
    }
	
	// Update is called once per frame
	void Update () {
	
	}
    void Init()
    {
        LeftButton = transform.FindChild("Status1/Button1").GetComponent<ButtonOnPress>();
        RightButton = transform.FindChild("Status1/Button2").GetComponent<ButtonOnPress>();
    }
    void ButtonPress(int ie, bool isPress)
    {
        if (isPress)
            return;
        switch ((ButtonBtn)ie)
        {
            case ButtonBtn.LeftButton:
                CGLCtrl_GameLogic.Instance.EmsgToss_AskAddObserver(RoomID, PassWord, PassWordLength);
                break;
            case ButtonBtn.RightButton:
                DestroyImmediate(this.gameObject);
                break;
        }
    }
    public void SetSendMsg(string roomid ,string password,int passwordlength)
    {
        RoomID = roomid;
        PassWord = password;
        PassWordLength = passwordlength;
    }
}
