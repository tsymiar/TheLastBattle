using System;
//using System.Web;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using UnityEngine;

namespace BlGame.Resource
{
    //版本信息
    class VersionFile
    {
        //版本号
        public string version;
        //是否强制更新
        public bool forceUpdate;
        //版本类型 整包(package)还是资源(resource)
        public string type;
        //大小
        public int size;
        //日期
        public string date;

        //更新地址，用于不支持更新机制的渠道，例如:中手游
        public string updateAddress;
    }


    //版本资源信息    
    class VersionResourceFile
    {
        public string mNumber;
        public bool mBig;
        public Dictionary<string, string> mPathMD5;
        public Dictionary<string, int> mSize;

        public VersionResourceFile()
        {
            mPathMD5 = new Dictionary<string, string>();
            mSize = new Dictionary<string, int>();
        }
    }


    //SDK渠道信息
    class SDKChannelInfo
    {
        //渠道名称
        public string channelName;
        //更新版本地址
        public string updateAddress;
    }


    public class ResourceUpdateManager : UnitySingleton<ResourceUpdateManager>
    {
        //当前版本Version        
        private string mCurVersion;
        //当前版本Identifier
        private string mCurIdentifier;
        //CDN服务器版本信息
        private VersionFile mVersionFile = new VersionFile();
        private VersionResourceFile mLocalPathMD5;
        private VersionResourceFile mServerPathMD5;
        private List<string> mUpdateAssets;
        private List<int> mUpdateAssetsSize;
        private int mAllNeedDownLoadSize;
        private int mCurrentDownLoadSize;
        private int mAllNeedDownLoadBytes;
        private int mCurrentDownLoadBytes;
        private bool mComplete;

        private string mCurrentDownLoadAsset;
        private int mCurrentDownLoadAssetFailedTimes;
        private int mMaxRepeatDownLoadTimes;
        private bool mForceTermination;

        private bool mNetworkConnections;
        private bool mNeedCheckNetWorkConnections;
        private float mCheckNetWorkConnectionsMaxTimes;

        private Dictionary<string, SDKChannelInfo> mSdkChannelInfos = new Dictionary<string, SDKChannelInfo>();



        /***********UI相关**********/

        //确定
        private ButtonOnPress mConfirmButtonPress;
        //取消
        private ButtonOnPress mCancelmButtonPress;
        //更新提示ui
        private GameObject mUpdateTipObj;
        //提示信息Lable2

        private GameObject mProcessBar;
        private GameObject mProcessBar_Lable2;
        private GameObject mProcessBar_BG2;

        void Awake()
        {
            base.Awake();

            ////初始更新提示UI
            //mUpdateTipObj = GameObject.Find("TipFrame");
            //if (mUpdateTipObj != null)
            //{

            //    mUpdateTipObj.SetActive(false);

            //    GameObject confirmObj = mUpdateTipObj.transform.FindChild("Confirm").gameObject;
            //    GameObject cancelObj = mUpdateTipObj.transform.FindChild("Cancel").gameObject;

            //    mConfirmButtonPress = confirmObj.GetComponent<ButtonOnPress>();
            //    mCancelmButtonPress = cancelObj.GetComponent<ButtonOnPress>();

            //    //添加listener    
            //    mConfirmButtonPress.AddListener(ConfirmUpdate);
            //    mCancelmButtonPress.AddListener(CancelUpdate);
            //}

            ////进度条
            //mProcessBar = GameObject.Find("ProcessBar");
            //if (mProcessBar != null)
            //{
            //    mProcessBar_BG2 = mProcessBar.transform.FindChild("BG2").gameObject;
            //    mProcessBar_Lable2 = mProcessBar.transform.FindChild("Label2").gameObject;
            //}


            //创建SDK prefab
            CreateSDKPrefab();
           
        }


        void Start()
        { 

#if UNITY_STANDALONE_WIN || UNITY_EDITOR || SKIP_SDK
            OnUpdateComplete();
#endif
        }


        public int Prograss
        {
            get
            {
                if (mComplete)
                    return 100;
                return 0 == mAllNeedDownLoadSize ? 0 : (int)(((float)mCurrentDownLoadSize / mAllNeedDownLoadSize) * 100);
            }
        }

        public int AllNeedDownLoadBytes
        {
            get
            {
                return mAllNeedDownLoadBytes;
            }
        }

        public int CurrentDownLoadBytes
        {
            get
            {
                return CurrentDownLoadBytes;
            }
        }

        public bool Complete
        {
            get
            {
                return mComplete;
            }
        }

