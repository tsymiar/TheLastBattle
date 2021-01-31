using UnityEngine;
using System.Collections;
using System.Collections.Generic;

using BlGame.GameEntity;
using System.Linq;
using UICommon;
using JT.FWW.GameData;
using GameDefine;
using BlGame;
using BlGame.GameData;
using BlGame.Resource;
using BlGame.Model;
using BlGame.Ctrl;

public class UIShop : MonoBehaviour {

    public static UIShop Instance
    {
        private set;
        get;
    }
    public const int toggleGroup = 10000;
    public enum ShopSelectType
    { 
        TypeBuy,
        TypeUser,
        TypeCompose,
        TypeAfter,
    }

    enum FuncButtons
    { 
        CancelBtn,
        BuyBtn,
        SellBtn,
    }

    protected ShopSelectType SelectType;

    public Dictionary<ShopSelectType, ShopItemBase> ShopItemAll = new Dictionary<ShopSelectType, ShopItemBase>();

    public ItemDestribe itemDestribe;
    public PackageSelect Package
    {
        private set;
        get;
    }

    public int ShopId; 

    private UILabel labelCp;

    private UILabel teampCp;

    private UILabel gold;

    public static bool isRecommendEquip = false;

    /// <summary>
    /// 选中标签页
    /// </summary>
    /// <param name="page"></param>
    public void OnPageSelect(PackageSelect.ShopPage page)
    {

        if (ShopId == 0)
        {
            return;
        }
        ShopConfigInfo info = ConfigReader.ShopXmlInfoDict[ShopId];
        List<int> items = new List<int>();
        switch (page)
        {
            case PackageSelect.ShopPage.ShopPage1:
                items = info.un32ItemIDPage1;
                break;
            case PackageSelect.ShopPage.ShopPage2:
                items = info.un32ItemIDPage2;
                break;
            case PackageSelect.ShopPage.ShopPage3:
                items = info.un32ItemIDPage3;
                break;
            case PackageSelect.ShopPage.ShopPage4:
                items = info.un32ItemIDPage4;
                break;
            case PackageSelect.ShopPage.ShopPage5:
                items = info.un32ItemIDPage5;
                break;
            case PackageSelect.ShopPage.ShopPage6:
                items = GetHeroRecommendEquip();
                break;
        }
        if (items != null && items.Count != 0) {
            isRecommendEquip = false;
            if (page == PackageSelect.ShopPage.ShopPage6) {
                isRecommendEquip = true;
            }
            
            BuyPackageItem buy = (BuyPackageItem)ShopItemAll[ShopSelectType.TypeBuy];
            buy.ToggleSelectItem(items);
            this.ToggleSelectPageItem(buy, (items.Count > 0) ? items[0] : 0); 
        }

    }

    public List<int> GetHeroRecommendEquip() {
        if (PlayerManager.Instance.LocalAccount.ObType == ObPlayerOrPlayer.PlayerObType)
            return null;
        List<int> items = new List<int>();        
        Iselfplayer player = PlayerManager.Instance.LocalPlayer;
        HeroConfigInfo info = ConfigReader.GetHeroInfo(player.NpcGUIDType);

        for (int i = 0; i < 3;i++){
            switch (i)
            { 
                case 0:
                    items.AddRange(info.HeroPreEquip);
                    break;
                case 1:
                    items.AddRange(info.HeroMidEquip);
                    break;
                case 2:
                    items.AddRange(info.HeroLatEquip);
                    break;
            }
            int count = items.Count / 6;
            int tempCount = items.Count;
            if (items.Count % 6 != 0)
                count += 1;
            for (int j = 0; j < (count * 6 - tempCount); j++)
            {
                items.Add(0);
            }
        }          

        return items;
    }

    void OnEnable() {
        Instance = this;
        SendOpenShop(true);
    }

    void SendOpenShop(bool tag) {
        CEvent eve = new CEvent(EGameEvent.eGameEvent_NotifyOpenShop);
        eve.AddParam("Tag",tag);
        EventCenter.SendEvent(eve);
    }

    public void InitShop(int shopId)
    {
        ShopId = shopId;
        ShopItemAll.Add(ShopSelectType.TypeUser , new UserPackageItem(this.transform.FindChild("Backpackage")));
        ShopItemAll.Add(ShopSelectType.TypeAfter, new AfterPackageItem(this.transform.FindChild("PropertyAdvanced/Grid")));
        ShopItemAll.Add(ShopSelectType.TypeBuy , new BuyPackageItem(this.transform.FindChild("PropertySelect/Grid")));
        ShopItemAll.Add(ShopSelectType.TypeCompose , new ComposePackageItem(this.transform.FindChild("PropertyConstruct")));

        RegistFunctionButtons();
        itemDestribe = new ItemDestribe(transform.FindChild("PropertyDestribe"));
        Package = new PackageSelect(this.transform.FindChild("KindSelect"), ShopId);
        UserPackageItem use = (UserPackageItem)ShopItemAll[ShopSelectType.TypeUser];
        EventCenter.AddListener(EGameEvent.eGameEvent_UpdateUserGameItems, use.OnShowItemInUserInterface);        
         

    }

    private void RegistFunctionButtons()
    {
        UIButton mCancel = this.transform.FindChild("CancelButton").GetComponent<UIButton>();
        UIGuideCtrl.Instance.AddUiGuideEventBtn(mCancel.gameObject);

        UIButton mBuy = this.transform.FindChild("Backpackage/BuyBtn").GetComponent<UIButton>();
        EventDelegate.Add(mBuy.onClick, OnBuyButtonClick);
        UIGuideCtrl.Instance.AddUiGuideEventBtn(mBuy.gameObject);

        ButtonOnPress mSell = this.transform.FindChild("Backpackage/SellBtn").GetComponent<ButtonOnPress>();
        EventDelegate.Add(mCancel.onClick, OnShopClose);
        mSell.AddListener((int)FuncButtons.SellBtn, FeatureButtonFunc);
    }

    void OnDisable()
    {
        SendOpenShop(false);
        if (ShopItemAll.ContainsKey(ShopSelectType.TypeUser))
        {
            UserPackageItem use = (UserPackageItem)ShopItemAll[ShopSelectType.TypeUser];
            if (EventCenter.mEventTable.ContainsKey(EGameEvent.eGameEvent_UpdateUserGameItems))
            {
                EventCenter.RemoveListener(EGameEvent.eGameEvent_UpdateUserGameItems, use.OnShowItemInUserInterface);
            }
        }
       
        Instance = null;
    }


