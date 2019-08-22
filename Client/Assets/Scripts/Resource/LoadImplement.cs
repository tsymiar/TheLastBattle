using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using GameDefine;
namespace BlGame.Resource
{
    public delegate void OnResLoaded(string path);
    public delegate void OnResLoadError(string path);
    public class LoadImplement
    {
        public string resPath;
        public PhyResouce.EPhyResType phyResType;
        public void start()
        {
            
            string path = GameConstDefine.ResRootPath + resPath;
            www = new WWW(path);
        }
        public WWW www;
        public AssetBundleCreateRequest createReq;
        public OnResLoaded onResLoaded;
        public OnResLoadError onResLoadError;
    }
}
