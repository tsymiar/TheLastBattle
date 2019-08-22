using UnityEngine;
using System.Collections.Generic;

//击飞运动效果
public class BeatFlyMotion : MonoBehaviour
{
    //当前影响的buff的id
    public uint mBuffId = 0;
    //默认高度
    private float mDefaultHeigt;
    //向上初始速度
    private float mUpSpeed = 0;
    //向下加速度
    private float mUpAccerate = 0;
    //向下初始速度
    private float mDownSpeed = 0;
    //向下加速度
    private float mDownAccerate = 0;
    
    //滞空时间
    public float mStayTime;

    //当前高度
    public float mCurrentHeight;
    
    //上次速度
    private float mLastSpeed;
    //当前速度
    private float mCurrentSpeed;

    //激活运动
    private bool mEnable = false;
    //滞空持续时间
    private float mTotalStatyTime = 0;
    //运动方向
    private bool bDir = true;
    //击飞是否能被覆盖
    private int mCanBeRecover = 1;          //可以被覆盖    


    private Transform mTransform = null;
    private Animation mAnimation = null;

    void Start()
    {             
    }

    void OnEnable()
    {
        //重置数据      
        mTotalStatyTime = 0;
        mEnable = false;
        bDir = true;
        mCanBeRecover = 1;
        mBuffId = 0;
    }

    private Animation GetAnimation()
    {
        if(mAnimation == null)
            mAnimation = gameObject.GetComponent<Animation>();

        return mAnimation;
    }

    //击飞
    public void BeatFly(uint buffId, string action, float upSpeed = 5, float upAccerate = -5, float downSpeed = 5, float downAccerate = 15, float stayTime = 0, int canRecover = 1)
    {
        //已经是击飞状态
        if (mEnable)
        {
            //Debug.Log("recover state" + mCanBeRecover.ToString());

            if (mCanBeRecover == 1)
            {
                Reset();
            }
            else
                return;
        }

        mBuffId = buffId;

        //击飞是否被覆盖
        mCanBeRecover = canRecover;

        mDefaultHeigt = gameObject.transform.position.y;
        mCurrentHeight = mDefaultHeigt;
        mTransform = gameObject.transform;
      

        if (downAccerate < 0)
        {
            Debug.LogError("downAccerate do not set negative in BeatFly");
        }
        if (stayTime == 0)
            stayTime = 0.0001f;

                
        mUpSpeed = upSpeed;
        mUpAccerate = upAccerate;
        mDownSpeed = downSpeed;
        mDownAccerate = downAccerate;
        
        mStayTime = stayTime;       
        mEnable = true;


        //初始速度
        mLastSpeed = mUpSpeed;
        mCurrentSpeed = mUpSpeed;


        if (action != "0")
            GetAnimation().Play(action);
    }

    //立即下落 
    public void FallNow()
    {
        mStayTime = 0.0001f;
    }


    void Update()
    {
        //击飞运动更新
        if (mEnable)
        {          
            //进入滞空状态
            if (mCurrentSpeed <= 0 && mTotalStatyTime < mStayTime)
            {
                mLastSpeed = -mDownSpeed;
                mCurrentSpeed = -mDownSpeed;

                mTotalStatyTime += Time.deltaTime;
                bDir = false;
                return;
            }

            //当前速度v = v0 + at

            //向上运动
            if (bDir)
            {
                mCurrentSpeed = mCurrentSpeed  + mUpAccerate * Time.deltaTime;
            }
            //向下运动
            else
            {
                mCurrentSpeed = mCurrentSpeed - mDownAccerate * Time.deltaTime;                 
            }
                       
            //运动距离s
            float dist = (mLastSpeed + mCurrentSpeed) * Time.deltaTime * 0.5f;

            //当前高度
            mCurrentHeight += dist;

            //保存前一次速度
            mLastSpeed = mCurrentSpeed;



            mTransform.position = new Vector3(mTransform.position.x, mCurrentHeight, mTransform.position.z);
            //回到原来位置或者反向速度为0
            if (mCurrentHeight < mDefaultHeigt)
            {
                Reset();
            }
          
        }
    }

    void Reset() 
    {
        mCurrentHeight = mDefaultHeigt;

        //重置数据      
        mTotalStatyTime = 0;
        mEnable = false;
        bDir = true;
        mCanBeRecover = 1;
        mBuffId = 0;

        mTransform.localPosition = new Vector3(mTransform.localPosition.x, mDefaultHeigt, mTransform.localPosition.z);
    }
}
