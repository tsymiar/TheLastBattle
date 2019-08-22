using UnityEngine;
using System.Collections;

public class XftVolumeFogObject : MonoBehaviour {
    protected MeshRenderer FogObject;
    protected float Radius = 10f;
    public Material VolumFogMaterial;
    public Color FogColor = Color.white;

    void Awake()
    {
        FogObject = gameObject.GetComponent<MeshRenderer>();
        if (FogObject == null)
        {
            Debug.LogError("Volume Fog Object must have a MeshRenderer Component!");
        }
    }

    void OnEnable()
    {
        //Note: In forward lightning path, the depth texture is not automatically generated.
        if (Camera.main.depthTextureMode == DepthTextureMode.None)
            Camera.main.depthTextureMode = DepthTextureMode.Depth;
    }



    void Start()
    {
        FogObject.material = VolumFogMaterial;
        FogObject.material.SetColor("_FogColor", FogColor);
        Radius = (transform.localScale.x + transform.localScale.y + transform.localScale.z) / 6;
        FogObject.material.SetVector("FogParam", new Vector4(transform.position.x, transform.position.y, transform.position.z, Radius));
    }

	void Update () 
    {

	}
}
