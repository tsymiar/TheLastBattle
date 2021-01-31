using UnityEngine;
using System.Collections;

[ExecuteInEditMode]
[RequireComponent (typeof(Camera))]
public class XftRadialBlur : MonoBehaviour
{
	protected Material m_material;
	public float SampleDist = 1f;
	public float SampleStrength = 1f;
	public Vector3 Center = new Vector3 (0.5f, 0.5f, 0f);
	public Shader RadialBlurShader;

	public void Init (Shader shader)
	{
		RadialBlurShader = shader;
		if (m_material == null) {
			m_material = new Material (shader);
			m_material.hideFlags = HideFlags.HideAndDontSave;
		}
	}
	
	public Material MyMaterial
	{
		get 
		{
			if (m_material == null) 
			{
				m_material = new Material (RadialBlurShader);
				m_material.hideFlags = HideFlags.HideAndDontSave;
			}
			return m_material;
		}
	}
	
	public bool CheckSupport ()
	{
		bool ret = true;
		if (!SystemInfo.supportsImageEffects)
			ret = false;
		
		if (!MyMaterial.shader.isSupported)
			ret = false;
		
		return ret;
	}
	
	
	void Awake()
	{
		this.enabled = false;
	}

	void OnRenderImage (RenderTexture source, RenderTexture destination)
	{
		if (m_material == null) {
			return;
		}   
		//MyMaterial.SetFloat ("_SampleDist", SampleDist);
		MyMaterial.SetFloat ("_SampleStrength", SampleStrength);
		Vector4 center = Vector4.zero;
		center.x = Center.x / Screen.width;
		center.y = Center.y / Screen.height;
		MyMaterial.SetVector ("_Center", center);
		Graphics.Blit (source, destination, m_material);
	}
}
