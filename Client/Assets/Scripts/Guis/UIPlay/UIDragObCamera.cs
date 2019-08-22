using UnityEngine; 
using System;
using System.Collections;
using System.Collections.Generic;
using GameDefine;

public class UIDragObCamera : MonoBehaviour
{

    public static UIDragObCamera Instance;

    public enum RectDir
    { 
        TopLeft,
        Top,
        TopRight,
        Left,
        Right,
        BottomLeft,
        BottomRight,
        Bottom,
        None,
    }

    private const float ConnerXLimit = 10f;

    private const float ConnerYLimit = 10f;    

    private bool canUse = false; 

    public void SetUsable(bool can) {
        canUse = can;
    }

    void OnEnable() {
        Instance = this; 
    }

    void OnDisable() {
       
    }
     
    void Update() {
        if (Application.platform == RuntimePlatform.IPhonePlayer || Application.platform == RuntimePlatform.Android)
            return;
        DragCamear(); 
    }


    float speed = 50f;

    void DragCamear()
    {
        if (!canUse)
            return;
        if (UICamera.selectedObject != null && Input.GetMouseButtonDown(0))
            return;
        RectDir rectDir = GetRectDir(); 
        if (rectDir == RectDir.None)
            return;

        if (GameMethod.GetMainCamera != null && GameMethod.GetMainCamera.enabled)
        {
            GameMethod.GetMainCamera.enabled = false;
            Camera.main.transform.localRotation = Quaternion.Euler(new Vector3(45f, 45f, 0f));
        }
        if (GameMethod.GetMainCamera != null && GameMethod.GetMainCamera.target != null)
            GameMethod.GetMainCamera.target = null;

        Vector3 dir = new Vector3();
        switch (rectDir)
        { 
            case RectDir.TopLeft://
                dir = new Vector3(0.1f, 0f,1f); 
                break;
            case RectDir.TopRight:
                dir = new Vector3(1f, 0f, 0.1f); 
                break;
            case RectDir.Right://
                dir = new Vector3(1f, 0f, -1f);
                break;
            case RectDir.Left://
                dir = new Vector3(-1f, 0f, 1f);
                break;
            case RectDir.Top://
                dir = new Vector3(1f, 0f, 1f); 
                break;
            case RectDir.Bottom://
                dir = new Vector3(-1f, 0f, -1f);
                break;
            case RectDir.BottomRight://
                dir = new Vector3(-0.1f, 0f, -1f); 
                break;
            case RectDir.BottomLeft://
                dir = new Vector3(-1f, 0f, -0.1f);
                break;
        }


        Vector3 targetPos = Camera.main.transform.position + dir * speed * Time.deltaTime;
        if (targetPos.x < 6.7f || targetPos.x > 154f)
            return;
        if (targetPos.z > 166f || targetPos.z < 6.8f)
            return;
        if (UIViewerPersonInfo.Instance != null)
        {
            UIViewerPersonInfo.Instance.SetVib();
        }

        Camera.main.transform.position = Vector3.Lerp(Camera.main.transform.position, targetPos, Time.time);  
    }


    RectDir GetRectDir() {
        
        Vector3 pos = Input.mousePosition; 
        if (InEquipArea(pos) || InSkillArea(pos))
        { 
            return RectDir.None;
        }
        if (InTopLeftConner(pos))
            return RectDir.TopLeft;
        if (InTopRightConner(pos))
            return RectDir.TopRight;
        if (InBottomLeftConner(pos))
            return RectDir.BottomLeft;
        if (InBottomRightConner(pos))
            return RectDir.BottomRight;
        if (InLeft(pos))
            return RectDir.Left;
        if (InRight(pos))
            return RectDir.Right;
        if (InTop(pos))
            return RectDir.Top;
        if (InBottom(pos))
            return RectDir.Bottom;
        return RectDir.None;
    }

    bool InTopLeftConner(Vector3 pos)
    { 
        if (pos.x < ConnerXLimit && pos.y > (Screen.height - ConnerYLimit)) {
            return true;
        }
        return false;
    }

    bool InBottomLeftConner(Vector3 pos)
    {
        if (pos.x < ConnerXLimit && pos.y < ConnerYLimit)
        {
            return true;
        }
        return false;
    }

    bool InTopRightConner(Vector3 pos)
    { 
        if (pos.x > (Screen.width - ConnerXLimit) && pos.y > (Screen.height - ConnerYLimit))
        {
            return true;
        }
        return false;
    }

    bool InBottomRightConner(Vector3 pos)
    { 
        if (pos.x > (Screen.width - ConnerXLimit) && pos.y < ConnerYLimit)
        {
            return true;
        }
        return false;
    }

    bool InTop(Vector3 pos)
    { 
        if (pos.y > (Screen.height - ConnerYLimit))
            return true;
        return false;
    }

    bool InBottom(Vector3 pos)
    { 
        if (pos.y < ConnerYLimit)
            return true;
        return false;
    }

    bool InLeft(Vector3 pos)
    { 
        if (pos.x < ConnerXLimit)
            return true;
        return false;
    }

    bool InRight(Vector3 pos)
    { 
        if (pos.x > (Screen.width - ConnerXLimit))
            return true;
        return false;
    }

    float equipXLimit = 100f;
    bool InEquipArea(Vector3 pos)
    {
        if (pos.x >= (Screen.width / 2f - equipXLimit) && pos.x <= (Screen.width / 2f + equipXLimit) 
            && pos.y > 10f && pos.y < 50f)
            return true;
        return false;
    }

    float skillXLimit = 180f;
    bool InSkillArea(Vector3 pos)
    {
        if (pos.x < (Screen.width - ConnerXLimit) && pos.x >= (Screen.width - skillXLimit) && pos.y < 50f && pos.y > 10f)
            return true;
        return false;
    }
    //GameObject markObj = null;
    //void DragCamera(FingerGestures.DragDirectionEnum dragDir, Vector2 delta) { 
    //    if (!CheckCanMove()) return;
    //    if (GameMethod.GetMainCamera.enabled)
    //    {
    //        GameMethod.GetMainCamera.enabled = false;
    //        Camera.main.transform.localRotation = Quaternion.Euler(new Vector3(45f, 45f, 0f));
    //    }
    //    if (GameMethod.GetMainCamera.target != null)
    //        GameMethod.GetMainCamera.target = null; 
    //    Vector3 dir = Vector3.up;
    //    switch (dragDir)
    //    {
    //        case FingerGestures.DragDirectionEnum.DragDown:
    //            dir = new Vector3(0f, 0f, -2f); 
    //            break;
    //        case FingerGestures.DragDirectionEnum.DragUp:
    //            dir = new Vector3(0f, 0f, 2f); 
    //            break;
    //        case FingerGestures.DragDirectionEnum.DragLeft:
    //            dir = new Vector3(2f, 0f, -2f);                  
    //            break;
    //        case FingerGestures.DragDirectionEnum.DragRight:
    //            dir = new Vector3(-2f, 0f, 2f);
    //            break;
    //    }
         
    //    Vector3 targetPos = Camera.main.transform.position + dir * speed * Time.deltaTime; 
    //        return;
    //    if (targetPos.x > 154f)
    //        return;
    //    if (targetPos.z > 166f)
    //        return;
    //    if (targetPos.z < 6.8f)
    //        return;
    //    if (UIViewerPersonInfo.Instance != null)
    //    {
    //        UIViewerPersonInfo.Instance.SetVib();
    //    }

        
    //    Camera.main.transform.position = Vector3.Lerp(Camera.main.transform.position, targetPos,Time.time);  
    //}

}
