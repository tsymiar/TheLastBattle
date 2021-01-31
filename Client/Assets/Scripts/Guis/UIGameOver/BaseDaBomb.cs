using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using GameDefine;
using System;
using BlGame.Resource;
using BlGame;

public class BaseDaBomb : MonoBehaviour {

    public static BaseDaBomb Instance
    {
        set;
        get;
    }

    public static Action<float> StartEffectEvent;

    public GameObject RotateAroundTarget;

    void Awake()
    {
        Instance = this;
    }

	// Use this for initialization
	void Start () {
	
	}
    private Ientity Entity;
    Vector3 posDest = Vector3.zero;
    Vector3 move ;//baocun 
    bool isBombBase = false;
//    public void SetBaseBomb(bool isBomb, Ientity entity, int npcID, UInt32 mapID)
    public void SetBaseBomb(bool isBomb, Ientity entity,UInt32 mapID)
    {
        Entity = entity;
        isBombBase = isBomb;
        MapInfo map = MapLoadConfig.Instance.GetMapInfo(mapID);

        if(map != null)
        {
            GameObject ta = GameObject.Find(map.mACameraPos);
            GameObject tb = GameObject.Find(map.mBCameraPos);

            if (ta != null && tb != null)
            {
                //设置移动座标点
                SetMapPos(entity, ta.transform.position, tb.transform.position);

                //设置旋转特效目标点
//                 if (PlayerManager.Instance.LocalPlayer.EntityCamp == entity.EntityCamp)
//                 {
//                     if (PlayerManager.Instance.LocalPlayer.EntityCamp == EntityCampType.CampTypeA)
//                         RotateAroundTarget = ta;
//                     else
//                         RotateAroundTarget = tb;
//                 }
            }
        }
    }

    Vector3 disA = Vector3.zero;
    Vector3 disB = Vector3.zero;
    Vector3 targetA = Vector3.zero;
    Vector3 targetB = Vector3.zero;
    void SetMapPos(Ientity homeBase,Vector3 PosA, Vector3 PosB)
    {
        if (homeBase.IsSameCamp(EntityCampType.CampTypeA))
        {
            disA = PosA - PlayerManager.Instance.LocalPlayer.objTransform.position;
            targetA = Camera.main.transform.position + disA;
            move = targetA;
        }
        else 
        {
            disB = PosB - PlayerManager.Instance.LocalPlayer.objTransform.position;
            targetB = Camera.main.transform.position + disB;
            move = targetB;
        }

        if (PlayerManager.Instance.LocalPlayer.EntityCamp == EntityCampType.CampTypeA)
        {
            posDest = targetA;
        }
        else

        {
            posDest = targetB;
        } 
    }
   float startTime = 0f;
   Vector3 res = Vector3.zero;
	// Update is called once per frame
	void Update () {
        if (isBombBase)
        {
            if (startTime == 0f) {
                startTime = Time.time;
            }
            if (move == transform.position && Time.time - startTime >= 1f)
            {
                BlGame.AudioManager.Instance.StopBgAudio();
                AudioSource soul = null;
                if (move == posDest)
                {
                    /*
                    Animator ani = this.gameObject.GetComponent<Animator>();
                    ani.enabled = true;
                     */
					//soul = BlGame.AudioManager.Instance.PlayEffectAudio(Resources.Load(AudioDefine.PATH_GAME_OVER_FAILE) as AudioClip);

                    ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_GAME_OVER_FAILE, ResourceType.ASSET);
                    AudioClip clip = clipUnit.Asset as AudioClip;

                    soul = BlGame.AudioManager.Instance.PlayEffectAudio(clip);
                }
                else
                {
					//soul = BlGame.AudioManager.Instance.PlayEffectAudio(Resources.Load(AudioDefine.PATH_GAME_OVER_VICTORY) as AudioClip);

                    ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_GAME_OVER_VICTORY, ResourceType.ASSET);
                    AudioClip clip = clipUnit.Asset as AudioClip;
                        
                    soul = BlGame.AudioManager.Instance.PlayEffectAudio(clip); 
                }
                isBombBase = false;
                if (StartEffectEvent != null) {
                    StartEffectEvent(4f);
                }
                
                if (Entity != null)
                {
                    GameMethod.SetLoadJianTaBaoZha(Entity,false);

                    Entity.OnEnterDead();
                }
                
                return;
            }
           float time = Vector3.Distance(Camera.main.transform.position, move) / 60f ;
           Camera.main.transform.position = Vector3.SmoothDamp(Camera.main.transform.position, move, ref res, time);

           GameMethod.GetMainCamera.RefreshFrustumPlanes();
            //Camera.main.transform.position = Vector3.Lerp(Camera.main.transform.position, move,Time.deltaTime * time);
       }
   }

    public void ReplaceBuild()
    {
        GameMethod.BuildReplaceObject(Entity);
        int ret = EntityManager.HandleDelectEntity(Entity.GameObjGUID);
        if (ret != 0)
        {
            Debug.LogError("OnEnterDead HandleDelectEntity:" + Entity.GameObjGUID + "failed");
        }
    }

}
