using System;
using System.IO;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using System.Security.Cryptography;
using System.Text;
using UnityEditor;
using Object = UnityEngine.Object;
using BlGame.Resource;

//资源assetbundle打包功能
public class BuildAssetBundle
{
    //assetbundle资源
    static string assetFilePath = "/assetbundles";
    static string buildCfgFilePath = Application.dataPath + "/Editor/Build/build.txt";
    static string levelCfgFilePath = Application.dataPath + "/Editor/Build/level.txt";
    static string buildResourceCfg = Application.dataPath + "/Editor/Build/Resource.txt";

    //打包环境设置
    static BuildAssetBundleOptions options = BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.CompleteAssets | BuildAssetBundleOptions.DeterministicAssetBundle;
    static BuildTarget buildPlatform = BuildTarget.StandaloneWindows;

    //保存所有的scene信息
    static List<string> mScenes = new List<string>();

    //保存所有Resource信息
    static List<string> mResources = new List<string>();

    //保存所有的Asset信息 场景+Resource
    static Dictionary<int, Dictionary<string, AssetUnit>> allLevelAssets = new Dictionary<int, Dictionary<string, AssetUnit>>();

    //Andriod打包
    [MenuItem("Build/BuildAndriod")]
    public static void BuildAndriod()
    {
        buildPlatform = BuildTarget.Android;
        BuildResourceFromUnityRule();
    }

    //IOS打包
    [MenuItem("Build/BuildIOS")]
    public static void BuildIOS()
    {
        buildPlatform = BuildTarget.iPhone;
        BuildResourceFromUnityRule();
    }

    //Windows打包
    [MenuItem("Build/BuildWindows")]
    public static void BuildWindows()
    {
        buildPlatform = BuildTarget.StandaloneWindows;
        BuildResourceFromUnityRule();
    }


    
    public static void BuildResourceFromUnityRule()
    {
        //清空数据
        if (Directory.Exists(ResourceCommon.assetbundleFilePath))
            Directory.Delete(ResourceCommon.assetbundleFilePath, true);

        //刷新数据
        Caching.CleanCache();
        AssetDatabase.Refresh();

        //获取资源信息
        GetBuildScenes();
        GetBuildResources();

        //获取所有的打包asset
        GetAllAssets();

        //保存Asset资源信息
        DumpAssetInfoFile();

        //打包asset
        BuildResource();

        //保存资源配置assetbundle
        DumpResourceFile();

        //根据所有assetbundle文件生成版本信息
        DumpVersionFile();

        //根据生成的assetbundle文件大小填充 assetInfo.byte文件 
        AddAssetSizeToAssetInfoFile();

        AssetDatabase.Refresh();
        DebugEx.LogError("BuildResource finish", "BuildResource", true);
    }

    //获取需要打包Resources目录下所有资源信息
    [MenuItem("Build/Tools/GetBuildResources")]
    public static void GetBuildResources()
    {
        mResources.Clear();

        //Resource资源路径
        string resourcePath = Application.dataPath + "/Resources/";

        string[] files = Directory.GetFiles(resourcePath, "*.*", SearchOption.AllDirectories);
        foreach (string file in files)
        {
            string suffix = BuildCommon.getFileSuffix(file);
            if (suffix == "meta")
                continue;
            string realFile = file.Replace("\\", "/");
            realFile = realFile.Replace(Application.dataPath, "Assets");
			
			if (realFile == "Assets/Resources/Version.bytes")
				continue;
			
            mResources.Add(realFile);
        }
    }


