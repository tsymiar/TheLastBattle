using GameDefine;
using JT.FWW.Tools;
using System;
using System.Collections.Generic;
using System.Linq;

namespace BlGame.GameEntity
{
   public class Friend
    {
       public UInt64 SGUID
       {
           private set;
           get;
       }
        public bool isOnline
        {
            private set;
            get;
        }
        public string NiceName
        {
            private set;
            get;
        }

        public int HeadId
        {
            private set;
            get;
        }
        public uint isVip
        {
            private set;
            get;
        }
        public FRIENDTYPE FriendType
        {
            private set;
            get;
        }
        public List<IChat> Chat = new List<IChat>();
        public void SetFriendInfo(UInt64 sGUID,string niceName, int headId, bool isonline = false,uint vipLevel = 0)
        {
            this.SGUID = sGUID;
            this.NiceName = niceName;
            this.HeadId = headId;
            this.isOnline = isonline;
            this.isVip = vipLevel;
        }
    }
}
