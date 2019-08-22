using System;
using System.IO;
using System.Xml;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Object = UnityEngine.Object;

namespace BlGame.Resource
{
    public class ResourcesManager : UnitySingleton<ResourcesManager>
    {
        //是否通过assetbundle加载资源
        public bool UsedAssetBundle = false;

        private bool mInit = false;
        private int mFrameCount = 0;
        private Request mCurrentRequest = null;
        private Queue<Request> mAllRequests = new Queue<Request>();

        //保存读取的Resource信息
        private AssetInfoManager mAssetInfoManager = null;
        private Dictionary<string, string> mResources = new Dictionary<string, string>();
        //加载的资源信息
        private Dictionary<string, ResourceUnit> mLoadedResourceUnit = new Dictionary<string, ResourceUnit>();

        public delegate void HandleFinishLoad(ResourceUnit resource);
        public delegate void HandleFinishLoadLevel();
        public delegate void HandleFinishUnLoadLevel();



        //internal Hashtable mLoadedResourceUnit;
        //private Hashtable mGameObjectInstanceID;
        //private List<AssetBundle> mTempAssetBundles;

        public void Init()
        {
            //使用assetbundle打包功能
            if (UsedAssetBundle)
            {
                mAssetInfoManager = new AssetInfoManager();
                mAssetInfoManager.LoadAssetInfo();

                ArchiveManager.Instance.Init();
            }

            mInit = true;
        }

        //public void Init()
        //{
        //    //使用assetbundle打包功能
        //    if (UsedAssetBundle)
        //    {
        //        mAssetExtraInfoManager = new AssetExtraInfoManager();
        //        mAssetExtraInfoManager.importAssetExtraInfo();

        //        ArchiveManager.Instance.Init();
        //    }

        //    mFrameCount = 0;
        //    mLoadedResourceUnit = new Hashtable();
        //    mGameObjectInstanceID = new Hashtable();
        //    mTempAssetBundles = new List<AssetBundle>();
        //    mAllRequests = new Queue<Request>();
        //    mCurrentRequest = null;

        //    ResourceUnitMonitor.mCHandle = ConstructGameObjectFromPrefab;
        //    ResourceUnitMonitor.mDHandle = DestructGameObjectFromPrefab;

        //    mInit = true;
        //}

        //public void UnInit()
        //{
        //    if (mLoadedResourceUnit.Count > 0)
        //        DebugEx.LogError("load and unLoad not match in ResourceManager!!!", ResourceCommon.DEBUGTYPENAME);
        //    if (mGameObjectInstanceID.Count > 0)
        //        DebugEx.LogError("construct and destruct not match in Object Instantiate", ResourceCommon.DEBUGTYPENAME);
        //}

        //public void dumpAllLoadedAssets()
        //{
        //    string info = "the num assets is : " + mLoadedResourceUnit.Count + "\n";
        //    foreach (DictionaryEntry asset in mLoadedResourceUnit)
        //    {
        //        info += asset.Key + "; and the mReferenceCount is : " + (asset.Value as ResourceUnit).ReferenceCount + "\n";
        //    }
        //    DebugEx.Log(info, ResourceCommon.DEBUGTYPENAME);
        //}

        //public void dumpAllAssetBundleSize()
        //{
        //    string info = "";
        //    int allSize = 0;
        //    foreach (DictionaryEntry asset in mLoadedResourceUnit)
        //    {
        //        ResourceUnit resource = asset.Value as ResourceUnit;
        //        if (resource.Assetbundle != null)
        //        {
        //            allSize += resource.AssetBundleSize;
        //            info += "asset : " + asset.Key + "; the size is : " + resource.AssetBundleSize + "\n";
        //        }
        //    }
        //    info += "all size is : " + allSize + "; same is : " + ((float)allSize / 1048576).ToString() + " M";
        //    DebugEx.Log(info, ResourceCommon.DEBUGTYPENAME, true);
        //}

        //public void deleteAllAssetBundle()
        //{
        //    foreach (DictionaryEntry asset in mLoadedResourceUnit)
        //    {
        //        ResourceUnit resource = asset.Value as ResourceUnit;
        //        if (resource.Assetbundle != null)
        //        {
        //            //resource.Assetbundle.Unload(false);
        //            resource.Assetbundle = null;
        //        }
        //    }
        //}

        public void Update()
        {
            if (!mInit)
                return;

            if (null == mCurrentRequest && mAllRequests.Count > 0)
                handleRequest();

            ++mFrameCount;
            if (mFrameCount == 300)
            {
                //Resources.UnloadUnusedAssets();
                mFrameCount = 0;
            }
        }

