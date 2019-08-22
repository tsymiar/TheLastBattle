using UnityEngine;
using System.Collections;

public class SM_UVScroller : MonoBehaviour {

    public int targetMaterialSlot = 0;
    public Material scrollThis;
    public float speedY = 0.5f;
    public float speedX = 0.0f;
    private float timeWentX = 0.0f;
    private float timeWentY = 0.0f;
    


	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () 
    {
        timeWentY += Time.deltaTime*speedY;
        timeWentX += Time.deltaTime*speedX;
        renderer.materials[targetMaterialSlot].SetTextureOffset ("_MainTex", new Vector2(timeWentX, timeWentY));	
	}
}
