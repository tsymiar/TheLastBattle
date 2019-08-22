using UnityEngine;
using System.Collections;
using GameDefine;
using System.Collections.Generic;
using System.Linq;

public class SystemNoticeData {

    private static SystemNoticeData instance = null;
    public static SystemNoticeData Instance
    {
        get {
            if (instance == null)
            {
                instance = new SystemNoticeData();
            }
            return instance;
        }
    }
    string value = "[d2853e]最后一战精英测试公告[-]\n[ffe9db]亲爱的小伙伴们:\n 最后一战精英测试已经隆重拉开帷幕，我们真切地邀请各位小伙伴们参与该次测试并把你们对最后一战的期望和建议反馈给我们。我们也准备了大量精美的礼品送给积极反馈的你们。祝各位游戏愉快!\n[58e901]测试时间：2014年10月18日-10月25日[-]反馈请发送至邮箱：bl@skymoon.com\n[00fffc]最后一战运营团队[-]";
    internal void SetSystem()
    {
        //for (int i = 0; i < 10; i++)
        //{
        //    SetSystemNotList("title_" + i, (NoticeIdentify)Mathf.Abs(i % 3), (NoticeState)Mathf.Abs(i / 2), i,i + value);
        //}
    }
    public void Clear()
    {
        if (systemNotList.Count != 0)
        {
            systemNotList.Clear();
        }
    }
    public List<SystemNoticeType> systemNotList = new List<SystemNoticeType>();
    public void SetSystemNotList(string title, NoticeIdentify identify, NoticeState state, int sort, string content)
    {
        SystemNoticeType notice = new SystemNoticeType();
        notice.SetSystemNotice(title, identify, state, sort, content);
        SystemNoticeType temp = null;
        foreach (var item in systemNotList)
        {
            if (item.mNoticeSort == sort)
            {
                item.SetNoticChildSort(item.mNoticChildSort);
            }
        }
        for (int i = 0; i < systemNotList.Count; i++)
        {
            if (systemNotList[i].mNoticeSort <= notice.mNoticeSort && 
                systemNotList[i].mNoticChildSort > notice.mNoticChildSort)//当新公告优先级高于
            {
                temp = systemNotList[i];
                systemNotList[i] = notice;
                notice = temp;
            }
        }
        systemNotList.Add(notice);
    }

    Dictionary<int, SystemNoticeType> systemNotDic = new Dictionary<int, SystemNoticeType>();

    public void SetSystemNotDic(string title, NoticeIdentify identify, NoticeState state, int sort, string content)
    {
        SystemNoticeType notice = new SystemNoticeType();
        notice.SetSystemNotice(title, identify, state, sort, content);

        List<KeyValuePair<int, SystemNoticeType>> myList = new List<KeyValuePair<int, SystemNoticeType>>(systemNotDic);

        myList.Sort(delegate(KeyValuePair<int, SystemNoticeType> s1, KeyValuePair<int, SystemNoticeType> s2)
        {
            return s1.Value.mNoticeSort.CompareTo(s2.Value.mNoticeSort);
        });

        systemNotDic.Clear();

        foreach (KeyValuePair<int, SystemNoticeType> pair in myList)
        {
            systemNotDic.Add(pair.Key, pair.Value);
        }
 
        //foreach(string key in dic.Keys)
        // {
        //     Response.Write(dic[key] +"<br />");
        // }
        //foreach(var item in systemNotDic.OrderByDescending)
    }



    public class SystemNoticeType
    {

        public string mNoticeTitle//公告标题
        {
            private set;
            get;
        }
        public NoticeIdentify mNoticeIdentify//公告标识
        {
            private set;
            get;
        }

        public NoticeState mNoticeState // 公告状态
        {
            private set;
            get;
        }

        public int mNoticeSort//公告排序
        {
            private set;
            get;
        }

        public int mNoticChildSort
        {
            private set;
            get;
        }
                
        public string mNoticeContent//公告内容
        {
            private set;
            get;
        }

        public void SetNoticChildSort(int childSort)
        {
            mNoticChildSort = (++childSort);
        }

        public void SetSystemNotice(string title, NoticeIdentify identify, NoticeState state, int sort, string content)
        {
            mNoticChildSort = 0;
            mNoticeTitle = title;
            mNoticeIdentify = identify;
            mNoticeState = state;
            mNoticeSort = sort;
            mNoticeContent = content;
        }
    }

    
}
