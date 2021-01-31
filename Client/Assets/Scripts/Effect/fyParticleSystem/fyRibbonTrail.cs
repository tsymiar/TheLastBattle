using UnityEngine;
using System.Collections.Generic;

[System.Serializable]
public class ColorKey
{
    public ColorKey()
    {
        time = 1.0f;
        color = new Color(1, 1, 1, 1);
    }
    
    public Color color;
    public float time;

    public ColorKey(Color color, float time)
    {
        this.color = color;
        this.time = time;
    }
}



public class fyRibbonTrail : MonoBehaviour 
{    
    //最大Ribbon段数
    public int maxElement = 10;

    //Ribbon每段的宽度
    public float segWidth = 2.0f;

    //Ribbon每段的长度
    public float segLength = 1.0f;

    //源对象 
    public Transform source = null;

    //绘制对象
    public GameObject renderObj = null;

    //UV重复长度
    public float mUVLength = 1.0f;

    //Mesh
    protected Mesh mMesh = null;
    //MeshRender
    protected MeshRenderer mMeshRender = null;

    //纹理旋转矩阵
    protected Matrix4x4 mMatrix = Matrix4x4.identity;

    //偏移位置
    public Vector2 mOffset = new Vector2(0, 0);
    //旋转位置
    public float mOffRot = 0;
    //平移
    public Vector2 mTranslate = new Vector2(0, 0);
    //缩放
    public Vector2 mScale = new Vector2(1, 1);
    //旋转
    public float mRotate = 0.0f;


    //设置周期时间
    public float mColorPeriodTime = 0.0f;
    public float mColorCurTime = 0;

    //颜色渐变
    public ColorKey[] colorKey = new ColorKey[5];

    //保存对应的Shader
    public Shader mShader = null;

    //保存对应的Texture2D
    public Texture2D mTexture = null;

    //朝向类型
    public enum OrientType
    {
        World_Type,
        Billboard_Type
    }

    //纹理寻址类型
    public enum MaterialAddressType
    {
        Common_Type,                //普通纹理寻址
        Sequence_Type,              //序列帧寻址
        Repeat_Type,                //重复寻址
    }

    //Blend类型
    public enum BlendType
    {
        Replace_Type,
        Alpha_Type,
        Additive_Type,
        Distort_Additive,
    }

    public OrientType mOrientType = OrientType.World_Type;
    public MaterialAddressType mMaterialType = MaterialAddressType.Common_Type;
    public BlendType mBlendType = BlendType.Additive_Type;


    public Vector3 mNormal = new Vector3(0, 1, 0);


  
    //Ribbon Segment列表    
    protected Vector3[] elemPosPool;
    protected Vector2[] elemUVPool;
    protected Color[] elemColorPool;

    //是否创建完成
    protected bool bCreated = false;


	void Start () 
    {
        renderObj = this.gameObject;
        source = renderObj.transform;

        //运行状态，已经有Mesh数据，不需要重新创建        
        ReCreate();

        //初始变换矩阵
        InitMatrix();    
	}

    
    public void InitMatrix()
    {
        //初始偏移
        Matrix4x4 xlate = Matrix4x4.identity;
        if (mOffset.x != 0 || mOffset.y != 0)
        {
            xlate.m03 = mOffset.x;
            xlate.m13 = mOffset.y;
        }
        //初始旋转
        Matrix4x4 rotMatrix = Matrix4x4.identity;
        if (mOffRot != 0)
        {
            float theta = mOffRot * Mathf.Deg2Rad;
            float cosTheta = Mathf.Cos(theta);
            float sinTheta = Mathf.Sin(theta);
            rotMatrix.m00 = cosTheta;
            rotMatrix.m01 = -sinTheta;
            rotMatrix.m10 = sinTheta;
            rotMatrix.m11 = cosTheta;
            // Offset center of rotation to center of texture
            rotMatrix.m03 = 0.5f + ((-0.5f * cosTheta) - (-0.5f * sinTheta));
            rotMatrix.m13 = 0.5f + ((-0.5f * sinTheta) + (-0.5f * cosTheta));
        }
        mMatrix = xlate * rotMatrix;
      
        //设置旋转效果        
        if (mMeshRender != null)
        {            
            //编辑模式
            if (!Application.isPlaying )
            {
                if (mMeshRender.sharedMaterial != null)
                {                    
                    //设置旋转效果                    
                    mMeshRender.sharedMaterial.SetMatrix("_TransMatrix", mMatrix);
                }           
            }
            //运行模式
            else
            {                
                if (mMeshRender.material != null)
                {             
                    mMeshRender.material.SetMatrix("_TransMatrix", mMatrix);                                        
                }
            }                         
        }
    }

	
	// Update is called once per frame
	void Update () 
    {
        //更新颜色
        UpdateColor(Time.deltaTime);

        //更新Ribbon
        UpdateRibbon(Time.deltaTime);

        //更新变换矩阵
        UpdateMatrix();
	}

