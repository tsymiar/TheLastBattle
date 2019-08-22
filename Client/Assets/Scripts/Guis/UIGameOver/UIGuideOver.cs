using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using System.Collections.Generic;
using JT.FWW.Tools;
using JT.FWW.GameData;
using System.Linq;
using GameDefine;
using BlGame.GuideDate;
using BlGame.Resource;

public class UIGuideOver : MonoBehaviour
{


    private float timeLimit = 0f;
    private float showTime = 10f;
    private float timeStart = 0f;

    static GameObject objUI = null;

    ButtonOnPress btnPress;

    string path = "effect/ui_effect/guide_congratulation";

    void Awake() {
       // objUI.gameObject.SetActive(false);
        BaseDaBomb.StartEffectEvent += StartGuideOver;
        btnPress = transform.FindChild("Position").GetComponent<ButtonOnPress>();
    }

    void PressOver(int ie, bool Press) { 
        NotifyGameOver();
    }

    void NotifyGameOver() {
        timeLimit = 0f;
        SceneGuideTaskManager.Instance().SetHasFinishedAllGuide(true);
        CGLCtrl_GameLogic.Instance.EmsgToss_FinishAllGuideToLobby();
        if (UINewsGuide.Instance != null) {
            LoadUiResource.DestroyLoad(SceneGuideTaskManager.guideUiPath);
        }
    }

    void OnDisable() {
        BaseDaBomb.StartEffectEvent -= StartGuideOver;
        btnPress.RemoveListener(PressOver);
    }

    void StartGuideOver(float time) {
        timeLimit = time;
        timeStart = Time.time;
        hasShowEffect = false;
    }

    bool hasShowEffect = false;

    void Update() {
        if (timeLimit > 0f) {
            float timeBetween = Time.time - timeStart;
            if ((timeBetween > timeLimit) && !hasShowEffect)
            {
                LoadUiResource.LoadRes(transform, path);
                hasShowEffect = true;
            }
            else if ((timeBetween > timeLimit +0.5f) && timeBetween < (timeLimit + showTime+0.5f) && (!objUI.activeInHierarchy))
            {
                objUI.SetActive(true);
            }
            else if (timeBetween > (timeLimit + showTime))
            { 
                timeLimit = 0f;
                NotifyGameOver();
            }
        }
    }

    void OnEnable() {
        btnPress.AddListener(PressOver);
    }

    public static void LoadGuideOver()
    {       
        //GameObject obj = GameObject.Instantiate(Resources.Load(GameConstDefine.GuideOverUi)) as GameObject;

        ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.GuideOverUi, ResourceType.PREFAB);
        GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
           
        obj.transform.parent = UINewsGuide.Instance.transform;
        obj.transform.localPosition = Vector3.zero;
        obj.transform.localScale = Vector3.one;
        objUI = obj.transform.FindChild("Position").gameObject;
        objUI.SetActive(false);

        if (SceneGuideTaskManager.Instance().sceneGuideTask != null && SceneGuideTaskManager.Instance().sceneGuideTask.currentTask != null)
        {
            SceneGuideTaskManager.Instance().sceneGuideTask.currentTask.CleanAllTask();
        }
    }
}
