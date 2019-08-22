using UnityEngine;
using System.Collections;
using UnityEditor;
using Xft;

[CustomEditor(typeof(EffectLayer))]
public class EffectLayerCustom : Editor
{
    public static bool DisplaySpriteConfig = false;
    public static bool DisplayRibbonConfig = false;
    public static bool DisplayConeConfig = false;
    public static bool DisplayCMeshConfig = false;
    public static bool DisplayOriginalVelocityConfig = false;
    public static bool DisplayEmitterConfig = false;
    public static bool DisplayUVConfig = false;
    public static bool DisplayRotateConfig = false;
    public static bool DisplayScaleConfig = false;
    public static bool DisplayColorConfig = false;

    public static bool DisplayAffectorConfig = false;
    public static bool DisplayJetAffectorConfig = false;
    public static bool DisplayVortexAffectorConfig = false;
    public static bool DisplayGravityAffectorConfig = false;
    public static bool DisplayAirAffectorConfig = false;
    public static bool DisplayBombAffectorConfig = false;
    public static bool DisplayTurbulenceAffectorConfig = false;
    public static bool DisplayDragAffectorConfig = false;

    public static bool DisplayCollisionConfig = false;

    public static float PreLableWidth = 150;

    string[] RenderTypes;
    string[] SpriteTypes;
    string[] SpriteUVStretchTypes;
    string[] OriginalPoint;
    string[] EmitTypes;
    string[] AffectorTypes;
    string[] UVTypes;
    string[] RotateTypes;
    string[] ScaleTypes;
    string[] ColorTypes;
    string[] ColorGradualTypes;
    string[] StretchTypes;
    string[] MagnitudeTypes;
    string[] EmitMeshType;
    
    int AffectorIndex;
    bool SameXY;

    public EffectLayerCustom()
    {
        RenderTypes = new string[4];
        RenderTypes[0] = "Sprite";
        RenderTypes[1] = "RibbonTrail";
        RenderTypes[2] = "Cone";
        RenderTypes[3] = "Custom Mesh";

        SpriteTypes = new string[3];
        SpriteTypes[0] = "Billboard";
        SpriteTypes[1] = "Billboard Self";
        SpriteTypes[2] = "XZ Plane";

        SpriteUVStretchTypes = new string[2];
        SpriteUVStretchTypes[0] = "VERTICAL";
        SpriteUVStretchTypes[1] = "HORIZONTAL";


        OriginalPoint = new string[9];
        OriginalPoint[0] = "Center";
        OriginalPoint[1] = "Left Up";
        OriginalPoint[2] = "Left Bottom";
        OriginalPoint[3] = "Right Bottom";
        OriginalPoint[4] = "Right Up";
        OriginalPoint[5] = "Bottom Center";
        OriginalPoint[6] = "Top Center";
        OriginalPoint[7] = "Left Center";
        OriginalPoint[8] = "Right Center";

        EmitTypes = new string[6];
        EmitTypes[0] = "Point";
        EmitTypes[1] = "Box";
        EmitTypes[2] = "Sphere Surface";
        EmitTypes[3] = "Circle";
        EmitTypes[4] = "Line";
        EmitTypes[5] = "Mesh";

        AffectorTypes = new string[7];
        AffectorTypes[0] = "GravityAffector";
        AffectorTypes[1] = "BombAffector";
        AffectorTypes[2] = "AirAffector";
        AffectorTypes[3] = "VortexAffector";
        AffectorTypes[4] = "JetAffector";
        AffectorTypes[5] = "TurbulenceAffector";
        AffectorTypes[6] = "DragAffector";

        UVTypes = new string[3];
        UVTypes[0] = "None";
        UVTypes[1] = "Build UV Animation";
        UVTypes[2] = "UV Scroll";

        RotateTypes = new string[3];
        RotateTypes[0] = "None";
        RotateTypes[1] = "Simple";
        RotateTypes[2] = "Curve";

        ScaleTypes = new string[3];
        ScaleTypes[0] = "None";
        ScaleTypes[1] = "Simple";
        ScaleTypes[2] = "Curve";

        ColorTypes = new string[3];
        ColorTypes[0] = "None";
        ColorTypes[1] = "2Gradual";
        ColorTypes[2] = "5Gradual";

        ColorGradualTypes = new string[3];
        ColorGradualTypes[0] = "Clamp";
        ColorGradualTypes[1] = "Loop";
        ColorGradualTypes[2] = "Reverse";

        StretchTypes = new string[2];
        StretchTypes[0] = "Up to Bottom";
        StretchTypes[1] = "Left to Right";

        MagnitudeTypes = new string[2];
        MagnitudeTypes[0] = "Fixed";
        MagnitudeTypes[1] = "Curve";

        EmitMeshType = new string[2];
        EmitMeshType[0] = "By Vertex";
        EmitMeshType[1] = "By Triangle";
    }

