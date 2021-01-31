using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class AltarData {

    private static AltarData instance = null;
    public static AltarData Instance
    {
        get {
            if (instance == null)
            {
                instance = new AltarData();
            }
            return instance;
        }
    }
   public static Dictionary<int, int> AllAltarDic = new Dictionary<int, int>();

    public void AddAltarDic(int entityID,int type)
    {
        if (!AllAltarDic.ContainsKey(entityID))
        {
            AllAltarDic.Add(entityID, type);
        }
    }
    public void DelAltarDic(int entityID) {
        if (AllAltarDic.ContainsKey(entityID))
        {
            AllAltarDic.Remove(entityID);
        }
    }
    public void DelAllAltar()
    {
        AltarData.AllAltarDic.Clear();
    }
}
