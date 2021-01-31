using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using BlGame.Resource;

public class LoadUiResource
{
	public static GameObject LoadRes(Transform parent,string path)
	{
		if(CheckResInDic(path))			
		{
			if(GetResInDic(path) != null){
				return GetResInDic(path);
			}
			else{
				LoadResDic.Remove(path);
			}
		}
		
		GameObject objLoad = null;

        ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
        if (objUnit == null || objUnit.Asset == null)
        {
            Debug.LogError("load unit failed" + path);
            return null;
        }
        objLoad = GameObject.Instantiate(objUnit.Asset) as GameObject;
		objLoad.transform.parent = parent;
		objLoad.transform.localScale = Vector3.one;
		objLoad.transform.localPosition = Vector3.zero;
		LoadResDic.Add(path,objLoad);
		return objLoad;
	}

    //创建窗口子对象，不加入资源管理
    public static GameObject AddChildObject(Transform parent, string path)
    {
        GameObject objLoad = null;

        ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
        if (objUnit == null || objUnit.Asset == null)
        {
            Debug.LogError("load unit failed" + path);
            return null;
        }
        objLoad = GameObject.Instantiate(objUnit.Asset) as GameObject;
        objLoad.transform.parent = parent;
        objLoad.transform.localScale = Vector3.one;
        objLoad.transform.localPosition = Vector3.zero;

        return objLoad;
    }

    public static void ClearAllChild(Transform transform)
    {
        while (transform.childCount > 0)
        {
            GameObject.DestroyImmediate(transform.GetChild(0).gameObject);
        }
        transform.DetachChildren();
    }

    public static void ClearOneChild(Transform transform,string name)
    {
        for (int i = 0; i < transform.childCount; i++)
        {
            if (transform.GetChild(i).gameObject.name == name)
            {
                GameObject.DestroyImmediate(transform.GetChild(i).gameObject);
            }
        }
    }

	public static void DestroyLoad(string path)
	{
		if(LoadResDic == null || LoadResDic.Count == 0)
			return;
		GameObject obj = null;
        if (LoadResDic.TryGetValue(path, out obj) && obj != null)
		{
			GameObject.DestroyImmediate(obj);
			LoadResDic.Remove(path);
			//System.GC.Collect();
		}
	}
	
	public static void DestroyLoad(GameObject obj)
	{
		if(LoadResDic == null || LoadResDic.Count == 0)
			return;
		if(obj == null)
			return;
		foreach(string key in LoadResDic.Keys)
		{
			GameObject objLoad;
			if(LoadResDic.TryGetValue(key,out objLoad) && objLoad == obj)
			{
				GameObject.DestroyImmediate(obj);
				LoadResDic.Remove(key);
				break;
			}
		}		
	}


	public static GameObject GetResInDic(string path)
	{
		if(LoadResDic == null || LoadResDic.Count == 0)
			return null;
		GameObject obj = null ;
		if(LoadResDic.TryGetValue(path,out obj))
		{
			return obj;
		}
		return null;
	}
	
	public  static bool CheckResInDic(string path)
	{
		if(LoadResDic == null || LoadResDic.Count == 0)
			return false;
		return LoadResDic.ContainsKey(path);
	}
	
	public static void Clean()
	{
		if(LoadResDic == null || LoadResDic.Count == 0)
			return;
		for(int i = LoadResDic.Count - 1;i >=0;i--)
		{
			GameObject obj = LoadResDic.ElementAt(i).Value ;
			if( obj != null)
			{
				GameObject.DestroyImmediate(obj);
			}
		}
		LoadResDic.Clear();
	}

	public static Dictionary<string,GameObject> LoadResDic = new Dictionary<string, GameObject>();

}


