using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BlGame.GameEntity
{ 
    public enum EMailCurtState
    {
        eMailState_None = 0,
        eMailState_New = 1,
        eMailState_LookedButNotGotGift = 2,
        eMailState_Look = 3,
        eMailState_Del = 4,
    }

    public enum MailGiftType
    {
        eMerchType_None = 0,
        eMerchType_Gold = 1,
        eMerchType_Diamond = 2, 
        eMerchType_Runne = 3,
        eMerchType_Hero = 4,
        eMerchType_Skin = 5,
        eMerchType_Goods = 6,
        eMerchType_Exp = 7,
    }

    public class MailData
    {
        public int MailId;
        public string mailTitle;
        public string mailContent;
        public string mailGift;
        public string mailSender;
        public string mailCreateTime;
        public int mailType;
        public EMailCurtState mailCurtState;
    }

   
}
