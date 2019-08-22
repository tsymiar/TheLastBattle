using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using UICommon;
using BlGame.GameEntity;
using System.Linq;
using JT.FWW.GameData;
using GameDefine;

namespace BlGame.View
{
    public class EquipmentWindow : BaseWindow
    {
        public EquipmentWindow() 
        {
            mScenesType = EScenesType.EST_Play;
            mResName = GameConstDefine.LoadEquipmentUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
           mPanel = mRoot.FindChild("Panel");

           mBackPackage = mRoot.FindChild("Panel/Adjust/Package/BackPackage");
           for (int i = 0; i < 3; i++)
           {
               mSpriteBackPack.Add(mBackPackage.FindChild("ItemPack" + (i + 1).ToString()).FindChild("icon").GetComponent<UISprite>());
               mBoxList.Add(mBackPackage.FindChild("ItemPack" + (i + 1).ToString()).gameObject);
           }

            //pack
            Transform package = mRoot.FindChild("Panel/Adjust/Package");
            for (int it = 0; it < 3; it++)
            {
                int iti = it + 1;
                ButtonOnPress itBtn = package.FindChild("Item" + iti.ToString()).GetComponent<ButtonOnPress>();
                mBtnItems.Add(itBtn);
                mItemDic.Add(it, itBtn.transform.FindChild("icon").GetComponent<UISprite>());

                mItemSelect.Add(it, itBtn.transform.FindChild("SpriteSelect").GetComponent<UISprite>());
                mItemCost.Add(it, itBtn.transform.FindChild("Cost").gameObject);
                mItemCountDic.Add(it, itBtn.transform.FindChild("LabelCount").GetComponent<UILabel>());
                mItemCdDic.Add(it, itBtn.transform.GetComponent<CdCountDown>());
                mBoxList.Add(itBtn.gameObject);
            }

            //pack page btn
            mBtnPage = mRoot.FindChild("Panel/Adjust/Package/pack").GetComponent<ButtonOnPress>();

            for (int i = 0; i < 2; i++)
            {
                UISprite sprite = mBtnPage.transform.FindChild("Fg" + (i + 1).ToString()).GetComponent<UISprite>();
                mSpritePage.Add(sprite);
            }
            mAdjust = mRoot.FindChild("Panel/Adjust");
			//设置比例
			AdjustUI adjust = mAdjust.GetComponent<AdjustUI> ();
			adjust.ForceUpdate ();


            mObjPack = mAdjust.FindChild("Package").gameObject;
            mShowBtn = mAdjust.FindChild("ShowButton").GetComponent<ButtonOnPress>();
            
            mBg = mShowBtn.transform.FindChild("BG2").gameObject;
            mBtnPage.AddListener(OnSelectpage);
            mShowBtn.AddListener(OnShowEquipBlank);

            for (int it = 0; it < 3; it++)
            {
                UIObjDragEvent drag = mBtnItems.ElementAt(it).gameObject.GetComponent<UIObjDragEvent>();
                drag.ObjDragEvent -= OnItemDragEvent;

                UIEventListener.Get(mBtnItems.ElementAt(it).gameObject).onClick = OnSelectItemToUse;
            }

            ShowBackPackage(false);
        }

