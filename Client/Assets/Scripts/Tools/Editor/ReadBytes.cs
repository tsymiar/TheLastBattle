using UnityEditor;
using System.Collections;
using UnityEngine;
using System;

public class ReadBytes :MonoBehaviour {

//you cuowu 	string url = "file://"+Application.dataPath+" /Prefabs/AreaMark/AreaMarks.bytes";

	public  string url = "file://E:/bolong/FbAll/HuaShen2/Assets/Prefabs/AreaMark/AreaMarks.bytes";
	[@MenuItem("BoLong/Read AssetBundles From Directory of Files")]
	
	IEnumerator Start(){
		WWW www =  new WWW (url);
		yield return www;
		byte [] encryptedData = www.bytes;

        byte[] decrytedData = YourDecryptionMethod(encryptedData);

//        AssetBundle bundle =(AssetBundle) AssetBundle.CreateFromMemory(decrytedData).assetBundle;

//		AssetBundle bundle =(AssetBundle) AssetBundle.CreateFromMemory(encryptedData).assetBundle;

	}

	private byte[] YourDecryptionMethod(byte[] encryptedData)
	{
		return encryptedData;
	}
}
