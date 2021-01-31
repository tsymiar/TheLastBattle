using UnityEngine;
using System.Collections;
using Xft;

[ExecuteInEditMode]
public class XftCameraShakeComp : MonoBehaviour
{
	
	public Spring PositionSpring = null;
	public Spring RotationSpring = null;
	
	protected XftEventComponent m_client;
	protected float m_elapsedTime = 0f;
	
	public bool EarthQuakeToggled = false;
	
	protected float m_earthQuakeTimeTemp = 0f;
    
	public XftEventComponent Client
	{
		get {return m_client;}
	}
	
	void Awake()
	{
		this.enabled = false;
	}

	public void Init ()
	{
		PositionSpring = new Spring (transform, Spring.TransformType.Position);
		PositionSpring.MinVelocity = 0.00001f;
		RotationSpring = new Spring (transform, Spring.TransformType.Rotation);
		RotationSpring.MinVelocity = 0.00001f;
	}
	
	public void Reset(XftEventComponent client)
	{
		
		if (m_client != null && !CheckDone())
		{
			//Debug.LogWarning("can't reset CameraShake Component, may be the last shake is not finished?");
			//return;
		}
		
		m_client = client;
		PositionSpring.Stiffness = new Vector3(m_client.PositionStifness,m_client.PositionStifness,m_client.PositionStifness);
		PositionSpring.Damping = Vector3.one - new Vector3(m_client.PositionDamping,m_client.PositionDamping,m_client.PositionDamping);
		
		RotationSpring.Stiffness = new Vector3(m_client.RotationStiffness,m_client.RotationStiffness,m_client.RotationStiffness);
		RotationSpring.Damping = Vector3.one - new Vector3(m_client.RotationDamping,m_client.RotationDamping,m_client.RotationDamping);
		m_elapsedTime = 0f;
		
		PositionSpring.RefreshTransformType();
		RotationSpring.RefreshTransformType();
		
		m_earthQuakeTimeTemp = m_client.EarthQuakeTime;
	}
	
	//constant spring force.
	void UpdateEarthQuake()
	{
		if (m_client == null || !m_client.UseEarthQuake || m_earthQuakeTimeTemp <= 0.0f || !EarthQuakeToggled)
			return;
  
		m_elapsedTime += Time.deltaTime;
		
		m_earthQuakeTimeTemp -= 0.0166f * (60 * Time.deltaTime);
		
		float magnitude = 0f;
		if (m_client.EarthQuakeMagTye == MAGTYPE.Fixed)
			magnitude= m_client.EarthQuakeMagnitude;
		else
			magnitude = m_client.EarthQuakeMagCurve.Evaluate(m_elapsedTime);
			
		// the horisontal move is a perlin noise value between 0 and
		// 'EarthQuakeMagnitude' (depending on 'EarthQuakeTime').
		// horizMove will ease out during the last second.
		Vector3 horizMove = Vector3.Scale(XftSmoothRandom.GetVector3Centered(1), new Vector3(magnitude,
													0, magnitude)) * Mathf.Min(m_earthQuakeTimeTemp, 1.0f);

		// the vertical move is half the earthquake magnitude and
		// has a 30% chance of occuring each frame. when it does,
		// it alternates between positive and negative. this produces
		// sharp shakes with nice spring smoothness inbetween.
		// vertMove will ease out during the last second.
		float vertMove = 0;
		if (UnityEngine.Random.value < 0.3f)
		{
			vertMove = UnityEngine.Random.Range(0, (magnitude * 0.35f)) * Mathf.Min(m_earthQuakeTimeTemp, 1.0f);
			if (PositionSpring.State.y >= PositionSpring.RestState.y)
				vertMove = -vertMove;
		}
		
		PositionSpring.AddForce(horizMove);

		// apply earthquake roll force on the camera rotation spring
		RotationSpring.AddForce(new Vector3(0, 0, -horizMove.x * 2) * m_client.EarthQuakeCameraRollFactor);
		
		// vertical move is always applied to the camera spring.
		PositionSpring.AddForce(new Vector3(0, vertMove, 0));
	}
	
	public bool CheckDone()
	{
		if (PositionSpring.Done && RotationSpring.Done)
			return true;
		return false;
	}
	
	void Update ()
	{
		
        if (PositionSpring == null || RotationSpring == null)
        {
            //not inited
            return;
        }
        
		UpdateEarthQuake();
		
		if (CheckDone())
		{
			this.enabled = false;
			return;
		}
		PositionSpring.FixedUpdate();
		RotationSpring.FixedUpdate();
	}
}
