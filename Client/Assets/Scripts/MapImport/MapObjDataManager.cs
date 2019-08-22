 
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

 
public class MapObjDataManager:MonoBehaviour
{
    //public List<MapObjData> objData;

	public int MapName;

	public int MapWidth = 400;

	public int MapHeight = 400;

    //public int totalObjectsNum = 60;

    //public int heroCount = 6;

	//private Dictionary<int,MapObjData> indexDic = new Dictionary<int, MapObjData>();

	void Awake()
	{
		return;
        //for (int i = 0; i < objData.Count; i++) {  
        //    CheckRepeatId(objData[i]);
        //}
	}
	void Start()
	{
		return;
	//	MapObjData[] objs = GameObject.FindObjectsOfType(typeof(MapObjData)) as MapObjData[];
		
        //if (objs.Length != objData.Count) {
        //    Debug.LogError("数量不匹配");		
        //}
	}
	 
    //private void CheckRepeatId(MapObjData data)
    //{
    //    if (!indexDic.ContainsKey (data.objIndex)) {
    //        indexDic.Add (data.objIndex,data);
    //    }
    //    else {
    //        MapObjData hasData ;
    //        indexDic.TryGetValue(data.objIndex,out hasData);
    //        Debug.LogError(hasData.transform.name + "和"+data.transform.name+"冲突了。Id = " +data.objIndex);
    //    }
    //} 
}



