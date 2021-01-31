using UnityEngine;
using System.Collections;

public class SimpleRotate : MonoBehaviour {

    protected float OriAngleX = 0;
    protected float OriAngleY = 0;
    public float RotateSpeed = 20f;
    public bool RotateX = true;
    public bool RotateY = false;

    void Start()
    {
        OriAngleX = transform.rotation.x;
        OriAngleY = transform.rotation.y;
    }

	// Update is called once per frame
	void Update () 
    {
        if (RotateX)
            OriAngleX += Time.deltaTime * RotateSpeed;
        if (RotateY)
            OriAngleY -= Time.deltaTime * RotateSpeed;
        transform.rotation = Quaternion.Euler(OriAngleX, OriAngleY, transform.rotation.z);
	}
}