    //获取选中需要打包的Resources资源信息
    [MenuItem("Build/Tools/GetSelectedBuildResources")]
    public static void GetSelectedBuildResources()
    {
        mResources.Clear();

        //Resource资源路径
        //string resourcePath = Application.dataPath + "/Resources/";

        //string[] files = Directory.GetFiles(resourcePath, "*.*", SearchOption.AllDirectories);
        //foreach (string file in files)
        //{
        //    string suffix = BuildCommon.getFileSuffix(file);
        //    if (suffix == "meta" || suffix == "cs")
        //        continue;
        //    string realFile = file.Replace("\\", "/");
        //    realFile = realFile.Replace(Application.dataPath, "Assets");
        //    mResources.Add(realFile);
        //}
        Object[] selection = Selection.GetFiltered(typeof(Object), SelectionMode.DeepAssets);
        if (selection.Length == 0)
            return;

        foreach (Object obj in selection)
        {
            string assetPath = AssetDatabase.GetAssetPath(obj);
            mResources.Add(assetPath);


            Debug.Log("select:" + assetPath);
        }
    }



    //获取需要打包的Scene目录下的所有资源信息
    public static void GetBuildScenes()
    {
        mScenes.Clear();

        //获取场景路径
        FileStream fs = new FileStream(levelCfgFilePath, FileMode.Open);
        StreamReader sr = new StreamReader(fs);
        string path = "";
        while (path != null)
        {
            if (path != "")
            {
                mScenes.Add(path);
            }
            path = sr.ReadLine();
        }
        sr.Close();
        fs.Close();
    }


    [MenuItem("Build/Tools/TestSceneAssets")]
    public static void TestSceneAssets()
    {
        string sceneName = "Assets/Scenes/1.unity";

        string[] depends = AssetDatabase.GetDependencies(new string[] { sceneName });
        foreach (string str in depends)
        {
            Debug.Log("depency: " + str);
        }
    }

    //获取所有的Asset
    [MenuItem("Build/Tools/GetAllAssets")]
    public static void GetAllAssets()
    {
        //清空操作
        allLevelAssets.Clear();

        List<string> allAssetPath = new List<string>();
        //1添加场景路径
       
        foreach (string scene in mScenes)
        {
            allAssetPath.Add(scene);
        }

        //2添加Resource资源路径
        foreach(string resrPath in mResources)
        {
            allAssetPath.Add(resrPath);
        }
       
        //获取场景以及预制件中所有的资源信息
        string[] allExportAssets = AssetDatabase.GetDependencies(allAssetPath.ToArray());


        Dictionary<string, AssetUnit> allFiles = new Dictionary<string, AssetUnit>();
        foreach (string p in allExportAssets)
        {
            //if (p.Contains(".dll") || p.Contains(".cs"))
            if (p.Contains(".dll"))
                continue;

            AssetUnit unit = new AssetUnit(p);
          
            //Asset等级
            int level = unit.mLevel;

            //存在
            if (allLevelAssets.ContainsKey(level))
            {
                allLevelAssets[level].Add(p, unit);
            }
            else 
            {
                //添加等级
                Dictionary<string, AssetUnit> levelAsset = new Dictionary<string, AssetUnit>();
                allLevelAssets.Add(level,levelAsset);
                //添加asset信息
                allLevelAssets[level].Add(p, unit);
            }
        }

        //创建Asset索引
        BuildAssetUnitIndex();
    }

    [MenuItem("Build/Tools/CheckResources")]
    public static void CheckResources()
    {
         AssetDatabase.Refresh();

        //获取资源信息
        GetBuildScenes();
        GetBuildResources();

        CheckAssetValid();

        Debug.Log("Check finised");
    }

   
    [MenuItem("Build/Tools/BuildSelected")]
    public static void BuildSelected()
    {
        //清空数据
        if (Directory.Exists(ResourceCommon.assetbundleFilePath))
            Directory.Delete(ResourceCommon.assetbundleFilePath, true);

        //刷新数据
        Caching.CleanCache();
        AssetDatabase.Refresh();

        //获取资源信息
        GetBuildScenes();
        GetSelectedBuildResources();

        //获取所有的打包asset
        GetAllAssets();

        //保存Asset资源信息
        DumpAssetInfoFile();

        //打包asset
        BuildResource();

        //保存资源配置assetbundle
        DumpResourceFile();

        //根据所有assetbundle文件生成版本信息
        DumpVersionFile();

        //根据生成的assetbundle文件大小填充 assetInfo.byte文件 
        AddAssetSizeToAssetInfoFile();

        AssetDatabase.Refresh();
        DebugEx.LogError("BuildResource finish", "BuildResource", true);
    }



