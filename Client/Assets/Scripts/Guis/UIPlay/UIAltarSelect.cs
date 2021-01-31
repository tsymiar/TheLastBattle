using UnityEngine;
using System.Collections;
using System.Collections.Generic;

using System;
using BlGame.GameEntity;
using JT.FWW.Tools;
using GameDefine;
using BlGame.Resource;
using BlGame.Ctrl;

public class UIAltarSelect : MonoBehaviour {

    public static UIAltarSelect Instance
    {
        private set;
        get;
    }

    private List<ButtonOnPress> SoldierSelectBtn;
    private List<UISprite> AltarHeadPhoto;

    enum AltarSelectBtn 
    { 
        SelectFirst,
        SelectSecond,
    }
    int cuttSolt = 0;
    /// <summary>
    /// 触发的祭坛
    /// #############################
    /// </summary>
    public Ientity TrAltar
    {
        private set;
        get;
    }

    void OnEnable()
    {
        Instance = this;
        SoldierSelectBtn = new List<ButtonOnPress>();
        AltarHeadPhoto = new List<UISprite>();
        ButtonOnPress mBtnSoldier1 = this.transform.FindChild("HeadShow/SoldierSelect/Soldier1").GetComponent<ButtonOnPress>();
        SoldierSelectBtn.Add(mBtnSoldier1);
        UIGuideCtrl.Instance.AddUiGuideEventBtn(mBtnSoldier1.gameObject);
        SoldierSelectBtn.Add(this.transform.FindChild("HeadShow/SoldierSelect/Soldier2").GetComponent<ButtonOnPress>());
        AltarHeadPhoto.Add(this.transform.FindChild("HeadShow/LastSelect/HalfPhoto").GetComponent<UISprite>());
        AltarHeadPhoto.Add(SoldierSelectBtn[0].transform.FindChild("HalfPhoto").GetComponent<UISprite>());
        AltarHeadPhoto.Add(SoldierSelectBtn[1].transform.FindChild("HalfPhoto").GetComponent<UISprite>());
        for (int id = 0; id < SoldierSelectBtn.Count; id++)
        {
            SoldierSelectBtn[id].AddListener(id, OnSoldierSelectFunc);
        }
        ButtonOnPress BtnClose = this.transform.FindChild("CtrlBtn").GetComponent<ButtonOnPress>();
        BtnClose.AddListener(OnUiCloseFunc);
        EventCenter.AddListener<Iplayer>(EGameEvent.eGameEvent_HeroDeathTime, ReMoveCurrGameObj);//死亡后重置祭坛
        EventCenter.AddListener<Iplayer>(EGameEvent.eGameEvent_HeroBackTown, ReMoveCurrGameObj);//回城后重置祭坛
        mStartEffect = false;
        mSetupEffect = false;
        mEffectTime = 0f;
    }
    private const string path = "effect/ui_effect/Selectportrait";
    private const string Curtpath = "effect/ui_effect/nowselection";
    public void Destoys()
    {        
        //GameObject obj = GameObject.Instantiate(Resources.Load(path)) as GameObject;
        
        ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
        GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;

        obj.gameObject.transform.parent = AltarHeadPhoto[cuttSolt].transform;
        obj.transform.localScale = Vector3.one;
        obj.transform.localPosition = Vector3.one;

        mStartEffect = true;
        AltarHeadPhoto[0].gameObject.SetActive(true);
        if (TrAltar.EntityHoldNpcType != 0)
            AltarHeadPhoto[0].spriteName = ConfigReader.GetNpcInfo(TrAltar.EntityHoldNpcType).HeadPhoto.ToString();
        
    }
    void ReMoveCurrGameObj(Iplayer player)
    {
        if (player == PlayerManager.Instance.LocalPlayer)
            RemoveAltar();
    }
    public void RemoveAltar()
    {
        if (this != null)
            DestroyImmediate(transform.parent.gameObject);
    }
    private void ShowAltarSelectPic()
    {
        Iselfplayer pl = PlayerManager.Instance.LocalPlayer;
        VirtualStickUI.Instance.SetVirtualStickUsable(false);
        for (int i = 0; i < 2; i++) {
            if (pl.AbsorbMonsterType[i] != 0)
            {
                AltarHeadPhoto[1 + i].gameObject.SetActive(true);
                AltarHeadPhoto[1 + i].spriteName = ConfigReader.GetNpcInfo((int)pl.AbsorbMonsterType[i]).HeadPhoto.ToString();
            }
        }
        if (TrAltar.EntityHoldNpcType == 0 )
        {
            if (AltarData.AllAltarDic != null && AltarData.AllAltarDic.Count != 0 && AltarData.AllAltarDic.ContainsKey(TrAltar.MapObgId))
            {
                TrAltar.EntityHoldNpcType = AltarData.AllAltarDic[TrAltar.MapObgId];
            }else
                return;
        }
		AltarHeadPhoto[0].gameObject.SetActive(true);
        AltarHeadPhoto[0].spriteName = ConfigReader.GetNpcInfo(TrAltar.EntityHoldNpcType).HeadPhoto.ToString();
    }

