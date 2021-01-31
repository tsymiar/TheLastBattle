using UnityEngine;
using System.Collections.Generic;

//淡入淡出效果
public class FadeInOut : MonoBehaviour
{
    public enum FADE_TYPE
    {
        FADE_IN,            //淡入状态
        FADE_OUT,           //淡出状态
        FADE_NOT,           //非淡入淡出
    }
    
    //角色当前透明度
    public float mCurFadeValue = 1.0f;
    //角色需要的透明度
    private float mFadeValue = -1.0f;
    //过渡时间
    private float mFadeTime = -1.0f;
    //单位淡出值 
    private float mDeltaFade = 0.0f;

    
    //淡入淡出类型
    public FADE_TYPE mFadeType = FADE_TYPE.FADE_NOT;

    //角色默认材质
    private Material mDefaultMat;
    //角色透明材质
    private Material mTransparentMat;
    //保存角色的SkinnedMeshRender
    private List<SkinnedMeshRenderer> mMeshRenders = new List<SkinnedMeshRenderer>();
    //保存角色的ParticleSystem
    private ParticleSystem[] mParticleSystems;
    //保存每个ParticleSystem的alpha值
    private float[] mPsAlphas;
    //保存每个ParticleSystem的alpha值
    private float[] mPsCurAlphas;

    
    void Start()
    {        
        //遍历获取第一层SkinnedMeshRender
        for (int i = 0; i < gameObject.transform.childCount; i++)
        {
            Transform ts = gameObject.transform.GetChild(i);
            GameObject psObj = ts.gameObject;
            if (psObj)
            {
                SkinnedMeshRenderer render = psObj.GetComponent<SkinnedMeshRenderer>();
                if (render != null)
                    mMeshRenders.Add(render);
            }
        }

        //获取玩家身上所有特效
        mParticleSystems = gameObject.GetComponentsInChildren<ParticleSystem>();
        if (mParticleSystems.Length != 0)
        {
            mPsAlphas = new float[mParticleSystems.Length];
            mPsCurAlphas = new float[mParticleSystems.Length];
        }


        for (int i = 0; i < mParticleSystems.Length; i++)
        {
            mPsAlphas[i] = mParticleSystems[i].startColor.a;
            mPsCurAlphas[i] = mParticleSystems[i].startColor.a;
        }

        if (mMeshRenders.Count == 0)
            Debug.LogError("the SkinnedMeshRender can not be empty in fadeInOut!");


        //获取默认材质
        mDefaultMat = mMeshRenders[0].sharedMaterial;
        
        //创建角色透明材质
        mTransparentMat = new Material(Shader.Find("Custom/Alpha_Diffuse"));
        //修改材质
        mTransparentMat.SetTexture(0,mDefaultMat.GetTexture(0));
    }

    //修改模型材质
    void ChangeMaterial(Material mat)
    {
        foreach (SkinnedMeshRenderer render in mMeshRenders)
        {
            render.material = mat;
        }
    }

    //设置角色模型可见性
    void SetMeshVisible(bool flag)
    {
        foreach (SkinnedMeshRenderer render in mMeshRenders)
        {
            render.enabled = flag;
        }
    }


    //淡入效果
    public void FadeIn(float deltaTime, float alphaValue)
    {
        if (alphaValue > mCurFadeValue)
        {
            mFadeType = FADE_TYPE.FADE_IN;
            mFadeValue = alphaValue;
            mFadeTime = deltaTime;
            mDeltaFade = (alphaValue - mCurFadeValue) / deltaTime;
            ChangeMaterial(mTransparentMat);

            SetMeshVisible(true);     
        }        
    }

    //淡出效果
    public void FadeOut(float deltaTime, float alphaValue)
    {
        if (alphaValue < mCurFadeValue)
        {
            mFadeType = FADE_TYPE.FADE_OUT;
            mFadeValue = alphaValue;
            mFadeTime = deltaTime;
            mDeltaFade = (alphaValue - mCurFadeValue) / deltaTime;
            ChangeMaterial(mTransparentMat);
        }
    }

    void Update()
    {               
        if(mFadeType == FADE_TYPE.FADE_NOT)
            return;

        float deltaFade = mDeltaFade * Time.deltaTime;

        //处理每个特效alpha
        for (int i = 0; i < mPsCurAlphas.Length; i++)
        {
            float psDelta = deltaFade * mPsAlphas[i];

            mPsCurAlphas[i] += psDelta;

            if (mPsCurAlphas[i] <= 0.0f)
                mPsCurAlphas[i] = 0.0f;
            else if(mPsCurAlphas[i] >= 1.0f)
                mPsCurAlphas[i] = 1.0f;

            Color colour = mParticleSystems[i].startColor;

            mParticleSystems[i].startColor = new Color(colour.r, colour.g, colour.b, mPsCurAlphas[i]);
        }

        //淡入效果
        if (mFadeType == FADE_TYPE.FADE_IN)
        {
            mCurFadeValue = mCurFadeValue + deltaFade;
            //结束
            if (mCurFadeValue >= mFadeValue)
            {
                mCurFadeValue = mFadeValue;
                mFadeType = FADE_TYPE.FADE_NOT;
                ChangeMaterial(mDefaultMat);
            }
            mTransparentMat.SetColor("_Color", new Color(1, 1, 1, mCurFadeValue));
        }
        //淡出效果
        else if (mFadeType == FADE_TYPE.FADE_OUT)
        {
            mCurFadeValue = mCurFadeValue + deltaFade;
            //结束
            if (mCurFadeValue <= mFadeValue)
            {
                mCurFadeValue = mFadeValue;
                mFadeType = FADE_TYPE.FADE_NOT;

               // SetMeshVisible(false);
            }

            mTransparentMat.SetColor("_Color", new Color(1, 1, 1, mCurFadeValue));
        }
         
    }   

    
}