    public static void BuildResource()
    {
        AssetDatabase.Refresh();

        //执行依赖性打包

        //资源最大等级
        int maxLevel = allLevelAssets.Count;
        if (maxLevel == 0)
            return;

        //从最低等级开始打包
        for (int level = 1; level <= maxLevel; level++)
        {
            BuildPipeline.PushAssetDependencies();
       
            //获取不同等级的aaset
            Dictionary<string, AssetUnit> levelAssets = allLevelAssets[level];
           
            //遍历该等级的所有asset打包
            foreach(KeyValuePair<string, AssetUnit> pair in levelAssets)
            {
                //根据路径获取asset资源
                Object asset = AssetDatabase.LoadMainAssetAtPath(pair.Value.mPath);
                if (null == asset)
                    DebugEx.LogError("load " + pair.Value.mPath + " failed!!!", "BuildResource", true);

                //生成打包保存路径
                string savePath = pair.Value.mPath.Insert(6, assetFilePath) + ResourceCommon.assetbundleFileSuffix;
                BuildCommon.CheckFolder(BuildCommon.getPath(savePath));

                //打包名称去Asset
               


                //普通资源
                if (pair.Value.mSuffix != "unity")//普通资源
                {
                    string assetName = pair.Value.mPath.Replace("Assets/", "");

                    //资源打包
                    if (!BuildPipeline.BuildAssetBundleExplicitAssetNames(
                               new Object[] { asset }, new string[] { assetName }, savePath, options, buildPlatform))
                        DebugEx.LogError("build assetbundle " + savePath + " failed!!!", "BuildResource", true);
                }
                //场景资源，没有依赖场景的
                else 
                {
                    AssetDatabase.Refresh();
                    BuildPipeline.PushAssetDependencies();
                    string error = BuildPipeline.BuildStreamedSceneAssetBundle(new string[]{pair.Value.mPath}, savePath, buildPlatform);
                    if (error != "")
                        DebugEx.LogError(error, "BuildResource", true);
                    BuildPipeline.PopAssetDependencies();

                    Debug.Log("scene path" + pair.Value.mPath);
                    //pair.Value.mPath
                }
            }   
        }

        //popdepency依赖
        for (int level = 1; level <= maxLevel; level++)
        {
            BuildPipeline.PopAssetDependencies();
        }
    }


    //创建Asset资源信息
    public static void DumpAssetInfoFile()
    {
        if(allLevelAssets.Count ==0)
            return;

        ////创建所有资源Asset列表
        XmlDocument doc = new XmlDocument();
        XmlElement root = doc.CreateElement("AllAssets");

        //遍历所有Asset数据
        for (int level = 1; level <= allLevelAssets.Count; level++)
        {
            Dictionary<string, AssetUnit> levelAssets = allLevelAssets[level];
            foreach (KeyValuePair<string, AssetUnit> asset in levelAssets)
            {
                string assetName = asset.Key;
                AssetUnit assetUnit = asset.Value;

                XmlElement ele = doc.CreateElement("Asset");
                
                //设置路径名称
                assetName = assetName.Replace("Assets/", "");
                ele.SetAttribute("name", assetName);
               
                //设置asset索引
                ele.SetAttribute("index", assetUnit.mIndex.ToString());
               
                //设置等级
                ele.SetAttribute("level", level.ToString());

                List<AssetUnit> sortDepencys = new List<AssetUnit>();
                //获取AssetUnit所有依赖，并排序
                List<string> depencys = assetUnit.mAllDependencies;
                foreach (string depency in depencys)
                {
                    AssetUnit depencyUnit = GetAssetUnit(depency);
                    sortDepencys.Add(depencyUnit);
                }

                //排序
                sortDepencys.Sort(SortAssetUnit);
                //保存依赖索引
                string depencystr = "";
                for (int i = 0; i < sortDepencys.Count; i++)
                {
                    AssetUnit unit = sortDepencys[i];

                    if (i != sortDepencys.Count - 1)
                        depencystr += unit.mIndex + ",";
                    else
                        depencystr += unit.mIndex;
                }
                ele.SetAttribute("depency", depencystr.ToString());

                root.AppendChild(ele);
            }
        }

        doc.AppendChild(root);
        BuildCommon.CheckFolder(BuildCommon.getPath(ResourceCommon.assetbundleFilePath));
        doc.Save(ResourceCommon.assetbundleFilePath + "AssetInfo.bytes");

        Debug.Log("CreateAssetInfo success!!!");
    }