    public override void OnInspectorGUI()
    {
        EffectLayer ctarget = (EffectLayer)target;

        EditorGUIUtility.LookLikeControls(PreLableWidth);

        EditorGUILayout.BeginVertical();

        EditorGUILayout.Space();
        EditorGUILayout.BeginHorizontal();
        EditorGUILayout.PrefixLabel("render type:");
        ctarget.RenderType = EditorGUILayout.Popup(ctarget.RenderType, RenderTypes);
        EditorGUILayout.EndHorizontal();

        EditorGUILayout.Space();
        EditorGUILayout.BeginHorizontal();
        EditorGUILayout.PrefixLabel("client transform:");
        ctarget.ClientTransform = (Transform)EditorGUILayout.ObjectField(ctarget.ClientTransform, typeof(Transform),true);
        EditorGUILayout.EndHorizontal();


        EditorGUILayout.BeginHorizontal();
        EditorGUILayout.PrefixLabel("sync to client pos:");
        ctarget.SyncClient = EditorGUILayout.Toggle(ctarget.SyncClient);
        EditorGUILayout.EndHorizontal();

        EditorGUILayout.Space();
        EditorGUILayout.BeginHorizontal();
        EditorGUILayout.PrefixLabel("material :");
        ctarget.Material = (Material)EditorGUILayout.ObjectField(ctarget.Material, typeof(Material),false);
        EditorGUILayout.EndHorizontal();
        EditorGUILayout.BeginHorizontal();
        EditorGUILayout.PrefixLabel("render queue :");
        ctarget.Depth = EditorGUILayout.IntField(ctarget.Depth);
        EditorGUILayout.EndHorizontal();

        EditorGUILayout.Space(); 
        EditorGUILayout.BeginHorizontal();
        ctarget.StartTime = EditorGUILayout.FloatField("delay(s):", ctarget.StartTime);
        EditorGUILayout.EndHorizontal();

        EditorGUILayout.Space();
        EditorGUILayout.BeginHorizontal();
        ctarget.MaxFps = EditorGUILayout.FloatField("Max FPS:", ctarget.MaxFps);
        EditorGUILayout.EndHorizontal();
        EditorGUILayout.Space();
        EditorGUILayout.BeginHorizontal();
        ctarget.DrawDebug = EditorGUILayout.Toggle("Draw Debug?", ctarget.DrawDebug);
        EditorGUILayout.EndHorizontal();

        if (ctarget.DrawDebug)
        {
            EditorGUILayout.BeginHorizontal();
            ctarget.DebugColor = EditorGUILayout.ColorField("debug color:", ctarget.DebugColor);
            EditorGUILayout.EndHorizontal();
        }

        EditorGUILayout.Space();
        EditorGUILayout.Separator();
        if (ctarget.RenderType == 0)
            SpriteConfig(ctarget);
        else if (ctarget.RenderType == 1)
            RibbonTrailConfig(ctarget);
        else if (ctarget.RenderType == 2)
            ConeConfig(ctarget);
        else if (ctarget.RenderType == 3)
            CustomMeshConfig(ctarget);
        if (ctarget.RenderType == 0 || ctarget.RenderType == 2 || ctarget.RenderType == 3)
        {
            EditorGUILayout.Space();
            EditorGUILayout.Separator();
            RotateConfig(ctarget);
            EditorGUILayout.Space();
            EditorGUILayout.Separator();
            ScaleConfig(ctarget);
        }
        EditorGUILayout.Space();
        EditorGUILayout.Separator();
        ColorConfig(ctarget);
        
        if (ctarget.RenderType != 3)
        {
            EditorGUILayout.Space();
            EditorGUILayout.Separator();
            UVConfig(ctarget);
        }

        EditorGUILayout.Space();
        EditorGUILayout.Separator();
        EmitterConfig(ctarget);
        EditorGUILayout.Space();
        EditorGUILayout.Separator();
        OriginalVelocityConfig(ctarget);
        EditorGUILayout.Space();
        EditorGUILayout.Separator();
        AffectorConfig(ctarget);
        EditorGUILayout.Space();
        EditorGUILayout.Separator();

        CollisionConfig(ctarget);

        EditorGUILayout.Space();
        EditorGUILayout.Separator();
        
        EditorGUILayout.EndVertical();

        if (GUI.changed)
        {
            EditorUtility.SetDirty(target);
        }
    }

    protected void SpriteConfig(EffectLayer ctarget)
    {
        DisplaySpriteConfig = EditorGUILayout.Foldout(DisplaySpriteConfig, "Sprite Configuration");

        //DisplaySpriteConfig = EditorGUILayout.InspectorTitlebar(DisplaySpriteConfig, SpriteTitle);

        if (DisplaySpriteConfig)
        {
            EditorGUILayout.BeginVertical();
            
            ctarget.SpriteType = EditorGUILayout.Popup("sprite type:",ctarget.SpriteType, SpriteTypes);
            if (ctarget.SpriteType == 1)
            {//billboard self
                ctarget.SpriteUVStretch = EditorGUILayout.Popup("uv stretch:", ctarget.SpriteUVStretch, SpriteUVStretchTypes);
            }
            ctarget.OriPoint = EditorGUILayout.Popup("original point:", ctarget.OriPoint, OriginalPoint);
            ctarget.SpriteWidth = EditorGUILayout.FloatField("width:", ctarget.SpriteWidth);
            ctarget.SpriteHeight = EditorGUILayout.FloatField("height:", ctarget.SpriteHeight);
            if (ctarget.SpriteType == 1 || ctarget.SpriteType == 2)
                EditorGUILayout.LabelField("[sprite face dir is based on Direction config.]");
            EditorGUILayout.EndVertical();
        }
    }

    protected void RotateConfig(EffectLayer ctarget)
    {
        DisplayRotateConfig = EditorGUILayout.Foldout(DisplayRotateConfig, "Rotate Configuration");

        if (DisplayRotateConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.RandomOriRot = EditorGUILayout.Toggle("random original rotation?", ctarget.RandomOriRot);
            if (ctarget.RandomOriRot)
            {
                ctarget.OriRotationMin = EditorGUILayout.IntField("original rotation min:", ctarget.OriRotationMin);
                ctarget.OriRotationMax = EditorGUILayout.IntField("original rotation max:", ctarget.OriRotationMax);
            }
            else
            {
                ctarget.OriRotationMin = EditorGUILayout.IntField("original rotation:", ctarget.OriRotationMin);
                ctarget.OriRotationMax = ctarget.OriRotationMin;
            }
            EditorGUILayout.Space();
            ctarget.RotateType = (RSTYPE)EditorGUILayout.Popup("rotate change type", (int)ctarget.RotateType, RotateTypes);
            if (ctarget.RotateType == RSTYPE.NONE)
            {
                ctarget.RotAffectorEnable = false;
            }
            else if (ctarget.RotateType == RSTYPE.SIMPLE)
            {
                ctarget.RotAffectorEnable = true;
                ctarget.DeltaRot = EditorGUILayout.FloatField("delta angle per second:", ctarget.DeltaRot);
                if (ctarget.RenderType == 0 && ctarget.SpriteType == 1)
                    EditorGUILayout.LabelField("warning: billboard self is not recommended to rotate. It may not work properly.");
            }
            else
            {
                ctarget.RotAffectorEnable = true;
                ctarget.RotateCurve = EditorGUILayout.CurveField("rotate by curve:", ctarget.RotateCurve);
                if (ctarget.RenderType == 0 && ctarget.SpriteType == 1)
                    EditorGUILayout.LabelField("warning: billboard self is not recommended to rotate. It may not work properly.");
            }
            EditorGUILayout.EndVertical();
        }
    }

