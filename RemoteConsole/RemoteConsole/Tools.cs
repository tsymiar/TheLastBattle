using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace RemoteConsole
{
   public class Tools
    {
       static public List<String> SpliteStr(String sStr, char sign)
       {
           if (sStr.Length < 1)
           {
               return null;
           }
           List<String> listStr = new List<String>();
           String[] strStr = sStr.Split(new char[] { sign });
           for (int i = 0; i < strStr.Length; i++)
           {
               listStr.Add(strStr[i]);
           }
           return listStr;
       }

       static public bool CreateLogFile(String str)
       {
           if (!Directory.Exists("log"))
           {
               Directory.CreateDirectory("log");
           }
           string fileLog = "log/err_user_" + DateTime.Now.ToString("yyyyMMddHHmmss") + ".txt"; 
           if (!File.Exists(fileLog))
           {
               FileStream myfs = new FileStream(fileLog, FileMode.Create);
               StreamWriter mysw = new StreamWriter(myfs);
               mysw.Write(str);
               mysw.Close();
               myfs.Close();
           }
           System.Diagnostics.Process.Start("notepad.exe", fileLog);
           return true;
       }
      
    }
}