        private void handleRequest()
        {
            //使用assetbundle打包功能
            if (UsedAssetBundle)
            {
                mCurrentRequest = mAllRequests.Dequeue();

                //相对Asset的完整资源路径
                string fileName = mCurrentRequest.mFileName;

                //ResourceCommon.Log("handleRequest, the type is : " + mCurrentRequest.mResourceType + "\nthe relativePath path is : " + relativePath);

                switch (mCurrentRequest.mRequestType)
                {
                    case RequestType.LOAD:
                        {
                            switch (mCurrentRequest.mResourceType)
                            {
                                case ResourceType.ASSET:
                                case ResourceType.PREFAB:
                                    {
                                        if (mLoadedResourceUnit.ContainsKey(fileName))
                                        {
                                            //(mLoadedResourceUnit[fileName] as ResourceUnit).addReferenceCount();

                                            mCurrentRequest.mResourceAsyncOperation.mComplete = true;
                                            mCurrentRequest.mResourceAsyncOperation.mResource = mLoadedResourceUnit[fileName] as ResourceUnit;

                                            if (null != mCurrentRequest.mHandle)
                                                mCurrentRequest.mHandle(mLoadedResourceUnit[fileName] as ResourceUnit);
                                            handleResponse();
                                        }
                                        else
                                        {
                                            //传入相对路径名称
                                            //StartCoroutine(_load(fileName, mCurrentRequest.mHandle, mCurrentRequest.mResourceType, mCurrentRequest.mResourceAsyncOperation));
                                        }
                                    }
                                    break;
                                case ResourceType.LEVELASSET:
                                    {
                                        DebugEx.LogError("do you real need a single level asset??? this is have not decide!!!", ResourceCommon.DEBUGTYPENAME);
                                    }
                                    break;
                                case ResourceType.LEVEL:
                                    {
                                        DebugEx.LogError("this is impossible!!!", ResourceCommon.DEBUGTYPENAME);
                                    }
                                    break;
                            }
                        }
                        break;
                    case RequestType.UNLOAD:
                        {
                            if (!mLoadedResourceUnit.ContainsKey(fileName))
                                DebugEx.LogError("can not find " + fileName, ResourceCommon.DEBUGTYPENAME);
                            else
                            {
                                //(mLoadedResourceUnit[fileName] as ResourceUnit).reduceReferenceCount();
                            }
                            handleResponse();
                        }
                        break;
                    case RequestType.LOADLEVEL:
                        {
                            StartCoroutine(_loadLevel(fileName, mCurrentRequest.mHandleLevel, ResourceType.LEVEL, mCurrentRequest.mResourceAsyncOperation));
                        }
                        break;
                    case RequestType.UNLOADLEVEL:
                        {
                            if (!mLoadedResourceUnit.ContainsKey(fileName))
                                DebugEx.LogError("can not find level " + fileName, ResourceCommon.DEBUGTYPENAME);
                            else
                            {
                                //(mLoadedResourceUnit[fileName] as ResourceUnit).reduceReferenceCount();

                                if (null != mCurrentRequest.mHandleUnloadLevel)
                                    mCurrentRequest.mHandleUnloadLevel();
                            }
                            handleResponse();
                        }
                        break;
                }
            }
            //不使用打包
            else
            {
                mCurrentRequest = mAllRequests.Dequeue();

                switch (mCurrentRequest.mRequestType)
                {
                    case RequestType.LOAD:
                        {
                            switch (mCurrentRequest.mResourceType)
                            {
                                case ResourceType.ASSET:
                                case ResourceType.PREFAB:
                                    {
                                        //暂时不处理，直接使用资源相对路径
                                        //StartCoroutine(_load(mCurrentRequest.mFileName, mCurrentRequest.mHandle, mCurrentRequest.mResourceType, mCurrentRequest.mResourceAsyncOperation));
                                    }
                                    break;
                                case ResourceType.LEVELASSET:
                                    {
                                        DebugEx.LogError("do you real need a single level asset??? this is have not decide!!!", ResourceCommon.DEBUGTYPENAME);
                                    }
                                    break;
                                case ResourceType.LEVEL:
                                    {
                                        DebugEx.LogError("this is impossible!!!", ResourceCommon.DEBUGTYPENAME);
                                    }
                                    break;
                            }
                        }
                        break;
                    case RequestType.UNLOAD:
                        {
                            handleResponse();
                        }
                        break;
                    case RequestType.LOADLEVEL:
                        {
                            StartCoroutine(_loadLevel(mCurrentRequest.mFileName, mCurrentRequest.mHandleLevel, ResourceType.LEVEL, mCurrentRequest.mResourceAsyncOperation));
                        }
                        break;
                    case RequestType.UNLOADLEVEL:
                        {
                            if (null != mCurrentRequest.mHandleUnloadLevel)
                                mCurrentRequest.mHandleUnloadLevel();
                            handleResponse();
                        }
                        break;
                }
            }
        }

        private void handleResponse()
        {
            mCurrentRequest = null;
        }

