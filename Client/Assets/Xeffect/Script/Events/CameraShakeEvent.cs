using UnityEngine;
using System.Collections;
using System;

namespace Xft
{
	#region perlin class
	// this is a modified version of the perlin noise class from
	// the official Unity 'Procedural Examples' at the following URL:
	// http://unity3d.com/support/resources/example-projects/procedural-examples.html
	// the main change is the addition of the method 'GetVector3Centered',
	// which returns a fractal noise that is relative to i.e. Vector3.zero.
	public class XftSmoothRandom
	{
		public static Vector3 GetVector3 (float speed)
		{
			float time = Time.time * 0.01F * speed;
			return new Vector3 (Get ().HybridMultifractal (time, 15.73F, 0.58F), Get ().HybridMultifractal (time, 63.94F, 0.58F), Get ().HybridMultifractal (time, 0.2F, 0.58F));
		}

		public static Vector3 GetVector3Centered (float speed)
		{
			float time1 = (Time.time) * 0.01F * speed;
			float time2 = (Time.time - 1) * 0.01F * speed;
			Vector3 noise1 = new Vector3 (Get ().HybridMultifractal (time1, 15.73F, 0.58F), Get ().HybridMultifractal (time1, 63.94F, 0.58F), Get ().HybridMultifractal (time1, 0.2F, 0.58F));
			Vector3 noise2 = new Vector3 (Get ().HybridMultifractal (time2, 15.73F, 0.58F), Get ().HybridMultifractal (time2, 63.94F, 0.58F), Get ().HybridMultifractal (time2, 0.2F, 0.58F));
			return noise1 - noise2;
		}

		public static float Get (float speed)
		{
			float time = Time.time * 0.01F * speed;
			return Get ().HybridMultifractal (time * 0.01F, 15.7F, 0.65F);
		}

		private static FractalNoise Get ()
		{ 
			if (s_Noise == null)
				s_Noise = new FractalNoise (1.27F, 2.04F, 8.36F);
			return s_Noise;		
		}

		private static FractalNoise s_Noise;
	}

	public class Perlin
	{
		// Original C code derived from 
		// http://astronomy.swin.edu.au/~pbourke/texture/perlin/perlin.c
		// http://astronomy.swin.edu.au/~pbourke/texture/perlin/perlin.h
		const int B = 0x100;
		const int BM = 0xff;
		const int N = 0x1000;
		int[] p = new int[B + B + 2];
		float[,] g3 = new float [B + B + 2, 3];
		float[,] g2 = new float[B + B + 2, 2];
		float[] g1 = new float[B + B + 2];

		float s_curve (float t)
		{
			return t * t * (3.0F - 2.0F * t);
		}
	
		float lerp (float t, float a, float b)
		{ 
			return a + t * (b - a);
		}

		void setup (float value, out int b0, out int b1, out float r0, out float r1)
		{ 
			float t = value + N;
			b0 = ((int)t) & BM;
			b1 = (b0 + 1) & BM;
			r0 = t - (int)t;
			r1 = r0 - 1.0F;
		}
	
		float at2 (float rx, float ry, float x, float y)
		{
			return rx * x + ry * y;
		}

		float at3 (float rx, float ry, float rz, float x, float y, float z)
		{
			return rx * x + ry * y + rz * z;
		}

		public float Noise (float arg)
		{
			int bx0, bx1;
			float rx0, rx1, sx, u, v;
			setup (arg, out bx0, out bx1, out rx0, out rx1);
		
			sx = s_curve (rx0);
			u = rx0 * g1 [p [bx0]];
			v = rx1 * g1 [p [bx1]];
		
			return(lerp (sx, u, v));
		}

		public float Noise (float x, float y)
		{
			int bx0, bx1, by0, by1, b00, b10, b01, b11;
			float rx0, rx1, ry0, ry1, sx, sy, a, b, u, v;
			int i, j;
		
			setup (x, out bx0, out bx1, out rx0, out rx1);
			setup (y, out by0, out by1, out ry0, out ry1);
		
			i = p [bx0];
			j = p [bx1];
		
			b00 = p [i + by0];
			b10 = p [j + by0];
			b01 = p [i + by1];
			b11 = p [j + by1];
		
			sx = s_curve (rx0);
			sy = s_curve (ry0);
		
			u = at2 (rx0, ry0, g2 [b00, 0], g2 [b00, 1]);
			v = at2 (rx1, ry0, g2 [b10, 0], g2 [b10, 1]);
			a = lerp (sx, u, v);
		
			u = at2 (rx0, ry1, g2 [b01, 0], g2 [b01, 1]);
			v = at2 (rx1, ry1, g2 [b11, 0], g2 [b11, 1]);
			b = lerp (sx, u, v);
		
			return lerp (sy, a, b);
		}
	
