//----------------------------------------------
//            Xffect Editor
// Copyright © 2012- Shallway Studio
// http://shallway.net
//----------------------------------------------
using UnityEngine;
using System.Collections;
using Xft;
using System.Collections.Generic;

namespace Xft
{
	public enum STYPE
{
    BILLBOARD,
    BILLBOARD_SELF,
    XZ,
}

public enum ORIPOINT
{
    CENTER,
    LEFT_UP,
    LEFT_BOTTOM,
    RIGHT_BOTTOM,
    RIGHT_UP,
    BOTTOM_CENTER,
    TOP_CENTER,
    LEFT_CENTER,
    RIGHT_CENTER
}

public enum EMITTYPE
{
    POINT,
    BOX,
    SPHERE,
    CIRCLE,
    LINE,
    Mesh
}

public enum RSTYPE
{
    NONE,
    SIMPLE,
    CURVE
}

public enum MAGTYPE
{
    Fixed,
    Curve
}


public enum COLLITION_TYPE
{
    Sphere,
    CollisionLayer
}

public enum COLOR_GRADUAL_TYPE
{
    CLAMP,
    LOOP,
    REVERSE,
        CURVE
}

public enum DIRECTION_TYPE
{
    Planar,
    Cone,
    Sphere,
    Cylindrical
}
}

public class EffectLayer : MonoBehaviour
{
 
    public int VersionIdentifier1 = 222;
    
    public VertexPool Vertexpool;
    //Main Config
    public Transform ClientTransform;
    public bool SyncClient;
    public Material Material;
    public int RenderType; //0 sprite 1 ribbon 2 cone, 3 custom mesh
    public float StartTime = 0f;
    public float MaxFps = 60f;
    public bool DrawDebug = true;
    public Color DebugColor = Color.white;
    //ver 1.2.1
    public int Depth = 0;

    //Sprite Config
    public int SpriteType;
    public int OriPoint;
    public float SpriteWidth = 1;
    public float SpriteHeight = 1;
    public int SpriteUVStretch = 0;

    //Rotation Config
    public int OriRotationMin;
    public int OriRotationMax;
    public bool RotAffectorEnable = false;
    public RSTYPE RotateType = RSTYPE.NONE;
    public float DeltaRot;
    public AnimationCurve RotateCurve;

    //Scale Config
    public float OriScaleXMin = 1f;
    public float OriScaleXMax = 1f;
    public float OriScaleYMin = 1f;
    public float OriScaleYMax = 1f;
    public bool ScaleAffectorEnable = false;
    public RSTYPE ScaleType = RSTYPE.NONE;
    public float DeltaScaleX = 0f;
    public float DeltaScaleY = 0f;
    public AnimationCurve ScaleXCurve;
    public AnimationCurve ScaleYCurve;
    
    public bool UseSameScaleCurve = false;

    //Color Config
    public bool ColorAffectorEnable = false;
    public int ColorAffectType = 0;
    public float ColorGradualTimeLength = 1f;
    public COLOR_GRADUAL_TYPE ColorGradualType = COLOR_GRADUAL_TYPE.CLAMP;
    public Color Color1 = Color.white;
    public Color Color2;
    public Color Color3;
    public Color Color4;
    public Color Color5;

    //RibbonTrail Config
    public float RibbonWidth = 1f;
    public int MaxRibbonElements = 8;
    public float RibbonLen = 15f;
    public float TailDistance = 0f;
    public int StretchType = 0;
    public bool FaceToObject = false;
    public Transform FaceObject;
    //ver 1.2.1
    public bool UseRandomRibbon = false;
    public float RibbonWidthMin = 1f;
    public float RibbonWidthMax = 1f;
    public float RibbonLenMin = 15f;
    public float RibbonLenMax = 15f;


    //Cone Config
    public Vector2 ConeSize = Vector2.zero;
    public float ConeAngle;
    public int ConeSegment = 4;
    public bool UseConeAngleChange = false;
    public AnimationCurve ConeDeltaAngle;
    
