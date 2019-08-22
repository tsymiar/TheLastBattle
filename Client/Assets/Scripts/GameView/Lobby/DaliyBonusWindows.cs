using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using BlGame.Ctrl;
using BlGame.GameEntity;
using BlGame.Model;
using BlGame.Resource;
using System;
using BlGame.Network;

namespace BlGame.View
{

    public class DaliyBonusWindow : BaseWindow
    {
        public List<GameObject> DaliyGameObjs = new List<GameObject>();
        public GameObject LinkGameObjs;
        public UIButton mCloseBtn;
        public UIGrid mGrid;//签到右侧列表
        public UIGrid mDailyTaskGrid;//每日任务列表
        public UIGrid mInfiniteTaskGrid;//永久任务列表
        public Transform mNewDailyLabel;
        public UIToggle mDailyUIToggle;

        public uint mMonthInWindow = 0;
        public uint mHadDayInWindow = 0;
        public bool mTodayCanInWindow = false;

        public DaliyBonusWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadDailyBonusUI;
            mResident = false;
        }

        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_DailyBonusEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_DailyBonusExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_DailyBonusEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_DailyBonusExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mCloseBtn = mRoot.FindChild("CloseBtn").GetComponent<UIButton>();
            mGrid = mRoot.FindChild("Interface/SignInterface/ScrollView/Grid").GetComponent<UIGrid>();
            mDailyTaskGrid = mRoot.FindChild("Interface/DailyTask/Grid").GetComponent<UIGrid>();
            mInfiniteTaskGrid = mRoot.FindChild("Interface/AchieveInterface/Grid").GetComponent<UIGrid>();
            mNewDailyLabel = mRoot.FindChild("CheckButton/TaskBtn/Tips");
            mDailyUIToggle = mRoot.FindChild("CheckButton/TaskBtn").GetComponent<UIToggle>();
            EventDelegate.Add(mCloseBtn.onClick, OnCloseBtnClick);
            EventDelegate.Add(mDailyUIToggle.onChange, OnDailyUIToggleChange);
            OnEvent_DailyBonusUpdate();
            OnEvent_DailyTaskUpdate();
            OnEvent_InfiniteTaskUpdate();
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
            DaliyGameObjs.Clear();
            mMonthInWindow = 0;
            mHadDayInWindow = 0;
            mTodayCanInWindow = false;
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_DailyBonusUpdate, OnEvent_DailyBonusUpdate);
            EventCenter.AddListener(EGameEvent.eGameEvent_NotifyAllTaskUpdate, OnEvent_DailyTaskUpdate);
            EventCenter.AddListener(EGameEvent.eGameEvent_NotifyAllTaskUpdate, OnEvent_InfiniteTaskUpdate);
            EventCenter.AddListener(EGameEvent.eGameEvent_NotifyDailyTaskUpdate, OnEvent_DailyTaskUpdate);
            EventCenter.AddListener<CTask>(EGameEvent.eGameEvent_NotifyOneTaskAdd, OnEvent_OneTaskAdd);
            EventCenter.AddListener<CTask>(EGameEvent.eGameEvent_NotifyOneTaskUpdate, OnEvent_OneTaskUpdate);
            EventCenter.AddListener<CTask>(EGameEvent.eGameEvent_NotifyOneTaskDel, OnEvent_OneTaskDel);
            EventCenter.AddListener<CTask>(EGameEvent.eGameEvent_NotifyOneTaskRewards, OnEvent_OneTaskRewards);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_DailyBonusUpdate, OnEvent_DailyBonusUpdate);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_NotifyAllTaskUpdate, OnEvent_DailyTaskUpdate);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_NotifyAllTaskUpdate, OnEvent_InfiniteTaskUpdate);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_NotifyDailyTaskUpdate, OnEvent_DailyTaskUpdate);
            EventCenter.RemoveListener<CTask>(EGameEvent.eGameEvent_NotifyOneTaskAdd, OnEvent_OneTaskAdd);
            EventCenter.RemoveListener<CTask>(EGameEvent.eGameEvent_NotifyOneTaskUpdate, OnEvent_OneTaskUpdate);
            EventCenter.RemoveListener<CTask>(EGameEvent.eGameEvent_NotifyOneTaskDel, OnEvent_OneTaskDel);
            EventCenter.RemoveListener<CTask>(EGameEvent.eGameEvent_NotifyOneTaskRewards, OnEvent_OneTaskRewards);
        }

        //显示
        public override void OnEnable()
        {

        }

        //隐藏
        public override void OnDisable()
        {

        }

        //////////////////////////////////////////////////////////////////////////

        private void OnCloseBtnClick()
        {
            DailyBonusCtrl.Instance.Exit();
        }

        private void OnDailyUIToggleChange()
        {
            if (mDailyUIToggle.value == true)
            {
                mNewDailyLabel.gameObject.SetActive(false);
                DailyBonusCtrl.Instance.mIsHadNewDailyTask = false;
                DailyBonusCtrl.Instance.mIsHadTaskFinished = false;
            }
        }

        //领取奖励
        private void OnDaliyGameObjsClick(GameObject obj)
        {
            Debug.Log("DaliyGameObjs" + obj.name + "clicked");
            DailyBonusCtrl.Instance.AskGetAwards();
            DailyBonusConfigInfo bonus = ConfigReader.GetDailyBonusInfo(DailyBonusCtrl.Instance.mHadDay+1);// 设置奖励坑位（1.金币 2.钱 3.物品）
            if (bonus == null) return;// 没有奖励不显示
            EventCenter.Broadcast(EGameEvent.eGameEvent_PurchaseSuccessWindowEnter);
            int pos = -1;
            foreach (string itemStr in bonus.n32ItemID)
            {
                ++pos;
                int count = Convert.ToInt32(bonus.n32ItemNum[pos]);
                UInt32 itemID = Convert.ToUInt32(itemStr);//根据id获取图片id，显示图片和数量和名称
                if (itemID == 1)
                {
                    EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin, 
                        EPurchaseType.EPT_Gold, "金币", "", count);
                }
                else if (itemID == 2)
                {
                    EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin, 
                        EPurchaseType.EPT_Crystal, "钻石", "", count);
                }
                else if (itemID > 100000 && itemID < 110000)
                {
                    EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin,
                        EPurchaseType.EPT_Hero, ConfigReader.HeroBuyXmlInfoDict[(int)itemID].Name, ConfigReader.HeroBuyXmlInfoDict[(int)itemID].DefaultIcon, count);
                }
                else if (itemID > 110000 && itemID < 120000)
                {
                }
                else if (itemID > 120000 && itemID < 130000)
                {
                    EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin, 
                        EPurchaseType.EPT_Rune, "符文AA", "", count);
                }
                else if (itemID > 130000 && itemID < 140000)
                {
                    EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin,
                        EPurchaseType.EPT_TrialCard, ConfigReader.OtherItemXmlInfoDic[itemID].sName, "", count);
                }
            }
            
        }

        private void OnEvent_DailyBonusUpdate()
        {
            ResourceUnit GetMoneyUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadDailyHeroIcon, ResourceType.PREFAB);
            UIAtlas uia_hero = (GetMoneyUnit.Asset as GameObject).GetComponent<UIAtlas>();

            mMonthInWindow = DailyBonusCtrl.Instance.mMonth;
            mHadDayInWindow = DailyBonusCtrl.Instance.mHadDay;
            mTodayCanInWindow = DailyBonusCtrl.Instance.mTodayCan;
            if (LinkGameObjs!=null)
            {
                UIEventListener.Get(LinkGameObjs).onClick -= OnDaliyGameObjsClick;
                LinkGameObjs = null;
            }
            LoadUiResource.ClearAllChild(mGrid.transform);
            DaliyGameObjs.Clear();
            for (uint i = 0; i < DailyBonusCtrl.Instance.mMaxDay; ++i)
            {
                DailyBonusConfigInfo bonus = ConfigReader.GetDailyBonusInfo(i + 1);// 设置奖励坑位（1.金币 2.钱 3.物品）
                if (bonus == null) continue;// 没有奖励不显示
                GameObject addobj = LoadUiResource.AddChildObject(mGrid.transform, GameConstDefine.LoadDailyPrizeUI);
                addobj.transform.parent = mGrid.transform;
                addobj.transform.localScale = Vector3.one;
                addobj.name = (DaliyGameObjs.Count + 1).ToString();
                addobj.transform.FindChild("BG/Label").GetComponent<UILabel>().text = "累计签到" + (i + 1).ToString() + "天";
                DaliyGameObjs.Add(addobj);
                string hookname = "Interface/SignInterface/Calendar/Date" + (i + 1).ToString() + "/Hook";
                //////////////////////////////////////////////////////////////////////////
                if (i < DailyBonusCtrl.Instance.mHadDay)
                {//已领取
                    mRoot.FindChild(hookname).GetComponent<UISprite>().gameObject.SetActive(true);
                    addobj.transform.FindChild("Done").GetComponent<UISprite>().gameObject.SetActive(true);
                    addobj.transform.FindChild("NotDone").GetComponent<UISprite>().gameObject.SetActive(false);
                    addobj.transform.FindChild("UnDone").GetComponent<UISprite>().gameObject.SetActive(false);
                }
                else if (i == DailyBonusCtrl.Instance.mHadDay)
                {
                    if (DailyBonusCtrl.Instance.mTodayCan)
                    {//可领取
                        mRoot.FindChild(hookname).GetComponent<UISprite>().gameObject.SetActive(false);
                        addobj.transform.FindChild("Done").GetComponent<UISprite>().gameObject.SetActive(false);
                        addobj.transform.FindChild("NotDone").GetComponent<UISprite>().gameObject.SetActive(true);
                        addobj.transform.FindChild("UnDone").GetComponent<UISprite>().gameObject.SetActive(false);
                        //LinkGameObjs = addobj.transform.FindChild("NotDone").gameObject;
                        LinkGameObjs = addobj;
                        UIEventListener.Get(LinkGameObjs).onClick += OnDaliyGameObjsClick;
                    }
                    else
                    {//无法领取
                        mRoot.FindChild(hookname).GetComponent<UISprite>().gameObject.SetActive(false);
                        addobj.transform.FindChild("Done").GetComponent<UISprite>().gameObject.SetActive(false);
                        addobj.transform.FindChild("NotDone").GetComponent<UISprite>().gameObject.SetActive(false);
                        addobj.transform.FindChild("UnDone").GetComponent<UISprite>().gameObject.SetActive(true);
                    }
                }
                else
                {//无法领取
                    mRoot.FindChild(hookname).GetComponent<UISprite>().gameObject.SetActive(false);
                    addobj.transform.FindChild("Done").GetComponent<UISprite>().gameObject.SetActive(false);
                    addobj.transform.FindChild("NotDone").GetComponent<UISprite>().gameObject.SetActive(false);
                    addobj.transform.FindChild("UnDone").GetComponent<UISprite>().gameObject.SetActive(true);
                }
                //////////////////////////////////////////////////////////////////////////
                int pos = -1;
                foreach (string itemStr in bonus.n32ItemID)
                {
                    ++pos;
                    UIGrid tmpGrid = addobj.transform.FindChild("Grid").GetComponent<UIGrid>();
                    GameObject oneBouns = LoadUiResource.AddChildObject(tmpGrid.transform, GameConstDefine.LoadDailyOneBonusUI);
                    oneBouns.transform.FindChild("Num").GetComponent<UILabel>().text = "x" + bonus.n32ItemNum[pos];// 数量
                    bool isHadExtend = bonus.n32Type[pos] == "2";//是否有额外奖励
                    if (isHadExtend)
                    {
                        string starname = "Interface/SignInterface/Calendar/Date" + (i + 1).ToString() + "/Star";
                        mRoot.FindChild(starname).gameObject.SetActive(true);
                        oneBouns.transform.FindChild("Tip").gameObject.SetActive(true);
                    }
                    UInt32 itemID = Convert.ToUInt32(itemStr);//根据id获取图片id，显示图片和数量和名称
                    if (itemID == 1)
                    {
                        oneBouns.transform.GetComponent<UISprite>().spriteName = (9).ToString();
                        oneBouns.transform.FindChild("Name").GetComponent<UILabel>().text = "金币";
                    }
                    else if (itemID == 2)
                    {
                        oneBouns.transform.GetComponent<UISprite>().spriteName = (10).ToString();
                        oneBouns.transform.FindChild("Name").GetComponent<UILabel>().text = "钻石";
                    }
                    else if (itemID > 100000 && itemID < 110000)
                    {
                        oneBouns.transform.GetComponent<UISprite>().atlas = uia_hero;
                        oneBouns.transform.FindChild("Name").GetComponent<UILabel>().text = ConfigReader.HeroBuyXmlInfoDict[(int)itemID].Name;
                        oneBouns.transform.GetComponent<UISprite>().spriteName = ConfigReader.HeroBuyXmlInfoDict[(int)itemID].DefaultIcon;
                    }
                    else if (itemID > 110000 && itemID < 120000)
                    {
                        oneBouns.transform.FindChild("Name").GetComponent<UILabel>().text = "皮肤";
                        oneBouns.transform.GetComponent<UISprite>().spriteName = ConfigReader.HeroSkinXmlInfoDict[(int)itemID].Icon;
                    }
                    else if (itemID > 120000 && itemID < 130000)
                    {
                        oneBouns.transform.FindChild("Name").GetComponent<UILabel>().text = "符文";
                        oneBouns.transform.GetComponent<UISprite>().spriteName = ConfigReader.RuneXmlInfoDict[itemID].Icon;
                    }
                    else if (itemID > 130000 && itemID < 140000)
                    {
                        oneBouns.transform.FindChild("Name").GetComponent<UILabel>().text = ConfigReader.OtherItemXmlInfoDic[itemID].sName;
                        oneBouns.transform.GetComponent<UISprite>().spriteName = ConfigReader.OtherItemXmlInfoDic[itemID].icon;
                    }
                    else
                    {
                        oneBouns.transform.FindChild("Name").GetComponent<UILabel>().text = "????";
                        oneBouns.transform.GetComponent<UISprite>().spriteName = (115).ToString();
                        Debug.LogError("unknow itemid in daliybouns " + itemStr);
                    }
                }
            }
            for (uint i = DailyBonusCtrl.Instance.mMaxDay; i < 31; ++i)
            {
                string lightname = "Interface/SignInterface/Calendar/Date" + (i + 1).ToString() + "/Light";
                mRoot.FindChild(lightname).gameObject.SetActive(false);
            }
            mGrid.Reposition();
        }

        private void OnEvent_DailyTaskUpdate()
        {
            if (DailyBonusCtrl.Instance.mIsHadNewDailyTask || DailyBonusCtrl.Instance.mIsHadTaskFinished)
            {
                mNewDailyLabel.gameObject.SetActive(true);
            }
            // 删除每日任务UI数据
            LoadUiResource.ClearAllChild(mDailyTaskGrid.transform);
            // 更新每日任务Logic数据到UI数据
            foreach (KeyValuePair<uint,CTask> one in DailyBonusCtrl.Instance.mDailyTaskDic)
            {
                GameObject addobj = LoadUiResource.AddChildObject(mDailyTaskGrid.transform, GameConstDefine.LoadDailyTaskUI);
                addobj.name = Convert.ToString(one.Value.mGuid);
                addobj.transform.FindChild("TaskName").GetComponent<UILabel>().text = one.Value.mConfig.taskDesc;
                UpdateTaskToUI(addobj, one.Value);
            }
            mDailyTaskGrid.Reposition();
        }

        private void OnEvent_InfiniteTaskUpdate()
        {
            // 删除永久任务UI数据
            LoadUiResource.ClearAllChild(mInfiniteTaskGrid.transform);
            // 更新永久任务Logic数据到UI数据
            foreach (KeyValuePair<uint, CTask> one in DailyBonusCtrl.Instance.mInfiniteTaskDic)
            {
                GameObject addobj = LoadUiResource.AddChildObject(mInfiniteTaskGrid.transform, GameConstDefine.LoadInfiniteTaskUI);
                addobj.name = Convert.ToString(one.Value.mGuid);
                addobj.transform.FindChild("TaskName").GetComponent<UILabel>().text = one.Value.mConfig.taskDesc;
                UpdateTaskToUI(addobj,one.Value);
            }
            mInfiniteTaskGrid.Reposition();
        }

        private void OnEvent_OneTaskAdd(CTask task)
        {
            // 增加某个任务
            switch(task.mConfig.taskTimeType)
            {
                case TASK_TIME_TYPE.TTT_Daily:
                    {
                        GameObject addobj = LoadUiResource.AddChildObject(mDailyTaskGrid.transform, GameConstDefine.LoadDailyTaskUI);
                        addobj.name = Convert.ToString(task.mGuid);
                        addobj.transform.FindChild("TaskName").GetComponent<UILabel>().text = task.mConfig.taskDesc;
                        UpdateTaskToUI(addobj, task);
                        mDailyTaskGrid.Reposition();
                    }
                    break;
                case TASK_TIME_TYPE.TTT_Infinite:
                    {
                        GameObject addobj = LoadUiResource.AddChildObject(mInfiniteTaskGrid.transform, GameConstDefine.LoadInfiniteTaskUI);
                        addobj.name = Convert.ToString(task.mGuid);
                        addobj.transform.FindChild("TaskName").GetComponent<UILabel>().text = task.mConfig.taskDesc;
                        UpdateTaskToUI(addobj, task);
                        mInfiniteTaskGrid.Reposition();
                    }
                    break;
            }
        }

        private void OnEvent_OneTaskUpdate(CTask task)
        {
            // 更新某个任务
            Transform child = GetTaskUiItem(task.mGuid);
            if (child != null)
            {
                if (DailyBonusCtrl.Instance.mIsHadTaskFinished)
                {
                    mNewDailyLabel.gameObject.SetActive(true);
                }
                UpdateTaskToUI(child.gameObject, task);
            }
        }

        private void OnEvent_OneTaskDel(CTask task)
        {
            // 删除某个任务
            Transform child = GetTaskUiItem(task.mGuid);
            if (child != null)
            {
                switch (task.mConfig.taskTimeType)
                {
                    case TASK_TIME_TYPE.TTT_Daily:
                        {
                            LoadUiResource.ClearOneChild(mDailyTaskGrid.transform,child.name);
                            mDailyTaskGrid.Reposition();
                        }
                        break;
                    case TASK_TIME_TYPE.TTT_Infinite:
                        {
                            LoadUiResource.ClearOneChild(mInfiniteTaskGrid.transform, child.name);
                            mInfiniteTaskGrid.Reposition();
                        }
                        break;
                }
            }
        }

        private void OnEvent_OneTaskRewards(CTask task)
        {
            // 奖励某个任务
            Transform child = GetTaskUiItem(task.mGuid);
            if (child != null)
            {
                EventCenter.Broadcast(EGameEvent.eGameEvent_PurchaseSuccessWindowEnter);
                int pos = -1;
                foreach (string itemStr in task.mConfig.n32ItemID)
                {
                    ++pos;
                    int count = Convert.ToInt32(task.mConfig.n32ItemNum[pos]);
                    UInt32 itemID = Convert.ToUInt32(itemStr);//根据id获取图片id，显示图片和数量和名称
                    if (itemID == 1)
                    {
                        EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin,
                            EPurchaseType.EPT_Gold, "金币", "", count);
                    }
                    else if (itemID == 2)
                    {
                        EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin,
                            EPurchaseType.EPT_Crystal, "钻石", "", count);
                    }
                    else if (itemID > 100000 && itemID < 110000)
                    {
                        EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin,
                            EPurchaseType.EPT_Hero, ConfigReader.HeroBuyXmlInfoDict[(int)itemID].Name, ConfigReader.HeroBuyXmlInfoDict[(int)itemID].DefaultIcon, count);
                    }
                    else if (itemID > 110000 && itemID < 120000)
                    {
                    }
                    else if (itemID > 120000 && itemID < 130000)
                    {
                        EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin,
                            EPurchaseType.EPT_Rune, "符文AA", "", count);
                    }
                    else if (itemID > 130000 && itemID < 140000)
                    {
                        EventCenter.Broadcast<EPurchaseType, string, string, int>(EGameEvent.eGameEvent_PurchaseRuneSuccessWin,
                            EPurchaseType.EPT_TrialCard, ConfigReader.OtherItemXmlInfoDic[itemID].sName, "", count);
                    }
                }
            }
        }

        private Transform GetTaskUiItem(uint taskGuid)
        {
            string taskGuidStr = Convert.ToString(taskGuid);
            List<Transform> childList = null;
            childList = mDailyTaskGrid.GetChildList();
            foreach (Transform one in childList)
            {
                if (one.name == taskGuidStr)
                {
                    return one;
                }
            }
            childList = mInfiniteTaskGrid.GetChildList();
            foreach (Transform one in childList)
            {
                if (one.name == taskGuidStr)
                {
                    return one;
                }
            }
            return null;
        }

        private void OnTaskFinishBtnClick(GameObject obj)
        {
            Debug.Log("DaliyGameObjs" + obj.name + "clicked");
            GCToCS.AskOneTaskRewards pMsg = new GCToCS.AskOneTaskRewards()
            {
                task_guid = Convert.ToUInt32(obj.name)
            };
            NetworkManager.Instance.SendMsg(pMsg, (int)pMsg.msgnum);
        }

        private void UpdateTaskToUI(GameObject uiTask, CTask task)
        {
            uint curCount = task.mCurCount;
            if (task.mCurCount == UInt32.MaxValue)
            {//已领取
                uiTask.transform.FindChild("Complete").gameObject.SetActive(false);
                uiTask.transform.FindChild("Done").gameObject.SetActive(true);
                UIEventListener.Get(uiTask.transform.gameObject).onClick = null;
                curCount = task.mConfig.taskMaxCount;
            }
            else if (task.mCurCount == task.mConfig.taskMaxCount)
            {//已完成
                uiTask.transform.FindChild("Complete").gameObject.SetActive(true);
                uiTask.transform.FindChild("Done").gameObject.SetActive(false);
                UIEventListener.Get(uiTask.transform.gameObject).onClick += OnTaskFinishBtnClick;
            }
            uiTask.transform.FindChild("ProgressLabel").GetComponent<UILabel>().text = Convert.ToString(curCount) + "/" + Convert.ToString(task.mConfig.taskMaxCount);
        }
    }
}


