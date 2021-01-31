//----------------------------------------------
//            Xffect Editor
// Copyright © 2012- Shallway Studio
// http://shallway.net
//----------------------------------------------
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

#if UNITY_EDITOR
using UnityEditor;
#endif

using Xft;

[AddComponentMenu("XffectComponent")]
[ExecuteInEditMode]
public class XffectComponent : MonoBehaviour
{

    Dictionary<string, VertexPool> MatDic = new Dictionary<string, VertexPool>();
    List<EffectLayer> EflList = new List<EffectLayer>();
    List<XftEventComponent> EventList = new List<XftEventComponent>();

    //Editable
    public float LifeTime = -1f;
    
    public bool IgnoreTimeScale = false;
    
    protected float ElapsedTime = 0f;

    protected bool Initialized = false;
    protected ArrayList MeshList = new ArrayList();

    protected double LastTime = 0f;
    protected double CurTime = 0f;

    public bool EditView = false;

    //added version 1.2.2
    public float Scale = 1f;

    void Awake()
    {
        if (!Initialized)
            Initialize();
    }

    #region edit mode functions
#if UNITY_EDITOR
    public void EnableEditView()
    {
        if (!EditorApplication.isPlaying)
        {
            EditView = true;
        }
        
    }

    public void DisableEditView()
    {
        if (!EditorApplication.isPlaying)
        {
            EditView = false;
            ClearEditModeMeshs();

            //let the update to know its disabled when re enabled.
            EflList.Clear();
			ResetEditorEvents();
			EventList.Clear();
        }
    }


    public void ResetEditScene()
    {
        if (!IsActive(gameObject))
            Active();
        EflList.Clear();
        EventList.Clear();
        MatDic.Clear();
        MeshList.Clear();
        ClearEditModeMeshs();
        Initialize();
        Reset();
        Start();
        ElapsedTime = 0f;

    }

    public bool CheckEditModeInited()
    {
        if (EflList.Count == 0)
            return false;
        foreach (EffectLayer el in EflList)
        {
            //ugly trick....
            if (el.emitter == null)
                return false;
        }
        return true;
    }

    public void ClearEditModeMeshs()
    {
        ArrayList tobeDelete = new ArrayList();
        foreach (Transform child in transform)
        {

            if (!child.name.Contains("xftmesh"))
                continue;

            MeshFilter mf = (MeshFilter)child.GetComponent(typeof(MeshFilter));
            if (mf != null)
            {
                tobeDelete.Add(mf.gameObject);
            }
        }
  
        //dangerious code, do not put your own mesh assets to the Xffect Object's children.
        foreach (GameObject obj in tobeDelete)
        {
            GameObject.DestroyImmediate(obj,true);
        }
    }

#endif
    #endregion
	
