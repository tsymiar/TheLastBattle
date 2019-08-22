


using UnityEngine;
using System.Collections;
using SuperSocket.ClientEngine;
using System.Collections.Generic;
using System;
using System.Net;
using ProtoBuf;
using JT.FWW.Tools;
using BlGame.GameData;
using BlGame.GameState;
using System.Net.Sockets;
using System.Threading;
using BlGame.View;

namespace BlGame.Network
{
    public class NetworkManager : Singleton<NetworkManager>
    {
        public enum ServerType
        {
            GateServer = 0,
            BalanceServer,
            LoginServer
        }

        private TcpClient m_Client = null;
        private TcpClient m_Connecting = null;
        private string m_IP = "127.0.0.1";
        private Int32 m_Port = 40001;
        private Int32 m_n32ConnectTimes = 0;
        private ServerType serverType = ServerType.BalanceServer;
        private float m_CanConnectTime = 0f;
        private float m_RecvOverTime = 0f;
        private float mRecvOverDelayTime = 2f;
        private Int32 m_ConnectOverCount = 0;
        private float m_ConnectOverTime = 0f;
        private Int32 m_RecvOverCount = 0;
        public bool canReconnect = false;
        public byte[] m_RecvBuffer = new byte[2 * 1024 * 1024];
        public Int32 m_RecvPos = 0;
        IAsyncResult mRecvResult;
        IAsyncResult mConnectResult;
        //发送数据stream
        public System.IO.MemoryStream mSendStream = new System.IO.MemoryStream();
        //接收数据stream
        public List<int> mReceiveMsgIDs = new List<int>();
        public List<System.IO.MemoryStream> mReceiveStreams = new List<System.IO.MemoryStream>();
        public List<System.IO.MemoryStream> mReceiveStreamsPool = new List<System.IO.MemoryStream>();

        public NetworkManager()
        {
            for (int i=0;i<50;++i)
            {
                mReceiveStreamsPool.Add(new System.IO.MemoryStream());
            }
        
            //预先创建消息RuntimeTypeModel运行时类型模型    
            //A
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.AbsorbBegin>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.AbsorbRes>();
            //B
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.BroadcastBuildingDestory>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.BuffEffect>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.BroadcastBattleHeroInfo>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.BattleStateChange>();
            //C
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.CurCP>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.CurDeadTimes>();
            //D
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.DestroyEmitEffect>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.DisappearInfo>();
            //E
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.EmitSkill>();
            //F
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.FreeState>();
            //G
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.GOAppear>();
            //H
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.HitTar>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.HeroKills>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.HPChange>();
            //L
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.LastingSkillState>();
            //M
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.MpChange>();
            //N
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifyOBAppear>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifySkillModelEmitTurn>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifyPassitiveSkillLoad>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifyPassitiveSkillUnLoad>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifyPassitiveSkillRelease>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifySkillModelStartForceMoveTeleport>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifySkillModelStartForceMoveStop>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifySkillModelStartForceMove>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifyBornObj>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifySummonLifeTime>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifySkillModelLeading>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifyAltarBSIco>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifySkillInfo>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.NotifyOtherItemInfo>();
            //P
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.PrepareSkillState>();
            //S
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.SummonEffect>();
            //R
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.RangeEffectEnd>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.RebornSuccess>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.RebornTimes>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.ReleasingSkillState>();
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.RunningState>();
            //U
            ProtoBuf.Serializer.PrepareSerializer<GSToGC.UsingSkillState>();
        }

        ~NetworkManager()
        {
            //接收stream
            foreach (System.IO.MemoryStream one in mReceiveStreams)
            {
                one.Close();
            }
            foreach (System.IO.MemoryStream one in mReceiveStreamsPool)
            {
                one.Close();
            }

            //发送stream
            if (mSendStream != null)
            {
                mSendStream.Close();
            }

            if (m_Client != null)
            {
                m_Client.Client.Shutdown(SocketShutdown.Both);
                m_Client.GetStream().Close();
                m_Client.Close();
                m_Client = null;
            }                            
        }


