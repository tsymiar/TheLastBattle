using UnityEngine;
using System.Collections;
using BlGame;
using JT.FWW.GameData;
using BlGame.GameData;
using BlGame.Network;
using LSToGC;
using System.IO;
using System.Linq;
using GameDefine;
using System;
using GCToCS;
using System.Collections.Generic;

namespace BlGame.Ctrl
{
    public class RoomItem
    {
        public UInt64 mRoomId;
        public UInt32 mMapId;
        public string mOwer;
        public UInt32 mCurNum;
        public UInt32 mMaxNum;
        public bool mIsPassWord;

        public RoomItem(UInt64 roomId,  UInt32 mapId, string ower, UInt32 curNum, UInt32 maxNum, bool isPassWord)
        {
            mRoomId = roomId;
            mMapId = mapId;
            mOwer = ower;
            mCurNum = curNum;
            mMaxNum = maxNum;
            mIsPassWord = isPassWord;
        }
    }

    public class BattleCtrl : Singleton<BattleCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_BattleEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_BattleExit);
        }

        //创建房间
        public void CreateRoom(int mapId, string password)
        {
            CGLCtrl_GameLogic.Instance.CreateRoom(mapId, password);
        }

        //向服务器请求刷新房间列表
        public void AskRoomList()
        {
            CGLCtrl_GameLogic.Instance.AskRoomList();
        }

        //请求加入房间
        public void AskAddRoom(string roomId, string password)
        {
            if (roomId == null || roomId == "" || UInt64.Parse(roomId) <= 0)
            {
                MsgInfoManager.Instance.ShowMsg((int)EErrorCode.eEC_NullBattle);
                EventCenter.Broadcast(EGameEvent.eGameEvent_BattleUpdateRoomList);
                return;
            }

            UInt64 id = UInt64.Parse(roomId);
            if (mRoomList.ContainsKey(id))
            {
                if (mRoomList[id].mIsPassWord && (password == null || password == ""))
                {
                    MsgInfoManager.Instance.ShowMsg((int)EErrorCode.eEC_NULLPassWord);
                    return;
                }
            }

            CGLCtrl_GameLogic.Instance.AddRoom(roomId, password);
        }

        //刷新服务器列表
        public void UpdateRoomList(List<GSToGC.RoomInfo> roomList)
        {
            mRoomList.Clear();

            foreach (GSToGC.RoomInfo roomInfo in roomList)
            {
                AddRoomItem((UInt32)roomInfo.roomId, (UInt32)roomInfo.mapId, roomInfo.master, (UInt32)roomInfo.curUserCount,(UInt32)roomInfo.maxUserCount, (roomInfo.ifPwd == 1));
            }

            EventCenter.Broadcast(EGameEvent.eGameEvent_BattleUpdateRoomList);
        }

        //更新房间列表
        private void AddRoomItem(UInt64 roomId,UInt32 mapId,string ower,UInt32 curNum,UInt32 maxNum,bool isPassword)
        {
            RoomItem room = new RoomItem(roomId,mapId,ower,curNum,maxNum, isPassword);
            
            mRoomList.Add(roomId,room);
        }

        //取得房间列表
        public Dictionary<UInt64, RoomItem> GetRoomList()
        {
            return mRoomList;
        }

        //取得房间信息
        public RoomItem GetRoomInfo(UInt32 id)
        {
            if (mRoomList.ContainsKey(id))
            {
                return mRoomList[id];
            }

            return null;
        }

        //申请组队匹配
        public void AskMatchBattle(int mapId, EBattleMatchType mt)
        {
            mMapId = mapId;
            mMatchType = mt;

            CGLCtrl_GameLogic.Instance.AskMatchBattle(mapId,mt);
        }

        //申请创建新手战役
        public void AskCreateGuideBattle(int mapId, AskCSCreateGuideBattle.guidetype mGtype)
        {
            mMapId = mapId;

            UIGuideCtrl.Instance.GuideBattleType(mGtype);

            CGLCtrl_GameLogic.Instance.AskCSToCreateGuideBattle(mapId, mGtype);
        }

        public int GetMapId()
        {
            return mMapId;
        }

        public EBattleMatchType GetMatchType()
        {
            return mMatchType;
        }

        Dictionary<UInt64, RoomItem> mRoomList = new Dictionary<UInt64, RoomItem>();

        private int mMapId = 0;
        private EBattleMatchType mMatchType = EBattleMatchType.EBMT_Normal;
    }
}
