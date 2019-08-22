using System;
using UnityEditor;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;

public class ShowPathRoute : EditorWindow
{
	[MenuItem ("BoLong/显示寻路路径")]
	static void Init(){
		ShowPathRoute window = (ShowPathRoute)EditorWindow.GetWindow (typeof (ShowPathRoute));
		window.title = "显示  寻路  路径";
	}

	void OnGUI(){

		if (GUILayout.Button ("显示  路径")){

			StreamReader sr = new StreamReader("Assets/PathRoute.txt"  , System.Text.Encoding.Default);
			if(sr == null){
				Debug.LogError("Path Assets Has no PathRoute.txt");
				return;
			}
			GameObject sp;
			sp = GameObject.Find("Path");
			if(sp == null){
				sp = new GameObject("Path");
			}else{
				for(int cd = 0 ; cd < sp.transform.childCount ; cd++){
					GameObject.DestroyImmediate(sp.transform.GetChild(0).gameObject);
				}
			}

			sp.transform.position = Vector3.zero;
			string cont;
			while ((cont = sr.ReadLine()) != null){
				GameObject spChld = GameObject.CreatePrimitive(PrimitiveType.Sphere);
				spChld.transform.parent = sp.transform;
				spChld.transform.localScale = new Vector3(0.5f , 0.5f , 0.5f);

				string[] vec = cont.Split(',');
				float x = Convert.ToSingle(vec[0]) / 2 + 0.25f;
				float z = Convert.ToSingle(vec[1]) / 2 + 0.25f;
				spChld.transform.position = new Vector3(x , 0.0f , z) ;

			}
		}
	}


}


