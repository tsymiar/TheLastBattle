using UnityEngine;
using System.Collections;

public class SM_moveThis : MonoBehaviour 
{
    public float translationSpeedX = 0;
    public float translationSpeedY = 1;
    public float translationSpeedZ = 0;
    public bool local = true;

    //保存初始位置
    private Vector3 mPostion;


    void Awake()
    {
        mPostion = transform.position;
    }

    void OnEnable()
    {

        transform.position = mPostion;      
    }

    void OnDisable()
    {

    }

	// Use this for initialization
	void Start ()
    {
	
	}
	
	// Update is called once per frame
	void Update ()
    {
        if (local == true)
        {
            transform.Translate(new Vector3(translationSpeedX, translationSpeedY, translationSpeedZ) * Time.deltaTime);
        }

        if (local == false)
        {
            transform.Translate(new Vector3(translationSpeedX, translationSpeedY, translationSpeedZ) * Time.deltaTime, Space.World);
        }
	}
}