    //创建Ribbon
    virtual public void ReCreate()
    {       
        //检测源物体
        if (maxElement < 1)
        {
            Debug.LogError("the ribbon max element must be more than 0!");
            return;
        }
    
      
        renderObj.name = "fyRibbon";

        MeshFilter mf = gameObject.GetComponent<MeshFilter>();
        if (mf == null)
        {
            mf = renderObj.AddComponent<MeshFilter>();            
            mf.hideFlags = HideFlags.NotEditable;
        }

        MeshRenderer mr = gameObject.GetComponent<MeshRenderer>();
        if (mr == null)
        {
            mr = renderObj.AddComponent<MeshRenderer>();
            mr.castShadows = false;
            mr.receiveShadows = false;
            //设置不可编辑状态
            mr.hideFlags = HideFlags.NotEditable;
          
        }
   
        mMeshRender = mr;
        //创建mesh
        if (mf.sharedMesh == null)
        {
            mf.sharedMesh = new Mesh();          
        }
        mMesh = mf.sharedMesh;

        //组件不可见
        //mf.hideFlags = HideFlags.HideInInspector;

        //创建RibbonMesh
        CreateRibbonMesh(mf.sharedMesh);



      
        
        //编辑状态
        if (!Application.isPlaying)
        {
            //第一次创建
            if (mMeshRender.sharedMaterial == null)
            {
                Texture2D defaultTex = Resources.Load("effect/test/flare") as Texture2D;

                //设置默认shader
                if (mShader == null)
                {
                    //修改为默认材质以及贴图
                    Shader shader = Shader.Find("fyShader/Additive");
                    mShader = shader;
                    mMeshRender.sharedMaterial = new Material(shader);
                }
               
                //设置默认texture
                if (mTexture == null)
                {
                    mTexture = defaultTex;
                    mMeshRender.sharedMaterial.SetTexture("_MainTex", defaultTex);
                }                                                    
            }

            if (mShader != null)
            {
                mMeshRender.sharedMaterial = new Material(mShader);
                if (mTexture != null)
                    mMeshRender.sharedMaterial.SetTexture("_MainTex", mTexture);                
            }       
        }
        //运行状态
        else
        {
            if (mShader != null)
            {
                mMeshRender.material = new Material(mShader);

                if (mTexture != null)
                    mMeshRender.material.SetTexture("_MainTex", mTexture);
            }


            
        }
                        
        bCreated = true;
    }
    
    //改变贴图
    public void ChangeTexture(Texture2D tex)
    {
        //保存贴图
        mTexture = tex;

        if (mMeshRender != null && mMeshRender.sharedMaterial != null)
        {
            mMeshRender.sharedMaterial.SetTexture("_MainTex", tex);
            
        }                 
    }


