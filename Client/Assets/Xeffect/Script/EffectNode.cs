//----------------------------------------------
//            Xffect Editor
// Copyright © 2012- Shallway Studio
// http://shallway.net
//----------------------------------------------

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Xft;

namespace Xft
{
 
    public class CollisionParam
    {
        protected GameObject m_collideObject;
        protected Vector3 m_collidePos;
        
        public GameObject CollideObject
        {
            get {return m_collideObject;}
            set {m_collideObject = value;}
        }
        
        public Vector3 CollidePos
        {
            get {return m_collidePos;}
            set {m_collidePos = value;}
        }
        
        public CollisionParam(GameObject obj, Vector3 pos)
        {
            m_collideObject = obj;
            m_collidePos = pos;
        }
    }
    
    public class EffectNode
    {

        //constructor
        protected int Type;  //1 sprite, 2 ribbon trail, 3 cone, 4, custom mesh.
        public int Index;
        public Transform ClientTrans;
        public bool SyncClient;
        public EffectLayer Owner;

        //internal using
        protected Vector3 CurDirection;
        protected Vector3 LastWorldPos = Vector3.zero;
        protected Vector3 CurWorldPos;
        protected float ElapsedTime;
        public Sprite Sprite;    //1
        public RibbonTrail Ribbon; //2
        public Cone Cone;   //3
        public CustomMesh CusMesh;   //4

        //affect by affector
        public Vector3 Position;
        public Vector2 LowerLeftUV;
        public Vector2 UVDimensions;
        public Vector3 Velocity;
        public Vector2 Scale;
        public float RotateAngle;
        public Color Color;

        //reset
        protected List<Affector> AffectorList;
        protected Vector3 OriDirection;
        protected float LifeTime;
        protected int OriRotateAngle;
        protected float OriScaleX;
        protected float OriScaleY;

        protected bool SimpleSprite = false;

        //added 2012 6.3 for collision use

        protected bool IsCollisionEventSended = false;
        protected Vector3 LastCollisionDetectDir = Vector3.zero;

        public EffectNode(int index, Transform clienttrans, bool sync, EffectLayer owner)
        {
            Index = index;
            ClientTrans = clienttrans;
            SyncClient = sync;
            Owner = owner;
            LowerLeftUV = Vector2.zero;
            UVDimensions = Vector2.one;
            Scale = Vector2.one;
            RotateAngle = 0;
            Color = Color.white;
        }

        public void SetAffectorList(List<Affector> afts)
        {
            AffectorList = afts;
        }

        public List<Affector> GetAffectorList()
        {
            return AffectorList;
        }

        public void Init(Vector3 oriDir, float speed, float life, int oriRot, float oriScaleX, float oriScaleY, Color oriColor, Vector2 oriLowerUv, Vector2 oriUVDimension)
        {
            //OriDirection = ClientTrans.TransformDirection(oriDir);
            OriDirection = oriDir;
            LifeTime = life;
            OriRotateAngle = oriRot;
            OriScaleX = oriScaleX;
            OriScaleY = oriScaleY;
            Color = oriColor;
            ElapsedTime = 0f;
            // direction is synced to client rotation, except sphere dir
            if (Owner.DirType != DIRECTION_TYPE.Sphere)
                Velocity = Owner.transform.rotation * OriDirection * speed;
            else
                Velocity = OriDirection * speed;
            LowerLeftUV = oriLowerUv;
            UVDimensions = oriUVDimension;
            IsCollisionEventSended = false;

            if (Type == 1)
            {
                Sprite.SetUVCoord(LowerLeftUV, UVDimensions);
                Sprite.SetColor(oriColor);
                if (SimpleSprite)
                {
                    //Simple sprite 只更新一次。
                    Update(0f);
                    Sprite.Transform();
                }
            }

            else if (Type == 2)
            {
                Ribbon.SetUVCoord(LowerLeftUV, UVDimensions);
                Ribbon.SetColor(oriColor);
                Ribbon.SetHeadPosition(GetRealClientPos() + Position + OriDirection.normalized * Owner.TailDistance);
                Ribbon.ResetElementsPos();
            }
            else if (Type == 3)
            {
                Cone.SetUVCoord(LowerLeftUV, UVDimensions);
                Cone.SetColor(oriColor);
                Cone.SetRotation(oriRot);
            }
            else if (Type == 4)
            {
                CusMesh.SetColor(oriColor);
                CusMesh.SetRotation(oriRot);
                CusMesh.SetScale(oriScaleX,oriScaleY);
            }

            //set  sprite direction
            if (Type == 1)
            {
                //direction is synced to client rotation, except Sphere direction.
                if (Owner.DirType != DIRECTION_TYPE.Sphere)
                    Sprite.SetRotationTo(Owner.ClientTransform.rotation * OriDirection);
                else
                    Sprite.SetRotationTo(OriDirection);
                
            }
        }
        
        
        public void ResetCamera(Camera cam)
        {
            if (Type == 1)
                Sprite.MainCamera = cam;
            else if (Type == 2)
                Ribbon.MainCamera = cam;
        }
        
