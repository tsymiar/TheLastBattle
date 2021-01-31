using UnityEngine;
using System.Collections;

public class SM_TransRimShaderFader : MonoBehaviour 
{
    public float startStr = 2.0f;
    public float speed = 3.0f;
    private float timeGoes = 0.0f;
    private float currStr = 0.0f;


	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () 
    {
        timeGoes += Time.deltaTime * speed * startStr;
        currStr = startStr - timeGoes;
        renderer.material.SetFloat("_AllPower", currStr);	
	}
}
