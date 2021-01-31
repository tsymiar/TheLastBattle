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
    public class AirFieldAffector : Affector
    {
        protected Transform AirObj;
        protected Vector3 Direction;
        protected MAGTYPE MType;
        protected float Magnitude;
        protected AnimationCurve MagCurve;
        protected float Attenuation;
        protected bool UseMaxDistance;
        protected float MaxDistance;
        protected float MaxDistanceSqr;
        protected bool EnableSpread;
        protected float Spread;
        protected float InheritVelocity;
        protected bool InheritRotation;

        protected Vector3 LastFieldPos;

        public AirFieldAffector(Transform airObj, Vector3 dir, MAGTYPE mtype,float mag, AnimationCurve curve,float atten, bool useMaxdist, 
            float maxDist,bool enableSpread, float spread, float inhV, bool inhRot, EffectNode node)
            : base(node, AFFECTORTYPE.AirFieldAffector)
        {
            AirObj = airObj;
            Direction = dir.normalized;
            MType = mtype;
            Magnitude = mag;
            MagCurve = curve;
            Attenuation = atten;
            UseMaxDistance = useMaxdist;
            MaxDistance = maxDist;
            MaxDistanceSqr = MaxDistance * MaxDistance;
            EnableSpread = enableSpread;
            Spread = spread;
            InheritVelocity = inhV;
            InheritRotation = inhRot;
            LastFieldPos = AirObj.position;
        }

        public override void Reset()
        {
            LastFieldPos = AirObj.position;
        }

        public override void Update(float deltaTime)
        {
            Vector3 airDir;
            if (InheritRotation)
            {
                airDir = AirObj.rotation * Direction;
            }
            else
            {
                airDir = Direction;
            }

            Vector3 airFiledVelocity = Vector3.zero;
            airFiledVelocity = (AirObj.position - LastFieldPos) * InheritVelocity / deltaTime;
            LastFieldPos = AirObj.position;

            float mag = 0f;
            if (MType == MAGTYPE.Fixed)
                mag = Magnitude;
            else
                mag = MagCurve.Evaluate(Node.GetElapsedTime());

            airFiledVelocity += airDir * mag;

            float airFieldVelocityLen = airFiledVelocity.magnitude;

            float cosSpread = EnableSpread ? Mathf.Cos(Mathf.PI / 2 * Spread) : 0f;

            Vector3 offset;
            offset = Node.GetOriginalPos() - AirObj.position;

            float distSqr = offset.sqrMagnitude;
            if (!UseMaxDistance || distSqr < MaxDistanceSqr)
            {
                Vector3 airVelocity = airFiledVelocity;
                if (EnableSpread)
                {
                    airVelocity = offset.normalized;
                    // if outside the cone they don't affect this node
                    if (Vector3.Dot(airFiledVelocity, airVelocity) < cosSpread)
                    {
                        return;
                    }
                    airVelocity *= airFieldVelocityLen;
                }
                Vector3 nodeVelocity = Node.Velocity;
                // Don't go faster than the air
                if (Vector3.Dot(airVelocity, nodeVelocity - airVelocity) < 0f)
                {
                    float AttenuationWithTime = deltaTime;
                    // Apply Attenuation
                    if (UseMaxDistance && (Attenuation < 1e-06f))
                    {
                        AttenuationWithTime *= Mathf.Pow(1.0f -(Mathf.Sqrt(distSqr) / MaxDistance),
                            Attenuation);
                    }

                    // Apply Air to the particle velocity
                    nodeVelocity+= airVelocity * AttenuationWithTime;
                    // Apply the computed velocity
                    Node.Velocity = nodeVelocity;
                }
            }
        }
    }
}

