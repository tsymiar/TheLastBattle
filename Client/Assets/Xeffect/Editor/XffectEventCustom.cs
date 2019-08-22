using UnityEngine;
using System.Collections;
using UnityEditor;
using Xft;

[CustomEditor(typeof(XftEventComponent))]
public class XffectEventCustom : Editor 
{
	public static bool DisplayRadialBlurConfig = false;
    public static bool DisplayRadialBlurTexAddConfig = false;
	public static bool DisplayGlowConfig = false;
    public static bool DisplayColorInverseConfig = false;
    public static bool DisplayTimeScaleConfig = false;
	public static bool DisplaySoundConfig = false;
	public static bool DisplayCameraShakeConfig = false;
	public static bool DisplayLightConfig = false;
	
	public override void OnInspectorGUI()
	{
		XftEventComponent ctarget = (XftEventComponent)target;
		EditorGUILayout.BeginVertical();
		
		ctarget.EventType = (XftEventType)EditorGUILayout.EnumPopup("event type:", ctarget.EventType);
		
		ctarget.StartTime = EditorGUILayout.FloatField("start time:", ctarget.StartTime);
		ctarget.EndTime = EditorGUILayout.FloatField("end time:", ctarget.EndTime);
		
		switch (ctarget.EventType)
		{
		case XftEventType.CameraShake:
			CameraShakeConfig(ctarget);
			break;
		case XftEventType.Sound:
			SoundConfig(ctarget);
			break;
		case XftEventType.Light:
			LightConfig(ctarget);
			break;
		case XftEventType.CameraRadialBlur:
			RadialBlurConfig(ctarget);
			break;
        case XftEventType.CameraRadialBlurMask:
            RadialBlurTexAddConfig(ctarget);
            break;
		case XftEventType.CameraGlow:
			GlowConfig(ctarget);
			break;
        case XftEventType.CameraColorInverse:
            ColorInverseConfig(ctarget);
            break;
        case XftEventType.TimeScale:
            TimeScaleEventConfig(ctarget);
            break;
		default:
			break;
		}
		
		EditorGUILayout.EndVertical();
		
		if (GUI.changed)
        {
            EditorUtility.SetDirty(target);
        }
	}
	
    
    protected void TimeScaleEventConfig(XftEventComponent ctarget)
    {
        DisplayTimeScaleConfig = EditorGUILayout.Foldout(DisplayTimeScaleConfig, "Time Scale Configuration");
        //if (DisplayTimeScaleConfig)
        {
            EditorGUILayout.BeginVertical();
            ctarget.TimeScale = EditorGUILayout.FloatField("time scale:",ctarget.TimeScale);
            ctarget.TimeScaleDuration = EditorGUILayout.FloatField("duration:",ctarget.TimeScaleDuration);
            EditorGUILayout.EndVertical();
        }
    }
    
    protected void ColorInverseConfig(XftEventComponent ctarget)
    {
        DisplayColorInverseConfig = EditorGUILayout.Foldout(DisplayColorInverseConfig, "Color Inverse Configuration");
     
        //if (DisplayColorInverseConfig)
        {
            EditorGUILayout.BeginVertical();

            ctarget.ColorInverseShader = (Shader)EditorGUILayout.ObjectField("color inverse shader:",ctarget.ColorInverseShader, typeof(Shader),true);
            
            ctarget.CIStrengthCurve = EditorGUILayout.CurveField("strength curve:", ctarget.CIStrengthCurve,
                Color.green, new Rect(0f,0f,99f,1f));
            EditorGUILayout.EndVertical();
        }
    }
    