    void Awake() {
        labelCp = transform.FindChild("CPShow/CP/Label").GetComponent<UILabel>();
        gold = transform.FindChild("CPShow/Gold/Label").GetComponent<UILabel>();
        teampCp = transform.FindChild("CPShow/TeamCP/Label").GetComponent<UILabel>();
    }

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
        if (PlayerManager.Instance == null || PlayerManager.Instance.LocalPlayer == null) return;
        if (labelCp != null ) {
            labelCp.text = PlayerManager.Instance.LocalPlayer.Cp.ToString();
        }
        if (teampCp != null) {
            teampCp.text = PlayerManager.Instance.LocalPlayer.TeamCp.ToString();
        }
        if (gold != null) {
            gold.text = GameUserModel.Instance.mGameUserGold.ToString();
        }
	}


    private bool CheckCanBuy() {
        if (ShopItemBase.SelectIndex == null || ShopItemBase.SelectIndex.Count == 0)
        {
            MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_NeedAItermID);
            return false;
        }

        ShopSelectType type = ShopItemBase.SelectIndex.ElementAt(0).Key;
        int index = ShopItemBase.SelectIndex.ElementAt(0).Value;
        if (type == ShopSelectType.TypeUser)
        {
            UserPackageItem pack = (UserPackageItem)ShopItemAll[ShopSelectType.TypeUser];           
          
            int count = 0, item = -1;
            PlayerManager.Instance.LocalPlayer.UserGameItemsCount.TryGetValue(index, out count);
            PlayerManager.Instance.LocalPlayer.UserGameItems.TryGetValue(index, out item);
            if (count == 0 || !ConfigReader.ItemXmlInfoDict.ContainsKey(item))
            {
                MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_NeedAItermID);
                return false;
            }
        }
       return true;    
    }


    private void OnBuyButtonClick()
    {
        if (CheckCanBuy() && ConfigReader.ItemXmlInfoDict.ContainsKey(itemDestribe.ItemId))
        {
            this.OnUserBuyItem(itemDestribe.ItemId);
        }
    }

    private void OnShopClose()
    {
        GameObject.DestroyImmediate(gameObject);
    }

    /// <summary>
    /// 功能界面响应事件
    /// </summary>
    /// <param name="ie"></param>
    /// 响应的编号
    /// <param name="presses"></param>
    /// Is pressed
    private void FeatureButtonFunc(int ie , bool presses)
    {
        if (presses)
        {
            return;
        } 
        switch ((FuncButtons)ie)
        { 
            case FuncButtons.SellBtn:
                this.OnUserSellItem();
                break;
        }
    }

    /// <summary>
    /// 购买物品
    /// </summary>
    /// <param name="item"></param>
    public void OnUserBuyItem(int item)
    {        
        CGLCtrl_GameLogic.Instance.EmsgToss_AskBuyGoods(item);
    }

    /// <summary>
    /// 出售物品
    /// </summary>
    /// <param name="item"></param>
    public void OnUserSellItem()
    {
        bool isEmpty = true;
        UserPackageItem pack = (UserPackageItem)ShopItemAll[ShopSelectType.TypeUser];
        foreach (var itemCount in PlayerManager.Instance.LocalPlayer.UserGameItemsCount.Values) {
            if (itemCount != 0)
            {
                isEmpty = false;
                break;
            }
        }

        if (isEmpty)
        {
            MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_BagIsEmpty);
            pack.SelectSelfCloseOtherSelect(pack, -1);
            return;//如果道具个数为空
        }

        if (ShopItemBase.SelectIndex == null || ShopItemBase.SelectIndex.Count == 0)
        {
            MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_NeedABagID);
            return;
        }

        ShopSelectType type = ShopItemBase.SelectIndex.ElementAt(0).Key;
        int index = ShopItemBase.SelectIndex.ElementAt(0).Value;
        if (type != ShopSelectType.TypeUser)
        {
            MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_NeedABagID);
            //GameMethod.DebugError("aaaaa");
            return;
        }
        int count = 0,item = -1;
        PlayerManager.Instance.LocalPlayer.UserGameItemsCount.TryGetValue(index,out count);
        PlayerManager.Instance.LocalPlayer.UserGameItems.TryGetValue(index,out item);
        if (count != 0 && ConfigReader.ItemXmlInfoDict.ContainsKey(item))
        {
            CGLCtrl_GameLogic.Instance.EmsgToss_AskSellGoods(index);
        }
        else {
            MsgInfoManager.Instance.ShowMsg((int)ERROR_TYPE.eT_NeedABagID);
            ShopItemBase.SelectIndex.Clear();
        }
    }

    /// <summary>
    /// 显示商店选中的物品信息
    /// </summary>
    /// <param name="item"></param>
    /// 选中的物品Id
    public void ToggleSelectPageItem(ShopItemBase from, int item)
    {
        ShopItemAll[ShopSelectType.TypeCompose].ToggleSelectItem(from,item);
        ShopItemAll[ShopSelectType.TypeAfter].ToggleSelectItem(from,item);
        ShopItemAll[ShopSelectType.TypeUser].ToggleSelectItem(from,item);
    }
 
}

//******************************************###########################  角色物品栏   ############################*******************************************
//******************************************###########################  角色物品栏   ############################*******************************************
//******************************************###########################  角色物品栏   ############################*******************************************

public class UserPackageItem : ShopItemBase
{
    private int UserItemBtnCount = 6;
    private Iselfplayer localPlayer;   

    
    public UserPackageItem(Transform head)  : 
        base(head)
    {
        ShopType = UIShop.ShopSelectType.TypeUser;
       // Clear();
        localPlayer = PlayerManager.Instance.LocalPlayer;
        OnShowItemInUserInterface(); 
       
    }

    public override void ToggleSelectItem(ShopItemBase from,int item)
    {
     //   ItemsInUserInterface = PlayerManager.Instance.LocalPlayer.UserGameItems;
      //  this.OnShowItemInUserInterface();
    }

    

