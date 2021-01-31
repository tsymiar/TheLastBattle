using UnityEngine;
using System.Collections;

using GameDefine;
using BlGame.GameData;
using BlGame.GameState;
using System.Collections.Generic;
using BlGame.Resource;

public class LoadScene : MonoBehaviour {

	public static LoadScene Instance {
				private set;
				get;
	}

	public UIProgressBar m_ProGressbar;

	public bool isCloseBySelf = true;

    public GameStateType GState
    {
				set;
				get;
	}

    ResourceAsyncOperation async = null;

	bool isEnd = false;
     
    private float startX = 0f;

    private GameObject objEffect;

    private UILabel labelTip;

   // private float xOffset = 45f;


	void Awake () {
		Instance = this;
        startX = m_ProGressbar.foregroundWidget.transform.localPosition.x;// +xOffset;
        objEffect = m_ProGressbar.transform.FindChild("load_flash").gameObject;
        labelTip = transform.FindChild("ShowTip/ShowTip_001/TipLabel/randLable1").GetComponent<UILabel>();
	}

    int Progress;
	// Use this for initialization
	void Start () {
        Progress = 0;
        isEnd = false;
	}

    const int MaxProgressPerc = 95;
	
	// Update is called once per frame
	void Update () 
    {
        if (async == null)
        {
            return;
        }
        if (m_ProGressbar != null)
        {
            int pg = (int)m_ProGressbar.value;
            if (Progress < MaxProgressPerc)
            {
                Progress += 10;
            }
            if (pg > MaxProgressPerc)
            {
                Progress = pg;
            }
            m_ProGressbar.value = Progress / 100.0f;// async.progress;
            SetEffectPos();
        }
        if(async.Complete && !isEnd)
        {
            if (OnLoadFinish != null) 
            {
                OnLoadFinish(GState);
                isEnd = true;
               
                //进入游戏场景前预加载特效 
                ReadPreLoadConfig.Instance.PreLoad();
            }	
            if(isCloseBySelf)
                CloseLoading();
        }
        //if(isEnd) return;
        //if (m_ProGressbar != null) {

        //    m_ProGressbar.value = ResourceManager.Instance.loadProcess.curProgress;// async.progress;
        //    SetEffectPos();
        //}
        //if (ResourceManager.Instance.loadProcess.isDone == true)
        //{
        //    if (OnLoadFinish != null) {
        //        OnLoadFinish(GState);
        //    }	
        //    if(isCloseBySelf)
        //        CloseLoading();
        //    isEnd = true;
        //}

	}

    public void ReleaseResource()
    {
        //进入游戏前进行资源清理

        //强制释放界面贴图资源
        ReadReleaseResourceConfig.Instance.ForceReleaseResource();
        //UIDrawCall.ReleaseAll();
        Resources.UnloadUnusedAssets();
        System.GC.Collect();
    }

	public void CloseLoading(){
		DestroyImmediate(this.gameObject);
	}
	void OnEnable(){
		if (m_ProGressbar != null) {
			m_ProGressbar.value = 0.0f;	 
            SetEffectPos();
		}

        int[] idArray = GameMethod.GetRandIntArrayFromList(LoadingTipData.GetIdList(), 1);
        string tip = ConfigReader.GetLoadingTipInfo(idArray[0]).Tip;
        labelTip.text = tip;
	}

	void OnDisable(){

	}

	void OnDestroy(){
		if (Instance != null) {
			Instance = null;		
		}
	}
    //public void LoadSceneAsset(string name)
    //{
    //    ResourceManager.Instance.loadProcess.Start();
    //    BlGame.Resource.ResourceManager.Instance.loadProcess.AddLogicFile("Scene/Prefab/" + name + "_Scene.assetbundle", BlGame.Resource.LogicResouce.ELogicResType.ERes_Scene);
    //    BlGame.Resource.ResourceManager.Instance.loadProcess.SceneRes = "Scene/Prefab/" + name + "_Scene.assetbundle";
    //}
	public void LoadAsignedSene(string name)
    {
        //加载场景之前需要进行清除操作
        BlGame.Effect.EffectManager.Instance.DestroyAllEffect();
        //清除GameObjectPool数据
        GameObjectPool.Instance.Clear();

        async = ResourcesManager.Instance.loadLevel(name, null);
	}

    //IEnumerator loadScene(string mapName)
    //{
    //    ResourceAsyncOperation async = ResourcesManager.Instance.loadLevel(mapName, null);

    //    yield return async;
    //}

    void SetEffectPos()
    {
        float value = m_ProGressbar.value;

        Vector3 pos = new Vector3(value * m_ProGressbar.foregroundWidget.width + startX, m_ProGressbar.foregroundWidget.transform.localPosition.y, m_ProGressbar.foregroundWidget.transform.localPosition.z);
       // Vector3 pos = new Vector3(value * m_ProGressbar.foregroundWidget.width + xOffset, m_ProGressbar.foregroundWidget.transform.localPosition.y, m_ProGressbar.foregroundWidget.transform.localPosition.z);

        objEffect.transform.localPosition = pos;
    }


    public delegate void LoadFinish(BlGame.GameState.GameStateType St);
	public LoadFinish OnLoadFinish;
}
