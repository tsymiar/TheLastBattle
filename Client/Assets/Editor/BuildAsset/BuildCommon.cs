using System;
using System.IO;
using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using Object = UnityEngine.Object;


    public class BuildCommon
    {

        public static string getFolder(string path)
        {
            path = path.Replace("\\", "/");
            int index = path.LastIndexOf("/");
            if (-1 == index)
                throw new Exception("can not find /!!!");
            return path.Substring(index + 1, path.Length - index - 1);
        }

        public static string getFileName(string fileName)
        {
            int index = fileName.IndexOf(".");
            if (-1 == index)
                throw new Exception("can not find .!!!");
            return fileName.Substring(0, index);
        }

        public static string getFileName(string filePath,bool suffix)
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

        public static string getFileSuffix(string filePath)
        {
            int index = filePath.LastIndexOf(".");
            if (-1 == index)
                throw new Exception("can not find Suffix!!! the filePath is : " + filePath);
            return filePath.Substring(index + 1, filePath.Length - index - 1);
        }

        public static void getFiles(string path, ref Dictionary<string, AssetUnit> list)
        {
            string[] dirs = Directory.GetDirectories(path);
            foreach (string dir in dirs)
            {
                if (getFolder(dir) == ".svn")
                    continue;
                getFiles(dir, ref list);
            }

            string[] files = Directory.GetFiles(path);
            foreach (string file in files)
            {
                string suffix = getFileSuffix(file);
                if (suffix == "meta" || suffix == "dll")
                    continue;
                string realFile = file.Replace("\\", "/");
                realFile = "Assets" + realFile.Replace(Application.dataPath,"");
                list.Add(realFile, new AssetUnit(realFile));
            }
        }

        public static void GetFiles(string path, List<string> list, bool recursion)
        {
            if (recursion)
            {
                string[] dirs = Directory.GetDirectories(path);
                foreach (string dir in dirs)
                {
                    if (getFolder(dir) == ".svn")
                        continue;
                    GetFiles(dir, list, recursion);
                }
            }

            string[] files = Directory.GetFiles(path);
            foreach (string file in files)
            {
                string suffix = getFileSuffix(file);
                if (suffix == "meta")
                    continue;
                string realFile = file.Replace("\\", "/");
                realFile = realFile.Replace(Application.streamingAssetsPath + "/", "");
                list.Add(realFile);
            }
        }

        public static void getFloder(string path, string speicalName, bool recursion, List<string> allPath)
        {
            if (recursion)
            {
                string[] dirs = Directory.GetDirectories(path);
                foreach (string dir in dirs)
                {
                    if (getFolder(dir) == speicalName)
                        allPath.Add(dir);
                    getFloder(dir, speicalName, recursion, allPath);
                }
            }
        }

        public static int getAssetLevel(string filePath)
        {
            string[] depencys = AssetDatabase.GetDependencies(new string[] { filePath });

            List<string> deps = new List<string>();

            foreach (string file in depencys)
            {
                //排除关联脚本
                string suffix = BuildCommon.getFileSuffix(file);
                //if (suffix == "dll" || suffix == "cs")
                if (suffix == "dll")
                    continue;

                deps.Add(file);
            }

            if (deps.Count == 1)
                return 1;

            int maxLevel = 0;
            foreach (string file in deps)
            {
                if (file == filePath)
                    continue;
                int level = getAssetLevel(file);
                maxLevel = maxLevel > level + 1 ? maxLevel : level + 1;
            }
            return maxLevel;
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

        public static bool isDependenced(string asset,string sourceAsset)
        {
            string[] deps = AssetDatabase.GetDependencies(new string[] { sourceAsset });
            bool isDep = false;
            foreach (string path in deps)
            {
                if (path == sourceAsset)
                    continue;

                if (path == asset)
                    return true;
                isDep = isDependenced(asset,path);
            }
            return isDep;
        }

        public static bool isSingleDependenced(AssetUnit asset)
        {
            if (asset.mDirectUpperDependences.Count > 1)
                return false;
            else if (asset.mDirectUpperDependences.Count == 1)
                return isSingleDependenced(asset.mDirectUpperDependences[0]);
            else
                return true;
        }

        //public static void getAllUpperDependence(AssetUnit asset,ref string info)
        //{
        //    info += "name is : " + asset.mPath + "\n";
        //    info += "begin\n";
        //    foreach (AssetUnit p in asset.mDirectUpperDependences)
        //    {
        //        info += p.mPath + "\n";
        //    }
        //    info += "end\n";
        //    foreach (AssetUnit p in asset.mDirectUpperDependences)
        //    {
        //        getAllUpperDependence(p,ref info);
        //    }
        //}

        //public static void getNextLevelDependencedAllAssetSize(AssetUnit asset, List<string> LeveldependencedPrefabs, ref int count)
        //{
        //    Object mainAsset = AssetDatabase.LoadMainAssetAtPath(asset.mPath);
        //    bool isBatchingStatic = ((asset.mSuffix == "prefab") &&
        //        (StaticEditorFlags.BatchingStatic == (StaticEditorFlags.BatchingStatic & GameObjectUtility.GetStaticEditorFlags(mainAsset as GameObject))));

        //    int mCurrentIndexCount = 0;
        //    foreach(AssetUnit nextdeps in asset.mNextLevelDependencies)
        //    {  
        //        //踢出独立的预制件
        //        if (nextdeps.mPath.Contains(".prefab") && !LeveldependencedPrefabs.Contains(nextdeps.mPath))
        //        {
        //            _writePrefabDependence(LeveldependencedPrefabs, nextdeps, ref count);
        //            continue;
        //        }

        //        //剔除静态批次prefab依赖的fbx
        //        if (isBatchingStatic && nextdeps.mPath.ToLower().Contains(".fbx") && !nextdeps.mPath.ToLower().Contains("_coll"))
        //        {
        //            foreach (AssetUnit fbxAssetdps in nextdeps.mNextLevelDependencies)
        //            {
        //                getNextLevelDependencedAllAssetSize(fbxAssetdps, LeveldependencedPrefabs, ref count);
        //                ++mCurrentIndexCount;
        //            }
        //            continue;
        //        }

        //        getNextLevelDependencedAllAssetSize(nextdeps, LeveldependencedPrefabs, ref count);
        //        ++mCurrentIndexCount;
        //    }
        //    count += mCurrentIndexCount;
        //}

        //public static void _writePrefabDependence(List<string> LeveldependencedPrefabs, AssetUnit prefab, ref int count)
        //{
        //    foreach (AssetUnit asset in prefab.mNextLevelDependencies)
        //    {
        //        if ((asset.mPath.Contains(".prefab") && !LeveldependencedPrefabs.Contains(asset.mPath)))
        //        {
        //            _writePrefabDependence(LeveldependencedPrefabs, asset,ref count);
        //            continue;
        //        }
        //        getNextLevelDependencedAllAssetSize(asset, LeveldependencedPrefabs, ref count);
        //        ++count;
        //    }
        //}

        //public static void writePrefabDependence(ref AssetExtraInfo extraInfo, List<string> LeveldependencedPrefabs, AssetUnit prefab)
        //{
        //    foreach(AssetUnit asset in prefab.mNextLevelDependencies)
        //    {
        //        if ((asset.mPath.Contains(".prefab") && !LeveldependencedPrefabs.Contains(asset.mPath)))
        //        {
        //            writePrefabDependence(ref extraInfo, LeveldependencedPrefabs, asset);
        //            continue;
        //        }
        //        extraInfo.mAllDps.Add(asset.mPath.Replace("Assets/", ""));
        //    }
        //}
    }
