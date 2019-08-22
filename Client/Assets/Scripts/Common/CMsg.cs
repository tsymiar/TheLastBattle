using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;
using System.Collections;
using System.Text;
using System.Security.Cryptography;
using System.Collections.Generic;

using JT.FWW.Tools;

namespace JT
{
    namespace FWW
    {
        namespace Tools
        {
			public class CMsg
            {
                public struct SMsgHeader
                {
                    public Int32 n32Size;
                    public Int32 n32ProtocalID;
                };

                public static Int32 s_n32MsgHeaderSize = 8;

                private CMemBuffer m_cnbBuffer = null;
                private Int32 m_n32MsgSize = 0;

                public CMsg(Int32 n32Size)
                {
                    this.ConstructMsg(n32Size);
                }

                public Int32 GetReadPos()
                {
                    return this.m_cnbBuffer.ReadPos;
                }

                public void SetReadPos(Int32 n32Pos)
                {
                    this.m_cnbBuffer.ReadPos = n32Pos;
                }

                public Int32 GetWritePos()
                {
                    return this.m_cnbBuffer.WritePos;
                }

                public void SetWritePos(Int32 n32Pos)
                {
                    this.m_cnbBuffer.WritePos = n32Pos;
                }

                public Boolean Reset()
                {
                    this.m_cnbBuffer.SetReadPos(0);
                    this.m_cnbBuffer.SetWritePos(0);
                    SMsgHeader myHeader;
                    myHeader.n32Size = 0;
                    myHeader.n32ProtocalID = 0;
                    this.Add(myHeader.n32Size);
                    this.Add(myHeader.n32ProtocalID);
                    this.m_cnbBuffer.SetReadPos(s_n32MsgHeaderSize);
                    this.m_cnbBuffer.SetWritePos(s_n32MsgHeaderSize);
                    return true;
                }

                private bool ConstructMsg(Int32 n32Size)
                {
                    //create memory stream and initialization the member variables.
                    this.m_cnbBuffer = new CMemBuffer(n32Size);
                    m_n32MsgSize = n32Size;

                    //assign default value to the message header.
                    SMsgHeader myHeader;
                    myHeader.n32Size = 0;
                    myHeader.n32ProtocalID = 0;

                    //Push message header to the stream.
                    this.Add(myHeader.n32Size);
                    this.Add(myHeader.n32ProtocalID);
                    this.m_cnbBuffer.SetReadPos(s_n32MsgHeaderSize);
                    this.m_cnbBuffer.SetWritePos(s_n32MsgHeaderSize);                    

                    return true;
                }

                private void UpdateMsgSize()
                {
                    Int32 n32MsgSize = this.m_cnbBuffer.GetWritePos();
                    Int32 n32ProtocalIDSize = sizeof(Int32);
                    byte[] arrByteStream = BitConverter.GetBytes(n32MsgSize);
                    this.m_cnbBuffer.ReWrite(0, arrByteStream, (Int32)n32ProtocalIDSize);
                }

                public byte[] GetMsgBuffer()
                {
                    return this.m_cnbBuffer.GetMemBuffer();
                }

                public Int32 GetMsgSize()
                {
                    return BitConverter.ToInt32(this.m_cnbBuffer.GetMemBuffer(), 0);
                }

                public Int32 GetProtocalID()
                {
                    return BitConverter.ToInt32(this.m_cnbBuffer.GetMemBuffer(), sizeof(Int32));
                }

                public Int32 GetMsgTime()
                {
                    return BitConverter.ToInt32(this.m_cnbBuffer.GetMemBuffer(), sizeof(Int32) * 2);
                }

                public Int32 GetMsgVerifyCode()
                {
                    return BitConverter.ToInt32(this.m_cnbBuffer.GetMemBuffer(), sizeof(Int32) * 3);
                }

                public void SetProtocalID(Int32 n32ProtocalID)
                {
                    Int32 n32ProtocalIDSize = sizeof(Int32);
                    byte[] arrByteStream = BitConverter.GetBytes(n32ProtocalID);
                    this.m_cnbBuffer.ReWrite((Int32)n32ProtocalIDSize, arrByteStream, (Int32)n32ProtocalIDSize);
                }