    //创建RibbonMesh 
    void CreateRibbonMesh(Mesh mesh)    
    {
        //局部坐标系 法线方向
        Vector3 normal = new Vector3(0, 1, 0);
        Vector3 sourcePos = new Vector3(0, 0, 0);
        Debug.Log("source pos" + sourcePos.ToString());

        //初始RibbonElement Pool
        elemPosPool = new Vector3[(maxElement + 1) * 2];
        elemUVPool = new Vector2[(maxElement + 1) * 2];
        elemColorPool = new Color[(maxElement + 1) * 2];

        //初始方向 
        //Vector3 dir = targetPos - sourcePos;
        Vector3 dir = new Vector3(0, 0, 1);
        dir.Normalize();

                        
        mesh.Clear();
        //默认初始长度,编辑时候需要处理
        float length = 0.0001f;
        //if (!Application.isPlaying)
        {
            length = maxElement * segLength;
        }
    

        float deltaLength = length / maxElement;

        //默认结束位置
        Vector3 endPos = sourcePos + dir*length;

        //数据点
        for (int i = 0; i < elemPosPool.Length / 2.0f; i++)
        {
            //距离end点距离
            float vLength = i * deltaLength;
            Vector3 vPos = endPos - dir * vLength;

            //垂直法向
            Vector3 vDir = Vector3.Cross(dir, normal);
            Vector3 p1 = vPos + vDir * segWidth / 2.0f;
            Vector3 p2 = vPos - vDir * segWidth / 2.0f;

            //保存顶点数据
            elemPosPool[i * 2] = p1;       
            elemColorPool[i * 2] = new Color(1, 1, 1, 1);

            elemPosPool[i * 2 + 1] = p2;
            elemColorPool[i * 2 + 1] = new Color(1, 1, 1, 1);


            if (mMaterialType == MaterialAddressType.Repeat_Type)
            {
                float uv = vLength / mUVLength;

                elemUVPool[i * 2] = new Vector2(-uv, 1.0f);
                elemUVPool[i * 2 + 1] = new Vector2(-uv, 0.0f);
            }
            else
            {
                //根据UV类型       
                elemUVPool[i * 2] = new Vector2(1.0f - vLength / length, 1.0f);
                elemUVPool[i * 2 + 1] = new Vector2(1.0f - vLength / length, 0.0f);
            }                     
        }

        //索引数据
        int[] index = new int[maxElement*2*3];
        for (int i = 0; i < maxElement; i++)
        {
            int index0 = i*2;
            int index1 = i*2+1;
            int index2 = (i+1)*2;
            int index3 = (i+1)*2+1;


            index[i*6+0] = index0;
            index[i*6+1] = index1;
            index[i*6+2] = index2;

            index[i*6+3] = index3;
            index[i*6+4] = index2;
            index[i*6+5] = index1;
        }

        mesh.vertices = elemPosPool;
        mesh.uv = elemUVPool;
        mesh.colors = elemColorPool;
        mesh.triangles = index;


        mesh.RecalculateNormals();
        mesh.RecalculateBounds();
       
        mesh.Optimize();       
    }

    //更新变换矩阵
    public virtual void UpdateMatrix()
    {        
        if (mMeshRender != null)
        {
            bool updateMatrix = false;
            //缩放
            if (mScale.x != 1 || mScale.y != 1)
            {
                // Offset to center of texture
                mMatrix.m00 /= mScale.x;
                mMatrix.m11 /= mScale.y;
                // Skip matrix concat since first matrix update
                mMatrix.m03 = (-0.5f * mMatrix.m00) + 0.5f;
                mMatrix.m13 = (-0.5f * mMatrix.m11) + 0.5f;

                updateMatrix = true;
            }
            //平移
            if (mTranslate.x != 0 || mTranslate.y != 0)
            {
                Matrix4x4 xlate = Matrix4x4.identity;
                xlate.m03 = mTranslate.x;
                xlate.m13 = mTranslate.y;
                mMatrix = xlate * mMatrix;

                updateMatrix = true;
            }
            //旋转
            if (mRotate != 0)
            {
                Matrix4x4 rotMatrix = Matrix4x4.identity;
                float theta = mRotate * Mathf.Deg2Rad;
                float cosTheta = Mathf.Cos(theta);
                float sinTheta = Mathf.Sin(theta);
                rotMatrix.m00 = cosTheta;
                rotMatrix.m01 = -sinTheta;
                rotMatrix.m10 = sinTheta;
                rotMatrix.m11 = cosTheta;
                // Offset center of rotation to center of texture
                rotMatrix.m03 = 0.5f + ((-0.5f * cosTheta) - (-0.5f * sinTheta));
                rotMatrix.m13 = 0.5f + ((-0.5f * sinTheta) + (-0.5f * cosTheta));
                mMatrix = rotMatrix * mMatrix;

                updateMatrix = true;
            }

            //设置旋转效果
            if (updateMatrix)
            {
                //编辑模式
                if (!Application.isPlaying)
                {
                    if (mMeshRender.sharedMaterial != null)
                    {
                        //设置旋转效果                    
                        mMeshRender.sharedMaterial.SetMatrix("_TransMatrix", mMatrix);
                    }
                }
                //运行模式
                else
                {
                    if (mMeshRender.material != null)
                    {
                        mMeshRender.material.SetMatrix("_TransMatrix", mMatrix);
                    }
                }                                              
            }
        }       
    }



