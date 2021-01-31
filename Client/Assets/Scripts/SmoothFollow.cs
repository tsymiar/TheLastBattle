using UnityEngine;
using System.Collections;

public class SmoothFollow : MonoBehaviour {

    // The target we are following
    public Transform target;
    // The distance in the x-z plane to the target
    public float distance = 10.0f;
    // the height we want the camera to be above the target
    public float height = 5.0f;
    // How much we 
    public float heightDamping = 2.0f;
    public float rotationDamping = 3.0f;

    //当前帧数值,用来检测是否有重复更新 
    private int curFrame = -1;

    //是否优化动态建筑显示
    public bool useOptimizeDynamicBuild = true;

    //是否优化静态特效显示
    public bool useOptimizeStaticEffect = true;

    //是否优化动态模型显示
    public bool useOptimizeDynamicModel = true;

    //相机视截体
    public Plane[] planes;
    private Camera mainCamera = null;

    public bool CameraMoving;
    public Transform mTransform = null;



	// Use this for initialization
	void Start ()
    {
        mainCamera = Camera.main;

        //Debug.Log("start camera name" + mainCamera.name);
	}

    //[yaz]不需要每帧更新
//     void FixedUpdate()
//     {
//         //FixedUpdatePosition();
//     }

    //获取Transform
    public Transform GetTransform()
    {
        if (mTransform == null)
        {
            mTransform = gameObject.transform;
        }
        return mTransform;
    }


    //创建视平截体
    public void RefreshFrustumPlanes()
    {
        if (!useOptimizeDynamicBuild)
            return;

        if (mainCamera == null)
            return;

        planes = GeometryUtility.CalculateFrustumPlanes(mainCamera);        
    }
  
    //检测包围盒是否可见
    public bool IsInFrustum(Plane[] planes,Bounds bound)
    {
        return GeometryUtility.TestPlanesAABB(planes, bound);
    }
	
	// Update is called once per frame
    public void FixedUpdatePosition()
    {
        // Early out if we don't have a target
        if (target == null || CameraMoving)
        {
            return;
        }

        //相机位置不随玩家位置高度，高度60固定
        Vector3 targetPos = new Vector3(target.position.x, 60, target.position.z);


        // Calculate the current rotation angles
        float wantedRotationAngle = target.eulerAngles.y;
        float wantedHeight = targetPos.y + height;

        float currentRotationAngle = transform.eulerAngles.y;
        float currentHeight = transform.position.y;

        // Damp the rotation around the y-axis
        currentRotationAngle = Mathf.LerpAngle(currentRotationAngle, wantedRotationAngle, rotationDamping * Time.deltaTime);

        // Damp the height
        currentHeight = Mathf.Lerp(currentHeight, wantedHeight, heightDamping * Time.deltaTime);

        // Convert the angle into a rotation

        Quaternion wantRotation = Quaternion.Euler(0, currentRotationAngle, 0);
        //Debug.Log("wantRotation  " + wantRotation);
        //Quaternion nowRotation = this.transform.rotation;
        //Quaternion midQua = wantRotation;
        ////Debug.Log("Quaternion.Angle(wantRotation, nowRotation)  " + Quaternion.Angle(wantRotation, nowRotation));
        //if (Quaternion.Angle(wantRotation, nowRotation) >= 10.0f)
        //{
        //    //this.transform.rotation = wantRotation;
        //}
        //else {
        //    midQua = Quaternion.Lerp(nowRotation, wantRotation, 2.0f * Time.deltaTime);
        //}
        //Quaternion currentRotation = Quaternion.Euler(0, currentRotationAngle, 0);

        // Set the position of the camera on the x-z plane to:
        // distance meters behind the target
        Vector3 wantPos = targetPos - wantRotation * Vector3.forward * distance;
        wantPos.y = currentHeight;
        Vector3 nowPos = GetTransform().position;
        float mtoDis = Vector3.Distance(wantPos, nowPos);
        GetTransform().position = wantPos;
        Vector3 dir = targetPos - GetTransform().position;
        dir.Normalize();
        GetTransform().rotation = Quaternion.LookRotation(dir);
        //transform.LookAt(target);      

        RefreshFrustumPlanes();

        //检测是否有重复更新
        if (curFrame == Time.frameCount)
        {
            Debug.LogError("update more than once in one frame!");
        }
        curFrame = Time.frameCount;
    }
}
