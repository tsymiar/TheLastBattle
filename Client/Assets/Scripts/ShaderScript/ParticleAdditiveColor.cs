using UnityEngine;
using System.Collections;


[ExecuteInEditMode]
public class ParticleAdditiveColor : MonoBehaviour 
{
    public Color color = new Color(1,1,1,1);

	// Use this for initialization
	void Awake() 
    {
        //运行模式
        if (Application.isPlaying)
        {
            //获取材质信息
            MeshRenderer meshRender = gameObject.GetComponent<MeshRenderer>();
            if (meshRender != null)
            {
               // meshRender.sharedMaterial.color = Color.black;

                //meshRender.sharedMaterial.SetVector("_TintColor", new Vector4(1, 0, 0, 1));
                MaterialPropertyBlock block = new MaterialPropertyBlock();
                block.AddVector("_TintColor", new Vector4(color.r, color.g, color.b, 1));
                meshRender.SetPropertyBlock(block);
                //Debug.Log("run" + color.ToString());
            }
        }
	}
	
	// Update is called once per frame
	void Update () 
    {
        //编辑模式
        if (!Application.isPlaying)
        {
            //获取材质信息
            MeshRenderer meshRender = gameObject.GetComponent<MeshRenderer>();
            if (meshRender != null)
            {
                // meshRender.sharedMaterial.color = Color.black;

                //meshRender.sharedMaterial.SetVector("_TintColor", new Vector4(1, 0, 0, 1));
                MaterialPropertyBlock block = new MaterialPropertyBlock();
                block.AddVector("_TintColor", new Vector4(color.r, color.g, color.b, 1));
                meshRender.SetPropertyBlock(block);
                //Debug.Log("editor" + color.ToString());
            }
        }
	}
}
