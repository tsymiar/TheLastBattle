using UnityEngine;
using System.Collections;
using JT.FWW.Tools;
using System.Collections.Generic;
using BlGame.GameData;
using JT.FWW.GameData;
using BlGame;
using GameDefine;
using BlGame.Resource;
using BlGame.View;
using System;

namespace BlGame.GameEntity
{
    public class FriendManager
    {
        private static FriendManager instance = null;
        public static FriendManager Instance
        {
            get{
                if (instance == null)
                {
                    instance = new FriendManager();
                }
                return instance;
            }
        }

        FriendManager() { 

        }
        public Dictionary<UInt64, Friend> AllFriends = new Dictionary<UInt64, Friend>();
        public Dictionary<UInt64, Friend> AllBlackDic = new Dictionary<UInt64, Friend>();
        public Dictionary<string, string> AllSearch = new Dictionary<string, string>();
        public Dictionary<GameObject, Friend> OnClickKey = new Dictionary<GameObject, Friend>();

        public Dictionary<UInt64, IChat> AllTalkDic = new Dictionary<UInt64, IChat>();


        public void AddOnClickKey(GameObject obj, Friend temp)
        {
            if (OnClickKey.ContainsKey(obj))
            {
                return;
            }
            OnClickKey.Add(obj, temp);
        }
        public void RemoveOnClickKey(GameObject obj)
        {
            if (OnClickKey.ContainsKey(obj))
            {
                OnClickKey[obj] = null;
                OnClickKey.Remove(obj);
            }
        }

        public GameObject GetObj(UInt64 sGUID)
        {
            foreach (var item in OnClickKey)
            {
                if (sGUID != 0 && item.Value.SGUID == sGUID)
                {
                    return item.Key;
                }
            }
            return null;
        }

        /// <summary>
        /// 查询玩家列表
        /// </summary>

        public void AddSearchDic( string name,string headid)
        {
            if (AllSearch.ContainsKey(name))
            {
                return;
            }
            AllSearch.Add(name,headid);
        }
        /// <summary>
        /// 删除当前玩家的查询
        /// </summary>
        /// <param name="sGUID"></param>
        public void RemoveSearchDic(string name)
        {
            if (AllSearch.ContainsKey(name))
            {
                AllSearch[name] = null;
                AllSearch.Remove(name);
            }
        }

        //private Queue<string> InvitaInfo = new Queue<string>();
        //private Queue<UInt64> InvataIndex = new Queue<UInt64>();
        //public void SetInvitaData(UInt64 index,string name)
        //{
        //    InvitaInfo.Enqueue(name);
        //    InvataIndex.Enqueue(index);
        //}
        //public string getName()
        //{
        //    if (InvitaInfo.Count < 1)
        //    {
        //        return null;
        //     }
        //    string str = InvitaInfo.Dequeue();
        //    if (string.IsNullOrEmpty(str))
        //    {
        //        return null;
        //    }
        //    return str;
        //}

        /***添加黑名单***/
        public void AddOnLineBlackList(UInt64 sGUID,Friend friend) {
            if (AllBlackDic.ContainsKey(sGUID))
                return;
            AllBlackDic.Add(sGUID, friend);
        }
        /***删除当前黑名单***/
        public void DelBlackList(UInt64 sGUID)
        {
            if (AllBlackDic.ContainsKey(sGUID))
            {
                AllBlackDic[sGUID] = null;
                AllBlackDic.Remove(sGUID);
            }
        }
        /***清空数据***/
        public void DelAllList()
        {
            AllBlackDic.Clear();
            AllFriends.Clear();
            AllTalkDic.Clear();
            //foreach (var item in AllBlackDic)
            //{
            //    AllBlackDic[item.Key] = null;
            //    AllBlackDic.Remove(item.Key);
            //}
            //foreach (var item in AllFriends)
            //{
            //    AllFriends[item.Key] = null;
            //    AllFriends.Remove(item.Key);
            //}
            //foreach (var item in AllTalkDic)
            //{
            //    AllTalkDic[item.Key] = null;
            //    AllTalkDic.Remove(item.Key);
            //}
        }

        /***添加好友***/
        public void AddFriend(UInt64 sGUID, Friend friend)
        {
            if (AllFriends.ContainsKey(sGUID))
            {
                return;
            }
            AllFriends.Add(sGUID, friend);
        }

        /***删除在线好友***/
        public void DelFriend(UInt64 sGUID)
        {
            if (AllFriends.ContainsKey(sGUID))
            {
                AllFriends[sGUID] = null;
                AllFriends.Remove(sGUID);
            }
        }
        public void DelAllTalkDic(UInt64 sGUID)
        {
            if (AllTalkDic.ContainsKey(sGUID))
            {
                AllTalkDic[sGUID] = null;
                AllTalkDic.Remove(sGUID);
            }
        }
        public class RoomIviteList {
            public int headID;
            public string nickName;
        }


        public Dictionary<UInt64, RoomIviteList> RoomInvite = new Dictionary<UInt64, RoomIviteList>();
        public void AddInvite(UInt64 sGUID, int headId, string nickName)
        {
            RoomIviteList room = null;
            if (!RoomInvite.ContainsKey(sGUID))
            {
                room = new RoomIviteList();
                room.headID = headId;
                room.nickName = nickName;
                RoomInvite.Add(sGUID, room);
                return;
            }
            room.headID = headId;
            room.nickName = nickName;
            RoomInvite[sGUID] = room;
        }
        public void Remove(UInt64 sGUID)
        {
            if (RoomInvite.ContainsKey(sGUID))
            {
                RoomInvite.Remove(sGUID);
            }
        }
    }
}
