using UnityEngine;
using System.Collections;
using Xft;


public enum XftEventType
{
	CameraShake,
	Sound,
	Light,
	CameraRadialBlur,
	CameraGlow,
    CameraRadialBlurMask,
    CameraColorInverse,
    TimeScale,
}

public class XftEventComponent : MonoBehaviour 
{
	
	public XftEventType EventType;
	
    public float StartTime = 0f;
    public float EndTime = 10f;

    public Shader RadialBlurShader;
    public Transform RadialBlurObj;
    public float RBSampleDist = 0.3f;
	
	public MAGTYPE RBStrengthType = MAGTYPE.Fixed;
	public float RBSampleStrength = 1f;
	public AnimationCurve RBSampleStrengthCurve = null;
	
	
    //radial blur tex add event
    public Shader RadialBlurTexAddShader;
    public Texture2D RadialBlurMask;
    
    public float RBMaskSampleDist = 3f;
    public MAGTYPE RBMaskStrengthType = MAGTYPE.Fixed;
    public float RBMaskSampleStrength = 5f;
    public AnimationCurve RBMaskSampleStrengthCurve = null;
    
	//glow event
	public Shader GlowCompositeShader;
	public Shader GlowBlurShader;
	public Shader GlowDownSampleShader;
	public float GlowIntensity = 1.5f;
	public int GlowBlurIterations = 3;
	public float GlowBlurSpread = 0.7f;
	public Color GlowColorStart = Color.clear;
	public Color GlowColorEnd = new Color(1f,1f,1f,0f);
	public COLOR_GRADUAL_TYPE GlowColorGradualType = COLOR_GRADUAL_TYPE.CLAMP;
	public float GlowColorGradualTime = 2f;
    public AnimationCurve ColorCurve = null;
    
    //color inverse event
    public Shader ColorInverseShader;
    public AnimationCurve CIStrengthCurve = null;
	
	//Sound Event
	public AudioClip Clip;
	public float Volume = 1f;
	public float Pitch = 1f;
	
	//Camera Shake Event
	public Vector3 PositionForce = new Vector3(0,6,0);
	public Vector3 RotationForce = Vector3.zero;
	public float PositionStifness = 0.3f;
	public float PositionDamping = 0.1f;
	public float RotationStiffness = 0.1f;
	public float RotationDamping = 0.25f;
	
	public bool UseEarthQuake = false;
	public float EarthQuakeMagnitude = 2f;
	public MAGTYPE EarthQuakeMagTye = MAGTYPE.Fixed;
	public AnimationCurve EarthQuakeMagCurve = null;
	public float EarthQuakeTime = 2f;
	public float EarthQuakeCameraRollFactor = 0.1f;
	
	//Light Event
	public Light LightComp;
	public float LightIntensity = 1f;
	public MAGTYPE LightIntensityType = MAGTYPE.Fixed;
	public AnimationCurve LightIntensityCurve = null;
	public float LightRange = 10f;
	public MAGTYPE LightRangeType = MAGTYPE.Fixed;
	public AnimationCurve LightRangeCurve = null;
	
    
    //Time Scale Event
    public float TimeScale = 1f;
    public float TimeScaleDuration = 1f;

    protected XftEvent m_eventHandler;


    public void Initialize()
    {
		switch (EventType)
		{
		case XftEventType.CameraShake:
			m_eventHandler = new CameraShakeEvent(this);
			break;
		case XftEventType.Light:
			m_eventHandler = new LightEvent(this);
			break;
		case XftEventType.Sound:
			m_eventHandler = new SoundEvent(this);
			break;
		case XftEventType.CameraRadialBlur:
			m_eventHandler = new RadialBlurEvent(this);
			break;
        case XftEventType.CameraRadialBlurMask:
            m_eventHandler = new RadialBlurTexAddEvent(this);
            break;
		case XftEventType.CameraGlow:
			m_eventHandler = new GlowEvent(this);
			break;
        case XftEventType.CameraColorInverse:
            m_eventHandler = new ColorInverseEvent(this);
            break;
        case XftEventType.TimeScale:
            m_eventHandler = new TimeScaleEvent(this);
            break;
		default:
			Debug.LogWarning("invalid event type!");
			break;
		}
        m_eventHandler.Initialize();
    }

    public void ResetCustom()
    {
		if (m_eventHandler != null)
        	m_eventHandler.Reset();
    }

    public void UpdateCustom(float deltaTime)
    {
		if (m_eventHandler != null)
        	m_eventHandler.Update(deltaTime);
    }
	
	public void EditorDone()
	{
		if (m_eventHandler != null)
		{
			m_eventHandler.EditorDone();
		}
	}
}
