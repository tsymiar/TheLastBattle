using UnityEngine;
using System.Collections;

public class SM_rotateThis : MonoBehaviour 
{
    public float rotationSpeedX = 90;
    public float rotationSpeedY = 0;
    public float rotationSpeedZ = 0;

    private Quaternion rotation;
    
    // Use this for initialization
	void Awake () 
    {
        rotation = transform.rotation;
	}

    void Enable()
    {
        transform.rotation = rotation;
    }
	
	// Update is called once per frame
	void Update () 
    {
        transform.Rotate(new Vector3(rotationSpeedX,rotationSpeedY,rotationSpeedZ)*Time.deltaTime);	
	}
}