        public void Init(string ip, Int32 port, ServerType type)
        {
            Debugger.Log("set network ip:" + ip + " port:" + port + " type:" + type);
            m_IP = ip;
            m_Port = port;
            serverType = type;
            m_n32ConnectTimes = 0;
            canReconnect = true;
            m_RecvPos = 0;

#if UNITY_EDITOR
            mRecvOverDelayTime = 20000f;
#endif
        }

        public void UnInit()
        {
            canReconnect = false;
        }

        public void Connect()
        {
            if (!canReconnect) return;

            if (m_CanConnectTime > Time.time) return;

            if (m_Client != null)
                throw new Exception("fuck, The socket is connecting, cannot connect again!");

            if (m_Connecting != null)
                throw new Exception("fuck, The socket is connecting, cannot connect again!");

            Debugger.Log("IClientSession Connect");

            IPAddress ipAddress = IPAddress.Parse(m_IP);

            try
            {
                m_Connecting = new TcpClient();

                mConnectResult = m_Connecting.BeginConnect(m_IP, m_Port, null, null);

                m_ConnectOverCount = 0;

                m_ConnectOverTime = Time.time + 2;
            }
            catch (Exception exc)
            {
                Debugger.LogError(exc.ToString());

                m_Client = m_Connecting;

                m_Connecting = null;

                mConnectResult = null;

                OnConnectError(m_Client, null);
            }
        }

        public void Close()
        {
            if (m_Client != null)
            {
                OnClosed(m_Client, null);
            }
        }

