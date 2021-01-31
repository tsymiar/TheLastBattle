using UnityEngine;
using System.Collections;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using JT.FWW.Tools;
using JT.FWW.GameData;
using BlGame.Resource;

namespace JT
{
    namespace FWW
    {
        namespace Tools
        {
            public class CConfigReader
            {
                private TextAsset m_pcText = null;
                private string m_pcString = null;
                private Int32 m_n32ReadPos = 0;
                static  StringBuilder s_pcTempBuilder = null;

                private Boolean IsValidChar(Char cTempChar)
                {
					if ('\t' == cTempChar || '\n' == cTempChar || '\r' == cTempChar)
                    {
                        return false;
                    }
                    return true;
                }

                private Boolean IsChangeLineChar(Char cTempChar)
                {
                    if ('\n' == cTempChar || '\r' == cTempChar)
                    {
                        return true;
                    }
                    return false;
                }

                public  CConfigReader()
                {
                    m_pcText = null;
                    m_pcString = null;
                    m_n32ReadPos = 0;
                    if (null == s_pcTempBuilder)
                    {
                        s_pcTempBuilder = new StringBuilder();
                    }
                }

                public  Int32   Init(string paConfigFile)
                {
                    //m_pcText = (TextAsset)Resources.Load(paConfigFile);
                    ResourceUnit texUnit = ResourcesManager.Instance.loadImmediate(paConfigFile, ResourceType.ASSET);
                    m_pcText = texUnit.Asset as TextAsset;


                    m_n32ReadPos = 0;
                    if (null == m_pcText)
                    {
                        return (Int32)EErrorCode.eEC_LoadConfigFileFail;
                    }

                    m_pcString = m_pcText.text;

                    return (Int32)EErrorCode.eNormal;
                }

                public Int32 Uninit()
                {
                    Resources.UnloadAsset(m_pcText);
                    m_pcText = null;
                    m_pcString = null;
                    return (Int32)EErrorCode.eNormal;
                }

                public bool EOf()
                {
                    if (m_n32ReadPos >= m_pcString.Length)
                    {
                        return true;
                    }

                    return false;
                }

                public Int32 GetTextLength()
                {
                    return m_pcString.Length;
                }

                public Char ReadChar()
                {
                    if (EOf())
                    {
                        return '\0';
                    }
                    return m_pcString[m_n32ReadPos++];
                }
				
				public Char CurChar()
				{
					if (EOf ())
					{
						return '\0';
					}
					return m_pcString[m_n32ReadPos];
				}
				
                public Boolean SkipChar()
                {
                    if (EOf())
                    {
                        return false;
                    }
                    m_n32ReadPos++;
                    return true;
                }

                public  Boolean   ReadTo(string pcMark)
                {
                    string pcReadStr = ReadWords();
                    Boolean bFind = false;
                    while (!EOf())
                    {
                        if (pcReadStr == pcMark)
                        {
                            bFind = true;
                            break;
                        }
                        pcReadStr = ReadWords();
                    }
                    return bFind;
                }

                public  string  ReadLine()
                {
                    s_pcTempBuilder.Length = 0;
                    Char cTempChar = ReadChar();

                    while (!IsChangeLineChar(cTempChar) && !EOf())
                    {
                        s_pcTempBuilder.Append(cTempChar);
                        cTempChar = ReadChar();
                    }

                    if (s_pcTempBuilder.Length > 0)
                    {
                        return s_pcTempBuilder.ToString();
                    }
                    else
                    {
                        return null;
                    }
                }

                public Boolean SkipLine()
                {
                    Char cTempChar = ReadChar();

                    while (!IsChangeLineChar(cTempChar) && !EOf())
                    {
                        cTempChar = ReadChar();
                    }

                    return true;
                }
				public string ReadWords()
				{
					Boolean bOnlyDigitF = false;
					string de = ReadWordsHe (bOnlyDigitF);
					return de;
				}

