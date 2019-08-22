using UnityEngine;
using System.Collections;
using BlGame.Effect;

public class EffectScript : MonoBehaviour
{
    /// <summary>
    /// 特效生命周期
    /// </summary>
    public float lifeTime = 2.0f;

    //特效显示等级
    public EffectLodLevel lodLevel = EffectLodLevel.High;

    // Use this for initialization
    void Start()
    {
        //编辑器模式
        if (Application.isEditor && !Application.isPlaying)
        {
            if (lifeTime == 0)
                lifeTime = 10000000;

            DestroyObject(gameObject, lifeTime);
        }
    }

    // Update is called once per frame
    void Update()
    {

    }

    void OnDestroy()
    {

    }
}


