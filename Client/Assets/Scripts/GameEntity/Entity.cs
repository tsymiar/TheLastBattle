using GameDefine;
using UnityEngine;
using JT.FWW.Tools;
using JT.FWW.GameData;

using BlGame.GameEntity;
using System.Collections;
using System.Collections.Generic;
using BlGame.Resource;
using System;

[System.Serializable]
public class EntityAttrubte
{
    public float Hp;
    public float HpMax;
    public float Mp;
    public float MpMax;
    public float Speed;
    public float PhyAtk;
    public float MagAtk;
    public float PhyDef;
    public float MagDef;
    public float AtkSpeed;
    public float AtkDis;
    public float HpRecover;
    public float MpRecover;
    public float RebornTime;
    public void AttribbuteUpdate(Ientity entity)
    {
        Hp = entity.Hp;
        HpMax = entity.HpMax;
        Mp = entity.Mp;
        MpMax = entity.MpMax;
        Speed = entity.EntityFSMMoveSpeed;

        PhyAtk = entity.PhyAtk;
        MagAtk = entity.MagAtk;
        PhyDef = entity.PhyDef;
        MagDef = entity.MagDef;

        AtkSpeed = entity.AtkSpeed;
        AtkDis = entity.AtkDis;

        HpRecover = entity.HpRecover;
        MpRecover = entity.MpRecover;
        RebornTime = entity.RebornTime;
    }
};
public class Entity : MonoBehaviour
{
    //获取Transform
    public Transform GetTransform()
    {
        if (mTransform == null)
        {
            mTransform = this.transform;
        }
        return mTransform;
    }    

	public Transform objAttackPoint
	{
		private set;
		get;
	}
    public Transform objBuffPoint
    {
        private set;
        get;
    }
    public Transform objPoint
    {
        private set;
        get;
    }

	public Ientity SyncEntity {
		set;
		get;
	}	

	public CharacterController Controller {
		set;
		get;
	}

    public NavMeshAgent NavAgent
    {
        private set;
        get;
    }

    public OptimizeDynamicModel dynModel;

    public EntityCampType CampType;
	public string FSMStateName;
    public EntityType Type;
    public ENPCCateChild NPCCateChild;
    public EntityAttrubte EntityAttribute;
    public int sGUID;
    public int sMasterGuid;
    public Transform mTransform = null;
    
	private GameObject furyEffect = null;

    public GameObject focoEffectStart
    {
        private set;
        get;
    }

    public GameObject focoEffectSuccess
    {
        private set;
        get;
    }

    public virtual void Awake()
    {
        NavAgent = this.transform.GetComponent<NavMeshAgent>();
        objAttackPoint = transform.FindChild("hitpoint");
        objBuffPoint = transform.FindChild("buffpoint");
        objPoint = transform.FindChild("point");
        if (objAttackPoint == null || objBuffPoint == null || objPoint == null)
        {
            //Debug.LogError("Entity  hitpoint or buffpoint or point is null");
        }
        EntityAttribute = new EntityAttrubte();
    }



	// Use this for initialization
    public virtual void Start() 
    {
		if (SyncEntity != null && SyncEntity.entityType == EntityType.Player) {
			if (this.animation == null)
			{
				return;
			}
			List<string> aniList = ConfigReader.HeroXmlInfoDict[SyncEntity.NpcGUIDType].n32RandomAttack;
			foreach(string ani in aniList)
			{
				if (animation.GetClip(ani))
				{
					AnimationEvent skillEvent = new AnimationEvent();
					skillEvent.time = getAnimationTime(ani);
					skillEvent.functionName = "OnAttackEnd";
					animation.GetClip(ani).AddEvent(skillEvent);
				}

			}

		}
	}

	public void OnAttackEnd()
	{
		if (this.animation == null) {
			return;
		}

        if(this.SyncEntity == null || this.SyncEntity.FSM == null || this.SyncEntity.FSM.State != BlGame.FSM.FsmState.FSM_STATE_RELEASE)
        {
            return;
        }

		int id = UnityEngine.Random.Range (0, ConfigReader.HeroXmlInfoDict [SyncEntity.NpcGUIDType].n32RandomAttack.Count);
		string aniName = ConfigReader.HeroXmlInfoDict [SyncEntity.NpcGUIDType].n32RandomAttack [id];


        if (Type == EntityType.Player && animation.IsPlaying(aniName))
            animation.Stop();

        PlayerAnimation(aniName);
	}
	