                public string ReadWords (Boolean bOnlyDigitT)
                {
					string de = ReadWordsHe (bOnlyDigitT);
					return de;
                }
				public string ReadWordsHe(Boolean bOnlyDigitTF)
				{
					s_pcTempBuilder.Length = 0;
					
					while(!IsValidChar(CurChar()) && !EOf())
					{
						SkipChar();
					}
					
					while (!EOf () && IsValidChar(CurChar()))
					{
						if (bOnlyDigitTF)
						{
							if (Char.IsDigit(CurChar()))
							{
								s_pcTempBuilder.Append(CurChar());
							}
						}
						else
						{
							s_pcTempBuilder.Append(CurChar());
						}
						SkipChar();
					}
					
					if (s_pcTempBuilder.Length > 0)
					{
						return s_pcTempBuilder.ToString();
					}
					else
					{
						return null;
					}
				}
				
				public Boolean ReadCharArray(char[] pszCharArray, Int32 n32ArraySize)
				{
                    while (!IsValidChar(CurChar()) && !EOf())
                    {
                        SkipChar();
                    }
					
					Int32 n32Index = 0;
					while (!EOf() && IsValidChar(CurChar()))
                    {
						pszCharArray[n32Index++] = CurChar();
						SkipChar();
						if (n32Index >= n32ArraySize)
						{
							break;
						}
                    }
					
					if (n32Index < n32ArraySize)
					{
						return false;
					}
					
					return true;
				}

                public Boolean ReadBoolean()
                {
                    Int32 n32Value = 0;
                    string pcTempStr = ReadWords(true);
                    if (null != pcTempStr)
                    {
                        n32Value = Convert.ToInt32(pcTempStr);
                    }

                    if (0 == n32Value)
                    {
                        return false;
                    }

                    return true;
                }

                public byte ReadByte()
                {
                    byte bValue = 0;
                    string pcTempStr = ReadWords();
                    if (null != pcTempStr)
                    {
                        bValue = Convert.ToByte(pcTempStr);
                    }
                    return bValue;
                }

                public Int16 ReadInt16()
                {
                    Int16 n16Value = 0;
                    string pcTempStr = ReadWords(true);
                    if (null != pcTempStr)
                    {
                        n16Value = Convert.ToInt16(pcTempStr);
                    }
                    return n16Value;
                }

                public UInt16 ReadUInt16()
                {
                    UInt16 un16Value = 0;
                    string pcTempStr = ReadWords(true);
                    if (null != pcTempStr)
                    {
                        un16Value = Convert.ToUInt16(pcTempStr);
                    }
                    return un16Value;
                }

                public Int32 ReadInt32()
                {
                    Int32 n32ReadValue = 0;
                    string pcTempStr = ReadWords(true);
                    if (null != pcTempStr)
                    {
                       n32ReadValue = Convert.ToInt32(pcTempStr);
                    }
                    return n32ReadValue;
                }

                public UInt32 ReadUInt32()
                {
                    UInt32 un32ReadValue = 0;
                    string pcTempStr = ReadWords(true);
                    if (null != pcTempStr)
                    {
                        un32ReadValue = Convert.ToUInt32(pcTempStr);
                    }
                    return un32ReadValue;
                }

                public Int64 ReadInt64()
                {
                    Int64 n64ReadValue = 0;
                    string pcTempStr = ReadWords();
                    if (null != pcTempStr)
                    {
                        n64ReadValue = Convert.ToInt64(pcTempStr);
                    }
                    return n64ReadValue;
                }

                public UInt64 ReadUInt64()
                {
                    UInt64 un64ReadValue = 0;
                    string pcTempStr = ReadWords(true);
                    if (null != pcTempStr)
                    {
                        un64ReadValue = Convert.ToUInt64(pcTempStr);
                    }
                    return un64ReadValue;
                }

                public float ReadFloat()
                {
                    float fReadValue = 0f;
                    string pcTempStr = ReadWords(true);
                    if (null != pcTempStr)
                    {
                        fReadValue = (float)Convert.ToDouble(pcTempStr);
                    }
                    return fReadValue;
                }
				
				public Double ReadDouble()
				{
					Double dReadValue = 0f;
					string pcTempStr = ReadWords(true);
					if (null != pcTempStr)
                    {
                        dReadValue = (Double)Convert.ToDouble(pcTempStr);
                    }
                    return dReadValue;
				}
            }
        }
    }
}