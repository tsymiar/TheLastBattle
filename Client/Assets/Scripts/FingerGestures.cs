using UnityEngine;
using System.Collections;
using System.Collections.Generic;
public class FingerGestures : MonoBehaviour
{
    public static FingerGestures Instance
    {
        private set;
        get;
    }

    LayerMask Mask;

    bool useMouse = true;

    public float mouseDragThreshold = 13f;

    public float touchDragThreshold = 60f;

    public float xLimit = 8f;
    public float yLimit = 8f;

    public delegate void OnDragBegin(Vector2 pos);
    public OnDragBegin DragBegin;

    public delegate void OnDrag(Vector2 pos, Vector2 deltaPosition);
    public OnDrag Drag;

    public delegate void OnDragOver(Vector2 pos);
    public OnDragOver DragOver;

    public delegate void OnDragDirection(DragDirectionEnum dir, float offset);
    public OnDragDirection DragDirection;

    public delegate void OnDragFixDirection(DragDirectionEnum dir, Vector2 delta);
    public OnDragFixDirection DragFixDirection;

    public enum DragDirectionEnum
    { 
        DragLeft,
        DragRight,
        DragUp,
        DragDown,
        DragDirNone,
    }

   

    public class TouchInfo
    {
        public Vector2 pos;				// Current position of the mouse or touch event
        public Vector2 lastPos;			// Previous position of the mouse or touch event
        public Vector2 delta;			// Delta since last update
        public Vector2 totalDelta;		// Delta since the event started being tracked
        public TouchDragState dragState;
        public bool IsDraging;
    }

    public TouchInfo currentTouch = new TouchInfo();

    public enum TouchDragState
    { 
        Begin,
        StartDrag,
        Drag,
        End
    }

	// Use this for initialization
	void Start () {
        Mask = LayerMask.NameToLayer("Entity");
        if (Application.platform == RuntimePlatform.Android ||
            Application.platform == RuntimePlatform.IPhonePlayer
         || Application.platform == RuntimePlatform.WP8Player
         || Application.platform == RuntimePlatform.BB10Player)
        {
            useMouse = false;
            if (Application.platform == RuntimePlatform.IPhonePlayer)
            {
                //useKeyboard = false;
                //useController = false;
            }
        }
        //else if (Application.platform == RuntimePlatform.PS3 ||
        //         Application.platform == RuntimePlatform.XBOX360)
        //{
            //useMouse = false;
            //useTouch = false;
            //useKeyboard = false;
            //useController = true;
	}
	
	// Update is called once per frame
	void Update () {
        if (useMouse)
        {
            ProcessMouse();
        }
        else {
            ProcessTouches();
        }
        //ProcessTouches();
        //UpdateFingers();
        
	}

    void OnEnable()
    {
        Instance = this;
        currentTouch.dragState = TouchDragState.End;
    }
    
    protected void ProcessTouches()
    {
        SetTouchDragState();
        if (Camera.main == null)
        {
            return;
        }

        for (int i = 0; i < Input.touchCount; ++i)
        {
            Touch touch = Input.GetTouch(i);  
            GameObject selection = SelectionObject(touch.position);
            if (UICamera.hoveredObject != null && UICamera.hoveredObject.name != "GameUI") continue;
            if (selection == null)
            {
                continue;
            }
            selection.SendMessage("OnFingerClick", SendMessageOptions.DontRequireReceiver);
        }
    }
      

    /// <summary>
    /// 选取一个对象
    /// </summary>
    /// <returns>
    /// The object.
    /// </returns>
    /// <param name='screenPos'>
    /// Screen position.
    /// </param>
    protected GameObject SelectionObject(Vector3 screenPos)
    {
        Ray ray = Camera.main.ScreenPointToRay(screenPos);
        RaycastHit hit;

        if (Physics.Raycast(ray , out hit, 100.0f, 1 << Mask.value))
        {
            return hit.collider.gameObject;
        } 
        return null;
    }

    /// <summary>
    /// ProcessMouse Event
    /// </summary>
    protected void ProcessMouse()
    {

        SetMouseDragState();

        if (Camera.main == null)
        {
            return;
        }
        for (int i = 0; i < 3; ++i)
        {
            if (Input.GetMouseButtonDown(i))
            { 
                GameObject selection = SelectionObject(Input.mousePosition);
                if (UICamera.hoveredObject != null && UICamera.hoveredObject.name != "GameUI") continue;	
                if (selection == null)
                {
                   continue;
                }
                selection.SendMessage("OnFingerClick", SendMessageOptions.DontRequireReceiver);
            }
        }
    }
     