        //传入Resources下相对路径名称 例如Resources/Game/Effect1    传入Game/Effect1
        public ResourceUnit loadImmediate(string filePathName, ResourceType resourceType, string archiveName = "Resources")
        {
            //使用assetbundle打包
            if (UsedAssetBundle)
            {
                //添加Resource
                string completePath = "Resources/" + filePathName;

                string completeName = ArchiveManager.Instance.getPath("Resources", completePath);

                //根据场景名称获取asset信息
                AssetInfo sceneAssetInfo = mAssetInfoManager.GetAssetInfo(completeName);

                //获取依赖的asset的索引
                foreach (int index in sceneAssetInfo.mDependencys)
                {
                    //根据索引获取依赖的Asset
                    AssetInfo depencyAsset = mAssetInfoManager.GetAssetInfo(index);
                    string depencyAssetName = depencyAsset.mName;

                    //加载场景依赖assetbundle


                    _LoadImmediate(depencyAssetName, ResourceType.ASSET);
                }

                //加载本身预制件
                ResourceUnit unit = _LoadImmediate(completeName, resourceType);

                return unit;
            }
            //不使用
            else
            {
                Object asset = Resources.Load(filePathName);
                ResourceUnit resource = new ResourceUnit(null, 0, asset, null, resourceType);
                return resource;
            }
            ////使用打包
            //if (UsedAssetBundle)
            //{
            //    //获取资源名称，无路径
            //    string fileName = ResourceCommon.getResourceName(filePathName);

            //    string path = ArchiveManager.Instance.getPath(archiveName, fileName);
            //    if (path == null)
            //    {
            //        DebugEx.LogError("can not find " + filePathName + "in load immediate");
            //    }

            //    string completeName = ResourceCommon.getFileName(path, true);
            //    if (mLoadedResourceUnit.ContainsKey(completeName))
            //    {
            //        (mLoadedResourceUnit[completeName] as ResourceUnit).addReferenceCount();
            //        return mLoadedResourceUnit[completeName] as ResourceUnit;
            //    }

            //    ResourceCommon.Log("begin inner loadImmediate :" + path);

            //    int assetBundleSize = 0;

            //    byte[] binary = ResourceCommon.getAssetBundleFileBytes(path, ref assetBundleSize);
            //    AssetBundle assetBundle = AssetBundle.CreateFromMemoryImmediate(binary);

            //    if (null == assetBundle)//如果发生,有可能是刚好异步创建assetbundle刚好完成了.检测到再处理吧。。。
            //        DebugEx.LogError("the assetbundle create error!!! and the asset is : " + path, "CreateAssetBunlde", true);

            //    string name = ResourceCommon.getFileName(path, true);

            //    AssetExtraInfo mAssetExtraInfo = mAssetExtraInfoManager.getAssetExtraInfo(completeName);
            //    List<Object> mHasLoaded = new List<Object>();

            //    foreach (string assetdeps in mAssetExtraInfo.mAllDps)
            //    {
            //        string depscompleteName = ResourceCommon.getFileName(assetdeps, true);
            //        if (mLoadedResourceUnit.ContainsKey(ResourceCommon.getFileName(depscompleteName)))
            //        {
            //            mHasLoaded.Add((mLoadedResourceUnit[depscompleteName] as ResourceUnit).Asset);
            //        }
            //    }

            //    if (0 != mAssetExtraInfo.mAllDps.Count)
            //        loadDependenceAssetImmediate(mAssetExtraInfo.mAllDps, ResourceType.ASSET);

            //    Object asset = assetBundle.Load(name);


            //    Debug.Log("ssss" + name);



            //    if (ResourceType.ASSET == resourceType)
            //    {
            //        foreach (Object loadedAsset in mHasLoaded)
            //        {
            //            ////处理材质
            //            if (asset.GetType() == typeof(Material) &&
            //                (loadedAsset.GetType() == typeof(Texture) || loadedAsset.GetType() == typeof(Texture2D) || loadedAsset.GetType() == typeof(Texture3D)))
            //            {
            //                Material mat = asset as Material;
            //                if (!mAssetExtraInfo.mhandleDeleteAssetBundleExtraInfo.mAllTextureNameInShader.ContainsKey(loadedAsset.name))
            //                    DebugEx.LogError("the asset : " + loadedAsset.name + " dependenced by : " + path + " but the asset is not exist in mAllTextureNameInShader, so maybe is your material and shader is not match!!!", ResourceCommon.DEBUGTYPENAME);
            //                else
            //                    mat.SetTexture(mAssetExtraInfo.mhandleDeleteAssetBundleExtraInfo.mAllTextureNameInShader[loadedAsset.name], loadedAsset as Texture);
            //            }
            //        }
            //    }

            //    ResourceUnit resource = new ResourceUnit(mAssetExtraInfo.mCanDeleteAssetbundle || (mAssetExtraInfo.mSingleDependenced && mAssetExtraInfo.mNoUpeerDependenced) ? null : assetBundle, assetBundleSize, asset, path, resourceType, mAssetExtraInfo.mAllDependencesAssetSize);
            //    if (null != mAssetExtraInfo.mAllDps)
            //    {
            //        foreach (string p in mAssetExtraInfo.mAllDps)
            //        {
            //            string depcompleteName = ResourceCommon.getFileName(p, true);
            //            resource.NextLevelAssets.Add(mLoadedResourceUnit[depcompleteName] as ResourceUnit);
            //        }
            //    }

            //    resource.addReferenceCount();
            //    mLoadedResourceUnit.Add(completeName, resource);
            //    ResourceCommon.Log("mLoadedResourceUnit.Add : " + path);

            //    if (ResourceType.PREFAB == resource.resourceType)
            //    {
            //        GameObject prefab = asset as GameObject;
            //        if (null == prefab)
            //            Debug.LogError("the : " + path + " load asset failed , maybe you set a error ResourceType!!!");
            //        prefab.AddComponent<ResourceUnitMonitor>();
            //    }

            //    //if (mAssetExtraInfo.mCanDeleteAssetbundle || (mAssetExtraInfo.mSingleDependenced && mAssetExtraInfo.mNoUpeerDependenced))
            //    //    assetBundle.Unload(false);
            //    //foreach (AssetBundle bundle in mTempAssetBundles)
            //    //    bundle.Unload(false);
            //    //mTempAssetBundles.Clear();

            //    ResourceCommon.Log("end inner load :" + path);

            //    return resource;
            //}
            ////不使用打包
            //else
            //{
            //    Object asset = Resources.Load(filePathName);
            //    ResourceUnit resource = new ResourceUnit(null, 0, asset, null, resourceType, 0);
            //    return resource;
            //}
        }

        //public void unLoadImmediate(string fileName, string archiveName = "Resources")
        //{
        //    //使用AssetBundle打包
        //    if (UsedAssetBundle)
        //    {
        //        string path = ArchiveManager.Instance.getPath(archiveName, fileName);

        //        string completeName = ResourceCommon.getFileName(path, true);
        //        if (!mLoadedResourceUnit.ContainsKey(completeName))
        //            DebugEx.LogError("can not find " + path, ResourceCommon.DEBUGTYPENAME);
        //        else
        //        {
        //            (mLoadedResourceUnit[completeName] as ResourceUnit).reduceReferenceCount();
        //        }
        //    }
        //}

        //public ResourceAsyncOperation load(string filePathName, HandleFinishLoad handle, ResourceType type, string archiveName = "Resources")
        //{
        //    //使用assetbundle打包
        //    if (UsedAssetBundle)
        //    {
        //        //ResourceCommon.Log("load : " + fileName + " and the type is : " + type.ToString() + " and the archiveName is : " + archiveName);

        //        string fileName = ResourceCommon.getResourceName(filePathName);
        //        string relativePath = ArchiveManager.Instance.getPath(archiveName, fileName);
        //        string completeName = ResourceCommon.getFileName(relativePath, true);

        //        if (mLoadedResourceUnit.ContainsKey(completeName))
        //        {
        //            (mLoadedResourceUnit[completeName] as ResourceUnit).addReferenceCount();
        //            ResourceAsyncOperation operation = new ResourceAsyncOperation(RequestType.LOAD);
        //            operation.mComplete = true;
        //            operation.mResource = mLoadedResourceUnit[completeName] as ResourceUnit;
        //            if (null != handle)
        //                handle(mLoadedResourceUnit[completeName] as ResourceUnit);
        //            return operation;
        //        }
        //        else
        //        {
        //            ResourceAsyncOperation operation = new ResourceAsyncOperation(RequestType.LOAD);
        //            mAllRequests.Enqueue(new Request(filePathName, archiveName, type, handle, RequestType.LOAD, operation));
        //            return operation;
        //        }
        //    }
        //    //不使用
        //    else
        //    {
        //        ResourceAsyncOperation operation = new ResourceAsyncOperation(RequestType.LOAD);
        //        mAllRequests.Enqueue(new Request(filePathName, archiveName, type, handle, RequestType.LOAD, operation));
        //        return operation;
        //    }
        //}

