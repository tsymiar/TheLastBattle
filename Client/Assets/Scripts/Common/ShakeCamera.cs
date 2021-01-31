using UnityEngine;
using System.Collections;

public class ShakeCamera : MonoBehaviour
 {
    public enum ShakeOrient
    {
        horizontal,     //水平
        vertical,       //垂直
        forward,        //正朝向
    }

    //周期
    public float mPeriod = 2;

    //偏移周期
    public float mOffPeriod = 0;

    public ShakeOrient mShakeOrient = ShakeOrient.horizontal;
   
    //振动时间
    public float mShakeTime = 10.0f;

    //最大波峰
    public float mMaxWave = 5;               

    //最小波峰
    public float mMinWave  = 1;

    //总共经过时间
    private float mCurTime = 0;

    //是否shake状态
    public bool mIsShake = false;

    //初始位置
    public Vector3 mDefaultPos;
    
    //振动方向
    public Vector3 mShakeDir;    
    public Transform mCamerTrans;

    void Start()
    {
        //mCamerTrans = gameObject.transform;
    }


    //获取Transform
    public Transform GetTransform()
    {
        if (mCamerTrans == null)
        {
            mCamerTrans = gameObject.transform;
        }
        return mCamerTrans;
    }    

    //振屏
    public void ShakeScreen(ShakeOrient shakeOrient, float period, float shakeTime, float maxWave, float minWave, float offPeriod = 0)
    {
        //不在振动状态        
        if (!mIsShake)
        {
            //确保Transform有效
            if (GetTransform() == null)
                return;


            mShakeOrient = shakeOrient;                        
            mPeriod = period;
            mShakeTime = shakeTime;
            mMaxWave = maxWave;
            mMinWave = minWave;
            mOffPeriod = offPeriod;

            //保存默认位置
            mDefaultPos = transform.localPosition;

            //垂直方向 
            if (shakeOrient == ShakeOrient.vertical)
            {
                mShakeDir = new Vector3(0, 1, 0);
            }
            else if (shakeOrient == ShakeOrient.forward)
            {
                mShakeDir = mCamerTrans.forward;
            }
            else if (shakeOrient == ShakeOrient.horizontal)
            {
                Vector3 v1 = new Vector3(0, 1, 0);
                Vector3 v2 = mCamerTrans.forward;

                mShakeDir = Vector3.Cross(v1, v2);
                mShakeDir.Normalize();
            }
            
            mIsShake = true;
        }
    }

   
    public void Update()
    {      
        if (mIsShake)
        {

            float factor = mCurTime / mShakeTime;
            //总周期
            float totalPeriod = mPeriod * Mathf.PI;

            //当前时刻值
            float maxValue = mMaxWave - (mMaxWave - mMinWave) * factor;

            //当前弧度值
            float radValue = mOffPeriod * Mathf.PI + factor * totalPeriod;
            float value = maxValue * Mathf.Sin(radValue);

            //垂直振动，只固定y方向
            if (mShakeOrient == ShakeOrient.vertical)
                transform.localPosition = new Vector3(transform.localPosition.x, mDefaultPos.y, transform.localPosition.z) + mShakeDir * value;
            else
                transform.localPosition = mDefaultPos + mShakeDir * value;
            

            mCurTime += Time.deltaTime;          
            //结束振屏状态上
            if (mCurTime > mShakeTime)
            {
                mIsShake = false;
                mCurTime = 0;
            }
        }
    }


#if UNITY_EDITOR
    public void OnGUI()
    {
        if (GUILayout.Button("test"))
        {

            ShakeScreen(mShakeOrient, mPeriod, mShakeTime, mMaxWave, mMinWave, mOffPeriod);

            mIsShake = true;
            //保存默认位置
            mDefaultPos = transform.localPosition;

        }
    }
#endif
}