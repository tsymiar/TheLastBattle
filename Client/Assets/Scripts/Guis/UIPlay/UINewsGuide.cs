using UnityEngine; 
using System;
using System.Collections;
using System.Collections.Generic;
using JT.FWW.Tools;
using BlGame.GuideDate;
using BlGame.GameEntity;
using System.Linq;
using GameDefine;
using BlGame.Resource;

public class UINewsGuide : MonoBehaviour
{
    public static UINewsGuide Instance;

    bool isSet = false;

    bool showBuildingTips = false;

    Dictionary<Ientity, GameObject> bTips = new Dictionary<Ientity, GameObject>();

    void OnEnable()
    {
        Instance = this;
        //EventCenter.AddListener<UInt64, int, int, int>(EGameEvent.eGameEvent_GuideKillTask, OnEvent);
        EventCenter.AddListener<bool>(EGameEvent.eGameEvent_GuideShowBuildingTips, OnShowBuildingTips);
        isSet = false;
    }

    void OnDisable() {
        //EventCenter.RemoveListener<UInt64, int, int, int>(EGameEvent.eGameEvent_GuideKillTask, OnEvent);
        EventCenter.RemoveListener<bool>(EGameEvent.eGameEvent_GuideShowBuildingTips, OnShowBuildingTips);
    }

    void Update() {
        if (bTips == null || bTips.Count == 0)
            return;
        for (int i = bTips.Count - 1; i >= 0; i--) {
            Transform tran = bTips.ElementAt(i).Value.transform;
            UpdatePosition(bTips.ElementAt(i).Key, tran);
        }
    }

	float GetTipHeight(Ientity entity){
		for (int i = 0; i < GuideBuildingTips.Instance.tipHeight.Length; i++) {
			if(entity.NpcGUIDType == GuideBuildingTips.Instance.npcIdArray[i]){
				return GuideBuildingTips.Instance.tipHeight[i];
			}
		}
		return 5f;
	}

    void UpdatePosition(Ientity target,Transform obj)




    {

        if (Camera.main == null || GameMethod.GetUiCamera == null)
            return;
		float height = GetTipHeight (target);
        // 血条对应在3d场景的位置
		Vector3 pos_3d = target.realObject.transform.position + new Vector3(0f, height, 0f);
        // 血条对应在屏幕的位置
        Vector2 pos_screen = Camera.main.WorldToScreenPoint(pos_3d);
        // 血条对应在ui中的位置
        Vector3 pos_ui = GameMethod.GetUiCamera.ScreenToWorldPoint(pos_screen);

        obj.position = Vector3.Slerp(transform.position, pos_ui, Time.time); 
    } 

    void OnShowBuildingTips(bool show) {
        if (show)
        {
            bTips.Clear();
			Dictionary<int, Ientity> dic = GuideBuildingTips.Instance.GetTipTargetDic(); 
            for (int i = 0; i < dic.Count; i++)
            {
				int index = GetEntityIndexByNpcId(dic.ElementAt(i).Value.NpcGUIDType);
				ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GuideBuildingTips.Instance.pathArray[index], ResourceType.PREFAB);
                GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
                obj.transform.parent = XueTiaoManager.Instance.transform; 
                obj.transform.localScale = Vector3.one;
                bTips.Add(dic.ElementAt(i).Value, obj);
            }
        }
        else
        {
            for (int i = 0; i < bTips.Count; i++) {
                GameObject.DestroyImmediate(bTips.ElementAt(i).Value);
            }
            bTips.Clear();
        }
        showBuildingTips = show;
    }

	int  GetEntityIndexByNpcId(int npcId){ 
		for (int i = 0; i < GuideBuildingTips.Instance.npcIdArray.Length; i++) {
			if(GuideBuildingTips.Instance.npcIdArray[i] == npcId)		
				return i;
		}
		return -1;
	}

   

    void OnEvent(UInt64 kill, int deadType, int deadId, int reason)
    {
        if (isSet)
            return;
        Iselfplayer player  = PlayerManager.Instance.LocalPlayer;
        bool isFirstBlood = false;

        if (player.GameObjGUID == kill && SceneGuideTaskManager.Instance().IsFakeHero(deadId)) 
            isFirstBlood = true;
        else if(player.NpcGUIDType == deadId && SceneGuideTaskManager.Instance().GetFakeHero(kill) != null)
            isFirstBlood = true;

        if (isFirstBlood)
        {  
            string name = SceneGuideTaskManager.Instance().GetFakeHeroName(deadId);
            string readXml = ConfigReader.GetMsgInfo(10008).content;
            MsgInfoManager.Instance.SetKills(MsgInfoManager.eKillMsgType.eFirstBlood, false, player.GameUserNick, name, readXml);
            MsgInfoManager.Instance.SetAuido(MsgInfoManager.eKillMsgType.eFirstBlood, false);
            isSet = true;
        } 

    }
    
}