    protected void ColorConfig(EffectLayer ctarget)
    {
        DisplayColorConfig = EditorGUILayout.Foldout(DisplayColorConfig, "Color Configuration");

        if (DisplayColorConfig)
        {
            EditorGUILayout.BeginVertical();

            EditorGUILayout.Space();
            ctarget.ColorAffectType = EditorGUILayout.Popup("color change type:", ctarget.ColorAffectType, ColorTypes);
            if (ctarget.ColorAffectType == 0)
            {
                ctarget.Color1 = EditorGUILayout.ColorField("original color:", ctarget.Color1);
                ctarget.ColorAffectorEnable = false;
            }
            else if (ctarget.ColorAffectType == 1)
            {
                ctarget.ColorAffectorEnable = true;
                ctarget.ColorGradualTimeLength = EditorGUILayout.FloatField("gradual time(-1 node life):", ctarget.ColorGradualTimeLength);
                ctarget.ColorGradualType = (COLOR_GRADUAL_TYPE)EditorGUILayout.Popup("gradual type:", (int)ctarget.ColorGradualType, ColorGradualTypes);
                EditorGUILayout.Space();
                ctarget.Color1 = EditorGUILayout.ColorField("color1:", ctarget.Color1);
                ctarget.Color2 = EditorGUILayout.ColorField("color2:", ctarget.Color2);
            }
            else
            {
                ctarget.ColorAffectorEnable = true;
                ctarget.ColorGradualTimeLength = EditorGUILayout.FloatField("gradual time(-1 node life):", ctarget.ColorGradualTimeLength);
                ctarget.ColorGradualType = (COLOR_GRADUAL_TYPE)EditorGUILayout.Popup("gradual type:", (int)ctarget.ColorGradualType, ColorGradualTypes);
                EditorGUILayout.Space();
                ctarget.Color1 = EditorGUILayout.ColorField("color1:", ctarget.Color1);
                ctarget.Color2 = EditorGUILayout.ColorField("color2:", ctarget.Color2);
                ctarget.Color3 = EditorGUILayout.ColorField("color3:", ctarget.Color3);
                ctarget.Color4 = EditorGUILayout.ColorField("color4:", ctarget.Color4);
                ctarget.Color5 = EditorGUILayout.ColorField("color5:", ctarget.Color5);
            }
            EditorGUILayout.EndVertical();
        }
    }

    protected void ScaleConfig(EffectLayer ctarget)
    {
        DisplayScaleConfig = EditorGUILayout.Foldout(DisplayScaleConfig, "Scale Configuration");

        if (DisplayScaleConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.RandomOriScale = EditorGUILayout.Toggle("random original scale?", ctarget.RandomOriScale);
            if (ctarget.RandomOriScale)
            {
                ctarget.OriScaleXMin  = EditorGUILayout.FloatField("original x scale min:", ctarget.OriScaleXMin);
                ctarget.OriScaleXMax = EditorGUILayout.FloatField("original x scale max:", ctarget.OriScaleXMax);
                EditorGUILayout.Space();
                ctarget.OriScaleYMin = EditorGUILayout.FloatField("original y scale min:", ctarget.OriScaleYMin);
                ctarget.OriScaleYMax = EditorGUILayout.FloatField("original y scale max:", ctarget.OriScaleYMax);
            }
            else
            {
                ctarget.OriScaleXMin = EditorGUILayout.FloatField("original x scale:", ctarget.OriScaleXMin);
                ctarget.OriScaleYMin = EditorGUILayout.FloatField("original y scale:", ctarget.OriScaleYMin);
                ctarget.OriScaleXMax = ctarget.OriScaleXMin;
                ctarget.OriScaleYMax = ctarget.OriScaleYMin;
            }
            EditorGUILayout.Space();
            ctarget.ScaleType = (RSTYPE)EditorGUILayout.Popup("scale change type:", (int)ctarget.ScaleType, ScaleTypes);
            if (ctarget.ScaleType == RSTYPE.NONE)
            {
                ctarget.ScaleAffectorEnable = false;
            }
            else if (ctarget.ScaleType == RSTYPE.SIMPLE)
            {
                ctarget.ScaleAffectorEnable = true;
                ctarget.DeltaScaleX = EditorGUILayout.FloatField("delta scaleX per second:", ctarget.DeltaScaleX);
                ctarget.DeltaScaleY = EditorGUILayout.FloatField("delta scaleY per second:", ctarget.DeltaScaleY);
            }
            else
            {
                ctarget.ScaleAffectorEnable = true;
                
                ctarget.UseSameScaleCurve = EditorGUILayout.Toggle("use same curve?",ctarget.UseSameScaleCurve);
                if (ctarget.UseSameScaleCurve)
                {
                    ctarget.ScaleXCurve = EditorGUILayout.CurveField("scaleXY curve:", ctarget.ScaleXCurve);
                }
                else
                {
                    ctarget.ScaleXCurve = EditorGUILayout.CurveField("scaleX curve:", ctarget.ScaleXCurve);
                    ctarget.ScaleYCurve = EditorGUILayout.CurveField("scaleY curve:", ctarget.ScaleYCurve);
                }
            }
            EditorGUILayout.EndVertical();
        }
    }

