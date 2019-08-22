using UnityEngine;
using System.Collections;

public class SM_trailFade : MonoBehaviour 
{

    public float fadeInTime = 0.1f;
    public float stayTime = 1.0f;
    public float fadeOutTime = 0.7f;
    public TrailRenderer thisTrail = null;
    private float timeElapsed = 0.0f;
    private float timeElapsedLast = 0.0f;
    private float percent;


	// Use this for initialization
	void Start () {
        thisTrail.material.SetColor("_TintColor", new Color(0.5f, 0.5f, 0.5f, 1));
        if (fadeInTime < 0.01) fadeInTime = 0.01f; //hack to avoid division with zero
        percent = timeElapsed / fadeInTime;	
	}
	
	// Update is called once per frame
	void Update () 
    {
        timeElapsed += Time.deltaTime;


        if (timeElapsed <= fadeInTime) //fade in
        {
            percent = timeElapsed / fadeInTime;
            thisTrail.material.SetColor("_TintColor", new Color(0.5f, 0.5f, 0.5f, percent));
        }

        if ((timeElapsed > fadeInTime) && (timeElapsed < fadeInTime + stayTime)) //set the normal color
        {
            thisTrail.material.SetColor("_TintColor", new Color(0.5f, 0.5f, 0.5f, 1));
        }

        if (timeElapsed >= fadeInTime + stayTime && timeElapsed < fadeInTime + stayTime + fadeOutTime) //fade out
        {
            timeElapsedLast += Time.deltaTime;
            percent = 1 - (timeElapsedLast / fadeOutTime);
            thisTrail.material.SetColor("_TintColor", new Color(0.5f, 0.5f, 0.5f, percent));
        }	
	}
}
