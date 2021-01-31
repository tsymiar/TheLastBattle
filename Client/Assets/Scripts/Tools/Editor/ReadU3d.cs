using UnityEngine;
//using UnityEditor;
using System.Collections;
using System.IO;

public class ReadU3d :MonoBehaviour {

//baocuo 	string url1 = "file://" + Application.dataPath + "/Prefabs/AreaMark/AreaMarks.unity3d";
	public  string url1 = "file://E:/bolong/FbAll/HuaShen2/Assets/scripts/Tools/BuildU3d/BytesAdd.unity3d";

//	[@MenuItem("BoLong/Read AssetBundles From Directory of Files")]
//	void Update(){
//	}

	IEnumerator Start(){
		WWW www = WWW.LoadFromCacheOrDownload(url1 , 1);
		yield return www;
		AssetBundle bundle = www.assetBundle;

		GameObject go = bundle.Load("panel_npc" , typeof(GameObject)) as GameObject;
		Instantiate(go);
//		string writePath = Path.Combine(Application.persistentDataPath , url);

//		TextAsset txt = bundle.Load("AreaMarks" , typeof(TextAsset)) as TextAsset;
//		byte [] bytes = www.bytes;



	}
}
