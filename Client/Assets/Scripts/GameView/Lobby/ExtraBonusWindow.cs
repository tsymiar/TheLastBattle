using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Ctrl;
using System.Text.RegularExpressions;

namespace BlGame.View
{
    public class ExtraBonusWindow : BaseWindow
    {

        public ExtraBonusWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadExtraBonusUI;
            mResident = false;
        }
        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_ExtraBonusEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_ExtraBonusExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_GameSettingExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }


        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ExtraBonusEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_ExtraBonusExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GameSettingExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mExchangeBtn = mRoot.FindChild("ExchangeBtn").GetComponent<UIButton>();
            mInputKey = mRoot.FindChild("InputArea").GetComponent<UIInput>();
            mClose = mRoot.FindChild("CloseBtn").GetComponent<UIButton>();

            EventDelegate.Add(mExchangeBtn.onClick, ExchangePress);
            EventDelegate.Add(mClose.onClick, ClosePress);
        }

        private void ClosePress()
        {
            ExtraBonusCtrl.Instance.Exit();
        }

        private void ExchangePress()
        {
            if (mInputKey.value.Trim() == null || string.IsNullOrEmpty(mInputKey.value.Trim()) || string.Compare(mInputKey.value.Trim(), "") == 0)
            {
                MsgInfoManager.Instance.ShowMsg(10044);
                return;
            }
            string pat = @"[\u4e00-\u9fa5]";
            Regex rg = new Regex(pat);
            Match mh = rg.Match(mInputKey.value.Trim());
            if (mh.Success)
            {
                MsgInfoManager.Instance.ShowMsg(-130755);
                return;
            }
            ExtraBonusCtrl.Instance.SendMsg(mInputKey.value.Trim());
        }

        protected override void RealseWidget()
        {

        }


        public override void OnEnable()
        {

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

        UIInput mInputKey;
        UIButton mExchangeBtn;
        UIButton mClose;
    }
}