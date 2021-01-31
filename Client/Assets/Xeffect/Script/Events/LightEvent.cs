using UnityEngine;
using System.Collections;

namespace Xft
{
	public class LightEvent : XftEvent
	{
		public LightEvent (XftEventComponent owner)
            : base(XftEventType.Light, owner)
		{

		}
		public override void Initialize()
        {
			if (m_owner.LightComp == null)
			{
				Debug.LogWarning("you should assign a light source to Light Event to use it!");
				return;
			}
            m_elapsedTime = 0f;
			XffectComponent.SetActive(m_owner.LightComp.gameObject,false);
        }

        public override void Reset()
        {
			if (m_owner.LightComp == null)
				return;
            XffectComponent.SetActive(m_owner.LightComp.gameObject,false);
            m_elapsedTime = 0f;
			m_editorFlag = false;
        }
		
		
		public override void EditorDone ()
		{
			m_editorFlag = true;
		}
		
		public override void Update (float deltaTime)
		{
			
			if (m_editorFlag)
				return;
			if (m_owner.LightComp == null)
				return;
			
			m_elapsedTime += deltaTime;
			
			if (m_elapsedTime > m_owner.EndTime)
			{
				XffectComponent.SetActive(m_owner.LightComp.gameObject,false);
				return;
			}
			
			if (m_elapsedTime > m_owner.StartTime)
			{
				XffectComponent.SetActive(m_owner.LightComp.gameObject,true);
				float intensity = 0f;
				if (m_owner.LightIntensityType == MAGTYPE.Curve && m_owner.LightIntensityCurve != null)
				{
					intensity = m_owner.LightIntensityCurve.Evaluate(m_elapsedTime - m_owner.StartTime);
				}
				else
				{
					intensity = m_owner.LightIntensity;
				}
				m_owner.LightComp.intensity = intensity;
				
				float range = 0f;
				if (m_owner.LightRangeType == MAGTYPE.Curve && m_owner.LightRangeCurve != null)
				{
					range = m_owner.LightRangeCurve.Evaluate(m_elapsedTime - m_owner.StartTime);
				}
				else
				{
					range = m_owner.LightRange;
				}
				m_owner.LightComp.range = range;
			}
		}
		
	}

}
