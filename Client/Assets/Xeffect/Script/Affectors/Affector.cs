//----------------------------------------------
//            Xffect Editor
// Copyright © 2012- Shallway Studio
// http://shallway.net
//----------------------------------------------

using UnityEngine;
using System.Collections;
using Xft;

namespace Xft
{

    public enum AFFECTORTYPE
    {
        RotateAffector,
        UVAffector,
        UVRotAffector,
        ScaleAffector,
        ColorAffector,
        GravityAffector,
        BombAffector,
        AirFieldAffector,
        JetAffector,
        VortexAffector,
        TurbulenceAffector,
        DragAffector
    }

    public class Affector
    {
        public EffectNode Node;
        public AFFECTORTYPE Type;
        public Affector(EffectNode node, AFFECTORTYPE type)
        {
            Node = node;
            Type = type;
        }


        public virtual void Update(float deltaTime)
        {

        }

        public virtual void Reset()
        {
        }
    }
    public class RotateAffector : Affector
    {
        protected AnimationCurve RotateCurve;
        protected RSTYPE RType;
        protected float Delta = 0f;

        public RotateAffector(AnimationCurve curve, EffectNode node)
            : base(node,AFFECTORTYPE.RotateAffector)
        {
            RType = RSTYPE.CURVE;
            RotateCurve = curve;
        }

        public RotateAffector(float delta, EffectNode node)
            : base(node, AFFECTORTYPE.RotateAffector)
        {
            RType = RSTYPE.SIMPLE;
            Delta = delta;
        }


        public override void Update(float deltaTime)
        {
            float time = Node.GetElapsedTime();
            if (RType == RSTYPE.CURVE)
                Node.RotateAngle = (int)RotateCurve.Evaluate(time);
            else if (RType == RSTYPE.SIMPLE)
            {
                float angle = Node.RotateAngle + Delta * deltaTime;
                Node.RotateAngle = angle;
            }
        }
    }


    //注意，如果贴图不是整张，那么uv scroll还是在整张范围内scroll的
    public class UVRotAffector : Affector
    {
        protected float RotXSpeed;
        protected float RotYSpeed;
        protected bool FirstUpdate = true;
        public UVRotAffector(float rotXSpeed, float rotYSpeed, EffectNode node)
            : base(node, AFFECTORTYPE.UVRotAffector)
        {
            RotXSpeed = rotXSpeed;
            RotYSpeed = rotYSpeed;
        }

        public override void Reset()
        {
            FirstUpdate = true;
        }

        public override void Update(float deltaTime)
        {
            if (FirstUpdate)
            {
                if (Node.Owner.RandomStartFrame)
                {
                    Node.LowerLeftUV.x += Random.Range(-1f, 1f);
                    Node.LowerLeftUV.y += Random.Range(-1f, 1f);
                }
                FirstUpdate = false;
            }
            Vector2 nodeuv = Node.LowerLeftUV;
            nodeuv.x = nodeuv.x + RotXSpeed * deltaTime;
            nodeuv.y = nodeuv.y + RotYSpeed * deltaTime;
            Node.LowerLeftUV = nodeuv;
        }
    }

    public class UVAffector : Affector
    {
        protected UVAnimation Frames;
        protected float ElapsedTime;
        protected float UVTime;
        protected bool RandomStartFrame = false;
        protected bool FirstUpdate = true;

        public UVAffector(UVAnimation frame, float time, EffectNode node, bool randomStart)
            : base(node, AFFECTORTYPE.UVAffector)
        {
            Frames = frame;
            UVTime = time;
            RandomStartFrame = randomStart;
            if (RandomStartFrame)
            {
                Frames.curFrame = Random.Range(0, Frames.frames.Length - 1);
            }
        }

        public override void Reset()
        {
            ElapsedTime = 0;
            FirstUpdate = true;
            Frames.curFrame = 0;
            Frames.numLoops = 0;
            if (RandomStartFrame)
            {
                Frames.curFrame = Random.Range(0, Frames.frames.Length - 1);
            }
        }
        public override void Update(float deltaTime)
        {
            ElapsedTime += deltaTime;
            float framerate;
            if (UVTime <= 0f)
            {
                framerate = Node.GetLifeTime() / Frames.frames.Length;
            }
            else
            {
                framerate = UVTime / Frames.frames.Length;
            }
            if (ElapsedTime >= framerate || FirstUpdate)
            {
                Vector2 uv = Vector2.zero;
                Vector2 dm = Vector2.zero;
                Frames.GetNextFrame(ref uv, ref dm);
                Node.LowerLeftUV = uv;
                Node.UVDimensions = dm;
                ElapsedTime -= framerate;
            }

            FirstUpdate = false;
        }
    }

    public class ScaleAffector : Affector
    {
        protected AnimationCurve ScaleXCurve;
        protected AnimationCurve ScaleYCurve;