		public float Noise (float x, float y, float z)
		{
			int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
			float rx0, rx1, ry0, ry1, rz0, rz1, sy, sz, a, b, c, d, t, u, v;
			int i, j;
		
			setup (x, out bx0, out bx1, out rx0, out rx1);
			setup (y, out by0, out by1, out ry0, out ry1);
			setup (z, out bz0, out bz1, out rz0, out rz1);
		
			i = p [bx0];
			j = p [bx1];
		
			b00 = p [i + by0];
			b10 = p [j + by0];
			b01 = p [i + by1];
			b11 = p [j + by1];
		
			t = s_curve (rx0);
			sy = s_curve (ry0);
			sz = s_curve (rz0);
		
			u = at3 (rx0, ry0, rz0, g3 [b00 + bz0, 0], g3 [b00 + bz0, 1], g3 [b00 + bz0, 2]);
			v = at3 (rx1, ry0, rz0, g3 [b10 + bz0, 0], g3 [b10 + bz0, 1], g3 [b10 + bz0, 2]);
			a = lerp (t, u, v);
		
			u = at3 (rx0, ry1, rz0, g3 [b01 + bz0, 0], g3 [b01 + bz0, 1], g3 [b01 + bz0, 2]);
			v = at3 (rx1, ry1, rz0, g3 [b11 + bz0, 0], g3 [b11 + bz0, 1], g3 [b11 + bz0, 2]);
			b = lerp (t, u, v);
		
			c = lerp (sy, a, b);
		
			u = at3 (rx0, ry0, rz1, g3 [b00 + bz1, 0], g3 [b00 + bz1, 2], g3 [b00 + bz1, 2]);
			v = at3 (rx1, ry0, rz1, g3 [b10 + bz1, 0], g3 [b10 + bz1, 1], g3 [b10 + bz1, 2]);
			a = lerp (t, u, v);
		
			u = at3 (rx0, ry1, rz1, g3 [b01 + bz1, 0], g3 [b01 + bz1, 1], g3 [b01 + bz1, 2]);
			v = at3 (rx1, ry1, rz1, g3 [b11 + bz1, 0], g3 [b11 + bz1, 1], g3 [b11 + bz1, 2]);
			b = lerp (t, u, v);
		
			d = lerp (sy, a, b);
		
			return lerp (sz, c, d);
		}
	
		void normalize2 (ref float x, ref float y)
		{
			float s;
	
			s = (float)Math.Sqrt (x * x + y * y);
			x = y / s;
			y = y / s;
		}
	
		void normalize3 (ref float x, ref float y, ref float z)
		{
			float s;
			s = (float)Math.Sqrt (x * x + y * y + z * z);
			x = y / s;
			y = y / s;
			z = z / s;
		}
	
		public Perlin ()
		{
			int i, j, k;
			System.Random rnd = new System.Random ();
	
			for (i = 0; i < B; i++) {
				p [i] = i;
				g1 [i] = (float)(rnd.Next (B + B) - B) / B;
	
				for (j = 0; j < 2; j++)
					g2 [i, j] = (float)(rnd.Next (B + B) - B) / B;
				normalize2 (ref g2 [i, 0], ref g2 [i, 1]);
	
				for (j = 0; j < 3; j++)
					g3 [i, j] = (float)(rnd.Next (B + B) - B) / B;
			 
	
				normalize3 (ref g3 [i, 0], ref g3 [i, 1], ref g3 [i, 2]);
			}
	
			while (--i != 0) {
				k = p [i];
				p [i] = p [j = rnd.Next (B)];
				p [j] = k;
			}
	
			for (i = 0; i < B + 2; i++) {
				p [B + i] = p [i];
				g1 [B + i] = g1 [i];
				for (j = 0; j < 2; j++)
					g2 [B + i, j] = g2 [i, j];
				for (j = 0; j < 3; j++)
					g3 [B + i, j] = g3 [i, j];
			}
		}
	}

	public class FractalNoise
	{
		public FractalNoise (float inH, float inLacunarity, float inOctaves)
		: this (inH, inLacunarity, inOctaves, null)
		{
		
		}

