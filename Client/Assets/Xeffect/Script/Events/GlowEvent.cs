using UnityEngine;
using System.Collections;

namespace Xft
{
    public class GlowEvent : XftEvent
    {
        protected Camera m_camera;
        protected XftGlow m_glowComp;
        protected bool m_supported = true;
        
        public GlowEvent (XftEventComponent owner)
            : base(XftEventType.CameraGlow, owner)
        {

        }

        protected void FindCamera ()
        {
            int layerMask = 1 << m_owner.gameObject.layer;
            Camera[] cameras = GameObject.FindObjectsOfType (typeof(Camera)) as Camera[];
            for (int i = 0, imax = cameras.Length; i < imax; ++i) {
                Camera cam = cameras [i];

                if ((cam.cullingMask & layerMask) != 0) {
                    m_camera = cam;
                    return;
                }
            }
            Debug.LogError ("can't find proper camera for GlowEvent!");
        }

        protected void ToggleCameraComponent (bool flag)
        {
            m_glowComp = m_camera.gameObject.GetComponent<XftGlow> ();
            if (m_glowComp == null) {
                m_glowComp = m_camera.gameObject.AddComponent<XftGlow> ();
            }
            //assign the shader here.
            m_glowComp.Init (m_owner);
            m_glowComp.enabled = flag;
        }

        public override void Initialize ()
        {
            FindCamera ();
            ToggleCameraComponent (false);
            m_elapsedTime = 0f;
            m_supported = m_glowComp.CheckSupport ();
            if (!m_supported)
                Debug.LogWarning ("can't support Image Effect: Glow on this device!");
        }

        public override void Reset ()
        {
            m_glowComp.enabled = false;
            m_elapsedTime = 0f;
            m_editorFlag = false;
        }
     
        public override void EditorDone ()
        {
            m_editorFlag = true;
        }

        public override void Update (float deltaTime)
        {
         
            if (!m_supported) {
                m_owner.enabled = false;
                return;
            }
         
            if (m_editorFlag)
                return;
         
            m_elapsedTime += deltaTime;

            if (m_elapsedTime > m_owner.EndTime) {
                m_glowComp.enabled = false;
                return;
            }

            if (m_elapsedTime > m_owner.StartTime) {
                float t = 0f;
                Color tint = Color.clear;
                if (m_owner.GlowColorGradualType == COLOR_GRADUAL_TYPE.CURVE) {
                    t = m_owner.ColorCurve.Evaluate (m_elapsedTime - m_owner.StartTime);
                    tint = Color.Lerp (m_owner.GlowColorStart, m_owner.GlowColorEnd, t);
                } else {
                    t = (m_elapsedTime - m_owner.StartTime) / m_owner.GlowColorGradualTime;
                    if (t <= 1) {
                        tint = Color.Lerp (m_owner.GlowColorStart, m_owner.GlowColorEnd, t);
                    } else {
                        if (m_owner.GlowColorGradualType == COLOR_GRADUAL_TYPE.CLAMP) {
                            // do nothing
                        } else if (m_owner.GlowColorGradualType == COLOR_GRADUAL_TYPE.LOOP) {
                            tint = Color.Lerp (m_owner.GlowColorStart, m_owner.GlowColorEnd, t - 1);
                        } else {
                            tint = Color.Lerp (m_owner.GlowColorEnd, m_owner.GlowColorStart, t - 1);
                        }
                    }
                }
                m_glowComp.glowTint = tint;
                m_glowComp.enabled = true;
            }
        }
    }
}

