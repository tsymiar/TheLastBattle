using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using RCToCS;
using CSToRC;
using RemoteConsole;

namespace RemoteConsole
{
    public class CenterServer : ISocketClass
    {
        private String name;
        public System.String Name
        {
            get { return name; }
            set { name = value; }
        }
        private String ip;
        public System.String Ip
        {
            get { return ip; }
            set { ip = value; }
        }
        private String port;
        public System.String Port
        {
            get { return port; }
            set { port = value; }
        }
        private String key;
        public System.String Key
        {
            get { return key; }
            set { key = value; }
        }
        private bool ifConnect;
        public bool IfConnect
        {
            get { return socket != null && socket.isConnected(); }
        }
        private int userNum;
        public int UserNum
        {
            get { return userNum; }
        }
        private int userInRoomNum;
        public int UserInRoomNum
        {
            get { return userInRoomNum; }
        }
        private int userInFightNum;
        public int UserInFightNum
        {
            get { return userInFightNum; }
        }
        private List<notice_info> noticeList = new List<notice_info>();
        public List<notice_info> NoticeList
        {
            get { return noticeList; }
        }
        private void clearNoticeList()
        {
            noticeList.Clear();
        }
        private void addNotice(notice_info notice)
        {
            noticeList.Add(notice);
        }
        //上次刷新时间
        private DateTime lastRefTime;
        public DateTime LastRefTime
        {
            get { return lastRefTime; }
        }
        //server curt time
        private string curttime;
        public string ServerCurtTime
        {
            get { return curttime; }
        }

        private Thread t;

        private SocketClass socket = new SocketClass();
        private bool ifThreadRunning = false;
        private bool ifRegist = false;

        private string key_1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        public struct CDKey_Event
        {
            public string title;
            public EPlatform platform;
	        public int CDKeyNum;
	        public string letter;
	        public int CDKeyLen;
	        public Int64 end_time;
            public string gift_msg;
	        public int gift_type;
            public int gift_num;
            public int[] item_id;
            public int[] item_num;
        }

        public struct notice_info
        {
            //字段、属性、方法、事件
            public EPlatform platform_id;
            public string titel_str;
            public EFlag flag;
            public EState state;
            public int priority;
            public string notice;
            public Int64 star_time;
            public Int64 end_time;
        }

        public enum EServerID
        {
            eServer_None = 0,
            eServer_All = 1,
            eServer_CsOne = 2,
            eServer_CsTwo = 3,
            eServer_CsThree = 4,
        }

        public enum EPlatform
        {
            ePlatform_None = 0,
            ePlatform_91 = 1,
            ePlatform_TB = 2,
            ePlatform_PP = 3,
            ePlatform_CMGE = 4,
            ePlatform_UC = 5,
            ePlatform_iTools = 6,
            ePlatform_OnlineGame = 7,
            ePlatform_As = 8,
            ePlatform_XY = 9,
            ePlatform_CMGE_ZB = 10,
            ePlatform_All = 1000,
        }

        public enum EFlag
        {
            eFlag_None = 0,
            eFlag_OnSale = 1,
            eFlag_Notice = 2,
            eFlag_Services = 3,
        }

        public enum EState
        {
            eState_None = 0,
            eState_Hot = 1,
            eState_New = 2,
        }

        private List<string> NewCDKeyList = new List<string>();

        //private Dictionary<int, SCDKey_Info> CDKey_Info = new Dictionary<int, SCDKey_Info>();

        public struct SCDKey_Event
        {
            public int id;
            public string title;
            public int plat;
            public Int64 end_time;
            public int code_num;
            public int code_len;
            public int type;
            public string content;
            public string item;
        }

        public struct SCDKey_Info
        {
            public string key;
            public int event_id;
            public int use_state;
            public Int64 use_time;
            public int server_id;
            public string user_name;
        }

        void ReConnect()
        {
            socket.Callback = this;
            int serverPort = Convert.ToInt32(port);
            socket.Connect(ip, serverPort);
        }
        void threadFunc()
        { 
            while (true)
            {
                if (false == socket.isConnected())
                {
                    ReConnect();
                }
                else
                {
                    if (false == ifRegist)
                    {
                        sendAskRegistRequest();
                    }
                    else
                    {
                        sendAskServerRequest();
                        //sendAskServerUserRequest();
                        sendAskNoticeInfo(); 
                    }
                }
                Thread.Sleep(5000); 
            }
        }