        protected RSTYPE SType;
        protected float DeltaX = 0f;
        protected float DeltaY = 0f;

        public ScaleAffector(AnimationCurve curveX, AnimationCurve curveY, EffectNode node)
            : base(node, AFFECTORTYPE.ScaleAffector)
        {
            SType = RSTYPE.CURVE;
            ScaleXCurve = curveX;
            ScaleYCurve = curveY;
        }

        public ScaleAffector(float x, float y, EffectNode node)
            : base(node, AFFECTORTYPE.ScaleAffector)
        {
            SType = RSTYPE.SIMPLE;
            DeltaX = x;
            DeltaY = y;
        }

        public override void Update(float deltaTime)
        {
            float time = Node.GetElapsedTime();
            if (SType == RSTYPE.CURVE)
            {
                if (ScaleXCurve != null)
                    Node.Scale.x = ScaleXCurve.Evaluate(time);
                if (ScaleYCurve != null)
                    Node.Scale.y = ScaleYCurve.Evaluate(time);
            }
            else if (SType == RSTYPE.SIMPLE)
            {
                float scalex = Node.Scale.x + DeltaX * deltaTime;
                float scaley = Node.Scale.y + DeltaY * deltaTime;
                if (scalex * Node.Scale.x > 0)
                    Node.Scale.x = scalex;
                if (scaley * Node.Scale.y > 0)
                    Node.Scale.y = scaley;
            }
        }
    }

    public class ColorAffector : Affector
    {
        protected Color[] ColorArr;
        protected Color[] OriColorArr;
        protected float GradualLen;
        protected COLOR_GRADUAL_TYPE CType;
        protected float ElapsedTime = 0f;
        protected bool IsNodeLife = false;
        public ColorAffector(Color[] colorArr, float gradualLen, COLOR_GRADUAL_TYPE type, EffectNode node)
            : base(node, AFFECTORTYPE.ColorAffector)
        {
            ColorArr = colorArr;
            OriColorArr = new Color[ColorArr.Length];
            ColorArr.CopyTo(OriColorArr, 0);
            CType = type;
            GradualLen = gradualLen;
            if (GradualLen < 0)
                IsNodeLife = true;
        }

        public override void Reset()
        {
            ElapsedTime = 0;
            //ColorArr = OriColorArr;
            //bug fixed，必须copy过去，否则还是会被改变。
            OriColorArr.CopyTo(ColorArr, 0);
        }

        public override void Update(float deltaTime)
        {
            ElapsedTime += deltaTime;
            if (IsNodeLife)
            {
                GradualLen = Node.GetLifeTime();
            }

            if (GradualLen <= 0f)//node life loop
                return;

            if (ElapsedTime > GradualLen)
            {
                if (CType == COLOR_GRADUAL_TYPE.CLAMP)
                    return;
                else if (CType == COLOR_GRADUAL_TYPE.LOOP)
                {
                    ElapsedTime -= GradualLen;
                    return;
                }
                else
                {
                    Color[] TempArr = new Color[ColorArr.Length];
                    ColorArr.CopyTo(TempArr, 0);
                    for (int i = 0; i < TempArr.Length / 2; i++)
                    {
                        ColorArr[TempArr.Length - i - 1] = TempArr[i];
                        ColorArr[i] = TempArr[TempArr.Length - i - 1];
                    }
                    ElapsedTime -= GradualLen;
                    return;
                }
            }
            int curIndex = (int)((ColorArr.Length - 1) * (ElapsedTime / GradualLen));
            if (curIndex == ColorArr.Length - 1)
                curIndex--;
            int targetIndex = curIndex + 1;
            float segmentTime = GradualLen / (ColorArr.Length - 1);
            float t = (ElapsedTime - segmentTime * curIndex) / segmentTime;
            Node.Color = Color.Lerp(ColorArr[curIndex], ColorArr[targetIndex], t);
        }
    }

    public class JetAffector : Affector
    {
        protected float Mag;
        protected MAGTYPE MType;
        protected AnimationCurve MagCurve;
        
        public JetAffector(float mag, MAGTYPE type, AnimationCurve curve,EffectNode node)
            : base(node, AFFECTORTYPE.JetAffector)
        {
            Mag = mag;
            MType = type;
            MagCurve = curve;
        }

        public override void Update(float deltaTime)
        {
            Vector3 delta = Vector3.zero;
            if (MType == MAGTYPE.Fixed)
                delta = Node.Velocity.normalized * Mag * deltaTime;
            else
                delta = Node.Velocity.normalized * MagCurve.Evaluate(Node.GetElapsedTime());
            
            Vector3 goalV = Node.Velocity + delta;
            if (Vector3.Dot(goalV,Node.Velocity) <= 0)
            {
                //don't change the dir
                Node.Velocity = Vector3.zero;
                return;
            }
            
            Node.Velocity = goalV;
        }
    }
}