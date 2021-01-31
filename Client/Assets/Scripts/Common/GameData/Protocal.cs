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
            enum EMsgToBSFromGC
            {
                eMsgToBSFromGC_Begin = 32768,
                eMsgToBSFromGC_AskGateAddress,			//客户端请求Gate地址
                eMsgToBSFromGC_End,
            };

			enum EMsgToGSToCSFromGC
			{
				eMsgToGSToCSFromGC_Begin = 8192,
				eMsgToGSToCSFromGC_AskPingCS,
				eMsgToGSToCSFromGC_AskRegiste,
				eMsgToGSToCSFromGC_AskLogin,
				eMsgToGSToCSFromGC_AskLogout,
				eMsgToGSToCSFromGC_AskQuickBattle,
				eMsgToGSToCSFromGC_AskLeaveBattle,
				eMsgToGSToCSFromGC_AskChangeBattleSeat,
				eMsgToGSToCSFromGC_AskReadyBattle,
				eMsgToGSToCSFromGC_AskBeginBattle,
				eMsgToGSToCSFromGC_AskCancelReadyBattle,
				eMsgToGSToCSFromGC_AskSelectHero,
				eMsgToGSToCSFromGC_ReportLoadBattleComplete, //8204
				
				eMsgToGSToCSFromGC_BUYRunes = 8205,
				eMsgToGSToCSFromGC_EuipRunes,
				eMsgToGSToCSFromGC_UnloadRunes,
				eMsgToGSToCSFromGC_ComposeRunes,
				eMsgToGSToCSFromGC_ExchangeGold,
				eMsgToGSToCSFromGC_AskCreateBattle,
				eMsgToGSToCSFromGC_AskQuickMatch,
				eMsgToGSToCSFromGC_ChatInRoom,
				eMsgToGSToCSFromGC_AskComleteUserInfo,
				eMsgToGSToCSFromGC_TryToSelectHero,
				eMsgToGSToCSFromGC_AskAddBattle,//加入房间
				eMsgToGSToCSFromGC_AskUserGameInfo = 8216,//个人资料
                eMsgToGSToCSFromGC_AskAddObserver = 8217,
                eMsgToGSToCSFromGC_AskGuideInfo = 8218,
                eMsgToGSToCSFromGC_AskReConnetToBattle = 8219,
                eMsgToGSToCSFromGC_AskReconnect = 8220,
                
                eMsgToGSToCSFromGC_AskAddToFriendList = 8223,
                eMsgToGSToCSFromGC_AskRemoveFromFriendList = 8224,
                eMsgToGSToCSFromGC_AskSendMsgToUser = 8225,
                eMsgToGSToCSFromGC_AskInviteFriendsToBattle = 8226,
                eMsgToGSToCSFromGC_AskSendFriendPersonInfo = 8227,//请求查找信息
                eMsgToGSToCSFromGC_GCReplyAddFriendRequst = 8228,
				eMsgToGSToCSFromGC_GCReplyDownTime = 8229,
                eMsgToGSToCSFromGC_FinishUIGuideTask = 8231,
                eMsgToGSToCSFromGC_ForceQuit = 8232 ,    //客户端强制退出
                eMsgToGSToCSFromGC_AskGetLoginReWard = 8233,

                eMsgToGSToCSFromGC_Notice = 8240,
                eMsgToGSToCSFromGC_AskBuyGoods = 8224,
			}

			enum EMsgToGCFromGS
			{
				eMsgToGCFromGS_Begin = 0,
				eMsgToGCFromGS_GCAskPingRet = 1,
				eMsgToGCFromGS_NotifyUserBaseInfo = 2,
				eMsgToGCFromGS_NotifySystemAnnounce = 3,
				eMsgToGCFromGS_NotifyNetClash = 4,
				eMsgToGCFromGS_GCAskRet = 5,
				eMsgToGCFromGS_NotifyUserAssetChange = 6,
				eMsgToGCFromGS_NotifyHeroList = 7,
				eMsgToGCFromGS_NotifyBattleBaseInfo = 8,
				eMsgToGCFromGS_NotifyBattleSeatPosInfo = 9,
				eMsgToGCFromGS_NotifyBattleStateChange = 10,
				eMsgToGCFromGS_NotifyCurBattleChange = 11,
				eMsgToGCFromGS_NotifyGameObjectAppear = 12,
				eMsgToGCFromGS_NotifyGameObjectDisappear = 13,
				eMsgToGCFromGS_NotifyGameObjectFreeState = 14,
				eMsgToGCFromGS_NotifyGameObjectRunState = 15,
				eMsgToGCFromGS_NotifyGameObjectFlyState = 16,
				eMsgToGCFromGS_NotifyGameObjectSwimingState = 17,
				eMsgToGCFromGS_NotifyGameObjectPrepareSkillState = 18,
				eMsgToGCFromGS_NotifyGameObjectReleaseSkillState = 19,
				eMsgToGCFromGS_NotifyGameObjectDeadlState = 20,
				eMsgToGCFromGS_NotifyGoodsInfo = 21,
				eMsgToGCFromGS_NotifyBattleLoadingState = 22,
				eMsgToGCFromGS_NotifyBattleHeroInfo = 23,
				eMsgToGCFromGS_BagRunesInfoChange = 24,
				eMsgToGCFromGS_ExchangeGold = 25,
				eMsgToGCFromGS_Chat = 26,
				eMsgToGCFromGS_NotifySkillHitTarget = 27,
				eMsgToGCFromGS_NotifyHPChange = 28,
				eMsgToGCFromGS_NotifyMPChange = 29,
				eMsgToGCFromGS_NotifyToChooseHero = 30,
				eMsgToGCFromGS_NotifyChooseHeroTimeEnd = 31,
				eMsgToGCFromGS_NotifySkillUnitInfo = 32,
				eMsgToGCFromGS_NotifyFightPropertyInfo = 33,
				eMsgToGCFromGS_NotifyHPInfo = 34,
				eMsgToGCFromGS_NotifyMPInfo = 35,
				eMsgToGCFromGS_NotifyHeroInfo = 36,
                eMsgToGCFromGS_NotifyBuffInfo = 37,
                eMsgToGCFromGS_NotifyBuffRemove = 38,
                eMsgToGCFromGS_NotifySkillInfo = 39,
                eMsgToGCFromGS_NotifyBattleFinish = 40,
                eMsgToGCFromGS_NotifyExpInfo = 41,
                eMsgToGCFromGS_NotifyLevelInfo = 42,
                eMsgToGCFromGS_NotifyAbsorbBegin = 43,
                eMsgToGCFromGS_NotifyAbsorbMonsterResult=44, 
                eMsgToGCFromGS_NotifyCancelAbsorbSkill = 45,
				eMsgToGCFromGS_NotifyUserGameInfo = 46,
				eMsgToGCFromGS_NotifyHeroKills = 47,
				eMsgToGCFromGS_NotifyCurDeadTimes = 48,
				eMsgToGCFromGS_NotifyCurCP = 49,
				 eMsgToGCFromGS_NotifyCurAtkSpeed=50,
                eMsgToGCFromGS_NotifyFuryValue = 51,
                eMsgToGCFromGS_NotifyFuryState = 52,
				eMsgToGCFromGS_NotifyHeroRebornTimes = 53,
				eMsgToGCFromGS_NotifyBuyRebornSuccess = 54,
                eMsgToGCFromGS_NotifyUpdateMiniMap = 55,
                eMsgToGCFromGS_NotifyUpdateMiniMapWarning = 56,
				eMsgToGCFromGS_BroadcastBattleHeroInfo = 57,
				eMsgToGCFromGS_NotifyPersonalCPChange = 58,
				eMsgToGCFromGS_BroadCurBattleResult = 59,
				eMsgToGCFromGS_BroadBuildingDestroyByWho = 60,
				eMsgToGCFromGS_NotifyVoipRoomId = 61,
                eMsgToGCFromGS_NotifyHeroDisplacementInfo = 62,
                eMsgToGCFromGS_BroadcastPlayFinishVedio = 63,
                eMsgToGCFromGS_NotifyLeaveBattleSuccess = 64,
                eMsgToGCFromGS_NotifyBornSolder = 65,
                eMsgToGCFromGS_NotifySkillEnd = 66,
                eMsgToGCFromGS_NotifyAltarBSIco = 67,
                eMsgToGCFromGS_NotifyMonsterDebugInfo = 68,
                eMsgToGCFromGS_WarningToSelectHero = 69,
                eMsgToGCFromGS_NotifyGameObjectReleaseSkill = 70,
                eMsgToGCFromGS_NotifyGameObjectSkillCD = 71,
                eMsgToGCFromGS_NotifySkillModelEmit = 72,
                eMsgToGCFromGS_NotifySkillModelEmitDestroy = 73,
                eMsgToGCFromGS_NotifySkillModelHitTarget = 74,
                eMsgToGCFromGS_NotifySkillModelRange = 75,
                eMsgToGCFromGS_NotifySkillModelRangeEnd = 76,
                eMsgToGCFromGS_NotifySkillModelLeading = 77,
                eMsgToGCFromGS_NotifySkillModelSummonEffect = 78,
                eMsgToGCFromGS_NotifySkillModelBuf = 79,
                eMsgToGCFromGS_NotifySummonLifeTime = 80,
                eMsgToGCFromGS_NotifySkillModelStartForceMove = 81,
                eMsgToGCFromGS_NotifySkillModelStartForceMoveStop = 82,
                eMsgToGCFromGS_NotifySkillModelStartForceMoveTeleport = 83,
                eMsgToGCFromGS_NotifyPassitiveSkill = 84,//触发,有多次
                eMsgToGCFromGS_NotifyPassitiveSkilllaod = 85,//启动
                eMsgToGCFromGS_NotifyPassitiveSkillUnlaod = 86,//卸载
                eMsgToGCFromGS_NotifySkillModelEmitTurn = 87,
                eMsgToGCFromGS_NotifyAFPData = 90,
                eMsgToSSFromCS_NotifyBattleManagerChange = 91,
                eMsgToSSFromCS_NotifyBornObj = 92,
                eMsgToGCFromCS_NotifyReconnectInfo = 93,
                eMsgToGCFromCS_NotifyReconnectHeroGoodsInfo = 94,
                eMsgToGCFromGS_NotifyOBReturnBattleRoom = 95,
				eMsgToSSFromCS_NotifyDefaultHeros = 100,
                eMsgToSSFromCS_NotifyOBAppear = 101,
                eMsgToSSFromCS_NotifyKillNPC = 102,
                eMsgToGCFromGS_NotifyAltarHeadIcon = 103,
				eMsgToGCFromGS_NotifySkillUpLv = 104,
                eMsgToGCFromGS_NotifyUserFriendsList = 105,
                eMsgToGCFromGS_NotifyUserRemovesList = 106,
                eMsgToGCFromGS_UserBeInvitedToBattle = 107,
                eMsgToGCFromGS_NotifyMsgFromUser = 108,
                eMsgToGCFromGS_NotifyBeAddFriendMsg = 109,
                eMsgToGCFromGS_NotifyQueryNickNameRet = 110,
                eMsgToSSFromCS_NotifyKillsInfo = 111,
                eMsgToSSFromCS_NotifyGuideLastComStep = 112,
				eMsgToSSFromCS_NotifyBlastHurt = 121,
                eMsgToGCFromGS_NotifyHeroReborn = 124,
                eMsgToGCFromGS_NotifyCLdays = 125,

				eMsgToGCFromGS_NotifyGameObjectReliveState = 201,
                eMsgToGCFromGS_NotifyUserGuideSetUps = 202,
                eMsgToGCFromBS_AskGateAddressRet = 203,//返回Gate地址
                eMsgToGCFromGs_NotifySendSoldierTip = 210,
                eMsgToGCFromGs_NotifySecondaryGuideTask = 211,
                eMsgToGCFromGS_NotifyNotice = 212,
			}

			enum EmsgToSSFromGC
			{
				EmsgToSSFromGC_Begin = 16384,
				eMsgToGSToSSFromGC_AskPingSS,
				eMsgToGSToSSFromGC_AskEnterBattle,
//				eMsgToGSToSSFromGC_AskSelectHero,
				eMsgToGSToSSFromGC_AskMoveDir,
				eMsgToGSToSSFromGC_AskStopMove,
                eMsgToGSToSSFromGC_AskBuyGoods = 16389,
                eMsgToGSToSSFromGC_AskUseGoods = 16390,
                eMsgToGSToSSFromGC_AskSellGoods = 16391,
                eMsgToGSToSSFromGC_AskMoveGoods = 16392,
				eMsgToGSToSSFromGC_AskAttack=16394,
				eMsgToGSToSSFromGC_AskUseSkill=16395,
				eMsgToGSToSSFromGC_AskLockTargt=16396,
                eMsgToGSToSSFromGC_ReportAltarStrawSolder = 16397,
                eMsgToGSToSSFromGC_AskAbsorbMonster=16398,
                eMsgToGSToSSFromGC_AskCancelAbsorbSkill=16399,
				eMsgToGSToCSFromGC_AskGasExplosion = 16400,
				eMsgToGSToSSFromGC_AskBuyRebornHero = 16401,
                eMsgToGSToCSFromGC_GMCmd = 16402, 
				eMsgToGsToCSFromGC_GuideStep = 16404,
                eMsgToGsToCSFromGC_GuideSendNpc = 16405,
				eMsgToGSToSSFromGC_AskAutoAttack = 16406,
                eMsgToGSToSSFromGC_AskGuideEndJumpToHall = 16407,
                eMsgToGSToSSFromGC_AskReconnectInfo = 16408,

			}

			public class Protocal
            {
                public struct SMsgHeader
                {
                    public Int32 n32Size;
                    public Int32 n32ProtocalID;
                };

                public static Int32 s_n32MsgHeaderSize = 8;

                private CMemBuffer m_cnbBuffer = null;
                private Int32 m_n32MsgSize = 0;

				public Protocal(Int32 n32Size)
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
//                    if (n32MsgSize >= cMsg.m_n32MsgSize)
//                    {
//                        return -2;
//                    }

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