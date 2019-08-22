using UnityEngine;
using UnityEditor;
using System.Collections;

[CustomEditor(typeof(fyDragRibbon))]
public class fyDragRibbonEditor : fyRibbonEditor
{
    fyDragRibbon mDragRibbon = null;
    bool bShowDragInfo = true;
    //Enable的时候
    public void OnEnable()
    {
        mDragRibbon = (fyDragRibbon)target;
    }

    
    public override void OnInspectorGUI()
    {
        bShowDragInfo = EditorGUILayout.Foldout(bShowDragInfo, "拖拽丝带信息");

        if (bShowDragInfo)
        {
            GUILayout.BeginVertical();
            mDragRibbon.mSpeed1 = EditorGUILayout.FloatField("前进速度", mDragRibbon.mSpeed1);
            mDragRibbon.mSpeed2 = EditorGUILayout.FloatField("后退速度", mDragRibbon.mSpeed2);
            mDragRibbon.mMaxLength = EditorGUILayout.FloatField("最大长度", mDragRibbon.mMaxLength);
            GUILayout.EndVertical();
        }
            
        base.OverrideGUI(mDragRibbon);


        //开始按钮 
        if (GUILayout.Button("Run"))
        {
            mDragRibbon.Reset();
            //重置并Link
            
            //mDragRibbon.Link(mDragRibbon.gameObject.transform);
            mDragRibbon.Link(mDragRibbon.gameObject.transform);
            mDragRibbon.renderObj.SetActive(true);
            
        }
        else if (GUILayout.Button("Back"))
        {
            //强制回退
            mDragRibbon.ForeceBack();

        }

     
   
        
        SceneView.RepaintAll();
    }
}