    protected override void AddButtonListener(Transform head)
    { 
        for (int ct = 0; ct < UserItemBtnCount; ct++)
        {
            int ctt = ct + 1;
            DbClickBotton click = head.FindChild("Item" + ctt.ToString()).GetComponent<DbClickBotton>();
            click.AddListener(ct, OnButtonClickFunc);
            click.AddListenerDb(ct, OnButtolDbClickFunc);
            ButtonUis.Add(click);
            
            UIObjDragEvent drag = click.GetComponent<UIObjDragEvent>();
            drag.ObjDragEvent += OnItemDragEvent;  
        }
         
    }

     void OnItemDragEvent(GameObject obj, UIObjDragEvent.DragState state, Vector2 pos)
    {        
        switch (state) {
            case UIObjDragEvent.DragState.DragMove:
                ItemMove(obj, pos);
                break;
            case UIObjDragEvent.DragState.InvalideDrag:
                if (objMove != null)
                {
                    objMove.gameObject.SetActive(false);
                    objMove.transform.localPosition = Vector3.zero;
                }
              //  int index = GetIndexOfItem(obj); 
                OnShowItemInUserInterface(); 
                isMove = false;
                break;
            case UIObjDragEvent.DragState.LongPressStart: 
                ItemStartMove(obj); 
                break;
            case UIObjDragEvent.DragState.DragEnd:
                ItemMoveEnd(obj,pos); 
                break;
        }
    }

     void SetItemEnableByIndex(int index, bool enable)
     {
         if (index < 0 || index >= ButtonUis.Count) return;
         DbClickBotton click = ButtonUis.ElementAt(index);
         UISprite sprite = click.transform.FindChild("icon").GetComponent<UISprite>();
         UILabel label = click.transform.FindChild("Label").GetComponent<UILabel>();
         sprite.gameObject.SetActive(enable);
         label.gameObject.SetActive(enable);
     }

     int GetIndexOfItem(GameObject obj)
     {
         for (int i = 0; i < 6; i++)
         {
             if (ButtonUis.ElementAt(i).gameObject == obj)
                 return i;
         }
         return -1;
     }

     int CheckInBackPackage()
     {
         if (objMove == null) return -1;
         if (UICamera.currentTouch.current == null) return -1;
         for (int i = 0; i < ButtonUis.Count; i++)
         {
             if (ButtonUis.ElementAt(i).gameObject == UICamera.currentTouch.current)
                 return i;
         }
         return -1;
     }

     void ItemStartMove(GameObject obj)
     {     
          
         if (isMove) return;
         isMove = true;

         int index = GetIndexOfItem(obj);
         SetItemEnableByIndex(index, false);
         objMove = obj.transform.FindChild("SpriteMove").gameObject;
         UISprite sprite = objMove.GetComponent<UISprite>();
         UISprite spriteIcon = obj.transform.FindChild("icon").GetComponent<UISprite>();
         sprite.spriteName = spriteIcon.spriteName;
         objMove.gameObject.SetActive(true);
     }


     GameObject objMove = null;
     bool isMove = false;
     void ItemMove(GameObject obj, Vector2 pos)
     {
         if (objMove == null || !isMove)
         {
             return;
         }
         Vector3 newPos = UICommonMethod.GetWorldPos(pos);
         objMove.transform.position = newPos;
     }



     void ItemMoveEnd(GameObject obj, Vector2 pos)
     {
         if (objMove == null || !isMove)
         {
             return;
         }
         int index = GetIndexOfItem(obj);

      
         if (objMove != null)
         {
             objMove.gameObject.SetActive(false);
             objMove.transform.localPosition = Vector3.zero;             
         }

         int dst = CheckInBackPackage();

         if (dst != -1 && index != -1 && dst != index)
         {
             SelectSelfCloseOtherSelect(this, -1);
             CGLCtrl_GameLogic.Instance.EmsgToss_AskMoveGoods(index, dst);             
         }
         else
         {
             OnShowItemInUserInterface(); 
             SelectSelfCloseOtherSelect(this, -1);
         }
         if (SelectIndex != null && SelectIndex.Count != 0 && SelectIndex.ContainsKey(UIShop.ShopSelectType.TypeUser))
         {
             SelectIndex.Clear();
         }
         isMove = false;
         objMove = null;
     }

    /// <summary>
    /// 刷新显示部分
    /// </summary>
    public override void OnShowItemInUserInterface()
    {

        for (int ct = 0; ct < UserItemBtnCount; ct++)
        {
            int itemId = -1;
            DbClickBotton click = ButtonUis.ElementAt(ct);
            UISprite sprite = click.transform.FindChild("icon").GetComponent<UISprite>();
            localPlayer.UserGameItems.TryGetValue(ct,out itemId);
            int count = 0;
            localPlayer.UserGameItemsCount.TryGetValue(ct, out count);
            UILabel label = click.transform.FindChild("Label").GetComponent<UILabel>();
            if (count == 0 || !ConfigReader.ItemXmlInfoDict.ContainsKey(itemId))
            {
                sprite.gameObject.SetActive(false); 
                label.gameObject.SetActive(false);
                label.text = "";
                click.GetComponent<UIObjDragEvent>().enabled = false; 
                SetSelectSpriteVisiable(ct, false);
                continue;
            }
            click.GetComponent<UIObjDragEvent>().enabled = true; 
            sprite.gameObject.SetActive(true);
            sprite.spriteName = ConfigReader.ItemXmlInfoDict[itemId].sIcon;
           
           
            if (ConfigReader.ItemXmlInfoDict[itemId].un8OverlapTimes >= 2 && count > 1)
            {
                label.gameObject.SetActive(true);
                label.text = count.ToString();
            }
            else {
                label.gameObject.SetActive(false);
                label.text = "";
            }
        }
         
    }


    protected override void  SetSelectSpriteVisiable(int index, bool visiable)
    {
        if (ButtonUis.Count <= index || index < 0) return;
        UISprite sprite = ButtonUis.ElementAt(index).transform.FindChild("SpriteSelect").GetComponent<UISprite>();
        sprite.gameObject.SetActive(visiable);        
    }