        //public void unload(string fileName, string archiveName = "Resources")
        //{
        //    ResourceCommon.Log("unload : " + fileName + " and the archiveName is : " + archiveName);

        //    mAllRequests.Enqueue(new Request(fileName, archiveName, RequestType.UNLOAD));
        //}


        //fileName = "Scene/1"
        public ResourceAsyncOperation loadLevel(string fileName, HandleFinishLoadLevel handle, string archiveName = "Level")
        {
            //使用assetbundle打包
            if (UsedAssetBundle)
            {
                //ResourceCommon.Log("loadLevel : " + fileName + " and the archiveName is : " + archiveName);

                //获取完整路径
                string completeName = ArchiveManager.Instance.getPath(archiveName, fileName);
                if (mLoadedResourceUnit.ContainsKey(completeName))
                {
                    DebugEx.LogError("why you load same level twice, maybe you have not unload last time!!!", ResourceCommon.DEBUGTYPENAME);
                    return null;
                }
                else
                {
                    ResourceAsyncOperation operation = new ResourceAsyncOperation(RequestType.LOADLEVEL);
                    mAllRequests.Enqueue(new Request(completeName, ResourceType.LEVEL, handle, RequestType.LOADLEVEL, operation));
                    return operation;
                }
            }
            //不使用
            else
            {
                ResourceAsyncOperation operation = new ResourceAsyncOperation(RequestType.LOADLEVEL);
                mAllRequests.Enqueue(new Request(fileName, ResourceType.LEVEL, handle, RequestType.LOADLEVEL, operation));
                return operation;
            }
        }

        //public void unLoadLevel(string fileName, HandleFinishUnLoadLevel handle, string archiveName = "Level")
        //{
        //    ResourceCommon.Log("unLoadLevel : " + fileName + " and the archiveName is : " + archiveName);

        //    mAllRequests.Enqueue(new Request(fileName, archiveName, handle, RequestType.UNLOADLEVEL));
        //}

        //private void ConstructGameObjectFromPrefab(int instanceID, string name)
        //{
        //    ResourceCommon.Log("construct : " + name);

        //    string prefabName = name.Replace("(Clone)", "");
        //    if (mGameObjectInstanceID.ContainsKey(instanceID))
        //        DebugEx.LogError(" this is impossible when construct : " + name, ResourceCommon.DEBUGTYPENAME);
        //    else
        //        mGameObjectInstanceID.Add(instanceID, prefabName);

        //    string path = ArchiveManager.Instance.getPath("Resources", prefabName);
        //    string completeName = ResourceCommon.getFileName(path, true);
        //    if (mLoadedResourceUnit.ContainsKey(completeName))
        //        (mLoadedResourceUnit[completeName] as ResourceUnit).addReferenceCount();
        //    else
        //        DebugEx.LogError("can not find : " + name + " in mLoadedResourceUnit!!!", ResourceCommon.DEBUGTYPENAME);
        //}

        //private void DestructGameObjectFromPrefab(int instanceID, string name)
        //{
        //    if (!mGameObjectInstanceID.ContainsKey(instanceID))
        //        DebugEx.LogError("can not find " + name, ResourceCommon.DEBUGTYPENAME);

        //    string prefabName = mGameObjectInstanceID[instanceID] as string;
        //    ResourceCommon.Log("destruct : " + prefabName);
        //    string path = ArchiveManager.Instance.getPath("Resources", prefabName);
        //    string completeName = ResourceCommon.getFileName(path, true);
        //    if (mLoadedResourceUnit.ContainsKey(completeName))
        //        (mLoadedResourceUnit[completeName] as ResourceUnit).reduceReferenceCount();
        //    else
        //        DebugEx.LogError("can not find : " + name + " in mLoadedResourceUnit!!!", ResourceCommon.DEBUGTYPENAME);

        //    mGameObjectInstanceID.Remove(instanceID);
        //}

        //private IEnumerator _load(string path, HandleFinishLoad handle, ResourceType resourceType, ResourceAsyncOperation operation)
        //{
        //    //使用assetbundle打包
        //    if (UsedAssetBundle)
        //    {
        //        //ResourceCommon.Log("begin inner load :" + path);

        //        int assetBundleSize = 0;
        //        AssetBundleCreateRequest bundleRequest = null;
        //        yield return StartCoroutine(ResourceCommon._CreateFromMemory(path, delegate(AssetBundleCreateRequest request, int size)
        //        {
        //            bundleRequest = request;
        //            assetBundleSize = size;
        //        }));

        //        //AssetBundle assetBundle = bundleRequest.assetBundle;

        //        //if (null == assetBundle)//如果发生,有可能是刚好同步过程创建assetbundle刚好完成了.检测到再处理吧。。。
        //        //    DebugEx.LogError("the assetbundle create error!!! and the asset is : " + path, "CreateAssetBunlde", true);

        //        //string name = ResourceCommon.getFileName(path, true);

        //        //AssetExtraInfo mAssetExtraInfo = mAssetExtraInfoManager.getAssetExtraInfo(name);

        //        //List<Object> mHasLoaded = new List<Object>();
        //        //operation.mAllDependencesAssetSize = mAssetExtraInfo.mAllDependencesAssetSize;

        //        //foreach (string asset in mAssetExtraInfo.mAllDps)
        //        //{
        //        //    string completeName = ResourceCommon.getFileName(asset, true);
        //        //    if (mLoadedResourceUnit.ContainsKey(completeName))
        //        //    {
        //        //        mHasLoaded.Add((mLoadedResourceUnit[completeName] as ResourceUnit).Asset);
        //        //    }
        //        //}

        //        //if (0 != mAssetExtraInfo.mAllDps.Count)
        //        //    yield return StartCoroutine(loadDependenceAsset(mAssetExtraInfo.mAllDps, ResourceType.ASSET, operation));

