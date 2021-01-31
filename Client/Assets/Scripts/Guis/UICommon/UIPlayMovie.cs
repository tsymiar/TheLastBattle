using UnityEngine;
using System;
using System.Collections;
using GameDefine;
using BlGame.Resource;

public class UIPlayMovie : MonoBehaviour
{
    const string prefabPath = "Game/PlayMovie";
    private static string mName = "";
    private static Color mCol = Color.black;
    private static int mMode = 3;// FullScreenMovieControlMode.Hidden
    private static int mScal = 3;//FullScreenMovieScalingMode.Fill; 
    GameObject objMark = null;

	void OnEnable() {
        if(string.IsNullOrEmpty(mName))
            return;

            CreateMark();
            //if(Application.platform == RuntimePlatform.Android || Application.platform == RuntimePlatform.IPhonePlayer)

#if UNITY_STANDALONE_WIN || UNITY_EDITOR
#else 
            Handheld.PlayFullScreenMovie(mName, mCol, (FullScreenMovieControlMode)mMode, (FullScreenMovieScalingMode)mScal);
#endif

            DestroyMark();
            DestroyObject(gameObject); 	
    } 

    public static void PlayMovie(string name, Color col, int mode, int scal ) {
        mName = name;
        mCol = col;
        mMode = mode;
        mScal = scal; 
        //ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(prefabPath, ResourceType.PREFAB);        
          
        #if UNITY_STANDALONE_WIN || UNITY_EDITOR
        //return (float)m_HiPerfTimer.GetDuration(n64StopTime, n64Start);
        #else
            GameObject.Instantiate( Resources.Load(prefabPath));
        #endif
    }

    void CreateMark()
    {
        if (UICamera.currentCamera == null )
            return;
        ResourceUnit objMarkUnit = ResourcesManager.Instance.loadImmediate("Guis/Over/superwan", ResourceType.PREFAB);
        objMark = GameObject.Instantiate(objMarkUnit.Asset) as GameObject;
        objMark.transform.parent = GameMethod.GetUiCamera.transform;
        objMark.transform.localScale = Vector3.one;
        objMark.transform.localPosition = Vector3.zero;
        UISprite sprite = objMark.transform.FindChild("Background").GetComponent<UISprite>();
        TweenAlpha.Begin(sprite.gameObject, 0f, 1f); 
    }

    void DestroyMark()
    {
        if (objMark != null)
        {
            DestroyObject(objMark);
        }
        objMark = null;
    }


}