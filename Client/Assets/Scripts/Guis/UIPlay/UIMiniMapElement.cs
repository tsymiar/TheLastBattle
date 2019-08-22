using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using GameDefine;
using BlGame.GameData;
using JT.FWW.Tools;
using System;

public class UIMiniMapElement : MonoBehaviour
{

    protected UInt64 mapTarget;

   ///protected Vector3 pos = Vector3.zero;

    protected UIAnchor alineAnchor;

    protected Transform gameMapPitch;

    Vector2 relativeOffset = new Vector2();

    public string resPath;

    /// <summary>
    /// 设置不同的头像和显示图标
    /// </summary>
    /// <param name="entity"></param>
    public virtual void CreateMiniMapElement(UInt64 guid,float x,float y,float z) {
        mapTarget = guid;
        UpdatePosDirect(x, y, z);
    }   
     
    ///// <summary>
    ///// 销毁地图元素
    ///// </summary>
    //public virtual void DestroyMiniMapElement()
    //{

    //}

    /// <summary>
    /// 获得显示的图标所对应的对象
    /// </summary>
    /// <returns></returns>
    public UInt64 GetMapTarget()
    {
        return mapTarget;
    }

    ///// <summary>
    ///// 将对象移动到目标点。
    ///// </summary>
    ///// <param name="targetPos"></param>
    //public void UpdatePosByTime(float x,float y,float z) {

    //    if (!alineAnchor) return;

    //    Vector3 pos = new Vector3(x, y, z);


    //    alineAnchor.relativeOffset.x = 0.5f - pos.z / UIMiniMap.Instance.GetMapSize().y;
    //    alineAnchor.relativeOffset.y = pos.x / UIMiniMap.Instance.GetMapSize().x - 0.5f;
    //}

    /// <summary>
    /// 将对象直接设置到目标点
    /// </summary>
    /// <param name="targetPos"></param>
    public void UpdatePosDirect(float x,float y,float z) {
        transform.localPosition = new Vector3(x, z, 0);
    }

    public void UpdatePosDirect()
    {
        Ientity entity;
        if (!EntityManager.AllEntitys.TryGetValue(mapTarget, out entity))
            return;
        GameObject objReal = entity.realObject;
        if (objReal == null)
            return;
        Vector3 pos = new Vector3(objReal.transform.position.x, objReal.transform.position.z, 0);

        transform.localPosition = pos;
    }


    protected virtual void Awake() {        
        gameMapPitch = transform.FindChild("IconPoint");        
        gameMapPitch.localPosition = new Vector3(0f, 0f, -1f);        

    }   
    
}