        //        //AssetBundleRequest abr = null;
        //        //yield return StartCoroutine(ResourceCommon.LoadAsyncFromAssetBundle(assetBundle, name, typeof(Object), delegate(AssetBundleRequest request)
        //        //{
        //        //    abr = request;
        //        //}));

        //        //if (ResourceType.ASSET == resourceType)
        //        //{
        //        //    if (null != mHasLoaded)
        //        //    {
        //        //        foreach (Object asset in mHasLoaded)
        //        //        {
        //        //            ////处理材质
        //        //            if (abr.asset.GetType() == typeof(Material) &&
        //        //                (asset.GetType() == typeof(Texture) || asset.GetType() == typeof(Texture2D) || asset.GetType() == typeof(Texture3D)))
        //        //            {
        //        //                Material mat = abr.asset as Material;
        //        //                if (!mAssetExtraInfo.mhandleDeleteAssetBundleExtraInfo.mAllTextureNameInShader.ContainsKey(asset.name))
        //        //                    DebugEx.LogError("the asset : " + asset.name + " dependenced by : " + path + " but the asset is not exist in mAllTextureNameInShader, so maybe is your material and shader is not match!!!", ResourceCommon.DEBUGTYPENAME);
        //        //                mat.SetTexture(mAssetExtraInfo.mhandleDeleteAssetBundleExtraInfo.mAllTextureNameInShader[asset.name], asset as Texture);
        //        //            }
        //        //        }
        //        //    }
        //        //}

        //        //ResourceUnit resource = new ResourceUnit(mAssetExtraInfo.mCanDeleteAssetbundle || (mAssetExtraInfo.mSingleDependenced && mAssetExtraInfo.mNoUpeerDependenced) ? null : assetBundle, assetBundleSize, abr.asset, path, resourceType, mAssetExtraInfo.mAllDependencesAssetSize);
        //        //if (null != mAssetExtraInfo.mAllDps)
        //        //{
        //        //    foreach (string p in mAssetExtraInfo.mAllDps)
        //        //    {
        //        //        string completeName = ResourceCommon.getFileName(p, true);
        //        //        resource.NextLevelAssets.Add(mLoadedResourceUnit[completeName] as ResourceUnit);
        //        //    }
        //        //}

        //        //resource.addReferenceCount();
        //        //mLoadedResourceUnit.Add(name, resource);
        //        //ResourceCommon.Log("mLoadedResourceUnit.Add : " + path);

        //        //if (ResourceType.PREFAB == resource.resourceType)
        //        //{
        //        //    GameObject prefab = abr.asset as GameObject;
        //        //    if (null == prefab)
        //        //        Debug.LogError("the : " + path + " load asset failed , maybe you set a error ResourceType!!!");
        //        //    prefab.AddComponent<ResourceUnitMonitor>();
        //        //}

        //        ////if (mAssetExtraInfo.mCanDeleteAssetbundle || (mAssetExtraInfo.mSingleDependenced && mAssetExtraInfo.mNoUpeerDependenced))
        //        ////    assetBundle.Unload(false);
        //        ////foreach (AssetBundle bundle in mTempAssetBundles)
        //        ////    bundle.Unload(false);
        //        ////mTempAssetBundles.Clear();

        //        //handleResponse();

        //        //operation.mLoadDependencesAssetSize += 1;
        //        //if (operation.mLoadDependencesAssetSize != operation.mAllDependencesAssetSize)
        //        //    DebugEx.LogError("mLoadDependencesAssetSize and mAllDependencesAssetSize are not match in asset :" + path, ResourceCommon.DEBUGTYPENAME);
        //        //operation.mComplete = true;
        //        //operation.mResource = resource;

        //        //ResourceCommon.Log("end inner load :" + path);

        //        //if (null != handle)
        //        //    handle(resource);
        //    }
        //    else
        //    {
        //        yield return null;

        //        Object asset = Resources.Load(path);
        //        ResourceUnit resource = new ResourceUnit(null, 0, asset, path, resourceType);

        //        handleResponse();

        //        operation.mComplete = true;
        //        operation.mResource = resource;

        //        if (null != handle)
        //            handle(resource);
        //    }
        //}

