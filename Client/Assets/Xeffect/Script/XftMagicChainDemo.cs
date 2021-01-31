using UnityEngine;
using System.Collections;

//see this thread to know more: http://shallway.net/forum.php?mod=viewthread&tid=90
[ExecuteInEditMode]
public class XftMagicChainDemo : MonoBehaviour 
{
    public XffectComponent MagicChain;
    public Transform Target;
    
    void Update()
    {
        //calculate the distance to target
        float distance = (Target.transform.position - MagicChain.transform.position).magnitude;
        
        //set corresponding scale of MagicChain
        Vector2 scale = new Vector2(1f,distance);
        //API SetScale()
        MagicChain.SetScale(scale,"line");
        
        //set rotation
        Vector3 direction = Target.transform.position - MagicChain.transform.position;
        MagicChain.transform.rotation = Quaternion.LookRotation(direction, Vector3.up);
    }
    
    
//    void OnGUI()
//    {
//        GUI.Label(new Rect(150, 0, 350, 25), "move around Sphere2 in the editor to see how it works!");
//    }
}