    //Custom Mesh Config
    public Mesh CMesh;

    //Emitter Config
    public int EmitType; // 0:point 1:box, 2: sphere surface, 3: circle, 4: line. 5: mesh
    public Vector3 BoxSize;
    public bool BoxInheritRotation = false;
    public Vector3 EmitPoint;
    public float Radius;
    public Vector3 CircleDir = Vector3.up;
    public bool EmitUniform = false;
    public float LineLengthLeft = -1f;
    public float LineLengthRight = 1f;
    public int MaxENodes = 1;
    public bool IsNodeLifeLoop = true;
    public float NodeLifeMin = 1;
    public float NodeLifeMax = 1;
    
    public EEmitWay EmitWay = EEmitWay.ByRate;
    public AnimationCurve EmitterCurve = null;
    
    //for downward compatibility
    //public bool IsEmitByDistance = false;
    
    public float DiffDistance = 0.1f;
    public Mesh EmitMesh;
    public int EmitMeshType;

    public float ChanceToEmit = 100f;
    public float EmitDuration = 100f;
    public float EmitRate = 20;
    public int EmitLoop = -1;
    public float EmitDelay = 0f;

    //Original Direction Config
    
    //discarded
    public bool IsRandomDir;
    
    public DIRECTION_TYPE DirType = DIRECTION_TYPE.Planar;
    public Transform DirCenter;
    public Vector3 OriVelocityAxis;
    public int AngleAroundAxis;
    public float OriSpeed;
    public bool AlongVelocity = false;
    public bool AlwaysSyncRotation = false;
    public bool IsRandomSpeed = false;
    public float SpeedMin = 0f;
    public float SpeedMax = 0f;


    //for editor convenience

    public int AffectorIndex;

    //Jet Affector
    public bool JetAffectorEnable = false;
    public MAGTYPE JetMagType = MAGTYPE.Fixed;
    public float JetMag;
    public AnimationCurve JetCurve;
    //public float JetMin;
    //public float JetMax;

    //Vortex Affector
    public bool VortexAffectorEnable = false;
    public MAGTYPE VortexMagType = MAGTYPE.Fixed;
    public float VortexMag = 1f;
    public AnimationCurve VortexCurve;
    public Vector3 VortexDirection = Vector3.up;
    public bool VortexInheritRotation = true;
    public Transform VortexObj;
    //ver 1.2.1
    public bool IsRandomVortexDir = false;
	//2012.9.22
	public bool IsVortexAccelerate = false;
	public float VortexAttenuation = 0f;
	public bool UseVortexMaxDistance = false;
	public float VortexMaxDistance = 0f;

    //UVRotationAffector
    public bool UVRotAffectorEnable = false;
    public bool RandomUVRotateSpeed = false;
    public float UVRotXSpeed = 0;
    public float UVRotYSpeed = 0;
    public float UVRotXSpeedMax = 0;
    public float UVRotYSpeedMax = 0;

    //Gravity Affector
    public bool GravityAffectorEnable = false;
    public GAFTTYPE GravityAftType = GAFTTYPE.Planar;
    public MAGTYPE GravityMagType = MAGTYPE.Fixed;
    public float GravityMag;
    public AnimationCurve GravityCurve;
    public Vector3 GravityDirection = Vector3.up;
    public Transform GravityObject;
    public bool IsGravityAccelerate = true;

    //AirField Affector
    public bool AirAffectorEnable = false;
    public Transform AirObject;
    public MAGTYPE AirMagType = MAGTYPE.Fixed;
    public float AirMagnitude;
    public AnimationCurve AirMagCurve;
    public Vector3 AirDirection;
    public float AirAttenuation;
    public bool AirUseMaxDistance;
    public float AirMaxDistance;
    public bool AirEnableSpread;
    public float AirSpread;
    public float AirInheritVelocity;
    public bool AirInheritRotation;

