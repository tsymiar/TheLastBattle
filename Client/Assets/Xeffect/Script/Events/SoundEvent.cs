using UnityEngine;
using System.Collections;


namespace Xft
{
	public class SoundEvent : XftEvent 
	{
		protected bool m_eventFired = false;
		static AudioListener m_Listener;
		public SoundEvent(XftEventComponent owner)
			: base(XftEventType.Sound, owner)
		{
			
		}
		
		protected AudioSource PlaySound(AudioClip clip, float volume, float pitch)
		{
			if (clip != null)
        	{
            	if (m_Listener == null)
            	{
                	m_Listener = GameObject.FindObjectOfType(typeof(AudioListener)) as AudioListener;

                	if (m_Listener == null)
                	{
                    	Camera cam = Camera.main;
                    	if (cam == null) cam = GameObject.FindObjectOfType(typeof(Camera)) as Camera;
                    	if (cam != null) m_Listener = cam.gameObject.AddComponent<AudioListener>();
                	}
            	}

            	if (m_Listener != null)
            	{
                	AudioSource source = m_Listener.audio;
                	if (source == null) source = m_Listener.gameObject.AddComponent<AudioSource>();
                	source.pitch = pitch;
                	source.PlayOneShot(clip, volume);
                	return source;
            	}
        	}
        	return null;
		}
		
		public override void Reset ()
		{
			m_elapsedTime = 0f;
			m_eventFired = false;
			m_editorFlag = false;
		}
		
		public override void EditorDone ()
		{
			m_editorFlag = true;
		}
		
		public override void Update (float deltaTime)
		{
			if (m_eventFired)
				return;
			
			if (m_editorFlag)
				return;
			
			m_elapsedTime += deltaTime;
			if (m_elapsedTime > m_owner.StartTime)
			{
				m_eventFired = true;
				PlaySound(m_owner.Clip,m_owner.Volume,m_owner.Pitch);
			}
		}
	}
}

