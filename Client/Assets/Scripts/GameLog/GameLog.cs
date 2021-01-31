using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace BlGame
{
    public class GameLog : Singleton<GameLog>
    {
        //记录点击ui次数
        public Dictionary<UIEventType, int> uiEventList = new Dictionary<UIEventType, int>();
        //记录是否UI点击次数有改变 
        private bool bFlag = false;
        //记录的UI点击类型
        public enum UIEventType
        {
            UIEventType_None = 0,
            UIEventType_PersonalInfo = 2,//个人资料按钮
            UIEventType_Chat = 3,      //聊天
            UIEventType_Mail = 4,       //邮件
            UIEventType_System = 5,     //系统设置
            UIEventType_HomePage = 6,       //主页
            UIEventType_Battle = 7,     //战斗按钮
            UIEventType_Market = 8,     //商城
            UIEventType_Friend = 9,     //社交
            UIEventType_Activity = 10,  //精彩活动
            UIEventType_Daily = 11,     //每日签到
            UIEventType_HeroInfo = 12,  //英雄资料
            UIEventType_Rune = 13,     //符文
            UIEventType_FastMarch = 14, //快速匹配
            UIEventType_FastAIMarch = 15,//快速人机匹配
            UIEventType_WeekCard = 16,  //超值周卡
            UIEventType_Robe = 17,      //豪华礼包
            UIEventType_Charge = 18,    //游戏充值
        };
        //添加ui点击事件
        public void AddUIEvent(UIEventType eKey)
        {
            int uiEventValue = 0;
            if (uiEventList.TryGetValue(eKey, out uiEventValue))
            {
                uiEventList.Remove(eKey);
            }
            uiEventList.Add(eKey, uiEventValue + 1);

            bFlag = true;
        }
        //触发获取用户点击事件
        public Dictionary<UIEventType, int> GetCurtUIEvent()
        {
            if (bFlag)
            {
                bFlag = false;
                return uiEventList;
            }
            return null;
        }
    }

}