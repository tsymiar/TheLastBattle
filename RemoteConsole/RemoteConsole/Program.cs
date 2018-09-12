using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Xml;
using System.Threading;

namespace RemoteConsole
{
    static class Program
    {
       static public List<uint> platformlist = new List<uint>();
       static public List<CenterServer> serverList = new List<CenterServer>(); 
       static public List<MailInfoCfg> dbSfgList = new List<MailInfoCfg>();
       //static public List<Channel> channelList = new List<Channel>();
       static public List<Channel> channelList = new List<Channel>();
       static public List<HandSel> handselList = new List<HandSel>();
       static public List<MailTypeCfg> mailTypeList = new List<MailTypeCfg>();
       static public Dictionary<string, CenterServer.SCDKey_Event> ALLCDKeyEvents = new Dictionary<string, CenterServer.SCDKey_Event>();
       //static public List<UserTypeCfg> userTypeList = new List<UserTypeCfg>();
       static public CenterServer DBQuery = new CenterServer(); 

        static MainForm mainFrom;
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            mainFrom = new MainForm();

            try
            {
                XmlDocument xml = new XmlDocument();
                string appPath = AppDomain.CurrentDomain.BaseDirectory;
                //xml.Load(appPath + "\\ServerConfig.xml");   //XML地址
                xml.Load("cfg/ServerConfig.xml");
                XmlNodeList serverNodeList = xml.SelectNodes("/Servers/Server");
                if (null != serverNodeList)
                {
                    foreach (XmlNode serverNode in serverNodeList)
                    {
                        string name = serverNode.SelectSingleNode("name").InnerText;
                        string ip = serverNode.SelectSingleNode("ip").InnerText;
                        string port = serverNode.SelectSingleNode("port").InnerText;
                        string key = serverNode.SelectSingleNode("key").InnerText;
                        CenterServer server = new CenterServer();
                        server.Name = name;
                        server.Ip = ip;
                        server.Port = port;
                        server.Key = key;
                        serverList.Add(server); 
                    }
                }

                LoadMailDbCfg();
                LoadDBQueryInfo();
            }
            catch (Exception e)
            {
                Console.Write(e.ToString());
            }
            
            foreach(CenterServer server in serverList){
                server.start();
            }

            Application.EnableVisualStyles();  
            
            mainFrom.FreshCSServerView(serverList);

            mainFrom.AddMailServerList(serverList);

            mainFrom.AddMailTypeList(mailTypeList); 

            mainFrom.FreshChannelView(channelList);

            mainFrom.AddNoticeServerList(serverList);

            mainFrom.AddNoticePlatformList(channelList);

            mainFrom.AddDBQuery(DBQuery); 

            Application.Run(mainFrom); 
        } 

        static public void close(){
            
            foreach (CenterServer server in serverList)
            {
                server.stop();
            }
           
        }

        static public void LoadMailDbCfg()
        {
            XmlDocument xml = new XmlDocument();
            string appPath = AppDomain.CurrentDomain.BaseDirectory;
            //xml.Load(appPath + "\\ServerConfig.xml");   //XML地址
            xml.Load("cfg/DbConfig.xml");
            XmlNodeList serverNodeList = xml.SelectNodes("/DbCfg/mail");
            if (null != serverNodeList)
            {
                foreach (XmlNode serverNode in serverNodeList)
                {
                    //string csip = serverNode.SelectSingleNode("csname").InnerText;
                    string ip = serverNode.SelectSingleNode("csdb").InnerText; 
                    string user = serverNode.SelectSingleNode("user").InnerText;
                    string pwd = serverNode.SelectSingleNode("pwd").InnerText;
                    string db = serverNode.SelectSingleNode("dbname").InnerText;

                    MailInfoCfg dbcfg = new MailInfoCfg();
                    dbcfg.InitDbCfg(ip,  user,pwd,db);
                    dbSfgList.Add(dbcfg); 
                }
            }

            LoadChannelCfg();
        }

        static public void LoadDBQueryInfo()
        {
            XmlDocument xml = new XmlDocument();
            string appPath = AppDomain.CurrentDomain.BaseDirectory;

            xml.Load("cfg/DBQueryCfg.xml");
            XmlNodeList DBQueryNode = xml.SelectNodes("/Servers/Server");
            if (null != DBQueryNode)
            {
                foreach (XmlNode serverNode in DBQueryNode)
                {
                    string name = serverNode.SelectSingleNode("name").InnerText;
                    string ip = serverNode.SelectSingleNode("ip").InnerText;
                    string port = serverNode.SelectSingleNode("port").InnerText;
                    string key = serverNode.SelectSingleNode("key").InnerText;

                    DBQuery.Name = name;
                    DBQuery.Ip = ip;
                    DBQuery.Port = port;
                    DBQuery.Key = key;
                }
            }
        }
        

