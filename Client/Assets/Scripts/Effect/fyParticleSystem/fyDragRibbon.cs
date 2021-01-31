using UnityEngine;
using System.Collections.Generic;


[ExecuteInEditMode]
//拖拽丝带效果
public class fyDragRibbon : fyRibbonTrail
{
    //前进速度
    public float mSpeed1 = 2.0f;
    //回退速度
    public float mSpeed2 = 2.0f;

    //最长距离
    public float mMaxLength = 10;
    
    //激活
    public bool mEnable = false;

    //强制回退
    public bool mForceBack = false;

    //朝向
    public Vector3 mDir = new Vector3(0, 0, 1);    

    //当前经过长度
    public float mCurLength = 0;

    //前进还是回退
    public bool mIsAdvance = true;
 
    //源目标对象
    Transform mSourceTM;

    //绑定节点
    Transform mBindTM;



    public fyDragRibbon() { }
    
    public override void ReCreate()
    {
        
        base.ReCreate();
        renderObj.name = "fyDragRibbon";
     
    
        //创建一个绑定节点
        mBindTM = renderObj.transform.FindChild("BindObj");
        if (mBindTM == null)
        {
            GameObject bindObj = new GameObject("BindObj");
            bindObj.transform.parent = renderObj.transform;
            mBindTM = bindObj.transform;        
        }
        mBindTM.LookAt(mBindTM.position + mDir);
    }



    //功能函数
    public void Link(Transform sourceObj)
    {
        if (sourceObj == null)
            return;

        //绑定源目标对象        
        mSourceTM = sourceObj;

        mEnable = true;
    }

    //强制回退
     public void ForeceBack()
    {
        if(mEnable)
            mForceBack = true;
    }

    public override void UpdateRibbon(float deltaTime)
    {
        ////未创建或者未连接
        if (!bCreated || !mEnable || mMesh == null)
            return;


        mMesh.bounds = new Bounds(Camera.main.transform.position, new Vector3(1000, 1000, 1000));


        //源目标位置
        Vector3 sourcePos = mSourceTM.position;

        //前进
        if (mIsAdvance)
            mCurLength += mSpeed1 * deltaTime;
        //后退
        else
            mCurLength -= mSpeed2 * deltaTime;

        //到达最大位置
        if (mForceBack || mCurLength >= mMaxLength)
        {
            mIsAdvance = false;
        }

        //是否回退结束
        if (mCurLength <= 0)
        {
            mCurLength = 0;
            mEnable = false;
            mIsAdvance = true;
            mForceBack = false;
            gameObject.SetActive(false);
            return;
        }

        //计算当前目标点
        Vector3 targetPos = sourcePos  + mDir * mCurLength;
        mBindTM.position = targetPos;

        float totalDist = 0;

        //遍历修改数据点               
        for (int i = 0; i < elemPosPool.Length / 2.0f; i++)
        {
            float vLength = i * segLength;
            vLength = vLength < mCurLength ? vLength : mCurLength;


            if (mCurLength >= mMaxLength)
                totalDist = mMaxLength;
            else
                totalDist = mCurLength;
            

              
            //最终中心位置
            Vector3 vPos = targetPos - mDir * vLength;

           
            //垂直法向
            Vector3 vDir = Vector3.Cross(mDir, mNormal);
            Vector3 p1 = vPos + vDir * segWidth / 2.0f;
            Vector3 p2 = vPos - vDir * segWidth / 2.0f;

            ////转为局部坐标
            //p1 -= sourcePos;
            //p2 -= sourcePos;

            //重置顶点数据
            elemPosPool[i * 2] = p1;           
            elemPosPool[i * 2 + 1] = p2;


            if (mMaterialType == MaterialAddressType.Repeat_Type)
            {
                float uv = vLength / mUVLength;

                elemUVPool[i * 2] = new Vector2(-uv, 1.0f);
                elemUVPool[i * 2 + 1] = new Vector2(-uv, 0.0f);
            }
            else
            {
                elemUVPool[i * 2] = new Vector2(1.0f - vLength / totalDist, 1.0f);
                elemUVPool[i * 2 + 1] = new Vector2(1.0f - vLength / totalDist, 0.0f);
            }
        }

        mMesh.vertices = elemPosPool;
        mMesh.uv = elemUVPool;          
    }



    public override void Reset()
    {
        base.Reset();
        mCurLength = 0;
        mEnable = false;
        mIsAdvance = true;
        mForceBack = false;                
    }

}