	//in editor mode, Awake() method called twice, why?.
    public void Initialize()
    {

        if (EflList.Count > 0)
        {//already inited.
            return;
        }
        //fixed : 2012.5.25. can't add child game object during foreach..
        //ArrayList tobeAdded = new ArrayList();
        
        foreach (Transform child in transform)
        {
            EffectLayer el = (EffectLayer)child.GetComponent(typeof(EffectLayer));
            if (el == null)
                continue;

            if (el.Material == null)
            {
                Debug.LogWarning("effect layer: " + el.gameObject.name + " has no material, please assign a material first!");
                continue;
            }
   
            SetActive(el.gameObject,true);
            
            Material mat = el.Material;
            //ver 1.2.1
            mat.renderQueue = mat.shader.renderQueue;
            mat.renderQueue += el.Depth;
            EflList.Add(el);
            MeshFilter Meshfilter;
            MeshRenderer Meshrenderer;

            Transform oldMesh = transform.Find("xftmesh " + mat.name);
            if (oldMesh != null)
            {//already instaniate by object cache.just recreate vertex pool.\
                Meshfilter = (MeshFilter)oldMesh.GetComponent(typeof(MeshFilter));
                Meshrenderer = (MeshRenderer)oldMesh.GetComponent(typeof(MeshRenderer));
                Meshfilter.sharedMesh = new Mesh();
                Meshfilter.sharedMesh.Clear();
                MatDic[mat.name] = new VertexPool(Meshfilter.sharedMesh, mat);
                //fixed 2012.7.20 if already exist, Add to MeshList too.
                if (!MeshList.Contains(oldMesh.gameObject))
                    MeshList.Add(oldMesh.gameObject);

            }
            if (!MatDic.ContainsKey(mat.name))
            {
                GameObject obj = new GameObject("xftmesh " + mat.name);
                obj.layer = gameObject.layer;
                //obj.transform.parent = this.transform;
                //tobeAdded.Add(obj);
                MeshList.Add(obj);
                obj.AddComponent("MeshFilter");
                obj.AddComponent("MeshRenderer");
                SetActive(obj,IsActive(gameObject));
    
#if UNITY_EDITOR
                if (!EditorApplication.isPlaying)
                {
                    XffectComponent.SetActive(obj,true);
                }
#endif
                
                
                Meshfilter = (MeshFilter)obj.GetComponent(typeof(MeshFilter));
                Meshrenderer = (MeshRenderer)obj.GetComponent(typeof(MeshRenderer));
                Meshrenderer.castShadows = false;
                Meshrenderer.receiveShadows = false;
                Meshrenderer.renderer.sharedMaterial = mat;
                Meshfilter.sharedMesh = new Mesh();
                MatDic[mat.name] = new VertexPool(Meshfilter.sharedMesh, mat);
            }
        }

        //now set each gameobject's parent.
        foreach (GameObject obj in MeshList)
        {
            obj.transform.parent = this.transform;

            //fixed 2012.6.25, 
            obj.transform.position = Vector3.zero;
            obj.transform.rotation = Quaternion.identity;

            //fixed 2012.7.11, avoid the lossy scale influence the mesh object.
            Vector3 realLocalScale = Vector3.zero;
            realLocalScale.x = 1 / obj.transform.parent.lossyScale.x;
            realLocalScale.y = 1 / obj.transform.parent.lossyScale.y;
            realLocalScale.z = 1 / obj.transform.parent.lossyScale.z;
            
            obj.transform.localScale = realLocalScale * Scale;
        }


        //start each effect layer.
        foreach (EffectLayer efl in EflList)
        {
            efl.Vertexpool = MatDic[efl.Material.name];
        }
        //this.transform.position = Vector3.zero;
        //this.transform.rotation = Quaternion.identity;
        this.transform.localScale = Vector3.one;

        foreach (EffectLayer el in EflList)
        {
            el.StartCustom();
        }

        //add events
		EventList.Clear();
        foreach (Transform child in transform)
        {
            XftEventComponent xftevent = child.GetComponent<XftEventComponent>();
            if (xftevent == null)
                continue;
            EventList.Add(xftevent);
            xftevent.Initialize();
        }
        Initialized = true;
    }
    void Start()
    {
        LastTime = Time.realtimeSinceStartup;
#if UNITY_EDITOR
        if (!EditorApplication.isPlaying)
        {
            LastTime = (float)EditorApplication.timeSinceStartup;
        }
#endif
    }

    public void Update()
    {
        CurTime = Time.realtimeSinceStartup;
#if UNITY_EDITOR
        if (!EditorApplication.isPlaying)
        { 
            if (!EditView)
                return;
            CurTime = EditorApplication.timeSinceStartup;
            if (!CheckEditModeInited())
                ResetEditScene();
        }
#endif
        float deltaTime = (float)(CurTime - LastTime);
        
        if (!IgnoreTimeScale)
            deltaTime *= Time.timeScale;
		
        ElapsedTime += deltaTime;
        foreach (EffectLayer el in EflList)
        {
			if(el == null) continue;
            if (ElapsedTime > el.StartTime && IsActive(el.gameObject))
            {
                el.FixedUpdateCustom(deltaTime);
            }
        }
		
        for (int i = 0; i < EventList.Count; i++)
        {
            XftEventComponent e = EventList[i];
            if (IsActive(e.gameObject))
            {
                e.UpdateCustom(deltaTime);
            }
        }

        LastTime = CurTime;

#if UNITY_EDITOR
        if (!EditorApplication.isPlaying)
        {
            LateUpdate();
        }
#endif
    }
	
	
	public void ResetEditorEvents()
	{
		foreach (XftEventComponent e in EventList)
		{
			e.ResetCustom();
			e.EditorDone();
		}
	}
	
    public void DoFinish()
    {
#if UNITY_EDITOR
        if (!EditorApplication.isPlaying)
        {
            ClearEditModeMeshs();
            foreach (EffectLayer el in EflList)
            {
                el.Reset();
            }
			ResetEditorEvents();
            ElapsedTime = 0f;
            return;
        }
#endif
        foreach (EffectLayer el in EflList)
        {
            el.Reset();
        }
        DeActive();
        ElapsedTime = 0f;
    }


    void LateUpdate()
    {
        //fixed 2012.6.25. if client==xffectComponent, make sure each mesh position is always zero.
        foreach (GameObject obj in MeshList)
        {
            if (obj != null)
            {
                //fixed ver 2.0.2, we only need the mesh's world position to be zero.
                obj.transform.position = Vector3.zero;
                obj.transform.rotation = Quaternion.identity;
                //fixed 2012.7.11 avoid the lossy scale to influence the position and scale
                //Vector3 realLocalPos = Vector3.zero;
                //realLocalPos.x = obj.transform.localPosition.x * obj.transform.lossyScale.x;
                //realLocalPos.y = obj.transform.localPosition.y * obj.transform.lossyScale.y;
                //realLocalPos.z = obj.transform.localPosition.z * obj.transform.lossyScale.z;
                //obj.transform.localPosition = realLocalPos;

                //fixed, ver 1.2.3
                //obj.transform.rotation = Quaternion.identity;
            }
                
        }

        foreach (KeyValuePair<string, VertexPool> pair in MatDic)
        {
            pair.Value.LateUpdate();
        }
        if (ElapsedTime > LifeTime && LifeTime >= 0)
        {
            DoFinish();
        }
        else if (LifeTime < 0 && EflList.Count > 0)
        {
            //added. 2012.6.3
            //Xffect LifeTime < 0， 且又是EmitByRate的话，会自动判断是否已经没有活动节点，没有则自动Deactive()。
            float deltaTime = (float)(CurTime - LastTime);
            bool allEfLFinished = true;
            foreach (EffectLayer el in EflList)
            {
                if (!el.EmitOver(deltaTime))
                {
                    allEfLFinished = false;
                }
            }
            if (allEfLFinished)
            {
                DoFinish();
            }
        }
    }