        private IEnumerator _loadLevel(string path, HandleFinishLoadLevel handle, ResourceType resourceType, ResourceAsyncOperation operation)
        {
            //使用assetbundle打包
            if (UsedAssetBundle)
            {
                //根据场景名称获取asset信息
                AssetInfo sceneAssetInfo = mAssetInfoManager.GetAssetInfo(path);
                //获取该包总大小
                operation.mAllDependencesAssetSize = mAssetInfoManager.GetAllAssetSize(sceneAssetInfo);

                //获取依赖的asset的索引
                foreach (int index in sceneAssetInfo.mDependencys)
                {
                    //根据索引获取依赖的Asset
                    AssetInfo depencyAsset = mAssetInfoManager.GetAssetInfo(index);
                    string depencyAssetName = depencyAsset.mName;

                    //加载场景依赖assetbundle
                    ResourceUnit unit = _LoadImmediate(depencyAssetName, ResourceType.LEVEL);
                    operation.mLoadDependencesAssetSize += unit.AssetBundleSize;
                }

                //加载场景assetbundle     
                int scenAssetBundleSize = 0;
                byte[] binary = ResourceCommon.getAssetBundleFileBytes(path, ref scenAssetBundleSize);
                AssetBundle assetBundle = AssetBundle.CreateFromMemoryImmediate(binary);
                if (!assetBundle)
                    DebugEx.LogError("create scene assetbundle " + path + "in _LoadImmediate failed");

                //添加场景大小
                operation.mLoadDependencesAssetSize += scenAssetBundleSize;

                AsyncOperation asyncOperation = Application.LoadLevelAsync(ResourceCommon.getFileName(path, false));
                operation.asyncOperation = asyncOperation;
                yield return asyncOperation;

                ////levelAssetBundle.Unload(false);
                //levelAssetBundle = null;

                ////foreach (AssetBundle bundle in mTempAssetBundles)
                ////    bundle.Unload(false);
                ////mTempAssetBundles.Clear();

                ////Resources.UnloadUnusedAssets();

                handleResponse();

                //operation.mLoadDependencesAssetSize += 1;
                //if (operation.mLoadDependencesAssetSize != operation.mAllDependencesAssetSize)
                //    DebugEx.LogError("mLoadDependencesAssetSize and mAllDependencesAssetSize are not match in asset :" + path, ResourceCommon.DEBUGTYPENAME);
                operation.asyncOperation = null;
                operation.mComplete = true;
                operation.mResource = null;

                //ResourceCommon.Log("end inner loadLevel :" + path);

                if (null != handle)
                    handle();


                //ResourceCommon.Log("begin inner loadLevel :" + path);

                //int assetBundleSize = 0;
                //AssetBundleCreateRequest bundleRequest = null;
                //yield return StartCoroutine(ResourceCommon._CreateFromMemory(path + "_depsinfo", delegate(AssetBundleCreateRequest request, int size)
                //{
                //    bundleRequest = request;
                //    assetBundleSize = size;
                //}));

                //AssetBundle assetBundle = bundleRequest.assetBundle;

                //List<string> deps = null;

                //string name = ResourceCommon.getFileName(path, true);

                //AssetBundleRequest scenedps_abr = assetBundle.LoadAsync(name + "_depsinfo", typeof(TextAsset));
                //yield return scenedps_abr;
                //TextAsset scenedps = scenedps_abr.asset as TextAsset;

                //if (scenedps != null)
                //{
                //    deps = new List<string>();
                //    parseLevelDependence(scenedps.text, ref operation.mAllDependencesAssetSize, deps);

                //    ResourceCommon.Log("inner loadLevel , the level is : " + path + "\n the mAllDependencesAssetSize : " + operation.mAllDependencesAssetSize + "\n the deps is :" + scenedps.text);

                //    if (0 != deps.Count)
                //        yield return StartCoroutine(loadDependenceAsset(deps, ResourceType.LEVELASSET, operation));
                //}
                //else
                //{
                //    DebugEx.LogError("load " + path + " assetBundle depsFileName error!!!", ResourceCommon.DEBUGTYPENAME);
                //}
                ////assetBundle.Unload(true);
                //ResourceUnit resource = new ResourceUnit(null, assetBundleSize, null, path, resourceType, operation.mAllDependencesAssetSize);

                //if (null != deps)
                //{
                //    foreach (string p in deps)
                //    {
                //        string completeName = ResourceCommon.getFileName(p, true);
                //        resource.NextLevelAssets.Add(mLoadedResourceUnit[completeName] as ResourceUnit);
                //    }
                //}
                //resource.addReferenceCount();
                //mLoadedResourceUnit.Add(name, resource);
                //ResourceCommon.Log("mLoadedResourceUnit.Add : " + path);

                //yield return StartCoroutine(ResourceCommon._CreateFromMemory(path, delegate(AssetBundleCreateRequest request, int size)
                //{
                //    bundleRequest = request;
                //    assetBundleSize = size;
                //}));

                //AssetBundle levelAssetBundle = bundleRequest.assetBundle;
                //AsyncOperation asyncOperation = Application.LoadLevelAsync(ResourceCommon.getFileName(path, false));
                //operation.asyncOperation = asyncOperation;
                //yield return asyncOperation;

                ////levelAssetBundle.Unload(false);
                //levelAssetBundle = null;

                ////foreach (AssetBundle bundle in mTempAssetBundles)
                ////    bundle.Unload(false);
                ////mTempAssetBundles.Clear();

                ////Resources.UnloadUnusedAssets();

                //handleResponse();

                //operation.mLoadDependencesAssetSize += 1;
                //if (operation.mLoadDependencesAssetSize != operation.mAllDependencesAssetSize)
                //    DebugEx.LogError("mLoadDependencesAssetSize and mAllDependencesAssetSize are not match in asset :" + path, ResourceCommon.DEBUGTYPENAME);
                //operation.asyncOperation = null;
                //operation.mComplete = true;
                //operation.mResource = resource;

                //ResourceCommon.Log("end inner loadLevel :" + path);

                //if (null != handle)
                //    handle();
            }
            //不使用
            else
            {
                ResourceUnit level = new ResourceUnit(null, 0, null, path, resourceType);

                //获取加载场景名称
                string sceneName = ResourceCommon.getFileName(path, true);
                AsyncOperation asyncOperation = Application.LoadLevelAsync(sceneName);
                operation.asyncOperation = asyncOperation;
                yield return asyncOperation;

                handleResponse();

                operation.asyncOperation = null;
                operation.mComplete = true;

                if (null != handle)
                    handle();
            }
        }

        //单个资源加载
        ResourceUnit _LoadImmediate(string fileName, ResourceType resourceType)
        {
            //没有该资源，加载
            if (!mLoadedResourceUnit.ContainsKey(fileName))
            {
                //资源大小
                int assetBundleSize = 0;
                byte[] binary = ResourceCommon.getAssetBundleFileBytes(fileName, ref assetBundleSize);
                AssetBundle assetBundle = AssetBundle.CreateFromMemoryImmediate(binary);
                if (!assetBundle)
                    DebugEx.LogError("create assetbundle " + fileName + "in _LoadImmediate failed");

                Object asset = assetBundle.Load(fileName);
                if (!asset)
                    DebugEx.LogError("load assetbundle " + fileName + "in _LoadImmediate failed");


                //调试用
                //DebugEx.LogError("load asset bundle:" + fileName);

                ResourceUnit ru = new ResourceUnit(assetBundle, assetBundleSize, asset, fileName, resourceType);

                //添加到资源中
                mLoadedResourceUnit.Add(fileName, ru);

                return ru;
            }
            else
            {
                return mLoadedResourceUnit[fileName];
            }
        }







        //private void parseLevelDependence(string text, ref int allDependencesAssetSize, List<string> deps)
        //{
        //    int index = 0;
        //    int subLeft = 0;
        //    int subRight = text.IndexOf(";");
        //    while (-1 != subRight)
        //    {
        //        string path = text.Substring(subLeft, subRight - subLeft);
        //        if (0 == index)
        //        {
        //            allDependencesAssetSize = Convert.ToInt32(path);
        //        }
        //        else
        //        {
        //            deps.Add(path);
        //        }
        //        subLeft = subRight + 1;
        //        subRight = text.IndexOf(";", subLeft);
        //        ++index;
        //    }
        //}

