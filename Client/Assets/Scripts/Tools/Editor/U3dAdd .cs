using UnityEngine;
using UnityEditor;
using System.IO;
//using System.Collections;

public class U3dAdd{

	[@MenuItem("BoLong/Build  AssetBundles/From prefabs of Files ")]

	static void ExportAssetBundles(){
		string path = AssetDatabase.GetAssetPath(Selection.activeObject);

		BuildAssetBundleOptions option = BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.CompleteAssets;//资源包编译选项


		if(path.Length !=0 )
		{
			path = path.Replace("Assets/","");
			string [] fileEntries = Directory.GetFiles(Application.dataPath + "/" + path);

			foreach(string fileName in fileEntries){
				string filePath = fileName;	
				int index = filePath.LastIndexOf("/");
				filePath = filePath.Substring(index);
				int indexp = path.LastIndexOf("/");
				path = path.Substring(0,indexp);
				string localPath = "Assets/" + path;
			
				if(index > 0)
				{
					localPath += filePath;
				}

				Object t =AssetDatabase.LoadMainAssetAtPath(localPath);

				if(t != null)
				{ 
					string bundlepath = "Assets/" + path + "/" +t.name + ".unity3d";
					BuildPipeline.BuildAssetBundle(t,null,bundlepath, option);
				}
			}	 
		}
	}
	


}