        public void Update(float fDeltaTime)
        {
            if (m_Client != null)
            {
                DealWithMsg();

                if (mRecvResult != null)
                {
                    if (m_RecvOverCount > 200 && Time.time > m_RecvOverTime)
                    {
                        Debugger.LogError("recv data over 200, so close network.");
                        Close();
                        return;
                    }

                    ++m_RecvOverCount;

                    if (mRecvResult.IsCompleted)
                    {
                        try
                        {
                            Int32 n32BytesRead = m_Client.GetStream().EndRead(mRecvResult);
                            m_RecvPos += n32BytesRead;

                            if (n32BytesRead == 0)
                            {
                                Debugger.LogError("can't recv data now, so close network 2.");
                                Close();
                                return;
                            }
                        }
                        catch (Exception exc)
                        {
                            Debugger.LogError(exc.ToString());
                            Close();
                            return;
                        }

                        OnDataReceived(null, null);

                        if (m_Client != null)
                        {
                            try
                            {
                                mRecvResult = m_Client.GetStream().BeginRead(m_RecvBuffer, m_RecvPos, m_RecvBuffer.Length - m_RecvPos, null, null);
                                m_RecvOverTime = Time.time + mRecvOverDelayTime;
                                m_RecvOverCount = 0;
                            }
                            catch (Exception exc)
                            {
                                Debugger.LogError(exc.ToString());
                                Close();
                                return;
                            }
                        }
                    }
                }

                if (m_Client != null && m_Client.Connected == false)
                {
                    Debugger.LogError("client is close by system, so close it now.");
                    Close();
                    return;
                }
            }
            else if (m_Connecting != null)
            {
                if (m_ConnectOverCount > 200 && Time.time > m_ConnectOverTime)
                {
                    Debugger.LogError("can't connect, so close network.");

                    m_Client = m_Connecting;

                    m_Connecting = null;

                    mConnectResult = null;

                    OnConnectError(m_Client, null);

                    return;
                }

                ++m_ConnectOverCount;

                if (mConnectResult.IsCompleted)
                {
                    m_Client = m_Connecting;

                    m_Connecting = null;

                    mConnectResult = null;

                    if (m_Client.Connected)
                    {
                        try
                        {
                            m_Client.NoDelay = true;

                            m_Client.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 2000);

                            m_Client.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 2000);

                            m_Client.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.KeepAlive, true);

                            mRecvResult = m_Client.GetStream().BeginRead(m_RecvBuffer, 0, m_RecvBuffer.Length, null, null);

                            m_RecvOverTime = Time.time + mRecvOverDelayTime;

                            m_RecvOverCount = 0;

                            OnConnected(m_Client, null);
                        }
                        catch (Exception exc)
                        {
                            Debugger.LogError(exc.ToString());
                            Close();
                            return;
                        }
                    }
                    else
                    {
                        OnConnectError(m_Client, null);
                    }
                }
            }
            else
            {
                Connect();
            }
        }

        public void SendMsg(ProtoBuf.IExtensible pMsg, Int32 n32MsgID)
        {
            if (m_Client != null)
            {
                //清除stream
                mSendStream.SetLength(0);
                mSendStream.Position = 0;

               
                //序列到stream
                ProtoBuf.Serializer.Serialize(mSendStream, pMsg);
                CMsg pcMsg = new CMsg((int)mSendStream.Length);
                pcMsg.SetProtocalID(n32MsgID);
                pcMsg.Add(mSendStream.ToArray(), 0, (int)mSendStream.Length);
                //ms.Close();
#if UNITY_EDITOR
#else
                try
                {
#endif

#if LOG_FILE && UNITY_EDITOR
                if (n32MsgID != 8192 && n32MsgID != 16385)
                {
                    string msgName = "";
                    if (Enum.IsDefined(typeof(GCToBS.MsgNum), n32MsgID))
                    {
                        msgName = ((GCToBS.MsgNum)n32MsgID).ToString();
                    }
                    else if (Enum.IsDefined(typeof(GCToCS.MsgNum), n32MsgID))
                    {
                        msgName = ((GCToCS.MsgNum)n32MsgID).ToString();
                    }
                    else if (Enum.IsDefined(typeof(GCToLS.MsgID), n32MsgID))
                    {
                        msgName = ((GCToLS.MsgID)n32MsgID).ToString();
                    }
                    else if (Enum.IsDefined(typeof(GCToSS.MsgNum), n32MsgID))
                    {
                        msgName = ((GCToSS.MsgNum)n32MsgID).ToString();
                    }

                    using (System.IO.StreamWriter sw = new System.IO.StreamWriter(@"E:\Log.txt", true))
                    {
                        sw.WriteLine(Time.time + "   发送消息：\t" + n32MsgID + "\t" + msgName);
                    }
                }
#endif
                m_Client.GetStream().Write(pcMsg.GetMsgBuffer(), 0, (int)pcMsg.GetMsgSize());
#if UNITY_EDITOR
#else
                }
                catch (Exception exc)
                {
                    Debugger.LogError(exc.ToString());
                    Close();
                }
#endif
            }
        }

        public void OnConnected(object sender, EventArgs e)
        {
            switch (serverType)
            {
                case ServerType.BalanceServer:
                    {
                        CGLCtrl_GameLogic.Instance.BsOneClinetLogin();
                    }
                    break;
                case ServerType.GateServer:
                    {
                        ++m_n32ConnectTimes;
                        if (m_n32ConnectTimes > 1)
                        {
                            CGLCtrl_GameLogic.Instance.EmsgTocsAskReconnect();
                        }
                        else
                        {
                            CGLCtrl_GameLogic.Instance.GameLogin();
                        }
                        EventCenter.Broadcast(EGameEvent.eGameEvent_ConnectServerSuccess);
                    }
                    break;
                case ServerType.LoginServer:
                    {
                        CGLCtrl_GameLogic.Instance.EmsgToLs_AskLogin();
                    }
                    break;
            }
        }

        public void OnConnectError(object sender, ErrorEventArgs e)
        {
            Debugger.Log("OnConnectError begin");
            
            try
            {
                m_Client.Client.Shutdown(SocketShutdown.Both);
                m_Client.GetStream().Close();          
                m_Client.Close();
                m_Client = null;
            }
            catch (Exception exc)
            {
                Debugger.Log(exc.ToString());
            }
            mRecvResult = null;
            m_Client = null;
            m_RecvPos = 0;
            m_RecvOverCount = 0;
            m_ConnectOverCount = 0;

            EventCenter.Broadcast(EGameEvent.eGameEvent_ConnectServerFail);

            Debugger.Log("OnConnectError end");
        }

        public void OnClosed(object sender, EventArgs e)
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_ConnectServerFail);
            
            try
            {
                m_Client.Client.Shutdown(SocketShutdown.Both);
                m_Client.GetStream().Close();
                m_Client.Close();
                m_Client = null;
            }
            catch (Exception exc)
            {
                Debugger.Log(exc.ToString());
            }

            mRecvResult = null;
            m_Client = null;
            m_RecvPos = 0;
            m_RecvOverCount = 0;
            m_ConnectOverCount = 0;
            mReceiveStreams.Clear();
            mReceiveMsgIDs.Clear();
        }

        public void DealWithMsg()
        {
            while (mReceiveMsgIDs.Count>0 && mReceiveStreams.Count>0)
            {
                int type = mReceiveMsgIDs[0];
                System.IO.MemoryStream iostream = mReceiveStreams[0];
                mReceiveMsgIDs.RemoveAt(0);
                mReceiveStreams.RemoveAt(0);
#if UNITY_EDITOR
#else
                try
                {
#endif
#if LOG_FILE && UNITY_EDITOR
                if (type != 1)
                {
                    string msgName = "";
                    if (Enum.IsDefined(typeof(BSToGC.MsgID), type))
                    {
                        msgName = ((BSToGC.MsgID)type).ToString();
                    }
                    else if (Enum.IsDefined(typeof(LSToGC.MsgID), type))
                    {
                        msgName = ((LSToGC.MsgID)type).ToString();
                    }
                    else if (Enum.IsDefined(typeof(GSToGC.MsgID), type))
                    {
                        msgName = ((GSToGC.MsgID)type).ToString();
                    }

                   using (System.IO.StreamWriter sw = new System.IO.StreamWriter(@"E:\Log.txt", true))
                   {
                       sw.WriteLine(Time.time + "  收到消息：\t" + type + "\t" + msgName);
                    }
                }
#endif

                CGLCtrl_GameLogic.Instance.HandleNetMsg(iostream, type);
                if (mReceiveStreamsPool.Count<100)
                {
                    mReceiveStreamsPool.Add(iostream);
                }
                else
                {
                    iostream = null;
                }

#if UNITY_EDITOR
#else
                }
                catch (Exception ecp)
                {
                    Debugger.LogError("Handle Error msgid: " + type);
                }
#endif
            }
        }

        public void OnDataReceived(object sender, DataEventArgs e)
        {
            int m_CurPos = 0;
            while (m_RecvPos - m_CurPos >= 8)
            {
                int len = BitConverter.ToInt32(m_RecvBuffer, m_CurPos);
                int type = BitConverter.ToInt32(m_RecvBuffer, m_CurPos + 4);
                if (len > m_RecvBuffer.Length)
                {
                    Debugger.LogError("can't pause message" + "type=" + type + "len=" + len);
                    break;
                }
                if (len > m_RecvPos - m_CurPos)
                {
                    break;//wait net recv more buffer to parse.
                }
                //获取stream
                System.IO.MemoryStream tempStream = null;
                if (mReceiveStreamsPool.Count>0)
                {
                    tempStream = mReceiveStreamsPool[0];
                    tempStream.SetLength(0);
                    tempStream.Position = 0;
                    mReceiveStreamsPool.RemoveAt(0);
                }
                else
                {
                    tempStream = new System.IO.MemoryStream();
                }
                //往stream填充网络数据
                tempStream.Write(m_RecvBuffer, m_CurPos + 8, len - 8);
                tempStream.Position = 0;
                m_CurPos += len;
                mReceiveMsgIDs.Add(type);
                mReceiveStreams.Add(tempStream);
            }
            if (m_CurPos > 0)
            {
                m_RecvPos = m_RecvPos - m_CurPos;

                if (m_RecvPos < 0)
                {
                    Debug.LogError("m_RecvPos < 0");
                }

                if (m_RecvPos > 0)
                {
                    Buffer.BlockCopy(m_RecvBuffer, m_CurPos, m_RecvBuffer, 0, m_RecvPos);                    
                }
            }
        }
    }
}