using UnityEngine;
using UnityEditor;
using System.IO;
using System;
using System.Xml;
using System.Collections.Generic;
using System.Collections;

public class BuildProjectAssets
{
    //public override void Execute()
    //{
    //    var alonePrefabs = Selection.GetFiltered(typeof(GameObject), SelectionMode.DeepAssets);
    //    foreach (var prefab in alonePrefabs)
    //    {
    //        string path = AssetDatabase.GetAssetPath(prefab);
    //        if (path.ToLower().Contains("standardalones/"))
    //        {
    //            this.BuildAloneAssetbundle(path);
    //        }
    //    }
    //}

    //private void BuildAloneAssetbundle(string prefabPath)
    //{
    //    Object prefab = AssetDatabase.LoadMainAssetAtPath(prefabPath);
    //    string[] strs = prefabPath.Split('/');
    //    string floderName = strs[strs.Length - 2];

    //    string path = AssetbundlePath + floderName + "_" + prefab.name + ".assetbundle";
    //    bool success = BuildPipeline.BuildAssetBundle(prefab, null, path, BuildOption, TargetPlatform);
    //    if (success)
    //    {
    //        UnityEngine.Debug.Log("Export " + path + " Success !");
    //    }
    //    else
    //    {
    //        UnityEngine.Debug.Log("Export " + path + " Error !");
    //    }

