using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Diagnostics;
using UnityEngine;

namespace SuperSocket.ClientEngine
{
    public abstract class TcpClientSession : ClientSession
    {
        protected string HostName { get; private set; }

        private bool m_InConnecting = false;

        public bool m_IsRecvThreadRun = false;

        public TcpClientSession(EndPoint remoteEndPoint)
            : this(remoteEndPoint, 1024)
        {

        }

        public TcpClientSession(EndPoint remoteEndPoint, int receiveBufferSize)
            : base(remoteEndPoint)
        {
            ReceiveBufferSize = receiveBufferSize;

            var ipEndPoint = remoteEndPoint as IPEndPoint;

            if (ipEndPoint != null)
                HostName = ipEndPoint.Address.ToString();
        }

        public override int ReceiveBufferSize
        {
            get
            {
                return base.ReceiveBufferSize;
            }

            set
            {
                if (BufferRecv.Array != null)
                    throw new Exception("ReceiveBufferSize cannot be set after the socket has been connected!");

                base.ReceiveBufferSize = value;
            }
        }

        protected virtual bool IsIgnorableException(Exception e)
        {
            if (e is System.ObjectDisposedException)
                return true;

            if (e is NullReferenceException)
                return true;

            return false;
        }

        protected bool IsIgnorableSocketError(int errorCode)
        {
            //SocketError.Shutdown = 10058
            //SocketError.ConnectionAborted = 10053
            //SocketError.ConnectionReset = 10054
            //SocketError.OperationAborted = 995
            if (errorCode == 10058 || errorCode == 10053 || errorCode == 10054 || errorCode == 995)
                return true;

            return false;
        }

#if SILVERLIGHT && !WINDOWS_PHONE
        private SocketClientAccessPolicyProtocol m_ClientAccessPolicyProtocol = SocketClientAccessPolicyProtocol.Http;

        public SocketClientAccessPolicyProtocol ClientAccessPolicyProtocol
        {
            get { return m_ClientAccessPolicyProtocol; }
            set { m_ClientAccessPolicyProtocol = value; }
        }
#endif

        protected abstract void SocketEventArgsCompleted(object sender, SocketAsyncEventArgs e);

        public override bool Connect()
        {
            if (m_InConnecting)
                throw new Exception("The socket is connecting, cannot connect again!");

            if (Client != null)
                throw new Exception("The socket is connected, you neednt' connect again!");

//WindowsPhone doesn't have this property
#if SILVERLIGHT && !WINDOWS_PHONE
            m_InConnecting = true;
            RemoteEndPoint.ConnectAsync(ClientAccessPolicyProtocol, ProcessConnect, null);
#else
            m_RecvingCache = null;

            m_SafeEvents.Clear();

            if (!IsSyncConnect)
            {
                m_InConnecting = true;

                RemoteEndPoint.ConnectAsync(ProcessConnect, null);
            }
            else
            {
                Socket socket = RemoteEndPoint.ConnectSync();

                if (socket == null)
                {
                    return false;
                }

                SocketAsyncEventArgs e = new SocketAsyncEventArgs();

                e.Completed += SocketEventArgsCompleted;

                Client = socket;

                OnGetSocket(e);
            }

            return true;
#endif
        }

        protected void ProcessConnect(Socket socket, object state, SocketAsyncEventArgs e)
        {
            if (e != null && e.SocketError != SocketError.Success)
            {
                m_InConnecting = false;
                OnConnectError(new SocketException((int)e.SocketError));
                return;
            }

            if (socket == null)
            {
                m_InConnecting = false;
                OnConnectError(new SocketException((int)SocketError.ConnectionAborted));
                return;
            }

            //To walk around a MonoTouch's issue
            //one user reported in some cases the e.SocketError = SocketError.Succes but the socket is not connected in MonoTouch
            if (!socket.Connected)
            {
                m_InConnecting = false;
                OnConnectError(new SocketException((int)SocketError.ConnectionRefused));
                return;
            }

            if (e == null)
                e = new SocketAsyncEventArgs();

            e.Completed += SocketEventArgsCompleted;

            Client = socket;

#if !SILVERLIGHT
            //Set keep alive
            Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.KeepAlive, true);
#endif

            OnGetSocket(e);

            m_InConnecting = false;
        }

        protected abstract void OnGetSocket(SocketAsyncEventArgs e);

        protected bool EnsureSocketClosed()
        {
            return EnsureSocketClosed(null);
        }

