using UnityEngine;
using System.Collections;

public class SM_TransRimShaderIrisator : MonoBehaviour 
{
    public float topStr = 2.0f;
    public float botStr = 1.0f;
    public float minSpeed = 1.0f;
    public float maxSpeed = 1.0f;
    private float speed = 0.0f;
    private float timeGoes;
    private bool timeGoesUp = true;

    void RandomizeSpeed()
    {
        speed = Random.Range(minSpeed, maxSpeed);
    }
	// Use this for initialization
	void Start () {
        timeGoes = botStr;
        speed = Random.Range(minSpeed, maxSpeed);
	}
	
	// Update is called once per frame
	void Update ()
    {
        if (timeGoes > topStr)
        {
            timeGoesUp = false;
            RandomizeSpeed();
        }

        if (timeGoes < botStr)
        {
            timeGoesUp = true;
            RandomizeSpeed();
        }


        if (timeGoesUp) { timeGoes += Time.deltaTime * speed; }
        if (!timeGoesUp) { timeGoes -= Time.deltaTime * speed; }

        var currStr = timeGoes;

        renderer.material.SetFloat("_AllPower", currStr);	
	}
}
