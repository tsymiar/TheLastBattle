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
    public class TurbulenceFieldAffector : Affector
    {
        protected MAGTYPE MType;
        protected float Magnitude;
        protected AnimationCurve MagCurve;
        protected Transform TurbulenceObj;
        protected float Attenuation;
        protected bool UseMaxDistance;
        protected float MaxDistance;
        protected float MaxDistanceSqr;

        public TurbulenceFieldAffector(Transform obj, MAGTYPE mtype, float mag, AnimationCurve curve, float atten, bool useMax, float maxDist, EffectNode node)
            : base(node, AFFECTORTYPE.TurbulenceAffector)
        {
            TurbulenceObj = obj;
            MType = mtype;
            Magnitude = mag;
            MagCurve = curve;
            UseMaxDistance = useMax;
            MaxDistance = maxDist;
            MaxDistanceSqr = MaxDistance * MaxDistance;
        }

        protected void UpateNoAttenuation(float deltaTime)
        {
            float dist = (Node.GetOriginalPos() - TurbulenceObj.position).sqrMagnitude;
            Vector3 deltaV;

            float mag = 0f;
            if (MType == MAGTYPE.Fixed)
                mag = Magnitude;
            else
                mag = MagCurve.Evaluate(Node.GetElapsedTime());

            if (!UseMaxDistance || (dist <= MaxDistanceSqr))
            {
                deltaV.x = Random.Range(-1f, 1f);
                deltaV.y = Random.Range(-1f, 1f);
                deltaV.z = Random.Range(-1f, 1f);
                deltaV *= mag;
                Node.Velocity += deltaV;
            }
        }

        public override void Update(float deltaTime)
        {
            if (Attenuation < 1e-06)
            {
                UpateNoAttenuation(deltaTime);
                return;
            }

            float dist = (Node.GetOriginalPos() - TurbulenceObj.position).magnitude;
            Vector3 deltaV;
            float mag = 0f;
            if (MType == MAGTYPE.Fixed)
                mag = Magnitude;
            else
                mag = MagCurve.Evaluate(Node.GetElapsedTime());

            if (!UseMaxDistance || (dist <= MaxDistance))
            {
                deltaV.x = Random.Range(-1f, 1f);
                deltaV.y = Random.Range(-1f, 1f);
                deltaV.z = Random.Range(-1f, 1f);

                deltaV *= mag / (1.0f + dist * Attenuation);

                Node.Velocity += deltaV;
            }
        }
    }
}