    private void SetMouseDragState() {
        bool isPress = Input.GetMouseButtonDown(0);
        bool isUp = Input.GetMouseButtonUp(0);        
        currentTouch.lastPos = currentTouch.pos;
        currentTouch.pos = Input.mousePosition;
        ProgressTouch(isPress,isUp);
    }

    private void SetTouchDragState() {
        if (Input.touchCount == 0)
            return;
        Touch touch = Input.GetTouch(0);
        bool isPress = (touch.phase == TouchPhase.Began);
        bool isUp = (touch.phase == TouchPhase.Canceled || touch.phase == TouchPhase.Ended);
        currentTouch.lastPos = currentTouch.pos;
        currentTouch.pos = touch.position;
        ProgressTouch(isPress, isUp);
    }


    private void ProgressTouch(bool press,bool up) {
        if (press)
        { 
            currentTouch.lastPos = currentTouch.pos;
            currentTouch.dragState = TouchDragState.Begin; 
            currentTouch.delta = currentTouch.totalDelta = Vector2.zero;
            currentTouch.IsDraging = false;
        }
        else if (currentTouch.dragState != TouchDragState.End)
        {
            Vector2 delta = currentTouch.pos - currentTouch.lastPos;
            currentTouch.totalDelta += delta;

            if (currentTouch.dragState == TouchDragState.Begin && delta.magnitude != 0)
            { 
                currentTouch.delta = currentTouch.totalDelta;
                currentTouch.dragState = TouchDragState.StartDrag;
            }
            else if (currentTouch.dragState == TouchDragState.StartDrag)
            { 
                float drag = useMouse ? mouseDragThreshold : touchDragThreshold;
                if (drag < currentTouch.totalDelta.magnitude) {
                    currentTouch.dragState = TouchDragState.Drag;
                    currentTouch.delta = delta; 
                    if (DragBegin != null && !currentTouch.IsDraging)
                    {                        
                        DragBegin(currentTouch.pos);
                    }
                    currentTouch.IsDraging = false;
                }
            }

            if (currentTouch.dragState == TouchDragState.Drag) {   
                currentTouch.dragState = TouchDragState.StartDrag; 
                if (Drag != null) {
                    Drag(currentTouch.pos, currentTouch.delta);
                } 
                if (DragDirection != null) {
                    DragDirectionEnum dir = GetMoveDirection(currentTouch.delta);
                    DragDirection(dir, Mathf.Abs(currentTouch.delta.y));
                }
                if (DragFixDirection != null) {
                    List<DragDirectionEnum> list = GetMoveFixDirection(currentTouch.delta);
                    if (list != null && list.Count != 0)
                    {
                        foreach (var item in list) {
                            DragFixDirection(item, delta);
                        }
                    }
                }
                currentTouch.IsDraging = true;
            }
        }

        if (up) {
            currentTouch.dragState = TouchDragState.End;
            if (DragOver != null) {
                DragOver(currentTouch.pos);
            } 
            currentTouch.IsDraging = false;
        }
        
    }

    private List<DragDirectionEnum> GetMoveFixDirection(Vector2 delta) {
        List<DragDirectionEnum> listDir = new List<DragDirectionEnum>();
        float xMove = Mathf.Abs(delta.x);
        float yMove = Mathf.Abs(delta.y);
        if (xMove < xLimit && yMove < yLimit)
        {
            return null;
        }
        if (delta.x > 0)
            listDir.Add(DragDirectionEnum.DragRight);
        else if (delta.x < 0)
            listDir.Add(DragDirectionEnum.DragLeft);
        if (delta.y > 0)
            listDir.Add(DragDirectionEnum.DragUp);
        else if (delta.y < 0)
            listDir.Add(DragDirectionEnum.DragDown);
        return listDir;
    }

    private DragDirectionEnum GetMoveDirection(Vector2 delta) {
        float xMove = Mathf.Abs(delta.x);
        float yMove = Mathf.Abs(delta.y);
        DragDirectionEnum dir = DragDirectionEnum.DragDirNone;
        if (xMove < xLimit && yMove < yLimit)
        {
            return dir;
        }
        if (xMove > yMove)
        {
            if (currentTouch.delta.x > 0)
            {
                dir = DragDirectionEnum.DragRight; 
            }
            else
            {
                dir = DragDirectionEnum.DragLeft; 
            }
        }
        else
        {
            if (currentTouch.delta.y > 0)
            {
                dir = DragDirectionEnum.DragUp; 
            }
            else
            {
                dir = DragDirectionEnum.DragDown; 
            }
        }
        return dir;
    }
    
}
