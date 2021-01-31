using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;
using JT.FWW.Tools;
using System.Text;

namespace JT
{
    namespace FWW
    {
        namespace Tools
		{
            public class CMemBuffer
            {
                private Int32 m_n32BufferSize = 0;
                private MemoryStream m_msStream = null;
                private Int32 m_n32ReadPos = 0;
                private Int32 m_n32WritePos = 0;

                public CMemBuffer(Int32 n32Size)
                {
                    this.m_msStream = new MemoryStream((Int32)n32Size);
                    this.m_n32BufferSize = n32Size;
                }

                public Int32 ReadPos
                {
                    get { return m_n32ReadPos; }
                    set { m_n32ReadPos = value; }
                }

                public Int32 WritePos
                {
                    get { return m_n32WritePos; }
                    set { m_n32WritePos = value; }
                }

                public byte[] GetMemBuffer()
                {
                    return this.m_msStream.GetBuffer();
                }

                public Int32 GetBufferSize()
                {
                    return this.m_n32BufferSize;
                }

                public Int32 GetWritePos()
                {
                    return this.m_n32WritePos;
                }

                public Int32 GetReadPos()
                {
                    return this.m_n32ReadPos;
                }

                public Int32 AddWritePos(Int32 n32AddSize)
                {
                    if (this.m_n32BufferSize >= this.m_n32WritePos + n32AddSize)
                    {
                        this.m_n32WritePos += n32AddSize;
                    }
                    return this.m_n32WritePos;
                }

                public Int32 AddReadPos(Int32 n32AddSize)
                {
                    if (this.m_n32BufferSize >= this.m_n32ReadPos + n32AddSize)
                    {
                        this.m_n32ReadPos += n32AddSize;
                    }
                    return this.m_n32ReadPos;
                }

                public void SetReadPos(Int32 n32NewPos)
                {
                    this.m_n32ReadPos = n32NewPos;
                }

                public void SetWritePos(Int32 n32NewPos)
                {
                    this.m_n32WritePos = n32NewPos;
                }

                public Int32 Resize(Int32 n32NewSize)
                {
                    //check parameter.
                    if (n32NewSize <= 0)
                    {
                        return 0;
                    }
                    if (this.m_n32WritePos >= n32NewSize)
                    {
                        return 0;
                    }

                    //really resize the buffer.
                    //this.m_msStream.SetLength(n32NewSize);

                    MemoryStream msOld = this.m_msStream;
                    this.m_msStream = new MemoryStream((Int32)(n32NewSize));
                    this.m_n32BufferSize = n32NewSize;
                    //msOld.Seek(0 , SeekOrigin.Begin);
                    this.m_msStream.Seek(0, SeekOrigin.Begin);
                    this.m_msStream.Write(msOld.GetBuffer(), 0, (Int32)this.m_n32WritePos);

                    return n32NewSize;
                }

                public Boolean ReWrite(Int32 n32Pos, byte[] arrbData, Int32 n32Size)
                {
                    //check parameters.
                    if (n32Size + n32Pos > this.m_n32BufferSize)
                    {
                        return false;
                    }

                    //copy data.
                    this.m_msStream.Seek(n32Pos, SeekOrigin.Begin);
                    this.m_msStream.Write(arrbData, 0, (Int32)n32Size);
                    return true;
                }

                private void AddData(byte[] arrByteStream, Int32 n32Size)
                {
                    //check the stream size.
                    if (this.m_n32BufferSize <= this.m_n32WritePos + n32Size)
                    {
                        Int32 n32IncrementSize = n32Size * 2;
                        this.Resize(this.m_n32BufferSize + n32IncrementSize);
                    }

                    //write the data.
                    this.m_msStream.Seek(this.m_n32WritePos, SeekOrigin.Begin);
                    this.m_msStream.Write(arrByteStream, 0, (Int32)n32Size);
                    this.m_n32WritePos += n32Size;
                }