	public void SetAttackAnimationLoop(bool b)
	{
		if (animation == null)
		{
			return;
		}

       if (SyncEntity.entityType == EntityType.Player)
        {
            List<string> aniList = ConfigReader.HeroXmlInfoDict[SyncEntity.NpcGUIDType].n32RandomAttack;
            foreach (string ani in aniList)
            {
                AnimationState aSt = animation[ani];
                if (aSt != null)
                {
                    if (b == true)
                    {
                        aSt.wrapMode = WrapMode.Loop;
                    }
                    else
                    {
                        aSt.wrapMode = WrapMode.Once;
                    }
                }
            }
        }
        else
        {
            if (b == true)
            {
                AnimationState state = animation["attack"];
                state.wrapMode = WrapMode.Loop;
            }
            else
            {
                AnimationState state = animation["attack"];
                state.wrapMode = WrapMode.Once;
            }
        }
        
	}

	/// <summary>
	/// 设置移动速度播放缩放
	/// </summary>
	/// <param name="spd"></param>
	public void SetMoveAnimationSpd(float spd)
	{
		if (animation == null)
		{
			return;
		}
		AnimationState aState = animation["walk"];
		if (aState != null)
		{
			aState.speed = spd;
		}
	}

	/// <summary>
	/// 设置攻击速度
	/// </summary>
	/// <param name="spd">Spd.</param>
	public void SetAttackAnimationSpd(float spd)
	{
        if (animation == null || SyncEntity == null)
		{
			return;
		}
 		if (SyncEntity.entityType == EntityType.Player) {
			List<string> aniList = ConfigReader.HeroXmlInfoDict[SyncEntity.NpcGUIDType].n32RandomAttack;
			foreach(string ani in aniList)
			{
				AnimationState aSt = animation[ani];
				if (aSt != null)
				{
					aSt.speed = spd;
				}
			}
		} else {
			AnimationState aState = animation["attack"];
			if (aState != null)
			{
				aState.speed = spd;
			}
		}
		//spd = spd;
		
	}

	/// <summary>
	/// Raises the destroy collider event.
	/// </summary>
    public void OnDestroyCollider()
    {
        if (NavAgent != null)
        {
            DestroyImmediate(NavAgent);
        }
        if(Controller != null)
        {
            DestroyImmediate(Controller);
        }
    }

	// Update is called once per frame
	void Update ()
    {
		if (SyncEntity != null) 
        {
            SyncEntity.OnUpdate();
            EntityAttribute.AttribbuteUpdate(SyncEntity);            
		}
	}
    public void CrossFadeSqu(string name)
    {
        animation.CrossFadeQueued(name);
    }

    public void PlayerAnimation(string name)
    {
        if (name == "" || name == "0")
        {
            return;
        }
        if (this.animation == null)
        {
            return;
        }

        animation.CrossFade(name);
    }

    public void PlayerIdleAnimation()
    {
        if (this.animation == null)
        {
            return;
        }

        PlayerAnimation("idle");
        this.animation.PlayQueued("free");
    }

	public void PlayerFreeAnimation()
    {
        if (this.animation == null)
        {
            return;
        }

        PlayerAnimation("free");
	}

	public void PlayerRunAnimation(){
		if (this.animation == null) {
			return;
		}
        PlayerAnimation("walk");
	}

    public void PlayeAttackAnimation()
    {
        if (this.animation == null)
        {
            return;
        }

        string aniName = "attack";
        if (this.SyncEntity!=null && this.SyncEntity.entityType == EntityType.Player)
        {
            int id = UnityEngine.Random.Range(0, ConfigReader.HeroXmlInfoDict[SyncEntity.NpcGUIDType].n32RandomAttack.Count);
            aniName = ConfigReader.HeroXmlInfoDict[SyncEntity.NpcGUIDType].n32RandomAttack[id];
        }

        if (Type == EntityType.Player && animation.IsPlaying(aniName))
            animation.Stop();

        PlayerAnimation(aniName);
    }

	public void PlayerDeadAnimation()
	{
		if (this.animation == null) {
			return;
		}
        this.animation.cullingType = AnimationCullingType.AlwaysAnimate;
        PlayerAnimation("death");
	}


	public float getAnimationTime(string name)
	{
		AnimationClip clip = animation.GetClip(name);
		if(clip != null)
		{
			return clip.length;
		}
		return 0.0f;
	}

