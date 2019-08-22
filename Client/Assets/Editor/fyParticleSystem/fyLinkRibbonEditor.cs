using UnityEngine;
using UnityEditor;
using System.Collections;

[CustomEditor(typeof(fyLinkRibbon))]
public class fyLinkRibbonEditor : fyRibbonEditor
{
    fyLinkRibbon mLinkRibbon = null;
    //Enable的时候
    public void OnEnable()
    {
        mLinkRibbon = (fyLinkRibbon)target;
    }


   
    public override void OnInspectorGUI()
    {

        //target
        GUILayout.BeginHorizontal();
        GUILayout.Label("目标", GUILayout.Width(130));
        mLinkRibbon.target = EditorGUILayout.ObjectField("", mLinkRibbon.target, typeof(Transform), true) as Transform;
        GUILayout.EndHorizontal();

        base.OverrideGUI(mLinkRibbon);

       
        //开始按钮 
        if (GUILayout.Button("Run"))
        {
            //重置并Link
            mLinkRibbon.Reset();
            mLinkRibbon.Link(mLinkRibbon.target);
        }
        SceneView.RepaintAll();
    }
}
