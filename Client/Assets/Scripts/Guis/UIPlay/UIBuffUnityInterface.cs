using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using BlGame.GameEntity;
using System.Linq;
using BlGame.Skill;


public class BuffUIItem
{
    public UISprite effect;//转圈的效果
    public UISprite spriteIcon;//图标本身
    public UISprite spriteFrame;//边框
    public BlGame.Skill.Buff buff = null;
}

public class UIBuffUnityInterface : MonoBehaviour
{
    public static UIBuffUnityInterface Instance = null;
    List<BuffUIItem> buffUIItemList = new List<BuffUIItem>();
    void Awake()
    {
        Instance = this;
        for (int i = 1; i <= 14; ++i)
        {
            GameObject obj = gameObject.transform.FindChild("Buff" + i).gameObject;
            GameObject backObj = obj.transform.FindChild("Background").gameObject;
            GameObject spriteObj = obj.transform.FindChild("Sprite").gameObject;
            GameObject frameObj = obj.transform.FindChild("Frame").gameObject;
            BuffUIItem bItem= new BuffUIItem();
            bItem.effect = spriteObj.GetComponent<UISprite>();
            bItem.spriteIcon = backObj.GetComponent<UISprite>();
            bItem.spriteFrame = frameObj.GetComponent<UISprite>();
            buffUIItemList.Add(bItem);
        }
        RefreshUIItem();
		RefreshUIDisAble ();
    }
	public void RefreshUIDisAble()
	{
		foreach (BuffUIItem item in buffUIItemList)
		{
			item.buff = null;
			item.spriteIcon.enabled = false;
            item.spriteFrame.enabled = false;
			item.effect.enabled = false;
		}
	}
    //
    public void UpdateUIItem()
    {
        foreach (BuffUIItem bItem in buffUIItemList)
        {
            if (bItem.buff != null)
            {
                bItem.effect.fillAmount = (bItem.buff.getTotalTime() - bItem.buff.buffTime) / bItem.buff.getTotalTime();
            }
        }
    }
    //
    public void RefreshUIItem()
    {
        //return;
        foreach (BuffUIItem item in buffUIItemList)
        {
            item.buff = null;
            item.spriteIcon.enabled = false;
            item.spriteFrame.enabled = false;
            item.effect.enabled = false;
        }
        //return;
        int index = 0;
        foreach (Buff b in BuffManager.Instance.buffDict.Values)
        {
            if (index < 14)
            {
                buffUIItemList[index].buff = b;
                buffUIItemList[index].spriteIcon.enabled = true;
                buffUIItemList[index].spriteIcon.spriteName = b.getSpriteName();
                buffUIItemList[index].effect.enabled = true;
                buffUIItemList[index].spriteFrame.enabled = true;
            }
            index++;
        }
    }
    //
}

