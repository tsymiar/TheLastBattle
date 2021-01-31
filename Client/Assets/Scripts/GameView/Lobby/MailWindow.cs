using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameDefine;
using BlGame.Ctrl;
using UnityEngine;
using BlGame.GameEntity;
using BlGame.Resource;
using BlGame.Model;

namespace BlGame.View
{
    public class MailWindow : BaseWindow
    {
        public MailWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadMailGUI;
            mResident = false;
        }

        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_MailEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_MailExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_MailEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_MailExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide); 
        }

        //窗口控件初始化
        protected override void InitWidget()
        { 
            mTemp = mRoot.FindChild("MailIntroduction");
            curMailObj = new UIMail();
            //右边窗体内容
            curMailObj.mMailTitleTxt = mRoot.FindChild("MailIntroduction/Scroll View/Table/Title").GetComponent<UILabel>();
            curMailObj.mMailContentTxt = mRoot.FindChild("MailIntroduction/Scroll View/Table/Content").GetComponent<UILabel>();
            curMailObj.mMailCreateTimeTxt = mRoot.FindChild("MailIntroduction/Scroll View/Table/PublishDate").GetComponent<UILabel>();
            curMailObj.mMailSenderTxt = mRoot.FindChild("MailIntroduction/Scroll View/Table/Sender").GetComponent<UILabel>();

            curMailObj.mGrid = mRoot.FindChild("MailIntroduction/Scroll View/Table/Presents/Grid").GetComponent<UIGrid>();
            curMailObj.mCloseMailBtn = mRoot.FindChild("Button").gameObject;

            closeBtn = mRoot.FindChild("CloseBtn").gameObject;
            //左边邮件列表 
            leftMailGrid = mRoot.FindChild("MailList/Panel/Grid").GetComponent<UIGrid>();

            UIEventListener.Get(curMailObj.mCloseMailBtn).onClick += AskCloseOrGetMailGift;

            UIEventListener.Get(closeBtn).onClick += CloseMailGUI;

            InitMailTitleListInfo(); 
        } 

        Transform mTemp = null;
        GameObject closeBtn = null;
        UIMail curMailObj = null;
        int curtMailId = 0;
        UIGrid leftMailGrid = null;
        MailName LastObj = null;

        GameObject curtActiveObj = null;

        public class UIMail
        {
            public Int32 mMailId;
            public UISprite mMailSprite;         //邮件当前状态
            public int mMailCurtState;
            public int mMailType;

            public UILabel mMailTitleTxt;    //邮件标题
            public UILabel mMailContentTxt;   //邮件内容
            public UILabel mMailCreateTimeTxt;    //邮件发送时间
            public UILabel mMailSenderTxt;    //邮件发送者 

            public UIGrid mGrid;
            public GameObject mCloseMailBtn;    //button  

            public void Reset()
            {
                mMailId = 0; 
                mMailCurtState = 0;
                mMailType = 0;
                mMailTitleTxt.text = "";
                mMailContentTxt.text = "";
                mMailCreateTimeTxt.text = "";
                mMailSenderTxt.text="";
                mGrid.transform.parent.gameObject.SetActive(false);
            }
        }

        private Dictionary<GameObject, MailName> leftMailList = new Dictionary<GameObject, MailName>();
        private List<GameObject> GoldList = new List<GameObject>();
        public class MailName
        {
            public UILabel currMailName;
            public UILabel readedMailName;
           
            public UISprite currMailSelect;
            public GameObject objParent = null;
            public int currMailId;

            public void Reset()
            {
                currMailName.text = "";
                readedMailName.text = "";
                currMailId = 0;
                objParent = null;
            }
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<MailData>(EGameEvent.eGameEvent_UpdateMailInfo, UpdateMail); 
            EventCenter.AddListener(EGameEvent.eGameEvent_UpdateMailList, ResetMail);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<MailData>(EGameEvent.eGameEvent_UpdateMailInfo, UpdateMail);
            
            EventCenter.RemoveListener(EGameEvent.eGameEvent_UpdateMailList, ResetMail);
        }

        //窗口控件释放
        protected override void RealseWidget()
        { 
            Clear();
        } 

        //显示
        public override void OnEnable()
        {

#if UNITY_STANDALONE_WIN || UNITY_EDITOR
#else
         SdkConector.ShowToolBar(1);
#endif

        }

        //隐藏
        public override void OnDisable()
        {

        }
        public void InitMailTitleListInfo()
        {
            List<KeyValuePair<int, MailData>> mailList = MailModel.Instance.MailList;
            if (mailList != null && mailList.Count > 0)
            {
                foreach (var kv in mailList)
                {
                    GameObject obj = null;

                    ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadMailTitleGUI, ResourceType.PREFAB);
                    obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
                    obj.transform.parent = leftMailGrid.transform;
                    obj.transform.localPosition = Vector3.zero;
                    obj.transform.localScale = Vector3.one;

                    MailName mail = new MailName();
                    mail.currMailSelect = obj.transform.FindChild("Highlight").GetComponent<UISprite>();
                    mail.currMailName = obj.transform.FindChild("Name").GetComponent<UILabel>();
                    mail.readedMailName = obj.transform.FindChild("NameRead").GetComponent<UILabel>();
                    
                    mail.currMailId = kv.Key;

                    int start = kv.Value.mailTitle.IndexOf("]") + 1;
                    int end = kv.Value.mailTitle.LastIndexOf("[");
                    string mailText = kv.Value.mailTitle.Substring(start, end - start);

                    string newMailTitle = "";
                    //超过一定字符，则做切断
                    if (mailText.Length >= 6)
                    {
                        newMailTitle = mailText.Substring(0, 6) + "。。";
                        newMailTitle = kv.Value.mailTitle.Replace(mailText, newMailTitle);
                    }
                    else
                    {
                        newMailTitle = kv.Value.mailTitle;  
                    }

                    //如果查看过的 则设置颜色为灰色 
                     mail.readedMailName.text = mail.currMailName.text = newMailTitle;
                     if (kv.Value.mailCurtState != EMailCurtState.eMailState_New)
                     {
                         mail.readedMailName.gameObject.SetActive(true);
                         mail.currMailName.gameObject.SetActive(false);
                     }
                     else {
                         mail.readedMailName.gameObject.SetActive(false);
                         mail.currMailName.gameObject.SetActive(true);
                     }

                    if ( (LastObj == null)  || (LastObj.currMailId <= mail.currMailId && kv.Value.mailCurtState == EMailCurtState.eMailState_New) )
                    {
                        LastObj = mail;
                        curtActiveObj = obj;
                    }
                    mail.objParent = obj;

                    UIEventListener.Get(obj).onClick += AskMailInfo;

                    leftMailList.Add(obj, mail);
                }

                if (curtActiveObj != null)
                {
                    LastObj.currMailSelect.gameObject.SetActive(true);
                    curtMailId = LastObj.currMailId;
                    MailCtrl.Instance.GetMailContent(curtMailId);
                }
                UpdateLeftMailTitleList();
            }
        }
       
        //获取当前选择的邮件标题:通过邮件id
        public void AskMailInfo(GameObject obj)
        {
            MailName mail = null;
            if (leftMailList.TryGetValue(obj, out mail))
            {
                curtMailId = mail.currMailId;

                LastObj.currMailSelect.gameObject.SetActive(false);

                curtActiveObj = obj;
                LastObj = mail;
                LastObj.currMailSelect.gameObject.SetActive(true);

                MailCtrl.Instance.GetMailContent(curtMailId);

                UpdateLeftMailTitleList();
            }
        }

        public void UpdateLeftMailTitleList()
        {
            leftMailGrid.repositionNow = true;
            leftMailGrid.Reposition();
        }
        //通过邮件id
        private void AskCloseOrGetMailGift(GameObject mailObj)
        {
            Debug.Log("-----------AskCloseOrGetMailGift--------------" + curtMailId);
            if (curtMailId != null && curtMailId > 0)
            {
                MailCtrl.Instance.CloseOrGetMailGift(curtMailId);
            }
        }

        //关闭
        private void CloseMailGUI(GameObject mailObj)
        { 
            RealseWidget();
            Destroy();
            mVisible = false;
        }
        
        void Clear()
        {
            curtMailId = 0;
            LastObj = null;
             
            foreach (var item in leftMailList)
            {
                UnityEngine.GameObject.DestroyImmediate(item.Key);
            }
            leftMailList.Clear();

            curMailObj.Reset();   
        }

        public void ResetMail()
        {
            if (mRoot != null)
            {
                Debug.Log("------InitWidget-------------");

                Clear();
                InitMailTitleListInfo(); 
            }
        }

        public void UpdateMail(MailData mdb)
        {
            if (GoldList.Count != 0)
            {
                foreach (var item in GoldList)
                {
                    UnityEngine.GameObject.DestroyImmediate(item);
                }
                curMailObj.mGrid.transform.parent.gameObject.SetActive(false);
                GoldList.Clear();
            }
            if ((curMailObj != null) && (mdb != null) && (curtMailId == mdb.MailId))
            {
                curMailObj.mMailSenderTxt.text = mdb.mailSender;
                curMailObj.mMailTitleTxt.text = mdb.mailTitle;
                curMailObj.mMailContentTxt.text = mdb.mailContent;
                curMailObj.mMailCreateTimeTxt.text = mdb.mailCreateTime;
                mTemp.gameObject.SetActive(true);
                string[] giftArray = mdb.mailGift.Split('|');
                for (int i = 0; i < giftArray.Length; i++)
                {
                    if (giftArray[i].Length == 0)
                        continue;
                    curMailObj.mGrid.transform.parent.gameObject.SetActive(true);
                    ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadMailAttachGUI, ResourceType.PREFAB);
                    GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
                    obj.transform.parent = curMailObj.mGrid.transform;
                    obj.transform.localPosition = Vector3.zero;
                    obj.transform.localScale = Vector3.one;
                    GoldList.Add(obj);
                    string[] giftStr = giftArray[i].Split(',');
                    if (giftStr.Length == 3)
                    {
                        Int32 goodsId = Convert.ToInt32(giftStr[1]);
                        
                        MailGiftType giftType = (MailGiftType)Convert.ToInt32(giftStr[0]);
                        switch (giftType)
                        {
                            case MailGiftType.eMerchType_Gold:
                                {
                                    obj.GetComponent<UISprite>().spriteName = "9";
                                    obj.transform.FindChild("Label").GetComponent<UILabel>().text = giftStr[2];
                                } break;
                            case MailGiftType.eMerchType_Diamond:
                                {
                                    obj.GetComponent<UISprite>().spriteName = "10";
                                    obj.transform.FindChild("Label").GetComponent<UILabel>().text = giftStr[2];
                                } break; 
                               //HeroBuyCfg:100001-109999
                              //  SkinCfg:110001-119999
                             //   RuneCfg:120001-129999
                           default:
                            { 
                                if (goodsId >= 100001 && goodsId <= 109999)
                                {
                                    HeroBuyConfigInfo buyInfo = ConfigReader.GetHeroBuyInfo(goodsId);
                                    if (buyInfo != null)
                                    {
                                        ResourceUnit GetMoneyUnit = ResourcesManager.Instance.loadImmediate(GameConstDefine.LoadDailyHeroIcon, ResourceType.PREFAB);
                                        UIAtlas uia_hero = (GetMoneyUnit.Asset as GameObject).GetComponent<UIAtlas>();

                                        obj.GetComponent<UISprite>().atlas = uia_hero;
                                        obj.GetComponent<UISprite>().spriteName = buyInfo.DefaultIcon.ToString();
                                        obj.transform.FindChild("Label").GetComponent<UILabel>().text = giftStr[2];
                                    }
                                }
                                else if (goodsId >= 120001 && goodsId <= 129999)
                                {
                                    RuneConfigInfo runeInfo = ConfigReader.GetRuneFromID((uint)goodsId);
                                    if (runeInfo != null)
                                    {
                                        obj.GetComponent<UISprite>().spriteName = runeInfo.Icon.ToString();
                                        obj.transform.FindChild("Label").GetComponent<UILabel>().text = giftStr[2];
                                    }
                                }
                                else if (goodsId >= 110001 && goodsId <= 119999)
                                {
                                    HeroSkinConfigInfo skinInfo = ConfigReader.GetHeroSkinInfo(goodsId);
                                    if (skinInfo != null)
                                    {
                                        obj.GetComponent<UISprite>().spriteName = skinInfo.Icon.ToString();
                                        obj.transform.FindChild("Label").GetComponent<UILabel>().text = giftStr[2];
                                    }
                                    
                                }
                                else if (goodsId >= 130001 && goodsId <= 139999)
                                {
                                    OtherItemConfigInfo otherInfo = ConfigReader.GetOtherItemInfo((uint)goodsId);
                                    if (otherInfo != null)
                                    {
                                        obj.GetComponent<UISprite>().spriteName = otherInfo.icon;
                                        obj.transform.FindChild("Label").GetComponent<UILabel>().text = giftStr[2];
                                    }
                                }
                            } break; 
                        }
                    }
                    else
                    {
                        Debug.LogError("the error mail gift str:" + giftArray[i]);
                    }
                }
            }
            UpdateLeftMailTitleList();
            curMailObj.mGrid.repositionNow = true;
            curMailObj.mGrid.Reposition();
        }
    }

}