    /// <summary>
    /// 重载双击事件、个人物品栏有可能位置空
    /// </summary>
    /// <param name="ie"></param>
    protected override void OnButtolDbClickFunc(int ie)
    {
        //int count = 0;
        //int itemId = -1; 
        //PlayerManager.Instance.LocalPlayer.UserGameItemsCount.TryGetValue(ie,out count);
        //PlayerManager.Instance.LocalPlayer.UserGameItems.TryGetValue(ie,out itemId);
        //if (ConfigReader.ItemXmlInfoDict.ContainsKey(itemId) && count != 0)
        //{
        //    UIShop.Instance.OnUserSellItem();
        //}
    }

    protected override void OnButtonClickFunc(int ie)
    {           
        int count = 0;
        int itemId = -1; 
        PlayerManager.Instance.LocalPlayer.UserGameItemsCount.TryGetValue(ie,out count);
        PlayerManager.Instance.LocalPlayer.UserGameItems.TryGetValue(ie,out itemId);
        if (ConfigReader.ItemXmlInfoDict.ContainsKey(itemId) && count != 0)
        {
            UIShop.Instance.itemDestribe.ShowItemDestribe(itemId);
            UIShop.Instance.ToggleSelectPageItem(this, itemId);
            GobalSelectSelfCloseOtherSelect(this, ie);
            SelectIndex.Clear();
            SelectIndex.Add(UIShop.ShopSelectType.TypeUser,ie);
        }     
        
    }
}

//******************************************###########################  物品购买   ############################*******************************************
//******************************************###########################  物品购买   ############################*******************************************
//******************************************###########################  物品购买   ############################*******************************************

public class BuyPackageItem : ShopItemBase
{
    private List<int> ItemBuyList;
    private UIScrollView ScView;
    private UICenterTurnPage turnPage;

    private Transform buyPackageHead;
    Vector3 orignalPos = Vector3.zero;
    Vector4 orignalRangle = new Vector4();
    Vector2 orginalOffset = new Vector2();

    public Transform[] arrowUpDown = new Transform[2];
    public BuyPackageItem(Transform head) :
        base(head)
    {
        ScView = head.parent.GetComponent<UIScrollView>(); 
        ShopType = UIShop.ShopSelectType.TypeBuy;
        buyPackageHead = head;
        orignalPos = ScView.transform.localPosition;
        orignalRangle = ScView.GetComponent<UIPanel>().baseClipRegion;
        orginalOffset = ScView.GetComponent<UIPanel>().clipOffset;
        Transform btnParent = UIShop.Instance.transform.FindChild("SelectBG/InfoPageCtrl");
        for (int i = 0; i < 2; i++) {
            arrowUpDown[i] = btnParent.FindChild("Btn" + (i + 1).ToString());
        }
        arrowUpDown[0].gameObject.SetActive(false);
        arrowUpDown[1].gameObject.SetActive(true);

        //senlin
        ScView.onDragStarted += OnDragFinished;
        turnPage = ScView.GetComponentInChildren<UICenterTurnPage>();
        turnPage.nextPageThreshold = 50f;
    }
    void OnDragFinished()
    {
        Vector3 constraint = ScView.panel.CalculateConstrainOffset(ScView.bounds.min, ScView.bounds.max);
        if (constraint.magnitude > 0.5f)
        {
            if (constraint.y > 0f)//上到头
            {
                arrowUpDown[0].gameObject.SetActive(false);
                arrowUpDown[1].gameObject.SetActive(true);
            }
            else//左到头
            {
                arrowUpDown[0].gameObject.SetActive(true);
                arrowUpDown[1].gameObject.SetActive(false);
            }
        }
        else
        {
            arrowUpDown[0].gameObject.SetActive(true);
            arrowUpDown[1].gameObject.SetActive(true);
        }
    }
    public void ToggleSelectItem(List<int> items)
    {
        arrowUpDown[0].gameObject.SetActive(false);
        arrowUpDown[1].gameObject.SetActive(true);
        //ScView.enabled = false;
        //buyPackageHead.transform.parent.localPosition = orignalPos;
        //ScView.GetComponent<UIPanel>().baseClipRegion = orignalRangle;
        //ScView.GetComponent<UIPanel>().clipOffset = orginalOffset;
        //ScView.enabled = true;
        ScView.ResetPosition();
        ScView.enabled = false;
        buyPackageHead.transform.parent.localPosition = orignalPos;
        ScView.GetComponent<UIPanel>().baseClipRegion = orignalRangle;
        ScView.GetComponent<UIPanel>().clipOffset = orginalOffset;
        ScView.enabled = true;
        ItemsInUserInterface.Clear();

        ItemBuyList = items; 
        ItemsInUserInterface = SortItemByPrice(items);
         
        UIShop.Instance.itemDestribe.ShowItemDestribe(items.ElementAt(0));
        AutoCreateItems(ItemsInUserInterface.Count, buyPackageHead,GameDefine.GameConstDefine.BuyPackItemPath);
        this.OnShowItemInUserInterface();
        OnButtonClickFunc(0);
        turnPage.Reset();
    }


    //int SortItem(int a, int b) {

    //    ItemConfigInfo infoA = ConfigReader.ItemXmlInfoDict[a];
    //    ItemConfigInfo infoB = ConfigReader.ItemXmlInfoDict[b];
    //    if (infoA.n32CPCost < infoB.n32CPCost)
    //    {
    //        return -1;
    //    }
    //    else if (infoA.n32CPCost > infoB.n32CPCost)
    //    {
    //        return 1;
    //    }
    //    Debug.LogError("infoA.n32CPCost = " + infoA.GoodsId);
    //    Debug.LogError("infoB.n32CPCost = " + infoB.GoodsId);
    //    return -1;
    //}

    List<int> SortItemByPrice(List<int> itemList)
    {
        List<int> itemReturn = new List<int>();
        itemReturn.AddRange(itemList);
        for (int i = 0; i < itemReturn.Count; i++) {
            for (int j = 0; j < itemReturn.Count - 1 - i; j++) {
                if (!ConfigReader.ItemXmlInfoDict.ContainsKey(itemReturn[j]) || !ConfigReader.ItemXmlInfoDict.ContainsKey(itemReturn[j + 1]))
                    continue;
                ItemConfigInfo infoA = ConfigReader.ItemXmlInfoDict[itemReturn[j]];
                ItemConfigInfo infoB = ConfigReader.ItemXmlInfoDict[itemReturn[j+1]];
                if (infoA.n32CPCost > infoB.n32CPCost)
                {
                    int temp = itemReturn[j];
                    itemReturn[j] = itemReturn[j + 1];
                    itemReturn[j + 1] = temp;
                }
            }
        }
        return itemReturn;
    }

