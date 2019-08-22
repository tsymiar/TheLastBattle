using UnityEngine;
public class UICenterTurnPage : MonoBehaviour
{

    public float springStrength = 8f;
    public float nextPageThreshold = 20f;
    //  public float pageHeight = 140f;
    private int perCount = 6;
    public SpringPanel.OnFinished onFinished;

    UIScrollView mScrollView;
    UIGrid mGrid;
    GameObject mCenteredObject; 
    int pageIndex = 0; 
    public GameObject centeredObject { get { return mCenteredObject; } }

    void OnEnable()
    {
        if (mScrollView == null)
        {
            mScrollView = NGUITools.FindInParents<UIScrollView>(gameObject);
            mGrid = NGUITools.FindInParents<UIGrid>(gameObject);

            if (mScrollView == null)
            {
                Debug.LogWarning(GetType() + " requires " + typeof(UIScrollView) + " on a parent object in order to work", this);
                enabled = false;
                return;
            }
            else
            {
                Reset();
                mScrollView.onDragFinished = OnDragFinished;

                if (mScrollView.horizontalScrollBar != null)
                    mScrollView.horizontalScrollBar.onDragFinished = OnDragFinished;

                if (mScrollView.verticalScrollBar != null)
                    mScrollView.verticalScrollBar.onDragFinished = OnDragFinished;
            }
        }
    }
    void OnDragFinished() { if (enabled) Recenter(); }
 
    public void Recenter()
    {
       
        if (mScrollView.panel == null) return;
        if (transform.childCount == 0) return;

        // Calculate the panel's center in world coordinates
        Vector3[] corners = mScrollView.panel.worldCorners;
        Vector3 panelCenter = (corners[2] + corners[0]) * 0.5f;

        // Offset this value by the momentum
        Vector3 pickingPoint = panelCenter - mScrollView.currentMomentum * (mScrollView.momentumAmount * 0.1f);
        mScrollView.currentMomentum = Vector3.zero;

        float min = float.MaxValue;
        Transform closest = null;
        Transform trans = transform;
        int index = 0;

        // Determine the closest child
        for (int i = 0, imax = trans.childCount; i < imax; ++i)
        {
            Transform t = trans.GetChild(i);
            float sqrDist = Vector3.SqrMagnitude(t.position - pickingPoint);

            if (sqrDist < min)
            {
                min = sqrDist;
                closest = t;
                index = i;
            }
        }
        //if (UICamera.currentTouch != null && UICamera.currentTouch.current != null && UICamera.currentTouch.current == trans.GetChild(index).gameObject)
        //{
            closest = GetPageCenterItem(ref index);
            Vector2 totalDelta = UICamera.currentTouch.totalDelta;

            float delta = 0f;

            switch (mScrollView.movement)
            {
                case UIScrollView.Movement.Horizontal:
                    {
                        delta = totalDelta.x;
                        break;
                    }
                case UIScrollView.Movement.Vertical:
                    {
                        delta = totalDelta.y;
                        break;
                    }
                default:
                    {
                        delta = totalDelta.magnitude;
                        break;
                    }
            }
            if (delta == 0)
                return;
            if (Mathf.Abs(delta) > Mathf.Abs(nextPageThreshold))
            {

                if (delta < 0)
                {
                    if (mScrollView.movement == UIScrollView.Movement.Horizontal)
                    {
                        SetUpOrLeftSwap(ref index, ref closest, trans);
                    }
                    else
                    {
                        SetDownOrRightSwap(ref index, ref closest, trans);
                    }
                }
                else
                {
                    if (mScrollView.movement == UIScrollView.Movement.Vertical)
                    {
                        SetUpOrLeftSwap(ref index, ref closest, trans);
                    }
                    else
                    {
                        SetDownOrRightSwap(ref index, ref closest, trans);
                    }
                }

            }
            int curPageIndex = index / perCount; // swip page 
            //get goal page
            if (!GetGoalPage(curPageIndex, index))// if curpage dif goal page ,select goal page item
            { 
                int item = pageIndex * perCount + index % perCount;
                closest = GetItemByIndex(item);
            }
            if (closest != null){ 
                CenterOn(closest, panelCenter);
            }
           
          
           
    }
    int GetTotalPage() {
        int totalPage = transform.childCount / perCount;
        if (transform.childCount % perCount != 0)
            totalPage += 1;
        return totalPage;
    }

    bool GetGoalPage(int curPageIndex,int index) {

        if (curPageIndex == pageIndex) { 
            return true;
        }

        if (curPageIndex > pageIndex)
        {
            pageIndex += 1;
        }
        else if(curPageIndex < pageIndex){
            pageIndex -= 1;
        }
        
        if (pageIndex < 0)
        {
            pageIndex = 0;
        }

        if (pageIndex >= GetTotalPage())
        {
            pageIndex = GetTotalPage() - 1;
        }

        if (pageIndex != curPageIndex) { 
            return false;
        } 
        return true;
    }

    void SetUpOrLeftSwap(ref int index, ref Transform closest, Transform trans)
    {
        if (index + perCount < trans.childCount)//up swip
        {
            index = index + perCount;
            closest = trans.GetChild(index);
        }
    }

    Transform GetItemByIndex(int index) {
        for (int i = 0; i < transform.childCount; i++) {
            if (i == index) {
                return transform.GetChild(index);                
            }
        }
        Debug.LogError("index error");
        return null;
    }

    void SetDownOrRightSwap(ref int index, ref Transform closest, Transform trans)
    {
        if (index - perCount >= 0)
        {
            index = index - perCount;
            closest = trans.GetChild(index);
        }
    }

    Transform GetPageCenterItem(ref int index)
    {
        int page = index / perCount;
        if (mScrollView.movement == UIScrollView.Movement.Horizontal)
        {
            index = (page * perCount) + 1;
        }
        else
        {
            index = (page * perCount) + (perCount / 2 - 1);
        } 
        return transform.GetChild(index);

    }

    /// <summary>
    /// Center the panel on the specified target.
    /// </summary>

    void CenterOn(Transform target, Vector3 panelCenter)
    {
        if (target != null && mScrollView != null && mScrollView.panel != null)
        {
            Transform panelTrans = mScrollView.panel.cachedTransform;
            mCenteredObject = target.gameObject;

            // Figure out the difference between the chosen child and the panel's center in local coordinates     
            Vector3 cp = panelTrans.InverseTransformPoint(target.position);
            Vector3 cc = panelTrans.InverseTransformPoint(panelCenter);
            Vector3 localOffset = cp - cc;
            if (mScrollView.movement == UIScrollView.Movement.Horizontal)
            {
                localOffset.x += (mGrid.cellWidth / 2f);
                localOffset.y = 0f;                
            }
            else if (mScrollView.movement == UIScrollView.Movement.Vertical)
                localOffset.x = 0f;
            localOffset.z = 0f;
            // Spring the panel to this calculated position 
            SpringPanel.Begin(mScrollView.panel.cachedGameObject,
                panelTrans.localPosition - localOffset, springStrength).onFinished = onFinished;
        }
        else
        {            
            mCenteredObject = null;
        }
    }

    public void Reset() {
        pageIndex = 0;
        mCenteredObject = null; 
    }
}
