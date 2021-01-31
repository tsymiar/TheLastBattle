using UnityEngine;
using System.Collections;

public class UIViewer : MonoBehaviour {

    public static UIViewer Instance
    {
        get;
        private set;
    }

    void Awake()
    {
        Instance = this;
    }
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
