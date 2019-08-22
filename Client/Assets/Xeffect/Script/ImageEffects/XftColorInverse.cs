using UnityEngine;
using System.Collections;

[ExecuteInEditMode]
[RequireComponent (typeof(Camera))]
public class XftColorInverse : MonoBehaviour
{
    protected Material m_material;
    public float Strength;
    public Shader ColorInverseShader;

    public void Init (Shader shader)
    {
        ColorInverseShader = shader;
        if (m_material == null) {
            m_material = new Material (ColorInverseShader);
            m_material.hideFlags = HideFlags.HideAndDontSave;
        }
 }
 
 public Material MyMaterial
 {
     get 
     {
         if (m_material == null) 
         {
             m_material = new Material (ColorInverseShader);
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
     MyMaterial.SetFloat ("_Strength", Strength);
     Graphics.Blit (source, destination, m_material);
 }
}
