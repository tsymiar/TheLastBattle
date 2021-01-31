using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using UICommon;
using BlGame.GameEntity;
using System.Linq;
public class UIObjDragEvent : MonoBehaviour
{
    public enum DragState
    { 
        InvalideDrag,
        LongPressStart,
        DragMove, 
        DragEnd,
    }

    public DragState dragState
    {
        private set;
        get;
    }

    public float longPressTime = 1.2f;

    public Action<GameObject,DragState,Vector2> ObjDragEvent;

    private float pressTime = 0f;

    private bool IsPressDown = false;

    void OnEnable() {
        Clean();
    }

    void Clean() {
        dragState = DragState.InvalideDrag;
        pressTime = 0f;
        IsPressDown = false;
    }

    void SendDragEvent(Vector2 pos)
    {
        if (ObjDragEvent != null && gameObject != null)
        {
            ObjDragEvent(gameObject,dragState, pos);
        }
    }

    void OnPress(bool isDown) {
        IsPressDown = isDown;
        if (isDown) {
            dragState = DragState.InvalideDrag;
            pressTime = Time.time;
            return;
        }
        if (dragState == DragState.LongPressStart || dragState == DragState.InvalideDrag )
        {
            dragState = DragState.InvalideDrag;
            pressTime = 0f;
            IsPressDown = false;
            SendDragEvent(Vector3.zero);
        }
    }

    void OnDrag(Vector2 delta)
    {
        if (dragState != DragState.LongPressStart && dragState != DragState.DragMove)
            return;
        dragState = DragState.DragMove;

        Vector2 touchPos = UICamera.currentTouch.pos;
        SendDragEvent(touchPos);
    }

   
    void OnDragEnd()
    {
        if (dragState != DragState.DragMove) return;
        dragState = DragState.DragEnd;
        pressTime = 0f;
        IsPressDown = false;
        Vector2 touchPos = UICamera.currentTouch.pos;
        SendDragEvent(touchPos);
    }

    void Update() {
        if (IsPressDown && pressTime != 0f && Time.time - pressTime >= longPressTime)
        {
            if (dragState == DragState.InvalideDrag) {
                dragState = DragState.LongPressStart;
                Vector2 pos_screen = Camera.main.WorldToScreenPoint(transform.position);
                SendDragEvent(pos_screen);
            }
            
        }
    }
 
}

