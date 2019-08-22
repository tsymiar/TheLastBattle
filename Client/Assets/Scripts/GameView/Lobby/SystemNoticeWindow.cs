using UnityEngine;
using System.Collections.Generic;
using System.Collections;
using System;
using GameDefine;
using BlGame.Ctrl;
using BlGame.Resource;
using System.Linq;

namespace BlGame.View
{
    public class SystemNoticeWindow : BaseWindow
    {
        public SystemNoticeWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadSystemNoticeUI;
            mResident = false;
        }
        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_SystemNoticeEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_SystemNoticeExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }


        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SystemNoticeEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SystemNoticeExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mContent = mRoot.FindChild("ScrollView/Text/Label").GetComponent<UILabel>();
            mSureBtn = mRoot.FindChild("CtrlBtn").gameObject;
            mGrid = mRoot.FindChild("List/Panel/Grid").GetComponent<UIGrid>();
            //close = mRoot.FindChild("Background/Black").gameObject;
            UIEventListener.Get(mSureBtn).onClick += CloseNotice;
            //UIEventListener.Get(close).onClick += Close;
        }

        private void Close(GameObject go)
        {
            SystemNoticeCtrl.Instance.Exit();
        }

        private void CloseNotice(GameObject go)
        {
            SystemNoticeCtrl.Instance.Exit();
            //System.Random rand = new System.Random();
            //int num = (rand.Next(100) / 20);
            //SystemNoticeData.Instance.SetSystemNotList("title_" + num, (NoticeIdentify)(rand.Next(100) % 3),
            //    (NoticeState)(rand.Next(20) % 2), num, num + sssss[rand.Next(100) % (sssss.Length )]);
        }
        //string[] sssss = { "[1313ff] 公告后台参考邮件后台\n，可采用相同模式。\n1.可选渠道and区服2.时间选定——上架/下架\n3.公告排序 ——升序/降序\n4.具备预览功能——前端显示匹配\n5.文本具备居中/左对齐/右对齐选择", "特点：登入游戏后弹出，左右两侧分栏显示，左侧为公告标题，右侧为公告正文区域，\n两侧皆可    上下滑动。", "左上角为公告属性，包括“活动”，“促销”，“维护”等字眼；", "右下角为公告状态，包括“NEW（新）”，“HOT（火爆）”字眼" };
        void SetAllNotice()
        {
            mLastObj = null;
            if (mNoticeTitle.Count > 0)
            {
                for (int i = 0; i < mNoticeTitle.Count - 1;i++ )
                {
                    UnityEngine.GameObject.DestroyImmediate(mNoticeTitle.ElementAt(i).Key);
                    mNoticeTitle.Remove(mNoticeTitle.ElementAt(i).Key);
                }
                for (int i = 0; i < mNoticeContent.Count - 1; i++)
                {
                    UnityEngine.GameObject.DestroyImmediate(mNoticeContent.ElementAt(i).Key);
                    mNoticeContent.Remove(mNoticeContent.ElementAt(i).Key);
                }
            }
            int index = 100000;
            foreach (var item in SystemNoticeData.Instance.systemNotList)
            {
                ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadNoticeTitleUI, ResourceType.PREFAB);
                GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
                obj.transform.parent = mGrid.transform;

                obj.transform.localPosition = Vector3.zero;
                obj.transform.localScale = Vector3.one;
                obj.name = item.mNoticeSort + "_" + item.mNoticChildSort;
                NoticeTitle not = null;
                if (!mNoticeTitle.TryGetValue(obj, out not))
                {
                    not = new NoticeTitle();
                    not.mHot = obj.transform.FindChild("Status/Hot").gameObject;
                    not.mNew = obj.transform.FindChild("Status/New").gameObject;

                    not.mActivities = obj.transform.FindChild("Tip/Activities").gameObject;
                    not.mDiscount = obj.transform.FindChild("Tip/Discount").gameObject;
                    not.mNotify = obj.transform.FindChild("Tip/Notify").gameObject;

                    not.mHighlight = obj.transform.FindChild("Highlight").gameObject;
                    not.mNoticeName = obj.transform.FindChild("Name").GetComponent<UILabel>();
                    mNoticeTitle.Add(obj, not);
                    mNoticeContent.Add(obj, item.mNoticeContent);
                }
                not.mHot.SetActive(item.mNoticeState == NoticeState.NoticeHot);

                not.mNew.SetActive(item.mNoticeState == NoticeState.NoticeNew);
                not.mActivities.SetActive(item.mNoticeIdentify == NoticeIdentify.NoticIdentifyActivity);
                not.mDiscount.SetActive(item.mNoticeIdentify == NoticeIdentify.NoticIdentifyDiscount);

                not.mNotify.SetActive(item.mNoticeIdentify == NoticeIdentify.NoticIdentifyNotify);
                if (item.mNoticeTitle.Length > 8)
                {
                    not.mNoticeName.text = item.mNoticeTitle.Substring(0, 8);
                }
                else
                {
                    not.mNoticeName.text = item.mNoticeTitle;
                }
                UIEventListener.Get(obj).onClick += OnPressBtn;

                if (item.mNoticeSort < index && item.mNoticChildSort == 0)
                {
                    if (mLastObj != null) {
                        mLastObj.mHighlight.SetActive(false);
                    }
                    index = item.mNoticeSort;
                    not.mHighlight.gameObject.SetActive(true);
                    not.mObjParent = obj; mLastObj = not;
                    mContent.text = item.mNoticeContent;
                }
            }
            mContent.gameObject.SetActive(SystemNoticeData.Instance.systemNotList.Count != 0);
            mGrid.repositionNow = true;
            mGrid.Reposition();
        }

        private void OnPressBtn(GameObject go)
        {
            string mTemp = null;
            NoticeTitle not = null;
            if (mLastObj == null)
            {
                return;
            }
            else
            {
                if (mLastObj.mObjParent == go)
                    return;
                if (!mLastObj.mObjParent || !go)
                    return;
                mNoticeContent.TryGetValue(go, out mTemp);
                if (mTemp == null || string.IsNullOrEmpty(mTemp))
                    return;
            }
            mLastObj.mHighlight.SetActive(false);
            mNoticeTitle.TryGetValue(go, out not);
            if (not == null)
                return;
            mLastObj = not;
            mLastObj.mObjParent = go;
            mLastObj.mHighlight.SetActive(true);
            mContent.text = mTemp;
        }

        protected override void RealseWidget()
        {

        }


        public override void OnEnable()
        {
            SetAllNotice();
        }
        //隐藏
        public override void OnDisable()
        {

        }

        //游戏事件注册
        protected override void OnAddListener()
        {

        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {

        }


        Dictionary<GameObject, NoticeTitle> mNoticeTitle = new Dictionary<GameObject, NoticeTitle>();
        Dictionary<GameObject, string> mNoticeContent = new Dictionary<GameObject, string>();
        NoticeTitle mLastObj = null;

        UILabel mContent = null;
        GameObject mSureBtn = null;
        UIGrid mGrid = null;
        GameObject close = null;
        class NoticeTitle
        {
            public GameObject mObjParent = null;
            public GameObject mHot = null;
            public GameObject mNew = null;
            public GameObject mActivities = null;
            public GameObject mDiscount = null;
            public GameObject mNotify = null;
            public GameObject mHighlight = null;
            public UILabel mNoticeName = null;
        }
    }
}