        //private void loadDependenceAssetImmediate(List<string> dependences, ResourceType type)
        //{
        //    foreach (string path in dependences)
        //    {
        //        string completeName = ResourceCommon.getFileName(path, true);
        //        if (mLoadedResourceUnit.ContainsKey(completeName))
        //        {
        //            continue;
        //        }

        //        int assetBundleSize = 0;
        //        AssetBundle assetBundle = AssetBundle.CreateFromMemoryImmediate(ResourceCommon.getAssetBundleFileBytes(path, ref assetBundleSize));

        //        if (null == assetBundle)//如果发生,有可能是刚好异步创建assetbundle刚好完成了.检测到再处理吧。。。
        //            DebugEx.LogError("the assetbundle create error!!! and the asset is : " + path, "CreateAssetBunlde", true);

        //        AssetExtraInfo mAssetExtraInfo = mAssetExtraInfoManager.getAssetExtraInfo(completeName);

        //        List<Object> mHasLoaded = new List<Object>();

        //        foreach (string assetdep in mAssetExtraInfo.mAllDps)
        //        {
        //            string depcompleteName = ResourceCommon.getFileName(assetdep, true);
        //            if (mLoadedResourceUnit.ContainsKey(depcompleteName))
        //            {
        //                mHasLoaded.Add((mLoadedResourceUnit[depcompleteName] as ResourceUnit).Asset);
        //            }
        //        }

        //        if (mAssetExtraInfo.mNoUpeerDependenced)
        //            DebugEx.LogError("mNoUpperDependenced asset apper in loadDependenceAsset is impossible!!!", ResourceCommon.DEBUGTYPENAME);

        //        loadDependenceAssetImmediate(mAssetExtraInfo.mAllDps, type);

        //        Object asset = assetBundle.Load(completeName);

        //        //Debug.Log("dependency" + completeName);

        //        foreach (Object loadedAsset in mHasLoaded)
        //        {
        //            ////处理材质
        //            if (asset.GetType() == typeof(Material) &&
        //                (loadedAsset.GetType() == typeof(Texture) || loadedAsset.GetType() == typeof(Texture2D) || loadedAsset.GetType() == typeof(Texture3D)))
        //            {
        //                Material mat = asset as Material;
        //                if (!mAssetExtraInfo.mhandleDeleteAssetBundleExtraInfo.mAllTextureNameInShader.ContainsKey(loadedAsset.name))
        //                    DebugEx.LogError("the asset : " + loadedAsset.name + " dependenced by : " + path + " but the asset is not exist in mAllTextureNameInShader, so maybe is your material and shader is not match!!!", ResourceCommon.DEBUGTYPENAME);
        //                else
        //                    mat.SetTexture(mAssetExtraInfo.mhandleDeleteAssetBundleExtraInfo.mAllTextureNameInShader[loadedAsset.name], loadedAsset as Texture);
        //            }
        //        }

        //        ResourceUnit ru = new ResourceUnit((mAssetExtraInfo.mCanDeleteAssetbundle || mAssetExtraInfo.mSingleDependenced) ? null : assetBundle, assetBundleSize, asset, path, type, mAssetExtraInfo.mAllDependencesAssetSize);
        //        if (null != mAssetExtraInfo.mAllDps)
        //        {
        //            foreach (string p in mAssetExtraInfo.mAllDps)
        //            {
        //                string depcompleteName = ResourceCommon.getFileName(p, true);
        //                ru.NextLevelAssets.Add(mLoadedResourceUnit[depcompleteName] as ResourceUnit);
        //            }
        //        }

        //        if (mAssetExtraInfo.mCanDeleteAssetbundle || mAssetExtraInfo.mSingleDependenced)
        //            mTempAssetBundles.Add(assetBundle);

        //        mLoadedResourceUnit.Add(completeName, ru);
        //        ResourceCommon.Log("mLoadedResourceUnit.Add : " + path);
        //    }
        //}

        //private IEnumerator loadDependenceAsset(List<string> dependences, ResourceType type, ResourceAsyncOperation operation)
        //{
        //    foreach (string path in dependences)
        //    {
        //        string completeName = ResourceCommon.getFileName(path, true);
        //        if (mLoadedResourceUnit.ContainsKey(completeName))
        //        {
        //            operation.mLoadDependencesAssetSize += (mLoadedResourceUnit[completeName] as ResourceUnit).AllDependencesAssetSize;
        //            continue;
        //        }

        //        int assetBundleSize = 0;
        //        AssetBundleCreateRequest bundleRequest = null;
        //        yield return StartCoroutine(ResourceCommon._CreateFromMemory(path, delegate(AssetBundleCreateRequest request, int size)
        //        {
        //            bundleRequest = request;
        //            assetBundleSize = size;
        //        }));

        //        AssetBundle assetBundle = bundleRequest.assetBundle;

        //        if (null == assetBundle)//如果发生,有可能是刚好同步过程创建assetbundle刚好完成了.检测到再处理吧。。。
        //            DebugEx.LogError("the assetbundle create error!!! and the asset is : " + path, "CreateAssetBunlde", true);

        //        AssetExtraInfo mAssetExtraInfo = mAssetExtraInfoManager.getAssetExtraInfo(completeName);

        //        List<Object> mHasLoaded = new List<Object>();

        //        foreach (string asset in mAssetExtraInfo.mAllDps)
        //        {
        //            string depcompleteName = ResourceCommon.getFileName(asset, true);
        //            if (mLoadedResourceUnit.ContainsKey(depcompleteName))
        //            {
        //                mHasLoaded.Add((mLoadedResourceUnit[depcompleteName] as ResourceUnit).Asset);
        //            }
        //        }

        //        if (mAssetExtraInfo.mNoUpeerDependenced)
        //            DebugEx.LogError("mNoUpperDependenced asset apper in loadDependenceAsset is impossible!!!", ResourceCommon.DEBUGTYPENAME);

        //        if (0 != mAssetExtraInfo.mAllDps.Count)
        //            yield return StartCoroutine(loadDependenceAsset(mAssetExtraInfo.mAllDps, type, operation));

