using UnityEngine;
using System.Collections;

//[ExecuteInEditMode]
public class ChangeRenderQ : MonoBehaviour {
	
	public int renderQueue = 3000;
    public Material mMat;
    bool change = false;
	
	void Start()
	{
		//Update();
	}
	
	void Update()
	{
        if (!change)
        {
            Renderer ren = renderer;
            if (ren == null)
            {
                ParticleSystem sys = GetComponent<ParticleSystem>();
                if (sys != null)
                    ren = sys.renderer;
            }

            //克隆材质
            if (ren != null && ren.sharedMaterial != null)
            {
                mMat = new Material(ren.sharedMaterial);
                mMat.renderQueue = renderQueue;
                ren.material = mMat;               
                change = true;
            }   
        } 
	}

    void OnDestroy()
    {
       // if (mMat != null) Destroy(mMat); 
    }
}