        //删除Login外其他控件，例如
        public static void DestroyOtherUI()
        {
          
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
            mSpriteBackPack.Clear();
            mBoxList.Clear();
            mBtnItems.Clear();
            mItemDic.Clear();
            mItemSelect.Clear();
            mItemCost.Clear();
            mItemCountDic.Clear();
            mItemCdDic.Clear();
            mBoxList.Clear();
            mSpritePage.Clear();
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_UpdateUserGameItems, UpdatePackBySever);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_UpdateUserGameItems, UpdatePackBySever);
        }

        //显示
        public override void OnEnable()
        {
            mSpritePage.ElementAt(0).gameObject.SetActive(true);
            mSpritePage.ElementAt(1).gameObject.SetActive(false);
            mPageIndex = PackPage.Page1;

            mOrginalPos = mPanel.transform.localPosition;
            updateTag = true;

            UpdatePack(mPageIndex);
            ShowBlank(BlankShowState.CloseState);
        }

        //隐藏
        public override void OnDisable()
        {
        }

        private enum PackPage
        {
            Page1 = 0,
            Page2,
        }
        
        private enum BlankShowState
        {
            OpenState,
            CloseState,
        }


        PackPage mPageIndex = PackPage.Page1;
        BlankShowState showState = BlankShowState.CloseState;

        Dictionary<int, UISprite> mItemDic = new Dictionary<int, UISprite>();
        Dictionary<int, UILabel> mItemCountDic = new Dictionary<int, UILabel>();
        Dictionary<int, CdCountDown> mItemCdDic = new Dictionary<int, CdCountDown>();
        Dictionary<int, UISprite> mItemSelect = new Dictionary<int, UISprite>();
        Dictionary<int, GameObject> mItemCost = new Dictionary<int, GameObject>();

        Transform mAdjust = null;

        private GameObject mObjPack = null;
        private Transform mPanel;   
        private ButtonOnPress mBtnPage = null;

        private List<ButtonOnPress> mBtnItems = new List<ButtonOnPress>();
        private List<UISprite> mSpritePage = new List<UISprite>();
       
        private GameObject mObjMove = null;

        private Transform mBackPackage = null;
        private List<UISprite> mSpriteBackPack = new List<UISprite>();
        private List<GameObject> mBoxList = new List<GameObject>();

        private ButtonOnPress mShowBtn = null;

        private bool mIsMove = false;

        private Vector3 mOrginalPos = new Vector3();
        private float mYOffset = 85f;
        private UITweener mTweenBlank = null;

        void OnShowEquipBlank(int ie, bool isPress)
        {
            if (showState == BlankShowState.OpenState)
            {
                ShowBlank(BlankShowState.CloseState);
            }
            else
            {
                ShowBlank(BlankShowState.OpenState);
            }
        }

        void ShowBlank(BlankShowState state)
        {
            showState = state;
            switch (state)
            {
			    case BlankShowState.CloseState:
                    Vector3 pos = new Vector3(mOrginalPos.x, mOrginalPos.y - (mYOffset * mAdjust.localScale.x), mOrginalPos.z);
                    mTweenBlank = TweenPosition.Begin(mPanel.gameObject, 0.2f, pos);
                    mBg.gameObject.SetActive(false);
                    break;
                case BlankShowState.OpenState:
                    mObjPack.SetActive(true);
                    mTweenBlank = TweenPosition.Begin(mPanel.gameObject, 0.2f, mOrginalPos);
                    mBg.gameObject.SetActive(true);
                    break;
            }
            
            mTweenBlank.method = UITweener.Method.Linear;
            mTweenBlank.style = UITweener.Style.Once;
            EventDelegate.Add(mTweenBlank.onFinished, FinishMoveBlank);
        }
        GameObject mBg = null;
        void FinishMoveBlank()
        {
            if (showState == BlankShowState.CloseState)
            {
                mObjPack.SetActive(false);
            }
            
            EventDelegate.Remove(mTweenBlank.onFinished, FinishMoveBlank);
            SetDragItems();
        }

        void SetDragItems()
        {
            mTweenBlank = null;
            for (int it = 0; it < 3; it++)
            {
                UIObjDragEvent drag = mBtnItems.ElementAt(it).gameObject.GetComponent<UIObjDragEvent>();
                drag.ObjDragEvent += OnItemDragEvent;
            }
        }

        void OnItemDragEvent(GameObject obj, UIObjDragEvent.DragState state, Vector2 pos)
        {
            switch (state)
            {
                case UIObjDragEvent.DragState.DragMove:
                    ItemMove(obj, pos);
                    break;
                case UIObjDragEvent.DragState.InvalideDrag:
                    if (mObjMove != null)
                        mObjMove.gameObject.SetActive(false);
                    if (mIsMove)
                    {
                        UpdatePackBySever();
                    }
                    ShowBackPackage(false);
                    mIsMove = false;

                    break;
                case UIObjDragEvent.DragState.LongPressStart:
                    ItemStartMove(obj);
                    ShowBackPackage(true);
                    break;
                case UIObjDragEvent.DragState.DragEnd:
                    ItemMoveEnd(obj, pos);
                    break;
            }
        }

        void ShowBackPackage(bool show)
        {
            int showStart = 0;
            if (mPageIndex == PackPage.Page1)
            {
                showStart = 3;
            }
            if (PlayerManager.Instance == null)
            {
                return;
            }

            mBackPackage.gameObject.SetActive(show);

            for (int i = 0; i < 3; i++)
            {
                mSpriteBackPack.ElementAt(i).gameObject.SetActive(show);
//                 if (mSpriteBackPack.ElementAt(i).parent != null)
//                     mSpriteBackPack.ElementAt(i).parent.gameObject.SetActive(show);
                
                int count = 0, itemId = -1;
                if (PlayerManager.Instance.LocalPlayer != null && PlayerManager.Instance.LocalPlayer.UserGameItemsCount != null && PlayerManager.Instance.LocalPlayer.UserGameItemsCount.Count > (showStart + i))
                {
                    count = PlayerManager.Instance.LocalPlayer.UserGameItemsCount.ElementAt(showStart + i).Value;
                    itemId = PlayerManager.Instance.LocalPlayer.UserGameItems.ElementAt(showStart + i).Value;
                }
                if (count != 0 && ConfigReader.ItemXmlInfoDict.ContainsKey(itemId))
                {
                    string name = ConfigReader.ItemXmlInfoDict[itemId].sIcon;
                    mSpriteBackPack.ElementAt(i).spriteName = name;
                    mSpriteBackPack.ElementAt(i).gameObject.SetActive(true);
                }
                else
                {
                    mSpriteBackPack.ElementAt(i).gameObject.SetActive(false);
                }
            }
        }



        void ItemStartMove(GameObject obj)
        {
            if (mIsMove) return;
            mIsMove = true;

            int index = GetIndexOfItem(obj);
            SetItemEnableByIndex(index, false);
            mObjMove = obj.transform.FindChild("SpriteMove").gameObject;
            UISprite sprite = mObjMove.GetComponent<UISprite>();
            UISprite icon;
            mItemDic.TryGetValue(index, out icon);
            sprite.spriteName = icon.spriteName;
            mObjMove.gameObject.SetActive(true);

        }

        void ItemMove(GameObject obj, Vector2 pos)
        {
            if (mObjMove == null || !mIsMove)
            {
                return;
            }
            Vector3 newPos = UICommonMethod.GetWorldPos(pos);
            mObjMove.transform.position = newPos;
        }

        void ItemMoveEnd(GameObject obj, Vector2 pos)
        {
            if (mObjMove == null || !mIsMove)
            {
                return;
            }
            int index = GetIndexOfItem(obj);

            if (mObjMove != null)
            {
                mObjMove.gameObject.SetActive(false);
                mObjMove.transform.localPosition = Vector3.zero;
            }

            int packIndex = CheckInBackPackage();
            int src = index;
            if (packIndex != -1)
            {

                if (mPageIndex == PackPage.Page2)
                {
                    src += 3;
                }
                else
                {
                    if (packIndex < 3)
                        packIndex += 3;
                    else
                        packIndex -= 3;
                }
                CGLCtrl_GameLogic.Instance.EmsgToss_AskMoveGoods(src, packIndex);
            }
            else
            {
                SetItemEnableByIndex(index, true);
            }
            ShowBackPackage(false);
            mIsMove = false;
            mObjMove = null;
        }

        int CheckInBackPackage()
        {
            if (mObjMove == null) return -1;
            if (UICamera.currentTouch.current == null) return -1;
            for (int i = 0; i < mBoxList.Count; i++)
            {
                if (mBoxList.ElementAt(i) == UICamera.currentTouch.current)
                    return i;
            }
            return -1;
        }

        int GetIndexOfItem(GameObject obj)
        {
            for (int i = 0; i < 3; i++)
            {
                if (mBtnItems.ElementAt(i).gameObject == obj)
                    return i;
            }
            return -1;
        }

        void SetItemEnableByIndex(int index, bool enable)
        {

            mItemDic.ElementAt(index).Value.gameObject.SetActive(enable);
            mItemCountDic.ElementAt(index).Value.gameObject.SetActive(enable);
            mItemCdDic.ElementAt(index).Value.SetCdShow(false);
            mItemCost.ElementAt(index).Value.SetActive(enable);
            updateTag = enable;
        }

        void ShowAllItemCost()
        {
            if (!updateTag) return;
            for (int i = 0; i < 3; i++)
            {
                ShowItemCost(i);
            }
        }

        void ShowItemCost(int index)
        {
            float cdTime = 0f;
            if (PlayerManager.Instance == null || PlayerManager.Instance.LocalPlayer == null)
                return;
            if (!mItemCdDic.ElementAt(index).Value.gameObject.activeInHierarchy)
                return;
            if (mItemCdDic.ElementAt(index).Value.cdSlider.gameObject.activeInHierarchy)
            {
                mItemCost.ElementAt(index).Value.SetActive(false);
            }
            else
            {
                int count = 0, itemId = -1;
                int start = 0;
                if (mPageIndex == PackPage.Page2)
                    start += 3;
                start += index;
                PlayerManager.Instance.LocalPlayer.UserGameItemsCount.TryGetValue(start, out count);
                PlayerManager.Instance.LocalPlayer.UserGameItems.TryGetValue(start, out itemId);
                if (count == 0 || !ConfigReader.ItemXmlInfoDict.ContainsKey(itemId) || ConfigReader.ItemXmlInfoDict[itemId].un8UseTimes == 0)
                {
                    if (mItemCost.ElementAt(index).Value.activeInHierarchy)
                    {
                        mItemCost.ElementAt(index).Value.SetActive(false);
                    }
                    return;
                }
                int skillId = ConfigReader.ItemXmlInfoDict[itemId].un32SkillID;
                SkillManagerConfig info = ConfigReader.GetSkillManagerCfg(skillId);
                bool showTag = false;
                Iselfplayer iPlayer = PlayerManager.Instance.LocalPlayer;
                if (iPlayer == null || cdTime > 0)
                    return;
                if (iPlayer.Mp < info.mpUse)
                {
                    showTag = true;
                }
                if (iPlayer.Hp < info.hpUse)
                {
                    showTag = true;
                }
                if (iPlayer.Cp < info.cpUse)
                {
                    showTag = true;
                }
                if (mItemCost.ElementAt(index).Value.activeInHierarchy != showTag)
                {
                    mItemCost.ElementAt(index).Value.SetActive(showTag);
                }
            }
        }


        void UpdatePackBySever()
        {
            UpdatePack(mPageIndex);
            updateTag = true;
            ShowAllItemCost();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ie"></param>
        /// <param name="pressed"></param>
        //    private void OnSelectItemToUse(int ie , bool pressed)
        private void OnSelectItemToUse(GameObject obj)
        {
            int ie = GetIndexOfItem(obj);
            //if (pressed)
            //{
            //    return;
            //}
            int count = 0, itemId = -1;
            float cdCool = 0f;
            int start = 0;
            if (mPageIndex == PackPage.Page2)
                start += 3;
            start += ie;
            PlayerManager.Instance.LocalPlayer.UserGameItemsCount.TryGetValue(start, out count);
            PlayerManager.Instance.LocalPlayer.UserGameItemsCoolDown.TryGetValue(start, out cdCool);
            PlayerManager.Instance.LocalPlayer.UserGameItems.TryGetValue(start, out itemId);
            if (cdCool > 0f && count != 0)
            {//如果对应背包物品为空，或者个数为0，或者正在CD不发送使用请求
                MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_GoodsIsCoolingDown);
                return;
            }
            if (!ConfigReader.ItemXmlInfoDict.ContainsKey(itemId) || count == 0)
            {
                return;
            }

            CGLCtrl_GameLogic.Instance.EmsgToss_AskUseGoods(start);
        }

        private void OnSelectpage(int ie, bool press)
        {
            if (mPageIndex == PackPage.Page1)
                mPageIndex = PackPage.Page2;
            else
                mPageIndex = PackPage.Page1;
            switch (mPageIndex)
            {
                case PackPage.Page1:
                    mSpritePage.ElementAt(0).gameObject.SetActive(true);
                    mSpritePage.ElementAt(1).gameObject.SetActive(false);
                    break;
                case PackPage.Page2:
                    mSpritePage.ElementAt(0).gameObject.SetActive(false);
                    mSpritePage.ElementAt(1).gameObject.SetActive(true);
                    break;
            }
            UpdatePack(mPageIndex);
            ShowAllItemCost();
        }

        private void UpdatePack(PackPage page)
        {
            if (PlayerManager.Instance == null)
            {
                return;
            }
            int index = 0;
            if (page != PackPage.Page1)
            {
                index = 3;
            }

            if (PlayerManager.Instance.LocalPlayer != null)
            {
                bool isEmpty = true;
                foreach (var itemCount in PlayerManager.Instance.LocalPlayer.UserGameItemsCount.Values)
                {
                    if (itemCount != 0)
                    {
                        isEmpty = false;
                        break;
                    }
                }
                if (isEmpty)
                {
                    if (showState == BlankShowState.OpenState)
                    {
                        updateTag = false;
                        ShowBlank(BlankShowState.CloseState);
                    }
                }
                else if (showState == BlankShowState.CloseState)
                {
                    timeStart = Time.time;
                    updateTag = true;
                    ShowBlank(BlankShowState.OpenState);
                }
            }

            for (int i = index; i < index + 3; i++)
            {
                int start = i % 3;
                int count = 0;
                int itemId = -1;
                float cd = 0f;
                UISprite sprite = null;
                UILabel label = null;
                CdCountDown cdCount = null;

                if (PlayerManager.Instance.LocalPlayer != null)
                {
                    PlayerManager.Instance.LocalPlayer.UserGameItemsCount.TryGetValue(i, out count);//0~6
                    PlayerManager.Instance.LocalPlayer.UserGameItems.TryGetValue(i, out itemId);
                    PlayerManager.Instance.LocalPlayer.UserGameItemsCoolDown.TryGetValue(i, out cd);
                }

                mItemDic.TryGetValue(start, out sprite);//0~3
                mItemCountDic.TryGetValue(start, out label);
                mItemCdDic.TryGetValue(start, out cdCount);
                label.gameObject.SetActive(false);
                label.text = "";
                cdCount.EndCdCountDown();
                if (count != 0 && ConfigReader.ItemXmlInfoDict.ContainsKey(itemId))
                {
                    if (ConfigReader.ItemXmlInfoDict[itemId].un8OverlapTimes >= 2 && count > 1)
                    {
                        label.gameObject.SetActive(true);
                        label.text = count.ToString();
                    }
                    if (cd > 0f && ConfigReader.ItemXmlInfoDict[itemId].un32CdTime > 0f)
                    {
                        cdCount.SetCdShow(true);
                        cdCount.StartCdCountDown(ConfigReader.ItemXmlInfoDict[itemId].un32CdTime, cd);
                    }
                    sprite.gameObject.SetActive(true);
                    sprite.spriteName = ConfigReader.ItemXmlInfoDict[itemId].sIcon;
                    cdCount.GetComponent<UIObjDragEvent>().enabled = true;
                    ShowSelect(start, true);
                }
                else
                {
                    sprite.gameObject.SetActive(false);
                    cdCount.GetComponent<UIObjDragEvent>().enabled = false;
                    ShowSelect(start, false);
                }
            }



        }

        void ShowSelect(int index, bool show)
        {// 0~3
            int count = 0, itemId = -1;
            float cdTime = 0f;
            UISprite spriteSelect = null;
            int itemIndex = index;
            if (mPageIndex != PackPage.Page1)
            {
                itemIndex += 3;
            }
            if (!mItemSelect.TryGetValue(index, out spriteSelect)) return;
            if (PlayerManager.Instance.LocalPlayer != null)
            {
                PlayerManager.Instance.LocalPlayer.UserGameItemsCount.TryGetValue(itemIndex, out count);
                PlayerManager.Instance.LocalPlayer.UserGameItems.TryGetValue(itemIndex, out itemId);
                PlayerManager.Instance.LocalPlayer.UserGameItemsCoolDown.TryGetValue(itemIndex, out cdTime);
            }
            spriteSelect.gameObject.SetActive(show);
            if (count == 0 || !ConfigReader.ItemXmlInfoDict.ContainsKey(itemId) || ConfigReader.ItemXmlInfoDict[itemId].un8UseTimes == 0)
            {
                spriteSelect.gameObject.SetActive(false);
            }
        }

        private void OnBackToCity(int ie, bool isDown)
        {
            if (isDown)
            {
                Debug.Log("OnBackToCity");
                return;
            }

        }

        const float updateTime = 1f;
        float timeStart = 0f;
        bool updateTag = false;
        public override void Update(float deltaTime)
        {
            if (Time.time - timeStart >= updateTime)
            {
                ShowAllItemCost();
                timeStart = Time.time;
            }
        }

    }
}


