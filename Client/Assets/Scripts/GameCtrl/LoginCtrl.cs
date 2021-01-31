using UnityEngine;
using System;
using System.Collections;
using BlGame;
using JT.FWW.GameData;
using BlGame.GameData;
using BlGame.Network;
using LSToGC;
using System.IO;
using System.Linq;
using BlGame.Model;

namespace BlGame.Ctrl
{
    public class LoginCtrl : Singleton<LoginCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_LoginEnter);

            SdkLink();
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_LoginExit);
        }

        //登陆
        public void Login(string account, string pass)
        {
            SelectServerData.Instance.SetServerInfo((int)SdkManager.Instance.GetPlatFrom(), account, pass);
            NetworkManager.Instance.canReconnect = false;
            NetworkManager.Instance.Close();
            NetworkManager.Instance.Init(JxBlGame.Instance.LoginServerAdress, 49996, NetworkManager.ServerType.LoginServer);
        }

        //登陆错误反馈
        public void LoginError(int code)
        {
            MsgInfoManager.Instance.ShowMsg(code);
            EventCenter.Broadcast<EErrorCode>(EGameEvent.eGameEvent_LoginError, (EErrorCode)code);
        }

        //接收GateServer信息
        public void RecvGateServerInfo(Stream stream)
        {
            BSToGC.AskGateAddressRet pMsg = ProtoBuf.Serializer.Deserialize<BSToGC.AskGateAddressRet>(stream);
            SelectServerData.Instance.GateServerAdress = pMsg.ip;
            SelectServerData.Instance.GateServerPort = pMsg.port;
            SelectServerData.Instance.GateServerToken = pMsg.token;
            SelectServerData.Instance.SetGateServerUin(pMsg.user_name);
            NetworkManager.Instance.canReconnect = false;
            NetworkManager.Instance.Close();
            NetworkManager.Instance.Init(pMsg.ip, pMsg.port, NetworkManager.ServerType.GateServer); 

            EventCenter.Broadcast(EGameEvent.eGameEvent_LoginSuccess);
        }

        //登陆失败
        public void LoginFail()
        {
            NetworkManager.Instance.canReconnect = false;
            EventCenter.Broadcast(EGameEvent.eGameEvent_LoginFail);
        }

        //选择LoginServer
        public void SelectLoginServer(int i)
        {
            string ip = JxBlGame.Instance.ipList.ElementAt(i);
            JxBlGame.Instance.LoginServerAdress = ip;
        }

        //更新服务器列表
        public void UpdateServerAddr(Stream stream)
        {
            LSToGC.ServerBSAddr pMsg = ProtoBuf.Serializer.Deserialize<LSToGC.ServerBSAddr>(stream);

            SelectServerData.Instance.Clean();
            for (int i = 0; i < pMsg.serverinfo.Count; i++)
            {
                string addr = pMsg.serverinfo.ElementAt(i).ServerAddr;
                int state = pMsg.serverinfo.ElementAt(i).ServerState;
                string name = pMsg.serverinfo.ElementAt(i).ServerName;
                int port = pMsg.serverinfo.ElementAt(i).ServerPort;
                SelectServerData.Instance.SetServerList(i, name, (SelectServerData.ServerState)state, addr, port);
            }
            
            NetworkManager.Instance.Close();
            NetworkManager.Instance.canReconnect = false;

            SelectServerData.Instance.SetDefaultServer();

            EventCenter.Broadcast(EGameEvent.eGameEvent_SdkServerCheckSuccess);
        }

        //选择服务器
        public void SelectServer(int id)
        {
            SelectServerData.Instance.SetSelectServer(id);
            EventCenter.Broadcast(EGameEvent.eGameEvent_SelectServer); 
        }

        //开始游戏
        public void GamePlay()
        {
            int index = SelectServerData.Instance.curSelectIndex;
            SelectServerData.ServerInfo info = SelectServerData.Instance.GetServerDicInfo().ElementAt(index).Value;
            NetworkManager.Instance.canReconnect = false;
            NetworkManager.Instance.Close();
            NetworkManager.Instance.Init(info.addr, info.port, NetworkManager.ServerType.BalanceServer);
            PlayerPrefs.SetString(SelectServerData.serverKey, info.name);
        }

        //SD注册成功
        public void SdkRegisterSuccess(string uin, string sessionId)
        {

            SelectServerData.Instance.SetServerInfo((int)SdkManager.Instance.GetPlatFrom(), uin, sessionId);

            NetworkManager.Instance.canReconnect = false;
            NetworkManager.Instance.Close();
            NetworkManager.Instance.Init(JxBlGame.Instance.LoginServerAdress, JxBlGame.Instance.LoginServerPort, NetworkManager.ServerType.LoginServer);                      
        }

        //SDK注消
        public void SdkLogOff()
        {
        //    NetworkManager.Instance.canReconnect = false;
        //    NetworkManager.Instance.Close();
            EventCenter.Broadcast(EGameEvent.eGameEvent_SdkLogOff);

            SdkLink();            
        }

        // 连接SDK
        public void SdkLink()
        {
            Debugger.Log("----sdk link----");

            
            #if UNITY_STANDALONE_WIN || UNITY_EDITOR || SKIP_SDK
            #else
                if (!SdkConector.islink)
                {
                       SdkConector.islink = true;
                       SdkConector.isLogOut = false;
                       SdkConector.Logout(1);
                } 
#endif
        }

   }
}
