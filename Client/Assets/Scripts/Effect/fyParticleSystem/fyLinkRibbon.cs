using UnityEngine;
using System.Collections.Generic;


[ExecuteInEditMode]
public class fyLinkRibbon : fyRibbonTrail
{
    //运动速度
    public float mSpeed = 1.0f;

    //目标对象
    public Transform target = null;

    //是否连接
    private bool bLink = false;

    //相机的Transform
    private Transform mCamerTrans;
    
    public fyLinkRibbon(){}

    //获取相机Transform
    public Transform GetCameraTrans()
    {
        if (mCamerTrans == null)
        {
            GameObject camera = GameObject.Find("Main Camera");
            if (camera != null)
            {
                mCamerTrans = camera.transform;

            }
       }

       return mCamerTrans; 
    }

    public override void ReCreate()
    {
        base.ReCreate();
        renderObj.name = "fyLinkRibbon";
    }


    //功能函数
    public void Link(Transform targetObj)
    {
        if (targetObj == null)
            return;

        //绑定目标物体
        target = targetObj;

        //距离过近，没法连接
        Vector3 sourcePos = source.position;
        Vector3 targetPos = target.position;

        float dist = Vector3.Distance(sourcePos, targetPos);
        //距离过近，返回
        if (dist > 0.00001f)
            bLink = true;

    }

    public override void UpdateRibbon(float deltaTime)
    {
        //未创建或者未连接
        if (!bCreated || !bLink || mMesh == null)
            return;
             
        Vector3 sourcePos = source.position;
        Vector3 targetPos = target.position;
        float length = Vector3.Distance(sourcePos, targetPos);

        Vector3 dir = targetPos - sourcePos;
        dir.Normalize();


          Vector3 normal;
        //世界坐标朝向
        if (mOrientType == OrientType.World_Type)
        {
            mNormal.Normalize();
            normal = mNormal;
        }
        //BillBoard朝向
        else if (mOrientType == OrientType.Billboard_Type)
        {
            Transform trans = GetCameraTrans();
            Vector3 forwad = trans.forward;
            Vector3 v1 = Vector3.Cross(forwad, new Vector3(0, 1, 0));
            normal = Vector3.Cross(v1, dir);
            normal.Normalize();
        }


     
        //初始RibbonElement Pool
        elemPosPool = new Vector3[(maxElement + 1) * 2];
        elemUVPool = new Vector2[(maxElement + 1) * 2];

        float totalElemLength = maxElement * segLength;

        //真实最大长度
        float realLength = totalElemLength > length ? length : totalElemLength;
      
        //遍历修改数据点               
        for (int i = 0; i < elemPosPool.Length / 2.0f; i++)
        {
            float vLength;

            if (i * segLength > length)            
                vLength = length;            
            else
                vLength = i * segLength;
            
            Vector3 vPos = targetPos - dir * vLength;

            //垂直法向
            Vector3 vDir = Vector3.Cross(dir, mNormal);
            Vector3 p1 = vPos + vDir * segWidth / 2.0f;
            Vector3 p2 = vPos - vDir * segWidth / 2.0f;
            
            //转为局部坐标
            p1 -= sourcePos;
            p2 -= sourcePos;

            //重置顶点数据
            elemPosPool[i * 2] = p1;
            elemUVPool[i * 2] = new Vector2(1.0f - vLength / realLength, 1.0f);

           
            elemPosPool[i * 2 + 1] = p2;
            elemUVPool[i * 2 + 1] = new Vector2(1.0f - vLength / realLength, 0.0f);
 
        }


        
        mMesh.vertices = elemPosPool;
        mMesh.uv = elemUVPool;
        mMesh.colors = elemColorPool;
  
        //mMesh.RecalculateNormals();
        //mMesh.RecalculateBounds();
        //mMesh.Optimize();                   
    }

    

    public override void Reset()
    {
        base.Reset();
        bLink = false;
       
    }
   
}
