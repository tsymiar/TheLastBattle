using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.GameEntity;
public class CrticalStrike : MonoBehaviour {

    public enum ATTACKTYPE
    {
        NormalAttack = 0,
        CriticalAttack = 1,
        BuffAttack = 2,
        ReplyBlood = 3,
        SkillAttack = 4,
        SystemAttack = 5,
    }
    
	// Use this for initialization
	void Start () {
	
	}
    
	// Update is called once per frame
	void Update () {
        if (CrticalOwner != null && CrticalOwner.realObject != null)
        {
            //UpdatePosition(CrticalOwner.realObject.transform);
        }
        
	}
    void Awake()
    {
        DigitNum = transform.FindChild("DigitNum").GetComponent<UILabel>();
    }
    public Ientity CrticalOwner
    {
        set;
        get;
    }
    private UILabel DigitNum;
    TweenAlpha itwee;
    public bool isAni = false;
    public void Init(Ientity entity)
    {
        transform.gameObject.SetActive(true);        
        CrticalOwner = entity;
    }
    public void SetShowDighit(int count)
    {
        transform.parent = CrticalStrikeManager.Instance.transform;
        //transform.localScale = Vector3.one;
        //switch ((ATTACKTYPE)type)
        //{
        //    case ATTACKTYPE.NormalAttack:
        //        DigitNum.text = count.ToString();
        //        break;
        //    case ATTACKTYPE.CriticalAttack:
		DigitNum.text = count.ToString() + "!";             
        //        break;
        //    case ATTACKTYPE.BuffAttack:
        //        DigitNum.text = count.ToString();
        //        break;
        //    case ATTACKTYPE.SkillAttack:
        //        DigitNum.text = count.ToString();
        //        break;
        //}
        isAni = true;
        //TweenPosition.Begin(this.gameObject, 1.5f, new Vector3(transform.localPosition.y, transform.localPosition.z + 5, transform.localPosition.x));
        itwee = TweenAlpha.Begin(this.gameObject, 1.2f, 0f);
        itwee.from = 1.0f;
        itwee.to = 0;
		itwee.delay = 0.34f;
        itwee.style = TweenAlpha.Style.Once;
        EventDelegate.Add(itwee.onFinished, FinishMove, true);
    }
    void FinishMove()
    {
        EventDelegate.Remove(itwee.onFinished, FinishMove);
        itwee.duration = 1.0f;
        itwee.value = 1.0f;
        isAni = false;
        transform.gameObject.SetActive(false);
    }
        
    public void UpdatePosition(Transform targetTran)
    {
        if (targetTran == null) return;
        // 血条对应在3d场景的位置
        Vector3 pos_3d = targetTran.position;
        // 血条对应在屏幕的位置
        Vector2 pos_screen = Camera.main.WorldToScreenPoint(pos_3d);

        //Debug.Log("targetTran  " + targetTran + "  pos_screen " + pos_screen);
        // 血条对应在ui中的位置
        Vector3 pos_ui =  GameMethod.GetUiCamera.ScreenToWorldPoint(pos_screen);
        transform.position = pos_ui;
        //transform.position = Vector3.Slerp(transform.position, pos_ui, Time.time);

        //		Vector3 pos = new Vector3(0.1f,0.1f,0.1f);
        //		transform.position = Vector3.SmoothDamp(transform.position,pos_ui,ref pos,Time.time);

        //Vector3 lerp = new Vector3(0.0f,0.05f,0.0f);
        //transform.position = Vector3.Lerp(pos_ui , pos_ui+lerp , 0.981f) ; 
    } 
}
