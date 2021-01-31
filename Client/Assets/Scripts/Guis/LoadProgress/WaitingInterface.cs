using UnityEngine;
using System.Collections;

public class WaitingInterface : MonoBehaviour {


    /// <summary>
    /// 连接实例
    /// </summary>
    public static WaitingInterface Instance
    {
        private set;
        get;
    }

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}

    void OnEnable()
    {
        Instance = this;
        EventCenter.AddListener(EGameEvent.eGameEvent_EndWaiting, DestorySelf);   
    }

    void OnDisble()
    {
        EventCenter.RemoveListener(EGameEvent.eGameEvent_EndWaiting, DestorySelf);   
    }

    void OnDestroy()
    {
        EventCenter.RemoveListener(EGameEvent.eGameEvent_EndWaiting, DestorySelf);   
    }

    void DestorySelf()
    {
         DestroyImmediate(gameObject);
    }
}
