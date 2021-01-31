using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic; 
using BlGame.GameEntity;
using BlGame.GameData;
using BlGame.GuideDate;
using BlGame.Resource;

public enum XueTiaoType{
	PlayerTypeRed,
	PlayerTypeBlue,
	SoldierTypeRed,
	SoldierTypeBlue,
	BuildingTypeRed,
	BuildingTypeBlue,
	Monster,
}

public class XueTiaoManager: MonoBehaviour 
{ 
	
	#region 血条路径
    private static string pathPlayerXuetiao = "HeroLifePlate{0}";
    private static string pathBuildingXuetiao = "TowerLifePlate{0}";
    private static string pathSummonedXuetiao = "Summoned{0}";
    private static string pathSoldierXuetiao = "SoldierLifePlate{0}";
    public static string pathMonster = "CreepsLifePlate";

    public static string xueTiaoName = "XueTiao_";
	#endregion


	public static XueTiaoManager Instance{
		private set;
		get;
	}

	void OnEnable(){
		if(Instance == null){
			Instance = this;
		}
	}

	private string GetXueTiaoPrefabPath(Ientity entity){
        string XuetiaoColor = "Green";

        if (PlayerManager.Instance.LocalPlayer == null || PlayerManager.Instance.LocalPlayer.realObject == null)
        {
            int campEntity = (int)entity.EntityCamp % 2;
            int playerCamp = (int)PlayerManager.Instance.LocalAccount.GameUserSeat % 2;
            if (playerCamp != campEntity) {
                XuetiaoColor = "Red";
            }
        }
        else if (entity.EntityCamp == EntityCampType.CampTypeBad || (PlayerManager.Instance.LocalPlayer != null && entity.EntityCamp != PlayerManager.Instance.LocalPlayer.EntityCamp))
        {            
            XuetiaoColor = "Red";
        }
        
        if (PlayerManager.Instance.LocalAccount.ObType == GameDefine.ObPlayerOrPlayer.PlayerObType && entity.EntityCamp != PlayerManager.Instance.LocalAccount.EntityCamp)
        {
            XuetiaoColor = "Red";
        }
        string path = "";
        switch (entity.entityType)
        { 
            case EntityType.Building:
                path = String.Format(pathBuildingXuetiao, XuetiaoColor);
                break;
            case EntityType.Player:
                path = String.Format(pathPlayerXuetiao, XuetiaoColor);
                break;
            case EntityType.Soldier:
            case EntityType.AltarSoldier:
                path = String.Format(pathSoldierXuetiao, XuetiaoColor);
                //Debug.LogError("path = " + path);
                break;
            case EntityType.Monster:
                path = pathMonster;
                break;
        }
        if (entity.NpcGUIDType == 21017 || entity.NpcGUIDType == 21025 || entity.NpcGUIDType == 21024)
        { //新手引导假的英雄
            path = String.Format(pathPlayerXuetiao, XuetiaoColor);
            SceneGuideTaskManager.Instance().AddFakeHero(entity);
        }
        else if (entity.NPCCateChild == ENPCCateChild.eNPCChild_BUILD_Summon)
        {
            path = String.Format(pathSummonedXuetiao, XuetiaoColor);
        }
        return GameDefine.GameConstDefine.GuisPlay + path;
	}

	public XueTiaoUI CreateXueTiao(Ientity entity){
        string path = GetXueTiaoPrefabPath(entity);
		XueTiaoUI xueTiao = LoadXueTiaoPrefab(entity,path);
		xueTiao.Init(entity);
		xueTiao.ResetXueTiaoValue();
		xueTiao.UpdatePosition(entity.realObject.transform);
		xueTiao.gameObject.transform.parent = transform;
        xueTiao.transform.localScale = Vector3.one;
		xueTiao.gameObject.transform.name = xueTiaoName + entity.ModelName;
		return xueTiao;
	}

	public XueTiaoUI LoadXueTiaoPrefab(Ientity entity,string path)
    {        
        //ResourceUnit pathLoadUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
        //GameObject pathLoad = pathLoadUnit.Asset as GameObject;
        GameObject obj = GameObjectPool.Instance.GetGO(path);

//         if (pathLoad == null)
//             Debug.LogError("error Not found  XueTiaoUI =  null");
//         GameObject obj = GameObject.Instantiate(pathLoad) as GameObject;
        if (obj == null)
            Debug.LogError("obj = null");
		XueTiaoUI xueTiao = obj.GetComponent<XueTiaoUI>();
        xueTiao.mResName = path;

		return xueTiao;
	} 
}
