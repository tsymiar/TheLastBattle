using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;

namespace BlGame.GameEntity
{
    public class InviteOtherPlayer
    {
        private static InviteOtherPlayer instance = null;
        public static InviteOtherPlayer Instance
        {
            get {
                if (instance == null)
                {
                    instance = new InviteOtherPlayer();
                }
                return instance;
            }
        }
        public UInt64 RoomID
        {
            private set;
            get;
        }

        public string PassWord
        {
            private set;
            get;
        }
        public string NiceName
        {
            private set;
            get;
        }
        public int Length
        {
            private set;
            get;
        }
        public void SetInviteion(UInt64 roomID, string passWord, int length,string niceName)
        {
            Length = length;
            RoomID = roomID;
            PassWord = passWord;
            NiceName = niceName;
        }
    }
}
