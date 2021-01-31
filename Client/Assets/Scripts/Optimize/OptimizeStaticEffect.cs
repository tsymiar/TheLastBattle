using UnityEngine;
using System.Collections.Generic;
using GameDefine;


//优化静态特效,不可见停止渲染以及更新
public class OptimizeStaticEffect : MonoBehaviour
{
    //保存碰撞体
    Collider collider = null;
    //包围盒子
    Bounds bounds;
    //是否可见
    bool mVisible = true;

    //子GameObject
    List<GameObject> childObjs = new List<GameObject>();


    // Use this for initialization
    void Start()
    {
        collider = GetComponent<Collider>();
        if (collider == null)
        {
            Debug.LogError("Optimize static effect should have collider!");
        }

        //获取子object        
        for (int i = 0; i < gameObject.transform.childCount; i++)
        {
            Transform ts = gameObject.transform.GetChild(i);
            GameObject psObj = ts.gameObject;
            childObjs.Add(psObj);
        }
    }

    float mTotalTime = 0;
    // Update is called once per frame
    void Update()
    {
        //每隔0.1更新一次
        mTotalTime += Time.deltaTime;
        if (mTotalTime <= 0.1f)
        {
            return;
        }
        else
        {
            mTotalTime = 0;
        }


        //是否使用优化方案
        if (!GameMethod.GetMainCamera.useOptimizeStaticEffect)
            return;

        if (collider == null)
            return;

        Plane[] planes = GameMethod.GetMainCamera.planes;
        if (planes == null)
            return;

        //获取碰撞体的包围盒子
        BoxCollider boxCollider = collider as BoxCollider;      
        bounds = new Bounds(boxCollider.bounds.center, boxCollider.size);
               
      
        bool isVisible = GameMethod.GetMainCamera.IsInFrustum(planes, bounds);
        if (isVisible != mVisible)
        {
            mVisible = isVisible;
            if (mVisible)
            {
                setActive(true);
            }
            else
            {
                setActive(false);
            }
        }
    }

    void setActive(bool flag)
    {
        //collider.enabled = flag;        
        foreach (GameObject obj in childObjs)
        {
            obj.SetActive(flag);
        }
    }
}