		public FractalNoise (float inH, float inLacunarity, float inOctaves, Perlin noise)
		{
			m_Lacunarity = inLacunarity;
			m_Octaves = inOctaves;
			m_IntOctaves = (int)inOctaves;
			m_Exponent = new float[m_IntOctaves + 1];
			float frequency = 1.0F;
			for (int i = 0; i < m_IntOctaves+1; i++) {
				m_Exponent [i] = (float)Math.Pow (m_Lacunarity, -inH);
				frequency *= m_Lacunarity;
			}
		
			if (noise == null)
				m_Noise = new Perlin ();
			else
				m_Noise = noise;
		}
	
		public float HybridMultifractal (float x, float y, float offset)
		{
			float weight, signal, remainder, result;
		
			result = (m_Noise.Noise (x, y) + offset) * m_Exponent [0];
			weight = result;
			x *= m_Lacunarity; 
			y *= m_Lacunarity;
			int i;
			for (i=1; i<m_IntOctaves; i++) {
				if (weight > 1.0F)
					weight = 1.0F;
				signal = (m_Noise.Noise (x, y) + offset) * m_Exponent [i];
				result += weight * signal;
				weight *= signal;
				x *= m_Lacunarity; 
				y *= m_Lacunarity;
			}
			remainder = m_Octaves - m_IntOctaves;
			result += remainder * m_Noise.Noise (x, y) * m_Exponent [i];
		
			return result;
		}
	
		public float RidgedMultifractal (float x, float y, float offset, float gain)
		{
			float weight, signal, result;
			int i;
		
			signal = Mathf.Abs (m_Noise.Noise (x, y));
			signal = offset - signal;
			signal *= signal;
			result = signal;
			weight = 1.0F;
	
			for (i=1; i<m_IntOctaves; i++) {
				x *= m_Lacunarity; 
				y *= m_Lacunarity;
			
				weight = signal * gain;
				weight = Mathf.Clamp01 (weight);
			
				signal = Mathf.Abs (m_Noise.Noise (x, y));
				signal = offset - signal;
				signal *= signal;
				signal *= weight;
				result += signal * m_Exponent [i];
			}
	
			return result;
		}

		public float BrownianMotion (float x, float y)
		{
			float value, remainder;
			long i;
		
			value = 0.0F;
			for (i=0; i<m_IntOctaves; i++) {
				value = m_Noise.Noise (x, y) * m_Exponent [i];
				x *= m_Lacunarity;
				y *= m_Lacunarity;
			}
			remainder = m_Octaves - m_IntOctaves;
			value += remainder * m_Noise.Noise (x, y) * m_Exponent [i];
		
			return value;
		}
	
		private Perlin  m_Noise;
		private float[] m_Exponent;
		private int     m_IntOctaves;
		private float   m_Octaves;
		private float   m_Lacunarity;
	}
	#endregion
	
	#region spring class
	public class Spring
	{

		// a spring can operate on either of three transform types:
		// position, rotation or scale
		public TransformType Modifier = TransformType.Position;
		public enum TransformType
		{
			Position,
			PositionAdditive,
			Rotation,
			RotationAdditive,
			Scale,
			ScaleAdditive
		}

		// a delegate is used to modify the parent transform
		// with the current motion values
		protected delegate void TransformDelegate ();

		protected TransformDelegate m_TransformFunc;

		// the 'State' variable dictates either position, rotation or scale
		// of this TransformModifier's gameobject, as defined by m_TransformType
		public Vector3 State = Vector3.zero;

		// properties can't be serialized, so this variable is used
		// to detect inspector changes to the transform type
		protected TransformType m_CurrentTransformType = TransformType.Position;

		// the current velocity of the spring, as resulting from added forces
		protected Vector3 m_Velocity = Vector3.zero;

		// the static equilibrium of this spring
		public Vector3 RestState = Vector3.zero;

		// mechanical strength of the spring
		public Vector3 Stiffness = new Vector3 (0.5f, 0.5f, 0.5f);

		// 'Damping' makes spring velocity wear off as it
		// approaches its rest state
		public Vector3 Damping = new Vector3 (0.75f, 0.75f, 0.75f);

		// force velocity fadein variables
		protected float m_VelocityFadeInCap = 1.0f;
		protected float m_VelocityFadeInEndTime = 0.0f;
		protected float m_VelocityFadeInLength = 0.0f;

		// transform limitations
		public float MaxVelocity = 10000.0f;
		public float MinVelocity = 0.0000001f;
		public Vector3 MaxState = new Vector3 (10000, 10000, 10000);
		public Vector3 MinState = new Vector3 (-10000, -10000, -10000);
	
		// transform & property
		protected Transform m_Transform;

		public Transform Transform {
			set {
				m_Transform = value;
				RefreshTransformType ();
			}
		}
		
		protected bool m_done = false;
		