        public float GetElapsedTime()
        {
            return ElapsedTime;
        }

        public float GetLifeTime()
        {
            return LifeTime;
        }
  
        
        
        public void SetLocalPosition(Vector3 pos)
        {
            //fixed 2012.6.3   ribbon trail need to reset the head.
            if (Type == 2)
            {
                if (!SyncClient)
                    Ribbon.OriHeadPos = pos;
                else
                    Ribbon.OriHeadPos = GetRealClientPos() + pos;
                //Ribbon.ResetElementsPos();
            }

            // collison may get through. use dir to detect.
            //if (Owner.UseCollisionDetection && Owner.CollisionType != COLLITION_TYPE.CollisionLayer)
                //LastCollisionDetectDir = Vector3.zero;

            Position = pos;
        }
        public Vector3 GetLocalPosition()
        {
            return Position;
        }
  
        public Vector3 GetRealClientPos()
        {
            Vector3 mscale = Vector3.one * Owner.Owner.Scale;
            Vector3 clientPos = Vector3.zero;
            clientPos.x = ClientTrans.position.x / mscale.x;
            clientPos.y = ClientTrans.position.y / mscale.y;
            clientPos.z = ClientTrans.position.z / mscale.z;
            return clientPos;
        }
        
        //back to original scale pos.
        public Vector3 GetOriginalPos()
        {
            Vector3 ret;
            Vector3 clientPos = GetRealClientPos();
            if (!SyncClient)
                ret = Position - clientPos + ClientTrans.position;
            else
                ret = Position + ClientTrans.position;
            return ret;
        }

        public Vector3 GetWorldPos()
        {
            return CurWorldPos;
        }


        //added to optimize grid effect..if simpe no Transform() every circle.
        protected bool IsSimpleSprite()
        {
            bool ret = false;
            if (Owner.SpriteType == (int)STYPE.XZ && Owner.OriVelocityAxis == Vector3.zero&& Owner.ScaleAffectorEnable == false && Owner.RotAffectorEnable == false &&
                Owner.OriSpeed < 1e-04 && Owner.GravityAffectorEnable == false&& Owner.AirAffectorEnable == false && Owner.TurbulenceAffectorEnable == false && Owner.BombAffectorEnable == false&&Owner.UVRotAffectorEnable == false&&
                Mathf.Abs(Owner.OriRotationMax - Owner.OriRotationMin) < 1e-04 && Mathf.Abs(Owner.OriScaleXMin - Owner.OriScaleXMax) < 1e-04 &&
                Mathf.Abs(Owner.OriScaleYMin - Owner.OriScaleYMax) < 1e-04)
            {
                ret = true;
                //Debug.Log("detected simple sprite which not transformed!");
            }
            return ret;
        }

        //NOTICE:SetType is called before Init()!!and called only once!like constructor.

        //sprite
        public void SetType(float width, float height, STYPE type, ORIPOINT orip, int uvStretch, float maxFps)
        {
            Type = 1;
            SimpleSprite = IsSimpleSprite();
            //SimpleSprite = false;
            Sprite = Owner.GetVertexPool().AddSprite(width, height, type, orip, Owner.MyCamera, uvStretch, maxFps,SimpleSprite);
            Sprite.Owner = this;
        }
        //ribbon trail
        public void SetType(bool useFaceObj, Transform faceobj,float width, int maxelemnt, float len, Vector3 pos, int stretchType, float maxFps)
        {
            Type = 2;
            Ribbon = Owner.GetVertexPool().AddRibbonTrail(Owner.MyCamera, useFaceObj, faceobj, width, maxelemnt, len, pos, stretchType, maxFps);
            Ribbon.Owner = this;
        }

