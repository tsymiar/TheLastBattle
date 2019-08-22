using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BlGame.GameEntity
{
    using BlGame.FSM;
    using JT.FWW.Tools;
    using JT.FWW.GameData;
    using GameDefine;
    using BlGame;
    using BlGame.Effect;
    using BlGame.Resource;

    public class Ientity
    {
        public Ientity(UInt64 sGUID, EntityCampType campType)
        {
            GameObjGUID = sGUID;
            EntityCamp = campType;
            m_pcGOSSI = new CGameObjectSyncInfo();
        }
        public AudioSource absorbSound;

        protected CGameObjectSyncInfo m_pcGOSSI = null;
        public CGameObjectSyncInfo GOSSI
        {
            get
            {
                return m_pcGOSSI;
            }
        }

        /// <summary>
        /// 祭坛持有的Npc type
        /// </summary>
        public int EntityHoldNpcType;

        //资源路径
        public string resPath;

        //Transform组件
        public Transform objTransform = null;

        //血条GameObject 
        public GameObject xueTiaoObj = null;

        //死亡现场(用于是否播放死亡特效)
        public bool deadSpot = false;

        //标示场景里的对象
        public UInt64 GameObjGUID
        {
            get;
            set;
        }

        //能否被锁定
        public bool mCanBeLocked = true;

        public UInt32 ObjTypeID
        {
            get;
            set;
        }

        public float EntityOverplusRemainTime
        {
            set;
            get;
        }

        public float EntityOverplusTotalTime
        {
            set;
            get;
        }

        public int MapObgId
        {
            get;
            set;
        }

        public float StrategyTick
        {
            set;
            get;
        }

        public EntityCampType EntityCamp
        {
            get;
            set;
        }

        public float PhyAtk
        {
            set;
            get;
        }

        public float MagAtk
        {
            set;
            get;
        }

        public float PhyDef
        {
            set;
            get;
        }

        public float MagDef
        {
            set;
            get;
        }

        public float AtkSpeed
        {
            set;
            get;
        }

        public float AtkDis
        {
            set;
            get;
        }

        public float HpRecover
        {
            set;
            get;
        }

        public float MpRecover
        {
            set;
            get;
        }

        public float RebornTime
        {
            set;
            get;
        }
        public float Hp
        {
            set;
            get;
        }

        public float HpMax
        {
            private set;
            get;
        }

        public float Mp
        {
            private set;
            get;
        }

        public float MpMax
        {
            private set;
            get;
        }

        public int Level
        {
            private set;
            get;
        }

        public float Cp
        {
            private set;
            get;
        }

        public float TeamCp
        {
            private set;
            get;
        }

        public float Exp
        {
            private set;
            get;
        }

        public int CooldownReduce
        {
            set;
            get;
        }

        public XueTiaoUI XueTiao
        {
            private set;
            get;
        }

        public EntityType entityType
        {
            get;
            set;
        }

        public bool IfNPC()
        {
            return entityType < EntityType.Player;
        }

        /// <summary>
        /// Npc类型
        /// </summary>
        public int NpcGUIDType
        {
            get;
            set;
        }

        public string ModelName
        {
            get;
            set;
        }

        

        public float ColliderRadius
        {
            get;
            set;
        }

        public ENPCCateChild NPCCateChild
        {
            get;
            set;
        }

        public GameObject realObject
        {
            set;
            get;
        }

        public Entity RealEntity
        {
            set;
            get;
        }

        public EntityFSM FSM
        {
            private set;
            get;
        }

        enum EAsyncState
        {
            eAS_Asynced,
            eAS_Error,
        }

        enum EActionState
        {
            eAS_Free,
            eAS_Run,
        }

        public virtual void OnUpdate()
        {
            if (FSM != null)
            {
                FSM.Execute(this);
            }
        }

        #region State change
        public Vector3 EntityFSMPosition
        {
            set;
            get;
        }

        public Vector3 EntityFSMDirection
        {
            private set;
            get;
        }

        public float EntityFSMMoveSpeed
        {
            private set;
            get;
        }

        public int EntitySkillID
        {
            set;
            get;
        }

        public int EntitySkillModelId
        {
            private set;
            get;
        }

        protected Ientity entitySkillTarget
        {
            get;
            set;
        }
        //变化原因(0:普通伤害;1:BUFF掉血;2:暴击;3:回复)

        public const byte HpUpdateOfCommon = 0;
        public const byte HpUpdateOfBuf = 1;
        public const byte HpUpdateOfCrit = 2;
        public const byte HpUpdateOfRecover = 3;

        /// <summary>
        /// 状态改变
        /// </summary>
        /// <param name="fsm"></param>
        /// <param name="last"></param>
        public void OnFSMStateChange(EntityFSM fsm, float last)
        {
            if (this.FSM != null && this.FSM.StateChange(this, fsm))
            {
                return;
            }

            if (this.FSM == fsm && this.FSM != null && this.FSM.State == FsmState.FSM_STATE_DEAD)
            {
                return;
            }
            if (this.FSM != null)
            {
                this.FSM.Exit(this);
            }

            if (this.FSM != null)
                this.RealEntity.FSMStateName = fsm.ToString();

            this.FSM = fsm;
            StrategyTick = Time.time;
            this.FSM.Enter(this, last);
        }

        public void OnFSMStateChange(EntityFSM fsm)
        {
            if (this.FSM != null && this.FSM.StateChange(this, fsm))
            {
                return;
            }

            if (this.FSM == fsm && this.FSM != null && (this.FSM.State == FsmState.FSM_STATE_DEAD))
            {
                return;
            }

            if (this.FSM != null)
            {
                this.FSM.Exit(this);
            }

            this.FSM = fsm;
            if (this.FSM != null)
                this.RealEntity.FSMStateName = fsm.ToString();
            StrategyTick = Time.time;
            this.FSM.Enter(this, 0.0f);
        }

        /// <summary>
        /// 角色中了束缚Buff
        /// </summary>
        public void OnEntityGetAstrictBuff()
        {

        }

        /// <summary>
        /// 移除buff状态
        /// </summary>
        public void OnEntityRomoveAstrictBuff()
        {

        }
        /// <summary>
        /// 复活
        /// </summary>
        public virtual void OnReborn()
        {
            this.DoReborn();
        }

        protected virtual void DoReborn()
        {

        }

        /// <summary>
        /// Entity移动属性变化
        /// </summary>
        public virtual void OnEntityMoveSpeedChange(int value)
        {
            this.GOSSI.sServerBeginPos = this.GOSSI.sServerSyncPos;
            this.GOSSI.fBeginTime = Time.realtimeSinceStartup;
            this.EntityFSMMoveSpeed = (float)value / 100;
            this.GOSSI.fServerSpeed = this.EntityFSMMoveSpeed;
            if (1 > this.GOSSI.fServerSpeed)
            {
                //本地调试，不要上传！
                //Debug.LogWarning("OnEntityMoveSpeedChange() GO " + GameObjGUID + " Speed " + this.GOSSI.fServerSpeed);
            }
            NpcConfigInfo npcConfig;
            if (ConfigReader.NpcXmlInfoDict.TryGetValue(NpcGUIDType, out npcConfig))
            {
                float speed = value / npcConfig.NpcMoveSpeed * npcConfig.n32BaseMoveSpeedScaling / 1000;
                if (speed == 0)
                {
                    return;
                }
                this.RealEntity.SetMoveAnimationSpd(speed);
            }
        }

        protected int skillState;//技能状态( 1:开始吟唱 2:开始施法(前摇) 3:释放技能)
        protected int isAbsorb;//1是吸附0不是
        public void EntityFSMChangeDataOnPrepareSkill(Vector3 mvPos, Vector3 mvDir, int skillID, Ientity targetID, int isAbsorbSkill = 0)
        {
            EntityFSMPosition = mvPos;
            EntityFSMDirection = mvDir;
            EntitySkillID = skillID;
            entitySkillTarget = targetID;
            isAbsorb = isAbsorbSkill;
        }

        /// <summary>
        /// 强制位移
        /// </summary>
        public void EntityFSMChangeDataOnForceMove(int skId)
        {
            EntitySkillModelId = skId;
        }

        /// <summary>
        /// EntityFSMChangeDataOnSing
        /// </summary>
        /// <param name="mvPos"></param>
        /// <param name="mvDir"></param>
        /// <param name="skillID"></param>
        /// <param name="targetID"></param>
        public void EntityFSMChangeDataOnSing(Vector3 mvPos, Vector3 mvDir, int skillID, Ientity targetID)
        {
            EntityFSMPosition = mvPos;
            EntityFSMDirection = mvDir;
            EntitySkillID = skillID;
            entitySkillTarget = targetID;
        }

        /// <summary>
        /// OnEntityPrepareAttack
        /// </summary>
        public virtual void OnEntityPrepareAttack()
        {
        }
        //引导技能
        public virtual void OnEntityLeadSkill()
        {
            RealEntity.PlayerFreeAnimation();
        }

        /// <summary>
        /// Entities the FSM changedata.
        /// </summary>
        /// <param name="mvPos">Mv position.</param>
        /// <param name="mvDir">Mv dir.</param>
        /// <param name="mvSpeed">Mv speed.</param>
        public void EntityFSMChangeDataOnDead(Vector3 mvPos, Vector3 mvDir)
        {
            EntityFSMPosition = mvPos;
            EntityFSMDirection = mvDir;
        }

        public void EntityFSMChangedata(Vector3 mvPos, Vector3 mvDir, float mvSpeed)
        {
            EntityFSMPosition = mvPos;
            EntityFSMDirection = mvDir;
            EntityFSMMoveSpeed = mvSpeed;
        }

        /// <summary>
        /// Entities the FSM changedata.
        /// </summary>
        /// <param name="mvPos">Mv position.</param>
        /// <param name="mvDir">Mv dir.</param>
        public void EntityFSMChangedata(Vector3 mvPos, Vector3 mvDir)
        {
            //objTransform.position = mvPos;
            EntityFSMPosition = mvPos;
            EntityFSMDirection = mvDir;
        }
        public virtual void OnEntitySkill()
        {
        }

        /// <summary>
        /// 进入Idle状态
        /// </summary>
        public virtual void OnEnterIdle()
        {
            //this.RealEntity.PlayerAnimation("idle");
        }

        /// <summary>
        /// Run状态进入时调用
        /// </summary>
        public virtual void OnEnterMove()
        {
        }

        /// <summary>
        /// Raises the entity move event.
        /// </summary>
        public virtual void OnExecuteMove()
        {
            if (RealEntity == null || this.entityType == EntityType.Building)
            {
                return;
            }

            RealEntity.PlayerRunAnimation();
            //			float fThisTime = Time.realtimeSinceStartup;
            float fThisRealTimeSinceStartup = Time.realtimeSinceStartup;
            float fTimeSpan = fThisRealTimeSinceStartup - m_pcGOSSI.fBeginTime;

            float fThisFrameTimeSpan = fThisRealTimeSinceStartup - m_pcGOSSI.fLastSyncSecond;
            m_pcGOSSI.fLastSyncSecond = fThisRealTimeSinceStartup;
            float fMoveDist = fTimeSpan * m_pcGOSSI.fServerSpeed;
            m_pcGOSSI.sServerSyncPos = m_pcGOSSI.sServerBeginPos + m_pcGOSSI.sServerDir * fMoveDist;

            //2D计算
            Vector3 serverPos2D = new Vector3(m_pcGOSSI.sServerSyncPos.x, 60, m_pcGOSSI.sServerSyncPos.z);
            Vector3 realPos2D = new Vector3(objTransform.position.x, 60, objTransform.position.z);

            //需要同步的情况
            float fDistToSyncPos = Vector3.Distance(serverPos2D, realPos2D);
            if (fDistToSyncPos > 10)
            {
                objTransform.position = m_pcGOSSI.sServerSyncPos;
                objTransform.rotation = Quaternion.LookRotation(EntityFSMDirection);
                m_pcGOSSI.sLocalSyncDir = EntityFSMDirection;
                return;
            }
            Vector3 sCrossPoint = 5 * m_pcGOSSI.sServerDir + m_pcGOSSI.sServerSyncPos;
            Vector3 sThisSyncDir = sCrossPoint - realPos2D;
            sThisSyncDir.y = 0;
            sThisSyncDir.Normalize();


            float fAngle = Vector3.Angle(sThisSyncDir, m_pcGOSSI.sLocalSyncDir);
            if (5 < fAngle)
            {
                float fLerpVlaue = fAngle / 100;
                Vector3 sTempThisSyncDir = Vector3.Lerp(m_pcGOSSI.sLocalSyncDir, sThisSyncDir, fLerpVlaue);
                sThisSyncDir = sTempThisSyncDir;
            }
            float fThisMoveSpeed = m_pcGOSSI.fServerSpeed;
            Vector3 sTempSyncDir = m_pcGOSSI.sServerSyncPos - realPos2D;
            sTempSyncDir.y = 0;
            sTempSyncDir.Normalize();


            fAngle = Vector3.Angle(sTempSyncDir, m_pcGOSSI.sLocalSyncDir);
            if (90 < fAngle)
            {
                if (1 > fDistToSyncPos)
                {
                    fThisMoveSpeed = m_pcGOSSI.fServerSpeed;
                }
                else if (2 > fDistToSyncPos)
                {
                    fThisMoveSpeed -= m_pcGOSSI.fServerSpeed * 0.01f;
                }
                else if (5 > fDistToSyncPos)
                {
                    fThisMoveSpeed -= fThisMoveSpeed * 0.05f;
                }
                else if (6 > fDistToSyncPos)
                {
                    fThisMoveSpeed -= fThisMoveSpeed * 0.10f;
                }
                fThisMoveSpeed = m_pcGOSSI.fServerSpeed;
            }
            else
            {
                if (1 > fDistToSyncPos)
                {
                    fThisMoveSpeed = m_pcGOSSI.fServerSpeed;
                }
                else if (2 > fDistToSyncPos)
                {
                    fThisMoveSpeed += fThisMoveSpeed * 0.05f;
                }
                else if (4 > fDistToSyncPos)
                {
                    fThisMoveSpeed += fThisMoveSpeed * 0.10f;
                }
                else if (6 > fDistToSyncPos)
                {
                    fThisMoveSpeed += fThisMoveSpeed * 0.20f;
                }
                fThisMoveSpeed = m_pcGOSSI.fServerSpeed;
            }

            float fDistDiff = Mathf.Abs(fThisMoveSpeed - m_pcGOSSI.fServerSpeed);
            if (0.1 <= fDistDiff)
            {
                Debug.Log("GO " + GameObjGUID + " fThisMoveSpeed:" + fThisMoveSpeed);
            }

            //位置计算
            float fThisSyncDist = fThisMoveSpeed * fThisFrameTimeSpan;
            Vector3 sNewPos = sThisSyncDir * fThisSyncDist + realPos2D;
            if (sNewPos.magnitude > 0)
            {
                objTransform.position = sNewPos;
            }
            m_pcGOSSI.sLocalSyncDir = sThisSyncDir;
            Quaternion DestQuaternion = Quaternion.LookRotation(sThisSyncDir);
            Quaternion sMidQuater = Quaternion.Lerp(objTransform.rotation, DestQuaternion, 3 * Time.deltaTime);
            objTransform.rotation = sMidQuater;
        }

        /// <summary>
        /// 进入强制移动
        /// </summary>
        public void OnEnterForceMove()
        {
            if (RealEntity == null || this.entityType == EntityType.Building)
            {
                return;
            }
        }

        /// <summary>
        /// 执行强制移动
        /// </summary>
        public virtual void OnExecuteForceMove()
        {
            if (RealEntity == null || this.entityType == EntityType.Building)
            {
                return;
            }
            SkillMoveConfig skillInfo = ConfigReader.GetSkillMoveConfig(EntitySkillModelId);
            if (skillInfo != null)
            {
                this.RealEntity.PlayerAnimation(skillInfo.action);
            }
            float timeSinceStartup = Time.realtimeSinceStartup;
            float fTimeSpan = timeSinceStartup - m_pcGOSSI.fBeginTime;
            float fMoveDist = fTimeSpan * m_pcGOSSI.fForceMoveSpeed;
            m_pcGOSSI.sServerSyncPos = m_pcGOSSI.sServerBeginPos + m_pcGOSSI.sServerDir * fMoveDist;
            float fDistToSyncPos = Vector3.Distance(m_pcGOSSI.sServerSyncPos, objTransform.position);
            if (fDistToSyncPos > 10)
            {
                objTransform.position = m_pcGOSSI.sServerSyncPos;
                objTransform.rotation = Quaternion.LookRotation(EntityFSMDirection);
                m_pcGOSSI.sLocalSyncDir = EntityFSMDirection;
                return;
            }
            if (fDistToSyncPos < 1)
            {
                return;
            }
            Vector3 sThisSyncDir = m_pcGOSSI.sServerSyncPos - objTransform.position;
            sThisSyncDir.Normalize();
            float fThisMoveSpeed = m_pcGOSSI.fForceMoveSpeed;
            float fThisSyncDist = fThisMoveSpeed * Time.deltaTime;
            Vector3 sNewPos = sThisSyncDir * fThisSyncDist + objTransform.position;
            this.objTransform.position = sNewPos;
            Quaternion DestQuaternion = Quaternion.LookRotation(sThisSyncDir);
            Quaternion sMidQuater = Quaternion.Lerp(objTransform.rotation, DestQuaternion, 3 * Time.deltaTime);
            objTransform.rotation = sMidQuater;
        }

        /// <summary>
        /// 进入玩家先行状态
        /// </summary>
        public virtual void OnEnterEntityAdMove()
        {
        }

        /// <summary>
        /// 执行玩家先行状态
        /// </summary>
        public virtual void OnExecuteEntityAdMove()
        {
        }

        public virtual void OnEnterRelive()
        {
            if (this.RealEntity == null)
            {
                return;
            }
            this.RealEntity.PlayerDeadAnimation();
            if (this.XueTiao != null)
            {
                this.XueTiao.SetVisible(false);
            }
        }

        public virtual void OnExitRelive()
        {
            if (this.XueTiao != null)
            {
                this.XueTiao.SetVisible(true);
            }
        }

        public virtual void OnEnterFree()
        {
            if (RealEntity == null)
            {
                return;
            }
            Vector2 serverPos2D = new Vector2(m_pcGOSSI.sServerBeginPos.x, m_pcGOSSI.sServerBeginPos.z);
            Vector2 objPos2D = new Vector2(objTransform.position.x, objTransform.position.z);

            float fDistToServerPos = Vector2.Distance(serverPos2D, objPos2D);
            if (fDistToServerPos > 10)
            {
                objTransform.position = m_pcGOSSI.sServerBeginPos;
                RealEntity.PlayerFreeAnimation();
                objTransform.rotation = Quaternion.LookRotation(EntityFSMDirection);
            }

            //ShowXueTiao();
        }

        /// <summary>
        /// Raises the entity free event.
        /// </summary>
        public virtual void OnExecuteFree()
        {
            if (RealEntity == null || this.entityType == EntityType.Building)
            {
                return;
            }

            Vector2 serverPos2D = new Vector2(m_pcGOSSI.sServerBeginPos.x, m_pcGOSSI.sServerBeginPos.z);
            Vector2 objPos2D = new Vector2(objTransform.position.x, objTransform.position.z);

            float fDistToServerPos = Vector2.Distance(serverPos2D, objPos2D);
            Vector3 sSyncDir = m_pcGOSSI.sServerBeginPos - objTransform.position;
            sSyncDir.y = 0;
            sSyncDir.Normalize();
            float fAngle = Vector3.Angle(objTransform.forward, sSyncDir);

            if (10 < fDistToServerPos)
            {
                objTransform.position = m_pcGOSSI.sServerBeginPos;
                objTransform.rotation = Quaternion.LookRotation(m_pcGOSSI.sServerDir);
                RealEntity.PlayerFreeAnimation();
                return;
            }
            else if (0.5f >= fDistToServerPos)
            {
                RealEntity.PlayerFreeAnimation();
                return;
            }

            RealEntity.PlayerRunAnimation();
            float fSpeed = m_pcGOSSI.fServerSpeed;
            float fThisSyncDist = fSpeed * Time.deltaTime;
            if (fThisSyncDist > fDistToServerPos)
            {
                fThisSyncDist = fDistToServerPos;
            }
            Vector3 sNewPos = sSyncDir * fThisSyncDist + objTransform.position;
            objTransform.position = sNewPos;
            Quaternion DestQuaternion = Quaternion.LookRotation(EntityFSMDirection);
            Quaternion sMidQuater = Quaternion.Lerp(objTransform.rotation, DestQuaternion, 2 * Time.deltaTime);
            objTransform.rotation = sMidQuater;
        }

        public virtual void OnEntityExitPrepareSkill()
        {
        }
        /// <summary>
        /// Raises the entity prepare skill event.
        /// </summary>
        protected static float skillDistanceErorrThresold = 1.0f;
        //public GameObject AbsorbAct;
        private void PlayAbsorbSkill()
        {
        }
        public virtual void OnEntityReleaseSkill()
        {
            //////////////////////////////////////////////////////////////////////////////////////////
            //
            SkillManagerConfig skillManagerConfig = ConfigReader.GetSkillManagerCfg(EntitySkillID);
            if (skillManagerConfig == null)
            {
                return;
            }
            //吸附技能
            if (skillManagerConfig.isAbsorb == 1)
            {
                //删除已有的相同特效
                foreach (Transform child in RealEntity.objAttackPoint)
                {
                    if (child.name.Contains(skillManagerConfig.absorbRes))
                    {
                        int offset = child.name.IndexOf('_');
                        if (offset != 0)
                        {
                            string name = child.name.Substring(offset + 1);
                            int id = Convert.ToInt32(name);
                            EffectManager.Instance.DestroyEffect(id);
                        }
                    }
                }

                //创建吸附技能效果
                string absortActPath = "effect/soul_act/" + skillManagerConfig.absorbRes;
                NormalEffect absortSkillEffect = EffectManager.Instance.CreateNormalEffect(absortActPath, RealEntity.objAttackPoint.gameObject);
                if (absortSkillEffect != null)
                {
                    Quaternion rt = Quaternion.LookRotation(EntityFSMDirection);
                    //absortSkillEffect.obj.transform.rotation = rt;
                }
                objTransform.rotation = Quaternion.LookRotation(EntityFSMDirection);
            }
            else
            {
                if (skillManagerConfig.isNormalAttack == 1)
                {
                    RealEntity.PlayeAttackAnimation();
                }
                else
                {
                    {
                        RealEntity.PlayerAnimation(skillManagerConfig.rAnimation);
                        if (RealEntity.animation[skillManagerConfig.rAnimation] != null && RealEntity.animation[skillManagerConfig.rAnimation].wrapMode != WrapMode.Loop)
                        {
                            RealEntity.CrossFadeSqu("free");
                        }
                        BlGame.Effect.EffectManager.playSkillReleaseSound(this, EntitySkillID);
                    }
                }
                float distance = GetDistanceToPos(EntityFSMPosition);
                if (entityType != EntityType.Building)
                {
                    objTransform.rotation = Quaternion.LookRotation(EntityFSMDirection);
                }
            }
        }

        public virtual void OnEntityLeadingSkill()
        {
            //////////////////////////////////////////////////////////////////////////////////////////
            //
            SkillManagerConfig skillManagerConfig = ConfigReader.GetSkillManagerCfg(EntitySkillID);
            if (skillManagerConfig == null)
            {
                return;
            }

            RealEntity.PlayerAnimation(skillManagerConfig.rAnimation);

            float distance = GetDistanceToPos(EntityFSMPosition);
            if (entityType != EntityType.Building)
            {
                objTransform.rotation = Quaternion.LookRotation(EntityFSMDirection);
            }
        }

        public virtual void OnEntityLastingSkill()
        {
            //////////////////////////////////////////////////////////////////////////////////////////
            //
            SkillManagerConfig skillManagerConfig = ConfigReader.GetSkillManagerCfg(EntitySkillID);
            if (skillManagerConfig == null)
            {
                return;
            }

            RealEntity.PlayerAnimation(skillManagerConfig.rAnimation);

            float distance = GetDistanceToPos(EntityFSMPosition);
            if (entityType != EntityType.Building)
            {
                objTransform.rotation = Quaternion.LookRotation(EntityFSMDirection);
            }
        }




        protected GameObject CircleAreaCollision = null;
        //
        public virtual void InitWhenCreateModel()
        {
            int id = (int)ObjTypeID;
            if (this.entityType != EntityType.Player)
            {
                RealEntity.SetAttackAnimationLoop(true);
            }
            NpcConfigInfo npcInfo = ConfigReader.GetNpcInfo(id);
            AtkSpeed = npcInfo.NpcAttackSpeed;
            if (AtkSpeed != 0)
            {
                float spd = 1 / (AtkSpeed / 1000.0f);
                RealEntity.SetAttackAnimationSpd(spd);
            }
            else
            {
                RealEntity.SetAttackAnimationSpd(1.0f);
            }
        }

        public virtual void OnCreateShadow()
        {
            if (this.realObject.transform.FindChild("shadow(Clone)"))
                return;

            int id = (int)ObjTypeID;
            float radius = ConfigReader.GetNpcInfo(id).NpcCollideRadius;
            float range = radius / 100.0f;

            //GameObject obj = UnityEngine.Object.Instantiate(Resources.Load(GameDefine.GameConstDefine.LoadGameShadow)) as GameObject;
            ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameDefine.GameConstDefine.LoadGameShadow, ResourceType.PREFAB);
            GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;

            obj.transform.parent = this.realObject.transform;
            obj.transform.localScale = new Vector3(range, 1, range);
            obj.transform.localPosition = Vector3.zero;
        }

        //不计算y方向
        public float GetDistanceToPos(Vector3 pos)
        {
            if (RealEntity == null)
            {
                return 0.0f;
            }
            Vector3 selfPos = objTransform.position;
            Vector3 tarPos = pos;
            Vector3 diff = tarPos - selfPos;
            return Mathf.Sqrt(diff.x * diff.x + diff.z * diff.z);
        }

        //不计算y方向
        public float GetDistanceToEntity(Ientity tar)
        {
            if (tar == null || tar.RealEntity == null || RealEntity == null)
            {
                return 0.0f;
            }
            Vector3 selfPos = objTransform.position;
            Vector3 tarPos = tar.objTransform.position;
            Vector3 diff = tarPos - selfPos;
            return Mathf.Sqrt(diff.x * diff.x + diff.z * diff.z);
        }

        /// <summary>
        /// Entity死亡
        /// </summary>
        public virtual void OnEnterDead()
        {
            //目标死亡主角相关处理
            Iselfplayer player = PlayerManager.Instance.LocalPlayer;
            if (player != null && player.SyncLockTarget != null && GameObjGUID == player.SyncLockTarget.GameObjGUID)
            {
                player.SetSyncLockTarget(null);
            }

            //播放死亡声音
            Vector3 pos = PlayerManager.Instance.LocalPlayer.RealEntity.transform.position;
            if (GetDistanceToPos(pos) <= 30)
            {
                this.PlayDeadSound();
            }

            //建筑物更换模型和播表现特效
            if (this.entityType == EntityType.Building && this.NPCCateChild != ENPCCateChild.eNPCChild_BUILD_Base)
            {
                if (!EntityManager.GetHomeBaseList().Contains(this))
                {
                    GameMethod.SetLoadJianTaBaoZha(this);

                    int ret = EntityManager.HandleDelectEntity(this.GameObjGUID);
                    if (ret != 0)
                    {
                        Debug.LogError("OnEnterDead HandleDelectEntity:" + GameObjGUID + "failed");
                    }
                }
                EventCenter.Broadcast<Ientity>(EGameEvent.eGameEvent_NotifyBuildingDes, this);
                return;
            }

            //非建筑物播死亡动画和清理血条等状态
            this.DoEntityDead();
            RealEntity.PlayerDeadAnimation();
        }

        /// <summary>
        /// 
        /// </summary>
        public virtual void OnExecuteDead()
        {

        }

        /// <summary>
        /// 播放死亡音效
        /// </summary>
        private void PlayDeadSound()
        {
            string deadSound = "";
            if (this.entityType == EntityType.Player)
            {
                deadSound = ConfigReader.GetHeroInfo(this.NpcGUIDType).HeroDeathSould;
            }
            else
            {
                deadSound = ConfigReader.GetNpcInfo(this.NpcGUIDType).NpcDeathSould;
            }
            if (deadSound != "" && deadSound != "0" && deadSound != null)
            {
                //AudioClip clip = Resources.Load(AudioDefine.PATH_HERO_DEADSOUND + deadSound) as AudioClip;
                ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_HERO_DEADSOUND + deadSound, ResourceType.ASSET);
                AudioClip clip = clipUnit.Asset as AudioClip;
                AudioManager.Instance.PlayEffectAudio(clip);
            }
        }

        /// <summary>
        /// Entity死亡要处理的事
        /// </summary>
        protected virtual void DoEntityDead()
        {
            this.HideXueTiao();
            RealEntity.OnDestroyCollider();
        }

        #endregion

        public virtual void OnAttackEnd()
        {
        }

        public virtual void OnSkillAnimationEnd()
        {
        }
        public virtual void OnAttackAnimationEnd()
        {
        }

        /// <summary>
        /// Raises the update hp event.
        /// </summary>
        public virtual void OnUpdateHp()
        {
            if (this.XueTiao != null)
            {
                this.XueTiao.UpdateHP();
            }
        }

        public void CreateXueTiao()
        {
            if (XueTiao == null && XueTiaoManager.Instance != null)
            {
                XueTiao = XueTiaoManager.Instance.CreateXueTiao(this);
                xueTiaoObj = XueTiao.gameObject;
               
            }

            if (FSM != null && FSM.State == BlGame.FSM.FsmState.FSM_STATE_DEAD)
            {
                HideXueTiao();
            }
            else
            {
                ShowXueTiao();
            }
        }

        public void DestroyXueTiao()
        {
            if (XueTiao != null)
            {
                if (XueTiao.gameObject == null)
                {
                    Debug.Log("xue tiao obj is null");
                }

                //MonoBehaviour.DestroyImmediate(XueTiao.gameObject);
                GameObjectPool.Instance.ReleaseGO(XueTiao.mResName, xueTiaoObj, PoolObjectType.POT_XueTiao);                
                XueTiao = null;
            }
        }

        public void ShowXueTiao()
        {
            if (XueTiao != null)
            {
                XueTiao.SetVisible(true);
               // xueTiaoObj.SetActive(true);
                //xueTiaoObj.transform.parent.GetComponent<UIPanel>().Invalidate(true);
                this.OnUpdateHp();

                //Player需要更新Mp 
                if (entityType == EntityType.Player)
                {
                    XueTiaoPlayer XueTiaoPlayer = XueTiao as XueTiaoPlayer;
                    XueTiaoPlayer.UpdateMp();
                }
            }
        }

        public void HideXueTiao()
        {
            if (XueTiao != null)
            {
                XueTiao.SetVisible(false);
            }
        }

        public virtual void SetMp(float curMp)
        {
            Mp = curMp;
        }

        public virtual void SetHp(float curHp)
        {
            Hp = curHp;
        }

        public virtual void SetHpMax(float curHpMax)
        {
            HpMax = curHpMax;
        }

        public virtual void SetMpMax(float curMpMax)
        {
            MpMax = curMpMax;
        }

        public virtual void SetLevel(int curLv)
        {
            Level = curLv;
        }

        public virtual void SetExp(float curExp)
        {
            Exp = curExp;
        }

        public void SetCp(float curCp)
        {
            Cp = curCp;
        }

        public void SetTeamCp(float curCp)
        {
            TeamCp = curCp;
        }

        public virtual float GetColisionRadius()
        {
            int id = (int)ObjTypeID;
            return ConfigReader.GetNpcInfo(id).NpcCollideRadius / 100.0f;
        }

        public void UpdateHpChange(byte reason, float curHp)
        {
            float changeValue = Hp - curHp;
            SetHp((float)curHp);
            if (XueTiao != null)
            {
                XueTiao.UpdateHP();
            }
            if (reason != HpUpdateOfRecover && XueTiao != null)
            {
                XueTiao.XueTiaoDmgShow(Hp, changeValue, HpMax);
            }
        }

        public virtual void OnSkillInfoChange(int skillID, float time,float maxTime, int slot)
        {
        }

        public virtual int GetSkillIdBySkillType(SkillType type)
        {
            switch (type)
            {
                case SkillType.SKILL_TYPE1:
                    return ConfigReader.GetNpcInfo(NpcGUIDType).NpcSkillType1;
                case SkillType.SKILL_TYPE2:
                    return ConfigReader.GetNpcInfo(NpcGUIDType).NpcSkillType2;
                case SkillType.SKILL_TYPE3:
                    return ConfigReader.GetNpcInfo(NpcGUIDType).NpcSkillType3;
                case SkillType.SKILL_TYPE4:
                    return ConfigReader.GetNpcInfo(NpcGUIDType).NpcSkillType4;
                case SkillType.SKILL_TYPE5:
                    return ConfigReader.GetNpcInfo(NpcGUIDType).NpcSkillType5;
            }
            return -1;
        }

        /// <summary>
        /// 从Entity回调过来
        /// 点击中Entity的事件
        /// </summary>
        public void OnEntityFingerClick()
        {
            Iplayer pl = PlayerManager.Instance.LocalPlayer;
            if (pl != null)
            {
                if (pl.FSM != null && pl.FSM.State == FsmState.FSM_STATE_DEAD)
                {
                    return;
                }
                if (this.FSM != null && this.FSM.State == FsmState.FSM_STATE_DEAD)
                {
                    return;
                }
                float dis = Vector3.Distance(pl.objTransform.position, this.objTransform.position);
                if (dis > GameDefine.GameConstDefine.PlayerLockTargetDis)
                {
                    return;
                }
                if (this.GameObjGUID == pl.GameObjGUID)
                {
                    return;
                }
                if (ConfigReader.NpcXmlInfoDict.ContainsKey(NpcGUIDType) && ConfigReader.NpcXmlInfoDict[NpcGUIDType].un32ShopID != 0)
                {
                    Debug.LogError(NpcGUIDType);
                    OpenShop();
                    return;
                }
                //pl.SetSyncLockTarget(this);
            }
            else
            {
                pl = PlayerManager.Instance.LocalAccount;
            }
            //if (Vector3.Distance(pl.realObject.transform.position, this.realObject.transform.position) > GameConstDefine.PlayerLockTargetDis)
            //{
            //    return;
            //}
            if (ConfigReader.NpcXmlInfoDict.ContainsKey(NpcGUIDType) && ConfigReader.NpcXmlInfoDict[NpcGUIDType].un32ShopID != 0)
            {
                return;
            }
            pl.SetSyncLockTarget(this);
        }

        public void OpenShop(int id = 0)
        {
            if (UIShop.Instance == null)
            {
                //AudioClip clip = Resources.Load(AudioDefine.PATH_CLICKSHOP_SOUND) as AudioClip;
                ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(AudioDefine.PATH_CLICKSHOP_SOUND, ResourceType.ASSET);
                AudioClip clip = clipUnit.Asset as AudioClip;

                NGUITools.PlaySound(clip);

                BlGameUI.Instance.OnOpenUI(GameConstDefine.GameShop);
                if (id == 0)
                {
                    id = ConfigReader.NpcXmlInfoDict[NpcGUIDType].un32ShopID;
                }
                UIShop.Instance.InitShop(id);
                UIShop.Instance.OnPageSelect(PackageSelect.ShopPage.ShopPage1);
            }
        }

        public bool IsSameCamp(EntityCampType camp)
        {
            int campIndex = (int)camp % 2;
            int campIndex2 = (int)EntityCamp % 2;
            if (campIndex == campIndex2)
                return true;
            return false;

        }

        private const float hpLimit = 200f;
        //播放概率与不播放概率
        private float[] ifPlayMonsterSound = { 0.5f, 0.5f };
        public void SetJungleMonsterBeAtkVoice()
        {
            int id = (int)ObjTypeID;
            NpcConfigInfo info = ConfigReader.GetNpcInfo(id);
            if (entityType != EntityType.Monster || Hp < hpLimit || AudioManager.Instance.HeroLinesAudioDict.ContainsKey(GameObjGUID))
                return;
            List<float> ifPlaySoundList = new List<float>();
            for (int i = 0; i < ifPlayMonsterSound.Length; i++)
            {
                ifPlaySoundList.Add(ifPlayMonsterSound[i]);
            }
            int indexPlay = GameMethod.RandProbablityIndex(ifPlaySoundList);//获得是否播放下标
            if (indexPlay == 1)
            {
                return;
            }
            if (HasSameTypeNpcPlaySoundIsPlaying()) return;

            if (info.n32Script1Rate == null)
                return;

            List<float> probabilityList = GameMethod.ResolveToFloatList(info.n32Script1Rate);//获得概率集合
            for (int i = 0; i < probabilityList.Count; i++)
            {
                probabilityList[i] = (float)(probabilityList[i] - 90000) / 100f;
            }
            int index = GameMethod.RandProbablityIndex(probabilityList);//获得概率下标
            List<string> voiceList = GameMethod.ResolveToStrList(info.un32Script1);
            string name = voiceList[index];//获得概率下标对应的声音
            string path = AudioDefine.PATH_JUNGLE_MONSTER_BE_ATK_SOUND + name;

            //AudioClip clip = Resources.Load(path) as AudioClip;          
            ResourceUnit clipUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.ASSET);
            AudioClip clip = clipUnit.Asset as AudioClip;


            AudioManager.Instance.PlayHeroLinesAudio(GameObjGUID, clip);
        }

        private bool HasSameTypeNpcPlaySoundIsPlaying()
        {
            int id = (int)ObjTypeID;
            foreach (var item in AudioManager.Instance.HeroLinesAudioDict.Keys)
            {
                if (id != (int)ObjTypeID)
                    continue;
                AudioSource adSource = AudioManager.Instance.HeroLinesAudioDict[item];
                if (adSource.isPlaying)
                {
                    return true;
                }
            }
            return false;
        }

        private GameObject singEffect;
        private AudioSource singSound;
        public virtual void OnEnterSing()
        {
            SkillManagerConfig skillconfig = ConfigReader.GetSkillManagerCfg(EntitySkillID);
            RealEntity.PlayerAnimation(skillconfig.yAnimation);
            if (singEffect != null)
            {
                singEffect.transform.parent = null;
                GameObject.DestroyImmediate(singEffect);
            }
            string path = GameConstDefine.LoadGameSkillEffectPath + "release/" + skillconfig.yEffect;

            ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
            GameObject obj = objUnit.Asset as GameObject;

            if (obj == null)
            {
                return;
            }
            singEffect = GameObject.Instantiate(obj) as GameObject;
            if (singEffect == null)
            {
                return;
            }

            singEffect.transform.parent = RealEntity.objAttackPoint;
            singEffect.transform.localPosition = new Vector3(0.0f, 0.0f, 0.0f);
            string soundPath = GameConstDefine.LoadGameSoundPath + skillconfig.ySound;

            ResourceUnit soundObjUnit = ResourcesManager.Instance.loadImmediate(soundPath, ResourceType.ASSET);

            if (soundObjUnit.Asset != null)
            {
                AudioClip clip = soundObjUnit.Asset as AudioClip;
                if (clip != null)
                {
                    AudioSource Audio = AudioManager.Instance.PlayLongVoiceAudio(clip);
                    SceneSoundManager.Instance.addSound(Audio, RealEntity.gameObject);
                    singSound = Audio;
                    AudioManager.Instance.ChangeAudioVolume(Audio, 1.0f);
                }
            }
        }
        public virtual void OnExitSing()
        {
            if (singEffect != null)
            {
                singEffect.transform.parent = null;
                GameObject.DestroyImmediate(singEffect);
            }
            if (singSound != null)
            {
                singSound.Stop();
                singSound = null;
            }
        }

        //被击飞效果
        public virtual void OnBeatFly(uint buffId, string action, float raiseSpeed, float raiseAccSpeed, float fallSpeed, float fallAccSpeed, float stayTime, int canRecover)
        {
            if (realObject != null)
            {
                BeatFlyMotion beatFlyMotion = realObject.GetComponent<BeatFlyMotion>();
                //判断是否有被击飞组件,没有就添加
                if (beatFlyMotion == null)
                {
                    beatFlyMotion = realObject.AddComponent<BeatFlyMotion>();
                }
                beatFlyMotion.BeatFly(buffId, action, raiseSpeed, raiseAccSpeed, fallSpeed, fallAccSpeed, stayTime, canRecover);
            }
        }

        //被击飞效果立即下落
        public virtual void BeatFlyFallDown(uint buffID)
        {
            if (realObject != null)
            {
                BeatFlyMotion beatFlyMotion = realObject.GetComponent<BeatFlyMotion>();
                //当前buff对应击飞buffer移除击击飞效果
                if (beatFlyMotion != null && buffID == beatFlyMotion.mBuffId)
                {
                    beatFlyMotion.FallNow();
                }
            }
        }

    }
}