using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Text;

namespace SuperSocket.ClientEngine
{
    public class AsyncTcpSession : TcpClientSession
    {
        private SocketAsyncEventArgs m_SocketEventArgsRecv;
        private SocketAsyncEventArgs m_SocketEventArgsSend;

        public AsyncTcpSession(EndPoint remoteEndPoint)
            : base(remoteEndPoint)
        {

        }

        public AsyncTcpSession(EndPoint remoteEndPoint, int receiveBufferSize)
            : base(remoteEndPoint, receiveBufferSize)
        {

        }

        protected override void SocketEventArgsCompleted(object sender, SocketAsyncEventArgs e)
        {
            if (e.LastOperation == SocketAsyncOperation.Connect)
            {
                ProcessConnect(sender as Socket, null, e);
                return;
            }

            ProcessReceive(e);
        }

        protected override void OnGetSocket(SocketAsyncEventArgs e)
        {
            if (BufferRecv.Array == null)
                BufferRecv = new ArraySegment<byte>(new byte[ReceiveBufferSize], 0, ReceiveBufferSize);

            e.SetBuffer(BufferRecv.Array, BufferRecv.Offset, BufferRecv.Count);

            if (m_SocketEventArgsRecv != null)
            {
                m_SocketEventArgsRecv.Dispose();
            }

            m_SocketEventArgsRecv = e;

            StartReceive();
            OnConnected();
        }

        //private void BeginReceive()
        //{
        //    if (!Client.ReceiveAsync(m_SocketEventArgs))
        //        ProcessReceive(m_SocketEventArgs);
        //}

        private void ProcessReceive(SocketAsyncEventArgs e)
        {
            if (e.SocketError != SocketError.Success)
            {
                OnReceiveThreadExit(new SocketException((int)e.SocketError));
                m_IsRecvThreadRun = false;
                return;
            }

            if (e.BytesTransferred == 0)
            {
                OnReceiveThreadExit(new SocketException((int)e.SocketError));
                m_IsRecvThreadRun = false;
                return;
            }

            OnDataReceived(e.Buffer, e.Offset, e.BytesTransferred);
            StartReceive();
        }

        void StartReceive()
        {
            m_IsRecvThreadRun = true;

            bool raiseEvent;

            var client = Client;

            if (client == null)
            {
                OnReceiveThreadExit(null);
                m_IsRecvThreadRun = false;
                return;
            }

            try
            {
                raiseEvent = client.ReceiveAsync(m_SocketEventArgsRecv);
            }
            catch (SocketException exc)
            {
                OnReceiveThreadExit(exc);
                m_IsRecvThreadRun = false;
                return;
            }
            catch(Exception e)
            {
                OnReceiveThreadExit(null);
                m_IsRecvThreadRun = false;
                return;
            }

            if (!raiseEvent)
                ProcessReceive(m_SocketEventArgsRecv);
        }

        protected override void SendInternal(List<ArraySegment<byte>> items)
        {
            if (m_SocketEventArgsSend == null)
            {
                m_SocketEventArgsSend = new SocketAsyncEventArgs();
                m_SocketEventArgsSend.Completed += new EventHandler<SocketAsyncEventArgs>(Sending_Completed);
            }

            bool raiseEvent;

            try
            {
                if (items.Count > 1)
                {
                    if (m_SocketEventArgsSend.Buffer != null)
                        m_SocketEventArgsSend.SetBuffer(null, 0, 0);

                    m_SocketEventArgsSend.BufferList = items;
                }
                else
                {
                    var currentItem = items[0];

                    try
                    {
                        if (m_SocketEventArgsSend.BufferList != null)
                            m_SocketEventArgsSend.BufferList = null;
                    }
                    catch//a strange NullReference exception
                    {
                    }

                    m_SocketEventArgsSend.SetBuffer(currentItem.Array, 0, currentItem.Count);
                }
                

                raiseEvent = Client.SendAsync(m_SocketEventArgsSend);
            }
            catch (SocketException exc)
            {
                if (EnsureSocketClosed() && !IsIgnorableSocketError(exc.ErrorCode))
                    OnError(exc);

                return;
            }
            catch (Exception e)
            {
                if (EnsureSocketClosed() && IsIgnorableException(e))
                    OnError(e);
                return;
            }

            if (!raiseEvent)
                Sending_Completed(Client, m_SocketEventArgsSend);
        }

        void Sending_Completed(object sender, SocketAsyncEventArgs e)
        {
            if (e.SocketError != SocketError.Success || e.BytesTransferred == 0)
            {
                OnReceiveThreadExit(new SocketException((int)e.SocketError));
                return;
            }

            OnSendingCompleted();
        }
    }
}
