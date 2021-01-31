using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using BlGame.GameEntity;
using BlGame.Resource;

public class CrticalStrikeManager : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}
    private const string res = "Guis/" + "CrticalStrike";

    public static CrticalStrikeManager Instance
    {
        private set;
        get;
    }

    public CrticalStrike crticalstrile
    {
        private set;
        get;
    }
    int index = 5;
    private List<CrticalStrike> CrticalStrikeList = new List<CrticalStrike>();
    void Awake()
    {
        Instance = this;
        for (int i = 0; i < index; i++)
        {
            CrticalStrike addui = LoadCPAddPrefab(res);
            addui.gameObject.transform.parent = transform;
            addui.gameObject.transform.localScale = Vector3.one;
            addui.gameObject.transform.localPosition = Vector3.zero;
            addui.gameObject.SetActive(false);
            CrticalStrikeList.Add(addui);
        }
    }
    public void GetList()
    {
        foreach (var item in CrticalStrikeList)
        {
            UIPanel panel = item.GetComponent<UIPanel>();
            if (panel.alpha <= 0 && item.gameObject.activeSelf == true)
            {
                item.gameObject.SetActive(false);
            }
            item.gameObject.transform.localScale = Vector3.one;
            item.gameObject.transform.localPosition = Vector3.zero;
        }
    }

    public void CreateCrticalStrike( int count, Ientity tran)
    {
        if (count == 0)
        {
            return ;
        }
        foreach (var item in CrticalStrikeList)
        {
            if (item.isAni == false)
            {
                item.Init(tran);
                item.gameObject.transform.parent = transform;
                item.gameObject.transform.localScale = Vector3.one;
                item.UpdatePosition(tran.realObject.transform);
                item.SetShowDighit(Mathf.Abs(count));
                break;
            }
        }
    }

    CrticalStrike LoadCPAddPrefab(string path)
    {       
        //ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
        //GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;

        GameObject obj = GameObjectPool.Instance.GetGO(res);

     

        CrticalStrike cpadd = obj.GetComponent<CrticalStrike>();
        return cpadd;
    }
}
