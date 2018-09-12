using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RemoteConsole
{
    class FormFunc
    {
      static  public bool SendNewMailToCS(CenterServer curtCS, bool ifAllCS, bool buserAll, List<UInt64> userIdList, MailInfo mail)
        {
            int totalSer = 0;
            if (ifAllCS)
            {
                foreach (CenterServer cs in Program.serverList)
                {
                 bool bFlag=  cs.onMsgAddNewMailToCS(buserAll, MailFunc.userIdList, mail);
                  if (bFlag)
                    {
                        totalSer++;
                    }
                }
            }
            else if (null != curtCS)
            {
                bool bFlag = curtCS.onMsgAddNewMailToCS(buserAll, MailFunc.userIdList, mail);
               if (bFlag)
               {
                   totalSer++;
               }
            }
            return totalSer > 0 ? true : false;
        }  
    }
}
