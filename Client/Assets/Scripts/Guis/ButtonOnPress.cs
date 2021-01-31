using System;
using System.Collections.Generic;
using UnityEngine;

public class ButtonOnPress : GuiBase {

    public enum EventType { 
        ClickType,
        PressType,
    }


	protected override void OnPress(bool pressed){
		base.OnPress (pressed);
        if (PressHandler != null)
        {
            PressHandler(PrIe, pressed);
		}
	}

    protected void OnClick()
    {
        if (ClickHandler != null)
        {
            ClickHandler(PrIe, false);
        }
    }

	public delegate void HandleOnPress(int ie , bool pressed);
	public HandleOnPress ClickHandler;
    public HandleOnPress PressHandler;

	public void AddListener(int ie , HandleOnPress handler,EventType type = EventType.ClickType) {
		PrIe = ie;
        if(type == EventType.ClickType)
            ClickHandler += handler;
        else
            PressHandler += handler;
	}

    public void AddListener(HandleOnPress handler, EventType type = EventType.ClickType)
    {
        if (type == EventType.ClickType)
            ClickHandler += handler;
        else
            PressHandler += handler;
	}

    public void RemoveListener(HandleOnPress handler, EventType type = EventType.ClickType)
    {
        if (type == EventType.ClickType)
            ClickHandler -= handler;
        else
            PressHandler -= handler;
	}
}