        protected bool EnsureSocketClosed(Socket prevClient)
        {
            lock (this)
            {
                //UnityEngine.Debug.Log("----111-------------");

                var client = Client;

                if (client == null)
                {
                    //UnityEngine.Debug.Log("----888-------------");
                    return false;
                }

                //UnityEngine.Debug.Log("----222-------------");

                var fireOnClosedEvent = true;

                if (prevClient != null && prevClient != client)//originalClient is previous disconnected socket, so we needn't fire event for it
                {
                    client = prevClient;
                    fireOnClosedEvent = false;
                }
                else
                {
                    Client = null;
                    m_IsSending = 0;
                }

                //UnityEngine.Debug.Log("----333-------------");

                if (client.Connected)
                {
                    //UnityEngine.Debug.Log("----444-------------");
                    try
                    {
                        client.Shutdown(SocketShutdown.Both);
                    }
                    catch (Exception e)
                    {
                        UnityEngine.Debug.Log(e.ToString());
                    }
                    //UnityEngine.Debug.Log("----555-------------");
                    try
                    {
                        while (m_IsRecvThreadRun)
                        {
                            Thread.Sleep(1);
                        }
                        Thread.Sleep(10);
                        client.Close();
                        Thread.Sleep(1);
                        client = null;
                        throw new Exception("kkkkkk");
                    }
                    catch (Exception e)
                    {
                        UnityEngine.Debug.Log(e.ToString());
                    }
                    //UnityEngine.Debug.Log("----666-------------");
                }

                //UnityEngine.Debug.Log("----777-------------");
                return fireOnClosedEvent;
            }
        }

        private void DetectConnected()
        {
            if (Client != null)
                return;

            throw new Exception("The socket is not connected!", new SocketException((int)SocketError.NotConnected));
        }

        private byte[] m_RecvingCache;

        protected override void OnDataReceived(byte[] data, int offset, int length)
        {
            while (length>0)
            {
                int cachesize = 0;

                if (m_RecvingCache != null)
                {
                    cachesize = m_RecvingCache.Length;

                    if (cachesize+length<4)
                    {
                        Array.Resize(ref m_RecvingCache, cachesize + length);

                        Array.Copy(data, offset, m_RecvingCache, cachesize, length);

                        break;
                    }
                    else if (cachesize<4)
                    {
                        Array.Resize(ref m_RecvingCache, 4);

                        Array.Copy(data, offset, m_RecvingCache, cachesize, 4 - cachesize);

                        offset += (4 - cachesize);

                        length -= (4 - cachesize);

                        cachesize = 4;
                    }
                }
                else if (length < 4)
                {
                    m_RecvingCache = new byte[length];

                    Array.Copy(data, offset, m_RecvingCache, 0, length);

                    break;
                }

                int needsize = 0;

                if (m_RecvingCache != null)
                {
                    needsize = BitConverter.ToInt32(m_RecvingCache, 0);
                }
                else
                {
                    needsize = BitConverter.ToInt32(data, offset);
                }

                if (cachesize+length<needsize)
                {
                    Array.Resize(ref m_RecvingCache, cachesize + length);

                    Array.Copy(data, offset, m_RecvingCache, cachesize, length);

                    break;
                }

                byte[] msgdata = new byte[needsize];

                if (m_RecvingCache != null)
                {
                    Array.Copy(m_RecvingCache, 0, msgdata, 0, cachesize);

                    m_RecvingCache = null;
                }

                Array.Copy(data, offset, msgdata, cachesize, needsize-cachesize);

                offset += (needsize-cachesize);

                length -= (needsize-cachesize);

                base.OnDataReceived(msgdata, 0, msgdata.Length);
            }
        }

        private IBatchQueue<ArraySegment<byte>> m_SendingQueue = new MyBatchQueue<ArraySegment<byte>>();

        private int m_IsSending = 0;

        protected bool IsSending
        {
            get { return m_IsSending == 1; }
        }

        public override bool TrySend(ArraySegment<byte> segment)
        {
            //DetectConnected();
            if (Client == null)
                return true;

            if (!m_SendingQueue.Enqueue(segment))
                return false;

            if (Interlocked.CompareExchange(ref m_IsSending, 1, 0) != 0)
                return true;

            DequeueSend();

            return true;
        }

        public override bool TrySend(IList<ArraySegment<byte>> segments)
        {
            //DetectConnected();
            if (Client == null)
                return true;

            if (!m_SendingQueue.Enqueue(segments))
                return false;

            if (Interlocked.CompareExchange(ref m_IsSending, 1, 0) != 0)
                return true;

            DequeueSend();

            return true;
        }

        public override bool FrameSend()
        {
            if (Interlocked.CompareExchange(ref m_IsSending, 1, 0) != 0)
                return false;

            DequeueSend();
            return true;
        }

        private void DequeueSend()
        {
            List<ArraySegment<byte>> sendingItems = new List<ArraySegment<byte>>();

            if (!m_SendingQueue.TryDequeue(sendingItems))
            {
                m_IsSending = 0;
                return;
            }

            SendInternal(sendingItems);
        }

        protected abstract void SendInternal(List<ArraySegment<byte>> items);

        protected void OnSendingCompleted()
        {
            m_IsSending = 0;
        }

        public override void Close()
        {
            //UnityEngine.Debug.Log("----aaaa-------------");

            if (EnsureSocketClosed())
                OnClosed();

            m_RecvingCache = null;

            //UnityEngine.Debug.Log("----ccccc-------------");
        }
    }
}
