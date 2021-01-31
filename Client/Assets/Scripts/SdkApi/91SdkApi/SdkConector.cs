using UnityEngine;
using BlGame;
using System.Runtime.InteropServices;
using BlGame.GameData;
using BlGame.Ctrl;

public class SdkConector : MonoBehaviour
{
    //是否初始化成功
    public static bool isInitSuccess = false;
    public static bool isLogOut = false;
    public static bool islink = false;
    public static SdkConector Instance = null;

    //Android平台sdk接口
#if UNITY_ANDROID
    public static AndroidJavaObject jo = null;
#endif

    void Awake() {
        Instance = this;
        DontDestroyOnLoad(gameObject);
    }

    void OnDisable() { 
        Instance = null;
    }

	void Start () 
	{      		
        Init();
	}

	#region 91SDK - 回调

    //初始化成功回调
	void InitSuccessCallBack()
	{
        isInitSuccess = true;
	}

	void LoginResultSuccess(string uin)
	{
        islink = false;

        Debugger.Log("LoginResultSuccess" + uin + "   " + SessionId());
#if SDK_PP || SDK_AS
		ShowToolBar(0);
#endif
		// TODO: 回调：账号登陆成功
        //TestObj.SendMessage("ShowResult","回调：账号登陆成功:"+uin,SendMessageOptions.RequireReceiver);

//         CEvent eve = new CEvent(EGameEvent.eGameEvent_SdkRegisterSuccess);
//         eve.AddParam("uin", uin);
//         eve.AddParam("sessionId", sessionId());
//         SelectServerData.Instance.SetServerInfo((int)SdkManager.Instance.GetPlatFrom(), uin, sessionId());
//         EventCenter.SendEvent(eve);          

        // LoginCtrl.Instance.SdkRegisterSuccess(uin, sessionId());

        EventCenter.Broadcast<string, string>(EGameEvent.eGameEvent_SdkRegisterSuccess, uin, SessionId());
	}

    void LeavePlatform(string str) {
        if (str.CompareTo("1") == 0)
        {
            LoginCtrl.Instance.SdkLogOff();
        }
        else
        {
            if (isLogOut)
            {
                LoginCtrl.Instance.SdkLogOff();
            }
        }
    }

	void LoginResultFail()
	{
		// TODO: 回调：账号登陆失败
		//TestObj.SendMessage("ShowResult","回调：账号登陆失败",SendMessageOptions.RequireReceiver);
        isLogOut = true;
	}

    //void PauseExit()
    //{
    //    // TODO: 回调：暂停页-从桌面回游戏
    //    //TestObj.SendMessage("ShowResult","回调：暂停页-从桌面回游戏",SendMessageOptions.RequireReceiver);
    //}

    //void PayResultUserCancel(string result)
    //{
    //    // TODO: 回调：用户取消支付
    //    //TestObj.SendMessage("ShowResult","回调：用户取消支付"+result,SendMessageOptions.RequireReceiver);
    //}

    //void PayResultNetworkFail(string result)
    //{
    //    // TODO: 回调：支付时网络错误
    //    //TestObj.SendMessage("ShowResult","回调：支付时网络错误"+result,SendMessageOptions.RequireReceiver);
    //}

    //void PayResultServerReturnError(string result)
    //{
    //    // TODO: 回调：支付时服务器发生错误
    //    //TestObj.SendMessage("ShowResult","回调：支付时服务器发生错误"+result,SendMessageOptions.RequireReceiver);
    //}

    //void PayResultOrderSerialSubmitted(string result)
    //{
    //    // TODO: 回调:订单已经提交，此时应当对游戏服务器进行轮询支付结果
    //    //使用异步支付或代币充值支付方式的需要处理
    //    //TestObj.SendMessage("ShowResult","回调:订单已经提交，此时应当对游戏服务器进行轮询支付结果"+result,SendMessageOptions.RequireReceiver);
    //}

    //void PayResultOtherError(string result)
    //{
    //    // TODO: 回调：支付时其他未知错误
    //    //TestObj.SendMessage("ShowResult","回调：支付时其他未知错误"+result,SendMessageOptions.RequireReceiver);
    //}

    //void PayResultSuccess(string result)
    //{
    //    // TODO: 回调：支付成功，应做发货处理
    ////	TestObj.SendMessage("ShowResult","回调：支付成功，应做发货处理"+result,SendMessageOptions.RequireReceiver);
    //}

    //void SessionInvalid(string result)
    //{
    //    // TODO: 回调：Session会话失效
    //    //TestObj.SendMessage("ShowResult","回调：Session会话失效",SendMessageOptions.RequireReceiver);
    //}

	#endregion

	#region 91SDK-API

	public static string GetBundleVersion()
	{
#if UNITY_STANDALONE_WIN || UNITY_EDITOR || SKIP_SDK
        return "1.0.1";
#elif UNITY_IPHONE
        return LsGetBundleVersion();
#elif UNITY_ANDROID
        return SdkConector.jo.Call<string>("GetBundleVersion");
#endif
	}

	public static string GetBundleIdentifier()
	{
#if UNITY_STANDALONE_WIN || UNITY_EDITOR || SKIP_SDK
        return "com.xxx.blzhyz";
#elif UNITY_IPHONE
		return LsGetBundleIdentifier();
#elif UNITY_ANDROID
        return SdkConector.jo.Call<string>("GetBundleIdentifier");        
#endif
    }