        public bool ForceTermination
        {
            get
            {
                return mForceTermination;
            }
        }

        public bool NetworkConnections
        {
            get
            {
                return mNetworkConnections;
            }
        }

        bool flag = true;

        public void Update()
        {

#if UNITY_STANDALONE_WIN || UNITY_EDITOR || SKIP_SDK
#else
            //IOS和Andriod
      	    if(SdkConector.isInitSuccess && flag)
		    {			    
			    //UpdateSDKFinished();            
                OnUpdateComplete();
                flag = false;
			}
#endif

            if (mForceTermination || !mNetworkConnections)
                StopAllCoroutines();
        }

        //更新SDK结束
        public void UpdateSDKFinished()
        {
            //Andriod和IOS平台，执行更新操作
            updateVersion();
        }


        public void OnGUI()
        {
            //GUI.Label(new Rect(0, Screen.height / 2 - 40, Screen.width, 20), "正在下载的资源: " + mCurrentDownLoadAsset);

            //GUI.Label(new Rect(0, Screen.height / 2 - 20, Screen.width, 20), "进度1: " + mAllNeedDownLoadSize + " : " + mCurrentDownLoadSize);

            //GUI.Label(new Rect(0, Screen.height / 2, Screen.width, 20), "进度2: " + mAllNeedDownLoadBytes + " : " + mCurrentDownLoadBytes);

            //GUI.Label(new Rect(0, Screen.height / 2 + 20, Screen.width, 20), "进度3: " + Prograss.ToString());

            //if(mComplete)
            //    GUI.Label(new Rect(0, Screen.height / 2 + 40, Screen.width, 20), "更新成功!!!");

            //if(mForceTermination)
            //    GUI.Label(new Rect(0, Screen.height / 2 + 60, Screen.width, 20), "达到最大重复下载,强制终止!!!");

            //if (!mNetworkConnections)
            //    GUI.Label(new Rect(0, Screen.height / 2 + 80, Screen.width, 20), "网络断开连接,强制终止!!!");

            //if (mNeedCheckNetWorkConnections)
            //{
            //    GUI.Button(new Rect(0, 0, Screen.width, Screen.height / 6), "正在检测网络连接。。。。");
            //}
        }


        //创建SDK Prefab
        public void CreateSDKPrefab()
        {
            SdkManager.EPlatform plType = SdkManager.EPlatform.ePlatform_None;
#if UNITY_STANDALONE_WIN || UNITY_EDITOR || SKIP_SDK         //Windows平台
            plType = SdkManager.EPlatform.ePlatform_None;
#elif UNITY_IPHONE //IOS平
    
    #if SDK_91
            plType =  SdkManager.EPlatform.ePlatform_IOS_91;
    #elif SDK_PP
            plType =  SdkManager.EPlatform.ePlatform_IOS_PP;
    #elif SDK_TBT
            plType =  SdkManager.EPlatform.ePlatform_IOS_TB;
    #elif SDK_CMGE
            plType = SdkManager.EPlatform.ePlatform_IOS_CMGE;     
    #elif SDK_ITOOLS
            plType = SdkManager.EPlatform.ePlatform_IOS_iTools;     
    #elif SDK_ONLINEGAME
            plType = SdkManager.EPlatform.ePlatform_IOS_OnlineGame;    
    #elif SDK_AS
            plType = SdkManager.EPlatform.ePlatform_IOS_As;    
    #elif SDK_XY
            plType = SdkManager.EPlatform.ePlatform_IOS_XY;    
    #elif SDK_CMGE_ZB
            plType = SdkManager.EPlatform.ePlatform_IOS_CMGE_ZB;    
       
#endif
           
#elif UNITY_ANDROID //Android平台
#if SDK_UC
            plType =  SdkManager.EPlatform.ePlatform_Android_UC;
#endif
#endif

            SdkManager.Instance.CreateSdkPrefab(plType);            
        }


        //加载sdk配置文件
        public void loadSdkCfg()
        {
            ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate("Config/SdkCfg", ResourceType.ASSET);
            TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.LoadXml(xmlfile.text);

            ////sdk_platform
            //XmlNode sdkNodes = xmlDoc.SelectSingleNode("sdk_platform");
            //if (sdkNodes != null)
            //{
            //    XmlNodeList sdkNodeList = sdkNodes.ChildNodes;
            //    if (sdkNodeList != null)
            //    {
            //        foreach (XmlNode sdkNode in sdkNodeList)
            //        {
            //            string path = sdkNode.;


            //        }
            //    }
            //}           


            //sdk_platforms
            XmlElement root = xmlDoc.DocumentElement;

            IEnumerator iter = root.GetEnumerator();
            //遍历sdk
            while (iter.MoveNext())
            {
                XmlElement sdk_elem = iter.Current as XmlElement;
                string channel = sdk_elem.GetAttribute("channel");
                string updateAddress = sdk_elem.GetAttribute("updateAddress");

                //Debug.Log("channel" + channel);
                //Debug.Log("server" + server);                


                //添加channel info
                SDKChannelInfo info = new SDKChannelInfo();
                info.channelName = channel;
                info.updateAddress = updateAddress;

                mSdkChannelInfos.Add(channel, info);
            }
        }

