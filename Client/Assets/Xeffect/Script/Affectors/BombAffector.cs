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
    public enum BOMBTYPE
    {
        Planar,
        Spherical,
        Cylindrical
    }

    public enum BOMBDECAYTYPE
    {
        None,
        Linear,
        Exponential
    }

    public class BombAffector : Affector
    {
        protected BOMBTYPE BombType;
        protected MAGTYPE MType;
        protected BOMBDECAYTYPE DecayType;
        protected float Magnitude;
        protected AnimationCurve MagCurve;
        protected float Decay;
        protected Vector3 BombAxis;
        protected Transform BombObj;

        protected float ElapsedTime = 0f;

        public BombAffector(Transform obj, BOMBTYPE gtype, MAGTYPE mtype,BOMBDECAYTYPE dtype, float mag, AnimationCurve curve,
            float decay, Vector3 axis, EffectNode node)
            : base(node, AFFECTORTYPE.BombAffector)
        {
            BombType = gtype;
            MType = mtype;
            DecayType = dtype;
            Magnitude = mag;
            MagCurve = curve;
            Decay = decay;
            BombAxis = axis;
            BombAxis.Normalize();
            BombObj = obj;
        }


        public override void Reset()
        {
            ElapsedTime = 0f;
        }

        public override void Update(float deltaTime)
        {
            float strength = 0f;
            if (MType == MAGTYPE.Fixed)
                strength = Magnitude;
            else
                strength = MagCurve.Evaluate(Node.GetElapsedTime());
            Vector3 bombDir = BombObj.rotation * BombAxis;
            Vector3 dir;
            dir = Node.GetOriginalPos() - BombObj.position;
            float dist = dir.magnitude;
            Vector3 force = Vector3.zero;
            if (DecayType == BOMBDECAYTYPE.None || dist <= Decay)
            {
                switch (BombType)
                {
                    case BOMBTYPE.Spherical:
                        force = dir / dist;
                        break;
                    case BOMBTYPE.Planar:
                        dist = Vector3.Dot(bombDir, dir);
                        if (dist < 0.0f)
                        {
                            dist = -dist;
                            force = -bombDir;
                        }
                        else
                        {
                            force = bombDir;
                        }
                        break;
                    case BOMBTYPE.Cylindrical:
                        // Subtract off the vector component along the
                        // cylinder axis
                        dist = Vector3.Dot(bombDir, dir);

                        force = dir - dist * bombDir;
                        dist = force.magnitude;
                        if (dist != 0.0f)
                        {
                            force /= dist;
                        }
                        break;
                    default:
                        Debug.LogError("wrong bomb type!");
                        break;
                }

                float decay = 1.0f;
                if (DecayType == BOMBDECAYTYPE.Linear)
                {
                    decay = (Decay - dist) / Decay;
                }
                else if (DecayType == BOMBDECAYTYPE.Exponential)
                {
                    decay = Mathf.Exp(-dist / Decay);
                }

                // we modify the velocity by multiplying the following;
                // 1) decay - a multiplier between 0 and 1 based off of
                //  how far the particle is from the bomb
                // 2) force - a vector representing the direction of force
                //  this is a unit vector.
                // 3) strength - the strength value exported from the modeling
                //  package.  This value is in units of mass / time^2, temporily the mass is 1.
                // 4) deltaTime - the change in time from the last update

                ElapsedTime += deltaTime;

                strength = strength / (ElapsedTime * ElapsedTime);

                Node.Velocity += decay * strength * deltaTime * force;
            }
        }
    }
}
