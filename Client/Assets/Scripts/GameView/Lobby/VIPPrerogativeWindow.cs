using UnityEngine;
using System.Collections;
using GameDefine;
using System;
using BlGame.Ctrl;
using BlGame.Resource;
using System.Collections.Generic;

namespace BlGame.View
{
    public class VIPPrerogativeWindow : BaseWindow
    {
        public VIPPrerogativeWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadVIPPrerogativeUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化

        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_VIPPrerogativeEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_VIPPrerogativeExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_PresonInfoExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_VIPPrerogativeEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_VIPPrerogativeExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_PresonInfoExit, Hide);
        }
           //窗口控件初始化
        protected override void InitWidget()
        {
            mPanel = mRoot.FindChild("ScrollView").GetComponent<UIPanel>();
            mGrid = mRoot.FindChild("ScrollView/Grid").GetComponent<UIGrid>();
            LastLeft = mRoot.FindChild("Arrow/Left").gameObject;
            LastRight = mRoot.FindChild("Arrow/Right").gameObject;
            CloseBtn = mRoot.FindChild("CloseBtn").gameObject;
            Recharge = mRoot.FindChild("ChargeBtn").gameObject;

            UIEventListener.Get(LastLeft).onClick += LeftButton;
            UIEventListener.Get(LastRight).onClick += RightButton;
            UIEventListener.Get(CloseBtn).onClick += CloseButton;
            UIEventListener.Get(Recharge).onClick += RechargeButton;
        }

        private void RechargeButton(GameObject go)
        {

        }

        private void CloseButton(GameObject go)
        {
            VIPPrerogativeCtrl.Instance.Exit();
        }

        private void RightButton(GameObject go)
        {
            Vector2 currVec = mPanel.clipOffset;
            Vector3 currPos = mPanel.transform.localPosition;
            if (Mathf.Abs(currPos.x) >= 2450)
                return;
            mPanel.clipOffset = new Vector2(currVec.x + clipOffset, currVec.y);
            mPanel.transform.localPosition = new Vector3(currPos.x - clipOffset, currPos.y, currPos.z);
            
        }

        private void LeftButton(GameObject go)
        {
            Vector2 currVec = mPanel.clipOffset;
            Vector3 currPos = mPanel.transform.localPosition;
            if (Mathf.Abs(currPos.x) <= 100)
                return;
            mPanel.clipOffset = new Vector2(currVec.x - clipOffset, currVec.y);
            mPanel.transform.localPosition = new Vector3(currPos.x + clipOffset, currPos.y, currPos.z);
        }


        //窗口控件释放
        protected override void RealseWidget()
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

        void ShowVipPre()
        {
            int vipLevel = PresonInfoCtrl.Instance.mVipLevel;
            
            foreach (var item in ConfigReader.ViplevelXmlInfoDict.Values)
            {
                string temp = null;
                string[] value = null;
                MsgConfigInfo info = null;
                HeroBuyConfigInfo heroinfo = null;
                RuneConfigInfo runeinfo = null;
                int runeLevel = 0;
                int j = 0;
                ResourceUnit objHomeBaseUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadVIPInfoUI, ResourceType.PREFAB);
                GameObject obj = GameObject.Instantiate(objHomeBaseUnit.Asset) as GameObject;
                obj.transform.parent = mGrid.transform;
                obj.transform.localPosition = Vector3.zero;
                obj.transform.localScale = Vector3.one;
                obj.name = (j++).ToString();
                value = item.VipHero.Split(':');

                UILabel uil = obj.transform.FindChild("InfoLabel").GetComponent<UILabel>();
                UILabel viplevel = obj.transform.FindChild("NameLabel").GetComponent<UILabel>();
                viplevel.text = "VIP "+item.VipUserLevel.ToString();
                info = ConfigReader.msgXmlInfoDic[20001];
                for (int i = 0; i < value.Length; i++)
                {
                    if (i % 2 == 0)
                    {
                        //英雄ID  20001
                         int index = Convert.ToInt32(value[i]);
                        if (index == 0)
                            break;
                        heroinfo = ConfigReader.GetHeroBuyInfo(Convert.ToInt32(value[i]));
                        if (heroinfo == null)
                        {
                            Debug.LogError("herocfg not find id");
                            return;
                        }
                    }
                    else
                    {
                        //英雄时限
                    }
                }//temp 暂时当Label用
                if (heroinfo != null)
                    temp = string.Format(info.content, heroinfo.Name);
                value = item.VipRune.Split(':');
                info = ConfigReader.msgXmlInfoDic[20002];
                for (int i = 0; i < value.Length; i++)
                {
                    if (i % 2 == 0)
                    {
                        //符文ID
                        int index = Convert.ToInt32(value[i]);
                        if (index == 0)
                            break;
                        runeinfo = ConfigReader.GetRuneFromID((uint)index);
                        if (runeinfo == null)
                        {
                            Debug.LogError("runecfg not find id");
                            return;
                        }
                    }
                    else
                    {
                        //符文等级
                        runeLevel = Convert.ToInt32(value[i]);
                    }
                }
                if (runeLevel != 0)
                    temp += (string.Format(info.content, runeinfo.RuneName, runeLevel));

                temp = GetTemp(temp, item.VipPrivilege1, 21001);
                temp = GetTemp(temp, item.VipPrivilege2, 21002);
                temp = GetTemp(temp, item.VipPrivilege3, 21003);
                temp = GetFloatTemp(temp, item.VipPrivilege4, 21004);

                temp = GetTemp(temp, item.VipPrivilege5, 21005);
                temp = GetFloatTemp(temp, item.VipPrivilege6, 21006);
                temp = GetFloatTemp(temp, item.VipPrivilege7, 21007);
                temp = GetTemp(temp, item.VipPrivilege8, 21008);

                temp = GetTemp(temp, item.VipPrivilege9, 21009);
                temp = GetTemp(temp, item.VipPrivilege10, 21010);
                uil.text = temp;
            }
        }

        string GetTemp(string oldTemp, string newTemp, int index)
        {
            int temp = Convert.ToInt32(newTemp);
            MsgConfigInfo info = ConfigReader.msgXmlInfoDic[index];
            if (temp != 0)
            {
                oldTemp += string.Format(info.content, newTemp);
                return oldTemp;
            }
            return oldTemp;
        }

        string GetFloatTemp(string oldTemp, string newTemp, int index)
        {
            float temp = 100 * Convert.ToSingle(newTemp);
            MsgConfigInfo info = ConfigReader.msgXmlInfoDic[index];
            if (temp != 0)
            {
                oldTemp += string.Format(info.content, newTemp + "%");
                return oldTemp;
            }
            return oldTemp;
        }

        //显示
        public override void OnEnable()
        {
            //ShowVipPre();
        }

        //隐藏
        public override void OnDisable()
        {

        }
        private const int clipOffset = 282;

        UIPanel mPanel = null;
        UIGrid mGrid = null;
        GameObject LastLeft = null;
        GameObject LastRight = null;
        GameObject CloseBtn = null;
        GameObject Recharge = null;
        //Dictionary<int,>
    }
}