        //强更新，更新版本
        public void updateVersion()
        {
            mNetworkConnections = true;

            //读取sdk配置信息
            //loadSdkCfg();

            //获取当前版本Version
            //string curVersion = SdkConector.GetBundleVersion();
            mCurVersion = "1.0.0";

            //获取当前版本Identifier
            //string curIdentifier = SdkConector.GetBundleIdentifier();
            mCurIdentifier = "com.cmge.blzhyz";

            //版本检测文件地址
            string checkVersionAddress = "http://zhyzcdn.joygame.cn/version.xml";

            Debug.Log("000...");
            //加载版本Version.xml信息
            StartCoroutine(downLoadCheckVersion(checkVersionAddress, delegate(WWW serverVersion)
            {

                Debug.Log("12222...");

                //解析版本信息
                parseVersionFile(serverVersion.text, mCurIdentifier, ref mVersionFile);

                //解析版本错误
                if (mVersionFile.version == null)
                {
                    Debug.Log("更新版本错误");
                    //OnUpdateComplete();
                    return;
                }
                //根据版本情况判断是否更新


                //版本不相同，更新
                if (mVersionFile.version != mCurVersion)
                {
                    //如果是整包,并且渠道不支持更新，显示更新提示菜单
                    if (mVersionFile.type == "package" && mVersionFile.updateAddress != "")
                    {
                        //隐藏信息窗口
                        mProcessBar_BG2.SetActive(false);
                        mProcessBar_Lable2.SetActive(false);

                        showUpdateTipUI();
                    }
                    //如果是资源，直接更新
                    else if (mVersionFile.version == "resource")
                    {
                        updateResource();
                    }
                }
                //版本相同
                else
                {
                    OnUpdateComplete();
                }
            }));
        }

        //热更新，更新资源
        public void updateResource()
        {
            mLocalPathMD5 = new VersionResourceFile();
            mServerPathMD5 = new VersionResourceFile();
            mUpdateAssets = new List<string>();
            mUpdateAssetsSize = new List<int>();
            mAllNeedDownLoadSize = 0;
            mCurrentDownLoadSize = 0;
            mAllNeedDownLoadBytes = 0;
            mCurrentDownLoadBytes = 0;
            mComplete = false;

            mCurrentDownLoadAsset = "";
            mCurrentDownLoadAssetFailedTimes = 0;
            mMaxRepeatDownLoadTimes = 5;
            mForceTermination = false;

            mNetworkConnections = true;
            mNeedCheckNetWorkConnections = false;
            mCheckNetWorkConnectionsMaxTimes = 10;

            //1.加载本地资源version.xml信息
            StreamReader localReader = ResourcesManager.OpenText("Version");
            parseResourceVersionFile(localReader.ReadToEnd(), ref mLocalPathMD5);
            localReader.Close();

            //2.加载服务器version.xml信息
            StartCoroutine(downLoad(ResourceCommon.getServerPath("Version.bytes"), delegate(WWW resourceVersion)
            {
                parseResourceVersionFile(resourceVersion.text, ref mServerPathMD5);


                if (needUpdate(mLocalPathMD5, mServerPathMD5, mUpdateAssets, mUpdateAssetsSize))
                {
                    Debug.LogError("need update true");

                    mAllNeedDownLoadSize = mUpdateAssets.Count;

                    //4.下载最新资源
                    downLoadAssets();
                }
                else
                {
                    mComplete = true;
                    OnUpdateComplete();
                    Debug.LogError("Complete update");
                }
            }));
        }


        //更新完成处理
        private void OnUpdateComplete()
        {
            //完成初始
            ResourcesManager.Instance.Init();

            //加载场景
            ResourcesManager.Instance.loadLevel("Scenes/Pvp_Login", null);
            //ResourceUnit unit = ResourcesManager.Instance.loadImmediate("Guis/UIGameLogin", ResourceType.ASSET);
            //GameObject obj = GameObject.Instantiate(unit.Asset) as GameObject;
        }