        private void sendAskRegistRequest()
        {
            RCToCS.AskRegist msg = new RCToCS.AskRegist();
            msg.key = Encoding.UTF8.GetBytes(key);
            sendMsg(msg, (int)msg.msgid);
        }

        private void sendAskServerRequest()
        {
            RCToCS.AskServerInfo msg = new RCToCS.AskServerInfo();
            sendMsg(msg, (int)msg.msgid); 
           
            TimeSpan ts = DateTime.Now - lastRefTime; 
            //断线
            ifConnect = (ts.Seconds > 10) ? false: true;
            if (false == ifConnect)
            {
                Console.WriteLine("error: disconnect! so reconnect to server!ip:"+ip );
                ReConnect(); 
            }
        }

        //private void sendAskServerUserRequest()
        //{
        //    RCToCS.AskServerUserInfo msg = new RCToCS.AskServerUserInfo();
        //    sendMsg(msg, (int)msg.msgid);
        //}

        private void sendAskNoticeInfo()
        {
            RCToCS.AskNoticeInfo msg = new RCToCS.AskNoticeInfo();
            sendMsg(msg, (int)msg.msgid);
        }

        public int sendAskAddNotice(notice_info notice)
        {
            RCToCS.AskAddNotice msg = new RCToCS.AskAddNotice();
            msg.platform = (int)notice.platform_id;
            msg.title = Encoding.UTF8.GetBytes(notice.titel_str);
            msg.flag = (int)notice.flag;
            msg.state = (int)notice.state;
            msg.priority = notice.priority;
            msg.noticeMsg = Encoding.UTF8.GetBytes(notice.notice);
            msg.star_time = (UInt64)notice.star_time;
            msg.end_time = (UInt64)notice.end_time;
            return sendMsg(msg, (int)msg.msgid);
        } 

        public int sendCreateCDKeyEventPara(CDKey_Event cdkey)
        {
            RCToCS.CreateCDKey msg = new RCToCS.CreateCDKey();
            msg.title = cdkey.title;
            msg.platform = (int)cdkey.platform;
            msg.end_time = (UInt64)cdkey.end_time;
            msg.content = cdkey.gift_msg;
            msg.gift_type = cdkey.gift_type;
            msg.code_len = cdkey.CDKeyLen;

            for (int i = 0; i < cdkey.gift_num; i++)
            {
                RCToCS.CreateCDKey.giftInfo info = new RCToCS.CreateCDKey.giftInfo();
                info.item_id = cdkey.item_id[i];
                info.item_num = cdkey.item_num[i];
                msg.gift.Add(info);
            }

            //MessageBox.Show("正在生成...");
            //产生CDKey
            List<string> temp_cdkey = new List<string>();
            int num = BuildRationCDKey(temp_cdkey, cdkey);

            //把CDKey发送到数据库
            for (int i = 0; i < temp_cdkey.Count; i++)
            {
                msg.cdkey.Add(temp_cdkey[i]);
                if (i % 10000 == 0 || i == temp_cdkey.Count - 1)
                {
                    sendMsg(msg, (int)msg.msgid);
                }
            }

            sendMsg(msg, (int)msg.msgid);
            return 0;
        }

        public int AskQueryCDkeyEvents()
        {
            RCToCS.QueryCDKey msg = new RCToCS.QueryCDKey();
            return sendMsg(msg, (int)msg.msgid);
        }

        private int sendMsg(ProtoBuf.IExtensible pMsg, Int32 n32MsgID)
        {
            if (false == socket.isConnected())
            {
                Console.WriteLine("warning: has disconnect to server!");
                return -1;
            }
            var ms = new System.IO.MemoryStream();
            ProtoBuf.Serializer.Serialize(ms, pMsg);

            byte[] data = ms.ToArray();
            int length = data.Length + 8;
            byte[] senddata = new byte[length];
            byte[] lengthArr = BitConverter.GetBytes(length);
            byte[] idArr = BitConverter.GetBytes(n32MsgID);
            lengthArr.CopyTo(senddata, 0);
            idArr.CopyTo(senddata, 4);
            data.CopyTo(senddata, 8);

            socket.Send(n32MsgID, senddata);
            return 0;
        }