    protected override void SetSelectSpriteVisiable(int index, bool visiable)
    {
        if (ButtonUis.Count <= index || index < 0) return;
        UISprite sprite = ButtonUis.ElementAt(index).transform.FindChild("Item/SpriteSelect").GetComponent<UISprite>();
        sprite.gameObject.SetActive(visiable);
    }    

    protected override void OnButtonClickFunc(int ie)
    { 
        if (ie >= ItemsInUserInterface.Count)
        {
            return;
        }
        GobalSelectSelfCloseOtherSelect(this, ie);
        int item = ItemsInUserInterface[ie];
        UIShop.Instance.itemDestribe.ShowItemDestribe(item);
        UIShop.Instance.ToggleSelectPageItem(this, ItemsInUserInterface[ie]);
        SelectIndex.Clear();
        SelectIndex.Add(UIShop.ShopSelectType.TypeBuy, ie);
    }
    /// <summary>
    /// 刷新显示部分
    /// </summary>
    public override void OnShowItemInUserInterface()
    {
        for (int ct = 0; ct < ButtonUis.Count; ct++)
        {
            DbClickBotton click = ButtonUis[ct];
            UISprite sprite = click.transform.FindChild("Item/icon").GetComponent<UISprite>();
          
            UILabel price = click.transform.FindChild("Price/Label").GetComponent<UILabel>();
            if (ct >= ItemsInUserInterface.Count ) 
            { 
                //click.gameObject.SetActive(false); 
                //click.GetComponent<BoxCollider>().enabled = false;
                DisableGameObjectSprites(click.gameObject);
                SetSelectSpriteVisiable(ct, false);
                continue;
            }
            int item = ItemsInUserInterface[ct];
            if (!ConfigReader.ItemXmlInfoDict.ContainsKey(item)) {
                //click.gameObject.SetActive(false); 
                //click.GetComponent<BoxCollider>().enabled = false;
                DisableGameObjectSprites(click.gameObject);
                SetSelectSpriteVisiable(ct, false);
                continue;
            }
            click.GetComponent<BoxCollider>().enabled = true;
            click.gameObject.SetActive(true);
            price.text = ConfigReader.ItemXmlInfoDict[item].n32CPCost.ToString();
            sprite.gameObject.SetActive(true);
            sprite.spriteName = ConfigReader.ItemXmlInfoDict[item].sIcon;
            if (UIShop.isRecommendEquip && PlayerManager.Instance.LocalAccount.ObType == ObPlayerOrPlayer.PlayerType) {                
                Transform parentKind = click.gameObject.transform.FindChild("Recommend");
                HeroConfigInfo info = ConfigReader.GetHeroInfo(PlayerManager.Instance.LocalPlayer.NpcGUIDType);
				//策划说，初级推荐装备，中级，高级都小于6个  
				int index = (info.HeroPreEquip.Count == 0) ? 0 : (info.HeroPreEquip.Count / 6 + 1) * 6;
				int index2 = (info.HeroMidEquip.Count == 0) ? 0 : (info.HeroMidEquip.Count / 6 + 1) * 6 ;
				if (info.HeroPreEquip.Contains(item) && ct < (info.HeroPreEquip.Count / 6 + 1) * 6 ) {
                    parentKind.FindChild("Early").gameObject.SetActive(true);
                }
				else if (info.HeroMidEquip.Contains(item) && ct < (info.HeroMidEquip.Count / 6 + 1) * 6 + index ){
                    parentKind.FindChild("Medium").gameObject.SetActive(true);
                }
				else if (info.HeroLatEquip.Contains(item) && ct < (info.HeroLatEquip.Count / 6 + 1) * 6 + index+ index2) {
                    parentKind.FindChild("Later").gameObject.SetActive(true);
                }
            }
        } 
    }

    private void DisableGameObjectSprites(GameObject obj) {
        UISprite[] sprites = obj.GetComponentsInChildren<UISprite>();
        for (int i = 0; i < sprites.Length; i++)
        {
            sprites[i].gameObject.SetActive(false);
        }

        UILabel[] labels = obj.GetComponentsInChildren<UILabel>();
        for (int i = 0; i < labels.Length; i++)
        {
            labels[i].gameObject.SetActive(false);
        }

        DbClickBotton dbClick = obj.GetComponent<DbClickBotton>();
        if (dbClick != null)
        {
            dbClick.RemoveListener(OnButtonClickFunc);
            dbClick.RemoveListenerDb(OnButtolDbClickFunc);
        }
        obj.AddComponent<UIWidget>();
    }
 
    /// <summary>
    /// 按钮双击事件
    /// </summary>
    /// <param name="ie"></param>
    protected override void OnButtolDbClickFunc(int ie)
    { 
        if (ie >= ItemsInUserInterface.Count)
        {
            return;
        }  
        UIShop.Instance.OnUserBuyItem(ItemsInUserInterface[ie]);
    }
}

//******************************************###########################  合成的物品   ############################*******************************************
//******************************************###########################  合成的物品   ############################*******************************************
//******************************************###########################  合成的物品   ############################*******************************************


public class ComposePackageItem : ShopItemBase
{
    private int ComposeBtnCount = 7;

    public ComposePackageItem(Transform head) :
        base(head)
    {
        ShopType = UIShop.ShopSelectType.TypeCompose;
    }

    public override void ToggleSelectItem(ShopItemBase from,int item)
    {
        if (from == this) return;
      
        ItemsInUserInterface.Clear();
        if (item == 0 || !ConfigReader.CombineXmlInfoDict.ContainsKey(item))
        {
            this.OnShowItemInUserInterface();
            return;
        }
        SelectSelfCloseOtherSelect(this, -1);

        ItemsInUserInterface.Add(item);
        if (item != 0 && ConfigReader.CombineXmlInfoDict[item].un32ChildID1 != 0 && ConfigReader.CombineXmlInfoDict[item].un32ChildID2 != 0)
        {
            ItemsInUserInterface.Add(ConfigReader.CombineXmlInfoDict[item].un32ChildID1);
            ItemsInUserInterface.Add(ConfigReader.CombineXmlInfoDict[item].un32ChildID2);
			ItemsInUserInterface.Add(ConfigReader.CombineXmlInfoDict[ItemsInUserInterface[1]].un32ChildID1);
			ItemsInUserInterface.Add(ConfigReader.CombineXmlInfoDict[ItemsInUserInterface[1]].un32ChildID2);
			ItemsInUserInterface.Add(ConfigReader.CombineXmlInfoDict[ItemsInUserInterface[2]].un32ChildID1);
			ItemsInUserInterface.Add(ConfigReader.CombineXmlInfoDict[ItemsInUserInterface[2]].un32ChildID2); 
        }        
        this.OnShowItemInUserInterface();   
    }