    public void OnTriggerAltar(Ientity entity)
    { 
        TrAltar = entity;
        ShowAltarSelectPic();
    }

    void OnDisable()
    {
        VirtualStickUI.Instance.SetVirtualStickUsable(true);
        Instance = null;
    }

    private void OnSoldierSelectFunc(int ie , bool presses)
    {
        if (presses || TrAltar == null)
        {
            return;
        }
        Iselfplayer pl = PlayerManager.Instance.LocalPlayer;
        int npcType = 0;
        cuttSolt = 0;
        if ((AltarSelectBtn)ie == AltarSelectBtn.SelectFirst)
        {
            if (pl.AbsorbMonsterType[0] == 0)
                return;
            npcType = (int)pl.AbsorbMonsterType[0];
            cuttSolt = 1;
        }
        if ((AltarSelectBtn)ie == AltarSelectBtn.SelectSecond)
        {
            if (pl.AbsorbMonsterType[1] == 0)
                return;
            npcType = (int)pl.AbsorbMonsterType[1];
            cuttSolt = 2;
        }
        if (npcType == 0)
        {
            return;
        }

        CGLCtrl_GameLogic.Instance.EmsgToss_ReportAltarStrawSolder((UInt32)npcType, (int)TrAltar.GameObjGUID);
        //if ((AltarSelectBtn)ie == AltarSelectBtn.SelectSecond)
        //{
        //    pl.AbsorbMonsterType1 = 0;
        //}
        //else {
        //    pl.AbsorbMonsterType2 = 0;
        //}
        TrAltar.EntityHoldNpcType = npcType;
        if (TrAltar.EntityHoldNpcType != 0 && TrAltar != null)
		{
            SoldierSelectBtn[0].RemoveListener(OnSoldierSelectFunc);
            SoldierSelectBtn[1].RemoveListener(OnSoldierSelectFunc);
            //Debug.LogError(npcType + "   TrAltar = " + TrAltar + " AltarManager.Instance = " + AltarManager.Instance);
			AltarManager.Instance.ShowAltarHead(npcType,TrAltar);
            Destoys();
		}
        //BlGameUI.Instance.OnDestroyUI(this.transform.parent.gameObject);
    }

    private void OnUiCloseFunc(int ie , bool presses)
    {
        if (presses)
        {
            return;
        }
        RemoveAltar();
        //BlGameUI.Instance.OnDestroyUI(this.gameObject);
    }

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
        if (TrAltar == null)
        {
            DestroyImmediate(this.transform.parent.gameObject);
        }
        else if (mStartEffect)
        {
            mEffectTime += Time.deltaTime;
            if (mEffectTime >= 1.5f)
            {
                RemoveAltar();
            }
            else if (mEffectTime >= 0.5f && !mSetupEffect)
            {
                ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(Curtpath, ResourceType.PREFAB);
                GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;

                AltarHeadPhoto[cuttSolt].gameObject.SetActive(false);
                obj.gameObject.transform.parent = AltarHeadPhoto[0].transform;
                obj.transform.localScale = Vector3.one;
                obj.transform.localPosition = Vector3.one;
                mSetupEffect = true;
            }
        }        
	}

    private bool mStartEffect = false;
    private bool mSetupEffect = false;
    private float mEffectTime = 0f;

}
