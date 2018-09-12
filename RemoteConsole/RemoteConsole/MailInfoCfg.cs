using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Configuration; 

namespace RemoteConsole
{
    public class MailInfo
    {
        public int msdk;
        public int mtype;
        public string mtitle;
        public string mcontent;
        public string mgiftstr;
        public string mcreatetime;
        public string mendtime;
        public int existday;
        public string msender; 

        public void init(int sdk, int mty, string title, string con, string gift, string ctime,string cendtime, int etime, string sd)
        {
            msdk = sdk;
            mtype = mty;
            mtitle = title;
            mcontent = con;
            mgiftstr = gift;
            mcreatetime = ctime;
            existday = etime;
            msender = sd;
            mendtime = cendtime;
            
        }
    }

    public class Channel
    {
        public String mName;
        public int mIndex;

        public void Set(String name, int index)
        {
            mName = name;
            mIndex = index;
        }
    }


   public class MailInfoCfg
    { 
       public String mIpAddr;
       public int mPort = 3306;
       public String mUser;
       public String mPwd;
       public String mDbname; 
       
        public void InitDbCfg(String ip,   String user, String pwd, String db)
        {
            mIpAddr = ip; 
            mUser = user;
            mPwd = pwd;
            mDbname = db; 
        }  
    }

   public class HandSel
   {
       public String desc;
       public int index;

       public void InitHandSel(String name, int idx)
       {
           desc = name;
           index = idx;
       }
   }

   public class MailTypeCfg
   {
       public String desc;
       public int index;

       public void InitMailType(String name, int idx)
       {
           desc = name;
           index = idx;
       }
    }

   public enum EUserType
   {
       eUser_None = 0,
       eUser_Id = 1,
       eUser_Name = 2,
       eUser_Account = 3,
   };

   public class UserInfo
   {
       public int csid;
       public EUserType eType;
       public String obj; 
   }
   
}