    //Bomb Affector
    public bool BombAffectorEnable = false;
    public Transform BombObject;
    public BOMBTYPE BombType = BOMBTYPE.Spherical;
    public BOMBDECAYTYPE BombDecayType = BOMBDECAYTYPE.None;
    public MAGTYPE BombMagType = MAGTYPE.Fixed;
    public float BombMagnitude;
    public AnimationCurve BombMagCurve;
    public Vector3 BombAxis;
    public float BombDecay;

    //TurbulenceFieldAffector
    public bool TurbulenceAffectorEnable = false;
    public Transform TurbulenceObject;
    public MAGTYPE TurbulenceMagType = MAGTYPE.Fixed;
    public float TurbulenceMagnitude;
    public AnimationCurve TurbulenceMagCurve;
    public float TurbulenceAttenuation;
    public bool TurbulenceUseMaxDistance;
    public float TurbulenceMaxDistance;
    
    //DragAffector
    public bool DragAffectorEnable = false;
    public Transform DragObj;
    public bool DragUseDir = false;
    public Vector3 DragDir = Vector3.up;
    public float DragMag = 10f;
    public bool DragUseMaxDist = false;
    public float DragMaxDist = 50f;
    public float DragAtten = 0f;

    //UV Config
    public bool UVAffectorEnable = false;
    
    public int UVType = 0;   //0: none 1: UVAnimation, 2: UVRotate
    public Vector2 OriTopLeftUV = Vector2.zero;
    public Vector2 OriUVDimensions = Vector2.one;
    protected Vector2 UVTopLeft;
    protected Vector2 UVDimension;
    public int Cols = 1;
    public int Rows = 1;
    public int LoopCircles = -1;
    public float UVTime = 1;
    public string EanPath = "none";
    public int EanIndex = 0;
    public bool RandomStartFrame = false;

    public bool RandomOriScale = false;
    public bool RandomOriRot = false;


    //Collision Config
    public bool UseCollisionDetection = false;
    public float ParticleRadius = 1f;
    public COLLITION_TYPE CollisionType = COLLITION_TYPE.Sphere;
    public bool CollisionAutoDestroy = true;
    public Transform EventReceiver;
    public string EventHandleFunctionName=" ";
    public Transform CollisionGoal;
    public float ColliisionPosRange;
    public LayerMask CollisionLayer;
    public Collider CollisionCollider = null;


    public Emitter emitter;


    public EffectNode[] AvailableENodes;
    public EffectNode[] ActiveENodes;
    public int AvailableNodeCount;
    public Vector3 LastClientPos;

    public  Camera MyCamera;
    
    
    public XffectComponent Owner;
    
    public void FindMyCamera()
    {
        int layerMask = 1 << gameObject.layer;
        Camera[] cameras = GameObject.FindObjectsOfType(typeof(Camera)) as Camera[];
        for (int i = 0, imax = cameras.Length; i < imax; ++i)
        {
            Camera cam = cameras[i];

            if ((cam.cullingMask & layerMask) != 0)
            {
                MyCamera = cam;
                return;
            }
        }
        Debug.LogWarning("can't find properly camera for this object's layer!");
    }


    protected void InitCollision()
    {
        if (!UseCollisionDetection)
            return;

        if (CollisionType == COLLITION_TYPE.CollisionLayer)
            return;

        if (CollisionGoal == null /* || CollisionGoal.gameObject.active == false*/)
        {
            //Debug.LogWarning("please set the collision goal!");
            //UseCollisionDetection = false;
            //return;
        }

        //暂时没有用到，先留着吧
        //if (CollisionType == COLLITION_TYPE.Collider)
        //{
        //    CollisionCollider = CollisionGoal.GetComponent<Collider>();
        //}
    }

