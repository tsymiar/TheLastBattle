using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Diagnostics;

namespace SuperSocket.ClientEngine
{
    public abstract class ClientSession : IClientSession
    {
        protected Socket Client { get; set; }

        protected EndPoint RemoteEndPoint { get; set; }

        public bool IsConnected { get; private set; }

        public bool IsNeedSafeEvent { get; set; }

        public bool IsSyncConnect { get; set; }

        public int user_data { get; set; }

#if !__IOS__
        public bool NoDeplay { get; set; }
#endif

        public ClientSession()
        {

        }

        public ClientSession(EndPoint remoteEndPoint)
        {
            if (remoteEndPoint == null)
                throw new ArgumentNullException("remoteEndPoint");

            RemoteEndPoint = remoteEndPoint;
        }

        public abstract bool Connect();

        public abstract bool TrySend(ArraySegment<byte> segment);

        public abstract bool TrySend(IList<ArraySegment<byte>> segments);

        public void Send(byte[] data, int offset, int length)
        {
            if (length == 0) return;
            this.Send(new ArraySegment<byte>(data, offset, length));
        }

        public void Send(ArraySegment<byte> segment)
        {
            if (TrySend(segment))
                return;

            while (true)
            {
                Thread.SpinWait(1);

                if (TrySend(segment))
                    return;
            }
        }

        public void Send(IList<ArraySegment<byte>> segments)
        {
            if (TrySend(segments))
                return;

            while (true)
            {
                Thread.SpinWait(1);

                if (TrySend(segments))
                    return;
            }
        }

        public abstract bool FrameSend();

        public abstract void Close();

        private EventHandler m_Closed;

        public event EventHandler Closed
        {
            add { m_Closed += value; }
            remove { m_Closed -= value; }
        }

        protected virtual void OnClosed()
        {
            IsConnected = false;

            var handler = m_Closed;

            if (handler == null)
                return;

            if (IsNeedSafeEvent)
            {
                UnityEngine.Debug.Log("----bbbb-------------");

                SafeEvent safe_event = new SafeEvent();
                safe_event.type = EventType.ET_Closed;
                safe_event.args = EventArgs.Empty;
                AddEvent(safe_event);
                return;
            }
            handler(this, EventArgs.Empty);
        }

        private EventHandler<ErrorEventArgs> m_Error;

        public event EventHandler<ErrorEventArgs> Error
        {
            add { m_Error += value; }
            remove { m_Error -= value; }
        }

        protected virtual void OnError(Exception e)
        {
            var handler = m_Error;
            if (handler == null)
                return;

            if (IsNeedSafeEvent)
            {
                SafeEvent safe_event = new SafeEvent();
                safe_event.type = EventType.ET_Error;
                safe_event.args = new ErrorEventArgs(e);
                AddEvent(safe_event);
                return;
            }
            handler(this, new ErrorEventArgs(e));
        }

        private EventHandler<ErrorEventArgs> m_ConnectError;

        public event EventHandler<ErrorEventArgs> ConnectError
        {
            add { m_ConnectError += value; }
            remove { m_ConnectError -= value; }
        }

        protected virtual void OnConnectError(Exception e)
        {
            var handler = m_ConnectError;
            if (handler == null)
                return;

            if (!IsSyncConnect && IsNeedSafeEvent)
            {
                SafeEvent safe_event = new SafeEvent();
                safe_event.type = EventType.ET_ConnectError;
                safe_event.args = new ErrorEventArgs(e);
                AddEvent(safe_event);
                return;
            }
            handler(this, new ErrorEventArgs(e));
        }

        private EventHandler m_Connected;

        public event EventHandler Connected
        {
            add { m_Connected += value; }
            remove { m_Connected -= value; }
        }

        protected virtual void OnConnected()
        {
            var client = Client;

#if !__IOS__
            if(client != null)
            {
                if(client.NoDelay != NoDeplay)
                    client.NoDelay = NoDeplay;
            }
#endif

            IsConnected = true;

            var handler = m_Connected;
            if (handler == null)
                return;

            if (!IsSyncConnect && IsNeedSafeEvent)
            {
                SafeEvent safe_event = new SafeEvent();
                safe_event.type = EventType.ET_Connected;
                safe_event.args = EventArgs.Empty;
                AddEvent(safe_event);
                return;
            }
            handler(this, EventArgs.Empty);
        }

        private EventHandler<ErrorEventArgs> m_ReceiveThreadExit;

        public event EventHandler<ErrorEventArgs> ReceiveThreadExit
        {
            add { m_ReceiveThreadExit += value; }
            remove { m_ReceiveThreadExit -= value; }
        }

        protected virtual void OnReceiveThreadExit(Exception e)
        {
            var handler = m_ReceiveThreadExit;
            if (handler == null)
                return;

            if (!IsSyncConnect && IsNeedSafeEvent)
            {
                SafeEvent safe_event = new SafeEvent();
                safe_event.type = EventType.ET_RecvTheadExit;
                safe_event.args = new ErrorEventArgs(e);
                AddEvent(safe_event);
                return;
            }
            handler(this, new ErrorEventArgs(e));
        }

        private EventHandler<DataEventArgs> m_DataReceived;

        public event EventHandler<DataEventArgs> DataReceived
        {
            add { m_DataReceived += value; }
            remove { m_DataReceived -= value; }
        }

        private DataEventArgs m_DataArgs = new DataEventArgs();

        protected virtual void OnDataReceived(byte[] data, int offset, int length)
        {
            var handler = m_DataReceived;
            if (handler == null)
                return;

            m_DataArgs.Data = data;
            m_DataArgs.Offset = offset;
            m_DataArgs.Length = length;

            if (IsNeedSafeEvent)
            {
                SafeEvent safe_event = new SafeEvent();
                safe_event.type = EventType.ET_DataReceived;
                safe_event.args = m_DataArgs;
                AddEvent(safe_event);
                m_DataArgs = new DataEventArgs();
                return;
            }
            handler(this, m_DataArgs);
        }

        public virtual int ReceiveBufferSize { get; set; }

        protected ArraySegment<byte> BufferRecv { get; set; }

        public object m_SyncCs = new object();

        public Queue<SafeEvent> m_SafeEvents = new Queue<SafeEvent>();

        public void AddEvent(SafeEvent s_event)
        {
            lock (m_SyncCs)
            {
                m_SafeEvents.Enqueue(s_event);
            }
        }

        public void DealEvents()
        {
            Queue<SafeEvent> tmpEvents = new Queue<SafeEvent>();

            lock (m_SyncCs)
            {
                tmpEvents = m_SafeEvents;

                m_SafeEvents = new Queue<SafeEvent>();
            }

            foreach (SafeEvent sEevent in tmpEvents)
            {
                switch(sEevent.type)
                {
                    case EventType.ET_Connected:
                        m_Connected(this, sEevent.args);
                        break;
                    case EventType.ET_ConnectError:
                        m_ConnectError(this, sEevent.args as ErrorEventArgs);
                        break;
                    case EventType.ET_Closed:
                        m_Closed(this, sEevent.args as ErrorEventArgs);
                        break;
                    case EventType.ET_Error:
                        m_Error(this, sEevent.args as ErrorEventArgs);
                        break;
                    case EventType.ET_DataReceived:
                        m_DataReceived(this, sEevent.args as DataEventArgs);
                        break;
                    case EventType.ET_RecvTheadExit:
                        m_ReceiveThreadExit(this, sEevent.args as ErrorEventArgs);
                        break;
                }
            }
        }
    }

    public class SafeEvent
    {
        public EventType type;
        public EventArgs args;
    }
}
