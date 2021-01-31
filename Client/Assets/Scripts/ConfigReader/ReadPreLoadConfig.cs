using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using BlGame;
using BlGame.Resource;

class ReadPreLoadConfig : Singleton<ReadPreLoadConfig>
{
    private Dictionary<int, List<string>> roleFile = new Dictionary<int, List<string>>();
    private List<string> sceneFile = new List<string>();

    //所有需要预加载的场景以及影响资源信息
    private List<string> allFiles = new List<string>();
    //所有需要预加载的场景UITip
    private List<string> uiTips = new List<string>();
 
    XmlDocument xmlDoc = null;
    public ReadPreLoadConfig()
    {                       
    }

    //初始角色预加载信息
    public void Init()
    {
        LoadConfig("Config/PreLoad");
    }

    //清除预加载信息 
    public void Clear()
    {
        allFiles.Clear();    
    }


    //进入场景后预加载特效
     public void PreLoad()
    {
        //预加载特效
        AddPreLoadScene();

        foreach (string file in allFiles)
        {                       
            GameObject preObj = GameObjectPool.Instance.GetGO(file);
            GameObjectPool.Instance.ReleaseGO(file, preObj, PoolObjectType.POT_Effect);
        }

        //预先加载uiTip
        List<GameObject> uiTipObjs = new List<GameObject>();
        foreach (string tipName in uiTips)
        {
            GameObject tipObj = GameObjectPool.Instance.GetGO(tipName);
            tipObj.name = tipName;
            uiTipObjs.Add(tipObj);
        }

        foreach (GameObject tipObj in uiTipObjs)
        {
            GameObjectPool.Instance.ReleaseGO(tipObj.name, tipObj, PoolObjectType.POT_UITip);
        }         
    }


    //添加预加载英雄特效
    public void AddPreLoadRoleEffect(int heroId)
    {
        if (!roleFile.ContainsKey(heroId))
        {
            Debug.LogError("PreLoadRoleEffect hero is not exist id = " + heroId);
            return;
        }

       

        List<string> files = roleFile[heroId];

        foreach (string file in files)
        {
            GameObject preObj = GameObjectPool.Instance.GetGO(file);
            GameObjectPool.Instance.ReleaseGO(file, preObj, PoolObjectType.POT_Effect);
        }
    }

    //添加预加载场景特效包括所有的吸附技能特效
    public void AddPreLoadScene()
    {
        foreach (string file in sceneFile)
        {
            allFiles.Add(file);
        }
    }
   
    public List<String> GetScene()
    {
        return sceneFile;
    }

    private void LoadConfig(string xmlFilePath)
    {
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

        xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(xmlfile.text);

        XmlNode preLoad = xmlDoc.SelectSingleNode("PreloadEffect");

        if (preLoad != null)
        {
            XmlNode roles = preLoad.SelectSingleNode("Roles");
            if (roles != null)
            {
                XmlNodeList roleList = roles.ChildNodes;
                if (roleList != null)
                {
                    foreach (XmlNode role in roleList)
                    {
                        int roleId = Convert.ToInt32(role.Attributes["id"].Value);
                        List<string> list = new List<string>();

                        XmlNodeList effects = role.ChildNodes;

                        if (effects != null)
                        {
                            foreach (XmlNode effect in effects)
                            {
                                string path = effect.InnerText;

                                list.Add(path);
                            }
                        }

                        roleFile.Add(roleId, list);
                    }
                }
            }


            XmlNode scene = preLoad.SelectSingleNode("Scene");
            if (scene != null)
            {
                XmlNodeList objects = scene.ChildNodes;

                if (objects != null)
                {
                    foreach (XmlNode obj in objects)
                    {
                        string objs = obj.InnerText;

                        sceneFile.Add(objs);
                    }
                }
            }

            //UITip
            XmlNode tips = preLoad.SelectSingleNode("UITips");
            if (tips != null)
            {
                XmlNodeList tipObjs = tips.ChildNodes;

                if (tipObjs != null)
                {
                    foreach (XmlNode tipObj in tipObjs)
                    {
                        string tipName = tipObj.InnerText;
                        uiTips.Add(tipName);
                    }
                }
            }
        }
    }
};