    protected List<Affector> InitAffectors(EffectNode node)
    {
        List<Affector> AffectorList = new List<Affector>();

        if (UVAffectorEnable)
        {
            UVAnimation uvAnim = new UVAnimation();
            if (UVType == 1)
            {
                float perWidth = OriUVDimensions.x / Cols;
                float perHeight = Mathf.Abs(OriUVDimensions.y / Rows);
                Vector2 cellSize = new Vector2(perWidth, perHeight);
                uvAnim.BuildUVAnim(OriTopLeftUV, cellSize, Cols, Rows, Cols * Rows);
            }
            UVDimension = uvAnim.UVDimensions[0];
            UVTopLeft = uvAnim.frames[0];

            if (uvAnim.frames.Length != 1)
            {
                uvAnim.loopCycles = LoopCircles;
                Affector aft = new UVAffector(uvAnim, UVTime, node, RandomStartFrame);
                AffectorList.Add(aft);
            }
        }
        else
        {
            UVDimension = OriUVDimensions;
            UVTopLeft = OriTopLeftUV;
        }


        if (RotAffectorEnable && RotateType != RSTYPE.NONE)
        {
            Affector aft;
            if (RotateType == RSTYPE.CURVE)
                aft = new RotateAffector(RotateCurve, node);
            else
                aft = new RotateAffector(DeltaRot, node);
            AffectorList.Add(aft);
        }
        if (ScaleAffectorEnable && ScaleType != RSTYPE.NONE)
        {
            Affector aft;
            
            if (UseSameScaleCurve)
                ScaleYCurve = ScaleXCurve;
            
            if (ScaleType == RSTYPE.CURVE)
                aft = new ScaleAffector(ScaleXCurve, ScaleYCurve, node);
            else
                aft = new ScaleAffector(DeltaScaleX, DeltaScaleY, node);
            AffectorList.Add(aft);
        }
        if (ColorAffectorEnable && ColorAffectType != 0)
        {
            ColorAffector aft;
            if (ColorAffectType == 2)
            {
                Color[] carr = new Color[5];
                carr[0] = Color1; carr[1] = Color2; carr[2] = Color3; carr[3] = Color4; carr[4] = Color5;
                aft = new ColorAffector(carr, ColorGradualTimeLength, ColorGradualType, node);
            }
            else
            {
                Color[] carr = new Color[2];
                carr[0] = Color1; carr[1] = Color2;
                aft = new ColorAffector(carr, ColorGradualTimeLength, ColorGradualType, node);
            }
            AffectorList.Add(aft);
        }
        if (JetAffectorEnable)
        {
            Affector aft = new JetAffector(JetMag,JetMagType,JetCurve,node);
            AffectorList.Add(aft);
        }
        if (VortexAffectorEnable)
        {
            Affector aft;
            aft = new VortexAffector(VortexObj, VortexMagType, VortexMag, VortexCurve, VortexDirection, VortexInheritRotation, node);

            AffectorList.Add(aft);
        }
        if (UVRotAffectorEnable)
        {
            Affector aft;
            
            float xscroll = UVRotXSpeed;
            float yscroll = UVRotYSpeed;
            if (RandomUVRotateSpeed)
            {
                xscroll = Random.Range(UVRotXSpeed,UVRotXSpeedMax);
                yscroll = Random.Range(UVRotYSpeed,UVRotYSpeedMax);
            }
            
            aft = new UVRotAffector(xscroll, yscroll, node);
            AffectorList.Add(aft);
        }


        if (GravityAffectorEnable)
        {
            Affector aft;
            aft = new GravityAffector(GravityObject, GravityAftType, GravityMagType, IsGravityAccelerate,GravityDirection, GravityMag, GravityCurve, node);
            AffectorList.Add(aft);

            if (GravityAftType == GAFTTYPE.Spherical && GravityObject == null)
            {
                Debug.LogWarning("Gravity Object is missing, automatically set to effect layer self:" + gameObject.name);
                GravityObject = transform;
            }

        }
        if (AirAffectorEnable)
        {
            Affector aft = new AirFieldAffector(AirObject, AirDirection, AirMagType, AirMagnitude, AirMagCurve, AirAttenuation, AirUseMaxDistance,
                AirMaxDistance, AirEnableSpread, AirSpread, AirInheritVelocity, AirInheritRotation, node);
            AffectorList.Add(aft);
        }
        if (BombAffectorEnable)
        {
            Affector aft = new BombAffector(BombObject, BombType, BombMagType, BombDecayType, BombMagnitude, BombMagCurve, BombDecay, BombAxis, node);
            AffectorList.Add(aft);
        }
        if (TurbulenceAffectorEnable)
        {
            Affector aft = new TurbulenceFieldAffector(TurbulenceObject, TurbulenceMagType, TurbulenceMagnitude, TurbulenceMagCurve, TurbulenceAttenuation, TurbulenceUseMaxDistance, TurbulenceMaxDistance, node);
            AffectorList.Add(aft);
        }
        if (DragAffectorEnable)
        {
            Affector aft = new DragAffector(DragObj,DragUseDir,DragDir,DragMag,DragUseMaxDist,DragMaxDist,DragAtten,node);
            AffectorList.Add(aft);
        }
        return AffectorList;
    }