    protected override void AddButtonListener(Transform head)
    {
        for (int ct = 0; ct < ComposeBtnCount; ct++)
        {
            int ctt = ct + 1;
            DbClickBotton click = head.FindChild("Button" + ctt.ToString()).GetComponent<DbClickBotton>();
            click.AddListener(ct, OnButtonClickFunc);
            click.AddListenerDb(ct, OnButtolDbClickFunc);
            ButtonUis.Add(click);
        }
    }

    /// <summary>
    /// 刷新显示部分
    /// </summary>
    public override void OnShowItemInUserInterface()
    { 

        for (int ct = 0; ct < ButtonUis.Count; ct++)
        {
            DbClickBotton click = ButtonUis[ct];
            UISprite sprite = click.transform.FindChild("icon").GetComponent<UISprite>();
           
            if (ct >= ItemsInUserInterface.Count)
            {
                sprite.gameObject.SetActive(false); 
                click.GetComponent<BoxCollider>().enabled = false;
                SetSelectSpriteVisiable(ct, false);
                continue;
            }

            int item = ItemsInUserInterface[ct];
			if (item == 0 || !ConfigReader.ItemXmlInfoDict.ContainsKey(item)) {
                sprite.gameObject.SetActive(false); 
                click.GetComponent<BoxCollider>().enabled = false;
                SetSelectSpriteVisiable(ct,false);
                continue;
            }
            click.GetComponent<BoxCollider>().enabled = true;
            sprite.gameObject.SetActive(true);
            sprite.spriteName = ConfigReader.ItemXmlInfoDict[item].sIcon;
        }
    }

    protected override void SetSelectSpriteVisiable(int index, bool visiable)
    {
        if (ButtonUis.Count <= index || index < 0) return;
        UISprite sprite = ButtonUis.ElementAt(index).transform.FindChild("SpriteSelect").GetComponent<UISprite>();
        sprite.gameObject.SetActive(visiable);
    }

    protected override void OnButtolDbClickFunc(int ie)
    { 
        if (ie >= ItemsInUserInterface.Count)
        {
            return;
        }
        UIShop.Instance.OnUserBuyItem(ItemsInUserInterface[ie]);
    }

    protected override void OnButtonClickFunc(int ie)
    {       
      
        if (ie >= ItemsInUserInterface.Count)
        {
            return;
        }
        GobalSelectSelfCloseOtherSelect(this,ie);
        UIShop.Instance.ToggleSelectPageItem(this,ItemsInUserInterface[ie]);
        int item = ItemsInUserInterface[ie];
        UIShop.Instance.itemDestribe.ShowItemDestribe(item);
        SelectIndex.Clear();
        SelectIndex.Add(UIShop.ShopSelectType.TypeCompose, ie);
    }

     
}

//******************************************###########################  可合成装备   ############################*******************************************
//******************************************###########################  可合成装备   ############################*******************************************
//******************************************###########################  可合成装备   ############################*******************************************

public class AfterPackageItem : ShopItemBase
{
    private Transform afterPackHead;
    public  AfterPackageItem(Transform head)
        :base(head)
    {
        afterPackHead = head;
        ShopType = UIShop.ShopSelectType.TypeAfter;
        
    }

    public override void ToggleSelectItem(ShopItemBase from,int item)
    {
        if (from == this) return;
        ItemsInUserInterface.Clear();
        if (item == 0)
        {
            this.OnShowItemInUserInterface();
            return;
        }       

        foreach (var it in ConfigReader.CombineXmlInfoDict)
        {
            if (it.Value.un32ChildID1 == item || it.Value.un32ChildID2 == item)
            {
                ItemsInUserInterface.Add(it.Key);
            }
        }

        AutoCreateItems(ItemsInUserInterface.Count, afterPackHead, GameDefine.GameConstDefine.AfterPackItemPath);        
        this.OnShowItemInUserInterface();
    }

    protected override void SetSelectSpriteVisiable(int index, bool visiable)
    {
        if (ButtonUis.Count <= index || index < 0) return;
        UISprite sprite = ButtonUis.ElementAt(index).transform.FindChild("SpriteSelect").GetComponent<UISprite>();
        sprite.gameObject.SetActive(visiable);
    }

    protected override void OnButtolDbClickFunc(int ie)
    { 
        if (ie >= ItemsInUserInterface.Count)
        {
            return;
        }
        UIShop.Instance.OnUserBuyItem(ItemsInUserInterface[ie]);
    }


    protected override void OnButtonClickFunc(int ie)
    {               
        if (ie >= ItemsInUserInterface.Count || ie == -1)
        {
            return;
        }
        GobalSelectSelfCloseOtherSelect(this,ie);
        UIShop.Instance.ToggleSelectPageItem(this,ItemsInUserInterface[ie]);
        int item = ItemsInUserInterface[ie];
        UIShop.Instance.itemDestribe.ShowItemDestribe(item);
        SelectIndex.Clear();
        SelectIndex.Add(UIShop.ShopSelectType.TypeAfter, ie);
    }
    /// <summary>
    /// 刷新显示部分
    /// </summary>
    public override void OnShowItemInUserInterface()
    {
        for (int ct = 0; ct < ButtonUis.Count; ct++)
        {
            DbClickBotton click = ButtonUis[ct];
            UISprite sprite = click.transform.FindChild("icon").GetComponent<UISprite>();
           
            if (ct >= ItemsInUserInterface.Count)
            {
                sprite.gameObject.SetActive(false); 
                click.GetComponent<BoxCollider>().enabled = false;
                SetSelectSpriteVisiable(ct, false);
                continue;
            }
            int item = ItemsInUserInterface[ct];
            if (!ConfigReader.ItemXmlInfoDict.ContainsKey(item)) {
                sprite.gameObject.SetActive(false);
                SetSelectSpriteVisiable(ct, false);
                click.GetComponent<BoxCollider>().enabled = false; 
                continue;
            }
            click.GetComponent<BoxCollider>().enabled = true; 
            sprite.gameObject.SetActive(true);
            sprite.spriteName = ConfigReader.ItemXmlInfoDict[item].sIcon;
        }
    }

}