                public void Add(Boolean blValue)
                {
                    this.m_cnbBuffer.Add(blValue);
                    this.UpdateMsgSize();
                }

                public void Add(byte bValue)
                {
                    this.m_cnbBuffer.Add(bValue);
                    this.UpdateMsgSize();
                }

                public void Add(char nchValue)
                {
                    this.m_cnbBuffer.Add(nchValue);
                    this.UpdateMsgSize();
                }

                public void Add(UInt16 un16Value)
                {
                    this.m_cnbBuffer.Add(un16Value);
                    this.UpdateMsgSize();
                }

                public void Add(Int16 n16Value)
                {
                    this.m_cnbBuffer.Add(n16Value);
                    this.UpdateMsgSize();
                }

                public void Add(UInt32 un32Value)
                {
                    this.m_cnbBuffer.Add(un32Value);
                    this.UpdateMsgSize();
                }

                public void Add(Int32 n32Value)
                {
                    this.m_cnbBuffer.Add(n32Value);
                    this.UpdateMsgSize();
                }

                public void Add(UInt64 un64Value)
                {
                    this.m_cnbBuffer.Add(un64Value);
                    this.UpdateMsgSize();
                }

                public void Add(Int64 n64Value)
                {
                    this.m_cnbBuffer.Add(n64Value);
                    this.UpdateMsgSize();
                }

                public void Add(float fValue)
                {
                    this.m_cnbBuffer.Add(fValue);
                    this.UpdateMsgSize();
                }

                public void Add(double dValue)
                {
                    this.m_cnbBuffer.Add(dValue);
                    this.UpdateMsgSize();
                }

                public void Add(string strValue)
				{
					byte[] abFromByte = Encoding.Unicode.GetBytes(strValue);
					byte[] abToByte = Encoding.Convert(Encoding.Unicode, Encoding.UTF8, abFromByte);
				    Int32 n32UTF8BytesCount = Encoding.UTF8.GetCharCount(abToByte);
					this.m_cnbBuffer.Add(abToByte, 0, n32UTF8BytesCount);
                    this.UpdateMsgSize();
                }

                public void Add(string strValue, UInt32 un32FixSize)
                {
					byte[] abFromByte = Encoding.Unicode.GetBytes(strValue);
					byte[] abToByte = Encoding.Convert(Encoding.Unicode, Encoding.UTF8, abFromByte);
					Int32 n32UTF8BytesCount = Encoding.UTF8.GetCharCount(abToByte);
					if (n32UTF8BytesCount > (Int32)un32FixSize)
					{
						n32UTF8BytesCount = (Int32)un32FixSize;
					}
					this.m_cnbBuffer.Add(abToByte, 0, n32UTF8BytesCount);
					Int32 n32AddByteNum = (Int32)un32FixSize - n32UTF8BytesCount;
					if (n32AddByteNum > 0)
					{
						for (Int32 i = 0; i < n32AddByteNum; i++)
						{
							this.m_cnbBuffer.Add((byte)0);
						}
					}
                    this.UpdateMsgSize();
                }

                public void Add(char[] arrchValue, Int32 n32Size)
                {
					byte[] abFromByte = Encoding.Unicode.GetBytes(arrchValue);					
					byte[] abToByte = Encoding.Convert(Encoding.Unicode, Encoding.UTF8, abFromByte, 0, n32Size);
					Int32 n32UTF8BytesCount = Encoding.UTF8.GetCharCount(abToByte);
					if (n32UTF8BytesCount > n32Size)
					{
						n32UTF8BytesCount = n32Size;
					}
					this.m_cnbBuffer.Add(abToByte, 0, n32UTF8BytesCount);
					Int32 n32AddByteNum = n32Size - n32UTF8BytesCount;
					if (n32AddByteNum > 0)
					{
						for (Int32 i = 0; i < n32AddByteNum; i++)
						{
							this.m_cnbBuffer.Add((byte)0);
						}
					}
                    this.UpdateMsgSize();
                }