    //创建资源索引
    public static void DumpResourceFile()
    {
        ////创建所有资源Asset列表
        XmlDocument doc = new XmlDocument();
        XmlElement root = doc.CreateElement("AllResources");

        XmlElement resource = doc.CreateElement("Resources");
        root.AppendChild(resource);
        foreach (string res in mResources)
        {
            string ex = BuildCommon.getFileSuffix(res);
            string path = res.Replace("Assets/", "");
            path = path.Replace("." + ex, "");

            XmlElement ele = doc.CreateElement("file");
            ele.SetAttribute("name", path);
            ele.SetAttribute("type", ex);
            resource.AppendChild(ele);
        }

        //创建所有需要打包的Level列表
        XmlElement sceneRes = doc.CreateElement("Level");
        root.AppendChild(sceneRes);
        foreach (string scene in mScenes)
        {
            XmlElement ele = doc.CreateElement("file");

            string path = scene.Replace("Assets/", "");
            path = path.Replace(".unity", "");

            ele.SetAttribute("name", path);
            ele.SetAttribute("type", "unity");
            sceneRes.AppendChild(ele);
        }
        doc.AppendChild(root);
        BuildCommon.CheckFolder(BuildCommon.getPath(ResourceCommon.assetbundleFilePath));
        doc.Save(ResourceCommon.assetbundleFilePath + "Resource.bytes");

        Debug.Log("CreateResourceCfg success!!!");
    }


    [MenuItem("Build/Tools/DumpVersionFile")]
    public static void DumpVersionFile()
    {
        List<string> allFiles = new List<string>();
        BuildCommon.GetFiles(ResourceCommon.assetbundleFilePath, allFiles, true);

        XmlDocument doc = new XmlDocument(); 
        XmlElement root = doc.CreateElement("Version");
        root.SetAttribute("Number", "1.0.0");
        root.SetAttribute("Big", "false");

        foreach (string element in allFiles)
        {
            int size = 0;
            string md5 = GetFileMD5(element, ref size);
            XmlElement ele = doc.CreateElement("file");
            string relativePath = element.Replace(Application.dataPath + "/assetbundles/", "");          

            ele.SetAttribute("fpath", relativePath);
            ele.SetAttribute("size", size.ToString());
            ele.SetAttribute("md5", md5);
            root.AppendChild(ele);

            //保存大小信息到AssetUnit中
            string assetName = "Assets/" + relativePath;
            assetName = assetName.Substring(0, assetName.Length - 6);
            AssetUnit assetUnit = GetAssetUnit(assetName);
            if(assetUnit != null)
                assetUnit.mAssetSize = size;
        }

        doc.AppendChild(root);
        string assetBundleVersionPath = ResourceCommon.assetbundleFilePath + "Version.bytes";
        doc.Save(assetBundleVersionPath);

        //拷贝到对应的Resource目录
        string resourceVersionPath = Application.dataPath + "/Resources/" + "Version.bytes";
        File.Copy(assetBundleVersionPath, resourceVersionPath, true);

        Debug.Log("Dump Version Success!!!");
    }

