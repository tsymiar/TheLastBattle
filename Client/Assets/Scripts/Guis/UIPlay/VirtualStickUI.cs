using UnityEngine; 
using System;
using System.Collections;
using System.Collections.Generic;
using UICommon ; 
using BlGame.GameEntity;
using BlGame.GameData;
using JT.FWW.GameData;
using BlGame.FSM;
using BlGame.GuideDate;
using BlGame.Model;
using BlGame.GameState;

public class VirtualStickUI : MonoBehaviour 
{
	#region 公有方法
	public void SetVirtualStickUsable(bool enable)
	{
		canUse = enable; 		
		if (!enable) 
        {
			CloseStick();	
		}
	}

	#endregion

	#region 私有方法 
    void Awake()
    {
        Init();
    }

	void Init()
	{ 
		Instance = this;
		orignalPos = transform.localPosition;
        VirtualStickState = StickState.InActiveState;
		point = transform.FindChild("stick");
		btnSelf = transform.GetComponent<ButtonOnPress>();
        
        //firstInit = true;
	}


	void OnEnable()
	{		
		canUse = true;
		SetVisiable(false);
        btnSelf.AddListener(PressVirtual, ButtonOnPress.EventType.PressType);
        BlGame.Ctrl.UIGuideCtrl.Instance.AddUiGuideEventBtn(btnSelf.gameObject);
	}

    void OnDisable() {
        btnSelf.RemoveListener(PressVirtual, ButtonOnPress.EventType.PressType);
        VirtualStickState = StickState.InActiveState;
    }
 
	/// <summary>
	/// 点击时显示摇杆，松开始隐藏摇杆
	/// </summary>
	/// <param name="index">Index.</param>
	/// <param name="isDown">If set to <c>true</c> is down.</param>
    void PressVirtual(int ie,bool isDown)
    {
		if(isDown){
			ShowStick();
		}else{
			CloseStick();
		}
	}

	void OnDrag(Vector2 pos)
	{
		if(canUse == false) return;

		Vector2 touchPos = UICamera.currentTouch.pos;
		 
		SetPointPos(touchPos);

        VirtualStickState = StickState.MoveState;
		SendMove();
	}

    /// <summary>
    /// 显示摇杆
    /// </summary>
	void ShowStick()
	{
		if(canUse == false) return;
        //PlayerManager.Instance.LocalPlayer.MoveCount = -1;
        Vector2 touchPos = UICamera.currentTouch.pos;

        SetPointPos(touchPos);

        VirtualStickState = StickState.MoveState;
        //SendMove();
        transform.position = new Vector3(UICommonMethod.GetWorldPos(touchPos).x, UICommonMethod.GetWorldPos(touchPos).y, transform.position.z);
        point.localPosition = Vector3.zero;
        //VirtualStickState = StickState.ActiveState; 
		SetVisiable(true);		 
	} 

	void CloseStick()
	{
        if (VirtualStickState == StickState.MoveState)
        {
            SendStop();
        }
        Iselfplayer player = PlayerManager.Instance.LocalPlayer;
        if (player != null && player.FSM != null && player.FSM.State == FsmState.FSM_STATE_ADMOVE)
        {
            player.OnFSMStateChange(EntityFreeFSM.Instance);
        }
		SetVisiable(false);
        VirtualStickState = StickState.InActiveState;
        beforeDir = Vector3.zero;
	}

	void SendStop(){
		CGLCtrl_GameLogic.Instance.EmsgToss_AskStopMove ();
	}
	