    protected void ConeConfig(EffectLayer ctarget)
    {
        DisplayConeConfig = EditorGUILayout.Foldout(DisplayConeConfig, "Cone Configuration");
        if (DisplayConeConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.ConeSize = EditorGUILayout.Vector2Field("size:", ctarget.ConeSize);
            ctarget.ConeSegment = EditorGUILayout.IntField("segment(>=3)", ctarget.ConeSegment);
            ctarget.ConeAngle = EditorGUILayout.FloatField("angle(0-90)", ctarget.ConeAngle);
            ctarget.UseConeAngleChange = EditorGUILayout.Toggle("angle change?", ctarget.UseConeAngleChange);
            if (ctarget.UseConeAngleChange)
                ctarget.ConeDeltaAngle = EditorGUILayout.CurveField("angle change curve", ctarget.ConeDeltaAngle);
            EditorGUILayout.EndVertical();
        }
    }
 
    protected void CustomMeshConfig(EffectLayer ctarget)
    {
        DisplayCMeshConfig = EditorGUILayout.Foldout(DisplayCMeshConfig, "Custom Mesh Configuration");
        if (DisplayCMeshConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.CMesh = (Mesh)EditorGUILayout.ObjectField("custom mesh:",ctarget.CMesh, typeof(Mesh),false);
            EditorGUILayout.EndVertical();
        }
    }
    
    protected void RibbonTrailConfig(EffectLayer ctarget)
    {
        DisplayRibbonConfig = EditorGUILayout.Foldout(DisplayRibbonConfig, "RibbonTrail Configuration");

        if (DisplayRibbonConfig)
        {
            EditorGUILayout.BeginVertical();

            ctarget.UseRandomRibbon = EditorGUILayout.Toggle("use random?", ctarget.UseRandomRibbon);
            if (!ctarget.UseRandomRibbon)
            {
                ctarget.RibbonWidth = EditorGUILayout.FloatField("width:", ctarget.RibbonWidth);
                ctarget.RibbonLen = EditorGUILayout.FloatField("trail length:", ctarget.RibbonLen);
            }
            else
            {
                ctarget.RibbonWidthMin = EditorGUILayout.FloatField("width min:", ctarget.RibbonWidthMin);
                ctarget.RibbonWidthMax = EditorGUILayout.FloatField("width max:", ctarget.RibbonWidthMax);
                ctarget.RibbonLenMin = EditorGUILayout.FloatField("length min:", ctarget.RibbonLenMin);
                ctarget.RibbonLenMax = EditorGUILayout.FloatField("length max:", ctarget.RibbonLenMax);
            }
            ctarget.MaxRibbonElements = EditorGUILayout.IntField("max elements(>=3):", ctarget.MaxRibbonElements);
            ctarget.StretchType = EditorGUILayout.Popup("uv stretch type:", ctarget.StretchType,StretchTypes);
            //ctarget.TailDistance = EditorGUILayout.FloatField("tail distance:", ctarget.TailDistance);
            ctarget.FaceToObject = EditorGUILayout.Toggle("slash trail?", ctarget.FaceToObject);
            if (ctarget.FaceToObject)
            {
                ctarget.FaceObject = (Transform)EditorGUILayout.ObjectField("face to object:", ctarget.FaceObject, typeof(Transform), true);
            }
            EditorGUILayout.EndVertical();
        }
    }

