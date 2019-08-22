//#define Resource_UseLog

using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Object = UnityEngine.Object;


namespace BlGame.Resource
{
    public class ResourceCommon
    {
        public static string textFilePath = Application.streamingAssetsPath;

        public static string assetbundleFilePath = Application.dataPath + "/assetbundles/";

        public static string assetbundleFileSuffix = ".bytes";

        public static string DEBUGTYPENAME = "Resource";

        //public static string mServerAssetPath = @"http://192.168.123.1/assetbundles/";
        public static string mServerAssetPath = "file:///c:/";

        public static void Log(string fileName)
        {
#if Resource_UseLog
            Debug.Log(fileName, DEBUGTYPENAME, true);
#endif
        }

        //根据资源路径获取资源名称
        public static string getResourceName(string resPathName)
        {
            int index = resPathName.LastIndexOf("/");
            if (index == -1)
                return resPathName;
            else
            {
                return resPathName.Substring(index + 1, resPathName.Length - index - 1);
            }
        }


        public static string getFileName(string fileName)
        {
            int index = fileName.IndexOf(".");
            if (-1 == index)
                throw new Exception("can not find .!!!");
            return fileName.Substring(0, index);
        }

        public static string getFileName(string filePath, bool suffix)
        {
            if (!suffix)
            {
                string path = filePath.Replace("\\", "/");
                int index = path.LastIndexOf("/");
                if (-1 == index)
                    throw new Exception("can not find .!!!");
                int index2 = path.LastIndexOf(".");
                if (-1 == index2)
                    throw new Exception("can not find /!!!");
                return path.Substring(index + 1, index2 - index - 1);
            }
            else
            {
                string path = filePath.Replace("\\", "/");
                int index = path.LastIndexOf("/");
                if (-1 == index)
                    throw new Exception("can not find /!!!");
                return path.Substring(index + 1, path.Length - index - 1);
            }
        }

        public static string getFolder(string path)
        {
            path = path.Replace("\\", "/");
            int index = path.LastIndexOf("/");
            if (-1 == index)
                throw new Exception("can not find /!!!");
            return path.Substring(index + 1, path.Length - index - 1);
        }

        public static string getFileSuffix(string filePath)
        {
            int index = filePath.LastIndexOf(".");
            if (-1 == index)
                throw new Exception("can not find Suffix!!! the filePath is : " + filePath);
            return filePath.Substring(index + 1, filePath.Length - index - 1);
        }

        public static void getFiles(string path, bool recursion, Dictionary<string, List<string>> allFiles, bool useSuffix, string suffix)
        {
            if (recursion)
            {
                string[] dirs = Directory.GetDirectories(path);
                foreach (string dir in dirs)
                {
                    if (getFolder(dir) == ".svn")
                        continue;
                    getFiles(dir, recursion, allFiles, useSuffix, suffix);
                }
            }

            string[] files = Directory.GetFiles(path);
            foreach (string file in files)
            {
                string fileSuffix = getFileSuffix(file);
                if (fileSuffix == "meta" || fileSuffix == "dll")
                    continue;
                if (useSuffix && fileSuffix != suffix)
                    continue;
                string relativePath = file.Replace("\\", "/");
                relativePath = relativePath.Replace(Application.dataPath, "");
                string fileName = getFileName(file, true);
                if (allFiles.ContainsKey(fileName))
                {
                    allFiles[fileName].Add(relativePath);
                }
                else
                {
                    List<string> list = new List<string>();
                    list.Add(relativePath);
                    allFiles.Add(fileName, list);
                }
            }
        }

        public static void CheckFolder(string path)
        {
            if (!Directory.Exists(path))
                Directory.CreateDirectory(path);
        }

        public static string getPath(string filePath)
        {
            string path = filePath.Replace("\\", "/");
            int index = path.LastIndexOf("/");
            if (-1 == index)
                throw new Exception("can not find /!!!");
            return path.Substring(0, index);
        }

        public static string getLocalPath(string path)
        {
            string localPath = string.Format("{0}/{1}", Application.persistentDataPath, path);
            if (!File.Exists(localPath))
            {
                if (Application.platform == RuntimePlatform.Android)
                {
                    localPath = string.Format("{0}/{1}", Application.streamingAssetsPath, path);
                }
                else if (Application.platform == RuntimePlatform.WindowsEditor || Application.platform == RuntimePlatform.WindowsPlayer)
                {
                    localPath = string.Format("file://{0}/{1}", Application.streamingAssetsPath, path);
                }
                return localPath;
            }
            return "file:///" + localPath;
        }

        public static string getServerPath(string path)
        {
            return string.Format("{0}/{1}", mServerAssetPath, path);
        }

        public static byte[] getAssetBundleFileBytes(string path, ref int size)
        {
            string localPath;

            //Andrio跟IOS环境使用沙箱目录
            if (Application.platform == RuntimePlatform.Android || Application.platform == RuntimePlatform.IPhonePlayer)
            {
                localPath = string.Format("{0}/{1}", Application.persistentDataPath, path + ResourceCommon.assetbundleFileSuffix);
            }
            //Window下使用assetbunlde资源目录
            else
            {
                localPath = ResourceCommon.assetbundleFilePath + path + ResourceCommon.assetbundleFileSuffix;
            }

            Debug.Log(localPath);

            //首先检测沙箱目录中是否有更新资源         
            if (File.Exists(localPath))
            {
                try
                {
                    FileStream bundleFile = File.Open(localPath, FileMode.Open, FileAccess.Read);
                    byte[] bytes = new byte[bundleFile.Length];
                    bundleFile.Read(bytes, 0, (int)bundleFile.Length);
                    size = (int)bundleFile.Length;
                    bundleFile.Close();
                    return bytes;
                }
                catch (Exception e)
                {
                    DebugEx.LogError(e.Message, "CreateAssetBunlde", true);
                    return null;
                }
            }
            //原始包中
            else
            {
                TextAsset bundleFile = Resources.Load(path) as TextAsset;

                if (null == bundleFile)
                    DebugEx.LogError("load : " + path + " bundleFile error!!!", "CreateAssetBunlde", true);
                size = bundleFile.bytes.Length;
                return bundleFile.bytes;
            }
        }
        public delegate void Handle_CreateFromMemory(AssetBundleCreateRequest request, int size);
        public static IEnumerator _CreateFromMemory(string path, Handle_CreateFromMemory handle)
        {
            int size = 0;
            AssetBundleCreateRequest bundleRequest = AssetBundle.CreateFromMemory(ResourceCommon.getAssetBundleFileBytes(path, ref size));
            yield return bundleRequest;
            handle(bundleRequest, size);
        }

        public delegate void HandleFinishLoadAsyncFromAssetBundle(AssetBundleRequest request);
        public static IEnumerator LoadAsyncFromAssetBundle(AssetBundle assetbundle, string name, System.Type type, HandleFinishLoadAsyncFromAssetBundle handle)
        {
            AssetBundleRequest request = assetbundle.LoadAsync(name, type);
            yield return request;
            handle(request);
        }

        //仅用于win_editor
        public static AssetBundle loadAssetBundleImmediateForDebug(string path)
        {
            string localPath = string.Format("file://{0}/{1}", assetbundleFilePath, path + ResourceCommon.assetbundleFileSuffix);
            WWW www = new WWW(localPath);

            if (!string.IsNullOrEmpty(www.error))
                Debug.LogError("the assetbundle path is error : " + path + "\nand the localPath is : " + localPath);

            while (true)
            {
                byte[] b = www.bytes;
                if (www.isDone)
                    break;
            }
            return www.assetBundle;
        }
    }

}