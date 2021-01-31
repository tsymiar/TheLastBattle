using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using BlGame.GameData;
using JT.FWW.GameData;
using JT.FWW.Tools;
using System.Linq;
using BlGame;
using BlGame.GuideDate;
using GameDefine;
using System.Text;
using BlGame.Model;
using BlGame.Resource;

namespace BlGame.View
{
    public class UserInfoWindow : BaseWindow
    {
        public UserInfoWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadGameUserUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_UserEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_UserExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_UserEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_UserExit, Hide);
        }

        protected override void RealseWidget()
        {

        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            spriteHead = mRoot.FindChild("Headselect/Background").GetComponent<UISprite>();

            pass = mRoot.FindChild("Pass");
            noPass = mRoot.FindChild("Nopass");
            nickNameInput = mRoot.FindChild("Nickname").GetComponent<UIInput>();

            Transform sexParent = mRoot.FindChild("Sex");
            boySelect = mRoot.Find("Sex/Male").GetComponent<UIToggle>();
            girlSelect = mRoot.Find("Sex/Female").GetComponent<UIToggle>();
     
            boySelect.group = sexSelectGroup;
            girlSelect.group = sexSelectGroup;

            tip = mRoot.FindChild("Tips1").gameObject;
            labelTip = mRoot.FindChild("Tips1/Label").GetComponent<UILabel>();
            labelTip2 = mRoot.FindChild("Tips2/Label").gameObject;


            view = mRoot.FindChild("Head");

            selectObj = mRoot.FindChild("Head/Highlight").gameObject;
            arrowObj = mRoot.transform.FindChild("Head/Highlight/Arrow").gameObject;

            FindHeroHead();
            CreateHead();

            submitBtn = mRoot.FindChild("Commit").GetComponent<UIButton>();
            submitBtn.isEnabled = false;
            scrollView = mRoot.FindChild("Head").GetComponent<UIScrollView>();
            


            arrowLeftRight[0] = mRoot.FindChild("Arrow/Left");
            arrowLeftRight[1] = mRoot.FindChild("Arrow/Right");

            btnDice = mRoot.FindChild("RandomName").GetComponent<ButtonOnPress>();

            UIEventListener.Get(scrollView.gameObject).onPress += OnPress;
            scrollView.onDragStarted += OnDragStart;

            UIEventListener.Get(nickNameInput.gameObject).onSelect += ResetDefaultInput;

           

            EventDelegate.Add(boySelect.onChange, SelectSex);
            EventDelegate.Add(girlSelect.onChange, SelectSex);
           
            /*
             * 当用户在选择了英雄的情况下，再去选择男女英雄时，清除之前的选中状态
             * 
            */
            EventDelegate.Add(boySelect.onChange,ClearSelectStatue);
            EventDelegate.Add(girlSelect.onChange,ClearSelectStatue);

            for (int i = 0; i < headList.Count; i++)
            {
                headList.ElementAt(i).AddListener(i, SelectHead);
            }


            EventDelegate.Add(submitBtn.onClick, SubmitRegister);

           

        }
      
        public void CreateHead()
        {
            Transform headParent = mRoot.FindChild("Head/Panel");
            GameObject headPanel = headParent.gameObject;
            ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadGameHeroHead, ResourceType.PREFAB);

            for (int i = 0; i < ConfigReader.HeadSelectXmlInfoDict.Count; i++)
            {
                GameObject prefab = objUnit.Asset as GameObject;
                GameObject hd = NGUITools.AddChild(headPanel, prefab);
                hd.name = "Head" + (i + 1).ToString();

                if (i >= headsBoy.Count)
                {
                    hd.SetActive(false);
                    continue;
                }


                UISprite sp = hd.transform.FindChild("Portrait/Sprite").GetComponent<UISprite>();
                sp.spriteName = ConfigReader.HeadSelectXmlInfoDict[headsBoy[i]].HeroHeadAtlas_Num;
                ButtonOnPress toggle = headParent.FindChild("Head" + (i + 1).ToString()).GetComponent<ButtonOnPress>();
                headList.Add(toggle);
            }
        }

        public void FindHeroHead()
        {

            foreach (var item in ConfigReader.HeadSelectXmlInfoDict)
            {
                if (item.Value.HeroHeadType.Contains(1))
                {
                    headsBoy.Add(item.Key);
                }

                if(item.Value.HeroHeadType.Contains(2))
                {
                    headsGirl.Add(item.Key);
                }
            }

        }

        public void showHeroHead(List<int> headsBoyOrGirl)
        {

            GameObject headPanel = mRoot.FindChild("Head/Panel").gameObject;
            const float distance = 175 * 4;
            UIGrid mGrid = headPanel.GetComponent<UIGrid>();
            UIScrollView mScroll = mRoot.FindChild("Head").GetComponent<UIScrollView>();
            for (int i = 0; i < ConfigReader.HeadSelectXmlInfoDict.Count;  i++)
            {
                Transform headParent = headPanel.transform.FindChild("Head" +(i + 1).ToString());
                GameObject head = headParent.gameObject;
                if(i >= headsBoyOrGirl.Count)
                {
                    head.SetActive(false);
                    continue;
                }
                head.SetActive(true);
                UISprite sp = headParent.transform.FindChild("Portrait/Sprite").GetComponent<UISprite>();
                sp.spriteName = ConfigReader.HeadSelectXmlInfoDict[headsBoyOrGirl[i]].HeroHeadAtlas_Num;
                ButtonOnPress toggle = headParent.GetComponent<ButtonOnPress>();
                headList.Add(toggle);
            }
            
            mGrid.enabled = true;
            mGrid.Reposition();
            mScroll.ResetPosition();
            GameMethod.ScrollViewTweenPosition(mScroll,distance);
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<EErrorCode>(EGameEvent.eGameEvent_AskComplementedRegisterInfo, RegisterResult);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<EErrorCode>(EGameEvent.eGameEvent_AskComplementedRegisterInfo, RegisterResult);
        }

        //显示
        public override void OnEnable()
        {
            tween = TweenAlpha.Begin(labelTip2, 1f, 0.1f);
            tween.method = UITweener.Method.EaseOut;
            tween.style = UITweener.Style.PingPong;
            ShowTip(nullName);
            IsFirstInput = true;
            posMove = view.transform.localPosition;
            arrowObj.transform.parent.localPosition = view.localPosition;


            PlayerArrowAnimation();
            arrowLeftRight[0].gameObject.SetActive(false);
            arrowLeftRight[1].gameObject.SetActive(true);
            nickNameInput.value = "请输入昵称";

            // nickNameInput.onSelectDelegate += ResetDefaultInput;
            noPass.gameObject.SetActive(false);
            pass.gameObject.SetActive(false);


            //UIGuide
            //boySelect.gameObject.AddComponent<ButtonOnPress>();
            //ButtonOnPress btnBoy = boySelect.gameObject.GetComponent<ButtonOnPress>();
            //btnBoy.AddListener(0, PressSex);
            //btnBoy.AddListener()
            //girlSelect.gameObject.AddComponent<ButtonOnPress>();
            //ButtonOnPress girlBtn = girlSelect.gameObject.GetComponent<ButtonOnPress>();
            //girlBtn.AddListener(1, PressSex);

            btnDice.AddListener(RandNickName, ButtonOnPress.EventType.ClickType);

            //IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEent_UIGuideInputNickNameStart, StartIGuideTask);
            //IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelectSexStart, StartIGuideTask);
            //IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelectHeadStart, StartIGuideTask);
            //IGuideTaskManager.Instance().AddTaskStartListerner(EGameEvent.eGameEvent_UIGuideCommitRegisterStart, StartIGuideTask);
            //IGuideTaskManager.Instance().SendTaskTrigger(EGameEvent.eGameEvent_UIGuideTriggerRegister);

            ShowSelect(headList.ElementAt(2));
         
        }

        //隐藏
        public override void OnDisable()
        {
            GameObject.Destroy(tween);
            IsFirstInput = true;
            tip.SetActive(false);

            btnDice.RemoveListener(RandNickName, ButtonOnPress.EventType.ClickType);
            //UIGuide
            //boySelect.gameObject.GetComponent<ButtonOnPress>().RemoveListener(PressSex);
            //girlSelect.gameObject.GetComponent<ButtonOnPress>().RemoveListener(PressSex);
            //IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEent_UIGuideInputNickNameStart, StartIGuideTask);
            //IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelectSexStart, StartIGuideTask);
            //IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEvent_UIGuideSelectHeadStart, StartIGuideTask);
            //IGuideTaskManager.Instance().RemoveTaskStartListerner(EGameEvent.eGameEvent_UIGuideCommitRegisterStart, StartIGuideTask);
        }


        private UISprite spriteHead;

        private Transform pass;

        private Transform noPass;

        private UILabel labelTip;

        private GameObject tip;

        private GameObject labelTip2;

        private UIInput nickNameInput;

        private UIToggle boySelect;

        private UIToggle girlSelect;

        private UIButton submitBtn;

        private List<ButtonOnPress> headList = new List<ButtonOnPress>();


        List<int> headsBoy = new List<int>();

        List<int> headsGirl = new List<int>();


        private GameObject arrowObj = null;

        private GameObject selectObj = null;

        private UITweener tween;

        private byte isBoy = (byte)1;

        private byte isGirl = (byte)2;


        private bool IsFirstInput = true;


        private const int headListGroup = 2;

        private const int sexSelectGroup = 3;


        private const string invalideName = "无效昵称名";

        private const string collisionName = "昵称已存在";

        private const string nullName = "昵称不能为空";

        bool inDrag = false;
        Transform[] arrowLeftRight = new Transform[2];

        private UIScrollView scrollView;

        #region 箭头效果
        float during = 0.5f;
        float yTween = -15f;
        Transform view = null;
        private Vector3 posMove = new Vector3();
        private ButtonOnPress lastToggle;
        #endregion


        private ButtonOnPress btnDice;

        private void ShowTip(string tipText)
        {
            labelTip.text = tipText;
            tip.SetActive(true);
        }


        void RandNickName(int ie, bool isPress)
        {
            nickNameInput.value = RandomNameData.Instance.GetRandName();
            IsFirstInput = false;
            if (tip.activeInHierarchy)
            {
                tip.SetActive(false);
            }
        }

        //void StartIGuideTask(EGameEvent taskId)
        //{
        //    //List<BoxCollider> boxList = new List<BoxCollider>();
        //    List<GameObject> objList = new List<GameObject>();
        //    switch (taskId)
        //    {
        //        case EGameEvent.eGameEent_UIGuideInputNickNameEnd:
        //            objList.Add(nickNameInput.gameObject);
        //            objList.Add(btnDice.gameObject);
        //            break;
        //        case EGameEvent.eGameEvent_UIGuideSelectSexEnd:
        //            objList.Add(boySelect.gameObject);
        //            objList.Add(girlSelect.gameObject);
        //            objList.Add(nickNameInput.gameObject);
        //            objList.Add(btnDice.gameObject);
        //            break;
        //        case EGameEvent.eGameEvent_UIGuideSelectHeadEnd:
        //            objList.Add(nickNameInput.gameObject);
        //            objList.Add(btnDice.gameObject);
        //            for (int i = 0; i < headList.Count; i++)
        //            {
        //                objList.Add(headList.ElementAt(i).gameObject);
        //            }
        //            break;
        //        case EGameEvent.eGameEvent_UIGuideCommitRegisterEnd:
        //            objList.Add(submitBtn.gameObject);
        //            objList.Add(nickNameInput.gameObject);
        //            objList.Add(btnDice.gameObject);
        //            for (int i = 0; i < headList.Count; i++)
        //            {
        //                objList.Add(headList.ElementAt(i).gameObject);
        //            }
        //            break;
        //    }
        //    //IGuideTaskManager.Instance().SendTaskMarkObjList(taskId, objList);
        //    //IGuideTaskManager.Instance().SendTaskEffectShow(taskId);
        //}

        //void PressSex(int index, bool isDown)
        //{
        //    IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideSelectSexEnd);
        //}

        void OnDragStart()
        {
            Vector3 constraint = scrollView.panel.CalculateConstrainOffset(scrollView.bounds.min, scrollView.bounds.max);
            if (constraint.magnitude > 0.5f)
            {
                if (constraint.x > 0f)//右到头
                {
                    arrowLeftRight[0].gameObject.SetActive(true);
                    arrowLeftRight[1].gameObject.SetActive(false);
                }
                else//左到头
                {
                    arrowLeftRight[0].gameObject.SetActive(false);
                    arrowLeftRight[1].gameObject.SetActive(true);
                }
            }
            else
            {
                arrowLeftRight[0].gameObject.SetActive(true);
                arrowLeftRight[1].gameObject.SetActive(true);
            }
        }

        void OnPress(GameObject go, bool state)
        {
            if (!state)
            {
                // ShowArrow(lastToggle);
                ShowSelect(lastToggle);
                inDrag = false;
            }
            else
            {
                //  ShowArrow(null);
                ShowSelect(null);
                inDrag = true;
            }
        }

        void RegisterResult(EErrorCode m_n32ErrorId)
        {
            if (m_n32ErrorId == EErrorCode.eEC_NULLNickName)
                ShowTip(nullName);
            MsgInfoManager.Instance.ShowMsg((int)m_n32ErrorId);
            noPass.gameObject.SetActive(true);
            pass.gameObject.SetActive(false);
        }

        void ResetDefaultInput(GameObject go, bool state)
        {
            if (state)
            {
                if (IsFirstInput)
                {
                    nickNameInput.value = "";
                    nickNameInput.defaultText = "";
                }
                IsFirstInput = false;
                if (tip.activeInHierarchy)
                {
                    tip.SetActive(false);
                }
            }
            else
            {
                if (!tip.activeInHierarchy)
                    CheckNameInput();
            }
        }

   
        void SelectSex()
        {
            UIToggle toggle = UIToggle.current;
            if (toggle != null && toggle.group == sexSelectGroup && toggle.value)
            {
                if (toggle == boySelect)
                {
                    isBoy = (byte)1;
                    showHeroHead(headsBoy);
                }
                else if( toggle == girlSelect)
                {
                    isBoy = (byte)2;
                    showHeroHead(headsGirl);
                }
            }
        }



        void SelectHead(int ie, bool isDown)
        {
            ButtonOnPress btn = headList.ElementAt(ie);
            Transform headParent = btn.transform;
            spriteHead.spriteName = headParent.FindChild("Portrait").FindChild("Sprite").GetComponent<UISprite>().spriteName;
            ShowSelect(btn);
            lastToggle = btn;

            submitBtn.isEnabled = true;

            IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideSelectHeadEnd);
        }

        void ShowSelect(ButtonOnPress toggle)
        {
            if (toggle == null)
            {
                selectObj.SetActive(false);
            }
            else
            {
                selectObj.transform.position = toggle.transform.position;
                if (!selectObj.activeInHierarchy)
                
                    selectObj.SetActive(true);
              }
        }


        /*
         * 当用户在选择了英雄的情况下，再去选择男女英雄时，清除之前的选中状态
         * 
         */
        void ClearSelectStatue()
        {
                selectObj.SetActive(false);
                submitBtn.isEnabled = false;
        }

        void PlayerArrowAnimation()
        {
            UITweener tweenpos = TweenPosition.Begin(arrowObj, during, new Vector3(arrowObj.transform.localPosition.x, arrowObj.transform.localPosition.y + yTween, 0f));
            tweenpos.style = TweenPosition.Style.PingPong;
            tweenpos.method = TweenPosition.Method.EaseIn;
        }

        bool CheckNameInput()
        {
            if (String.IsNullOrEmpty(nickNameInput.label.text))
            {
                ShowTip(nullName);
                return false;
            }

            if (!CTools.CheckName(nickNameInput.label.text))
            {
                ShowTip(invalideName);
                return false;
            }

            IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEent_UIGuideInputNickNameEnd);
            return true;
        }

        void SubmitRegister()
        {
            if (IsFirstInput)
            {
                noPass.gameObject.SetActive(true);
                MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_FillInNiceName);
                return;
            }
            if (!CheckNameInput())
            {
                noPass.gameObject.SetActive(true);
                MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_FillInNiceName);
                return;
            }

         

            List<string> split = GameMethod.GetSplit();
            string currStr = nickNameInput.value;
            foreach (string item in split)
            {
                int currPos = currStr.IndexOf(item, StringComparison.OrdinalIgnoreCase);
                if (currPos > -1)
                {
                    MsgInfoManager.Instance.ShowMsg(-130861);
                    return;
                }
            }
            IGuideTaskManager.Instance().SendTaskEnd(EGameEvent.eGameEvent_UIGuideCommitRegisterEnd);

            byte[] bytes = Encoding.UTF8.GetBytes(nickNameInput.label.text);
            CGLCtrl_GameLogic.Instance.GameCompleteBaseInfo(SelectServerData.Instance.serverUin, bytes, Convert.ToInt32(spriteHead.spriteName), isBoy);
        }

        
    }
}