		public bool Done
		{
			get {return m_done;}
			set {m_done = value;}
		}


		///////////////////////////////////////////////////////////
		// constructor
		///////////////////////////////////////////////////////////
		public Spring (Transform transform, TransformType modifier)
		{
			m_Transform = transform;
			Modifier = modifier;
			RefreshTransformType ();
		}


		///////////////////////////////////////////////////////////
		// this should be called from a monobehaviour's FixedUpdate,
		// i.e. 60 times per second
		///////////////////////////////////////////////////////////
		public void FixedUpdate ()
		{

			// handle forced velocity fadein
			if (m_VelocityFadeInEndTime > Time.time)
				m_VelocityFadeInCap = Mathf.Clamp01 (1 - ((m_VelocityFadeInEndTime - Time.time) / m_VelocityFadeInLength));
			else
				m_VelocityFadeInCap = 1.0f;

			// detect modifier changes
			if (Modifier != m_CurrentTransformType)
            {
                RefreshTransformType ();
            }
            
			m_TransformFunc ();

		}
	

		///////////////////////////////////////////////////////////
		// applies spring state to the transform's local position
		///////////////////////////////////////////////////////////
		void Position ()
		{
			Calculate ();
			m_Transform.localPosition = State;
		}


		///////////////////////////////////////////////////////////
		// additively applies spring state to the transform's position
		// 'additive' is for using a spring on an object which already
		// has another spring acting upon it (in order not to block
		// out the motion from the existing spring)
		///////////////////////////////////////////////////////////
		void PositionAdditive ()
		{
			Calculate ();
			m_Transform.localPosition += State;
		}


		///////////////////////////////////////////////////////////
		// applies spring state to the transform's local euler angles
		///////////////////////////////////////////////////////////
		void Rotation ()
		{
			Calculate ();
			m_Transform.localEulerAngles = State;
		}


		///////////////////////////////////////////////////////////
		// additively applies spring state to the transform's local
		// euler angles
		///////////////////////////////////////////////////////////
		void RotationAdditive ()
		{
			Calculate ();
			m_Transform.localEulerAngles += State;
		}


		///////////////////////////////////////////////////////////
		// applies spring state to the transform's local scale
		///////////////////////////////////////////////////////////
		void Scale ()
		{
			Calculate ();
			m_Transform.localScale = State;
		}


		///////////////////////////////////////////////////////////
		// additively applies spring state to the transform's local
		// scale
		///////////////////////////////////////////////////////////
		void ScaleAdditive ()
		{
			Calculate ();
			m_Transform.localScale += State;
		}


		///////////////////////////////////////////////////////////
		// this method sets the appropriate delegate for the trans-
		// formation and syncs 'State' with the parent transform
		///////////////////////////////////////////////////////////
		public void RefreshTransformType ()
		{

			switch (Modifier) {
			case TransformType.Position:
				State = m_Transform.localPosition;
				m_TransformFunc = new TransformDelegate (Position);
				break;
			case TransformType.Rotation:
				State = m_Transform.localEulerAngles;
				m_TransformFunc = new TransformDelegate (Rotation);
				break;
			case TransformType.Scale:
				State = m_Transform.localScale;
				m_TransformFunc = new TransformDelegate (Scale);
				break;
			case TransformType.PositionAdditive:
				State = m_Transform.localPosition;
				m_TransformFunc = new TransformDelegate (PositionAdditive);
				break;
			case TransformType.RotationAdditive:
				State = m_Transform.localEulerAngles;
				m_TransformFunc = new TransformDelegate (RotationAdditive);
				break;
			case TransformType.ScaleAdditive:
				State = m_Transform.localScale;
				m_TransformFunc = new TransformDelegate (ScaleAdditive);
				break;
			}

			m_CurrentTransformType = Modifier;

			RestState = State;

		}


		///////////////////////////////////////////////////////////
		// performs the spring physics calculations
		///////////////////////////////////////////////////////////
		protected void Calculate ()
		{
			if (State == RestState)
			{
				m_done = true;
				return;
			}

			Vector3 dist = (RestState - State);						// get distance to rest state
			m_Velocity += Vector3.Scale (dist, Stiffness);			// add distance * stiffness to velocity

			m_Velocity = (Vector3.Scale (m_Velocity, Damping));		// dampen velocity

			// clamp velocity to maximum
			m_Velocity = Vector3.ClampMagnitude (m_Velocity, MaxVelocity);

			// apply velocity, or stop if velocity is below minimum
			if (Mathf.Abs (m_Velocity.sqrMagnitude) > (MinVelocity * MinVelocity))
				Move ();
			else
				Reset ();

		}

	
		///////////////////////////////////////////////////////////
		// adds external velocity to the spring in one frame
		// NOTE: sometimes you may need to multiply 'force' with delta
		// time before calling AddForce, in order for the spring to be
		// framerate independent (even if the spring is updated in
		// FixedUpdate)
		///////////////////////////////////////////////////////////
		public void AddForce (Vector3 force)
		{

			force *= m_VelocityFadeInCap;
			m_Velocity += force;
			m_Velocity = Vector3.ClampMagnitude (m_Velocity, MaxVelocity);
			Move ();
			m_done = false;
		}