    protected void EmitterConfig(EffectLayer ctarget)
    {
        DisplayEmitterConfig = EditorGUILayout.Foldout(DisplayEmitterConfig, "Emitter Configuration");

        if (DisplayEmitterConfig)
        {
            EditorGUILayout.BeginVertical();

            ctarget.EmitType = EditorGUILayout.Popup("emitter type:", ctarget.EmitType, EmitTypes);
            if (ctarget.EmitType == 0)
            {
                ctarget.EmitPoint = EditorGUILayout.Vector3Field("emit position:", ctarget.EmitPoint);
            }
            else if (ctarget.EmitType == 1)
            {
                ctarget.EmitPoint = EditorGUILayout.Vector3Field("box center:", ctarget.EmitPoint);
                ctarget.BoxSize = EditorGUILayout.Vector3Field("box size:", ctarget.BoxSize);
                ctarget.BoxInheritRotation = EditorGUILayout.Toggle("inherit rotation?", ctarget.BoxInheritRotation);
            }
            else if (ctarget.EmitType == 2)
            {
                ctarget.EmitPoint = EditorGUILayout.Vector3Field("sphere center:", ctarget.EmitPoint);
                ctarget.Radius = EditorGUILayout.FloatField("radius:", ctarget.Radius);
            }
            else if (ctarget.EmitType == 3)
            {
                ctarget.EmitPoint = EditorGUILayout.Vector3Field("circle center:", ctarget.EmitPoint);
                ctarget.CircleDir = EditorGUILayout.Vector3Field("circle direction:", ctarget.CircleDir);
                ctarget.Radius = EditorGUILayout.FloatField("radius:", ctarget.Radius);
                ctarget.EmitUniform = EditorGUILayout.Toggle("emit uniformly?", ctarget.EmitUniform);
            }
            else if (ctarget.EmitType == 4)
            {
                ctarget.EmitPoint = EditorGUILayout.Vector3Field("line center:", ctarget.EmitPoint);
                ctarget.LineLengthLeft = EditorGUILayout.FloatField("line left length:", ctarget.LineLengthLeft);
                ctarget.LineLengthRight = EditorGUILayout.FloatField("line right length:", ctarget.LineLengthRight);
                EditorGUILayout.LabelField("[line direction is based on client's forward direction.]");
            }
            else if (ctarget.EmitType == 5)
            {
                ctarget.EmitMesh = (Mesh)EditorGUILayout.ObjectField("mesh",ctarget.EmitMesh, typeof(Mesh),true);
                ctarget.EmitMeshType = EditorGUILayout.Popup("mesh emit pos:", ctarget.EmitMeshType, EmitMeshType);
                ctarget.EmitUniform = EditorGUILayout.Toggle("emit uniform?", ctarget.EmitUniform);
            }
            EditorGUILayout.Space();

            ctarget.MaxENodes = EditorGUILayout.IntField("max nodes:", ctarget.MaxENodes);
            ctarget.IsNodeLifeLoop = EditorGUILayout.Toggle("is node life loop:", ctarget.IsNodeLifeLoop);
            if (!ctarget.IsNodeLifeLoop)
            {
                ctarget.NodeLifeMin = EditorGUILayout.FloatField("node life min:", ctarget.NodeLifeMin);
                ctarget.NodeLifeMax = EditorGUILayout.FloatField("node life max:", ctarget.NodeLifeMax);
            }

            //ctarget.IsEmitByDistance = EditorGUILayout.Toggle("emit by distance:", ctarget.IsEmitByDistance);
            
            ctarget.EmitWay = (EEmitWay)EditorGUILayout.EnumPopup("emit method:",ctarget.EmitWay);
            
            if (ctarget.EmitWay == EEmitWay.ByDistance)
            {
                ctarget.DiffDistance = EditorGUILayout.FloatField("diff distance:", ctarget.DiffDistance);
            }
            else if (ctarget.EmitWay == EEmitWay.ByRate)
            {
                ctarget.ChanceToEmit = EditorGUILayout.Slider("chance to emit per loop:", ctarget.ChanceToEmit, 1, 100);
                ctarget.EmitDuration = EditorGUILayout.FloatField("emit duration:", ctarget.EmitDuration);
                ctarget.EmitRate = EditorGUILayout.FloatField("emit rate:", ctarget.EmitRate);
                ctarget.EmitLoop = EditorGUILayout.IntField("loop count(-1 is infinite):", ctarget.EmitLoop);
                ctarget.EmitDelay = EditorGUILayout.FloatField("delay after each loop:", ctarget.EmitDelay);
            }
            else
            {
                ctarget.EmitterCurve = EditorGUILayout.CurveField("emit curve:",ctarget.EmitterCurve);
            }
            EditorGUILayout.EndVertical();
        }
    }
    protected void OriginalVelocityConfig(EffectLayer ctarget)
    {
        DisplayOriginalVelocityConfig = EditorGUILayout.Foldout(DisplayOriginalVelocityConfig, "Direction Configuration");

        if (DisplayOriginalVelocityConfig)
        {
            EditorGUILayout.BeginVertical();

            ctarget.DirType = (DIRECTION_TYPE)EditorGUILayout.EnumPopup("direction type:", ctarget.DirType);

            if (ctarget.DirType == DIRECTION_TYPE.Planar)
            {
                ctarget.OriVelocityAxis = EditorGUILayout.Vector3Field("original direction:", ctarget.OriVelocityAxis);
                
            }
            else if (ctarget.DirType == DIRECTION_TYPE.Sphere)
            {
                ctarget.DirCenter = (Transform)EditorGUILayout.ObjectField("dir center",ctarget.DirCenter, typeof(Transform),true);
            }
            else if (ctarget.DirType == DIRECTION_TYPE.Cone)
            {
                ctarget.OriVelocityAxis = EditorGUILayout.Vector3Field("cone axis:", ctarget.OriVelocityAxis);
                ctarget.AngleAroundAxis = EditorGUILayout.IntField("angle around axis:", ctarget.AngleAroundAxis);
            }
            else if (ctarget.DirType == DIRECTION_TYPE.Cylindrical)
            {
                ctarget.OriVelocityAxis = EditorGUILayout.Vector3Field("cylindrical axis:", ctarget.OriVelocityAxis);
                ctarget.DirCenter = (Transform)EditorGUILayout.ObjectField("dir center", ctarget.DirCenter, typeof(Transform), true);
            }

            ctarget.IsRandomSpeed = EditorGUILayout.Toggle("random speed?", ctarget.IsRandomSpeed);

            if (ctarget.IsRandomSpeed)
            {
                ctarget.SpeedMin = EditorGUILayout.FloatField("speed min: ", ctarget.SpeedMin);
                ctarget.SpeedMax = EditorGUILayout.FloatField("speed max: ", ctarget.SpeedMax);
            }
            else
            {
                ctarget.OriSpeed = EditorGUILayout.FloatField("original speed:", ctarget.OriSpeed);
            }
            ctarget.AlwaysSyncRotation = EditorGUILayout.Toggle("always sync to client rot?", ctarget.AlwaysSyncRotation);
            EditorGUILayout.EndVertical();
        }
    }



    protected void CollisionConfig(EffectLayer ctarget)
    {
        DisplayCollisionConfig = EditorGUILayout.Foldout(DisplayCollisionConfig, "Collision Configuration");
        if (DisplayCollisionConfig)
        {
            EditorGUILayout.BeginVertical();

            ctarget.UseCollisionDetection = EditorGUILayout.Toggle("use collision?", ctarget.UseCollisionDetection);

            if (ctarget.UseCollisionDetection)
            {
                ctarget.ParticleRadius = EditorGUILayout.FloatField("node radius:", ctarget.ParticleRadius);
                ctarget.CollisionAutoDestroy = EditorGUILayout.Toggle("auto destroy:", ctarget.CollisionAutoDestroy);
                EditorGUILayout.Separator();
                ctarget.CollisionType = (COLLITION_TYPE)EditorGUILayout.EnumPopup("collision type:", ctarget.CollisionType);
                if (ctarget.CollisionType == COLLITION_TYPE.Sphere)
                {
                    ctarget.CollisionGoal = (Transform)EditorGUILayout.ObjectField("collision goal:", ctarget.CollisionGoal, typeof(Transform), true);
                    ctarget.ColliisionPosRange = EditorGUILayout.FloatField("goal radius:", ctarget.ColliisionPosRange);
                }
                else
                {
                    ctarget.CollisionLayer = EditorGUILayout.LayerField("layer mask:", ctarget.CollisionLayer);
                }
                EditorGUILayout.Separator();
                ctarget.EventReceiver = (Transform)EditorGUILayout.ObjectField("event receiver:", ctarget.EventReceiver, typeof(Transform), true);
                ctarget.EventHandleFunctionName = EditorGUILayout.TextField("event handle function:", ctarget.EventHandleFunctionName);
            }

            EditorGUILayout.EndVertical();
        }
    }