        //        AssetBundleRequest abr = null;
        //        yield return StartCoroutine(ResourceCommon.LoadAsyncFromAssetBundle(assetBundle, completeName, typeof(Object), delegate(AssetBundleRequest request)
        //        {
        //            abr = request;
        //        }));

        //        if (mHasLoaded != null)
        //        {
        //            foreach (Object asset in mHasLoaded)
        //            {
        //                ////处理材质
        //                if (abr.asset.GetType() == typeof(Material) &&
        //                    (asset.GetType() == typeof(Texture) || asset.GetType() == typeof(Texture2D) || asset.GetType() == typeof(Texture3D)))
        //                {
        //                    Material mat = abr.asset as Material;
        //                    if (!mAssetExtraInfo.mhandleDeleteAssetBundleExtraInfo.mAllTextureNameInShader.ContainsKey(asset.name))
        //                        DebugEx.LogError("this is impossible,the deps in runtiume not match when build. the asset is :" + path, ResourceCommon.DEBUGTYPENAME);
        //                    mat.SetTexture(mAssetExtraInfo.mhandleDeleteAssetBundleExtraInfo.mAllTextureNameInShader[asset.name], asset as Texture);
        //                }
        //            }
        //        }

        //        ResourceUnit ru = new ResourceUnit((mAssetExtraInfo.mCanDeleteAssetbundle || mAssetExtraInfo.mSingleDependenced) ? null : assetBundle, assetBundleSize, abr.asset, path, type, mAssetExtraInfo.mAllDependencesAssetSize);
        //        if (null != mAssetExtraInfo.mAllDps)
        //        {
        //            foreach (string p in mAssetExtraInfo.mAllDps)
        //            {
        //                string depcompleteName = ResourceCommon.getFileName(p, true);
        //                ru.NextLevelAssets.Add(mLoadedResourceUnit[depcompleteName] as ResourceUnit);
        //            }
        //        }

        //        if (mAssetExtraInfo.mCanDeleteAssetbundle || mAssetExtraInfo.mSingleDependenced)
        //            mTempAssetBundles.Add(assetBundle);

        //        mLoadedResourceUnit.Add(completeName, ru);
        //        ResourceCommon.Log("mLoadedResourceUnit.Add : " + path);

        //        operation.mLoadDependencesAssetSize += 1;
        //    }
        //}

        ////处理文本文件
        //public static bool Exist(string path)
        //{
        //    string localPath = string.Format("{0}/{1}", Application.persistentDataPath, path + ResourceCommon.assetbundleFileSuffix);
        //    if (!File.Exists(localPath))//原始包中
        //    {
        //        TextAsset text = Resources.Load(path) as TextAsset;
        //        if (null != text)
        //            return true;
        //        return false;
        //    }
        //    else//沙箱环境中
        //    {
        //        return true;
        //    }
        //}

        public static Stream Open(string path)
        {
            string localPath;
            //Andrio跟IOS环境使用沙箱目录
            if (Application.platform == RuntimePlatform.Android || Application.platform == RuntimePlatform.IPhonePlayer)
            {
                localPath = string.Format("{0}/{1}", Application.persistentDataPath, path + ResourceCommon.assetbundleFileSuffix);
            }
            //Window下使用assetbunlde资源目录
            else
            {
                localPath = ResourceCommon.assetbundleFilePath + path + ResourceCommon.assetbundleFileSuffix;
            }

            //首先检查沙箱目录中是否有更新资源
            if (File.Exists(localPath))
            {
                return File.Open(localPath, FileMode.Open, FileAccess.Read, FileShare.Read);
            }
            //没有的话原始包中查找
            else
            {
                TextAsset text = Resources.Load(path) as TextAsset;
                if (null == text)
                    DebugEx.LogError("can not find : " + path + " in OpenText", ResourceCommon.DEBUGTYPENAME);
                return new MemoryStream(text.bytes);
            }


            //if (!File.Exists(localPath))//原始包中
            //{
            //    TextAsset text = Resources.Load(path) as TextAsset;
            //    if (null == text)
            //        Debug.LogError("can not find : " + path + " in OpenText", ResourceCommon.DEBUGTYPENAME);
            //    return new MemoryStream(text.bytes);
            //}
            //else//沙箱环境中
            //{
            //    return File.Open(localPath, FileMode.Open, FileAccess.Read, FileShare.Read);
            //}
        }

        public static StreamReader OpenText(string path)
        {
            return new StreamReader(Open(path), System.Text.Encoding.Default);
        }

        //public static bool Exist(string path)
        //{
        //    string localPath = string.Format("{0}/{1}", ResourceCommon.textFilePath, path);

        //    if (Application.platform == RuntimePlatform.Android)
        //    {
        //        WWW www = new WWW(localPath);
        //        while (!www.isDone) { }

        //        return string.IsNullOrEmpty(www.error);
        //    }

        //    return File.Exists(localPath);
        //}

        //public static Stream Open(string path)
        //{
        //    string localPath = "";
        //    if (Application.platform == RuntimePlatform.Android || Application.platform == RuntimePlatform.IPhonePlayer)
        //    {
        //        localPath = string.Format("{0}/{1}", ResourceCommon.textFilePath, path);
        //    }
        //    else if (Application.platform == RuntimePlatform.WindowsEditor || Application.platform == RuntimePlatform.WindowsPlayer)
        //    {
        //        localPath = string.Format("file://{0}/{1}", ResourceCommon.textFilePath, path);
        //    }

        //    if (Application.platform == RuntimePlatform.IPhonePlayer)
        //    {
        //        return File.Open(localPath, FileMode.Open, FileAccess.Read, FileShare.Read);
        //    }

        //    WWW www = new WWW(localPath);
        //    while (!www.isDone) { }

        //    if (!string.IsNullOrEmpty(www.error))
        //    {
        //        Debug.LogError(www.error);
        //    }

        //    return new MemoryStream(www.bytes);
        //}

        //public static StreamReader OpenText(string path)
        //{
        //    Stream stream = Open(path);
        //    return new StreamReader(stream, System.Text.Encoding.Default);
        //}
    }
}