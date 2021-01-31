using UnityEngine;
using System.Collections;
using System;
using JT.FWW.Tools;
using BlGame.GameEntity;
using System.Collections.Generic;

public class UISoleSoldier : MonoBehaviour {
    public static UISoleSoldier Instance
    {
        private set;
        get;
    }
    void Awake()
    {
        Instance = this;
    }
    private List<ButtonOnPress> SoldierSelectBtn;
    private List<UISprite> AltarHeadPhoto;

    enum AltarSelectBtn
    {
        SelectFirst,
        SelectSecond,
    }
    void OnEnable()
    {
        
        SoldierSelectBtn = new List<ButtonOnPress>();
        AltarHeadPhoto = new List<UISprite>();
        SoldierSelectBtn.Add(this.transform.FindChild("HeadShow/SoldierSelect/Soldier1").GetComponent<ButtonOnPress>());
        SoldierSelectBtn.Add(this.transform.FindChild("HeadShow/SoldierSelect/Soldier2").GetComponent<ButtonOnPress>());
        AltarHeadPhoto.Add(this.transform.FindChild("HeadShow/LastSelect/HalfPhoto").GetComponent<UISprite>());        
        AltarHeadPhoto.Add(SoldierSelectBtn[1].transform.FindChild("HalfPhoto").GetComponent<UISprite>());
        AltarHeadPhoto.Add(SoldierSelectBtn[0].transform.FindChild("HalfPhoto").GetComponent<UISprite>());
        for (int id = 0; id < SoldierSelectBtn.Count; id++)
        {
            SoldierSelectBtn[id].AddListener(id, OnSoldierSelectFunc);
        }
        ButtonOnPress BtnClose = this.transform.FindChild("CtrlBtn").GetComponent<ButtonOnPress>();
        BtnClose.AddListener(OnUiCloseFunc);
        EventCenter.AddListener<int, int>(EGameEvent.eGameEvent_ResetAbsHead, ResetAbsHead);
        EventCenter.AddListener<string>(EGameEvent.eGameEvent_ResetLockHead, ResetLockHead);
    }
    void ResetLockHead(string head)
    {
        //注释新的头相
        if (head != null)
        {
            AltarHeadPhoto[0].spriteName = head;
        }
        else
        {
            AltarHeadPhoto[0].spriteName = "0";
        }
        DestroySole();
    }
    void ResetAbsHead(int newMonsterID1, int newMonsterID2)
    {
        //注释更新头相
        NpcConfigInfo info = ConfigReader.GetNpcInfo(newMonsterID1);
        if (info != null)
            AltarHeadPhoto[1].spriteName = info.HeadPhoto.ToString();
        info = ConfigReader.GetNpcInfo(newMonsterID2);
        if (info != null)
            AltarHeadPhoto[2].spriteName = info.HeadPhoto.ToString();
        if (this != null)
        DestroySole();
    }
    public void DestroySole()
    {
        DestroyImmediate(this.transform.parent.gameObject);
        VirtualStickUI.Instance.SetVirtualStickUsable(true);
    }
    private void ShowAltarSelectPic()
    {
        Iselfplayer pl = PlayerManager.Instance.LocalPlayer;
        if (pl == null)
            return;
        VirtualStickUI.Instance.SetVirtualStickUsable(false);
        for (int i = 0; i < 2; i++) {
            if (pl.AbsorbMonsterType[i] != 0) { 
                AltarHeadPhoto[1+i].gameObject.SetActive(true);
                AltarHeadPhoto[1 + i].spriteName = ConfigReader.GetNpcInfo((int)pl.AbsorbMonsterType[i]).HeadPhoto.ToString();
            }
        }
        
        AltarHeadPhoto[0].gameObject.SetActive(true);
        AltarHeadPhoto[0].spriteName = ConfigReader.GetNpcInfo(pl.SyncLockTarget.NpcGUIDType).HeadPhoto.ToString();
    }

    public void OnTriggerAltar()
    {
        ShowAltarSelectPic();
    }

    void OnDisable()
    {
        Instance = null;
    }

    private void OnSoldierSelectFunc(int ie, bool presses)
    {
        if (presses)
        {
            return;
        }
        Iselfplayer pl = PlayerManager.Instance.LocalPlayer;
        int npcType = -1;
        if ((AltarSelectBtn)ie == AltarSelectBtn.SelectFirst && pl.Level >= 7)
        {           
            npcType = 1;
            CGLCtrl_GameLogic.Instance.EmsgToss_AskAbsorb(npcType);
        }
        if ((AltarSelectBtn)ie == AltarSelectBtn.SelectSecond)
        {
            npcType = 0;
            CGLCtrl_GameLogic.Instance.EmsgToss_AskAbsorb(npcType);
        }
        DestroySole();
        //BlGameUI.Instance.OnDestroyUI(this.transform.parent.gameObject);
    }

    private void OnUiCloseFunc(int ie, bool presses)
    {
        if (presses)
        {
            return;
        }
        DestroySole();
        //BlGameUI.Instance.OnDestroyUI(this.transform.parent.gameObject);
    }

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }
}