        //解析版本信息
        private void parseVersionFile(string text, string channel, ref VersionFile version)
        {
            XmlDocument doc = new XmlDocument();
            doc.LoadXml(text);
            XmlElement root = doc.DocumentElement;


            //获取最近的版本信息            
            IEnumerator sdkIter = root.GetEnumerator();
            while (sdkIter.MoveNext())
            {
                XmlElement sdkChild = sdkIter.Current as XmlElement;
                string channelName = sdkChild.GetAttribute("name");

                //匹配sdk
                if (channelName == channel)
                {
                    //版本更新地址，用于不支持更新的渠道
                    string updateAddress = sdkChild.GetAttribute("updateAddress"); ;

                    //Version
                    IEnumerator iter = sdkChild.GetEnumerator();
                    if (iter.MoveNext())
                    {
                        XmlElement child = iter.Current as XmlElement;

                        version.version = child.GetAttribute("version");
                        version.forceUpdate = bool.Parse(child.GetAttribute("forceUpdate"));
                        version.type = child.GetAttribute("type");
                        version.size = int.Parse(child.GetAttribute("size"));
                        version.updateAddress = updateAddress;
                        break;
                    }
                }
            }
        }


        //解析版本资源信息
        private void parseResourceVersionFile(string text, ref VersionResourceFile file)
        {
            XmlDocument doc = new XmlDocument();
            doc.LoadXml(text);
            XmlElement root = doc.DocumentElement;
            file.mNumber = root.GetAttribute("Number");
            file.mBig = Convert.ToBoolean(root.GetAttribute("Big"));
            IEnumerator iter = root.GetEnumerator();
            while (iter.MoveNext())
            {
                XmlElement child = iter.Current as XmlElement;
                file.mPathMD5.Add(child.GetAttribute("fpath"), child.GetAttribute("md5"));
                file.mSize.Add(child.GetAttribute("fpath"), Convert.ToInt32(child.GetAttribute("size")));
            }

            Debug.LogError("paser version fil okk" + text);
        }

        private bool needUpdate(VersionResourceFile local, VersionResourceFile server, List<string> update, List<int> updateSize)
        {
            //后期采用先判定版本号
            foreach (KeyValuePair<string, string> file in server.mPathMD5)
            {
                if (local.mPathMD5.ContainsKey(file.Key))
                {
                    if (local.mPathMD5[file.Key] != file.Value)
                    {
                        update.Add(file.Key);
                        mAllNeedDownLoadBytes += server.mSize[file.Key];
                        updateSize.Add(server.mSize[file.Key]);
                    }
                }
                else
                {
                    update.Add(file.Key);
                    mAllNeedDownLoadBytes += server.mSize[file.Key];
                    updateSize.Add(server.mSize[file.Key]);
                }
            }

            //测试
            foreach (string str in update)
            {
                Debug.LogError("need update" + str);
            }
            return update.Count > 0 ? true : false;
        }

        private void downLoadAssets()
        {
            if (0 == mUpdateAssets.Count)
            {
                StartCoroutine(downLoad(ResourceCommon.getServerPath("Version.bytes"), delegate(WWW serverVersion)
                {
                    //4.覆盖本地version.xml
                    ReplaceLocalRes("Version.bytes", serverVersion.bytes);

                    OnUpdateComplete();
                    mComplete = true;
                }));
                return;
            }

            string file = mUpdateAssets[0];
            mUpdateAssets.RemoveAt(0);
            ++mCurrentDownLoadSize;
            mCurrentDownLoadBytes += mUpdateAssetsSize[0];
            mUpdateAssetsSize.RemoveAt(0);

            StartCoroutine(downLoad(ResourceCommon.getServerPath(file), delegate(WWW w)
            {
                //将下载的资源替换为本地的资源
                ReplaceLocalRes(file, w.bytes);
                downLoadAssets();
            }));
        }

        private void ReplaceLocalRes(string file, byte[] data)
        {

            string localPath;
            //Andrio跟IOS环境使用沙箱目录
            if (Application.platform == RuntimePlatform.Android || Application.platform == RuntimePlatform.IPhonePlayer)
            {
                localPath = string.Format("{0}/{1}", Application.persistentDataPath, file);
            }
            //Window下使用assetbunlde资源目录
            else
            {
                localPath = ResourceCommon.assetbundleFilePath + file;
                Debug.LogError("Replace" + localPath);
            }

            ResourceCommon.CheckFolder(ResourceCommon.getPath(localPath));
            FileStream stream = new FileStream(localPath, FileMode.OpenOrCreate);

            stream.Write(data, 0, data.Length);
            stream.Flush();
            stream.Close();
        }

