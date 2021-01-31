using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using BlGame.GameEntity;
using JT.FWW.Tools;

//using BlGame

namespace BlGame.Model
{
    public class MailModel : Singleton<MailModel>
    {
        private List<KeyValuePair<int, MailData>> mailList = new List<KeyValuePair<int, MailData>>();

        public int defaultMailid;

        public void Clear()
        {
            mailList.Clear();
        }

        public List<KeyValuePair<int, MailData>> MailList
        {
            get { return mailList; }
        }

        private bool bIfNewMail;

        public bool BIfNewMail
        {
            get { return bIfNewMail; }
            set { bIfNewMail = value; }
        }
        /// <summary>
        /// 添加邮件标题列表
        /// </summary>
        /// <param name="mailType"></param>
        /// <param name="mailId"></param>
        /// <param name="mailTitle"></param>
        /// <param name="mailContent"></param>
        public void AddMailData(int mailType, int mailId, string mailTitle, EMailCurtState curtState)
        {
            MailData mail = new MailData();
            mail.MailId = mailId;
            mail.mailTitle = mailTitle;
            mail.mailType = mailType;
            mail.mailCurtState = curtState;

            KeyValuePair<int, MailData> kv = new KeyValuePair<int, MailData>(mailId, mail);
            mailList.Add(kv);
            bIfNewMail = true;
        }
        ///填充邮件具体内容///
        public void UpdateMailInfo(int mailId, string mailContent, string mailGift, string sender, string createtime)
        {
            MailData mdb = null;
            foreach (KeyValuePair<int, MailData> kv in mailList)
            {
                if (mailId == kv.Key)
                {
                    kv.Value.mailContent = mailContent;
                    kv.Value.mailGift = mailGift;
                    kv.Value.mailSender = sender;
                    kv.Value.mailCreateTime = createtime;
                    kv.Value.mailCurtState = EMailCurtState.eMailState_Look;

                    mdb = kv.Value;
                    break;
                }
            }
            SortMailList();

            EventCenter.Broadcast(EGameEvent.eGameEvent_UpdateMailInfo, mdb);
        }

        ///查看单封邮件，是否删除该邮件 ，并且 从新排序邮件
        public void ResetMailList(int mailId, bool ifDel, bool ifSort)
        {
            foreach (KeyValuePair<int, MailData> kv in mailList)
            {
                if (mailId == kv.Key)
                {
                    if (ifDel)
                    {
                        mailList.Remove(kv);
                    }
                    else
                    {
                        kv.Value.mailCurtState = EMailCurtState.eMailState_Del;
                    }
                    break;
                }
            }

            SortMailList();

            Debug.Log("------222222222222-------------");
            EventCenter.Broadcast(EGameEvent.eGameEvent_UpdateMailList);
            Debug.Log("------222222222222-------------");

            bIfNewMail = false;
        }


        private void StartSortMailList(List<KeyValuePair<int, MailData>> myList)
        {
            myList.Sort(delegate(KeyValuePair<int, MailData> s1, KeyValuePair<int, MailData> s2)
            {
                return s2.Key.CompareTo(s1.Key);
            });
        }
        //排序规则：
        //1 没有查看的在最前面，并且按照mailId从大到小
        //2 查看了的排在后边，并且按照mailId从大到小 
        public void SortMailList()
        {
            List<KeyValuePair<int, MailData>> notViewMailList = new List<KeyValuePair<int, MailData>>();
            List<KeyValuePair<int, MailData>> hasViewMailList = new List<KeyValuePair<int, MailData>>();
            foreach (var item in mailList)
            {
                if (item.Value.mailCurtState == EMailCurtState.eMailState_New)
                {
                    notViewMailList.Add(item);
                }
                else
                {
                    hasViewMailList.Add(item);
                }
            }
            mailList.Clear();

            StartSortMailList(notViewMailList);
            StartSortMailList(hasViewMailList);

            mailList.AddRange(notViewMailList);
            mailList.AddRange(hasViewMailList);
            //设置默认选择的邮件id
            if (notViewMailList.Count > 0)
            {
                defaultMailid = notViewMailList[0].Key;
            }
            else if (hasViewMailList.Count > 0 )
            {
                defaultMailid = hasViewMailList[0].Key;
            }
        }

        public bool CheckMailContentByMailId(int mailId)
        {
            if (mailList == null || mailList.Count < 1)
            {
                return false;
            }
            MailData mdb = null;
            foreach (var item in mailList)
            {
                if (item.Key == mailId)
                {
                    if (null == item.Value.mailContent)
                        return false;
                    mdb = item.Value;
                    break;
                }
            }

            EventCenter.Broadcast(EGameEvent.eGameEvent_UpdateMailInfo, mdb);

            return true;
        }
    }

}