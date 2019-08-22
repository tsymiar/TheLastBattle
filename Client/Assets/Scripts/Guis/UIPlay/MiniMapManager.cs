using System;
using System.Collections.Generic;
using System.Collections;
using UnityEngine; 
using BlGame.GameData;
using BlGame.GameEntity; 
using System.Linq;
using BlGame;
using BlGame.GameState;

public class MiniMapManager : Singleton<MiniMapManager>
{
    const float updateTime = 1f;
    float timeStart = 0f;
    bool tagCheck = false;
    public MiniMapManager() { 
         EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_Loading, OnEvent);
         EventCenter.AddListener<UInt64>(EGameEvent.eGameEvent_GameOver, OnGameOver);         
    }

    public void Update() {
        if (!tagCheck)
            return;
        if (Time.time - timeStart >= updateTime) {
            EventCenter.Broadcast(EGameEvent.eGameEvent_UpdateMiniMap);
            timeStart = Time.time;
        }
    }

    private void OnGameOver(UInt64 BaseGuid)
    {
        tagCheck = false;
    }

    private void OnEvent(CEvent evt)
    {
        switch (evt.GetEventId())
        {
            case EGameEvent.eGameEvent_Loading:
                {
                    GameStateType stateType = (GameStateType)evt.GetParam("NextState");
                    if (stateType != GameStateType.GS_Play)
                        return;
                    timeStart = Time.time;
                    tagCheck = true;
                }
                break;
        }
    }


    

}

