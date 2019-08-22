using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using System.Collections.Generic;
using BlGame;
using BlGame.Resource;

public class CPChanggeManager : MonoBehaviour {

    public const string cp_path = "Guis/Play/CPAddItem";

    public static CPChanggeManager Instance
    {
        private set;
        get;
    }

    public CPAddUI cpadd
    {
        private set;
        get;
    }
    int index = 5;
    public List<CPAddUI> CpAddList = new List<CPAddUI>();
    void Awake()
    {
        Instance = this;
        for (int i = 0; i < index; i ++ )
        {
            CPAddUI addui = LoadCPAddPrefab(cp_path);
            addui.gameObject.transform.parent = transform;
            addui.gameObject.transform.localScale = Vector3.one;
            addui.gameObject.transform.localPosition = Vector3.zero;
            addui.gameObject.SetActive(false);
            CpAddList.Add(addui);
        }
    }
    //public void GetList()
    //{
    //    //foreach (var item in CpAddList)
    //    //{
    //    //    UIWidget panel = item.GetComponent<UIWidget>();
    //    //    if (panel.alpha <= 0 && item.gameObject.activeSelf == true)
    //    //    {
    //    //        item.gameObject.SetActive(false);
    //    //    }
    //    //    item.gameObject.transform.localScale = Vector3.one;
    //    //    item.gameObject.transform.localPosition = Vector3.zero;
    //    //}
    //}

    public void ClearList()
    {
        foreach (var item in CpAddList)
        {
            item.gameObject.SetActive(false);
            item.gameObject.transform.localScale = Vector3.one;
            item.gameObject.transform.localPosition = Vector3.zero;
        }
    }


    public void CreateCPAdd(int count = 0)
    {
        //GetList();
        if (count == 0)
        {
            return;
        }
       foreach (var item in CpAddList)
       {           
           if(item.gameObject.activeSelf == false)
           {
               item.Init();
               AudioManager.Instance.GetMoneyAudio();
               item.SetCPAdd_Count(count.ToString(), item.gameObject);
               return;
           }
       }
       
        //if(cpadd == null)
        //     cpadd = LoadCPAddPrefab(cp_path);
        //cpadd.Init();
        //cpadd.gameObject.transform.parent = transform;
        //cpadd.SetCPAdd_Count(count.ToString());
    }

     CPAddUI LoadCPAddPrefab(string path)
    {    
        //ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
        //if (objUnit != null && objUnit.Asset != null)
        //{
        //    GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
        //    CPAddUI cpadd = obj.GetComponent<CPAddUI>();
        //    return cpadd;
        //}
        //else
        //{
        //    return null;
        //}       

        GameObject obj = GameObjectPool.Instance.GetGO(path);

      

        CPAddUI cpadd = obj.GetComponent<CPAddUI>();
        return cpadd;
    }
}
