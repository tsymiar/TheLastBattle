using System;
using System.Collections.Generic;
using UnityEngine;
using GameDefine;
using JT.FWW.Tools;
using BlGame.GameData;
using BlGame.GameEntity;
using BlGame.Effect;
using BlGame.Resource;

class ProgressBarInterface : MonoBehaviour
{
    public static ProgressBarInterface Instance = null;
    void Awake()
    {
        Instance = this;
    }
    void Update()
    {
        if (isStart == false)
        {
            return;
        }
        //Debug.Log("DeltaTime" + Time.deltaTime);
        //Debug.Log("timeCounter:" + timeCounter);
        setProgress(timeCounter / time);
        
        timeCounter += Time.deltaTime;
      
        if (timeCounter > time)
        {
            timeCounter = time;
            isStart = false;
            if (barType == BarType.BarSkill)
            {
                hideProgressBar();
                if (effectBacktoCity != null)
                {
                    if (effectBacktoCity.obj != null)
                    {
                        GameObject.DestroyImmediate(effectBacktoCity.obj);
                        if (audioCity != null && audioCity.clip != null && audioCity.isPlaying)
                        {
                            //Debug.Log("audioCity stop");
                            audioCity.Stop();
                        }
                    }
                }
            }
        }
    }
    private static float timeCounter = 0.0f;
    private static float time = 3.0f;
    public UISprite progressBar;
    private static bool isStart = false;

    AudioSource audioCity = null;

    private GameObject objEffect = null;

    void OnEnable()
    {
        GameObject obj = transform.FindChild("Postion").transform.FindChild("Foreground").gameObject;
        objEffect = transform.FindChild("Postion").transform.FindChild("load_flash").gameObject;
        if (obj != null)
        {
            progressBar = obj.GetComponent<UISprite>();
            progressBar.fillAmount = 0f;
        }

        startX = progressBar.transform.localPosition.x;
    }

    static GameObject rootNode = null;
    public enum BarType
    {
        BarAbsorb,
        BarSkill,
    }

    public static BarType barType;
    static IEffect effectBacktoCity = null;
    public static void startProgressBar(float totalTime)
    {
        barType = BarType.BarSkill;
        time = totalTime;
        ProgressBarInit();

    }
    public static void startProgressBar(BarType bType,int skillID = 0 )
    {
        Debug.Log("startProgressBar(BarType bType, int skillID)");
        barType = bType;
        if (bType == BarType.BarAbsorb)
        {
            time = 3.0f;
        }
        ProgressBarInit();
    }

    private static void ProgressBarInit()
    {
        if (rootNode == null)
        {
            ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate("Guis/Play/OccupyProcessbar", ResourceType.PREFAB);
            GameObject obj = objUnit.Asset as GameObject;
            rootNode =  GameObject.Instantiate(obj) as GameObject; 
            rootNode.transform.parent = GameMethod.GetUiCamera.transform;
            rootNode.transform.localPosition = Vector3.zero;
            rootNode.transform.localScale = Vector3.one;
        }
        isStart = true;
        rootNode.SetActive(true);
        timeCounter = 0;
    }

    public static void hideProgressBar()
    {
        //Debug.Log("hideProgressBar");
        if (rootNode != null)
        {
            rootNode.SetActive(false);
        }
    }
    //progress 0~1
    public void setProgress(float progress)
    {
        //Debug.Log("setProgress");
        progressBar.fillAmount = progress; 
        SetEffectPos();
    }

    private float startX = 0f;
    void SetEffectPos()
    {
        //Debug.Log("setEffectPos");
        Vector3 pos = new Vector3(progressBar.fillAmount * progressBar.width + startX, progressBar.transform.localPosition.y, progressBar.transform.localPosition.z - 50);
        objEffect.transform.localPosition = pos;
    }

}

