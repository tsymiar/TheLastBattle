using UnityEngine;
using System.Collections;
using GameDefine;
public class UIAltarInHead : MonoBehaviour
{
	UISprite HeadIcon;
	private float Height = 0;
	private Camera _ui2dCamera;
	private Camera _mainCamera;
	
	private Transform taget;
	void Awake()
	{
		_mainCamera = Camera.main;
		_ui2dCamera = GameMethod.GetUiCamera;
		HeadIcon = this.transform.FindChild("Head").GetComponent<UISprite>(); 
    }
    void OnDisable()
    {
        taget = null;
	}
	public void SetCurrHeadIcon(int type,Transform target)
	{
		if (taget == null)
			taget = target;
		Height = ConfigReader.GetNpcInfo(type).NpcXueTiaoHeight;
		HeadIcon.spriteName = ConfigReader.GetNpcInfo(type).HeadPhoto.ToString();
		SetPosInfo ();
	}
    public void DelAltarInHead()
    {
        DestroyImmediate(this.gameObject);
    }

	public void SetPosInfo()
	{
		Vector3 pos_3d = taget.position + new Vector3(0f, Height ,0f);
		// 血条对应在屏幕的位置
		Vector2 pos_screen = _mainCamera.WorldToScreenPoint(pos_3d);
		// 血条对应在ui中的位置
		Vector3 pos_ui = _ui2dCamera.ScreenToWorldPoint(pos_screen);
		
		transform.position = Vector3.Slerp(transform.position,pos_ui,Time.time);
	}

	// Use this for initialization
    void Start()
    {
	
	}
	
	// Update is called once per frame
    void Update()
    {
        if (taget != null)
        {
			SetPosInfo();
		}
	}
}
