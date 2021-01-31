using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using GameDefine;
using BlGame.Resource;

public class ReadMapObjConfig  {

	XmlDocument xmlDoc = null;

    public ReadMapObjConfig(string xmlFilePath)
	{

		//TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

		if(!xmlfile)
		{
			Debug.LogError(" error infos: 没有找到指定的xml文件："+xmlFilePath);
		}		

		xmlDoc = new XmlDocument();
		xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("MapObjCfg ").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null) continue;
			
			string tpName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;

            MapObjConfigInfo objInfo = new MapObjConfigInfo();
            objInfo.un32Id = Convert.ToInt32(tpName);
            //NpcSelectInfo.NpcId = Convert.ToInt32(typeName);
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
				switch(xEle.Name)
				{
                case "un32MapID":
                   objInfo.un32MapID = Convert.ToInt32(xEle.InnerText);
					break;
                case "eObjectTypeID":
                    objInfo.eObjectTypeID = Convert.ToInt32(xEle.InnerText);
                    break;
                case "un32ObjIdx":
                    objInfo.un32ObjIdx = Convert.ToInt32(xEle.InnerText);
                    break;
                case "n32Camp":
                    objInfo.n32Camp = Convert.ToInt32(xEle.InnerText);
                    break;
                //case "cBornPos":
                //    objInfo.cBornPos = GameMethod.ResolveToVector3(xEle.InnerText);
                //    break;
                case "cBornDir":
                    objInfo.cBornDir = GameMethod.ResolveToVector3(xEle.InnerText);
                    break;
                case "ReplaceID":
                    objInfo.ReplaceID = xEle.InnerText;
                    break;
                case "n32Visibility":
                    objInfo.n32Visibility = Convert.ToInt32(xEle.InnerText);
                    break;
                case "eAttackMode":
                    objInfo.eAttackMode = Convert.ToInt32(xEle.InnerText);
                    break;	
				}
				#endregion
			}
            ConfigReader.mapObjXmlInfoDict.Add(Convert.ToInt32(tpName), objInfo);
		}
	}
}

public class MapObjConfigInfo: System.Object
{
	#region NPC信息
    public int un32Id;
    public int un32MapID;
    public int eObjectTypeID;
    public int un32ObjIdx;
    public int n32Camp;
    public Vector3 cBornPos;
    public Vector3 cBornDir;
    public string ReplaceID;
    public int n32Visibility;
    public int eAttackMode;
	#endregion
	
}
