using UnityEngine;
using System.Collections;
using GameDefine;
public class GameLogin : MonoBehaviour {

    public static GameLogin Instance
    {
        private set;
        get;
    }

    void OnEnable()
    {
        Instance = this; 

    }

    void OnDisable()
    {
        Instance = null;
    }

	// Use this for initialization
	void Start () {
	
	}	
	 
}