    //fixed 2012.5.24
    //if set client by out caller. each node should update too.
    public void SetClient(Transform client)
    {
        ClientTransform = client;
        for (int i = 0; i < MaxENodes; i++)
        {
            EffectNode node = ActiveENodes[i];
            if (node == null)
                node = AvailableENodes[i];
            node.ClientTrans = client;
        }
    }

    protected void Init()
    {
        //added 2012.6.24
        InitCollision();
        
        Owner = transform.parent.gameObject.GetComponent<XffectComponent>();
        if (Owner == null)
            Debug.LogError("you must set EffectLayer to be XffectComponent's child.");

        //fixed 2012.6.2. ignoring the red errors.
        if (ClientTransform == null)
        {
            Debug.LogWarning("effect layer: "+ gameObject.name + " haven't assign a client transform, automaticly set to itself.");
            ClientTransform = transform;
        }
        AvailableENodes = new EffectNode[MaxENodes];
        ActiveENodes = new EffectNode[MaxENodes];
        for (int i = 0; i < MaxENodes; i++)
        {
            EffectNode n = new EffectNode(i, ClientTransform, SyncClient, this);
            List<Affector> afts = InitAffectors(n);
            n.SetAffectorList(afts);
            if (RenderType == 0)
                n.SetType(SpriteWidth, SpriteHeight, (STYPE)SpriteType, (ORIPOINT)OriPoint, SpriteUVStretch, MaxFps);
            else if (RenderType == 1)
            {
                float rwidth = RibbonWidth;
                float rlen = RibbonLen;
                if (UseRandomRibbon)
                {
                    rwidth = Random.Range(RibbonWidthMin, RibbonWidthMax);
                    rlen = Random.Range(RibbonLenMin, RibbonLenMax);
                }
                n.SetType(FaceToObject, FaceObject, rwidth, MaxRibbonElements, rlen, ClientTransform.position + EmitPoint, StretchType, MaxFps);
            }
            else if (RenderType == 2)
            {
                n.SetType(ConeSize,ConeSegment,ConeAngle, transform.rotation * OriVelocityAxis,0,MaxFps,UseConeAngleChange,ConeDeltaAngle);
            }  
            else if (RenderType == 3)
            {
                Vector3 dir = Vector3.zero;
                if (OriVelocityAxis == Vector3.zero)
                {
                    OriVelocityAxis = Vector3.up;
                }
                
                dir = transform.rotation * OriVelocityAxis;
                
                n.SetType(CMesh,dir,MaxFps);
            }
                
            AvailableENodes[i] = n;
        }
        AvailableNodeCount = MaxENodes;
        emitter = new Emitter(this);
    }


    public VertexPool GetVertexPool()
    {
        return Vertexpool;
    }


    public int GetActiveNodeCount()
    {
        return ActiveENodes.Length;
    }

    public void RemoveActiveNode(EffectNode node)
    {
        if (AvailableNodeCount == MaxENodes)
            Debug.LogError("out index!");
        if (ActiveENodes[node.Index] == null) //already removed
            return;
        ActiveENodes[node.Index] = null;
        AvailableENodes[node.Index] = node;
        AvailableNodeCount++;
    }