        public void start()
        {
            lastRefTime = DateTime.Now; 
            if (false == ifThreadRunning)
            {
                t = new Thread(new ThreadStart(threadFunc));
                t.Start();
                ifThreadRunning = true;
            }
        }

        public void stop()
        {
            if (t != null && t.IsAlive)
            {
                t.Abort();
            }
            t = null;
            if (socket != null && socket.isConnected())
            {
                socket.close();
            }
            socket = null;
        }

        public void onReceiveMsg(StateObject obj)
        {
            try
            {
                while (true)
                {
                    int length = BitConverter.ToInt32(obj.buffer, 0);
                    if (length <= 8) return;
                    int msgid = BitConverter.ToInt32(obj.buffer, 4);
                    System.IO.MemoryStream stream = new System.IO.MemoryStream();
                    stream.Write(obj.buffer, 8, length - 8);
                    stream.Position = 0;
                    Array.Copy(obj.buffer, length, obj.buffer, 0, obj.buffer.Length - length);
                    switch (msgid)
                    {
                        case (int)CSToRC.MsgID.eMsgCS2RC_AskRegistRsp:
                            OnMsgAskRegistRsp(stream);
                            break;
                        case (int)CSToRC.MsgID.eMsgCS2RC_AllServerInfo:
                            OnMsgAllServerInfo(stream);
                            break;
                        case (int)CSToRC.MsgID.eMsgCS2RC_ServerUserInfo:
                            OnMsgServerUserInfo(stream);
                            break;
                        case (int)CSToRC.MsgID.eMsgCS2RC_AllNoticeInfo:
                            OnMsgAllNoticeInfo(stream);
                            break;
                        case (int)CSToRC.MsgID.eMsgCS2RC_AddNoticeResp:
                            OnMsgAddNoticeResp(stream);
                            break;
                        case (int)CSToRC.MsgID.eMsgCS2RC_DeleteNoticeResp:
                            OnMsgDeleteNoticeResp(stream);
                            break;
                        case (int)CSToRC.MsgID.eMsgCS2RC_AskAddOneMailResp:
                            OnMsgAddMailResp(stream);
                            break;                       
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public void OnMsgAskRegistRsp(System.IO.MemoryStream stream)
        {
            CSToRC.AskRegistRsp msg = ProtoBuf.Serializer.Deserialize<CSToRC.AskRegistRsp>(stream);
            if (msg.rst == true)
            {
                ifRegist = true;
            }
        }

        public void OnMsgAllServerInfo(System.IO.MemoryStream stream)
        {
            CSToRC.AllServerInfo msg = ProtoBuf.Serializer.Deserialize<CSToRC.AllServerInfo>(stream);
            userInRoomNum = msg.roomUserNum;
            userInFightNum = msg.battleUserNum;
            curttime = msg.curttime;
            userNum = msg.allUserNum;

            lastRefTime = DateTime.Now; 

            Console.WriteLine("---msg from cs---------.");
        }

        public void OnMsgServerUserInfo(System.IO.MemoryStream stream)
        {
            CSToRC.ServerUserInfo msg = ProtoBuf.Serializer.Deserialize<CSToRC.ServerUserInfo>(stream);
            userNum = msg.allUserNum;
        }

        public void OnMsgAllNoticeInfo(System.IO.MemoryStream stream)
        {
            clearNoticeList();
            CSToRC.AllNoticeInfo msg = ProtoBuf.Serializer.Deserialize<CSToRC.AllNoticeInfo>(stream);
            foreach (NoticeInfo info in msg.notices)
            {
 //               addNotice(Encoding.UTF8.GetString(info.notice));
            }
        }

        public void OnMsgAddNoticeResp(System.IO.MemoryStream stream)
        {
            CSToRC.AddNoticeResp msg = ProtoBuf.Serializer.Deserialize<CSToRC.AddNoticeResp>(stream);
        }

        public void OnMsgDeleteNoticeResp(System.IO.MemoryStream stream)
        {
            CSToRC.DeleteNoticeResp msg = ProtoBuf.Serializer.Deserialize<CSToRC.DeleteNoticeResp>(stream);
        }

        public void OnMsgAddMailResp(System.IO.MemoryStream stream)
        {
            CSToRC.AddMailResp msg = ProtoBuf.Serializer.Deserialize<CSToRC.AddMailResp>(stream);
            if (msg.rst)
            {
                //add ok
            }
        }
            
        //mail
        public bool onMsgAddNewMailToCS(  bool bIfAll, List<UInt64> userIdList, MailInfo mail)
        {
            if (!IfConnect)
            {
                return false; 
            }

            RCToCS.AskAddMailReq msg = new RCToCS.AskAddMailReq();
            if (!bIfAll)
            {
                for (int i = 0; i < userIdList.Count; i++)
                {
                    msg.userIdlist.Add((long)userIdList[i]);
                }
            }

            msg.type = mail.mtype;
            msg.title = mail.mtitle;
            msg.content = mail.mcontent;
            msg.gift = mail.mgiftstr;
            msg.createtime = mail.mcreatetime;
            msg.endtime = mail.mendtime;
            msg.day = mail.existday;
            msg.sdk = mail.msdk;
            msg.sender = mail.msender;
            
           sendMsg(msg, (int)msg.msgid);

           return true;
        }

        private string CreateCDkey(int codelen, string letter)
        {
            UInt64 temp_u64_1 = 0;
            UInt64 temp_u64_2 = 0;
            string ret_str = "";
            string temp_1 = "";
            string temp_2 = "";
            string temp_ret = "";
            string str_guid = "";

            while (ret_str.Length != codelen)
            {
                ret_str = "";
                str_guid = Guid.NewGuid().ToString();
                for (int j = 0; j < str_guid.Length; j++)
                {
                    if (str_guid[j] == '-' || str_guid[j] == '{' || str_guid[j] == '}')
                    {
                        continue;
                    }
                    if (temp_1.Length < 16)
                    {
                        temp_1 += str_guid[j];
                    }
                    else
                    {
                        temp_2 += str_guid[j];
                    }
                }

                temp_u64_1 = Convert.ToUInt64(temp_1, 16);
                temp_u64_2 = Convert.ToUInt64(temp_2, 16);

                if (letter.Length > 0){
                    ret_str += letter;
                }

                for (; temp_u64_1 != 0; )
                {
                    ret_str += key_1[(int)(temp_u64_1 % 26)];
                    temp_u64_1 = temp_u64_1 / 26;

                    if (temp_u64_2 != 0)
                    {
                        temp_ret += key_1[(int)(temp_u64_2 % 26)];
                        temp_u64_2 = temp_u64_2 / 26;
                    }
                }

                if (ret_str.Length > codelen)
                {
                    string temp_str = "";
                    for (int i = 0; i < codelen; i++)
                    {
                        temp_str += ret_str[i];
                    }
                    ret_str = temp_str;
                }
                else
                {
                    int j = 0;
                    for (int i = ret_str.Length; i < codelen; i++ )
                    {
                        ret_str += temp_ret[j++];
                    }
                }
            }

            return ret_str;
        }


        private int BuildRationCDKey(List<string> temp_cdkey, CDKey_Event cdkey)
        {
            List<string> contrast_cdkey = new List<string>();
            for (int i = 0; i < cdkey.CDKeyNum; i++)
            {
                string cdk_str = "";
                cdk_str = CreateCDkey(cdkey.CDKeyLen, cdkey.letter);
                temp_cdkey.Add(cdk_str);
                contrast_cdkey.Add(cdk_str);
            }

            //检查是否重复
            int count = 0;
            int check = 0;

            for (int i = 0; i < temp_cdkey.Count; i++)
            {
                for (int j = 0; j < contrast_cdkey.Count; j++)
                {
                    if (temp_cdkey[i].Equals(contrast_cdkey[j]))
                    {
                        check++;
                        if (check >= 2)
                        {
                            //temp_cdkey.Remove(temp_cdkey[i]);
                            count++;
                            break;
                        }
                    }
                }
                check = 0;
            }
            //返回重复数量
            return count;
        }
    }
}