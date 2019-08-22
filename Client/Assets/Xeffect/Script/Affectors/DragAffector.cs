using UnityEngine;
using System.Collections;
using Xft;

namespace Xft
{
    public class DragAffector : Affector {
        protected Transform DragObj;
        protected bool UseDirection;
        protected Vector3 Direction;
        protected float Magnitude;
        protected bool UseMaxDistance;
        protected float MaxDistance;
        protected float Attenuation;
        
        
        public DragAffector(Transform dragObj, bool useDir, Vector3 dir,float mag, bool useMaxDist, float maxDist, float atten, EffectNode node)
            : base(node, AFFECTORTYPE.DragAffector)
        {
            DragObj = dragObj;
            UseDirection = useDir;
            Direction = dir;
            Magnitude = mag;
            UseMaxDistance = useMaxDist;
            MaxDistance = maxDist;
            Attenuation = atten;
        }
        
        
        protected void UpdateNoAttenuationNoDir(float deltaTime)
        {
            float distanceSqr = (Node.GetOriginalPos() - DragObj.position).sqrMagnitude;
            float reduction = Magnitude * deltaTime;
            if (reduction != 0f)
            {
                if (distanceSqr <= MaxDistance * MaxDistance)
                {
                    if (reduction < 1f)
                    {
                        Node.Velocity *= 1f - reduction;
                    }
                    else
                    {
                        Node.Velocity = Vector3.zero;
                    }
                }
            }
        }
        
        protected void UpdateNoAttenuationNoDirNoDist(float deltaTime)
        {
            float reduction = Magnitude * deltaTime;
            if (reduction < 1f)
            {
                Node.Velocity *= 1f - reduction;
            }
            else
            {
                Node.Velocity = Vector3.zero;
            }
        }
        
        public override void Update(float deltaTime)
        {
            
            //check for optimized update path.
            if (!UseDirection && (Attenuation == 0f))
            {
                if (UseMaxDistance)
                    UpdateNoAttenuationNoDir(deltaTime);
                else
                    UpdateNoAttenuationNoDirNoDist(deltaTime);
                
                return;
            }
            
            Vector3 dragAxis = Vector3.one;
            if (UseDirection && (Direction != Vector3.zero))
            {
                dragAxis = DragObj.rotation * Direction;
                dragAxis.Normalize();
            }
            
            float distance = (Node.GetOriginalPos() - DragObj.position).magnitude;
            
            if (! UseMaxDistance || distance <= MaxDistance)
            {
                float proj = 1f;
                if (UseDirection)
                {
                    Vector3 nodeDir = Node.Velocity;
                    nodeDir.Normalize();
                    proj = Vector3.Dot(nodeDir,dragAxis);
                }
                
                float reduction = (Magnitude * deltaTime * proj / (1f + distance * Attenuation));
                
                if (reduction < 1f)
                {
                    Node.Velocity -= reduction * Node.Velocity;
                }
                else
                {
                    Node.Velocity = Vector3.zero;
                }
            }
        }
        
    }
}