        static public void LoadChannelCfg()
        {
            XmlDocument xml = new XmlDocument();
            string appPath = AppDomain.CurrentDomain.BaseDirectory;

            xml.Load("cfg/Channel.xml");
            XmlNodeList serverNodeList = xml.SelectNodes("/SDK/Info");
            if (null != serverNodeList)
            {
                foreach (XmlNode serverNode in serverNodeList)
                {
                    string name = serverNode.SelectSingleNode("name").InnerText;
                    string idx = serverNode.SelectSingleNode("number").InnerText; 

                    Channel sdk = new Channel();
                    sdk.Set(name, Convert.ToInt32(idx));
                    channelList.Add(sdk);
                }
            }

            LoadHandSelCfg();
        }

        static public void LoadHandSelCfg()
        {
            XmlDocument xml = new XmlDocument();
            string appPath = AppDomain.CurrentDomain.BaseDirectory;

            xml.Load("cfg/MailGiftTypeCfg.xml"); 
            XmlNodeList serverNodeList = xml.SelectNodes("/handsel/seltype");
            if (null != serverNodeList)
            {
                foreach (XmlNode serverNode in serverNodeList)
                {
                    string name = serverNode.SelectSingleNode("name").InnerText;
                    string idx = serverNode.SelectSingleNode("type").InnerText;

                    HandSel sel = new HandSel();
                    sel.InitHandSel(name, Convert.ToInt32(idx));
                    handselList.Add(sel);
                }
            }

            LoadMailType();
        }

        static public void LoadMailType()
        {
            XmlDocument xml = new XmlDocument();
            string appPath = AppDomain.CurrentDomain.BaseDirectory;

            xml.Load("cfg/MailTypeCfg.xml");
            XmlNodeList serverNodeList = xml.SelectNodes("/mailType/stype");
            if (null != serverNodeList)
            {
                foreach (XmlNode serverNode in serverNodeList)
                {
                    string name = serverNode.SelectSingleNode("name").InnerText;
                    string idx = serverNode.SelectSingleNode("type").InnerText;

                    MailTypeCfg mt = new MailTypeCfg();

                    mt.InitMailType( name, Convert.ToInt32(idx));

                    mailTypeList.Add(mt);
                }
            }

            //LoadUserMailType();
        }

//         static public void LoadPlatformCfg()
//         {
//             XmlDocument xml = new XmlDocument();
//             string appPath = AppDomain.CurrentDomain.BaseDirectory;
// 
//             xml.Load("cfg/MailTypeCfg.xml");
//             XmlNodeList serverNodeList = xml.SelectNodes("/mailType/stype");
//             if (null != serverNodeList)
//             {
//                 foreach (XmlNode serverNode in serverNodeList)
//                 {
//                     string name = serverNode.SelectSingleNode("name").InnerText;
//                     string idx = serverNode.SelectSingleNode("type").InnerText;
// 
//                     MailTypeCfg mt = new MailTypeCfg();
// 
//                     mt.InitMailType(name, Convert.ToInt32(idx));
// 
//                     mailTypeList.Add(mt);
//                 }
//             }
//         }

        //static public void LoadUserMailType()
        //{
        //    XmlDocument xml = new XmlDocument();
        //    string appPath = AppDomain.CurrentDomain.BaseDirectory;

        //    xml.Load("mailType.xml");
        //    XmlNodeList serverNodeList = xml.SelectNodes("/mailType/stype");
        //    if (null != serverNodeList)
        //    {
        //        foreach (XmlNode serverNode in serverNodeList)
        //        {
        //            string name = serverNode.SelectSingleNode("name").InnerText;
        //            string idx = serverNode.SelectSingleNode("type").InnerText;

        //            UserTypeCfg mt = new UserTypeCfg();

        //            mt.InitUserType(name, Convert.ToInt32(idx)); 

        //            userTypeList.Add(mt);
        //        }
        //    }
        //}

        static public bool CheckHandSelType(int ntype)
        {
            foreach (HandSel sel in handselList)
            {
                if (sel.index == ntype)
                {
                    return true;
                }
            }
            return false;
        }

        static public CenterServer GetSelectServer(string serverIp)
        { 
                 foreach (CenterServer cs in serverList)
                 {
                     if (cs.Ip.CompareTo(serverIp) == 0)
                     {
                         return cs; 
                     }
                 }
                 return null;
        }
    }
}