    //}
    static private void clearBuff()
    {
        texExportList.Clear();
        meshExportList.Clear();
        audioExportList.Clear();
        shaderExportList.Clear();
    }
    static List<Texture> texExportList = new List<Texture>();
    static List<Mesh> meshExportList = new List<Mesh>();
    static List<AudioClip> audioExportList = new List<AudioClip>();
    static List<Shader> shaderExportList = new List<Shader>();
    static void ExportScene(string path, BuildTarget bTarget)
    {
        string platformPath = "";
        switch (bTarget)
        {
            case BuildTarget.WebPlayerStreamed:
            case BuildTarget.WebPlayer:
            case BuildTarget.StandaloneWindows:
                {
                    platformPath = "Windows";
                }
                break;
            case BuildTarget.Android:
                {
                    platformPath = "Android";
                }
                break;
            case BuildTarget.iPhone:
                {
                    platformPath = "Ios";
                }
                break;
        }
        BuildTarget sceneBuildTarget = bTarget;
        Debug.LogError("Export Scene:" + path);
        string SceneName = Path.GetFileNameWithoutExtension(path);
        //获取根节点
        GameObject[] objs = GameObject.FindObjectsOfType<GameObject>();
        GameObject root = null;
        foreach (GameObject o in objs)
        {
            if (o.transform.parent == null)
            {
                if (root != null)
                {
                    EditorUtility.DisplayDialog("warning", "not single root", "ok");
                }
                else
                {
                    root = o;
                }

            }
        }
        String prefabPath = "";
        if (root == null)
        {
            EditorUtility.DisplayDialog("warning", "no root gameobject", "ok");
            return;
        }
        else
        {

        }
        //
        BuildAssetBundleOptions options = BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.DeterministicAssetBundle;
        BuildPipeline.PushAssetDependencies();
        UnityEngine.Object[] refObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { root });
        List<string> depBundleNames = new List<string>();
        //
        //light map
        for (int i = 0; i < 100; ++i)
        {
            string mapPath = "Assets/Scenes/scenes/" + SceneName + "/" + SceneName + "/" + "LightmapFar-" + i + ".exr";
            if (File.Exists(mapPath) == false)
            {
                break;
            }
            UnityEngine.Object lightmap = AssetDatabase.LoadAssetAtPath(mapPath, typeof(UnityEngine.Object));
            if (lightmap != null)
            {
                if (BuildPipeline.BuildAssetBundle(lightmap, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Scene/Texture/" + SceneName + "_" + lightmap.name + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget))
                {
                    Debug.Log("Export Lightmap Success!" + lightmap.name);
                }
                else
                {
                    Debug.LogError("Export Lightmap Failed!" + lightmap.name);
                }
                depBundleNames.Add("Scene/Texture/" + SceneName + "_" + lightmap.name + ".assetbundle");
            }

        }
        //light probe
        string probePath = "Assets/Scenes/scenes/" + SceneName + "/" + SceneName + "/" + "LightProbes.asset";
        if (File.Exists(probePath) == true)
        {
            UnityEngine.Object lightprobe = AssetDatabase.LoadAssetAtPath(probePath, typeof(UnityEngine.Object));
            if (lightprobe != null)
            {
                if (BuildPipeline.BuildAssetBundle(lightprobe, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Scene/Texture/" + SceneName + "_" + "LightProbe.assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget))
                {
                    Debug.Log("Export LightProbe Success!" + SceneName);
                }
                else
                {
                    Debug.LogError("Export LightProbe Failed!" + SceneName);
                }
                depBundleNames.Add("Scene/Texture/" + SceneName + "_" + "LightProbe.assetbundle");
            }
        }
        else
        {
            EditorUtility.DisplayDialog("warning", "can not find lightprob" + probePath, "ok");
        }
        //


        List<string> lightmapIndexList = new List<string>();
        foreach (UnityEngine.Object o in refObjs)
        {
            //导出纹理
            MeshRenderer render = o as MeshRenderer;
            if (render != null && render.sharedMaterial != null)
            {
                lightmapIndexList.Add(render.gameObject.GetInstanceID() + "," + render.lightmapIndex + "," + string.Format("{0},{1},{2},{3}", render.lightmapTilingOffset.x, render.lightmapTilingOffset.y, render.lightmapTilingOffset.z, render.lightmapTilingOffset.w));
                //BlGame.Resource.LightmapHelper[] olds = render.gameObject.GetComponents<BlGame.Resource.LightmapHelper>();
                //foreach (BlGame.Resource.LightmapHelper helper in olds)
                //{
                //    GameObject.DestroyImmediate(helper,true);
                //}
                //BlGame.Resource.LightmapHelper lightMapHelper = render.gameObject.GetComponent<BlGame.Resource.LightmapHelper>();
                //if (lightMapHelper == null)
                //{
                //    lightMapHelper = render.gameObject.AddComponent<BlGame.Resource.LightmapHelper>();
                //}
                //lightMapHelper.lightmapIndex = render.lightmapIndex;
                //Debug.LogError(render.gameObject.name + "lightmapindex:" + render.lightmapIndex);
                //lightMapHelper.x = render.lightmapTilingOffset.x;
                //lightMapHelper.y = render.lightmapTilingOffset.y;
                //lightMapHelper.z = render.lightmapTilingOffset.z;
                //lightMapHelper.w = render.lightmapTilingOffset.w;
                UnityEngine.Object[] renderRefObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { render.sharedMaterial });
                foreach (UnityEngine.Object ro in renderRefObjs)
                {
                    Texture t = ro as Texture;
                    bool exported = false;
                    foreach (Texture tex in texExportList)
                    {
                        if (tex == t)
                        {
                            exported = true;
                        }
                    }
                    if (t != null && exported == false)
                    {

                        // Debug.LogError("Texture:" + AssetDatabase.GetAssetPath(t));
                        BuildPipeline.BuildAssetBundle(t, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Scene/Texture/" + t.name + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                        depBundleNames.Add("Scene/Texture/" + t.name + ".assetbundle");
                        texExportList.Add(t);
                    }
                    //Shader s = ro as Shader;
                    //if (s != null)
                    //{
                    //    Debug.Log("Shader:" + s.name);
                    //}
                }
            }
            ParticleSystem particle = o as ParticleSystem;
            if (particle != null && particle.renderer != null && particle.renderer.sharedMaterials != null)
            {
                foreach (Material m in particle.renderer.sharedMaterials)
                {
                    UnityEngine.Object[] renderRefObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { m });
                    /////////////////////////////////////////////////////////////////////////
                    {
                        Shader s = m.shader;
                        bool exported = false;
                        foreach (Shader shader in shaderExportList)
                        {
                            if (shader == s)
                            {
                                exported = true;
                            }
                        }
                        if (s != null && exported == false)
                        {
                            BuildPipeline.BuildAssetBundle(s, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Shader/" + BuildMethod.getShaderName(s.name) + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                            depBundleNames.Add("Shader/" + BuildMethod.getShaderName(s.name) + ".assetbundle");
                            shaderExportList.Add(s);
                        }
                    }
                    foreach (UnityEngine.Object ro in renderRefObjs)
                    {
                        Texture t = ro as Texture;
                        bool exported = false;
                        foreach (Texture tex in texExportList)
                        {
                            if (tex == t)
                            {
                                exported = true;
                            }
                        }
                        if (t != null && exported == false)
                        {

                            //Debug.LogError("Texture:" + AssetDatabase.GetAssetPath(t));
                            BuildPipeline.BuildAssetBundle(t, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Scene/Texture/" + t.name + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                            depBundleNames.Add("Scene/Texture/" + t.name + ".assetbundle");
                            texExportList.Add(t);
                        }
                    }
                }
            }
            //Terrain terrain = o as Terrain;
            //if (terrain != null && terrain.terrainData != null)
            //{
            //    foreach (SplatPrototype prop in terrain.terrainData.splatPrototypes)
            //    {
            //        Texture2D t = prop.texture;
            //        bool exported = false;
            //        foreach (Texture tex in texExportList)
            //        {
            //            if (tex == t)
            //            {
            //                exported = true;
            //            }
            //        }
            //        if (t != null && exported == false)
            //        {

            //            Debug.LogError("Terrain Texture:" + AssetDatabase.GetAssetPath(t));
            //            BuildPipeline.BuildAssetBundle(t, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Scene/Texture/" + t.name + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
            //            depBundleNames.Add("Scene/Texture/" + t.name + ".assetbundle");
            //            texExportList.Add(t);
            //        }
            //    }
            //}


            //导出模型
            MeshFilter mesh = o as MeshFilter;
            if (mesh != null && mesh.sharedMesh != null)
            {
                bool exported = false;
                foreach (Mesh m in meshExportList)
                {
                    if (m == mesh.sharedMesh)
                    {
                        exported = true;
                    }
                }
                if (exported == false)
                {
                    BuildPipeline.BuildAssetBundle(mesh.sharedMesh, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Scene/Mesh/" + mesh.sharedMesh.name + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                    //Debug.LogError("Mesh:" + AssetDatabase.GetAssetPath(mesh.sharedMesh));
                    depBundleNames.Add("Scene/Mesh/" + mesh.sharedMesh.name + ".assetbundle");
                    meshExportList.Add(mesh.sharedMesh);
                }
            }
            //导出声音
            AudioSource sound = o as AudioSource;
            if (sound != null && sound.clip != null)
            {
                bool exported = false;
                foreach (AudioClip c in audioExportList)
                {
                    if (c == sound.clip)
                    {
                        exported = true;
                    }
                }
                if (exported == false)
                {
                    BuildPipeline.BuildAssetBundle(sound.clip, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Scene/Sound/" + sound.clip.name + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                    depBundleNames.Add("Scene/Sound/" + sound.clip.name + ".assetbundle");
                    audioExportList.Add(sound.clip);
                }
            }
        }

        /////
        {

            BlGame.Resource.EnviromentSetting settings = root.GetComponent<BlGame.Resource.EnviromentSetting>();//
            if (settings == null)
            {
                settings = root.AddComponent<BlGame.Resource.EnviromentSetting>();
            }
            settings.setValueByRenderSetting();
            prefabPath = "Assets/Build/ScenePrefab/" + SceneName + ".prefab";
            GameObject rootClone = (GameObject)GameObject.Instantiate(root);
            PrefabUtility.CreatePrefab(prefabPath, rootClone);
            GameObject.DestroyImmediate(rootClone);
        }
        ////
        //lightmap index
        BlGame.Resource.StringScriptableObject lightmapindexholder = ScriptableObject.CreateInstance<BlGame.Resource.StringScriptableObject>();
        lightmapindexholder.content = lightmapIndexList.ToArray();
        AssetDatabase.CreateAsset(lightmapindexholder, "Assets/temp/LightMapIndex.asset");
        UnityEngine.Object lightmapIndex = AssetDatabase.LoadAssetAtPath("Assets/temp/LightMapIndex.asset", typeof(BlGame.Resource.StringScriptableObject));
        //

        BlGame.Resource.StringScriptableObject holder = ScriptableObject.CreateInstance<BlGame.Resource.StringScriptableObject>();
        holder.content = depBundleNames.ToArray();
        AssetDatabase.CreateAsset(holder, "Assets/temp/DependentBundleNames.asset");
        UnityEngine.Object depBundleDescription = AssetDatabase.LoadAssetAtPath("Assets/temp/DependentBundleNames.asset", typeof(BlGame.Resource.StringScriptableObject));
        UnityEngine.Object mainAsset = AssetDatabase.LoadMainAssetAtPath(prefabPath);




        //BuildPipeline.BuildAssetBundle(mainAsset, null, "Assets/Export/Prefab/" + curSceneName + "_Scene" + ".assetbundle",   BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.CompleteAssets);
        BuildPipeline.BuildAssetBundle(mainAsset, new UnityEngine.Object[] { depBundleDescription/*, lightmapIndex */}, Application.dataPath + "/StreamingAssets/" + platformPath + "/Scene/Prefab/" + SceneName + "_Scene" + ".assetbundle", options, sceneBuildTarget);
        BuildPipeline.PopAssetDependencies();
    }
    [@MenuItem("BoLong/Build  AssetBundles/Android/BuildAllScenes")]
    static void ExportAllScenesAndroid()
    {
        clearBuff();
        checkAndCreateFolder();
        List<String> listScenes = new List<string>();
      //  listScenes.Add("pvp_001");
      //  listScenes.Add("pvp_002");
        listScenes.Add("pvp_004");
        foreach (string sceneName in listScenes)
        {
            string fullPath = Application.dataPath + "/Scenes/scenes/" + sceneName + "/" + sceneName + ".unity";
            EditorApplication.OpenScene(fullPath);
            ExportScene(fullPath, BuildTarget.Android);
            //EditorApplication.
            Debug.LogError("load scene" + sceneName);
        }
    }
    [@MenuItem("BoLong/Build  AssetBundles/Ios/BuildAllScenes")]
    static void ExportAllScenesIos()
    {
        clearBuff();
        checkAndCreateFolder();
        List<String> listScenes = new List<string>();
        //listScenes.Add("pvp_001");
        //listScenes.Add("pvp_002");
        listScenes.Add("pvp_004");
        foreach (string sceneName in listScenes)
        {
            string fullPath = Application.dataPath + "/Scenes/scenes/" + sceneName + "/" + sceneName + ".unity";
            EditorApplication.OpenScene(fullPath);
            ExportScene(fullPath, BuildTarget.iPhone);
            //EditorApplication.
            Debug.LogError("load scene" + sceneName);
        }
    }
    [@MenuItem("BoLong/Build  AssetBundles/Windows/BuildAllScenes")]
    static void ExportAllScenesWindows()
    {
        clearBuff();
        checkAndCreateFolder();
        List<String> listScenes = new List<string>();
        //listScenes.Add("pvp_001");
       // listScenes.Add("pvp_002");
       listScenes.Add("pvp_004");
        foreach (string sceneName in listScenes)
        {
            string fullPath = Application.dataPath + "/Scenes/scenes/" + sceneName + "/" + sceneName + ".unity";
            EditorApplication.OpenScene(fullPath);
            ExportScene(fullPath, BuildTarget.StandaloneWindows);
            //EditorApplication.
            Debug.LogError("load scene" + sceneName);
        }
    }

    ////
    static private void checkAndCreateFolder()
    {
        if (Directory.Exists(Application.dataPath + "/../Export/Effect") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/../Export/Effect");
        }
        if (Directory.Exists(Application.dataPath + "/../Export/Texture") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/../Export/Texture");
        }
        if (Directory.Exists(Application.dataPath + "/../Export/Mesh") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/../Export/Mesh");
        }
        if (Directory.Exists(Application.dataPath + "/Export/Temp") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/Export/Temp");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Scene") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Scene");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Scene/Prefab") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Scene/Prefab");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Scene/Mesh") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Scene/Mesh");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Scene/Texture") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Scene/Texture");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Scene/Shader") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Scene/Shader");
        }


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Scene/Prefab") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Scene/Prefab");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Scene/Mesh") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Scene/Mesh");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Scene/Texture") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Scene/Texture");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Scene/Shader") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Scene/Shader");
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Scene/Prefab") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Scene/Prefab");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Scene/Mesh") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Scene/Mesh");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Scene/Texture") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Scene/Texture");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Scene/Shader") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Scene/Shader");
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Shader") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Shader");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Effect/Texture") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Effect/Texture");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Effect/Mesh") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Effect/Mesh");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Effect/Prefab") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Effect/Prefab");
        }
        //----------------------------------------------------------------------------------------------------------
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Shader") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Shader");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Effect/Texture") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Effect/Texture");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Effect/Mesh") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Effect/Mesh");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Effect/Prefab") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Effect/Prefab");
        }
        //----------------------------------------------------------------------------------------------------------
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Shader") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Shader");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Effect/Texture") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Effect/Texture");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Effect/Mesh") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Effect/Mesh");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Effect/Prefab") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Effect/Prefab");
        }
        //----------------------------------------------------------------------------------------------------------
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Character/Texture") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Character/Texture");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Character/Mesh") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Character/Mesh");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Android/Character/Prefab") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Android/Character/Prefab");
        }
        //-----------------------------------------------------------------------------------------------------------
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Character/Texture") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Character/Texture");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Character/Mesh") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Character/Mesh");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Windows/Character/Prefab") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Windows/Character/Prefab");
        }
        //-----------------------------------------------------------------------------------------------------------
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Character/Texture") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Character/Texture");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Character/Mesh") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Character/Mesh");
        }
        if (Directory.Exists(Application.dataPath + "/StreamingAssets/Ios/Character/Prefab") == false)
        {
            Directory.CreateDirectory(Application.dataPath + "/StreamingAssets/Ios/Character/Prefab");
        }
    }
    //
    [@MenuItem("BoLong/Build  AssetBundles/Windows/BuildAllCharacter")]
    static private void BuildWindowsCharacter()
    {
        checkAndCreateFolder();
        ExportAllCharacterAssetBundles(BuildTarget.StandaloneWindows);
    }
    [@MenuItem("BoLong/Build  AssetBundles/Android/BuildAllCharacter")]
    static private void BuildAndroidCharacter()
    {
        checkAndCreateFolder();
        ExportAllCharacterAssetBundles(BuildTarget.Android);
    }
    [@MenuItem("BoLong/Build  AssetBundles/Ios/BuildAllCharacter")]
    static private void BuildIosCharacter()
    {
        checkAndCreateFolder();
        ExportAllCharacterAssetBundles(BuildTarget.iPhone);
    }
    //
    static void ExportAllCharacterAssetBundles(BuildTarget bTarget)
    {

        string platformPath = "";
        switch (bTarget)
        {
            case BuildTarget.WebPlayerStreamed:
            case BuildTarget.WebPlayer:
            case BuildTarget.StandaloneWindows:
                {
                    platformPath = "Windows";
                }
                break;
            case BuildTarget.Android:
                {
                    platformPath = "Android";
                }
                break;
            case BuildTarget.iPhone:
                {
                    platformPath = "Ios";
                }
                break;
        }
        BuildTarget sceneBuildTarget = bTarget;
        string[] characters = Directory.GetFiles(Application.dataPath + "/Resources/Monsters", "*", SearchOption.AllDirectories);
        BuildAssetBundleOptions options = BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.DeterministicAssetBundle;
        List<string> depBundleNames = new List<string>();

        foreach (string s in characters)
        {
            if (s.EndsWith("prefab") == false)
            {
                continue;
            }
            UnityEngine.Object eff = AssetDatabase.LoadMainAssetAtPath(BuildMethod.getUnityPath(s));
            string effectResFolder = BuildMethod.getUnityPathInResourceFolder(s);
            UnityEngine.Object[] refObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { eff });
            BuildPipeline.PushAssetDependencies();
            foreach (UnityEngine.Object o in refObjs)
            {

                //Debug.LogError(o.GetType());
                Material mat = o as Material;
                if (mat != null)
                {
                    UnityEngine.Object[] renderRefObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { mat });
                    {
                        Shader s1 = mat.shader;
                        bool exported = false;
                        foreach (Shader shader in shaderExportList)
                        {
                            if (shader == s1)
                            {
                                exported = true;
                            }
                        }
                        if (s1 != null && exported == false)
                        {
                            BuildPipeline.BuildAssetBundle(s1, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Shader/" + BuildMethod.getShaderName(s1.name) + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                            depBundleNames.Add("Shader/" + BuildMethod.getShaderName(s1.name) + ".assetbundle");
                            shaderExportList.Add(s1);
                        }
                    }
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    foreach (UnityEngine.Object ro in renderRefObjs)
                    {
                        Texture t = ro as Texture;
                        bool exported = false;
                        foreach (Texture tex in texExportList)
                        {
                            if (tex == t)
                            {
                                exported = true;
                            }
                        }
                        if (t != null && exported == false)
                        {

                            Debug.Log("Texture:" + AssetDatabase.GetAssetPath(t));
                            BuildPipeline.BuildAssetBundle(t, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Character/Texture/" + t.name + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                            depBundleNames.Add("Character/Texture/" + t.name + ".assetbundle");
                            texExportList.Add(t);
                        }
                    }
                }


                MeshRenderer render = o as MeshRenderer;
                if (render != null && render.sharedMaterial != null)
                {
                    UnityEngine.Object[] renderRefObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { render.sharedMaterial });
                    foreach (UnityEngine.Object ro in renderRefObjs)
                    {
                        Texture t = ro as Texture;
                        bool exported = false;
                        foreach (Texture tex in texExportList)
                        {
                            if (tex == t)
                            {
                                exported = true;
                            }
                        }
                        if (t != null && exported == false)
                        {

                            Debug.Log("Texture:" + AssetDatabase.GetAssetPath(t));
                            BuildPipeline.BuildAssetBundle(t, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Character/Texture/" + t.name + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                            depBundleNames.Add("Character/Texture/" + t.name + ".assetbundle");
                            texExportList.Add(t);
                        }
                    }
                }
                ParticleSystem particle = o as ParticleSystem;
                if (particle != null && particle.renderer != null && particle.renderer.sharedMaterials != null)
                {
                    foreach (Material m in particle.renderer.sharedMaterials)
                    {
                        UnityEngine.Object[] renderRefObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { m });
                        /////////////////////////////////////////////////////////////////////////
                        {
                            if (m != null)
                            {
                                Shader pShader = m.shader;
                                bool exported = false;
                                foreach (Shader shader in shaderExportList)
                                {
                                    if (shader == pShader)
                                    {
                                        exported = true;
                                    }
                                }
                                if (pShader != null && exported == false)
                                {
                                    BuildPipeline.BuildAssetBundle(pShader, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Shader/" + BuildMethod.getShaderName(pShader.name) + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                                    depBundleNames.Add("Shader/" + BuildMethod.getShaderName(pShader.name) + ".assetbundle");
                                    shaderExportList.Add(pShader);
                                }
                            }
                        }
                        foreach (UnityEngine.Object ro in renderRefObjs)
                        {
                            Texture t = ro as Texture;
                            bool exported = false;
                            foreach (Texture tex in texExportList)
                            {
                                if (tex == t)
                                {
                                    exported = true;
                                }
                            }
                            if (t != null && exported == false)
                            {

                                //Debug.LogError("Texture:" + AssetDatabase.GetAssetPath(t));
                                BuildPipeline.BuildAssetBundle(t, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Character/Texture/" + t.name + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                                depBundleNames.Add("Character/Texture/" + t.name + ".assetbundle");
                                texExportList.Add(t);
                            }
                        }
                    }
                }
                //导出模型
                //MeshFilter mesh = o as MeshFilter;
                //if (mesh != null && mesh.sharedMesh != null)
                //{
                //    bool exported = false;
                //    foreach (Mesh m in meshExportList)
                //    {
                //        if (m == mesh.sharedMesh)
                //        {
                //            exported = true;
                //        }
                //    }
                //    if (exported == false)
                //    {
                //        BuildPipeline.BuildAssetBundle(mesh.sharedMesh, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Character/Mesh/" + mesh.sharedMesh.name + ".assetbundle", BuildAssetBundleOptions.DeterministicAssetBundle, sceneBuildTarget);
                //        Debug.Log("Mesh:" + AssetDatabase.GetAssetPath(mesh.sharedMesh));
                //        depBundleNames.Add("Effect/Mesh/" + mesh.sharedMesh.name + ".assetbundle");
                //        meshExportList.Add(mesh.sharedMesh);
                //    }
                //}
                //Debug.LogError(o.name);
            }
            BlGame.Resource.StringScriptableObject stringSO = ScriptableObject.CreateInstance<BlGame.Resource.StringScriptableObject>();
            stringSO.content = depBundleNames.ToArray();
            AssetDatabase.CreateAsset(stringSO, "Assets/Export/Temp/DependentBundleNames.asset");
            UnityEngine.Object depBundleDescription = AssetDatabase.LoadAssetAtPath("Assets/Export/Temp/DependentBundleNames.asset", typeof(BlGame.Resource.StringScriptableObject));


            string folder = Application.dataPath + "/StreamingAssets/" + platformPath + "/Character/Prefab/";
            if (Directory.Exists(folder) == false)
            {
                Directory.CreateDirectory(folder);
            }
            BuildPipeline.BuildAssetBundle(eff, new UnityEngine.Object[] { depBundleDescription }, folder + Path.GetFileNameWithoutExtension(s) + ".assetbundle", options, sceneBuildTarget);
            BuildPipeline.PopAssetDependencies();
        }
        
    }

    //
    [@MenuItem("BoLong/Build  AssetBundles/Windows/BuildAllEffects")]
    static private void BuildWindowsEffect()
    {
        checkAndCreateFolder();
        ExportAllEffectsAssetBundles(BuildTarget.StandaloneWindows);
    }
    [@MenuItem("BoLong/Build  AssetBundles/Android/BuildAllEffects")]
    static private void BuildAndroidEffect()
    {
        checkAndCreateFolder();
        ExportAllEffectsAssetBundles(BuildTarget.Android);
    }
    [@MenuItem("BoLong/Build  AssetBundles/Ios/BuildAllEffects")]
    static private void BuildIosEffect()
    {
        checkAndCreateFolder();
        ExportAllEffectsAssetBundles(BuildTarget.iPhone);
    }
    static void ExportAllEffectsAssetBundles(BuildTarget bTarget)
    {
        clearBuff();
        string platformPath = "";
        switch (bTarget)
        {
            case BuildTarget.WebPlayerStreamed:
            case BuildTarget.WebPlayer:
            case BuildTarget.StandaloneWindows:
                {
                    platformPath = "Windows";
                }
                break;
            case BuildTarget.Android:
                {
                    platformPath = "Android";
                }
                break;
            case BuildTarget.iPhone:
                {
                    platformPath = "Ios";
                }
                break;
        }
        BuildTarget sceneBuildTarget = bTarget;
        //Debug.LogError(Application.dataPath);
        //checkAndCreateFolder();


        string[] effects = Directory.GetFiles(Application.dataPath + "/Resources/effect", "*", SearchOption.AllDirectories);
        //Debug.LogError("file count" + effects.Length);
        BuildAssetBundleOptions options = BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.DeterministicAssetBundle
            | BuildAssetBundleOptions.CompleteAssets;
        List<string> depBundleNames = new List<string>();
        int i = 0;
        foreach (string s in effects)
        {
            depBundleNames.Clear();
            //if (i > 5)
            //{
            //    break;
            //}           
            if (s.EndsWith(".prefab"))
            {
                if (s.EndsWith("shadow.prefab") == false)
                {
                    continue;
                }
                UnityEngine.Object eff = AssetDatabase.LoadMainAssetAtPath(BuildMethod.getUnityPath(s));
                string effectResFolder = BuildMethod.getUnityPathInResourceFolder(s);
                UnityEngine.Object[] refObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { eff });
                BuildPipeline.PushAssetDependencies();
                foreach (UnityEngine.Object o in refObjs)
                {
                    //导出模型
                    //MeshFilter mesh = o as MeshFilter;
                    //if (mesh != null && mesh.sharedMesh != null)
                    //{
                    //    bool exported = false;
                    //    foreach (Mesh m in meshExportList)
                    //    {
                    //        if (m == mesh.sharedMesh)
                    //        {
                    //            exported = true;
                    //        }
                    //    }
                    //    if (exported == false)
                    //    {
                    //        BuildPipeline.BuildAssetBundle(mesh.sharedMesh, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Effect/Mesh/" + mesh.sharedMesh.name + ".assetbundle", options, sceneBuildTarget);
                    //        //Debug.Log("Mesh:" + AssetDatabase.GetAssetPath(mesh.sharedMesh));
                    //        depBundleNames.Add("Effect/Mesh/" + mesh.sharedMesh.name + ".assetbundle");
                    //        meshExportList.Add(mesh.sharedMesh);
                    //    }
                    //}

                    //Debug.LogError(o.GetType());
                    Material mat = o as Material;
                    if (mat != null)
                    {
                        UnityEngine.Object[] renderRefObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { mat });
                        {
                            Shader s1 = mat.shader;
                            bool exported = false;
                            foreach (Shader shader in shaderExportList)
                            {
                                if (shader == s1)
                                {
                                    exported = true;
                                }
                            }
                            if (s1 != null && exported == false)
                            {
                                BuildPipeline.BuildAssetBundle(s1, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Shader/" + BuildMethod.getShaderName(s1.name) + ".assetbundle", options, sceneBuildTarget);
                                depBundleNames.Add("Shader/" + BuildMethod.getShaderName(s1.name) + ".assetbundle");
                                shaderExportList.Add(s1);
                            }
                        }
                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        foreach (UnityEngine.Object ro in renderRefObjs)
                        {
                            Texture t = ro as Texture;
                            bool exported = false;
                            foreach (Texture tex in texExportList)
                            {
                                if (tex == t) 
                                {
                                    exported = true;
                                }
                            }
                            if (t != null && exported == false)
                            {

                                //Debug.Log("Texture:" + AssetDatabase.GetAssetPath(t));
                                BuildPipeline.BuildAssetBundle(t, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Effect/Texture/" + t.name + ".assetbundle", options, sceneBuildTarget);
                                depBundleNames.Add("Effect/Texture/" + t.name + ".assetbundle");
                                texExportList.Add(t);
                            }
                        }
                    }


                    MeshRenderer render = o as MeshRenderer;
                    if (render != null && render.sharedMaterial != null)
                    {
                        UnityEngine.Object[] renderRefObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { render.sharedMaterial });
                        foreach (UnityEngine.Object ro in renderRefObjs)
                        {
                            Texture t = ro as Texture;
                            bool exported = false;
                            foreach (Texture tex in texExportList)
                            {
                                if (tex == t)
                                {
                                    exported = true;
                                }
                            }
                            if (t != null && exported == false)
                            {

                                Debug.Log("Texture:" + AssetDatabase.GetAssetPath(t));
                                BuildPipeline.BuildAssetBundle(t, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Effect/Texture/" + t.name + ".assetbundle", options, sceneBuildTarget);
                                depBundleNames.Add("Effect/Texture/" + t.name + ".assetbundle");
                                texExportList.Add(t);
                            }
                        }
                    }
                    ParticleSystem particle = o as ParticleSystem;
                    if (particle != null && particle.renderer != null && particle.renderer.sharedMaterials != null)
                    {
                        foreach (Material m in particle.renderer.sharedMaterials)
                        {
                            UnityEngine.Object[] renderRefObjs = EditorUtility.CollectDependencies(new UnityEngine.Object[] { m });
                            /////////////////////////////////////////////////////////////////////////
                            {
                                if (m != null)
                                {
                                    Shader pShader = m.shader;
                                    bool exported = false;
                                    foreach (Shader shader in shaderExportList)
                                    {
                                        if (shader == pShader)
                                        {
                                            exported = true;
                                        }
                                    }
                                    if (pShader != null && exported == false)
                                    {
                                        BuildPipeline.BuildAssetBundle(pShader, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Shader/" + BuildMethod.getShaderName(pShader.name) + ".assetbundle", options, sceneBuildTarget);
                                        depBundleNames.Add("Shader/" + BuildMethod.getShaderName(pShader.name) + ".assetbundle");
                                        shaderExportList.Add(pShader);
                                    }
                                }
                            }
                            foreach (UnityEngine.Object ro in renderRefObjs)
                            {
                                Texture t = ro as Texture;
                                bool exported = false;
                                foreach (Texture tex in texExportList)
                                {
                                    if (tex == t)
                                    {
                                        exported = true;
                                    }
                                }
                                if (t != null && exported == false)
                                {

                                    //Debug.LogError("Texture:" + AssetDatabase.GetAssetPath(t));
                                    BuildPipeline.BuildAssetBundle(t, null, Application.dataPath + "/StreamingAssets/" + platformPath + "/Effect/Texture/" + t.name + ".assetbundle", options, sceneBuildTarget);
                                    depBundleNames.Add("Effect/Texture/" + t.name + ".assetbundle");
                                    texExportList.Add(t);
                                }
                            }
                        }
                    }

                    //Debug.LogError(o.name);
                }
                BlGame.Resource.StringScriptableObject stringSO = ScriptableObject.CreateInstance<BlGame.Resource.StringScriptableObject>();
                stringSO.content = depBundleNames.ToArray();
                AssetDatabase.CreateAsset(stringSO, "Assets/Export/Temp/DependentBundleNames.asset");
                UnityEngine.Object depBundleDescription = AssetDatabase.LoadAssetAtPath("Assets/Export/Temp/DependentBundleNames.asset", typeof(BlGame.Resource.StringScriptableObject));


                string folder = Application.dataPath + "/StreamingAssets/" + platformPath + "/Effect/Prefab/" + effectResFolder;
                if (Directory.Exists(folder) == false)
                {
                    Directory.CreateDirectory(Path.GetDirectoryName(folder));
                }
                BuildPipeline.PushAssetDependencies();
                BuildPipeline.BuildAssetBundle(eff, new UnityEngine.Object[] { depBundleDescription }, Path.GetDirectoryName(folder)+ "/" + Path.GetFileNameWithoutExtension(s) + ".assetbundle", options, sceneBuildTarget);
                BuildPipeline.PopAssetDependencies();
                BuildPipeline.PopAssetDependencies();

            }
        }

    }

    [@MenuItem("BoLong/Build  AssetBundles/BuildCurScene")]
    static void ExportCurScene()
    {

    }

}
