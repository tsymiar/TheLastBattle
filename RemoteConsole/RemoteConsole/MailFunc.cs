using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows.Forms;

namespace RemoteConsole
{
    public class MailFunc
    {  
        static public List<String> errorStr = new List<String>();

        static public List<UserInfo> existUserList = new List<UserInfo>();
        static public List<UserInfo> userList = new List<UserInfo>();
        static public List<UInt64> userIdList = new List<UInt64>(); 

        
        //22,333,444,555
        static private void SplitUserStr(int type, String str)
        {  
            List<String> listSplit = Tools.SpliteStr(str,','); 
          
            foreach (String strStr in listSplit)
            {
                if (strStr.Length > 0)
                {
                    UserInfo user = new UserInfo();
                    user.eType = (EUserType)type;
                    user.obj = strStr;
                    userList.Add(user);
                } 
            }
        }

        static public void CheckUserSql(MailInfoCfg cfg )
        { 
            foreach (UserInfo user in userList)
            {
                String strSql = "select id from account_user where ";

                try
                {
                    switch (user.eType)
                    {
                        case EUserType.eUser_Id:
                            {
                                strSql += "id=" + Convert.ToInt64(user.obj) + ";";
                            } break;
                        case EUserType.eUser_Name:
                            {
                                strSql += "user_name='" + Convert.ToString(user.obj) + "';"; ;
                            } break;
                        case EUserType.eUser_Account:
                            {
                                strSql += "cdkey='" + Convert.ToString(user.obj) + "';";
                            } break;
                        default: break;
                    }
                }
                catch (System.Exception ex)
                {
                    errorStr.Add(user.obj.ToString());
                    continue;
                } 

                UInt64 objIdx = DbFunc.CheckAndGetUserIdx(strSql );

                if (objIdx > 0)
                {
                    if (!userIdList.Contains(objIdx))
                    {
                        userIdList.Add(objIdx);
                        existUserList.Add(user);
                    } 
                }
                else
                {
                    errorStr.Add( user.obj.ToString());
                }
            }
        }
       
       static public bool GetUseridAndQuery(int type, String str, MailInfoCfg cfg)
        {
            ReleaseList();

            bool bFlag = DbFunc.IfConnDb(cfg);
            if(bFlag)
            {
                SplitUserStr(type, str);

                CheckUserSql(cfg );
            }
           return bFlag;
        }
        
        static void ReleaseList()
       {
           errorStr.Clear();

           existUserList.Clear();
           userList.Clear();
           userIdList.Clear();
       }

        //获取当前选择的cs-->db
        static public MailInfoCfg GetCurtCSDBInfo(String choseCS)
        {
            MailInfoCfg tcfg = new MailInfoCfg();
           
            foreach (MailInfoCfg cfg in Program.dbSfgList)
            { 
                    tcfg = cfg;
                    break; 
            }
            return tcfg;
        }


        //设置全选
        static public int GetChoseSdkIdx(string name)
        {
            foreach (Channel sdk in Program.channelList)
            {
                if (sdk.mName == name)
                {
                    return sdk.mIndex;
                }
            }
            return 0;
        }
        //获取当前选的邮件类型
        static public int GetCurtMailTypeIdx(string choseType)
        { 
            foreach (MailTypeCfg cfg in Program.mailTypeList)
            {
                if (cfg.desc == choseType)
                {
                    return cfg.index;
                }
            }
            return 1;
        }
    }
}
