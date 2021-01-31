using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;

public class AssetUnit
{
    public string mPath;                            //文件路径相对Asset      
    public string mName;                            //文件名称,无后缀名称
    public string mSuffix;                          //后缀
    public int mLevel;                              //层数
    public int mIndex;                              //索引
    public int mAssetSize;                          //保存该Asset的打包文件大小
    public List<string> mAllDependencies;           //所有依赖
    public List<AssetUnit> mNextLevelDependencies;  //下层依赖
    public List<AssetUnit> mDirectUpperDependences; //直接上层依赖,用于判定assetbundle加载后是否可以马上删除

    public AssetUnit(string path)
    {
        mAssetSize = 0;
        mPath = path;
        mName = BuildCommon.getFileName(mPath, true);
        mSuffix = BuildCommon.getFileSuffix(mName);
        mLevel = BuildCommon.getAssetLevel(mPath);
        mAllDependencies = new List<string>();
        mNextLevelDependencies = new List<AssetUnit>();
        string[] deps = AssetDatabase.GetDependencies(new string[] { mPath });

        foreach (string file in deps)
        {
            string suffix = BuildCommon.getFileSuffix(file);
            //if (file == mPath || suffix == "dll" || suffix == "cs")
            if (file == mPath || suffix == "dll")
                continue;
            mAllDependencies.Add(file);
        }
        mDirectUpperDependences = new List<AssetUnit>();
    }
}
   
