using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class ButtonChangePic : GuiBase
{
	public UISprite currentTure;
    
	public string clickName; 

	public string disableName;

	public bool isUpResset = true;

	public enum SelectState
	{
		EnableState = 0,
		ResetState,
		DisableState ,
	}
	
	public SelectState selectState
	{
		private set;
		get;
	}

	private string enableName;

	public void SetSelectState(SelectState state){
		selectState = state;
		switch(state)
		{
		case SelectState.DisableState:
            currentTure.spriteName = String.IsNullOrEmpty(disableName) ? enableName : disableName;
			break;
		case SelectState.EnableState:
			currentTure.spriteName = enableName;
			break;
		case SelectState.ResetState:
            currentTure.spriteName = String.IsNullOrEmpty(clickName) ? enableName : clickName;
			break;
		}
	}

	void OnEnable(){
		enableName = currentTure.spriteName;
        SetSelectState(SelectState.EnableState);
	}

	void OnDisable()
	{ 
		SetSelectState(SelectState.DisableState);
	}

	void ButtonChang(bool pressed)
	{
		if(selectState == SelectState.DisableState)
			return;
		if (pressed) {
			SetSelectState(SelectState.EnableState);
		}
		else if(isUpResset){
			SetSelectState(SelectState.ResetState);
		}
        if (Handler != null)
        {
            Handler(PrIe, pressed);
        }
	}


    protected override void OnPress(bool pressed)
    {
        base.OnPress(pressed);
        ButtonChang(pressed);       
    }
}