                public void Add(Boolean blValue)
                {
                    Int32 n32TypeSize = sizeof(Boolean);
                    byte[] arrByteStream = BitConverter.GetBytes(blValue);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(byte bValue)
                {
                    Int32 n32TypeSize = sizeof(byte);
                    byte[] arrByteStream = BitConverter.GetBytes(bValue);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(char nchValue)
                {
                    Int32 n32TypeSize = sizeof(char);

                    byte[] arrByteStream = BitConverter.GetBytes(nchValue);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(UInt16 n16Value)
                {
                    Int32 n32TypeSize = sizeof(UInt16);
                    byte[] arrByteStream = BitConverter.GetBytes(n16Value);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(Int16 n16Value)
                {
                    Int32 n32TypeSize = sizeof(Int16);
                    byte[] arrByteStream = BitConverter.GetBytes(n16Value);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(UInt32 un32Value)
                {
                    Int32 n32TypeSize = sizeof(UInt32);
                    byte[] arrByteStream = BitConverter.GetBytes(un32Value);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(Int32 n32Value)
                {
                    Int32 n32TypeSize = sizeof(Int32);
                    byte[] arrByteStream = BitConverter.GetBytes(n32Value);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(UInt64 n64Value)
                {
                    Int32 n32TypeSize = sizeof(UInt64);
                    byte[] arrByteStream = BitConverter.GetBytes(n64Value);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(Int64 n64Value)
                {
                    Int32 n32TypeSize = sizeof(Int64);
                    byte[] arrByteStream = BitConverter.GetBytes(n64Value);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(float fValue)
                {
                    Int32 n32TypeSize = sizeof(float);
                    byte[] arrByteStream = BitConverter.GetBytes(fValue);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(double dValue)
                {
                    Int32 n32TypeSize = sizeof(double);
                    byte[] arrByteStream = BitConverter.GetBytes(dValue);
                    this.AddData(arrByteStream, (Int32)n32TypeSize);
                }

                public void Add(string strValue)
                {
                    ////Int32  n32TypeSize = sizeof(char);
                    //Int32 n32Index = strValue.IndexOf('\0');
                    //if (-1 != n32Index)
                    //{
                    //    strValue = strValue.Substring(0, n32Index);
                    //}
                    //char[] arrch = strValue.ToCharArray();
                    //for (Int32 i = 0; i < strValue.Length; i++)
                    //{
                    //    this.Add(arrch[i]);
                    //}
                    //this.Add('\0');
                    for (Int32 i = 0; i < strValue.Length; i++)
                    {
                        Add(strValue[i]);
                    }
                    Add('\0');
                }

                public void Add(string strValue, UInt32 un32FixSize)
                {
                    for (UInt32 i = 0; i < un32FixSize; i++)
                    {
                        if (i < strValue.Length)
                        {
                            Add(strValue[(Int32)i]);
                        }
                        else
                        {
                            Add('\0');
                        }
                    }
                }

                public void Add(char[] arrchValue, Int32 n32Size)
                {
                    //this.AddData((byte[])arrchValue , n32Size);
                    for (Int32 i = 0; i < n32Size; i++)
                    {
                        this.Add(arrchValue[i]);
                    }
                    //this.Add('\0');
                }

                public void Add(byte[] arrbData, Int32 n32Offset, Int32 n32Size)
                {
                  // this.AddData(arrbData[i + n32Offset] , n32Size);

                    for (Int32 i = 0; i < n32Size; i++)
                    {
						if(i >= arrbData.Length){
							this.Add((byte)0);
						}
						else{
							this.Add(arrbData[i + n32Offset]);
						}
                        
                    }
                }

                public Boolean GetBoolean()
                {
                    Boolean bValue = BitConverter.ToBoolean(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    this.m_n32ReadPos += sizeof(Boolean);
                    return bValue;
                }

                public byte GetByte()
                {
                    this.m_msStream.Seek(this.m_n32ReadPos, SeekOrigin.Begin);
                    byte bValue = Convert.ToByte(this.m_msStream.ReadByte());
                    this.m_n32ReadPos++;
                    return bValue;
                }

                public char GetChar()
                {
                    char chValue = BitConverter.ToChar(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    this.m_n32ReadPos += sizeof(char);
                    return chValue;
                }

                public UInt16 GetUInt16()
                {
                    UInt16 n16Value = BitConverter.ToUInt16(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    this.m_n32ReadPos += sizeof(UInt16);
                    return n16Value;
                }

                public Int16 GetInt16()
                {
                    Int16 n16Value = BitConverter.ToInt16(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    this.m_n32ReadPos += sizeof(Int16);
                    return n16Value;
                }

                public UInt32 GetUInt32()
                {
                    UInt32 un32Value = BitConverter.ToUInt32(this.m_msStream.GetBuffer(), this.m_n32ReadPos);
                    this.m_n32ReadPos += sizeof(UInt32);
                    return un32Value;
                }

                public Int32 GetInt32()
                {
                    Int32 n32Value = BitConverter.ToInt32(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    this.m_n32ReadPos += sizeof(Int32);
                    return n32Value;
                }

                public UInt64 GetUInt64()
                {
                    UInt64 n64Value = BitConverter.ToUInt64(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    this.m_n32ReadPos += sizeof(UInt64);
                    return n64Value;
                }

                public Int64 GetInt64()
                {
                    Int64 n64Value = BitConverter.ToInt64(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    this.m_n32ReadPos += sizeof(Int64);
                    return n64Value;
                }

                public float GetFloat()
                {
                    float nValue = BitConverter.ToSingle(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    this.m_n32ReadPos += sizeof(float);
                    return nValue;
                }

                public double GetDouble()
                {
                    double dValue = BitConverter.ToDouble(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    this.m_n32ReadPos += sizeof(double);
                    return dValue;
                }

                public bool GetBytesArray(byte[] arrBData, Int32 n32GetSize)
                {
                    if (this.m_n32ReadPos + n32GetSize > this.m_n32WritePos)
                    {
                        return false;
                    }
                    this.m_msStream.Seek(this.m_n32ReadPos, SeekOrigin.Begin);
                    this.m_msStream.Read(arrBData, 0, (Int32)n32GetSize);
                    this.m_n32ReadPos += n32GetSize;
                    return true;
                }

                public bool GetCharArray(char[] arrch, Int32 n32CharArrayLen)
                {
                    //char[] arrch = new Char[n32CharArrayLen];
                    if (this.m_n32ReadPos + n32CharArrayLen * sizeof(char) > this.m_n32WritePos)
                    {
                        return false;
                    }
                    for (Int32 i = 0; i < n32CharArrayLen; i++)
                    {
                        arrch[i] = this.GetChar();
                    }
                    return true;
                }

                public string GetString(Int32 n32FixLen)
                {
                    //char chValue = BitConverter.ToChar(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    //string myTempStr = new string(chValue);
                    //string myTempStr = new string('\0');
                    //Int32 n32Index = 0;
                    //while ((this.m_n32ReadPos + sizeof(char)) < this.m_n32WritePos && myChar != '\0')
                    //{
                    //    myTempStr[n32Index] = GetChar();
                    //    n32Index++;
                    //}
                    //return myTempStr;

                    //string  myStr = BitConverter.ToString(this.m_msStream.GetBuffer() , (Int32)this.m_n32ReadPos);
                    //this.m_n32ReadPos += sizeof(char) * (Int32)myStr.Length;
                    //char  myChar = this.GetChar();

                    //char [] arrch = BitConverter.ToString(this.m_msStream.GetBuffer() , (Int32)this.m_n32ReadPos).ToCharArray();
                    //string  myStr = new  string(arrch);

                    //char[] arrch = new char[1];
                    //arrch[0] = this.GetChar();
                    //string  myStr = new string(arrch);

                    // char chValue = BitConverter.ToChar(this.m_msStream.GetBuffer(), (Int32)this.m_n32ReadPos);
                    //string cstrTemp = new string((char[])m_msStream.GetBuffer()), (Int32)this.m_n32ReadPos);

                    Int32 n32TempLen = n32FixLen;
                    if (0 == n32TempLen)
                    {
                        n32TempLen = 100;
                    }
                    Int32 n32GetLen = 0;
                    StringBuilder cSB = new StringBuilder(n32TempLen);
                    char myChar = this.GetChar();
                    n32GetLen++;
                    while ('\0' != myChar)
                    {
                        cSB.Append(myChar);
                        myChar = this.GetChar();
                        n32GetLen++;
                        if (n32FixLen > 0 && n32GetLen >= n32FixLen)
                        {
                            break;
                        }
                    }

                    if (n32FixLen > 0 && n32GetLen < n32FixLen)
                    {
                        Int32 n32AddWritePos = n32FixLen - n32GetLen;
                        m_n32WritePos += n32AddWritePos;
                    }

                    return cSB.ToString();
             
                    //Int32 n32InsertPos = 1;
                    //while ((this.m_n32ReadPos + sizeof(char)) < this.m_n32WritePos && myChar != '\0')
                    //{
                    //    //myStr = myStr + arrch[0].ToString();
                    //    //myStr.Insert(n32InsertPos , arrch[0].ToString());
                    //    //n32InsertPos++;
                    //    //arrch[0] = this.GetChar();	
                    //    n32InsertPos++;
                    //    myChar = this.GetChar();
                    //}
                    ////this.AddReadPos(-n32InsertPos * sizeof(char));
                    //this.SetReadPos(this.GetReadPos() - n32InsertPos * sizeof(char));
                    //char[] arrch = new char[n32InsertPos];
                    //for (Int32 i = 0; i < n32InsertPos; i++)
                    //{
                    //    arrch[i] = this.GetChar();
                    //}
                    //return new string(arrch);
                }



                public string GetString()
                {
                    StringBuilder cSB = new StringBuilder(10);
                    char myChar = this.GetChar();
      
                    while ('\0' != myChar)
                    {
                        cSB.Append(myChar);
                        myChar = this.GetChar();
                    }

                    return cSB.ToString();
                }
            }
            
        }
    }
}