    private void OnTriggerEnter(Collider other)
    {
        if (this.SyncEntity == null || !(other is SphereCollider))
        {
            return;
        }
        Iselfplayer player = PlayerManager.Instance.LocalPlayer;
        if (this.SyncEntity.GameObjGUID != player.GameObjGUID)          //当前ENTITY 不是玩家
        {
            return;
        }
        Entity entity = other.gameObject.GetComponent<Entity>();
        if(entity == null || entity.SyncEntity == null)                 //进入对象的entity为空
        {
            return;
        }
        if (player.AbsorbMonsterType[0] == 0 && player.AbsorbMonsterType[1] == 0)    //没有附身对象
        {
            return;
        }
        if (entity.SyncEntity.EntityCamp != player.EntityCamp || entity.SyncEntity.entityType != EntityType.Building            //如果进入的不是友方祭坛、
            || entity.SyncEntity.NPCCateChild != ENPCCateChild.eNPCChild_BUILD_Altar)
        {
            return;
        }
        BlGame.AudioManager.Instance.PlayEffectAudio(player.GetAltarClip());
        EventCenter.Broadcast(EGameEvent.eGameEvent_PlayerEnterAltar);
        if (UIAltarSelect.Instance == null)
        {
            BlGameUI.Instance.OnOpenUI(GameConstDefine.AltarSoldierSelect);
            UIAltarSelect.Instance.OnTriggerAltar(entity.SyncEntity);
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if(UIAltarSelect.Instance == null)
        {
            return;
        }
        if (this.SyncEntity == null || !(other is SphereCollider))
        {
            return;
        }
        Iselfplayer player = PlayerManager.Instance.LocalPlayer;
        if (this.SyncEntity.GameObjGUID != player.GameObjGUID)          //当前ENTITY 不是玩家
        {
            return;
        }
        Entity entity = other.gameObject.GetComponent<Entity>();
        if (entity == null || entity.SyncEntity == null)                 //退出对象的entity为空
        {
            return;
        }
        if (UIAltarSelect.Instance.TrAltar == entity.SyncEntity)
        {
            Destroy(UIAltarSelect.Instance.gameObject);
        }
    }
 
    public void UpdateFuryEffect() {
        if (SyncEntity == null)
            return;
        if (EntityType.Player != SyncEntity.entityType)
        {
            return;
        }
        Iplayer player = (Iplayer)SyncEntity;
        if (EFuryState.eFuryRunState == player.FuryState) {
            LoadFuryEffect();
        }
        else {
            DestroyFuryEffect();
        }
    }

    private void LoadFuryEffect() 
    {
        if (null == furyEffect) 
        {
            //furyEffect = GameObject.Instantiate(Resources.Load(GameDefine.GameConstDefine.FuryGasExplosionEffect)) as GameObject;
            ResourceUnit furyEffectUnit = ResourcesManager.Instance.loadImmediate(GameDefine.GameConstDefine.FuryGasExplosionEffect, ResourceType.PREFAB);
            furyEffect = GameObject.Instantiate(furyEffectUnit.Asset) as GameObject;
           
            
            furyEffect.transform.parent = transform;
            furyEffect.transform.localScale = Vector3.one;
            furyEffect.transform.localPosition = Vector3.zero; 
        }
    }

    private void DestroyFuryEffect() {
        if (null != furyEffect) {
            GameObject.DestroyImmediate(furyEffect);                      
            furyEffect = null;
            //System.GC.Collect(); 
        }
    }

    public void AddFocoEnergiaStart()
    {
        if (null == focoEffectStart) {
           focoEffectStart = LoadUiResource.LoadRes(transform, GameDefine.GameConstDefine.FuryStartLongPressEffect);
        }
    }

    public void DestroyFocoEnergiaStart()
    {
        if (null != focoEffectStart)
        {
            LoadUiResource.DestroyLoad(GameDefine.GameConstDefine.FuryStartLongPressEffect);
            focoEffectStart = null;
        }
    }

    /// <summary>
    /// 重载手指
    /// 或者鼠标点中消息
    /// </summary>
    private void OnFingerClick()
    {     
        if (this.SyncEntity != null)
        {
            this.SyncEntity.OnEntityFingerClick();
        }
    }


    private Dictionary<TrailRenderer, float> trailTime = new Dictionary<TrailRenderer, float>();

    public void HideTrail()
    {
        foreach (Transform child in objBuffPoint)
        {
            if (child.tag == "trail")
            {
                ParticleSystem[] particles = child.GetComponentsInChildren<ParticleSystem>(true);
                foreach (ParticleSystem part in particles)
                {
                    part.Clear();
                    part.gameObject.SetActive(false);
                }

                TrailRenderer[] trailRenders = child.GetComponentsInChildren<TrailRenderer>();
                foreach (TrailRenderer trail in trailRenders)
                {
                    trailTime[trail] = trail.time;
                    trail.time = -10;
                }
            }
        }

       
    }

    public void ShowTrail()
    {
        Invoke("ShowTrailDelay", 1.0f);
    }

    public void ShowTrailDelay()
    {
        foreach (Transform child in objBuffPoint)
        {
            if (child.tag == "trail")
            {
                ParticleSystem[] particles = child.GetComponentsInChildren<ParticleSystem>(true);
                foreach (ParticleSystem part in particles)
                {
                    part.gameObject.SetActive(true);
                }

                TrailRenderer[] trailRenders = child.GetComponentsInChildren<TrailRenderer>();
                foreach (TrailRenderer trail in trailRenders)
                {
                    trail.time = trailTime[trail];
                }
            }
        }

        trailTime.Clear();
    }
}