    protected void AffectorConfig(EffectLayer ctarget)
    {
        DisplayAffectorConfig = EditorGUILayout.Foldout(DisplayAffectorConfig, "Affector Configuration");


        DisplayJetAffectorConfig = ctarget.JetAffectorEnable;
        DisplayVortexAffectorConfig = ctarget.VortexAffectorEnable;
        DisplayGravityAffectorConfig = ctarget.GravityAffectorEnable;
        DisplayAirAffectorConfig = ctarget.AirAffectorEnable;
        DisplayBombAffectorConfig = ctarget.BombAffectorEnable;
        DisplayTurbulenceAffectorConfig = ctarget.TurbulenceAffectorEnable;
        DisplayDragAffectorConfig = ctarget.DragAffectorEnable;
        
        if (DisplayAffectorConfig)
        {
            EditorGUILayout.BeginVertical();
            EditorGUILayout.PrefixLabel("affector types:");
            ctarget.AffectorIndex = EditorGUILayout.Popup(ctarget.AffectorIndex, AffectorTypes);

            AffectorIndex = ctarget.AffectorIndex;

            EditorGUILayout.BeginHorizontal();
            if (GUILayout.Button("Add Affector"))
            {
                if (AffectorIndex == 0)
                    DisplayGravityAffectorConfig = true;
                else if (AffectorIndex == 1)
                    DisplayBombAffectorConfig = true;
                else if (AffectorIndex == 2)
                    DisplayAirAffectorConfig = true;
                else if (AffectorIndex == 3)
                    DisplayVortexAffectorConfig = true;
                else if (AffectorIndex == 4)
                    DisplayJetAffectorConfig = true;
                else if (AffectorIndex == 5)
                    DisplayTurbulenceAffectorConfig = true;
                else if (AffectorIndex == 6)
                    DisplayDragAffectorConfig = true;
            }
            if (GUILayout.Button("Delete Affector"))
            {
                if (AffectorIndex == 0)
                    DisplayGravityAffectorConfig = false;
                else if (AffectorIndex == 1)
                    DisplayBombAffectorConfig = false;
                else if (AffectorIndex == 2)
                    DisplayAirAffectorConfig = false;
                else if (AffectorIndex == 3)
                    DisplayVortexAffectorConfig = false;
                else if (AffectorIndex == 4)
                    DisplayJetAffectorConfig = false;
                else if (AffectorIndex == 5)
                    DisplayTurbulenceAffectorConfig = false;
                else if (AffectorIndex == 6)
                    DisplayDragAffectorConfig = false;
            }
            EditorGUILayout.EndHorizontal();

            if (DisplayGravityAffectorConfig)
                GravityAffectorConfig(ctarget);
            if (DisplayBombAffectorConfig)
                BombAffectorConfig(ctarget);
            if (DisplayAirAffectorConfig)
                AirAffectorConfig(ctarget);
            if (DisplayVortexAffectorConfig)
                VortexAffectorConfig(ctarget);
            if (DisplayJetAffectorConfig)
                JetAffectorConfig(ctarget);
            if (DisplayTurbulenceAffectorConfig)
                TurbulenceAffectorConfig(ctarget);
            if (DisplayDragAffectorConfig)
                DragAffectorConfig(ctarget);

            ctarget.GravityAffectorEnable = DisplayGravityAffectorConfig;
            ctarget.AirAffectorEnable = DisplayAirAffectorConfig;
            ctarget.BombAffectorEnable = DisplayBombAffectorConfig;
            ctarget.JetAffectorEnable = DisplayJetAffectorConfig;
            ctarget.VortexAffectorEnable = DisplayVortexAffectorConfig;
            ctarget.TurbulenceAffectorEnable = DisplayTurbulenceAffectorConfig;
            ctarget.DragAffectorEnable = DisplayDragAffectorConfig;
            EditorGUILayout.EndVertical();
        }
    }
 
    protected void DragAffectorConfig(EffectLayer ctarget)
    {
        DisplayDragAffectorConfig = EditorGUILayout.Foldout(DisplayDragAffectorConfig, "Drag Affector Configuration");
        if (DisplayDragAffectorConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.DragObj = (Transform)EditorGUILayout.ObjectField("drag object:", ctarget.DragObj, typeof(Transform), true);
            ctarget.DragUseDir = EditorGUILayout.Toggle("use direction?", ctarget.DragUseDir);
            if (ctarget.DragUseDir)
                ctarget.DragDir = EditorGUILayout.Vector3Field("direction:", ctarget.DragDir);
            ctarget.DragUseMaxDist = EditorGUILayout.Toggle("use max distance?", ctarget.DragUseMaxDist);
            if (ctarget.DragUseMaxDist)
                ctarget.DragMaxDist = EditorGUILayout.FloatField("max distance:", ctarget.DragMaxDist);
            ctarget.DragMag = EditorGUILayout.FloatField("magnitude:", ctarget.DragMag);
            ctarget.DragAtten = EditorGUILayout.FloatField("attenuation:", ctarget.DragAtten);
            EditorGUILayout.EndVertical();
        }
    }

    protected void TurbulenceAffectorConfig(EffectLayer ctarget)
    {
        DisplayTurbulenceAffectorConfig = EditorGUILayout.Foldout(DisplayTurbulenceAffectorConfig, "Turbulence Affector Configuration");
        if (DisplayTurbulenceAffectorConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.TurbulenceObject = (Transform)EditorGUILayout.ObjectField("turbulence center:", ctarget.TurbulenceObject, typeof(Transform), true);
            ctarget.TurbulenceMagType = (MAGTYPE)EditorGUILayout.EnumPopup("magnitude type:", ctarget.TurbulenceMagType);
            if (ctarget.TurbulenceMagType == MAGTYPE.Fixed)
                ctarget.TurbulenceMagnitude = EditorGUILayout.FloatField("magnitude:", ctarget.TurbulenceMagnitude);
            else
                ctarget.TurbulenceMagCurve = EditorGUILayout.CurveField("magnitude curve:", ctarget.TurbulenceMagCurve);
            ctarget.TurbulenceUseMaxDistance = EditorGUILayout.Toggle("use max distance:", ctarget.TurbulenceUseMaxDistance);
            if (ctarget.TurbulenceUseMaxDistance)
                ctarget.TurbulenceMaxDistance = EditorGUILayout.FloatField("turbulence max distance:", ctarget.TurbulenceMaxDistance);
            ctarget.TurbulenceAttenuation = EditorGUILayout.Slider("turbulence attenuation:", ctarget.TurbulenceAttenuation, 0f, 1f);
            EditorGUILayout.EndVertical();
        }
    }

