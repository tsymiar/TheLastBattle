using UnityEngine;
using BlGame;
using System.Runtime.InteropServices;
using BlGame.Resource;

public class SdkManager : Singleton<SdkManager>
{
    public enum EPlatform
    {
        //IOS平台SDK
        ePlatform_None = 0,
        ePlatform_IOS_91 = 1,
        ePlatform_IOS_TB = 2,
        ePlatform_IOS_PP = 3,
        ePlatform_IOS_CMGE = 4,
        ePlatform_IOS_UC = 5,
        ePlatform_IOS_iTools = 6,
        ePlatform_IOS_OnlineGame = 7,
        ePlatform_IOS_As = 8,
        ePlatform_IOS_XY = 9,
        ePlatform_IOS_CMGE_ZB = 10,


        //Andriod平台SDK
        ePlatform_Android_UC = 105,
    };

    public EPlatform sdkType;

    const string sdkPath = "Guis/SdkConnect";
    void CreateSdk(string path)
    {
        ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
        GameObject connector = GameObject.Instantiate(objUnit.Asset) as GameObject;
        connector.name = "SdkConnect";
    }

    public void CreateSdkPrefab(EPlatform type) 
    {
        //测试
        Debugger.Log("CreateSdkPrefab " + type.ToString());
        
        sdkType = type;
        if (type == EPlatform.ePlatform_None)
            return;
        CreateSdk(sdkPath);
    }

    public EPlatform GetPlatFrom() {
        return sdkType;
    } 
}
