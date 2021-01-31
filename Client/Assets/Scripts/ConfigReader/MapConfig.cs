using UnityEngine;
using System.Collections;
using System.Collections.Generic;

using GameDefine;
using System.IO;

public static class MapConfig{

	private static EanFile ReadInfoFromFile(string path){
		if (!File.Exists(path))
		{
			Debug.LogError("wrong ean file path!"+path);
			return null;
		}
		FileStream fs = new FileStream(path, FileMode.Open);
		BinaryReader br = new BinaryReader(fs);
		EanFile ean = new EanFile();
		ean.Load(br, fs);
		fs.Close();
		return ean;
	}

	private static Dictionary<int , EanFile> MapDataInfo = new Dictionary<int, EanFile> ();
	private static EanFile GetMapDataById(int mapId){
		if (!MapDataInfo.ContainsKey (mapId)) {
            string path = mapId.ToString() + ".map";
            MapDataInfo.Add(mapId, ReadInfoFromFile(@"Assets/Resources/Map/" + path));
		}
		return MapDataInfo[mapId];	
	}

	public static bool IsMapBlock(int mapId , Vector3 pos){
		int MapRowZ = (int)pos.z * 2;
		int MapRowX = (int)pos.x * 2;
		int MapIndex = MapRowZ * 400 + MapRowX; 
		if (GetMapDataById (mapId).MapData.ContainsKey(MapIndex)){
			return true;		
		}
		return false;
	}

	public class EanFile
	{
		public int MapId;
		public int MapWidth;	
		public int MapHeight;
		public int MapLength;
		public Dictionary<int , byte> MapData = new Dictionary<int, byte> ();

		public void Load(BinaryReader br, FileStream fs)
		{
			MapId = br.ReadInt32();
			MapWidth = br.ReadInt32();
			MapHeight = br.ReadInt32();
			MapLength = MapWidth * MapHeight;
			for (int index = 0; index < MapLength; index++) {
				byte info = br.ReadByte();
				if(info == 1){
					MapData.Add(index , info);
				}
			}
		}
	}
}