		public void AddForce (float x, float y, float z)
		{
			AddForce (new Vector3 (x, y, z));
		}


		///////////////////////////////////////////////////////////
		// adds velocity to the state and clamps state between min
		// and max values
		///////////////////////////////////////////////////////////
		protected void Move ()
		{
			State += m_Velocity;
			State = new Vector3 (Mathf.Clamp (State.x, MinState.x, MaxState.x),
							Mathf.Clamp (State.y, MinState.y, MaxState.y),
							Mathf.Clamp (State.z, MinState.z, MaxState.z));
		}


		///////////////////////////////////////////////////////////
		// stops spring velocity and resets state to the static
		// equilibrium
		///////////////////////////////////////////////////////////
		public void Reset ()
		{

			m_Velocity = Vector3.zero;
			State = RestState;
			m_done = true;
		}


		///////////////////////////////////////////////////////////
		// stops spring velocity
		///////////////////////////////////////////////////////////
		public void Stop ()
		{
			m_Velocity = Vector3.zero;
		}


		///////////////////////////////////////////////////////////
		// instantly strangles any forces added to the spring,
		// gradually easing them back in over 'seconds'.
		// this is useful when you need a spring to freeze up for a
		// brief amount of time, then slowly relaxing back to normal.
		///////////////////////////////////////////////////////////
		public void ForceVelocityFadeIn (float seconds)
		{

			m_VelocityFadeInLength = seconds;
			m_VelocityFadeInEndTime = Time.time + seconds;
			m_VelocityFadeInCap = 0.0f;

		}


	}
	#endregion
	
	public class CameraShakeEvent : XftEvent
	{
		protected Camera m_camera;
		protected bool m_eventFired = false;
		protected XftCameraShakeComp m_cameraShake = null;

        public CameraShakeEvent(XftEventComponent owner)
            : base(XftEventType.CameraShake, owner)
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
            Debug.LogError("can't find proper camera for CameraShakeEvent!");
        }
		
		public override void Initialize ()
		{
			FindCamera();
			ToggleCameraShakeComponent(false);
			m_elapsedTime = 0f;
			//m_cameraShake.Reset(m_owner);
		}
		
        public override void Reset()
        {
			//if there are other xffects using camera shake event, just ignore this one.
			//the rule : if a XftEvent have used CameraShake, it's his responsibility to disable it.
			if (m_cameraShake.enabled && m_cameraShake.Client != m_owner && m_cameraShake.Client != null)
			{
				Debug.Log("Camera Shake Event is in use! please wait a moment");
				m_eventFired = true;
				return;
			}
			
            m_elapsedTime = 0f;
			m_eventFired = false;
			//m_cameraShake.Reset(m_owner);
			m_editorFlag = false;
			m_cameraShake.EarthQuakeToggled = false;
        }
		
		public override void EditorDone ()
		{
			m_editorFlag = true;
		}
		
        protected void ToggleCameraShakeComponent(bool flag)
        {
            m_cameraShake = m_camera.gameObject.GetComponent<XftCameraShakeComp>();
            if (m_cameraShake == null)
            {
                m_cameraShake = m_camera.gameObject.AddComponent<XftCameraShakeComp>();
            }
            if (m_cameraShake.PositionSpring == null || m_cameraShake.RotationSpring == null)
			    m_cameraShake.Init();
            m_cameraShake.enabled = flag;
        }
		public override void Update (float deltaTime)
		{
			if (m_editorFlag)
				return;
			
			m_elapsedTime += deltaTime;
			if (m_elapsedTime > m_owner.StartTime && !m_eventFired)
			{
				m_eventFired = true;
                
                m_cameraShake.Reset(m_owner);
                

				//add instant force
				m_cameraShake.PositionSpring.AddForce(m_owner.PositionForce);
				m_cameraShake.RotationSpring.AddForce(m_owner.RotationForce);
				
				m_cameraShake.enabled = true;
				m_cameraShake.EarthQuakeToggled = true;
			}
		}
	}
}