	private const float SendMoveInterVal = 0.05f;
	private float MoveSendTime;
	void SendMove()
	{ 
		Vector3 direction = GetPointerDirection();	
		Transform target = JxBlGame.Instance.transform; 
		Entity entity = PlayerManager.Instance.LocalPlayer.RealEntity;
		if (entity == null) {
			return ;		
		}

        PlayState playState = GameStateManager.Instance.GetCurState() as PlayState;
        if (playState == null)
            return;


		target.position = entity.transform.position;
		target.LookAt(entity.transform.position + direction);
       
        //运动正方向
        Vector3 dir = new Vector3(0, 0, 0);

        //地图已经修改，不需要旋转
        //target.transform.Rotate(new Vector3(0.0f, 45f, 0.0f));

        //获取当前相机类型
        if (playState.mCameraType == 1)         //斜45度
        {
            //转为相机坐标系
            if (entity.CampType == EntityCampType.CampTypeA)
            {
                Quaternion rot = Quaternion.Euler(0, 45f, 0);
                dir = rot * target.forward;
            }
            else if (entity.CampType == EntityCampType.CampTypeB)
            {
                Quaternion rot = Quaternion.Euler(0, -135f, 0);
                dir = rot * target.forward;
            }
            else
            {
                Debug.Log("no valid entity camp type!");
            }
        }
        else                                 //水平
        {
            //转为相机坐标系
            if (entity.CampType == EntityCampType.CampTypeA)
            {
                Quaternion rot = Quaternion.Euler(0, 0f, 0);
                dir = rot * target.forward;
            }
            else if (entity.CampType == EntityCampType.CampTypeB)
            {
                Quaternion rot = Quaternion.Euler(0, -180f, 0);
                dir = rot * target.forward;
            }
            else
            {
                Debug.Log("no valid entity camp type!");
            }
        }
            
        Vector3 dealPos = entity.transform.position + dir * Time.deltaTime * PlayerManager.Instance.LocalPlayer.EntityFSMMoveSpeed;
        Vector3 dealPos1 = dealPos + dir * Time.deltaTime * PlayerManager.Instance.LocalPlayer.EntityFSMMoveSpeed * 2;       


		if(dir != Vector3.zero && Time.time - MoveSendTime >= SendMoveInterVal)
		{
            //if (SceneGuideTaskManager.Instance().IsNewsGuide() != SceneGuideTaskManager.SceneGuideType.NoGuide) {
            //    if (!PlayerManager.Instance.LocalPlayer.CheckGuideCanMove(dealPos1))
            //    {
            //        SendStop();
            //        return;
            //    }
            //}

			MoveSendTime = Time.time; 
			CGLCtrl_GameLogic.Instance.EmsgToss_AskMoveDir(dir);
			beforeDir = dir ;
            PlayerAdMove(dir);
		}
	}

    private void PlayerAdMove(Vector3 mvDir) {
        Iselfplayer player = PlayerManager.Instance.LocalPlayer;
        if (BlGame.Skill.BuffManager.Instance.isHaveStopBuff(player.GameObjGUID))
        {
            return;
        }
        if (player.FSM == null)
        {
            return;
        }
        if (player.FSM.State == FsmState.FSM_STATE_DEAD || player.FSM.State == FsmState.FSM_STATE_RUN || player.FSM.State == FsmState.FSM_STATE_FORCEMOVE
            || player.FSM.State == FsmState.FSM_STATE_RELIVE)
        {
            return;
        }
        float mvSpeed = player.EntityFSMMoveSpeed;
        if(mvSpeed <= 0)
        {
            mvSpeed = 3.0f;
        }
        player.EntityFSMChangedata(player.realObject.transform.position, mvDir, mvSpeed);
        player.OnFSMStateChange(PlayerAdMoveFSM.Instance);
    }

	void SetVisiable(bool visiable)
	{
		if (visiable) {
			UICommonMethod.TweenColorBegin(gameObject, 0f, 1f);
		}else {
			UICommonMethod.TweenColorBegin(gameObject, 0f, 0.5f);
			transform.localPosition = orignalPos;
			point.localPosition = Vector3.zero;
		}
	}

	void SetPointPos(Vector2 pos)
	{
		Vector3 newPos = UICommonMethod.GetWorldPos(pos)+new Vector3(0f,0f,point.position.z);
		
		if(Vector3.Distance(newPos,transform.position) > adjustRadius)
		{		
			Vector3 direction = newPos - transform.position;
			direction.Normalize();
			direction *= adjustRadius;
			newPos = transform.position + direction; 
		}
		point.position = newPos;		 
	} 

	Vector3 GetPointerDirection()
	{
		Vector3 direction = point.position - transform.position;
		
		direction = new Vector3(direction.x, 0f, direction.y);
		direction.Normalize();
		
		return direction;
	}
	
	Vector3 NormalVector3(Vector3 pos)
	{
		float x = (float)Math.Round(pos.x,1);
		float y = (float)Math.Round(pos.y,1);
		float z = (float)Math.Round(pos.z,1);
		Vector3 posTemp = new Vector3(x,y,z);
		return posTemp;
	}
	
	bool Vect3Compare(Vector3 pos1,Vector3 pos2)
	{  
		if(pos1.x != pos2.x){	
			return false ;
		}
		if(pos1.y != pos2.y){			 
			return false ;
		}
		if(pos1.z != pos2.z){ 
			return false ;
		}
		
		return true;
	}
 
	#endregion

	#region 变量区	
	public static VirtualStickUI Instance {
		get;
		private set;
	}


	public enum StickState
	{
		ActiveState,
		MoveState,
		InActiveState,
	}

	public StickState VirtualStickState{
		get;
		private set;
	}

	private Transform point;

	private ButtonOnPress btnSelf;

	private Vector3 orignalPos = new Vector3();

	private bool canUse = true; 

	public float adjustRadius = 0.3f;

	private Vector3 beforeDir;
	#endregion
}
