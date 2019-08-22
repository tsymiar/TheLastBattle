using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;
using JT.FWW.Tools;

namespace JT
{
    namespace FWW
    {
        namespace Tools
        {
            public class CMemQueue
            {
                private MemoryStream m_msStream = null;
                private UInt32 m_un32Size;
                private UInt32 m_un32TailPos;
                private UInt32 m_un32HeadPos;

                private UInt64 m_un64DataPushed;
                private UInt64 m_un64DataPoped;

                public CMemQueue(UInt32 un32Size)
                {
                    m_msStream = new MemoryStream((Int32)un32Size);
                    m_un32Size = un32Size;
                    m_un32TailPos = 0;
                    m_un32HeadPos = 0;
                    m_un64DataPushed = 0;
                    m_un64DataPoped = 0;
                }

                public UInt32 GetSize()
                {
                    return m_un32Size;
                }

                public UInt64 GetCachedDataSize()
                {
                    return m_un64DataPushed - m_un64DataPoped;
                }

                public UInt64 GetPushedDataSize()
                {
                    return m_un64DataPushed;
                }

                public UInt64 GetPopedDataSize()
                {
                    return m_un64DataPoped;
                }

                public void Reset()
                {
                    m_un32TailPos = 0;
                    m_un32HeadPos = 0;
                    m_un64DataPushed = 0;
                    m_un64DataPoped = 0;
                }

                //push data from pData with length un32Len into the memory queue.
                public Int32 PushBack(byte[] abByteStream, UInt32 un32Len)
                {
                    UInt32 un32Dist = m_un32TailPos + m_un32Size - m_un32HeadPos;
                    UInt32 un32Used = (un32Dist >= m_un32Size ? (un32Dist - m_un32Size) : un32Dist);
                    if (un32Len + un32Used + 1 > (UInt32)m_un32Size)
                    {
                        return -1;
                    }

                    if (m_un32TailPos + un32Len >= m_un32Size)
                    {
                        UInt32 un32Seg1 = (UInt32)(m_un32Size - m_un32TailPos);
                        UInt32 un32Seg2 = un32Len - un32Seg1;
                        m_msStream.Seek(m_un32TailPos, SeekOrigin.Begin);
                        m_msStream.Write(abByteStream, 0, (Int32)un32Seg1);

                        m_msStream.Seek(0, SeekOrigin.Begin);
                        m_msStream.Write(abByteStream, (Int32)un32Seg1, (Int32)un32Seg2);
                        m_un32TailPos = un32Seg2;
                    }
                    else
                    {
                        m_msStream.Seek(m_un32TailPos, SeekOrigin.Begin);
                        m_msStream.Write(abByteStream, 0, (Int32)un32Len);
                        m_un32TailPos += un32Len;
                    }
                    m_un64DataPushed += un32Len;
                    return 0;
                }

                //pop un32Len byte data from header to pBuf which would move read pointer.
                public Int32 PopFront(byte[] abByteStream, UInt32 un32Len)
                {
                    UInt32 un32Dist = m_un32TailPos + m_un32Size - m_un32HeadPos;
                    UInt32 un32Used = (un32Dist >= m_un32Size ? (un32Dist - m_un32Size) : un32Dist);
                    if (un32Len > un32Used)
                    {
                        return -1;
                    }

                    if (m_un32HeadPos + un32Len >= m_un32Size)
                    {
                        UInt32 un32Seg1 = (UInt32)(m_un32Size - m_un32HeadPos);
                        UInt32 un32Seg2 = un32Len - un32Seg1;


                        m_msStream.Seek(m_un32HeadPos, SeekOrigin.Begin);
                        m_msStream.Read(abByteStream, 0, (Int32)un32Seg1);

                        m_msStream.Seek(0, SeekOrigin.Begin);
                        m_msStream.Read(abByteStream, (Int32)un32Seg1, (Int32)un32Seg2);
                        m_un32HeadPos = un32Seg2;
                    }
                    else
                    {
                        m_msStream.Seek(m_un32HeadPos, SeekOrigin.Begin);
                        m_msStream.Read(abByteStream, 0, (Int32)un32Len);
                        m_un32HeadPos += un32Len;
                    }

                    m_un64DataPoped += un32Len;
                    return 0;
                }

                //get un32Len byte data from header to pBuf which do not move read pointer.
                public Int32 GetFront(byte[] abByteStream, UInt32 un32Len)
                {
                    UInt32 un32Dist = m_un32TailPos + m_un32Size - m_un32HeadPos;
                    UInt32 un32Used = (un32Dist >= m_un32Size ? (un32Dist - m_un32Size) : un32Dist);
                    if (un32Len > un32Used)
                    {
                        return -1;
                    }

                    if (m_un32HeadPos + un32Len >= m_un32Size)
                    {
                        UInt32 un32Seg1 = (UInt32)(m_un32Size - m_un32HeadPos);
                        UInt32 un32Seg2 = un32Len - un32Seg1;


                        m_msStream.Seek(m_un32HeadPos, SeekOrigin.Begin);
                        m_msStream.Read(abByteStream, 0, (Int32)un32Seg1);

                        m_msStream.Seek(0, SeekOrigin.Begin);
                        m_msStream.Read(abByteStream, (Int32)un32Seg1, (Int32)un32Seg2);
                        //m_un32HeadPos = un32Seg2;
                    }
                    else
                    {
                        m_msStream.Seek(m_un32HeadPos, SeekOrigin.Begin);
                        m_msStream.Read(abByteStream, 0, (Int32)un32Len);
                        //m_un32HeadPos += un32Len;
                    }

                    //m_un64DataPoped += un32Len;
                    return 0;
                }
            }
        }
    }
}