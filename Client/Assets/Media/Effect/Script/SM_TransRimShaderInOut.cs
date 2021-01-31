using UnityEngine;
using System.Collections;

public class NewBehaviourScript : MonoBehaviour 
{
    public float str = 1.0f;
    public float fadeIn = 1.0f;
    public float stay = 1.0f;
    public float fadeOut = 1.0f;
    private float timeGoes = 0.0f;
    private float currStr = 0.0f;

	// Use this for initialization
	void Start () 
    {
        renderer.material.SetFloat("_AllPower", currStr);	
	}
	
	// Update is called once per frame
	void Update ()
    {
        timeGoes += Time.deltaTime;

        if (timeGoes < fadeIn)
        {
            currStr = timeGoes * str * (1 / fadeIn);
        }

        if ((timeGoes > fadeIn) && (timeGoes < fadeIn + stay))
        {
            currStr = str;
        }

        if (timeGoes > fadeIn + stay)
        {
            currStr = str - ((timeGoes - (fadeIn + stay)) * (1 / fadeOut));
        }

        //currStr=startStr-timeGoes;
        renderer.material.SetFloat("_AllPower", currStr);	
	}
}
