using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using System;
using System.Collections.Generic;
using JT.FWW.Tools;

public class AltarManager : MonoBehaviour
{

    public static AltarManager Instance
    {
		private set;
		get;
	}

	//List<AltarInHead> headList = new List<AltarInHead>();
	AltarInHead []headList;
	void Awake()
	{
        if (Instance == null)
        {
			Instance = this;
		}
	
		//for(int i = 0; i < 6; i++)
		headList = GetComponentsInChildren<AltarInHead>();

	}
    void OnDisable()
    {
        Instance = null;
        for (int i = 0; i < headList.Length; i++)
        {
            DestroyImmediate(headList[i].targetEntity);
        }
    }
    private AltarInHead GetAsignedAltarHead(Ientity entity)
    {

        for (int i = 0; i < headList.Length; i++)
        {
            if (headList != null && headList[i] != null && headList[i].targetEntity != null)
                if (headList[i].targetEntity == entity.RealEntity)
                    return headList[i];
        }
        return null;
    }
    public void DelAltarHead(int entityID, int type)
    {
        MapObjConfigInfo configInfp = ConfigReader.MapObjXmlInfoDict[entityID];
        if (configInfp == null)
            return;
        AltarInHead head = GetAsignedAltarHead(entityID);
        if (head == null)
        {
            Debug.LogError("error:" + head.name);
            return;
        }
        head.DelHead();
    }

    private AltarInHead GetAsignedAltarHead(int id)
    {
        for (int i = 0; i < headList.Length; i++)
        {
            if (headList != null && headList[i] != null && headList[i].name != null)
                if (Convert.ToInt32(headList[i].name) == id)
                {
                    return headList[i];
                }
        } return null;
    }
    public void ShowAltarHead(int entityID,int type)
    {
        MapObjConfigInfo configInfp = ConfigReader.MapObjXmlInfoDict[entityID];
        if (configInfp == null)
            return;
        //int index = configInfp.un32ObjIdx;
        //int camp = configInfp.n32Camp;
        //UInt64 sGUID = CTools.MakeGUID((JT.FWW.GameData.EObjectType)type, (ulong)index);
        AltarInHead head = GetAsignedAltarHead(entityID);

        if (head == null)
        {
            Debug.LogError("AltarHead error:" + entityID);
            return;
        }
        head.CreateHead(type);
    }

    public void ShowAltarHead(int type, Ientity entity)
    {
        if (entity == null || entity.realObject == null || entity.RealEntity == null)
            return;
        if (!entity.IsSameCamp(PlayerManager.Instance.LocalPlayer.EntityCamp))
            return;
        AltarInHead head = GetAsignedAltarHead(entity);
        if (head == null)
            return;
        head.CreateHead(type);
        //UIAltarInHead.Instance.SetCurrHeadIcon (type,entity.realObject.transform);
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
