using System;
using System.Collections.Generic;
using System.Net;
using System.Reflection;
using System.Text;

namespace SuperSocket.ClientEngine
{
    public enum EventType
    {
        ET_Connected,
        ET_Closed,
        ET_Error,
        ET_ConnectError,
        ET_DataReceived,
        ET_RecvTheadExit,
    }

    public interface IClientSession
    {
        int ReceiveBufferSize { get; set; }

        bool IsConnected { get; }

        bool IsNeedSafeEvent { get; set; }

        bool IsSyncConnect { get; set; }

        int user_data { get; set; }

        void DealEvents();

        bool Connect();

        void Send(ArraySegment<byte> segment);

        void Send(IList<ArraySegment<byte>> segments);

        void Send(byte[] data, int offset, int length);

        bool TrySend(ArraySegment<byte> segment);

        bool TrySend(IList<ArraySegment<byte>> segments);

        bool FrameSend();

        void Close();

        event EventHandler Connected;

        event EventHandler Closed;

        event EventHandler<ErrorEventArgs> Error;

        event EventHandler<ErrorEventArgs> ConnectError;

        event EventHandler<DataEventArgs> DataReceived;

        event EventHandler<ErrorEventArgs> ReceiveThreadExit;
    }
}
