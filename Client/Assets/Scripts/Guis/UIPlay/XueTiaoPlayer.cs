using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic; 
using BlGame.GameEntity;
using BlGame.GuideDate;
public class XueTiaoPlayer : XueTiaoUI 
{	
	#region  玩家血条动态去血显示	 
	private UISprite hideSprite = null; 
	#endregion

	private UISprite mpSprite = null;
	private UILabel labelName = null;
	private UILabel labelLevel = null;

 
	#region 设置蓝条
	public void SetMp(float mp){
		mpSprite.fillAmount = mp;
	}
	#endregion

	#region 更新蓝条
	public void UpdateMp(){
		mpSprite.fillAmount = xueTiaoOwner.Mp / xueTiaoOwner.MpMax;
	}

	public void UpdateLevel(){        
        labelLevel.text = xueTiaoOwner.Level.ToString();
        ShowGuideXueTiaoInfo();
	}
	#endregion
    Transform hideSlider = null;
    void Awake()
    {
        hpSprite = transform.FindChild("Control_Hp/Foreground").GetComponent<UISprite>();
        mpSprite = transform.FindChild("Control_Mp/Foreground").GetComponent<UISprite>();
        labelLevel = transform.FindChild("Level").FindChild("Label").GetComponent<UILabel>();
        labelName = transform.FindChild("Name").GetComponent<UILabel>();
        hideSlider = transform.transform.FindChild("Control_Hp/ProgressHide");
        hideSprite = hideSlider.transform.FindChild("HideBg").GetComponent<UISprite>();
        hideSprite.fillAmount = 0;
    }

	#region 玩家血条缓慢扣除
	public override void XueTiaoDmgShow(float currentHp,float dmg, float maxHp)
	{
		if(dmg <= 0f)
			return;
		float value = currentHp/maxHp;
        
        if (hideSlider != null && hideSprite != null && dmg > 0) 
		{
            Vector3 currentPos = new Vector3(hpSprite.width * value, hideSlider.transform.localPosition.y, hideSlider.transform.localPosition.z);
            if ((hpSprite.transform.localScale.x * (dmg / maxHp)) != 0)
			{
				hideSlider.transform.localPosition = currentPos;
                hideSprite.SetDimensions((int)(hpSprite.width * (dmg / maxHp)), hideSprite.height);
                hideSprite.fillAmount = 1;
                if (gameObject.activeInHierarchy)
                {
                    StartCoroutine("SlowDown");
                }
                else
                {
                    hideSprite.fillAmount = 0f;
                    StopCoroutine("slowDown");
                }
			}
		}  
	}

    void OnDisable() {
        hideSprite.fillAmount = 0f;
    }

	IEnumerator SlowDown(){
		if (hideSlider != null && gameObject.activeInHierarchy) 
		{
			while (true)
			{
				yield return new WaitForSeconds (0.05f);
                hideSprite.fillAmount -= 0.05f;
                if (hideSprite.fillAmount <= 0)
				{
					StopCoroutine("slowDown");
					break;
				}
			}			
		}
	} 
	#endregion

	#region 初始化血条值
	public override void ResetXueTiaoValue ()
	{
		base.ResetXueTiaoValue ();
		mpSprite.fillAmount = 0f;
        //labelName.text = null;
		labelLevel.text = null;
        ShowGuideXueTiaoInfo();
	}
	#endregion
 

	#region 更新血条信息
	public override void SetXueTiaoInfo ()
	{
		base.SetXueTiaoInfo();
		UpdateMp();
		UpdateLevel();
        Iplayer player = (Iplayer)base.xueTiaoOwner;
        labelName.text = player.GameUserNick;
        ShowGuideXueTiaoInfo();
	}

    void ShowGuideXueTiaoInfo() {
        if ((SceneGuideTaskManager.Instance().IsNewsGuide() != SceneGuideTaskManager.SceneGuideType.NoGuide)
            && (xueTiaoOwner.entityType == EntityType.Soldier
            || xueTiaoOwner.entityType == EntityType.AltarSoldier))
        {
            if (xueTiaoOwner.NpcGUIDType == 21017)
                labelName.text = "伊芙蕾";
            else if (xueTiaoOwner.NpcGUIDType == 21025)
                labelName.text = "半人马酋长";
            else if (xueTiaoOwner.NpcGUIDType == 21024)
                labelName.text = "冷面黑寡妇";
            labelLevel.text = "4";
            mpSprite.fillAmount = 1f;
        }
    }
	#endregion

	protected override float GetXueTiaoHeight ()
	{
        HeroConfigInfo info = null;
        if ((SceneGuideTaskManager.Instance().IsNewsGuide() != SceneGuideTaskManager.SceneGuideType.NoGuide) 
            && (xueTiaoOwner.entityType == EntityType.Soldier 
            || xueTiaoOwner.entityType == EntityType.AltarSoldier))
        {
            NpcConfigInfo nInfo = ConfigReader.GetNpcInfo(xueTiaoOwner.NpcGUIDType);
            ShowGuideXueTiaoInfo();
            return nInfo.NpcXueTiaoHeight;
        }
        else {
            info = ConfigReader.GetHeroInfo(xueTiaoOwner.NpcGUIDType);
        }
		
		return info.HeroXueTiaoHeight;
	}
}
