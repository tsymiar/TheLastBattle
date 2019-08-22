using UnityEngine; 
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using System;


using System.IO; 
using System.Runtime.InteropServices; 
using System.Text; 
using JT.FWW.Tools;
public class SceneMapData
{
 
	//[StructLayout(LayoutKind.Sequential)]
	public struct SMapDataHeader
	{
		public UInt32 un32MapID;
		public UInt32 mapHeight;
		public UInt32 mapWidth;
	};
	//[StructLayout(LayoutKind.Sequential)]

	public struct SMapDataStruct{
		public SMapDataHeader sHeader;
		public byte []BlockData;
	};

	public const string layerName  = "MapData";

	public SMapDataStruct mapDataStruct = new SMapDataStruct();
	 
	private static SceneMapData instance = null;
	public static SceneMapData Instance {
		get
		{
			if(instance == null)
			{
				instance = new SceneMapData();
			}
			return instance;
		}
	}
 
}

public class ImportMapData : EditorWindow
{	 
	[MenuItem ("BoLong/BatchReplaceObjecsts/ImportMapData")]


	static void Init()
	{	 
		ImportMapData window = (ImportMapData)EditorWindow.GetWindow (typeof (ImportMapData));
		window.title = "生成地图数据";
	}
	 
	private static void CreateCube(){

        //CollsionDetection.createCollsionObject (MapData.mapLength,MapData.mapWidth,MapData.layerName);
	}

	//#region mapHeader
	private static void GetMapDataHeader(MapObjDataManager manager)
	{
        SceneMapData.SMapDataHeader head = new SceneMapData.SMapDataHeader();
        head.un32MapID = (UInt32)(manager.MapName);
        head.mapWidth = (UInt32)(manager.MapWidth);
        head.mapHeight = (UInt32)(manager.MapHeight);
        SceneMapData.Instance.mapDataStruct.sHeader = head;
        Debug.Log("MapData.Instance.headData.mapWidth  " + SceneMapData.Instance.mapDataStruct.sHeader.mapWidth + " xx " + SceneMapData.Instance.mapDataStruct.sHeader.mapHeight);
	}

//
//	#endregion
//
	#region map block
	private static void GetMapBlockData(MapObjDataManager manager)
	{
        int width = manager.MapWidth;
        int height = manager.MapHeight;

        byte []abBlockData;
        //SceneMapData.Instance.MapData.abBlockData = new byte[width * height];
        CollsionDetection.SetCollisionBuf(width, height, SceneMapData.layerName, out abBlockData);
        SceneMapData.Instance.mapDataStruct.BlockData = abBlockData;
	}
	#endregion

	private static CMemBuffer Export(){
		
		CMemBuffer pcBuffer = new CMemBuffer (1024 * 1024 * 3);

		#region 表头
        SceneMapData.SMapDataStruct data = SceneMapData.Instance.mapDataStruct;
        pcBuffer.Add(data.sHeader.un32MapID);
        pcBuffer.Add(data.sHeader.mapWidth);
        pcBuffer.Add(data.sHeader.mapHeight);
		#endregion

		#region 阻挡数据
        int size = (int)data.sHeader.mapHeight * (int)data.sHeader.mapWidth;
        pcBuffer.Add(SceneMapData.Instance.mapDataStruct.BlockData, 0 , size);
	
		#endregion
		return pcBuffer;		
	}


//	static byte[]headerBytes;
//	static byte[]blockBytes;
//	static byte[]objectrBytes;
	private static void ImportInfoToFile()
	{
        MapObjDataManager manager = GameObject.FindObjectOfType(typeof(MapObjDataManager)) as MapObjDataManager;
        GetMapDataHeader(manager);
        GetMapBlockData(manager);
		
        string filePath = Application.dataPath + "/Resources/Map/" + manager .MapName.ToString()+ ".map"; 

		CMemBuffer buf = Export();
		int un32DataSize = buf.GetWritePos ();	

		FileStream fs = new FileStream (filePath, FileMode.OpenOrCreate, FileAccess.Write);	
		BinaryWriter bw = new BinaryWriter (fs);
		bw.Write (buf.GetMemBuffer (),0, un32DataSize);
		Debug.LogError ("write = " + un32DataSize);
		fs.Close ();
		bw.Close ();

	}

    /// <summary>
    /// 通过读取Map文件获得阻挡信息
    /// </summary>
	private static void ReadInfoFromFile()
	{
		MapObjDataManager manager = GameObject.FindObjectOfType (typeof(MapObjDataManager)) as MapObjDataManager;
        string filePath = Application.dataPath + "/Resources/Map/" + manager.MapName.ToString() + ".map"; 
		FileStream fs = new FileStream (filePath, FileMode.Open, FileAccess.Read);	
		BinaryReader br = new BinaryReader (fs);
        int MapId = br.ReadInt32();
        int MapWidth = br.ReadInt32();
        int MapHeight = br.ReadInt32();
        int MapLength = MapWidth * MapHeight;
        GameObject cubeParent = new GameObject("CubeParent");
        cubeParent.transform.position = Vector3.zero;
        //MapLength = 20000;
        //GameObject obj_parent = UnityEngine.Object.Instantiate(Resources.Load("Cube/Cubebb"), Vector3.zero, Quaternion.identity) as GameObject; 
        for (int index = 0; index < MapLength; index++)
        {
            byte info = br.ReadByte();
            if (info == 1)
            {
                InstanceCubeByMapIndex(index, cubeParent, manager);
            }
        }
		fs.Close ();
		br.Close ();
	}

    /// <summary>
    /// 创建阻挡格子
    /// </summary>
    /// <param name="index"></param>
    /// <param name="obj_parent"></param>
    private static void InstanceCubeByMapIndex(int index, GameObject obj_parent, MapObjDataManager manager)
    {
        int row = (int)(index / manager.MapWidth);
        float posZ = row + 0.5f;
        float posX = (index - row * manager.MapWidth) + 0.5f;
        GameObject tobj = GameObject.CreatePrimitive(PrimitiveType.Cube);
        tobj.transform.position = new Vector3(posX, 100.0f, posZ);
        tobj.transform.parent = obj_parent.transform; 
    }


	void OnGUI ()
	{     
		if (GUILayout.Button ("生成地图盒子")) {
			CreateCube ();
		}
		if (GUILayout.Button ("导出地图数据")) {
			ImportInfoToFile();
		} 

		if (GUILayout.Button ("读取地图数据，生成盒子")) {
			ReadInfoFromFile();
		} 
       // if(GUILayout.lp)
	}
}
