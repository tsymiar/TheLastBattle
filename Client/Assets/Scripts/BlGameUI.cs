using UnityEngine;
using System;
using BlGame.GameData;
using System.Collections;
using BlGame.GameEntity;
using GameDefine;
using BlGame.GameState;
using BlGame.Resource;
using BlGame.View;

public class BlGameUI : MonoBehaviour
{
	//PP AS用户界面
	private GameObject mToolBarObj;
	

	public static BlGameUI Instance
	{
		private set;
		get;
	}
	
	void Awake()
	{
		if (Instance != null)
		{
			DestroyImmediate(this.gameObject);
			return;
		}
		Instance = this;
		DontDestroyOnLoad(this.gameObject);
	}
	
	void Start()
	{
		//创建PPSDK按钮
		#if SDK_PP
		CreatePPSdkButton();
		#elif SDK_AS
		CreateASSdkButton();
		
		#endif
	}
	
	void OnDestroy()
	{
		
	}
	
	
	public void CreatePPSdkButton()
	{      
		ResourceUnit ppUnit = ResourcesManager.Instance.loadImmediate("Guis/PP", ResourceType.PREFAB);
		mToolBarObj = GameObject.Instantiate(ppUnit.Asset) as GameObject;
		DontDestroyOnLoad(mToolBarObj);
		
		//绑定到camera
		GameObject cameraObj = GameObject.Find("Camera");
		mToolBarObj.transform.parent = cameraObj.transform;
		mToolBarObj.transform.localScale = new Vector3(1, 1, 1);
		mToolBarObj.transform.localPosition = new Vector3(0,0,0);
		
		//添加listener
		GameObject positonObj = mToolBarObj.transform.FindChild("Position").gameObject;
		ButtonOnPress positonButtonOnPress = positonObj.GetComponent<ButtonOnPress>();
		
		//添加listener    
		positonButtonOnPress.AddListener(PositionOnPress);
		
		
	}
	
	public void ShowPPSdkButton(bool flag)
	{
		if (mToolBarObj == null)
		{
			CreatePPSdkButton();
			Debug.Log("------------ mPPObje is null");
		}
		
		mToolBarObj.SetActive(flag);
	}
	
	
	
	
	public void CreateASSdkButton()
	{      
		ResourceUnit ASUnit = ResourcesManager.Instance.loadImmediate("Guis/Aisi", ResourceType.PREFAB);
		mToolBarObj = GameObject.Instantiate(ASUnit.Asset) as GameObject;
		DontDestroyOnLoad(mToolBarObj);
		
		//绑定到camera
		GameObject cameraObj = GameObject.Find("Camera");
		mToolBarObj.transform.parent = cameraObj.transform;
		mToolBarObj.transform.localScale = new Vector3(1, 1, 1);
		mToolBarObj.transform.localPosition = new Vector3 (0, 0, 0);
		
		
		//添加listener
		GameObject positonObj = mToolBarObj.transform.FindChild("Position").gameObject;
		ButtonOnPress positonButtonOnPress = positonObj.GetComponent<ButtonOnPress>();
		
		//添加listener    
		positonButtonOnPress.AddListener(PositionOnPress);
		
		
	}
	
	public void ShowASSdkButton(bool flag)
	{
		if (mToolBarObj == null)
		{

			CreateASSdkButton();			
		}

		mToolBarObj.SetActive(flag);
	}
	
	
	public void PositionOnPress(int ie, bool isPress)
	{
		//Debug.Log("button on press");
		SdkConector.ShowCenter ();
	}
	
	
	public void OnOpenUI(string path)
	{
		//GameObject Gobj = UnityEngine.Object.Instantiate(Resources.Load(path)) as GameObject
		
		ResourceUnit GobjUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
		GameObject Gobj = GameObject.Instantiate(GobjUnit.Asset) as GameObject;                               
		
		
		Gobj.transform.parent = GetUIParent().transform;
		Gobj.transform.localScale = Vector3.one;
		Gobj.transform.localPosition = Vector3.zero;
	}
	
	/// <summary>
	/// 在UI摄像机下打开UI
	/// </summary>
	/// <param name="path"></param>
	public void OnOpenUIPathCamera(string path)
	{
		//GameObject Gobj = UnityEngine.Object.Instantiate(Resources.Load(path)) as GameObject;
		ResourceUnit GobjUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
		GameObject Gobj = GameObject.Instantiate(GobjUnit.Asset) as GameObject;                               
		
		
		Gobj.transform.parent = GameDefine.GameMethod.GetUiCamera.transform;
		Gobj.transform.localScale = Vector3.one;
		Gobj.transform.localPosition = Vector3.zero;
	}
	
	public void OnDestroyUI(GameObject obj)
	{
		DestroyImmediate(obj);
	}
	
	private GameObject GetUIParent()
	{
		switch (GameStateManager.Instance.GetCurState().GetStateType())
		{
		case GameStateType.GS_Login:
		{
			BaseWindow pWindow = WindowManager.Instance.GetWindow(EWindowType.EWT_LoginWindow);
			if (pWindow != null && pWindow.GetRoot()!=null)
			{
				return pWindow.GetRoot().gameObject;
			}
			return null;
		}
		case GameStateType.GS_Play:
        {
            BaseWindow pWindow = WindowManager.Instance.GetWindow(EWindowType.EWT_GamePlayWindow);
            if (pWindow != null && pWindow.GetRoot() != null)
            {
                return pWindow.GetRoot().gameObject;
            }
            return null;
        }
		case GameStateType.GS_Over:
		{
			BaseWindow pWindow = WindowManager.Instance.GetWindow(EWindowType.EWT_ScoreWindow);
			if (pWindow != null && pWindow.GetRoot() != null)
			{
				return pWindow.GetRoot().gameObject;
			}
			return null;
		}
		}
		
		return null;
	}
	
}
