using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.IO;
//-------------------------------------------------------------------------------
//资源分为逻辑资源和物理资源
//逻辑资源是逻辑操作需要的接口，比如场景，角色等
//物理资源是物理资源的组成，比如mesh,texture等
//游戏资源的管理逻辑主要集中在逻辑层面，物理资源通过机制管理
//
//-------------------------------------------------------------------------------
//这可能是临时方案
//逻辑资源引用物理资源，引用哪些资源在逻辑资源的prefab中存储索引
//-------------------------------------------------------------------------------

namespace BlGame.Resource
{
		public class ResourceManager
		{
            private Dictionary<string, LogicResouce> logicResDic = new Dictionary<string, LogicResouce>();
            private Dictionary<string, PhyResouce> phyResDic = new Dictionary<string, PhyResouce>();
            //异步加载
            private Dictionary<string, LoadImplement> loadImplDic = new Dictionary<string, LoadImplement>();
            private Dictionary<string, LogicResourceBuilder> logicResBuilder = new Dictionary<string, LogicResourceBuilder>();
            public  static readonly ResourceManager Instance = new ResourceManager();
            //
            public LoadProcess loadProcess = new LoadProcess();
			public ResourceManager ()
			{

			}
            //获取LoadImplement的数量
            public int getLoadCount()
            {
                return loadImplDic.Count;
            }
            //
            public void Update()
            {
                if (loadProcess != null)
                {
                    if (loadProcess.isLoading == true)
                    {
                        loadProcess.Update();

                    }
                }

                List<string> removeLoadList = new List<string>();
                //遍历loadimplement
                foreach (LoadImplement l in loadImplDic.Values)
                {
                    if (l.www.isDone == true)
                    {
                        if (l.www.error == null)
                        {
                            if (l.createReq == null)
                            {
                                l.createReq = AssetBundle.CreateFromMemory(l.www.bytes);
                            }
                            if (l.createReq.isDone == true)
                            {
                               // GameDefine.GameMethod.DebugError("Load Res Success:" + l.resPath);
                                PhyResouce phyRes = new PhyResouce();
                                phyRes.resPath = l.resPath;
                                phyRes.phyResType = l.phyResType;
                                phyRes.assetBundle = l.createReq.assetBundle;
                                //if(phyResDic.ContainsKey(phyRes.resPath) == true)
                                //{
                                //    Debug.LogError("already have:"+phyRes.resPath);
                                //}
                                //else
                               // Debug.LogError("phy res :" + phyRes.resPath);
                                    phyResDic.Add(phyRes.resPath, phyRes);
                               // }
                                removeLoadList.Add(l.resPath);

                                
                            }
                        }
                        else
                        {
                            GameDefine.GameMethod.DebugError("Load Res Failed:" + l.www.url + "======" + l.resPath + " error is:" + l.www.error.ToString());
                            removeLoadList.Add(l.resPath);
                        }
                    }
                }
                foreach (string s in removeLoadList)
                {
                    
                    LoadImplement l = getLoadImplement(s);
                    if (l.onResLoaded != null)
                    {
                        l.onResLoaded(l.resPath);
                    }
                    loadImplDic.Remove(s);
                }

                List<string> removeBuilderList = new List<string>();
                foreach (LogicResourceBuilder builder in logicResBuilder.Values)
                {
                    bool allLoaded = true;
                    foreach (string s in builder.resLists)
                    {
                        if (GetPhyRes(s) == null)
                        {
                            allLoaded = false;
                            break;
                        }
                    }
                    if (allLoaded == true)
                    {
                        removeBuilderList.Add(builder.resPath);
                        if (builder.onLogicResourceBuilded != null)
                        {
                            builder.onLogicResourceBuilded(builder.resPath);
                        }
                        LogicResouce logicRes = new LogicResouce();
                        logicRes.resPath = builder.resPath;
                        logicRes.logicResType = builder.logicResType;
                        logicRes.phyResList = builder.resLists;
                        if (logicResDic.ContainsKey(logicRes.resPath) == true)
                        {
                            logicResDic[logicRes.resPath] = logicRes;
                        }
                        else
                        {
                            logicResDic.Add(logicRes.resPath, logicRes);
                        }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    }
                }
                foreach (string s in removeBuilderList)
                {
                    logicResBuilder.Remove(s);
                }
                //
            }
            //
            //加载一个逻辑资源,使用阻塞加载
            public void AsynLoadLogicRes(string path,LogicResouce.ELogicResType type,OnResLoaded onLoaded = null)
            {
                if(GetLogicRes(path) != null)
                {
                    if (onLoaded != null)
                    {
                        onLoaded(path);
                    }
                    return;
                }
                //
                //LogicResouce res = new LogicResouce();
                //res.resPath = path;
                //res.logicResType = type;
                switch (type)
                {
                    case LogicResouce.ELogicResType.ERes_Effect:
                        {
                            LoadImpl(path, PhyResouce.EPhyResType.EPhyResPrefab,
                                (resPath) =>
                                {
                                    AssetBundle ab = ResourceManager.Instance.GetPhyRes(path).assetBundle;
                                    StringScriptableObject holder = (StringScriptableObject)ab.Load("DependentBundleNames");
                                    LogicResourceBuilder logicBuilder = new LogicResourceBuilder();
                                    logicBuilder.resPath = resPath;
                                    logicBuilder.logicResType = type;
                                    logicBuilder.onLogicResourceBuilded = (builderPath) =>
                                        {
                                            if (onLoaded != null)
                                            {
                                                onLoaded(builderPath);
                                            }
                                        };
                                    //
                                    if (holder != null)
                                    {
                                        if (holder.content != null)
                                        {
                                            foreach (string s in holder.content)
                                            {
                                                logicBuilder.resLists.Add(s);
                                                LoadImpl(s, PhyResouce.EPhyResType.EPhyResPrefab);
                                                //Debug.LogError(s);
                                            }
                                        }
                                    }
                                    ResourceManager.Instance.AddLogicResourceBuilder(logicBuilder);
                                }
                            , null);
                        }
                        break;

                }
            }
            public void AddPhyRes(string resPath,PhyResouce phyRes)
            {
                if(GetPhyRes(resPath) != null)
                {
                    return;
                }
                phyResDic[resPath] = phyRes;
            }
            //获取资源
            public LogicResouce GetLogicRes(string path)
            {
                if (logicResDic.ContainsKey(path) == false)
                {
                    return null;
                }
                return logicResDic[path];
            }
            //释放资源
            public void ReleaseLogicResource(string path)
            {
            }
            //
            public void LoadImpl(string path,PhyResouce.EPhyResType type,OnResLoaded onLoaded = null,OnResLoadError onError = null)
            {
                //GameDefine.GameMethod.DebugError("load " + path);
                if (phyResDic.ContainsKey(path) == true)
                {
                    return;
                }
                if (loadImplDic.ContainsKey(path) == true)
                {
                    return;
                }
                LoadImplement l = new LoadImplement();
                l.phyResType = type;
                l.onResLoaded = onLoaded;
                l.onResLoadError = onError;
                l.resPath = path;
                l.start();
                loadImplDic.Add(path, l);
            }
            //获取物理资源
            public PhyResouce GetPhyRes(string path)
            {
                if (phyResDic.ContainsKey(path) == false)
                {
                    return null;
                }
                return phyResDic[path];
            }
            //
            public void AddLogicResourceBuilder(LogicResourceBuilder builder)
            {
                //
                if (logicResBuilder.ContainsKey(builder.resPath))
                {
                    return;
                }
                if (logicResDic.ContainsKey(builder.resPath))
                {
                    return;
                }
                logicResBuilder.Add(builder.resPath, builder);
            }
            //
            public LoadImplement getLoadImplement(string path)
            {
                if (loadImplDic.ContainsKey(path))
                {
                    return loadImplDic[path];
                }
                return null;
            }
		}
}