//******************************************###########################  标签页选择   ############################*******************************************
//******************************************###########################  标签页选择   ############################*******************************************
//******************************************###########################  标签页选择   ############################*******************************************


public class PackageSelect
{
    public enum ShopPage
    { 
        ShopPage1 = 1,
        ShopPage2,
        ShopPage3,
        ShopPage4,
        ShopPage5,
        ShopPage6,
    }

    private List<ButtonOnPress> PageButton = new List<ButtonOnPress>();

    private int PackageBtnCount = 6;

    public ShopPage Page
    {
        get;
        private set;
    }

    public PackageSelect(Transform head,int id)
    {
        AddButtonListener(head,id);
    }

    private void AddButtonListener(Transform head,int shopid)
    {
        ShopConfigInfo info = ConfigReader.ShopXmlInfoDict[shopid];
       
        List<int> items = new List<int>();
        for (int ct = 0; ct < PackageBtnCount; ct++)        {
            int ctt = ct + 1;       
            ShopPage page = (ShopPage)ctt;
            switch (page)
            {
                case PackageSelect.ShopPage.ShopPage1:
                    items = info.un32ItemIDPage1;
                    break;
                case PackageSelect.ShopPage.ShopPage2:
                    items = info.un32ItemIDPage2;
                    break;
                case PackageSelect.ShopPage.ShopPage3:
                    items = info.un32ItemIDPage3;
                    break;
                case PackageSelect.ShopPage.ShopPage4:
                    items = info.un32ItemIDPage4;
                    break;
                case PackageSelect.ShopPage.ShopPage5:
                    items = info.un32ItemIDPage5;
                    break;
                case PackageSelect.ShopPage.ShopPage6:
                    items = UIShop.Instance.GetHeroRecommendEquip();
                    break;
            }
            ButtonOnPress click = head.FindChild("Kind" + ctt.ToString()).GetComponent<ButtonOnPress>();
            if (items == null || items.Count == 0)
            {
                click.GetComponent<BoxCollider>().enabled = false;
                continue;
            }
            click.AddListener(ctt , KindSelectFunc,ButtonOnPress.EventType.PressType);
            PageButton.Add(click);
        }
    }

    public void PageButtonToggle(int index)
    {
        Page = (ShopPage)index;
        foreach (ButtonOnPress btn in PageButton)
        {
            if (btn.PrIe == index)
            {
                btn.transform.FindChild("SpriteSelect").gameObject.SetActive(true);
                continue;
            }
            btn.transform.FindChild("SpriteSelect").gameObject.SetActive(false);
        }

        BuyPackageItem buyPack = (BuyPackageItem)UIShop.Instance.ShopItemAll[UIShop.ShopSelectType.TypeBuy];
        buyPack.arrowUpDown[0].gameObject.SetActive(false);
        buyPack.arrowUpDown[1].gameObject.SetActive(true);
    }

    public void KindSelectFunc(int ie , bool pressed)
    {
        if (!pressed || Page == (ShopPage)ie)
        {
            return;
        }        
        PageButtonToggle(ie);
        UIShop.Instance.OnPageSelect(Page);
    }
}


//******************************************###########################  ITEMBASE   ############################*******************************************
//******************************************###########################  ITEMBASE   ############################*******************************************
//******************************************###########################  ITEMBASE   ############################*******************************************

public class ShopItemBase
{
    protected List<DbClickBotton> ButtonUis { set; get; }
    protected List<int> ItemsInUserInterface { set; get; }  
    public UIShop.ShopSelectType ShopType
    {
        protected set;
        get;
    }
    public static Dictionary<UIShop.ShopSelectType, int> SelectIndex = new Dictionary<UIShop.ShopSelectType, int>();
    public ShopItemBase(Transform head)
    {
        ButtonUis = new List<DbClickBotton>();
        ItemsInUserInterface = new List<int>();
        AddButtonListener(head); 
    }
    
    /// <summary>
    /// 重置物品选择
    /// </summary>
    /// <param name="item"></param>
    /// 物品Id
    public virtual void ToggleSelectItem(ShopItemBase from, int item) { }

    /// <summary>
    /// 添加按钮时间监听
    /// </summary>
    /// <param name="head"></param>
    protected virtual void AddButtonListener(Transform head) { }

    /// <summary>
    /// 物品按钮点击事件
    /// </summary>
    /// <param name="ie"></param>
    protected virtual void OnButtonClickFunc(int ie) 
    {
    }

    /// <summary>
    /// 物品双击事件
    /// </summary>
    /// <param name="ie"></param>
    protected virtual void OnButtolDbClickFunc(int ie)
    {
        if (ie >= ItemsInUserInterface.Count)
        {
            return;
        }
    }

    /// <summary>
    /// 数据刷新之后的显示部分
    /// </summary>
    public virtual void OnShowItemInUserInterface()
    {
    }
 

    /// <summary>
    /// 通过索引获取物品ID
    /// </summary>
    /// <param name="ie"></param>
    /// <returns></returns>
    public virtual int GetShopItem(int ie) 
    {
        if (ie >= ItemsInUserInterface.Count || ie < 0)
        {
            return 0;
        }
        return ItemsInUserInterface[ie];
    }