    protected void JetAffectorConfig(EffectLayer ctarget)
    {
        DisplayJetAffectorConfig = EditorGUILayout.Foldout(DisplayJetAffectorConfig, "Jet Force Affector Configuration");
        if (DisplayJetAffectorConfig)
        {
            EditorGUILayout.BeginVertical();
            
            ctarget.JetMagType = (MAGTYPE)EditorGUILayout.EnumPopup("magnitude type:", ctarget.JetMagType);
            if (ctarget.JetMagType == MAGTYPE.Fixed)
            {
                ctarget.JetMag = EditorGUILayout.FloatField("jet magnitude:", ctarget.JetMag);
            }
            else
            {
                ctarget.JetCurve = EditorGUILayout.CurveField("magnitude curve:", ctarget.JetCurve);
            }
            EditorGUILayout.EndVertical();
        }
    }

    protected void VortexAffectorConfig(EffectLayer ctarget)
    {
        DisplayVortexAffectorConfig = EditorGUILayout.Foldout(DisplayVortexAffectorConfig, "Vortex Affector Configuration");
        if (DisplayVortexAffectorConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.VortexObj = (Transform)EditorGUILayout.ObjectField("vortex object:", ctarget.VortexObj, typeof(Transform), true);
            ctarget.IsRandomVortexDir = EditorGUILayout.Toggle("random direction?", ctarget.IsRandomVortexDir);
            if (!ctarget.IsRandomVortexDir)
            {
                ctarget.VortexDirection = EditorGUILayout.Vector3Field("direction:", ctarget.VortexDirection);
            }
            ctarget.VortexInheritRotation = EditorGUILayout.Toggle("inherit rotation?", ctarget.VortexInheritRotation);
            ctarget.VortexMagType = (MAGTYPE)EditorGUILayout.EnumPopup("magnitude type:", ctarget.VortexMagType);
            if (ctarget.VortexMagType == MAGTYPE.Curve)
            {
                ctarget.VortexCurve = EditorGUILayout.CurveField("magnitude curve:", ctarget.VortexCurve);
            }
            else
            {
                ctarget.VortexMag = EditorGUILayout.FloatField("magnitude:", ctarget.VortexMag);
            }
			ctarget.UseVortexMaxDistance = EditorGUILayout.Toggle("use max distance:", ctarget.UseVortexMaxDistance);
			if (ctarget.UseVortexMaxDistance)
				ctarget.VortexMaxDistance = EditorGUILayout.FloatField("max distance:", ctarget.VortexMaxDistance);
			ctarget.VortexAttenuation = EditorGUILayout.Slider("attenuation",ctarget.VortexAttenuation,0f,1f);
			ctarget.IsVortexAccelerate = EditorGUILayout.Toggle("is accelerate?", ctarget.IsVortexAccelerate);

            EditorGUILayout.EndVertical();
        }
    }

    protected void BombAffectorConfig(EffectLayer ctarget)
    {
        DisplayBombAffectorConfig = EditorGUILayout.Foldout(DisplayBombAffectorConfig, "Bomb Affector Configuration");
        if (DisplayBombAffectorConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.BombObject = (Transform)EditorGUILayout.ObjectField("bomb center:", ctarget.BombObject, typeof(Transform), true);
            ctarget.BombType = (BOMBTYPE)EditorGUILayout.EnumPopup("bomb type:", ctarget.BombType);

            if (ctarget.BombType != BOMBTYPE.Spherical)
                ctarget.BombAxis = EditorGUILayout.Vector3Field("bomb axis:", ctarget.BombAxis);
            ctarget.BombMagType = (MAGTYPE)EditorGUILayout.EnumPopup("magnitude type:", ctarget.BombMagType);
            if (ctarget.BombMagType == MAGTYPE.Fixed)
                ctarget.BombMagnitude = EditorGUILayout.FloatField("magnitude:", ctarget.BombMagnitude);
            else
                ctarget.BombMagCurve = EditorGUILayout.CurveField("magnitude curve:", ctarget.BombMagCurve);

            ctarget.BombDecayType = (BOMBDECAYTYPE)EditorGUILayout.EnumPopup("decay type:", ctarget.BombDecayType);
            if (ctarget.BombDecayType != BOMBDECAYTYPE.None)
                ctarget.BombDecay = EditorGUILayout.FloatField("bomb decay distance:", ctarget.BombDecay);
            EditorGUILayout.EndVertical();
        }
    }

