using UnityEngine;
using System.Collections;

public class SetActiveScript : MonoBehaviour
{
    public bool active = false;
    public float time = 1.0f;

    private float currentTime = 0.0f;
	// Use this for initialization
	void Start () 
    {

       	
	}
	
	// Update is called once per frame
	void Update () 
    {
        currentTime += Time.deltaTime;

        if (currentTime >= time)
        {
            gameObject.SetActive(active);
            currentTime = 0;
        }
	}
}
