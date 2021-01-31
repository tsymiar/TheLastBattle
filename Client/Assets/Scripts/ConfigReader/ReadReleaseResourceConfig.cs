using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame;
using BlGame.Resource;

//读取需要强制释放的资源信息
class ReadReleaseResourceConfig : Singleton<ReadReleaseResourceConfig>
{
    //需要强制释放的资源
    private Dictionary<string, List<string>> mResList = new Dictionary<string, List<string>>();
   

    XmlDocument xmlDoc = null;
    public ReadReleaseResourceConfig()
    {
    }

    //需要强制释放的资源
    public void Init()
    {
        LoadConfig("Config/ReleaseResource");
    }


    //强制释放资源aaset
    public void ForceReleaseResource()
    {
        //残留的贴图资源
        Texture[] textures = Resources.FindObjectsOfTypeAll<Texture>();
        if (textures.Length == 0)
            return;

        foreach (KeyValuePair<string, List<string>> typeList in mResList)
        {
            string type = typeList.Key;
            if (type == "texture")
            {
                List<string> texList = typeList.Value;
                foreach (Texture texture in textures)
                {
                    string textureName = texture.name;

                    //匹配需要强制删除的贴图资源
                    if (texList.Contains(textureName))
                    {
                        Resources.UnloadAsset(texture);
                        //Debug.Log("unload texture asset" + textureName);
                    }
                }            
            }
        }        
    }



    private void LoadConfig(string xmlFilePath)
    {
        mResList.Clear();


        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNode resources = xmlDoc.SelectSingleNode("ResourceList");
        if (resources != null)
        {
            XmlNodeList resourceTypeList = resources.ChildNodes;
            //遍历所有类型资源
            foreach (XmlNode resourceType in resourceTypeList)
            {
                string typeName = resourceType.Attributes["type"].Value;

                List<string> sList = new List<string>();

                //遍历所有资源
                foreach (XmlNode res in resourceType)
                {
                    string resName = res.Attributes["name"].Value;

                    sList.Add(resName);
                }

                mResList.Add(typeName, sList);
            }
        }

        int s = 0;
        s++;
        
    }
};