    void OnDrawGizmosSelected()
    {
        //foreach (KeyValuePair<string, VertexPool> pair in MatDic)
        {
            //Mesh mesh = pair.Value.Mesh;
            //Gizmos.DrawWireCube(tempre.bounds.center, tempre.bounds.size);
            //Debug.Log(mesh.bounds.size);
        }
    }

    #region APIs
 
    
    public void ResetCamera(Camera cam)
    {
        foreach (EffectLayer el in EflList)
        {
            el.ResetCamera(cam);
        }
    }
    
    //set the client transform
    public void SetClient(Transform client)
    {
        foreach (EffectLayer el in EflList)
        {
            el.SetClient(client);
        }
    }

    //set Direction,  Not recommended, you can change the client's rotation to affect the direction.
    public void SetDirectionAxis(Vector3 axis)
    {
        foreach (EffectLayer el in EflList)
        {
            el.OriVelocityAxis = axis;
        }
    }

    public void SetEmitPosition(Vector3 pos)
    {
        foreach (EffectLayer el in EflList)
        {
            el.EmitPoint = pos;
        }
    }

    public void SetCollisionGoalPos(Transform pos)
    {
        foreach (EffectLayer el in EflList)
        {
            if (el.UseCollisionDetection)
                el.SetCollisionGoalPos(pos);
        }
    }

    public void SetCollisionGoalPos(Transform pos, string eflName)
    {
        foreach (EffectLayer el in EflList)
        {
            if (el.gameObject.name == eflName && el.UseCollisionDetection)
                el.SetCollisionGoalPos(pos);
        }
    }
    
    
    public void SetArractionAffectorGoal(Transform goal)
    {
        foreach (EffectLayer el in EflList)
        {
            if (el.GravityAffectorEnable)
                el.SetArractionAffectorGoal(goal);
        }
    }
    
    public void SetArractionAffectorGoal(Transform goal, string eflName)
    {
        foreach (EffectLayer el in EflList)
        {
            if (el.gameObject.name == eflName && el.GravityAffectorEnable)
                el.SetArractionAffectorGoal(goal);
        }
    }
    
    
    public void SetScale(Vector2 scale, string eflName)
    {
        foreach (EffectLayer el in EflList)
        {
            if (el.gameObject.name == eflName)
                el.SetScale(scale);
        }
    }
 
    //only support one effect layer.
    public EffectNode EmitByPos(Vector3 pos)
    {
        if (EflList.Count > 1)
            Debug.LogWarning("EmitByPos only support one effect layer!");
        EffectLayer el = EflList[0];
        return el.EmitByPos(pos);
    }

    public void Reset()
    {
        ElapsedTime = 0f;

        //reset times
        Start();
        foreach (EffectLayer el in EflList)
        {
            el.Reset();
        }

        foreach (XftEventComponent e in EventList)
        {
            e.ResetCustom();
        }
    }
 
    //deactive this xffect smoothly.
    public void StopEmit()
    {
        foreach (EffectLayer el in EflList)
        {
            el.StopEmit();
        }
    }
    
    public void DeActive()
    {
        if (!IsActive(gameObject))
            return;

        foreach (XftEventComponent e in EventList)
        {
            e.ResetCustom();
        }

        foreach (Transform child in transform)
        {
            SetActive(child.gameObject,false);
        }
        SetActive(gameObject,false);
    }

    public void Active()
    {
        if (!Initialized)
        {//may active by out caller, and not inited yet.
            Initialize();
        }

        if (IsActive(gameObject))
        {
            //already active, do nothing.
            return;
        }
        foreach (Transform child in transform)
        {
            SetActive(child.gameObject,true);
        }
        SetActive(gameObject,true);
        Reset();
    }
    #endregion
 
    #region for_unity4_compatible
    
    public static bool IsActive(GameObject obj)
    {
#if UNITY_4_0
        return obj.activeSelf;
#else
        return obj.active;
#endif
    }
    
    public static void SetActive(GameObject obj, bool flag)
    {
#if UNITY_4_0
        obj.SetActive(flag);
#else
        obj.active = flag;
#endif
    }
    
    #endregion
}