    //根据当前时间计算颜色
    public virtual Color GetGradientColor()
    {
        Color gcolor = new Color();

        //计算比例值
        float factor = mColorCurTime / mColorPeriodTime;
        //计算 color值      
        int colorLength = colorKey.Length;
        for (int i = 0; i < colorLength - 1; i++)
        {
            ColorKey key1 = colorKey[i % colorLength];
            ColorKey key2 = colorKey[(i + 1) % colorLength];

            //计算距离
            float dist = key2.time - key1.time;

            if (factor >= key1.time && factor <= key2.time)
            {
                //计算当前alpha值
                float ff = (factor - key1.time) / dist;
                gcolor = key1.color * (1 - ff) + key2.color * ff;
                break;
            }
        }

        return gcolor;
    }

    //更新颜色
    public virtual void UpdateColor(float deltaTime)
    {
        if (mColorPeriodTime == 0)
            return;

        //计算颜色值,颜色有效   
        Color gColor = new Color();

        mColorCurTime += deltaTime;
        //计算当前颜色时间
        if (mColorCurTime > mColorPeriodTime)
        {
            mColorCurTime = 0;
        }

        gColor = GetGradientColor();

        //Debug.Log("color" + gColor.ToString());

        //遍历修改数据点               
        for (int i = 0; i < elemPosPool.Length / 2.0f; i++)
        {
            elemColorPool[i * 2] = gColor;
            elemColorPool[i * 2 + 1] = gColor;
        }

        mMesh.colors = elemColorPool;        
    }

    //更新Ribbon 
    public virtual void UpdateRibbon(float deltaTime)
    {         
    }

    //重置
    public virtual void Reset()
    {
        if (mMesh == null)
            return;

        //重置位置       
        Vector3 sourcePos = source.position;

        //初始RibbonElement Pool
        elemPosPool = new Vector3[(maxElement + 1) * 2];
        elemUVPool = new Vector2[(maxElement + 1) * 2];

        //初始方向         
        Vector3 dir = new Vector3(0, 0, 1);
        dir.Normalize();
        
        //默认初始长度
        float length = maxElement * segLength;

        float deltaLength = length / maxElement;

        //默认结束位置
        Vector3 endPos = sourcePos + dir * length;

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

            Vector3 vPos = endPos - dir * vLength;

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
            elemColorPool[i * 2] = new Color(1, 1, 1, 1);

            elemPosPool[i * 2 + 1] = p2;
            elemUVPool[i * 2 + 1] = new Vector2(1.0f - vLength / realLength, 0.0f);
            elemColorPool[i * 2] = new Color(1, 1, 1, 1);
        }

        mMesh.vertices = elemPosPool;
        mMesh.uv = elemUVPool;

        mMesh.RecalculateNormals();
        mMesh.RecalculateBounds();
        mMesh.Optimize();   

        //刷新矩阵数据
        mMatrix = Matrix4x4.identity;
        InitMatrix();


        mColorCurTime = 0;

        Debug.Log("reset");
    }   



}
