using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace BlGame.Resource
{
    //加载过程,用于控制loading过程，可用于loading界面等。
    public class LoadProcess
    {
        public static int aSyncLoadCount = 4;
        public string SceneRes;
        public bool isLoading = false;
        //private int curLoadCount;
        public int curLoadCount
        {
            get
            {
                return ResourceManager.Instance.getLoadCount();
            }
        }
        public Dictionary<string, LogicLoadRequest> logicReqDic = new Dictionary<string, LogicLoadRequest>();
        public Dictionary<string, PhyLoadRequest> phyReqDic = new Dictionary<string, PhyLoadRequest>();
        //当前进度
        public float curProgress
        {
            get;
            set;
        }
        public bool isDone = false;
        //文件个数
        public int fileCount
        {
            get;
            set;
        }
        //完成的文件个数
        public int doneFileCount
        {
            get;
            set;
        }
        //总的文件物理大小
        public int filesSize
        {
            get;
            set;
        }
        //总的完成文件物理大小
        public int doneFilesSize
        {
            get;
            set;
        }

        //开始
        public void Start()
        {
            isLoading = true;
            logicReqDic.Clear();
            phyReqDic.Clear();
            isDone = false;
        }
        //暂停
        public void Stop()
        {

        }
        //结束
        public void End()
        {
            isLoading = false;
        }
        private bool isAllLogicComplete()
        {
            foreach (LogicLoadRequest r in logicReqDic.Values)
            {
                LogicResouce res = ResourceManager.Instance.GetLogicRes(r.resPath);
                if (res == null)
                {
                    return false;
                }
            }
            return true;
        }
        private bool isAllLogicRootComplete()
        {
            foreach (LogicLoadRequest r in logicReqDic.Values)
            {
                PhyResouce res = ResourceManager.Instance.GetPhyRes(r.resPath);
                if (res == null)
                {
                    return false;
                }
            }
            return true;
        }
        //更新
        public void Update()
        {
            
            if (isAllLogicRootComplete() == false)
            {
                foreach (LogicLoadRequest logicLoadReq in logicReqDic.Values)
                {
                    if (curLoadCount < aSyncLoadCount)
                    {
                        if (ResourceManager.Instance.GetLogicRes(logicLoadReq.resPath) != null)
                        {
                            logicLoadReq.status = LogicLoadRequest.LOADSTATUS.END;
                        }
                        else
                        {
                            if (logicLoadReq.status == LogicLoadRequest.LOADSTATUS.WAIT)
                            {
                                ResourceManager.Instance.LoadImpl(logicLoadReq.resPath, LogicResouce.getRootResType(logicLoadReq.resType),
                                (path) =>
                                {
                                    AssetBundle ab = ResourceManager.Instance.GetPhyRes(path).assetBundle;
                                    StringScriptableObject holder = (StringScriptableObject)ab.Load("DependentBundleNames");
                                    LogicResourceBuilder logicBuilder = new LogicResourceBuilder();
                                    logicBuilder.resPath = logicLoadReq.resPath;
                                    logicBuilder.logicResType = logicLoadReq.resType;
                                    if (holder != null)
                                    {
                                        if (holder.content != null)
                                        {
                                            foreach (string s in holder.content)
                                            {

                                                // GameDefine.GameMethod.DebugError("got dependency:" + s);
                                                AddPhyFile(s, PhyResouce.EPhyResType.EPhyResPrefab);
                                                logicBuilder.resLists.Add(s);
                                            }
                                        }
                                    }
                                    ResourceManager.Instance.AddLogicResourceBuilder(logicBuilder);

                                });
                                logicLoadReq.status = LogicLoadRequest.LOADSTATUS.START;
                            }
                        }
                    }
                }
            }
            else
            {
                foreach (PhyLoadRequest phyLoadReq in phyReqDic.Values)
                {
                    if (curLoadCount < aSyncLoadCount)
                    {
                        ResourceManager.Instance.LoadImpl(phyLoadReq.resPath, phyLoadReq.resType);
                    }
                }
            }
            UpdateProgress();
        }
        private void UpdateProgress()
        {
            if (isDone == true)
            {
                return;
            }
            float logicPercent = 0.15f;
            float phyPercent = 0.85f;


            float logicReqNum = logicReqDic.Count;
            float phyReqNum = phyReqDic.Count;
            curProgress = 0.0f;
            if (logicReqDic.Count == 0)
            {
                curProgress += 0.15f;
            }
            else
            {
                foreach (LogicLoadRequest lr in logicReqDic.Values)
                {
                    curProgress += (logicPercent * getFileDownloadPercent(lr.resPath) / logicReqNum);
                }
            }
            if (phyReqDic.Count == 0 && isAllLogicRootComplete() == true)
            {
                curProgress += 0.85f;
            }
            else
            {
                foreach (PhyLoadRequest pr in phyReqDic.Values)
                {
                    curProgress += (phyPercent * getFileDownloadPercent(pr.resPath) / phyReqNum);
                }
            }

            isDone = isAllLogicComplete();
            if (isDone == true)
            {
                OnSceneLoadComplete();
                Stop();
            }
        }
        private void OnSceneLoadComplete()
        {
            LogicResouce lRes = ResourceManager.Instance.GetLogicRes(SceneRes);
            //LogicResourceBuilder builder = null;
            LightmapSettings.lightmapsMode = LightmapsMode.Single;
            LightmapSettings.lightmapsMode = LightmapsMode.Single;


            Dictionary<int, Texture2D> lightMapDic = new Dictionary<int, Texture2D>();
            foreach (string s in lRes.phyResList)
            {
                if (s.Contains("LightmapFar"))
                {
                    string[] strs = s.Split(new char[] { '-', '.' });
                    if (strs != null && strs.Length > 0)
                    {
                        int index = Int32.Parse(strs[strs.Length - 2]);//strs[Length - 1];
                        if (lightMapDic.ContainsKey(index))
                        {
                        }
                        else
                        {
                            lightMapDic.Add(index, ResourceManager.Instance.GetPhyRes(s).getTexture());
                        }

                    }
                }
                if (s.Contains("LightProbe"))
                {
                    PhyResouce probe = ResourceManager.Instance.GetPhyRes(s);
                    if (probe != null)
                    {
                        LightProbes probes = (LightProbes)probe.assetBundle.mainAsset;
                        LightmapSettings.lightProbes = probes;
                    }
                }
            }
            LightmapData[] ldarr = new LightmapData[lightMapDic.Count];
            foreach (KeyValuePair<int, Texture2D> kv in lightMapDic)
            {
                LightmapData ld = new LightmapData();
                ld.lightmapFar = kv.Value;
                ldarr[kv.Key] = ld;
            }
            LightmapSettings.lightmaps = ldarr;




            PhyResouce res = ResourceManager.Instance.GetPhyRes(SceneRes);
            UnityEngine.Object obj = res.assetBundle.mainAsset;
            GameObject.Instantiate(obj);
        }
        private float getFileDownloadPercent(string path)
        {
            if (ResourceManager.Instance.GetPhyRes(path) != null)
            {
                return 1.0f;
            }
            LoadImplement impl = ResourceManager.Instance.getLoadImplement(path);
            if (impl == null)
            {
                return 0.0f;
            }
            return impl.www.progress;
        }
        private void OnLogicRootLoaded(string path)
        {
            //
        }

        //添加一个逻辑文件，并将所有依赖的文件加入加载队列
        public void AddLogicFile(string logicFile, LogicResouce.ELogicResType logicResType)
        {
            //if (ResourceManager.Instance.GetLogicRes(logicFile) != null)
            //{
            //    return;
            //}
            if (logicReqDic.ContainsKey(logicFile) == true)
            {
                return;
            }
            //
            LogicLoadRequest loadReq = new LogicLoadRequest();
            loadReq.resPath = logicFile;
            loadReq.resType = logicResType;

            logicReqDic.Add(logicFile, loadReq);

            //
        }
        //添加一个物理文件到加载队列
        public void AddPhyFile(string phyFile, PhyResouce.EPhyResType phyResType)
        {
            if (ResourceManager.Instance.GetPhyRes(phyFile) != null)
            {
                return;
            }
            if (phyReqDic.ContainsKey(phyFile) == true)
            {
                return;
            }

            PhyLoadRequest loadReq = new PhyLoadRequest();
            loadReq.resPath = phyFile;
            loadReq.resType = phyResType;


            phyReqDic.Add(phyFile, loadReq);
        }
    }
    
    //加载请求
    public class LogicLoadRequest
    {
        public string resPath;
        public LogicResouce.ELogicResType resType;
        public enum LOADSTATUS
        {
            WAIT,
            START,
            END,
        }
        //
        public LOADSTATUS status = LOADSTATUS.WAIT;
        //
    }
    public class PhyLoadRequest
    {
        public string resPath;
        public PhyResouce.EPhyResType resType;
    }
}
