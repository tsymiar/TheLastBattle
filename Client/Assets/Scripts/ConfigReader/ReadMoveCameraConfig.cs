using UnityEngine;
using System.Collections;
using System;
using System.Xml;
using BlGame.Resource;
using GameDefine;

public class ReadMoveCameraConfig {

    XmlDocument xmlDoc = null;

    public ReadMoveCameraConfig(string xmlFilePath)
	{
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

		if(!xmlfile)
		{
			Debug.LogError(" error infos: 没有找到指定的xml文件："+xmlFilePath);
		}		

		xmlDoc = new XmlDocument();
		xmlDoc.LoadXml(xmlfile.text);

        XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("CameraRemotion ").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//(XmlNode xNode in infoNodeList)
		{
            if ((infoNodeList[i] as XmlElement).GetAttributeNode("un32MapID") == null) continue;
			
			string tpName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32MapID").InnerText;

            CameraRemotionConfigInfo cameraInfo = new CameraRemotionConfigInfo();
            cameraInfo.un32MapID = Convert.ToInt32(tpName);
            //NpcSelectInfo.NpcId = Convert.ToInt32(typeName);
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
				switch(xEle.Name)
				{
                case "LeftEdge":
                    cameraInfo.mLeftEdge = GameMethod.ResolveToVector3(xEle.InnerText);
					break;
                case "RightEdge":
                    cameraInfo.mRightEdge = GameMethod.ResolveToVector3(xEle.InnerText);
                    break;
                case "TopEdge":
                    cameraInfo.mTopEdge = GameMethod.ResolveToVector3(xEle.InnerText);
                    break;
                case "BottomEdge":
                    cameraInfo.mBottomEdge = GameMethod.ResolveToVector3(xEle.InnerText);
                    break;
                case "MovementSpeed":
                    cameraInfo.mMovementSpeed = Convert.ToInt32(xEle.InnerText);
                    break;
				}
				#endregion
			}
            ConfigReader.cameraXmlInfoDict.Add(Convert.ToInt32(tpName), cameraInfo);
		}
	}
}
public class CameraRemotionConfigInfo : System.Object
{
    #region NPC信息
    public int un32MapID;
    public Vector3 mLeftEdge;
    public Vector3 mRightEdge;
    public Vector3 mTopEdge;
    public Vector3 mBottomEdge;
    public int mMovementSpeed;
    #endregion

}