        private delegate void HandleFinishDownload(WWW www);


        //下载版本检测信息
        private IEnumerator downLoadCheckVersion(string url, HandleFinishDownload finishFun)
        {
            WWW www = new WWW(url);
            yield return www;

            Debug.Log("1111...");

            //网络出错
            if (!string.IsNullOrEmpty(www.error))
            {
                DebugEx.Log(www.error, ResourceCommon.DEBUGTYPENAME);
                www.Dispose();

                //检测网络连接
                //yield return StartCoroutine(checkNetworkConnections());
                //StartCoroutine(downLoad(url, finishFun));
            }
            //成功
            else
            {
                if (finishFun != null)
                {
                    finishFun(www);
                }
                www.Dispose();
            }
        }

        //下载资源AssetBundle
        private IEnumerator downLoad(string url, HandleFinishDownload finishFun)
        {
            if (mCurrentDownLoadAsset == url)
                ++mCurrentDownLoadAssetFailedTimes;
            else
            {
                mCurrentDownLoadAssetFailedTimes = 0;
                mCurrentDownLoadAsset = url;
            }

            if (mCurrentDownLoadAssetFailedTimes > mMaxRepeatDownLoadTimes)
            {
                mForceTermination = true;
                yield break;
            }

            WWW www = new WWW(checkUrl(ref url));
            yield return www;
            if (!string.IsNullOrEmpty(www.error))
            {
                DebugEx.Log(www.error, ResourceCommon.DEBUGTYPENAME);
                www.Dispose();

                //检测网络连接
                yield return StartCoroutine(checkNetworkConnections());

                StartCoroutine(downLoad(url, finishFun));
            }
            else
            {
                if (finishFun != null)
                {
                    finishFun(www);
                }
                www.Dispose();
            }
        }

        private IEnumerator checkNetworkConnections()
        {
            mNeedCheckNetWorkConnections = true;
            //检测网络
            string url = ResourceCommon.getServerPath("checkNetworkConnections.bytes");
            float star = System.DateTime.Now.Second;
            while (true)
            {
                WWW www = new WWW(checkUrl(ref url));
                yield return www;
                if (string.IsNullOrEmpty(www.error))
                {
                    if (www.text == "true")
                    {
                        mNeedCheckNetWorkConnections = false;
                        www.Dispose();
                        break;
                    }
                }
                www.Dispose();
                float end = System.DateTime.Now.Second - star;
                if (end > mCheckNetWorkConnectionsMaxTimes)
                    mNetworkConnections = false;
            }
        }

        private string checkUrl(ref string url)
        {
            string p = url.Replace(" ", "%20");
            //p = p.Replace("+","%2B");
            //p = p.Replace("?","%3F");
            //p = p.Replace("%","%25");
            p = p.Replace("#", "%23");
            //p = p.Replace("&","%26");
            //p = p.Replace("=","%3D");
            return p;
        }

        //显示更新提示菜单
        private void showUpdateTipUI()
        {
            mUpdateTipObj.SetActive(true);

            //显示安装包大小
            GameObject sizeObj = mUpdateTipObj.transform.Find("CenterTip3").gameObject;
            UILabel sizeLable = sizeObj.GetComponent<UILabel>();

            float packageSize = mVersionFile.size / 1024.0f;

            sizeLable.text = ((int)packageSize).ToString() + "K";
        }


        void OnEnable()
        {
            //logo
            //  if (Application.platform == RuntimePlatform.IPhonePlayer || Application.platform == RuntimePlatform.Android)
            // {
            //GameObject obj = GameObject.Instantiate(Resources.Load("Game/LogoCamera")) as GameObject;

            //  UIPlayMovie.PlayMovie("logo.mp4", Color.black,3/* FullScreenMovieControlMode.Hidden*/, 3/*FullScreenMovieScalingMode.Fill*/);
            //  }
        }

        void OnDestroy()
        {
            //移除listener
            //mConfirmButtonPress.RemoveListener(ConfirmUpdate);
            //mCancelmButtonPress.RemoveListener(CancelUpdate);          
        }

        //确认更新
        private void ConfirmUpdate(int ie, bool isPress)
        {
            Debug.LogError("confirm update");
        }

        //取消更新
        private void CancelUpdate(int ie, bool isPress)
        {
            //Debug.LogError("cancel update");

            //进入游戏
            OnUpdateComplete();
        }

    }


}