	protected void RadialBlurConfig(XftEventComponent ctarget)
	{
		DisplayRadialBlurConfig = EditorGUILayout.Foldout(DisplayRadialBlurConfig, "RadialBlur Configuration");
		
		//if (DisplayRadialBlurConfig)
        {
            EditorGUILayout.BeginVertical();
            
            EditorGUILayout.LabelField("[note:if you want to use radial blur in mobile,");
            EditorGUILayout.LabelField("use 'Camera Radial Blur Mask' instead.]");
            
            ctarget.RadialBlurShader = (Shader)EditorGUILayout.ObjectField("radial blur shader:",ctarget.RadialBlurShader, typeof(Shader),true);

			ctarget.RBStrengthType = (MAGTYPE)EditorGUILayout.EnumPopup("sample strength type:", ctarget.RBStrengthType);
			
			if (ctarget.RBStrengthType == MAGTYPE.Fixed)
				ctarget.RBSampleStrength = EditorGUILayout.Slider("sample strength:",ctarget.RBSampleStrength,0f,1f);
			else
				ctarget.RBSampleStrengthCurve = EditorGUILayout.CurveField("strength curve:", ctarget.RBSampleStrengthCurve,
                    Color.green, new Rect(0f,0f,99f,1f));
            EditorGUILayout.EndVertical();
        }
	}
	
    
    protected void RadialBlurTexAddConfig(XftEventComponent ctarget)
    {
        DisplayRadialBlurTexAddConfig = EditorGUILayout.Foldout(DisplayRadialBlurTexAddConfig, "RadialBlur Mask Configuration");
     
     //if (DisplayRadialBlurTexAddConfig)
        {
            EditorGUILayout.BeginVertical();
            
            if (ctarget.RadialBlurTexAddShader == null)
            {
                ctarget.RadialBlurTexAddShader = Shader.Find ("Xffect/PP/radial_blur_mask");
            }
            
            ctarget.RadialBlurTexAddShader = (Shader)EditorGUILayout.ObjectField("radial blur mask shader:",ctarget.RadialBlurTexAddShader, typeof(Shader),true);
            string assetPath = AssetDatabase.GetAssetPath(ctarget.RadialBlurTexAddShader);
            int index = assetPath.LastIndexOf("Xffect");
            string basePath = assetPath.Substring (0,index+7);
            string texPath = basePath + "Resources/PostProcess/Textures/radial_mask1.psd";
            
            
            if (ctarget.RadialBlurMask == null)
                ctarget.RadialBlurMask = (Texture2D)AssetDatabase.LoadAssetAtPath(texPath,typeof(Texture2D));
            
            ctarget.RadialBlurMask = (Texture2D)EditorGUILayout.ObjectField("radial blur mask:",ctarget.RadialBlurMask, typeof(Texture2D),false);
            
            ctarget.RBMaskSampleDist = EditorGUILayout.FloatField("sample dist:",ctarget.RBMaskSampleDist);
           
            
            ctarget.RBMaskStrengthType = (MAGTYPE)EditorGUILayout.EnumPopup("sample strength type:", ctarget.RBMaskStrengthType);
         
            if (ctarget.RBMaskStrengthType == MAGTYPE.Fixed)
             ctarget.RBMaskSampleStrength = EditorGUILayout.Slider("sample strength:",ctarget.RBMaskSampleStrength,0f,1f);
            else
             ctarget.RBMaskSampleStrengthCurve = EditorGUILayout.CurveField("strength curve:", ctarget.RBMaskSampleStrengthCurve);
            
            EditorGUILayout.EndVertical();
        }
    }
    
	protected void GlowConfig(XftEventComponent ctarget)
	{
		DisplayGlowConfig = EditorGUILayout.Foldout(DisplayGlowConfig, "Glow Configuration");
		
		//if (DisplayGlowConfig)
        {
            EditorGUILayout.BeginVertical();
            
            ctarget.GlowCompositeShader = (Shader)EditorGUILayout.ObjectField("composite shader:",ctarget.GlowCompositeShader, typeof(Shader),true);
            ctarget.GlowDownSampleShader = (Shader)EditorGUILayout.ObjectField("down sample shader:",ctarget.GlowDownSampleShader, typeof(Shader),true);
            ctarget.GlowBlurShader = (Shader)EditorGUILayout.ObjectField("blur shader:",ctarget.GlowBlurShader, typeof(Shader),true);
            
			ctarget.GlowIntensity = EditorGUILayout.Slider("glow intensity:", ctarget.GlowIntensity,0f,10f);
			ctarget.GlowBlurIterations = EditorGUILayout.IntSlider("blur iterations:",ctarget.GlowBlurIterations,0,30);
			ctarget.GlowBlurSpread = EditorGUILayout.Slider("blur spread:", ctarget.GlowBlurSpread,0.5f,1f);
			
			EditorGUILayout.Space();
			ctarget.GlowColorGradualType = (COLOR_GRADUAL_TYPE)EditorGUILayout.EnumPopup("gradual type:", ctarget.GlowColorGradualType);
            if (ctarget.GlowColorGradualType == COLOR_GRADUAL_TYPE.CURVE)
            {
                ctarget.ColorCurve = EditorGUILayout.CurveField("color curve:", ctarget.ColorCurve,
                Color.green, new Rect(0f,0f,99f,1f));
            }
            else
            {
                ctarget.GlowColorGradualTime = EditorGUILayout.FloatField("color gradual time:", ctarget.GlowColorGradualTime);
            }
            ctarget.GlowColorStart = EditorGUILayout.ColorField("glow color start:", ctarget.GlowColorStart);
			ctarget.GlowColorEnd = EditorGUILayout.ColorField("glow color end:", ctarget.GlowColorEnd);
			
			
			
			EditorGUILayout.EndVertical();
        }
	}
	
