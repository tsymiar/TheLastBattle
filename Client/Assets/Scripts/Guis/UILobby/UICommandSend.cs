using UnityEngine;
using System.Collections;
using System;

public class UICommandSend : MonoBehaviour
{
    public static UICommandSend Instance
    {
        set;
        get;
    }
    ButtonOnPress SendBtn;
    UIInput InputCommand;
    UIInput InputType;
    void Awake()
    {
        Instance = this;
        Init();
    }
    void Init()
    {
        SendBtn = this.transform.FindChild("SendMsg").GetComponent<ButtonOnPress>();
        InputCommand = this.transform.FindChild("Input").GetComponent<UIInput>();
        InputType = this.transform.FindChild("type").GetComponent<UIInput>();

        UIEventListener.Get(InputCommand.gameObject).onSelect += ResetDefaultInput;

        UIEventListener.Get(InputType.gameObject).onSelect += ResetDefaultInput;
    }
    void OnDisable()
    {
        SendBtn.RemoveListener(BtnOnPress);
    }
    void OnEnable()
    {
        SendBtn.AddListener(BtnOnPress);
    }
    void BtnOnPress(int ie , bool isPress)
    {
        if (isPress)
            return;
        CGLCtrl_GameLogic.Instance.EmsgTocs_Notice(Convert.ToInt32(InputType.value), InputCommand.value);
        InputType.value = "";
        InputCommand.value = "";
    }

    void ResetDefaultInput(GameObject go, bool state)
    {
        go.GetComponent<UIInput>().value = "";
    }

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
