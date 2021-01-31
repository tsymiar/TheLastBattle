using UnityEngine;
using System.Collections;

namespace Xft
{
    public class ColorInverseEvent : XftEvent
    {
        protected Camera m_camera;
        protected XftColorInverse m_inverseComp;
        protected bool m_supported = true;
        
        public ColorInverseEvent (XftEventComponent owner)
            : base(XftEventType.CameraColorInverse, owner)
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
            Debug.LogError ("can't find proper camera for ColorInverseEvent!");
        }

        protected void ToggleCameraComponent (bool flag)
        {
            m_inverseComp = m_camera.gameObject.GetComponent<XftColorInverse> ();
            if (m_inverseComp == null) {
                m_inverseComp = m_camera.gameObject.AddComponent<XftColorInverse> ();
            }
            //assign the shader here.
            m_inverseComp.Init (m_owner.ColorInverseShader);
            m_inverseComp.enabled = flag;
        }

        public override void Initialize ()
        {
            FindCamera ();
            ToggleCameraComponent (false);
            m_elapsedTime = 0f;
            m_supported = m_inverseComp.CheckSupport ();
            if (!m_supported)
                Debug.LogWarning ("can't support Image Effect: ColorInverse on this device!");
        }

        public override void Reset ()
        {
            m_inverseComp.enabled = false;
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
                m_inverseComp.enabled = false;
                return;
            }

            if (m_elapsedTime > m_owner.StartTime) {
                float strength = m_owner.CIStrengthCurve.Evaluate(m_elapsedTime);
                m_inverseComp.Strength = strength;
                m_inverseComp.enabled = true;
            }
        }
    }
}

