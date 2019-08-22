using UnityEngine;
using System.Collections;
using JT.FWW.Tools;
using GameDefine;
using System.Collections.Generic;
using BlGame.GameData;
using System;
namespace BlGame.GameEntity
{
    public class IChat
    {
        public UInt64 SGUID
        {
            private set;
            get;
        }

        public string NickName
        {
            private set;
            get;
        }
        public int HeadID
        {
            private set;
            get;
        }

       public class IChatInfo
        {
           public string msg;
           public bool isLocalPlayer;
           public string nickName
           {
               private set;
               get;
           }
           public int head;
           public MsgTalk TalkState;
           public void SetNickName(string nick) {
               nickName = nick;
           }
        }

        //public List<string> MsgInfo = new List<string>();
        public List<IChatInfo> MsgInfo = new List<IChatInfo>();
        public MsgTalk TalkState
        {
            private set;
            get;
        }
        public void SetTalkState(MsgTalk talkState)
        {
            TalkState = talkState;
        }
        public void SetMsgInfo(UInt64 sGUID ,string nickName, string msgInfo, MsgTalk talkState,int headID,bool islocal)
        {
            SGUID = sGUID;
            NickName = nickName;
            TalkState = talkState;
            HeadID = headID;
            IChatInfo info = new IChatInfo();
            info.isLocalPlayer = islocal;
            info.msg = msgInfo;
            info.head = headID;
            info.SetNickName(nickName);
            info.TalkState = talkState;
            MsgInfo.Add(info);
        }
    }
}