	public static void Init()
	{
        //sdk初始化

        isLogOut = false;
        //测试
        Debugger.Log("Sdk Connector start");
#if UNITY_ANDROID
        AndroidJavaClass jc = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
        jo = jc.GetStatic<AndroidJavaObject>("currentActivity");
        jo.Call("Init");
#elif UNITY_IPHONE
        LsSetUnityReceiver("SdkConnect");             
		LsNdInit();
#endif
	}


	public static void ShowToolBar(int point)
	{
		Debugger.Log("-------1-----");
#if SDK_PP
        BlGameUI.Instance.ShowPPSdkButton(true);
#elif SDK_AS
		BlGameUI.Instance.ShowASSdkButton(true);
#else
    #if UNITY_ANDROID
        jo.Call("ShowToolBar");
    #elif UNITY_IPHONE            
		    LsNdShowToolBar(point);                 // TODO: API：显示工具条: point=1,2,3,4,5,6,分别表示：左上、右上、左中、右中、左下、右下 
    #endif
#endif
	}

	public static void HideToolBar()
	{
#if SDK_PP
        BlGameUI.Instance.ShowPPSdkButton(false);
#elif SDK_AS
		BlGameUI.Instance.ShowASSdkButton(false);
#else    
    #if UNITY_ANDROID
        jo.Call("HideToolBar");
    #elif UNITY_IPHONE
            LsNdHideToolBar();
    #endif
#endif
    }

    public static string LoginUin()
    {
#if UNITY_ANDROID
        return jo.Call<string>("LoginUin");
#elif UNITY_IPHONE   
		return LsloginUin();                      // TODO: API：获取账号主键ID
#else
        return "";
#endif
    }

	public static string  SessionId()
	{
#if UNITY_ANDROID
        return jo.Call<string>("SessionId");
#elif UNITY_IPHONE      
		return  LssessionId();                    // TODO: API：获取会话ID
#else
        return "";
#endif
    }

	public static string  NickName()
	{
#if UNITY_ANDROID
        return jo.Call<string>("NickName");
#elif UNITY_IPHONE   		
		return  LsnickName();                      // TODO: API：获取昵称
#else
        return "";
#endif
	}

	public static void Logout(int param)
	{
        /*
        //talkEnd 
        CEvent eve = new CEvent(EGameEvent.eGameEvent_TalkEnd); 
        EventCenter.SendEvent(eve);
        */
#if UNITY_ANDROID
        jo.Call("Logout", param);
#elif UNITY_IPHONE	
        LsNdLogout(param);                         // TODO: API：注销账号
#endif
	}



    public static void Quit()
    {
        
#if UNITY_ANDROID
        jo.Call("Quit");
#elif UNITY_IPHONE	
        
#endif
    }




    //PP需要添加按钮显示个人充值中心
	public static void ShowCenter()
	{
		LsNdShowCenter ();
	}

    //public static int NdUniPay(string cooOrderSerial,string productId,string productName,float productPrice,int productCount,string payDescription)
    //{
    //    // TODO: API：同步支付（订单号，道具ID，道具名，价格，数量，分区：不超过20个英文或数字的字符串）
    //    return LSNdUniPay(cooOrderSerial,productId,productName,productPrice,productCount,payDescription);
    //}

    //public static int NdUniPayAsyn(string cooOrderSerial,string productId,string productName,float productPrice,int productCount,string payDescription)
    //{
    //    // TODO: API：异步支付（订单号，道具ID，道具名，价格，数量，分区：不超过20个英文或数字的字符串）
    //    return LsNdUniPayAsyn(cooOrderSerial,productId,productName,productPrice,productCount,payDescription);
    //}

    //public static int NdUniPayForCoin(string cooOrderSerial,int needPayCoins,string payDescription)
    //{
    //    // TODO: API：代币充值（订单号，代币数量，分区：不超过20个英文或数字的字符串）
    //    return LsNdUniPayForCoin(cooOrderSerial,needPayCoins,payDescription);
    //}

	#endregion

	#region 用C对91SDK-API进行封装

	[DllImport("__Internal")]
	private extern static string LsGetBundleVersion();

	[DllImport("__Internal")]
	private extern static string LsGetBundleIdentifier();

	[DllImport("__Internal")]
	private static extern void LsSetUnityReceiver(string gbName);

	[DllImport("__Internal")]
	private static extern void LsNdInit();

	[DllImport("__Internal")]
	private static extern void LsNdLogin();

	[DllImport("__Internal")]
	private static extern void LsNdLoginEx();

	[DllImport("__Internal")]
	private static extern void LsNdShowToolBar(int point);

	[DllImport("__Internal")]
	private static extern void LsNdHideToolBar();

	[DllImport("__Internal")]
	private static extern bool LsisLogined();

	[DllImport("__Internal")]
	private static extern void LsNdPause();
	
	[DllImport("__Internal")]
	private static extern void LsNdShowCenter();
	
	[DllImport("__Internal")]
	private static extern int LsgetCurrentLoginState();

	[DllImport("__Internal")]
	private static extern string LsloginUin();

	[DllImport("__Internal")]
	private static extern string LssessionId();

	[DllImport("__Internal")]
	private static extern string LsnickName();

	[DllImport("__Internal")]
	private static extern int LsNdLogout(int param);

	[DllImport("__Internal")]
	private static extern int LSNdUniPay(string cooOrderSerial,string productId,string productName,float productPrice,int productCount,string payDescription);

	[DllImport("__Internal")]
	private static extern int LsNdUniPayAsyn(string cooOrderSerial,string productId,string productName,float productPrice,int productCount,string payDescription);

	[DllImport("__Internal")]
	private static extern int LsNdUniPayForCoin(string cooOrderSerial,int needPayCoins,string payDescription);

	#endregion
}
