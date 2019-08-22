using UnityEngine;
using System.Collections;

public class ShowFPS : MonoBehaviour {

    public static ShowFPS Instance
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

	public float f_UpdateInterval = 0.5F;

    private float f_LastInterval;


    private int i_Frames = 0;
    private float f_Fps;

    private GUIStyle style = new GUIStyle();


    public float sSPing
    {
        set;
        get;
    }

    public float cSPing
    {
        set;
        get;
    }

    void Start() 
    {
        Application.targetFrameRate = 300;
        f_LastInterval = Time.realtimeSinceStartup;
        i_Frames = 0;

        style.fontSize = 10;
        style.normal.textColor = new Color(0, 255, 0, 255);
    }


    void OnGUI() 
    {

        GUI.Label(new Rect(0, 0, 200, 200), "FPS:" + f_Fps.ToString("f2"), style);
        GUI.Label(new Rect(0, 10, 200, 200), "sSPing:" + sSPing.ToString("f2"), style);
        GUI.Label(new Rect(0, 20, 200, 200), "cSPing:" + cSPing.ToString("f2"), style);
    }


    void Update() 
    {
        ++i_Frames;


        if (Time.realtimeSinceStartup > f_LastInterval + f_UpdateInterval) 
        {
            f_Fps = i_Frames / (Time.realtimeSinceStartup - f_LastInterval);


            i_Frames = 0;


            f_LastInterval = Time.realtimeSinceStartup;
        }
    }

}
