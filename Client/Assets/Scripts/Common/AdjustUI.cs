using UnityEngine;
using System.Collections;
using System.Collections.Generic;

//using UnityEditor;
//UI调整
[ExecuteInEditMode]
public class AdjustUI : MonoBehaviour
{
    //保存原来默认缩放值
    public Vector3 defaultScale = new Vector3(1,1,1);
    //保存原来的位移值
    public Vector3 defaultPos = new Vector3(0, 0, 0);


    [System.Serializable]
    public class ResolutionInfo
    {      
        public ResolutionInfo(string resolution, float scale, Vector3 pos)
        {
            this.resolution = resolution;
            this.scale = scale;
            this.pos = pos;
        }

        public string resolution;
        public float scale;
        public Vector3 pos;
    }

    public List<ResolutionInfo> resolutionMap = new List<ResolutionInfo>();

    //VirtualStickUI脚本
    private VirtualStickUI mVirtualStickUI = null;

    public AdjustUI()
    {
       

//        //编辑器模式
//#if UNITY_EDITOR
//        if (!Application.isPlaying)
//        {
//            //resolutionMap.Add(new ResolutionInfo("1024x768", 1.0f));
//            resolutionMap.Add(new ResolutionInfo("854x480", 1.0f));            
//        }
//#endif
    }

    void Awake()
    {
        mVirtualStickUI = this.gameObject.GetComponent<VirtualStickUI>();
    }

    // Use this for initialization
    void Start()
    {
        //编辑器模式
#if UNITY_EDITOR
        if (!Application.isPlaying)
        {
            defaultScale = transform.localScale;
            defaultPos = transform.localPosition;           
        }
#endif
         
//程序运行状态
#if UNITY_ANDROID || UNITY_STANDALONE_WIN || UNITY_IPHONE      
        if (Application.isPlaying)
        {
            transform.localScale = defaultScale;
            transform.localPosition = defaultPos;

            //设备分辨率            
            int width = Screen.currentResolution.width;
            int height = Screen.currentResolution.height;

            string resName = width.ToString() + "x" + height.ToString();
            //遍历查询分辨率
            foreach (ResolutionInfo resolutionInfo in resolutionMap)
            {
                //有匹配当前分辨率
                if (resolutionInfo.resolution == resName)
                {
                    float scale = resolutionInfo.scale;
                    Vector3 pos = resolutionInfo.pos;
                    //修改UI大小
                    transform.localScale = new Vector3(scale,scale,scale);
                    transform.localPosition =  pos;

                    //如果有VirtualStickUI
                    if (mVirtualStickUI != null)
                        mVirtualStickUI.adjustRadius *= scale;
                    break;
                }
            }                     
        }
#endif 
    }

   
    // Update is called once per frame
    public void Update()
    {        
        //编辑器模式
#if UNITY_EDITOR     
        if (!Application.isPlaying)
        {
            //defaultScale = new Vector3(1, 1, 1);
            //defaultPos = new Vector3(0, 0, 0);

            //Debug.Log("aaa");
            //设备分辨率
            Vector2 screen = NGUITools.screenSize;
            int width = (int)screen.x;
            int height = (int)screen.y;

            ////int width = 854;
            ////int height = 480;
            string resName = width.ToString() + "x" + height.ToString();

          
            
            //遍历查询分辨率
            foreach (ResolutionInfo resolutionInfo in resolutionMap)
            {
                //有匹配当前分辨率
                if (resolutionInfo.resolution == resName)
                {
                    float scale = resolutionInfo.scale;
                    Vector3 pos = resolutionInfo.pos;
                    //修改UI大小
                    transform.localScale = new Vector3(scale,scale,scale);
                    transform.localPosition = pos;
                    break;
                }
                else
                {
                    //使用默认缩放值
                    transform.localScale = defaultScale;
                    transform.localPosition = defaultPos;                                                               
                }
            }  
        }     
#endif
    }


	// 强制更新
	public void ForceUpdate()
	{
		//defaultScale = new Vector3(1, 1, 1);
		//defaultPos = new Vector3(0, 0, 0);
		
		//Debug.Log("aaa");
		//设备分辨率
		Vector2 screen = NGUITools.screenSize;
		int width = (int)screen.x;
		int height = (int)screen.y;
		
		////int width = 854;
		////int height = 480;
		string resName = width.ToString() + "x" + height.ToString();
		
		
		
		//遍历查询分辨率
		foreach (ResolutionInfo resolutionInfo in resolutionMap)
		{
			//有匹配当前分辨率
			if (resolutionInfo.resolution == resName)
			{
				float scale = resolutionInfo.scale;
				Vector3 pos = resolutionInfo.pos;
				//修改UI大小
				transform.localScale = new Vector3(scale,scale,scale);
				transform.localPosition = pos;
				break;
			}
			else
			{
				//使用默认缩放值
				transform.localScale = defaultScale;
				transform.localPosition = defaultPos;                                                               
			}
		}  
	}
          
}


