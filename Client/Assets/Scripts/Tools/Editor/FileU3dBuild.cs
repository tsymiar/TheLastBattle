using UnityEngine;
using UnityEditor;
using System.IO;


public class FileU3dBuild  {

	[@MenuItem("BoLong/Build  AssetBundles/From Files")]
	// Use this for initialization

	static void ExportAssetBundles(){
		string path = AssetDatabase.GetAssetPath(Selection.activeObject);
		string path1 = Path.GetDirectoryName(path);

		

		BuildAssetBundleOptions option = BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.CompleteAssets;//资源包编译选项

		if(path1.Length != 0)
		{
			path1 = path1.Replace("Assets/","");

			string [] fileEntries1 = Directory.GetFiles(Application.dataPath + "/" +path1);
			foreach(string fileName in fileEntries1)
			{
				if (fileName.IndexOf(".meta") == -1)
				{
					string filePath =  fileName.Replace("\\","/");

					int index = filePath.LastIndexOf("/");
					filePath = filePath.Substring(index);
					string localPath = "Assets/" + path1;
					if(index > 0 )
					{
						localPath += filePath;
					}

					Object t = AssetDatabase.LoadMainAssetAtPath(localPath);
					if(t != null)
					{
						string bundlePath = "Assets/" + path1 + "/" + t.name + ".unity3d";
						BuildPipeline.BuildAssetBundle(t,null,bundlePath,option);
					}
				}
			}
		}
		else 
		{
			Debug.LogError("path null");
		}
	}
}
