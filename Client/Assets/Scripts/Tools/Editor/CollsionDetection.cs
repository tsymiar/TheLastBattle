
using UnityEngine; 
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System; 

public class CollsionDetection:MonoBehaviour
{
	public Transform obj;

	void Start()
	{

	}
	void Update()
	{

	}
	/// <summary>
	/// Creates the block file.
	/// </summary>
	/// <param name="map_width">Map_width.</param>
	/// <param name="map_height">Map_height.</param>
	/// <param name="fileName">File name.</param>
	/// <param name="layer_name">Layer_name.</param> 
	public static void  CreateTxtFile( float map_width, float map_height, string fileName, string layer_name )
	{
		
 
		byte tag = 0;
		int layer = 1 << LayerMask.NameToLayer(layer_name);		
	//	string filePath = "E:\\"+fileName+".dat"; 
		FileStream fs = new FileStream (fileName, FileMode.OpenOrCreate, FileAccess.Write);	
		BinaryWriter bw = new BinaryWriter (fs); 

		for(int i = 0;i < map_height;i++)
		{ 
			for(int j = 0;j < map_width;j++)
			{

				float a =  Convert.ToSingle(j / 2.00f + 0.25f);			 
				float b =Convert.ToSingle(i / 2.00f + 0.25f);		 
				Vector3 pos = new Vector3(a,66.00f,b);
				bool isBunker = Physics.CheckSphere(pos,0.2f,layer); 

				if(isBunker || (i == 0 && j == 0) || ((i == map_height -1) && (j == map_width -1)) )
				{
					tag = 1;
				}
				else
				{
					tag = 0;
				}

				if(i == 0 && j == 0)
				{
					tag = 1;
				} 
				bw.Write(tag); 
			} 
		} 
		fs.Close ();
		bw.Close ();
	}

	public static void SetCollisionBuf(int map_width, int map_height,string layer_name ,out byte [] buf)
	{
        Debug.Log("map_width  " + map_width + " height " + map_height);
		int layer = 1 << LayerMask.NameToLayer(layer_name);		
		byte tag = 0;
        buf = new byte[map_width * map_height];
		for(int i = 0;i < map_height;i++)
		{ 
			for(int j = 0;j < map_width;j++)
			{
                float a =  Convert.ToSingle(j + 0.5f);			 
                float b = Convert.ToSingle(i + 0.5f);		 
                Vector3 pos = new Vector3(a,66.00f,b);
				bool isBunker = Physics.CheckSphere(pos,0.5f,layer); 
				
				if(isBunker)
				{
					tag = 1;
				}
				else
				{
					tag = 0;
				}

				buf[j+i*map_width] = tag;
                //if ((j + i * map_width) % 10 == 0)
                //{
                //    Debug.Log(tag);
                //}
			} 
		} 

		buf [0] = 1;
		buf [map_width * map_height -1] = 1;
	}	 
	#region 创建碰撞体
	public static void createCollsionObject( float map_width, float map_height, string layer_name )
	{ 
		//Created BY Run ----Start------------
		 
		int tag = 0;
		int layer = 1 << LayerMask.NameToLayer(layer_name);
		GameObject obj_parent = UnityEngine.Object.Instantiate (Resources.Load ("Cube/Cubebb"), Vector3.zero, Quaternion.identity) as GameObject; 
		for(int i = 0;i < map_height;i++)
		{
			for(int j = 0;j < map_width;j++)
			{
				float a =  Convert.ToSingle(j / 2.00f + 0.25f);			 
				float b =Convert.ToSingle(i / 2.00f + 0.25f);		 
				Vector3 pos = new Vector3(a,66.00f,b);
				bool isBunker = Physics.CheckSphere(pos,0.2f,layer);
				if(i == 0 && j < 5)
				{
					tag = 1;
				} 

				if(isBunker)
				{
					tag = 1;
					GameObject tobj = UnityEngine.Object.Instantiate(Resources.Load("Cube/Cubebb") , pos , Quaternion.identity) as GameObject;
					tobj.transform.parent = obj_parent.transform; 
				}
				else
				{
					tag = 0;
				}	

			}

		}

	}

	#endregion
}

