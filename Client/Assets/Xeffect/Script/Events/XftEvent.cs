using UnityEngine;
using System.Collections;

namespace Xft
{
    public class XftEvent
    {
        protected XftEventType m_type;
        protected XftEventComponent m_owner;
        protected float m_elapsedTime = 0f;
        protected bool m_editorFlag = false;
     
        public XftEvent (XftEventType type, XftEventComponent owner)
        {
            m_type = type;
            m_owner = owner;
        }

        public virtual void Initialize ()
        {
        }

        public virtual void Update (float deltaTime)
        {
        }

        public virtual void Reset ()
        {
        }
     
        //In Editor mode, we need some trick to detect if Xffect is done, then disable the events.
        public virtual void EditorDone ()
        {
        }
    }
}