                public void Add(byte[] arrbData, Int32 n32Offset, Int32 n32Size)
                {
                    this.m_cnbBuffer.Add(arrbData, n32Offset, n32Size);
                    this.UpdateMsgSize();
                }

                public void Add(ref UInt64 rsGUID)
                {
                    this.m_cnbBuffer.Add(rsGUID);
                    this.UpdateMsgSize();
                }

                public Boolean GetBoolean()
                {
                    return this.m_cnbBuffer.GetBoolean();
                }

                public byte GetByte()
                {
                    return this.m_cnbBuffer.GetByte();
                }

                public char GetChar()
                {
                    return this.m_cnbBuffer.GetChar();
                }

                public UInt16 GetUInt16()
                {
                    return this.m_cnbBuffer.GetUInt16();
                }

                public Int16 GetInt16()
                {
                    return this.m_cnbBuffer.GetInt16();
                }

                public UInt32 GetUInt32()
                {
                    return this.m_cnbBuffer.GetUInt32();
                }

                public Int32 GetInt32()
                {
                    return this.m_cnbBuffer.GetInt32();
                }

                public UInt64 GetUInt64()
                {
                    return this.m_cnbBuffer.GetUInt64();
                }

                public Int64 GetInt64()
                {
                    return this.m_cnbBuffer.GetInt64();
                }

                public float GetFloat()
                {
                    return this.m_cnbBuffer.GetFloat();
                }

                public double GetDouble()
                {
                    return this.m_cnbBuffer.GetDouble();
                }

				public bool GetByteArray(byte[] arrch, Int32 n32CharArrayLen)
				{
					if (!this.m_cnbBuffer.GetBytesArray(arrch, n32CharArrayLen))
					{
						return false;
					}
					return true;
				}

                public bool GetCharArray(char[] arrch, Int32 n32CharArrayLen)
                {
					byte[] pTempUTF8ByteArray = new byte[n32CharArrayLen];
					if (!this.m_cnbBuffer.GetBytesArray(pTempUTF8ByteArray, n32CharArrayLen))
					{
						return false;
					}
					
					byte[] pTempUnicodeByteArray = Encoding.Convert(Encoding.UTF8, Encoding.Unicode, pTempUTF8ByteArray);
					
					Array.Copy(pTempUnicodeByteArray, arrch, n32CharArrayLen);
					
					return true;
                }
				
				public string GetString(UInt32 un32StrLen)
				{
					byte[] pTempUTF8ByteArray = new byte[(Int32)un32StrLen];
					if (!this.m_cnbBuffer.GetBytesArray(pTempUTF8ByteArray, (Int32)un32StrLen))
					{
						return null;
					}
					
					byte[] pTempUnicodeByteArray = Encoding.Convert(Encoding.UTF8, Encoding.Unicode, pTempUTF8ByteArray);
					return Encoding.Unicode.GetString(pTempUnicodeByteArray);
				}

                public string GetString()
                {
                    return this.m_cnbBuffer.GetString();
                }

                public UInt64 GetGUID()
                {
                    UInt64 sGUID;
                    sGUID = this.m_cnbBuffer.GetUInt64();
                    return sGUID;
                }

                public static Int32 PopMsgFromMemQueue(CMemQueue cMsgQueue, CMsg cMsg)
                {
                    Int32 n32Get = cMsgQueue.GetFront(cMsg.GetMsgBuffer(), (UInt32)s_n32MsgHeaderSize);
                    if (0 != n32Get)
                    {
                        return n32Get;
                    }

                    Int32 n32MsgSize = cMsg.GetMsgSize();
                    if (n32MsgSize >= cMsg.m_n32MsgSize)
                    {
                        return -2;
                    }

                    Int32 n32Pop = cMsgQueue.PopFront(cMsg.GetMsgBuffer(), (UInt32)n32MsgSize);
                    if (0 != n32Pop)
                    {
                        return -3;
                    }

                    cMsg.SetReadPos(s_n32MsgHeaderSize);
                    cMsg.SetWritePos(n32MsgSize);
                    return 0;
                }
            }
        }
    }
}