        //fence
        public void SetType(Vector2 size, int numSegment, float angle, Vector3 dir, int uvStretch, float maxFps, bool usedelta, AnimationCurve deltaAngle)
        {
            Type = 3;
            Cone = Owner.GetVertexPool().AddCone(size, numSegment, angle, dir, uvStretch, maxFps, usedelta,deltaAngle, this);
        }
        
        //custom mesh
        public void SetType(Mesh mesh, Vector3 dir, float maxFps)
        {
            Type = 4;
            CusMesh = Owner.GetVertexPool().AddCustomMesh(mesh,dir,maxFps,this);
        }

        public void Reset()
        {
            //Position = Vector3.up * -9999;
            Position = Owner.EmitPoint + Owner.transform.position;
            Velocity = Vector3.zero;
            ElapsedTime = 0f;
   
            
            CurWorldPos = Owner.transform.position;
            
            LastWorldPos = CurWorldPos;

            IsCollisionEventSended = false;

            //foreach (Affector aft in AffectorList)
            //{
            //    aft.Reset();
            //}

            //do not use foreach in your script!
            for (int i = 0; i < AffectorList.Count; i++)
            {
                Affector aft = AffectorList[i];
                aft.Reset();
            }
   
            Scale = Vector3.one;
            
            if (Type == 1)
            {
                Sprite.SetRotation(OriRotateAngle);
                Sprite.SetPosition(Position);
                Sprite.SetColor(Color.clear);
                Sprite.Update(true,0f);
                //TODO:should reset in ScaleAffector.
                Scale = Vector2.one;
            }
            else if (Type == 2)
            {
                Vector3 headpos;
                if (Owner.AlwaysSyncRotation)
                    headpos = ClientTrans.rotation * (GetRealClientPos() + Owner.EmitPoint);
                else
                    headpos = GetRealClientPos() + Owner.EmitPoint;
                Ribbon.SetHeadPosition(headpos /*+ OriDirection.normalized * Owner.TailDistance*/);
                Ribbon.Reset();
                Ribbon.SetColor(Color.clear);
                Ribbon.UpdateVertices(Vector3.zero);
            }
            else if (Type == 3)
            {
                Cone.SetRotation(OriRotateAngle);
                Cone.SetColor(Color.clear);
                Cone.SetPosition(Position);
                Scale = Vector2.one;
                Cone.ResetAngle();
                Cone.Update(true,0f);
            }
            else if (Type == 4)
            {
                CusMesh.SetColor(Color.clear);
                CusMesh.SetRotation(OriRotateAngle);

                CusMesh.Update(true,0f);
            }
                
        }

        public void Remove()
        {
            Owner.RemoveActiveNode(this);
        }
  
        public void Stop()
        {
            Reset();
            Remove();
        }

        public void UpdateSprite(float deltaTime)
        {
            //added 2012.7.6
            if (Owner.AlwaysSyncRotation)
            {
                if (Owner.DirType != DIRECTION_TYPE.Sphere)
                    Sprite.SetRotationTo(Owner.transform.rotation * OriDirection);
            }

            Sprite.SetScale(Scale.x * OriScaleX, Scale.y * OriScaleY);
            if (Owner.ColorAffectorEnable)
                Sprite.SetColor(Color);
            if (Owner.UVAffectorEnable || Owner.UVRotAffectorEnable)
                Sprite.SetUVCoord(LowerLeftUV, UVDimensions);

            Sprite.SetRotation((float)OriRotateAngle + RotateAngle);
            Sprite.SetPosition(CurWorldPos);
            Sprite.Update(false,deltaTime);
        }

        public void UpdateRibbonTrail(float deltaTime)
        {
            Ribbon.SetHeadPosition(CurWorldPos);
            if (Owner.UVAffectorEnable || Owner.UVRotAffectorEnable)
                Ribbon.SetUVCoord(LowerLeftUV, UVDimensions);


            Ribbon.SetColor(Color);
            Ribbon.Update(deltaTime);
        }