    public void AddActiveNode(EffectNode node)
    {
        if (AvailableNodeCount == 0)
            Debug.LogError("out index!");
        if (AvailableENodes[node.Index] == null) //already added
            return;
        ActiveENodes[node.Index] = node;
        AvailableENodes[node.Index] = null;
        AvailableNodeCount--;
    }


    protected void AddNodes(int num)
    {
        int added = 0;
        for (int i = 0; i < MaxENodes; i++)
        {
            if (added == num)
                break;
            EffectNode node = AvailableENodes[i];
            if (node != null)
            {
                AddActiveNode(node);
                added++;

                emitter.SetEmitPosition(node);
                float nodeLife = 0;
                if (IsNodeLifeLoop)
                    nodeLife = -1;
                else
                    nodeLife = Random.Range(NodeLifeMin, NodeLifeMax);
                Vector3 oriDir = emitter.GetEmitRotation(node);
                float speed = OriSpeed;
                if (IsRandomSpeed)
                {
                    speed = Random.Range(SpeedMin, SpeedMax);
                }
                node.Init(oriDir.normalized, speed, nodeLife, Random.Range(OriRotationMin, OriRotationMax),
                    Random.Range(OriScaleXMin, OriScaleXMax), Random.Range(OriScaleYMin, OriScaleYMax), Color1, UVTopLeft, UVDimension);
            }
            else
                continue;
        }
    }
    
    
    public void ResetCamera(Camera cam)
    {
        if (ActiveENodes == null)
            return;
        for (int i = 0; i < MaxENodes; i++)
        {
            EffectNode node = ActiveENodes[i];
            if (node != null)
            {
                node.ResetCamera(cam);
            }
            else
            {
                node = AvailableENodes[i];
                node.ResetCamera(cam);
            }
        }
    }
    
    public void Reset()
    {
        if (ActiveENodes == null)
            return;
        for (int i = 0; i < MaxENodes; i++)
        {
            EffectNode node = ActiveENodes[i];
            if (node != null)
            {
                node.Reset();
                RemoveActiveNode(node);
            }
        }
        emitter.Reset();
    }

    public void FixedUpdateCustom(float deltaTime)
    {
        //Version 2.2.0, update the camera when new level is loaded.
        if (MyCamera == null)
        {
            FindMyCamera();
            ResetCamera(MyCamera);
        }
        
        int needToAdd = emitter.GetNodes(deltaTime);
        AddNodes(needToAdd);
        for (int i = 0; i < MaxENodes; i++)
        {
            EffectNode node = ActiveENodes[i];
            if (node == null)
                continue;
            node.Update(deltaTime);
        }
    }

    public void StartCustom()
    {
        FindMyCamera();
        Init();
        LastClientPos = ClientTransform.position;
    }

    void OnDrawGizmos()
    {
        if (ClientTransform == null || !DrawDebug)
            return;

        Gizmos.color = DebugColor;

        float unitRadius = 0f;
        if (RenderType == 0)
            unitRadius = (SpriteWidth + SpriteHeight) / 6;
        else if (RenderType == 1)
            unitRadius = RibbonWidth / 3;
        else
            unitRadius = (ConeSize.x + ConeSize.y) / 6;
        unitRadius = Mathf.Clamp(unitRadius, 0, 1);
        //Visual Debug

        if (EmitType == 0 || EmitType == 3)
        {

            Gizmos.DrawWireSphere(ClientTransform.position + EmitPoint, unitRadius);
        }

        if (EmitType == 1)
        {
            Gizmos.DrawWireCube(ClientTransform.position + EmitPoint, BoxSize);
        }
        //else if (EmitType == 2)
        //{
        //    Gizmos.DrawWireSphere(ClientTransform.position + EmitPoint, Radius);
        //}
        else if (EmitType == 4)
        {
            //Gizmos.DrawSphere(ClientTransform.position + EmitPoint, Radius);
            Vector3 left = ClientTransform.position + EmitPoint + ClientTransform.forward * LineLengthLeft;
            Vector3 right = ClientTransform.position + EmitPoint + ClientTransform.forward * LineLengthRight;
            Gizmos.DrawLine(left, right);
        }
        else if (EmitType == 5)
        {
            //Graphics.DrawMeshNow(EmitMesh, Matrix4x4.identity);
        }

        if (OriVelocityAxis != Vector3.zero)
        {
            Gizmos.DrawLine(ClientTransform.position + EmitPoint, ClientTransform.position + EmitPoint + ClientTransform.rotation * OriVelocityAxis * unitRadius * 15);
        }
    }