    protected void AirAffectorConfig(EffectLayer ctarget)
    {
        DisplayAirAffectorConfig = EditorGUILayout.Foldout(DisplayAirAffectorConfig, "Air Affector Configuration");
        if (DisplayAirAffectorConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.AirObject = (Transform)EditorGUILayout.ObjectField("air object:", ctarget.AirObject, typeof(Transform), true);
            ctarget.AirDirection = EditorGUILayout.Vector3Field("air direction:", ctarget.AirDirection);
            ctarget.AirMagType = (MAGTYPE)EditorGUILayout.EnumPopup("magnitude type:", ctarget.AirMagType);
            if (ctarget.AirMagType == MAGTYPE.Fixed)
                ctarget.AirMagnitude = EditorGUILayout.FloatField("magnitude:", ctarget.AirMagnitude);
            else
                ctarget.AirMagCurve = EditorGUILayout.CurveField("magnitude curve:", ctarget.AirMagCurve);
            ctarget.AirUseMaxDistance = EditorGUILayout.Toggle("use max distance:", ctarget.AirUseMaxDistance);
            if (ctarget.AirUseMaxDistance)
                ctarget.AirMaxDistance = EditorGUILayout.FloatField("air max distance:", ctarget.AirMaxDistance);
            ctarget.AirAttenuation = EditorGUILayout.FloatField("attenuation:", ctarget.AirAttenuation);
            ctarget.AirEnableSpread = EditorGUILayout.Toggle("enable spread:", ctarget.AirEnableSpread);
            if (ctarget.AirEnableSpread)
                ctarget.AirSpread = EditorGUILayout.Slider("air spread:", ctarget.AirSpread, 0f, 1f);
            ctarget.AirInheritVelocity = EditorGUILayout.Slider("inherit velocity:", ctarget.AirInheritVelocity, 0f, 1f);
            ctarget.AirInheritRotation = EditorGUILayout.Toggle("inherit rotation:", ctarget.AirInheritRotation);
            
            EditorGUILayout.EndVertical();
        }
    }


    protected void GravityAffectorConfig(EffectLayer ctarget)
    {
        DisplayGravityAffectorConfig = EditorGUILayout.Foldout(DisplayGravityAffectorConfig, "Gravity Affector Configuration");
        if (DisplayGravityAffectorConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.GravityAftType = (GAFTTYPE)EditorGUILayout.EnumPopup("Gravity Type:", ctarget.GravityAftType);
            ctarget.GravityObject = (Transform)EditorGUILayout.ObjectField("gravity object:", ctarget.GravityObject, typeof(Transform), true);
            if (ctarget.GravityAftType == GAFTTYPE.Planar)
            {
                ctarget.GravityDirection = EditorGUILayout.Vector3Field("direction:", ctarget.GravityDirection);
                ctarget.GravityMagType = (MAGTYPE)EditorGUILayout.EnumPopup("magnitude type:", ctarget.GravityMagType);
                if (ctarget.GravityMagType == MAGTYPE.Fixed)
                    ctarget.GravityMag = EditorGUILayout.FloatField("magnitude:", ctarget.GravityMag);
                else
                    ctarget.GravityCurve = EditorGUILayout.CurveField("magnitude curve:", ctarget.GravityCurve);
            }
            else
            {
                ctarget.GravityMagType = (MAGTYPE)EditorGUILayout.EnumPopup("magnitude type:", ctarget.GravityMagType);
                if (ctarget.GravityMagType == MAGTYPE.Fixed)
                    ctarget.GravityMag = EditorGUILayout.FloatField("magnitude:", ctarget.GravityMag);
                else
                    ctarget.GravityCurve = EditorGUILayout.CurveField("magnitude curve:", ctarget.GravityCurve);
            }
            ctarget.IsGravityAccelerate = EditorGUILayout.Toggle("is accelerate:", ctarget.IsGravityAccelerate);
            EditorGUILayout.EndVertical();
        }
    }

    protected void UVConfig(EffectLayer ctarget)
    {
        DisplayUVConfig = EditorGUILayout.Foldout(DisplayUVConfig, "UV Configuration");

        if (DisplayUVConfig)
        {
            EditorGUILayout.BeginVertical();
            
            if (ctarget.RenderType != 2)
                ctarget.OriTopLeftUV = EditorGUILayout.Vector2Field("original top left uv:", ctarget.OriTopLeftUV);
            else
                ctarget.OriTopLeftUV = EditorGUILayout.Vector2Field("original bottom left uv:", ctarget.OriTopLeftUV);
            ctarget.OriUVDimensions = EditorGUILayout.Vector2Field("original uv dimensions:", ctarget.OriUVDimensions);
            ctarget.UVType = EditorGUILayout.Popup("UV Change Type:",ctarget.UVType, UVTypes);
            ctarget.UVAffectorEnable = false;
            ctarget.UVRotAffectorEnable = false;
            if (ctarget.UVType == 1)
            {
                ctarget.Cols = EditorGUILayout.IntField("x tile:", ctarget.Cols);
                ctarget.Rows = EditorGUILayout.IntField("y tile:", ctarget.Rows);
                ctarget.UVTime = EditorGUILayout.FloatField("time(-1 means node life):", ctarget.UVTime);
                ctarget.LoopCircles = EditorGUILayout.IntField("loop(-1 means infinite):", ctarget.LoopCircles);
                ctarget.UVAffectorEnable = true;
            }
            else if (ctarget.UVType == 2)
            {
                EditorGUILayout.BeginVertical();
                
                ctarget.RandomUVRotateSpeed = EditorGUILayout.Toggle("random uv scroll speed?" , ctarget.RandomUVRotateSpeed);
                
                if (ctarget.RandomUVRotateSpeed)
                {
                    ctarget.UVRotXSpeed = EditorGUILayout.FloatField("x scroll speed min:", ctarget.UVRotXSpeed);
                    ctarget.UVRotXSpeedMax = EditorGUILayout.FloatField("x scroll speed max:", ctarget.UVRotXSpeedMax);
                    
                    ctarget.UVRotYSpeed = EditorGUILayout.FloatField("y scroll speed min:", ctarget.UVRotYSpeed);
                    ctarget.UVRotYSpeedMax = EditorGUILayout.FloatField("y scroll speed max:", ctarget.UVRotYSpeedMax);
                }
                else
                {
                    ctarget.UVRotXSpeed = EditorGUILayout.FloatField("x scroll speed", ctarget.UVRotXSpeed);
                    ctarget.UVRotYSpeed = EditorGUILayout.FloatField("y scroll speed", ctarget.UVRotYSpeed);
                }
                EditorGUILayout.EndVertical();
                ctarget.UVRotAffectorEnable = true;
            }
            if (ctarget.UVType != 0)
            {
                EditorGUILayout.Separator();
                ctarget.RandomStartFrame = EditorGUILayout.Toggle("random start?", ctarget.RandomStartFrame);
            }
            EditorGUILayout.EndVertical();
        }
    }
}
