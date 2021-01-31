using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using BlGame.GameEntity;
using System.Linq;
using BlGame.Skill;

public class UIMoneyEffect:MonoBehaviour
{

    Transform team1;
    Transform team2;
    

    public static UIMoneyEffect  Instance = null;
    public List<MoneyEffectClass> playerMoneyList = new List<MoneyEffectClass>();

    void Awake() {
        Instance = this;
        Init();
    }

    void Init() {

        team1 = transform.FindChild("TeamBackground/TeamPlateBlue");
        team2 = transform.FindChild("TeamBackground/TeamPlateRed");

        for (int i = 0; i < 6; i++) {
            Transform parent = team1;
            int index = i+1;
            if (index % 2 == 0)
            {
                parent = team2;
            }
            Transform goldParent = parent.FindChild("GoldPanel");
            Transform labelParent = goldParent.FindChild("Gold"+index.ToString());
            Transform objIcon = labelParent.FindChild("Gold");
            MoneyEffectClass effect = new MoneyEffectClass();
            for (int j = 0; j < 6; j++) {
                Transform panel = labelParent.FindChild("Panel" + (j + 1).ToString());
                UILabel labelUp = panel.FindChild("Label1").GetComponent<UILabel>();
                UILabel labelDown = panel.FindChild("Label2").GetComponent<UILabel>();
                effect.labelUpList.Add(labelUp);
                effect.labelDownList.Add(labelDown);
            }
            effect.objIcon = objIcon;
            playerMoneyList.Add(effect);            
        }        
    }

    void OnEnable() {
        for (int i = 0; i < 6; i++)
        {
            UIMoneyEffect.Instance.InitShowMoneyEffect(i, 0, 0);
            MoneyEffectClass effect = playerMoneyList.ElementAt(i);
            effect.objIcon.parent.gameObject.SetActive(false);
        }
    }
    public void InitShowMoneyEffect(int camp, int moneySrc, int moneyDst)
    {
       // for (int i = 0; i < 6; i++)
       // {
        MoneyEffectClass effect = playerMoneyList.ElementAt(camp);
        effect.ShowInitMoney(moneySrc, moneyDst);
        effect.objIcon.parent.gameObject.SetActive(true);
       // }
    }

    public void BeginShowMoneyEffect(int camp) {
      //  for (int i = 0; i < 6; i++)
       // {
            MoneyEffectClass effect = playerMoneyList.ElementAt(camp);
            effect.StartMove();
       // }
    }
   
}


public class MoneyEffectClass
{
    public List<UILabel> labelUpList = new List<UILabel>();
    public List<UILabel> labelDownList = new List<UILabel>();
    public Transform objIcon;
    public int srcNum;
    public int dstNum;

    const int moneyBit = 6;
    const float moveDuring = 0.2f;
    float orignalY = 0f;
    float moveHeight1 = 0f;
    float moveHeight2 = 0f;
    int moveIndex = 0;
    int currentNum = 0;
    List<int> srcList = new List<int>();
    List<int> goalList = new List<int>();
    List<UILabel> moveLabelList = new List<UILabel>();
    UITweener tweenPos;

    public bool isMoveEnd = false;
    public void ShowInitMoney(int moneySrc, int moneyDst)
    {
        srcNum = moneySrc;
        dstNum = moneyDst;
        srcList = DivideIntNumber(moneySrc);
        goalList = DivideIntNumber(moneyDst);
        for (int i = 0; i < moneyBit; i++)
        {
            labelUpList.ElementAt(i).text = "0";
            if (i >= srcList.Count)
            {
                continue;
            }
            int num = srcList.ElementAt(i);
            labelUpList.ElementAt(i).text = num.ToString();
        }
        moveHeight1 = labelUpList.ElementAt(0).transform.localPosition.y + labelUpList.ElementAt(0).height;
        orignalY = moveHeight2 = labelUpList.ElementAt(0).transform.localPosition.y;
    }

    public void StartMove()
    {
        if (dstNum == 0) return;
        moveIndex = -1;
        SetCurrentMoveLabelList();
        isMoveEnd = false;
    }

    void SetCurrentMoveLabelList()
    {

        moveIndex += 1;
        moveLabelList.Clear();
        if (moveIndex >= goalList.Count)
        {
            isMoveEnd = true;
            return;
        }
        if (moveIndex >= srcList.Count){
            currentNum = 0;
        }
        else {
            currentNum = srcList.ElementAt(moveIndex);
        }
        
        moveLabelList.Add(labelUpList.ElementAt(moveIndex));
        moveLabelList.Add(labelDownList.ElementAt(moveIndex));
        MoveAsignedBitMoney(moveLabelList);
    }

    bool ReachAsignedBitGoalNum(int index)
    {
        int num = goalList.ElementAt(index);
        if (num == currentNum)
            return true;
        return false;
    }


    List<int> DivideIntNumber(int number)
    {
        List<int> numList = new List<int>();
        int temp = number;
        while (true)
        {
            int num = temp % 10;
            numList.Add(num);
            if (temp / 10 == 0) break;
            temp = temp / 10;
        }
        return numList;
    }

    void MoveAsignedBitMoney(List<UILabel> moveList)
    {
        if (ReachAsignedBitGoalNum(moveIndex))
        {
            SetCurrentMoveLabelList();
           
            return;
        }
        currentNum += 1;
        if (currentNum >= 10)
            currentNum = 0;
        moveList.ElementAt(1).text = currentNum.ToString();
        tweenPos = TweenPosition.Begin(moveList.ElementAt(0).gameObject, moveDuring, new Vector3(0f, moveHeight1, 0f));
        tweenPos.method = UITweener.Method.Linear;
        UITweener tween = TweenPosition.Begin(moveList.ElementAt(1).gameObject, moveDuring, new Vector3(0f, moveHeight2, 0f));
        tween.method = UITweener.Method.Linear;
        EventDelegate.Add(tweenPos.onFinished, OnFinishOnce, true);

    }

    void OnFinishOnce()
    {
        EventDelegate.Remove(tweenPos.onFinished, OnFinishOnce);
        if (ReachAsignedBitGoalNum(moveIndex))
        {
            SetCurrentMoveLabelList();
            return;
        }
        float x = moveLabelList.ElementAt(0).transform.localPosition.x;
        float z = moveLabelList.ElementAt(0).transform.localPosition.z;
        TweenPosition.Begin(moveLabelList.ElementAt(0).gameObject, 0f, new Vector3(0f, -moveHeight1, 0f));
        UILabel temp = moveLabelList.ElementAt(0);
        moveLabelList.RemoveAt(0);
        moveLabelList.Add(temp);
        MoveAsignedBitMoney(moveLabelList);
    }
}

