using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using BlGame.GameEntity;
using System;

public class MoveSprite 
{	
	private GameObject MoveObj;
	private Vector3 Vec;
	private float delay ;
    public static int count = 0;
    static int totalCount = 3;
	public void SetMoveState(float delate,GameObject obj,float dir, Vector3 v3)
	{
		delay = delate;
		MoveObj = obj;
		Vec = v3;
		UITweener uit = TweenPosition.Begin (obj, dir ,new Vector3(Vec.x - 30,0,0));
		uit.style = UITweener.Style.Once;
		EventDelegate.Add (uit.onFinished,Shark,true);
	}
	public void Shark()
	{
		UITweener uit = TweenPosition.Begin (MoveObj,0.15f ,new Vector3(Vec.x,0,0));
		uit.style = UITweener.Style.Once;
		uit.method = UITweener.Method.Linear;
		uit.delay = delay;
        EventDelegate.Add(uit.onFinished,FinishBack);
	}

    void FinishBack() {
        count += 1;
        if (count == totalCount) {
            //UIPvpOrGuideBase.Instance.DoSelfBtnMoveIn();
        }
    }

	GameObject Object;
	UITweener uit;
	public void CustonScaleAlpha(GameObject obj,float dir,float ahpl,float scaledir)
	{
		this.Object = obj;
		uit = TweenAlpha.Begin (obj,dir,ahpl);
		uit.delay = 0.5f;
		uit = TweenScale.Begin (obj,scaledir,new Vector3(1.01f,1.01f,1.01f));
//		uit.delay = 0.5f;
		EventDelegate.Add (uit.onFinished, CustonButtonBig,true);
	}
	void CustonButtonBig()
	{
		EventDelegate.Remove (uit.onFinished, CustonButtonBig);
		uit = TweenScale.Begin (this.Object,0.6f,new Vector3(1.1f,1.1f,1.1f));
//		uit.delay = 0.2f;
		EventDelegate.Add (uit.onFinished, CustonButtonEnd,true);
		
	}
	void CustonButtonEnd()
	{
		EventDelegate.Remove (uit.onFinished, CustonButtonEnd);
		uit = TweenScale.Begin (this.Object,0.6f,Vector3.one);
//		uit.delay = 0.2f;
//		UIPvpOrGuideBase.isKeyPress = true;
	//	EventDelegate.Add (uit.onFinished, ChangeState, true);
	}
    //void ChangeState()
    //{
    //    UIPvpOrGuideBase.Instance.isKeyPress = true;
    //}
}
