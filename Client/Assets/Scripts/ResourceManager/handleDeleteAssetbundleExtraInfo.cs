using System;
using System.Collections.Generic;

namespace BlGame.Resource
{
    //如果mesh,audio,animatioclip等资源的assetbundle对象会造成内存占用瓶颈的话,再添加处理
    public class handleDeleteAssetBundleExtraInfo
    {
        //texName,properityName(不存在texName同名的情况,工程中不允许存在同名文件(具体查看assetbundle对象规则))
        public Dictionary<string, string> mAllTextureNameInShader;

        public handleDeleteAssetBundleExtraInfo()
        {
            mAllTextureNameInShader = new Dictionary<string, string>();
        }

        public void export(ref string info)
        {
            info += mAllTextureNameInShader.Count.ToString() + ";";
            foreach (KeyValuePair<string, string> p in mAllTextureNameInShader)
            {
                info += p.Key + ";" + p.Value + ";";
            }
        }
    }

}