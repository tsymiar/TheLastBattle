using UnityEngine;
using System.Collections;
using System.Collections.Generic;

[ExecuteInEditMode]
[RequireComponent (typeof(Camera))]
public class XftRadialBlurTexAdd: MonoBehaviour
{
    protected Material m_material;
    public Shader RadialBlurShader;
    public float SampleDist;
    public float SampleStrength;
    public Texture2D Mask;
    
    public void Init (Shader shader)
    {
        RadialBlurShader = shader;
        if (m_material == null) {
            m_material = new Material (shader);
            m_material.hideFlags = HideFlags.HideAndDontSave;
        }
    }
 
    
    
    public Material MyMaterial {
        get {
            if (m_material == null) {
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
 
    void Awake ()
    {
        this.enabled = false;
    }

    void OnRenderImage (RenderTexture source, RenderTexture destination)
    {
        if (m_material == null) {
            return;
        }   
        MyMaterial.SetTexture("_Mask",Mask);
        MyMaterial.SetFloat("_SampleDist",SampleDist);
        MyMaterial.SetFloat("_SampleStrength",SampleStrength);
        Graphics.Blit (source, destination, m_material);
    }
}