    //添加asset信息
    private static void AddAssetSizeToAssetInfoFile()
    {
        //添加asset信息
        XmlDocument xmlDoc = new XmlDocument();
        xmlDoc.Load(ResourceCommon.assetbundleFilePath + "AssetInfo.bytes");

        XmlNodeList xnl = xmlDoc.SelectSingleNode("AllAssets").ChildNodes; 
        for (int i = 0; i < xnl.Count; i++)
        { 
            XmlElement xe = (XmlElement)xnl.Item(i);
            string assetName = xe.GetAttribute("name");

            //获取对应的AssetUnit
            AssetUnit unit = GetAssetUnit("Assets/" + assetName);

            xe.SetAttribute("bundlesize", unit.mAssetSize.ToString());
        }
        xmlDoc.Save(ResourceCommon.assetbundleFilePath + "AssetInfo.bytes");
    }

    private static string GetFileMD5(string fpath, ref int size)
    {
        FileStream fs = new FileStream(fpath, FileMode.Open);
        MD5 md5 = MD5.Create();
        byte[] vals = md5.ComputeHash(fs);
        string ret = BitConverter.ToString(vals);
        ret = ret.Replace("-", "");
        size = (int)fs.Length;
        fs.Close();
        return ret;
    }


    //按等级顺序创建AssetUnit的索引信息
    private static void BuildAssetUnitIndex()
    {
        if (allLevelAssets.Count == 0)
            return;

        int index = 0;
        for (int level = 1; level <= allLevelAssets.Count; level++)
        {
            Dictionary<string, AssetUnit> levelAssets = allLevelAssets[level];
            foreach (KeyValuePair<string, AssetUnit> pair in levelAssets)
            {
                AssetUnit unit = pair.Value;
                unit.mIndex = index;
                
                index++;
            }
        }     
    }

    //根据Asset名称获取对应的AssetUnit
    private static AssetUnit GetAssetUnit(string name)
    {
        if (allLevelAssets.Count == 0)
            return null;

        for (int level = 1; level <= allLevelAssets.Count; level++)
        {
            Dictionary<string, AssetUnit> assetUnit = allLevelAssets[level];

            if (assetUnit.ContainsKey(name))
                return assetUnit[name];
        }

        return null;
    }


     //检查所有的打包资源
     private static void CheckAssetValid()
     {
          //清空操作
        allLevelAssets.Clear();

        List<string> allAssetPath = new List<string>();
        //1添加场景路径
       
        foreach (string scene in mScenes)
        {
            allAssetPath.Add(scene);
        }

        //2添加Resource资源路径
        foreach(string resrPath in mResources)
        {
            allAssetPath.Add(resrPath);
        }
       
        //获取场景以及预制件中所有的资源信息
        string[] allExportAssets = AssetDatabase.GetDependencies(allAssetPath.ToArray());

        List<string> checkAssets = new List<string>();

        foreach (string p in allExportAssets)
        {
            if (p.Contains(".dll"))
                continue;

            string assetName = Path.GetFileName(p);
            assetName = assetName.ToLower();

            if(checkAssets.Contains(assetName))
            {
                DebugEx.Log("the asset " + assetName + "has already exsited");
            }
            else
            {
                checkAssets.Add(assetName);
            }
        }

     }

    //对AssetUnit进行排序
    private static int SortAssetUnit(AssetUnit unit1, AssetUnit unit2)
    {
        int res = 0;
        if (unit1.mLevel > unit2.mLevel)
        {
            res = 1;
        }
        else if (unit1.mLevel < unit2.mLevel)
        {
            res = -1;
        }
        return res;
    }
}
