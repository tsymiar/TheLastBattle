using UnityEngine;
using System.Collections.Generic;
using GameDefine;


//优化动态建筑(带动画),不可见停止渲染以及更新
public class OptimizeDynamicBuild : MonoBehaviour 
{
    //保存碰撞体
    Collider collider = null;
    //包围盒子
    Bounds bounds;
    //是否可见
    bool mVisible = true;
    //Entity脚本 暂时不处理
    //Entity entityScript = null;

    //子GameObject
    List<GameObject> childObjs = new List<GameObject>();
     
      
	// Use this for initialization
	void Start ()
    {
        collider = GetComponent<Collider>();
        if (collider == null)
        {
            Debug.LogError("Optimize dynamic building should have collider!");
        }  
        
         //获取子object        
        for (int i = 0; i < gameObject.transform.childCount; i++)
        {
            Transform ts = gameObject.transform.GetChild(i);
            GameObject psObj = ts.gameObject;

            if (psObj.tag != "posui" && psObj.name != "hitpoint" && psObj.name != "buffpoint" && psObj.name != "point")
                childObjs.Add(psObj);
        }             
	}
	
	// Update is called once per frame
    float mTotalTime = 0;
	void Update () 
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


        if (!GameMethod.GetMainCamera.useOptimizeDynamicBuild)
            return;

        if(collider == null)
            return;

        Plane[] planes = GameMethod.GetMainCamera.planes;
        if (planes == null)
            return;

        //获取碰撞体的包围盒子
        bounds = collider.bounds;

        //稍稍扩大
        bounds.Expand(5.0f);
        
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
        collider.enabled = flag;
        ///entityScript.enabled = flag;
        foreach (GameObject obj in childObjs)
        {
            obj.SetActive(flag);
        }
    }
}