    //added. 2012.6.3
    //Xffect LifeTime < 0， 且又是EmitByRate的话，会自动判断是否已经没有活动节点，没有则自动Deactive()。
    public bool EmitOver(float deltaTime)
    {
        if (ActiveENodes == null)
        {
            return false;
        }
        if (AvailableNodeCount == MaxENodes && EmitWay != EEmitWay.ByDistance)
        {
            if (EmitWay == EEmitWay.ByRate)
            {
                if (emitter.EmitLoop == 0)
                    return true;
            }
            else if (EmitWay == EEmitWay.ByCurve)
            {
                if (emitter.CurveEmitDone)
                    return true;
            }
        }

        return false;
    }
 
    public void StopEmit()
    {
        if (IsNodeLifeLoop)
        {
            for (int i = 0; i < MaxENodes; i++)
            {
                EffectNode node = ActiveENodes[i];
                if (node == null)
                    continue;
                node.Stop();
            }
        }
        emitter.StopEmit();
    }
    
    public void SetCollisionGoalPos(Transform pos)
    {
        if (UseCollisionDetection == false)
        {
            Debug.LogWarning(gameObject.name + "is not set to collision detect mode, please check it");
            return;
        }
        CollisionGoal = pos;
    }

    public void SetArractionAffectorGoal(Transform goal)
    {
        if (GravityAffectorEnable == false || GravityAftType == GAFTTYPE.Planar)
        {
            Debug.LogWarning(gameObject.name + "has no attraction affector, please check it");
            return;
        }
        foreach (EffectNode en in AvailableENodes)
        {
            List<Affector> affectorList = en.GetAffectorList();
            foreach (Affector aft in affectorList)
            {
                if (aft.Type == AFFECTORTYPE.GravityAffector)
                {
                    GravityAffector atft = (GravityAffector)aft;
                    atft.SetAttraction(goal);
                }
            }
        }

    }
    
    
    public void SetScale(Vector2 scale)
    {
        for (int i = 0; i < MaxENodes; i++)
        {
            EffectNode node = ActiveENodes[i];
            if (node == null)
                node = AvailableENodes[i];
            node.Scale = scale;
        }
    }

    // for out caller use.
    public EffectNode EmitByPos(Vector3 pos)
    {
        int added = 0;
        EffectNode ret = null;
        for (int i = 0; i < MaxENodes; i++)
        {
            if (added == 1)
                break;
            EffectNode node = AvailableENodes[i];
            if (node != null)
            {
                AddActiveNode(node);
                added++;

                node.SetLocalPosition(pos);
                float nodeLife = 0;
                if (IsNodeLifeLoop)
                    nodeLife = -1;
                else
                    nodeLife = Random.Range(NodeLifeMin, NodeLifeMax);
                Vector3 oriDir = emitter.GetEmitRotation(node);

                
                node.Init(oriDir.normalized, OriSpeed, nodeLife, Random.Range(OriRotationMin, OriRotationMax),
                    Random.Range(OriScaleXMin, OriScaleXMax), Random.Range(OriScaleYMin, OriScaleYMax), Color1, UVTopLeft, UVDimension);
                
                ret = node;
            }
            else
                continue;
        }
        return ret;
    }
}