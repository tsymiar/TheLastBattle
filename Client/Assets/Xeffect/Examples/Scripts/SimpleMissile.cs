using UnityEngine;
using System.Collections;

public class SimpleMissile : MonoBehaviour {

    public float Life = 3f;
    public float OriVelocity = 50f;
    public float Accelaration = 10f;
    public string ExplodeXftName = "";
    public XffectCache EffectCache;

    protected float ElapsedTime = 0f;
    protected XffectComponent Xffect;
    protected float Velocity;

    void Start()
    {
        Xffect = transform.GetComponent<XffectComponent>();
        Reset();
    }

	void Update () 
    {
        ElapsedTime += Time.deltaTime;
        if (ElapsedTime > Life)
        {
            ElapsedTime = 0f;
            Xffect.DeActive();
            if (ExplodeXftName != "")
                EffectCache.ReleaseEffect(ExplodeXftName, transform.position);
        }
        Velocity += Accelaration * Time.deltaTime;
        transform.position -= transform.forward * Velocity * Time.deltaTime;
	}

    public void Reset()
    {
        Velocity = OriVelocity;
        ElapsedTime = 0f;
    }
}