	protected void SoundConfig(XftEventComponent ctarget)
	{
		DisplaySoundConfig = EditorGUILayout.Foldout(DisplaySoundConfig, "Sound Configuration");
		//if (DisplaySoundConfig)
		{
			EditorGUILayout.BeginVertical();
			ctarget.Clip = (AudioClip)EditorGUILayout.ObjectField("audio clip:",ctarget.Clip,typeof(AudioClip),true);
			ctarget.Volume = EditorGUILayout.FloatField("volume:", ctarget.Volume);
			ctarget.Pitch = EditorGUILayout.FloatField("pitch:",ctarget.Pitch);
			EditorGUILayout.EndVertical();
		}
	}
	
	protected void LightConfig(XftEventComponent ctarget)
	{
		DisplayLightConfig = EditorGUILayout.Foldout(DisplayLightConfig, "Light Configuration");
		//if (DisplayLightConfig)
		{
			EditorGUILayout.BeginVertical();
			
			ctarget.LightComp = (Light)EditorGUILayout.ObjectField("light obj:",ctarget.LightComp,typeof(Light),true);
			
			ctarget.LightIntensityType = (MAGTYPE)EditorGUILayout.EnumPopup("intensity type:", ctarget.LightIntensityType);
			if (ctarget.LightIntensityType == MAGTYPE.Curve)
				ctarget.LightIntensityCurve = EditorGUILayout.CurveField("intensity curve:", ctarget.LightIntensityCurve);
			else
				ctarget.LightIntensity = EditorGUILayout.FloatField("intensity:", ctarget.LightIntensity);
			EditorGUILayout.Space();
			
			ctarget.LightRangeType = (MAGTYPE)EditorGUILayout.EnumPopup("range type:", ctarget.LightRangeType);
			if (ctarget.LightRangeType == MAGTYPE.Curve)
				ctarget.LightRangeCurve = EditorGUILayout.CurveField("range curve:", ctarget.LightRangeCurve);
			else
				ctarget.LightRange = EditorGUILayout.FloatField("range:", ctarget.LightRange);
			EditorGUILayout.EndVertical();
		}
	}
	
	protected void CameraShakeConfig(XftEventComponent ctarget)
	{
		DisplayCameraShakeConfig = EditorGUILayout.Foldout(DisplayCameraShakeConfig, "CameraShake Configuration");
		//if (DisplayCameraShakeConfig)
		{
			EditorGUILayout.BeginVertical();
			EditorGUILayout.LabelField("[note camera shake will change");
            EditorGUILayout.LabelField("the camera's local position directly.]");
			ctarget.PositionStifness = EditorGUILayout.FloatField("position stiffness", ctarget.PositionStifness);
			ctarget.PositionDamping = EditorGUILayout.FloatField("position damping:", ctarget.PositionDamping);
			EditorGUILayout.Space();
			ctarget.RotationStiffness = EditorGUILayout.FloatField("rotation stiffness:", ctarget.RotationStiffness);
			ctarget.RotationDamping = EditorGUILayout.FloatField("rotation damping:", ctarget.RotationDamping);
			EditorGUILayout.Space();
			EditorGUILayout.LabelField("spring force:");
			ctarget.PositionForce = EditorGUILayout.Vector3Field("position force:", ctarget.PositionForce);
			ctarget.RotationForce = EditorGUILayout.Vector3Field("rotation force:", ctarget.RotationForce);
			EditorGUILayout.Space();
			ctarget.UseEarthQuake = EditorGUILayout.Toggle("use earthquake?", ctarget.UseEarthQuake);
			if (ctarget.UseEarthQuake)
			{
				ctarget.EarthQuakeTime = EditorGUILayout.FloatField("earthquake time:", ctarget.EarthQuakeTime);
				ctarget.EarthQuakeCameraRollFactor = EditorGUILayout.FloatField("roll factor:", ctarget.EarthQuakeCameraRollFactor);
				ctarget.EarthQuakeMagTye = (MAGTYPE)EditorGUILayout.EnumPopup("magnitude type:", ctarget.EarthQuakeMagTye);
				if (ctarget.EarthQuakeMagTye == MAGTYPE.Curve)
				{
					ctarget.EarthQuakeMagCurve = EditorGUILayout.CurveField("magnitude curve:", ctarget.EarthQuakeMagCurve);
				}
				else
				{
					ctarget.EarthQuakeMagnitude = EditorGUILayout.FloatField("earthquake magnitude:", ctarget.EarthQuakeMagnitude);
				}
			}
			
			EditorGUILayout.EndVertical();
		}
	}
}
