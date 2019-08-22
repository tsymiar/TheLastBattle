using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System.IO;

public class EditorTools : MonoBehaviour {

    static int go_count = 0;
    static int components_count = 0;
    static int missing_count = 0;
    static List<string> mResources = new List<string>();
    static List<string> mEffectResources = new List<string>();

    //查找Resource资源中miss的脚本
    [MenuItem("Tools/FindMissScriptInResource")]
    public static void FindMissScriptInResource()
    {
        go_count = 0;
        components_count = 0;
        missing_count = 0;

        //获取所有资源路径       
        mResources.Clear();

        //Resource资源路径
        string resourcePath = Application.dataPath + "/Resources/";

        string[] files = Directory.GetFiles(resourcePath, "*.*", SearchOption.AllDirectories);
        foreach (string file in files)
        {
            string suffix = BuildCommon.getFileSuffix(file);
            if (suffix == "meta")
                continue;
            
            //查找预制件件
            if (suffix == "prefab")
            {
                string realFile = file.Replace("\\", "/");
                realFile = realFile.Replace(Application.dataPath, "Assets");
                mResources.Add(realFile);       
            }
        }
        
        //查找所有miss文件
        foreach(string assetPath in mResources)
        {
            GameObject asset = AssetDatabase.LoadMainAssetAtPath(assetPath) as GameObject;
            FindInGO(asset);    
        }
        Debug.Log(string.Format("Searched {0} GameObjects, {1} components, found {2} missing", go_count, components_count, missing_count));     
    }

    //检测Resource资源中的Effect是否有效
    [MenuItem("Tools/CheckEffectInResource")]
    public static void CheckEffectInResource()
    {
        //获取所有资源路径       
        mEffectResources.Clear();

        //Resource资源路径
        string resourcePath = Application.dataPath + "/Resources/effect";

        string[] files = Directory.GetFiles(resourcePath, "*.*", SearchOption.AllDirectories);
        foreach (string file in files)
        {
            string suffix = BuildCommon.getFileSuffix(file);
            if (suffix == "meta")
                continue;

            //查找预制件件
            if (suffix == "prefab")
            {
                string realFile = file.Replace("\\", "/");
                realFile = realFile.Replace(Application.dataPath, "Assets");
                mEffectResources.Add(realFile);
            }
        }

        List<ParticleSystem> pss = new List<ParticleSystem>();
        //查找是否缺少EffectScript
        foreach (string assetPath in mEffectResources)
        {
            GameObject asset = AssetDatabase.LoadMainAssetAtPath(assetPath) as GameObject;

            //检测是否缺少EffectScript脚本
            EffectScript effectScript = asset.GetComponent<EffectScript>();
            if (effectScript == null)
                Debug.Log("the effect " + assetPath + " miss EffectScript!");

            SL_DestroyByTime destTimeScript = asset.GetComponent<SL_DestroyByTime>();
            if (destTimeScript != null)
            {
                Debug.Log("do not use the SL_DestoryByTime script now!");                    
            }

            pss.Clear();
            //检测ParticleSystem
            RecusionParticleSystemObject(asset, ref pss);
            foreach(ParticleSystem ps in pss)
            {
                if (ps.renderer.castShadows || ps.renderer.receiveShadows)
                {
                    Debug.Log(asset.name + " " + ps.name + "is cast shadow or receive shadow");
                }         
            }         
        }
        Debug.Log("Check finised!");  
    }

    public static void RecusionParticleSystemObject(GameObject gameObject, ref List<ParticleSystem> pss)
    {
        //获取子ParticleSystem        
        for (int i = 0; i < gameObject.transform.childCount; i++)
        {
            Transform ts = gameObject.transform.GetChild(i);
            GameObject psObj = ts.gameObject;
            if (psObj)
            {
                //保存ParticleSystem
                ParticleSystem psComp = psObj.GetComponent<ParticleSystem>();
                if (psComp != null)
                {
                    pss.Add(psComp);
                }
            }
        }
    }

 















    private static void FindInGO(GameObject g)
    {
        go_count++;
        Component[] components = g.GetComponents<Component>();
        for (int i = 0; i < components.Length; i++)
        {
            components_count++;
            if (components[i] == null)
            {
                missing_count++;
                string s = g.name;
                Transform t = g.transform;
                while (t.parent != null)
                {
                    s = t.parent.name + "/" + s;
                    t = t.parent;
                }
                Debug.Log(s + " has an empty script attached in position: " + i, g);
            }
        }
        // Now recurse through each child GO (if there are any):
        foreach (Transform childT in g.transform)
        {
            //Debug.Log("Searching " + childT.name  + " " );
            FindInGO(childT.gameObject);
        }               
    }

    //检测Material材质在Resource资源中的所有引用关系
    [MenuItem("Tools/CheckMaterialReference")]
    public static void CheckMaterialReference()
    {
        Object[] objects = Selection.objects;
        if (objects.Length != 1)
        {
            EditorUtility.DisplayDialog("Error", "you should select one items", "ok");
            return;
        }

        Object obj = objects[0];
        Material mat = obj as Material;
        if (mat == null)
        {
            EditorUtility.DisplayDialog("Error", "you should select material", "ok");
            return;
        }


        string matPath = AssetDatabase.GetAssetPath(mat);
        //Debug.Log(matPath);



        //获取所有资源路径       
        mResources.Clear();

        //Resource资源路径
        string resourcePath = Application.dataPath + "/Resources/";

        string[] files = Directory.GetFiles(resourcePath, "*.*", SearchOption.AllDirectories);
        foreach (string file in files)
        {
            string suffix = BuildCommon.getFileSuffix(file);
            if (suffix == "meta")
                continue;

            //查找预制件件
            if (suffix == "prefab")
            {
                string realFile = file.Replace("\\", "/");
                realFile = realFile.Replace(Application.dataPath, "Assets");
                mResources.Add(realFile);
            }
        }

        //查找所有引用该Material的文件
        foreach (string assetPath in mResources)
        {
            //获取包含的所有依赖
            string[] depencies = AssetDatabase.GetDependencies(new string[] { assetPath });
            foreach (string dep in depencies)
            {
                //如果是材质
                if (dep.EndsWith(".mat") && matPath == dep)
                {

                    Debug.Log(mat.name + "is referenced by" + assetPath);
                }
            }
        }
    }
    
}
