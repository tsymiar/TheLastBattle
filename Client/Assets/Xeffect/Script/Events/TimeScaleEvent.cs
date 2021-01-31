using UnityEngine;
using System.Collections;

namespace Xft
{
    public class TimeScaleEvent : XftEvent
    {
        protected bool m_eventFired = false;
        
        public TimeScaleEvent (XftEventComponent owner)
         : base(XftEventType.TimeScale, owner)
        {
        }

        public override void Reset ()
        {
            m_elapsedTime = 0f;
            m_editorFlag = false;
            m_eventFired = false;
            Time.timeScale = 1f;
        }
     
        public override void EditorDone ()
        {
            m_editorFlag = true;
        }
     
        public override void Update (float deltaTime)
        {
            if (m_editorFlag)
                return;

            
            m_elapsedTime += deltaTime;
            if (m_elapsedTime > m_owner.StartTime && !m_eventFired) {
                Time.timeScale = m_owner.TimeScale;
                m_eventFired = true;
            }
            
            if (m_elapsedTime > m_owner.EndTime)
            {
                Time.timeScale = 1f;
                return;
            }
            
            if (m_eventFired)
            {
                float elapsed = m_elapsedTime - m_owner.StartTime;
                
                if (elapsed / m_owner.TimeScale > m_owner.TimeScaleDuration)
                {
                    Time.timeScale = 1f;
                    m_editorFlag = true;
                }
            }
        }
    }
}

