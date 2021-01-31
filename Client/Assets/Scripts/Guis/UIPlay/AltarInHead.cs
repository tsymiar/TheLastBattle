using UnityEngine;
using System.Collections;
using BlGame.GameEntity;
using BlGame.View;

public class AltarInHead : MonoBehaviour
{

    public static AltarInHead Instance
    {
		set;
		get;
	}



	public Entity targetEntity{ private set; get;}
	UIAltarInHead head = null;
	void Awake()
	{
		Instance = this;

	}

	// Use this for initialization
    void Start()
    {
	
	}
	
	// Update is called once per frame
    void Update()
    {
		//
        if (targetEntity == null)
        {
			targetEntity = transform.GetComponent<Entity>();		
		}
	}

    void OnDisable()
    {
        Instance = null;
        if (head != null)
        {
            DestroyImmediate(head.gameObject);
        }
    }
    public void CreateHead(int type)
        {
        if (head == null)
        {
            head = ((GamePlayWindow)WindowManager.Instance.GetWindow(EWindowType.EWT_GamePlayWindow)).CreateAltarUIPrefab(this, type);
        }
        else
            head.SetCurrHeadIcon(type, this.transform);
	}
    public void DelHead()
    {
        if (head != null)
        {
            head.DelAltarInHead();
        }
    }
}
