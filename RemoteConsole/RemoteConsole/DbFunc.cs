using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.SqlClient;
using MySql.Data.MySqlClient;


namespace RemoteConsole
{
    public class DbFunc
    {
        static public MySqlConnection conn = null;

        static public string err;

        public bool ifConnectDb(MailInfoCfg cfg)
        {
            return true;
        }

        static public bool IfConnDb(MailInfoCfg cfg)
        {
            if (conn != null )
            { 
                return true;
            }
            try
            {
                String mysqlStr = "Database=" + cfg.mDbname + ";Data Source=" + cfg.mIpAddr + ";User Id=" + cfg.mUser + ";Password=" + cfg.mPwd + ";pooling=false;CharSet=utf8;port=3306";
                conn = new MySqlConnection(mysqlStr);
                if (conn != null)
                { 
                    return true;
                }
                return false;
            }
            catch (System.Exception ex)
            {
                err = ex.Message ;
                return false;	
            }
            return false;	
        }
        static private MySqlCommand GetSqlCommand(String sql)
        {
            try
            { 
                return new MySqlCommand(sql, conn);
            }
            catch (System.Exception ex)
            {
                string err = ex.Message;
                return null;
            }
            return null;
        }

        //查看用户正确性
        static public UInt64 CheckAndGetUserIdx(String sql)
        {
           // MySqlCommand cmd = GetSqlCommand(sql); 
            UInt64 objIdx = 0;
            try
            {
                MySqlCommand cmd = new MySqlCommand(sql, conn);
                conn.Open();

                MySqlDataReader reader = cmd.ExecuteReader();
                while (reader.Read())
                {
                    if (reader.HasRows)
                    { 
                        objIdx = reader.GetUInt64(0); 
                    }
                }
                conn.Close();
                reader.Close();
            }
            catch (System.Exception ex)
            {
                String err = ex.Message; 
                return 0;
            }
            return objIdx;
        }
        static private bool InsertNewMail(String sql)
        {
            MySqlCommand cmd = GetSqlCommand(sql);
            if (cmd == null)
            {
                return false;
            }
            try
            {
                cmd.ExecuteNonQuery();
            }
            catch (System.Exception ex)
            { 
                return false;
            }
            return true;
        }
       
        static public void CloseConn()
        {
            if (conn != null)
            {
                conn.Close();
                conn = null;
            }
        }


        //static public bool AddNewMail(MailInfoCfg cfg, int mailType, bool userAll, int sdkIdx, String senderText, String userStr, String title,
        //    String content, String startTime, String endTime, String selStr)
        //{
        //    bool bFlag = IfConnDb(cfg);
        //    if (!bFlag)
        //    {
        //        return false;
        //    }
        //    String strSql = "";
        //    if (userAll)
        //    {
        //        strSql = "insert into game_mail(mail_platform,mail_type,mail_title,mail_content,";//mail_gift,";
        //        if (selStr.Length > 0)
        //        {
        //            strSql += "mail_gift,";
        //        }
        //        if (senderText.Length > 0)
        //        {
        //            strSql += "mail_send,";
        //        }

        //        strSql += "mail_create_time,mail_over_time)values(";

        //        strSql += sdkIdx + ",";
        //        strSql += mailType + ",'";
        //        strSql += title + "','";
        //        strSql += content + "','";
        //        if (selStr.Length > 0)
        //        {
        //            strSql += selStr + "','";
        //        }
        //        if (senderText.Length > 0)
        //        {
        //            strSql += senderText + "','";
        //        }
        //        strSql += startTime + "','";
        //        strSql += endTime + "');";
        //    }
        //    else
        //    {
        //        SplitUserStr(userStr);
        //        strSql = "insert into game_mail(mail_platform,mail_type,mail_user_type,mail_user_info,mail_title,mail_content,mail_gift,";
        //        if (senderText.Length > 0)
        //        {
        //            strSql += "mail_send,";
        //        }
        //        strSql += "mail_create_time,mail_over_time)values";

        //        int total = userList.Count;
        //        int npos = 0;
        //        foreach (UserInfo user in userList)
        //        {
        //            strSql += "(";
        //            strSql += sdkIdx + ",";
        //            strSql += mailType + ",";
        //            strSql += (int)user.eType + ",'";
        //            strSql += user.obj.ToString() + "','";
        //            strSql += title + "','";
        //            strSql += content + "','";
        //            strSql += selStr + "','";
        //            if (senderText.Length > 0)
        //            {
        //                strSql += senderText + "','";
        //            }
        //            strSql += startTime + "','";
        //            strSql += endTime + "')";
        //            npos++;
        //            if (npos < total)
        //            {
        //                strSql += ",";
        //            }
        //        }
        //    }
        //    return InsertNewMail(strSql);
        //}
    }
}
