using UnityEngine;
using System.Collections;

public class Example_Mobile : MonoBehaviour 
{
    public XffectComponent[] Effects;
    
    
    void ResetAllEffects()
    {
        foreach(XffectComponent xft in Effects)
        {
            xft.Active();
        }
    }
    
    void Start()
    {
        ResetAllEffects();
    }
    
    void OnGUI()
    {
        if (GUI.Button(new Rect(10, 80, 120, 20), "Reset"))
        {
            ResetAllEffects();
        }
    }
}
