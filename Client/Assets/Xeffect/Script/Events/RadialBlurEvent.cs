using UnityEngine;
using System.Collections;


namespace Xft
{
	 public class RadialBlurEvent : XftEvent
    {
        protected Camera m_camera;
        protected XftRadialBlur m_radialBlurComp;
		protected bool m_supported = true;

        public RadialBlurEvent(XftEventComponent owner)
            : base(XftEventType.CameraRadialBlur, owner)
        {

        }

        protected void FindCamera()
        {
            int layerMask = 1 << m_owner.gameObject.layer;
            Camera[] cameras = GameObject.FindObjectsOfType(typeof(Camera)) as Camera[];
            for (int i = 0, imax = cameras.Length; i < imax; ++i)
            {
                Camera cam = cameras[i];

                if ((cam.cullingMask & layerMask) != 0)
                {
                    m_camera = cam;
                    return;
                }
            }
            Debug.LogError("can't find proper camera for RadialBlurEvent!");
        }

        protected void ToggleCameraComponent(bool flag)
        {
            m_radialBlurComp = m_camera.gameObject.GetComponent<XftRadialBlur>();
            if (m_radialBlurComp == null)
            {
                m_radialBlurComp = m_camera.gameObject.AddComponent<XftRadialBlur>();
            }
			//assign the shader here.
			m_radialBlurComp.Init(m_owner.RadialBlurShader);
            m_radialBlurComp.enabled = flag;
        }

        public override void Initialize()
        {
            FindCamera();
            ToggleCameraComponent(false);
            m_elapsedTime = 0f;
			m_supported = m_radialBlurComp.CheckSupport();
			if (!m_supported)
				Debug.LogWarning("can't support Image Effect: Radial Blur on this device!");
        }

        public override void Reset()
        {
            m_radialBlurComp.enabled = false;
            m_elapsedTime = 0f;
			m_editorFlag = false;
        }
		
		public override void EditorDone ()
		{
			m_editorFlag = true;
		}

        public override void Update(float deltaTime)
        {
			if (!m_supported)
			{
				m_owner.enabled = false;
				return;
			}
			
			if (m_editorFlag)
				return;
			
            m_elapsedTime += deltaTime;

            if (m_elapsedTime > m_owner.EndTime)
            {
                m_radialBlurComp.enabled = false;
                return;
            }
   
            
            
            if (m_elapsedTime > m_owner.StartTime)
            {
                m_radialBlurComp.enabled = true;
                Vector3 pos = m_camera.WorldToScreenPoint(m_owner.RadialBlurObj.position);
                m_radialBlurComp.Center = pos;
				
				float strength = 0f;
				if (m_owner.RBStrengthType == MAGTYPE.Fixed)
					strength = m_owner.RBSampleStrength;
				else
					strength = m_owner.RBSampleStrengthCurve.Evaluate(m_elapsedTime - m_owner.StartTime);
				
				Mathf.Clamp(strength,0.05f,99f);
				m_radialBlurComp.SampleStrength = strength;
                m_radialBlurComp.SampleDist = m_owner.RBSampleDist;
            }
        }
    }
}