        public void UpdateCone(float deltaTime)
        {
            Cone.SetScale(Scale.x * OriScaleX, Scale.y * OriScaleY);
            if (Owner.ColorAffectorEnable)
                Cone.SetColor(Color);
            if (Owner.UVAffectorEnable || Owner.UVRotAffectorEnable)
                Cone.SetUVCoord(LowerLeftUV, UVDimensions);
            Cone.SetRotation((float)OriRotateAngle + RotateAngle);
            Cone.SetPosition(CurWorldPos);
            Cone.Update(false, deltaTime);
        }
        
        public void UpdateCustomMesh(float deltaTime)
        {
            CusMesh.SetScale(Scale.x * OriScaleX, Scale.y * OriScaleY);
            if (Owner.ColorAffectorEnable)
                CusMesh.SetColor(Color);
            CusMesh.SetRotation((float)OriRotateAngle + RotateAngle);
            CusMesh.SetPosition(CurWorldPos);
            CusMesh.Update(false, deltaTime);
        }

        public void CollisionDetection()
        {
            if (!Owner.UseCollisionDetection || IsCollisionEventSended)
                return;

            bool collided = false;
   
            GameObject collideObject = null;
            
            if (Owner.CollisionType == COLLITION_TYPE.Sphere)
            {
                Vector3 diff = CurWorldPos - Owner.CollisionGoal.position;
                float range = Owner.ColliisionPosRange + Owner.ParticleRadius;
                if (diff.sqrMagnitude <= range * range /*|| //fixed, don't check get through.
                    Vector3.Dot(diff, LastCollisionDetectDir) < 0*/)
                {
                    collided = true;
                    collideObject = Owner.CollisionGoal.gameObject;
                }
                LastCollisionDetectDir = diff;
            }
            else if (Owner.CollisionType == COLLITION_TYPE.CollisionLayer)
            {
                int layer = 1 << Owner.CollisionLayer;
                
                RaycastHit hit;
                Vector3 p1 = GetOriginalPos();
                if (Physics.SphereCast(p1, Owner.ParticleRadius, Velocity.normalized, out hit, Owner.ParticleRadius / 2,layer))
                {
                    collided = true;
                    collideObject = hit.collider.gameObject;
                }
                
                //if (Physics.CheckSphere(GetOriginalPos(), Owner.ParticleRadius, layer))
                //{
                    //collided = true;
                //}
            }
            else
            {
                Debug.LogError("invalid collision type!");
            }

            if (collided)
            {
                if (Owner.EventHandleFunctionName != "" && Owner.EventReceiver != null)
                {
                    //Owner.EventReceiver.SendMessage(Owner.EventHandleFunctionName, Owner.CollisionGoal);
                    Owner.EventReceiver.SendMessage(Owner.EventHandleFunctionName, new CollisionParam(collideObject,GetOriginalPos()));
                }
                    
                IsCollisionEventSended = true;
                if (Owner.CollisionAutoDestroy)
                {
                    //distroy.
                    ElapsedTime = Mathf.Infinity;
                }
            }
        }


        public void Update(float deltaTime)
        {
            ElapsedTime += deltaTime;


            //foreach (Affector aft in AffectorList)
            //{
            //    aft.Update(deltaTime);
            //}

            for (int i = 0; i < AffectorList.Count; i++)
            {
                Affector aft = AffectorList[i];
                aft.Update(deltaTime);
            }

            Position += Velocity * deltaTime;

            CollisionDetection();

            //if (Mathf.Abs(Acceleration) > 1e-04)
            //{
                //Velocity += Velocity.normalized * Acceleration * deltaTime;
            //}

            if (SyncClient)
            {
                //NOTICE: only sync position.
                //and the node direction is synced to client rotation by default. may add a option to choose?.
                CurWorldPos = Position + GetRealClientPos(); 
            }
            else
            {
                CurWorldPos = Position;
            }
            if (Type == 1)
                UpdateSprite(deltaTime);
            else if (Type == 2)
                UpdateRibbonTrail(deltaTime);
            else if (Type == 3)
                UpdateCone(deltaTime);
            else if (Type == 4)
                UpdateCustomMesh(deltaTime);

            LastWorldPos = CurWorldPos;

            if (ElapsedTime > LifeTime && LifeTime > 0)
            {
                Reset();
                Remove();
            }
        }
    }
}