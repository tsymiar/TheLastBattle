using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using GameDefine;
using BlGame.GameData;
using JT.FWW.Tools;
using BlGame.GuideDate;
using System;

public class UIMiniMapPlayerElement : UIMiniMapElement
{
    private string smallIconName;

    private UISprite spriteSmallIcon;

    private Vector2 orignalSize = new Vector2();

    private Vector2 scaleSize = new Vector2(40f, 40f);

    private UISprite headBg;

    protected override void Awake()
    {
        base.Awake();
        spriteSmallIcon = gameMapPitch.GetComponent<UISprite>();
        orignalSize = new Vector2(spriteSmallIcon.width, spriteSmallIcon.height);
        smallIconName = spriteSmallIcon.spriteName;
        headBg = spriteSmallIcon.transform.FindChild("Sprite").GetComponent<UISprite>();
        headBg.gameObject.SetActive(false);
    }

    public override void CreateMiniMapElement(UInt64 guid, float x, float y, float z)
    {
        base.CreateMiniMapElement(guid, x, y, z);
    }


    private void OnEnable()
    {
        EventCenter.AddListener(EGameEvent.eGameEvent_ChangeMapState, ChangeMapState);
    }

    private void OnDisable()
    {
        EventCenter.RemoveListener(EGameEvent.eGameEvent_ChangeMapState, ChangeMapState);
    }


    void ChangeMapState() { 
        UISprite sprite = headBg.transform.FindChild("Sprite").GetComponent<UISprite>();
        if (!EntityManager.AllEntitys.ContainsKey(mapTarget))
        {
            sprite.gameObject.SetActive(false);
            return;
        }

        Ientity entity = EntityManager.AllEntitys[mapTarget];
        if (entity == null || entity.realObject == null || !entity.realObject.activeInHierarchy)
        {
            sprite.gameObject.SetActive(false);
            return;
        }
        if (entity.entityType != EntityType.Player && entity.entityType != EntityType.Soldier && entity.entityType != EntityType.AltarSoldier)
        {
            sprite.gameObject.SetActive(false);
            return;
        }

        if (entity.FSM != null && entity.FSM.State == BlGame.FSM.FsmState.FSM_STATE_DEAD)
        {
            sprite.gameObject.SetActive(false);
            return;
        }
        Vector3 pos = entity.realObject.transform.position;
        UpdatePosDirect(pos.x, pos.y, pos.z);

       // if (state == UIMiniMap.MapState.SmallState)
        {
            spriteSmallIcon.spriteName = smallIconName;
            spriteSmallIcon.SetDimensions((int)orignalSize.x, (int)orignalSize.y);
            headBg.gameObject.SetActive(false);           
        }
        /*
        else
        {
            int id = (int)entity.ObjTypeID;
            string head = null;
           

            if (SceneGuideTaskManager.Instance().IsNewsGuide() != SceneGuideTaskManager.SceneGuideType.NoGuide && (entity.NpcGUIDType == 21017
                || entity.NpcGUIDType == 21025 || entity.NpcGUIDType == 21024))
            {
                NpcConfigInfo npcInfo = ConfigReader.GetNpcInfo(entity.NpcGUIDType);
                head = npcInfo.HeadPhoto.ToString();
            }
            else {
            HeroSelectConfigInfo info = ConfigReader.GetHeroSelectInfo(id);            
                head = info.HeroSelectHead.ToString();
            }
            spriteSmallIcon.spriteName = head;
            spriteSmallIcon.SetDimensions((int)scaleSize.x, (int)scaleSize.y);
            headBg.gameObject.SetActive(true);

            bool isPlayer = false;

            if (entity.entityType == EntityType.Player)
            {
                isPlayer = true;
            }
            else
            {
                if (SceneGuideTaskManager.Instance().IsNewsGuide() != SceneGuideTaskManager.SceneGuideType.NoGuide && (entity.NpcGUIDType == 21017
                || entity.NpcGUIDType == 21025 || entity.NpcGUIDType == 21024))
                {
                    isPlayer = true;
                }
                else {
                    isPlayer = false;
                }
            } 
            if (isPlayer)
            {                 
                sprite.gameObject.SetActive(true);
            }
            else {
                sprite.gameObject.SetActive(false);
            }

        } 
         */
    }
}
    