    /// <summary>
    /// 动态创建可滑动的预设
    /// </summary>
    /// <param name="count"></param>
    /// <param name="head"></param>
    /// <param name="path"></param>
    protected void AutoCreateItems(int count,Transform head,string path) {
        for (int i = ButtonUis.Count - 1; i >= 0; i--) {
            GameObject.DestroyImmediate(ButtonUis.ElementAt(i).gameObject);
        }

        ButtonUis.Clear();
        int index = count / 6;
        if (count % 6 != 0)
        {
            index += 1;
        }
        for (int i = 0; i < index * 6; i++)
        {
           ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate(path, ResourceType.PREFAB);
            GameObject obj = GameObject.Instantiate(objUnit.Asset) as GameObject;
            obj.name = obj.name + (i+1).ToString();
            obj.transform.parent = head;
            obj.transform.localScale = Vector3.one;
            //obj.transform.localPosition = Vector3.zero;
            DbClickBotton click = obj.GetComponent<DbClickBotton>();
            click.AddListener(i, OnButtonClickFunc);            
            click.AddListenerDb (i,OnButtolDbClickFunc);
            ButtonUis.Add(click);
            //click.transform.GetComponent<UIGrid>().Reposition() ;
        }
        UIGrid grid = head.GetComponentInChildren<UIGrid>();
        grid.enabled = true;
        grid.repositionNow = true;
        grid.Reposition();     
    }

    
    
    /// <summary>
    /// 设置某一个选项是否可视
    /// </summary>
    /// <param name="index"></param>
    /// <param name="visiable"></param>
    protected virtual void SetSelectSpriteVisiable(int index,bool visiable) {
        
    }


    public virtual void SelectSelfCloseOtherSelect(ShopItemBase type,int index) {
        if (type != this)
            index = -1;
        for (int i = 0; i < ButtonUis.Count; i++) {
            if (i == index)
            {
                SetSelectSpriteVisiable(i, true);
            }
            else {
                SetSelectSpriteVisiable(i, false);
            }
        }
    }

    protected void GobalSelectSelfCloseOtherSelect(ShopItemBase type, int index)
    {
        UIShop.Instance.ShopItemAll[UIShop.ShopSelectType.TypeAfter].SelectSelfCloseOtherSelect(type,index);
        UIShop.Instance.ShopItemAll[UIShop.ShopSelectType.TypeBuy].SelectSelfCloseOtherSelect(type, index);
        UIShop.Instance.ShopItemAll[UIShop.ShopSelectType.TypeCompose].SelectSelfCloseOtherSelect(type, index);
        UIShop.Instance.ShopItemAll[UIShop.ShopSelectType.TypeUser].SelectSelfCloseOtherSelect(type, index);
    }
   
  

}


//******************************************###########################  道具描述   ############################*******************************************
//******************************************###########################  道具描述   ############################*******************************************
//******************************************###########################  道具描述   ############################*******************************************
public class ItemDestribe
{
    UISprite itemIcon;
    UILabel itemName;
    UILabel itemMoney;    
    UITextList itemDes;

    public int ItemId
    {
        private set;
        get;
    }
    public ItemDestribe(Transform head)
    {
        itemIcon = head.FindChild("Item/Icon").GetComponent<UISprite>();
        itemName = head.FindChild("Item/Label").GetComponent<UILabel>();
        itemMoney = head.FindChild("Gold/Label").GetComponent<UILabel>();
        itemDes = head.GetComponent<UITextList>();
        ItemId = -1;
    }

    public void ShowItemDestribe(int itemId)
    {
        itemDes.Clear();
        if (!ConfigReader.ItemXmlInfoDict.ContainsKey(itemId)) return;

        ItemConfigInfo info = ConfigReader.ItemXmlInfoDict[itemId];
        itemIcon.spriteName = info.sIcon;
        itemName.text = info.sNameCh;      
        itemDes.Add(info.sDescribe);
        ItemId = itemId;
        ShowMoney(itemId);
    }

    private void ShowMoney(int itemId) {
        if (!ConfigReader.CombineXmlInfoDict.ContainsKey(itemId) || !ConfigReader.ItemXmlInfoDict.ContainsKey(itemId))
        {
            return;
        }
        CombineConfigInfo comInfo = ConfigReader.CombineXmlInfoDict[itemId];
        ItemConfigInfo info = ConfigReader.ItemXmlInfoDict[itemId];
        if (comInfo.un32ChildID1 == 0 || comInfo.un32ChildID2 == 0)
        {
            itemMoney.text = info.n32CPCost.ToString();
        }
        else
        {
            float moneyCut = info.n32CPCost;
            CutComMoney(ref moneyCut, itemId);
            itemMoney.text = moneyCut.ToString();
        }
    }

    private void CutComMoney(ref float money,int itemId) {
        if (!ConfigReader.CombineXmlInfoDict.ContainsKey(itemId) || !ConfigReader.ItemXmlInfoDict.ContainsKey(itemId))
        {
            return;
        }
        CombineConfigInfo comInfo = ConfigReader.CombineXmlInfoDict[itemId];
        ItemConfigInfo info = ConfigReader.ItemXmlInfoDict[itemId];
        if (comInfo.un32ChildID1 == 0 || comInfo.un32ChildID2 == 0) {
            return;
        }

        if (ContainAsignedItems(comInfo.un32ChildID1).Count != 0)
        {
            money -= GetAsignedItemsMoney(comInfo.un32ChildID1);
        }
        else {
            CutComMoney(ref money, comInfo.un32ChildID1);
        }
        bool canCut = true;
        int count = ContainAsignedItems(comInfo.un32ChildID2).Count;
        if (comInfo.un32ChildID2 == comInfo.un32ChildID1 && count == 1)
        {
            canCut = false;
        }
        if (canCut)
        {
            if (ContainAsignedItems(comInfo.un32ChildID2).Count != 0)
            {
                money -= GetAsignedItemsMoney(comInfo.un32ChildID2);
            }
            else {
                CutComMoney(ref money, comInfo.un32ChildID2);
            }
        }
       
           
       
    }

    private float GetAsignedItemsMoney(int itemId) {
        if (!ConfigReader.ItemXmlInfoDict.ContainsKey(itemId)) return 0f;
        ItemConfigInfo info = ConfigReader.ItemXmlInfoDict[itemId];
        return info.n32CPCost;
    }

    private List<int> ContainAsignedItems(int itemId) {
        Iselfplayer player = PlayerManager.Instance.LocalPlayer;
        List<int> indexList = new List<int>();
        foreach (var item in player.UserGameItems) {
            int count = 0;
            if (item.Value == itemId && player.UserGameItemsCount.TryGetValue(item.Key,out count)) {
                if (count != 0)
                {
                    indexList.Add(item.Key);
                }
            }
        }
        return indexList;
    }
}

