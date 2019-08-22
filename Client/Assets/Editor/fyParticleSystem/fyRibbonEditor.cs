using UnityEngine;
using UnityEditor;
using System.Collections;


public class fyRibbonEditor : Editor
{
    private static bool bShowMatrix = true;
    private static bool bShowColor = false;
    private static bool bShowMatType = true;
    private static int bColorNum = 5;

    private static bool bShowRibbonInfo = true;
    private static Texture2D tex;
   



    public void OverrideGUI(fyRibbonTrail ribbon)
    {
        //水平样式设置开始
        GUILayout.BeginVertical();

        bShowRibbonInfo = EditorGUILayout.Foldout(bShowRibbonInfo, "丝带基本信息");
        if (bShowRibbonInfo)
        {
            //#region Ribbon最大段数
            EditorGUI.BeginChangeCheck();

            //maxElement
            GUILayout.BeginHorizontal();
            GUILayout.Label("最大段数", GUILayout.Width(130));
            ribbon.maxElement = EditorGUILayout.IntField(ribbon.maxElement);
            GUILayout.EndHorizontal();

            //segLength
            GUILayout.BeginHorizontal();
            GUILayout.Label("每段长度", GUILayout.Width(130));
            ribbon.segLength = EditorGUILayout.FloatField(ribbon.segLength);
            GUILayout.EndHorizontal();

            //segWidth
            GUILayout.BeginHorizontal();
            GUILayout.Label("每段宽度", GUILayout.Width(130));
            ribbon.segWidth = EditorGUILayout.FloatField(ribbon.segWidth);
            GUILayout.EndHorizontal();

            if (EditorGUI.EndChangeCheck())
            {
                //Debug.Log("the max element changed");
                ribbon.ReCreate();
            }


            //渲染混合类型
            EditorGUI.BeginChangeCheck();
            ribbon.mBlendType = (fyRibbonTrail.BlendType)EditorGUILayout.EnumPopup("渲染混合类型", ribbon.mBlendType);
            if (EditorGUI.EndChangeCheck())
            {
                bShowMatType = true;
                //Shader shader = null;
                if (ribbon.mBlendType == fyRibbonTrail.BlendType.Additive_Type)
                {
                    if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Common_Type || ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Repeat_Type)
                        ribbon.mShader = Shader.Find("fyShader/Additive");
                    else if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Sequence_Type)
                        ribbon.mShader = Shader.Find("fyShader/AdditiveSequence");
                }
                else if (ribbon.mBlendType == fyRibbonTrail.BlendType.Alpha_Type)
                {
                    if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Common_Type || ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Repeat_Type)
                        ribbon.mShader = Shader.Find("fyShader/Alpha");
                    else if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Sequence_Type)
                        ribbon.mShader = Shader.Find("fyShader/AlphaSequence");
                }
                else if (ribbon.mBlendType == fyRibbonTrail.BlendType.Replace_Type)
                {
                    if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Common_Type || ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Repeat_Type)
                        ribbon.mShader = Shader.Find("fyShader/Replace");
                    else if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Sequence_Type)
                        ribbon.mShader = Shader.Find("fyShader/ReplaceSequence");
                }
                else if (ribbon.mBlendType == fyRibbonTrail.BlendType.Distort_Additive)
                {
                    ribbon.mShader = Shader.Find("fyShader/fyDistortAdditive");
                    bShowMatType = false;
                }

                //编辑器
                if (!Application.isPlaying)
                    ribbon.renderer.sharedMaterial.shader = ribbon.mShader;
                //运行
                else
                    ribbon.renderer.material.shader = ribbon.mShader;
            }

            EditorGUI.BeginChangeCheck();
            ribbon.mOrientType = (fyRibbonTrail.OrientType)EditorGUILayout.EnumPopup("朝向类型", ribbon.mOrientType);

            if (ribbon.mOrientType == fyRibbonTrail.OrientType.World_Type)
                ribbon.mNormal = EditorGUILayout.Vector3Field("法线", ribbon.mNormal);

            if (EditorGUI.EndChangeCheck())
            {
                ribbon.UpdateRibbon(0);
            }
        }

      

        //材质寻址类型
        EditorGUI.BeginChangeCheck();
        if (bShowMatType)
            ribbon.mMaterialType = (fyRibbonTrail.MaterialAddressType)EditorGUILayout.EnumPopup("贴图寻址类型", ribbon.mMaterialType);

        //UV长度
        if(ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Repeat_Type)
        {
            ribbon.mUVLength = EditorGUILayout.FloatField("UV长度", ribbon.mUVLength);
        }

        //纹理变换
        bShowMatrix = EditorGUILayout.Foldout(bShowMatrix, "纹理旋转");
        if (bShowMatrix)
        {
            //矩阵变换
            EditorGUI.BeginChangeCheck();

            //初始平移
            GUILayout.BeginHorizontal();
            ribbon.mOffset = EditorGUILayout.Vector2Field("初始平移", ribbon.mOffset);
            GUILayout.EndHorizontal();

            //初始旋转
            GUILayout.BeginHorizontal();
            GUILayout.Label("初始旋转", GUILayout.Width(130));
            ribbon.mOffRot = EditorGUILayout.FloatField(ribbon.mOffRot);
            GUILayout.EndHorizontal();

            if (EditorGUI.EndChangeCheck())
            {
                //Debug.Log("1111");
                ribbon.InitMatrix();
            }

            //平移，旋转，缩放
            EditorGUI.BeginChangeCheck();

            //初始平移
            GUILayout.BeginHorizontal();
            ribbon.mTranslate = EditorGUILayout.Vector2Field("平移", ribbon.mTranslate);
            GUILayout.EndHorizontal();

            //初始旋转
            GUILayout.BeginHorizontal();
            ribbon.mRotate = EditorGUILayout.FloatField("旋转", ribbon.mRotate);
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            ribbon.mScale = EditorGUILayout.Vector2Field("缩放", ribbon.mScale);
            GUILayout.EndHorizontal();


            if (EditorGUI.EndChangeCheck())
            {
                ribbon.UpdateMatrix();
            }
        }

      



        if (EditorGUI.EndChangeCheck())
        {
            //Shader shader = null;
            if (ribbon.mBlendType == fyRibbonTrail.BlendType.Additive_Type)
            {
                if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Common_Type || ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Repeat_Type)
                    ribbon.mShader = Shader.Find("fyShader/Additive");
                else if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Sequence_Type)
                    ribbon.mShader = Shader.Find("fyShader/AdditiveSequence");
            }
            else if (ribbon.mBlendType == fyRibbonTrail.BlendType.Alpha_Type)
            {
                if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Common_Type || ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Repeat_Type)
                    ribbon.mShader = Shader.Find("fyShader/Alpha");
                else if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Sequence_Type)
                    ribbon.mShader = Shader.Find("fyShader/AlphaSequence");
            }
            else if (ribbon.mBlendType == fyRibbonTrail.BlendType.Replace_Type)
            {
                if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Common_Type || ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Repeat_Type)
                    ribbon.mShader = Shader.Find("fyShader/Replace");
                else if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Sequence_Type)
                    ribbon.mShader = Shader.Find("fyShader/ReplaceSequence");
            }

            //编辑器
            if (!Application.isPlaying)
                ribbon.renderer.sharedMaterial.shader = ribbon.mShader;
            //运行
            else
                ribbon.renderer.material.shader = ribbon.mShader;

            //普通寻址
            if (ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Common_Type)
            {
                bShowMatrix = true;
     
            }
            else if(ribbon.mMaterialType == fyRibbonTrail.MaterialAddressType.Repeat_Type)
            {
                bShowMatrix= true;
               
            }
            //序列帧
            else
            {
                bShowMatrix = true;
    
            }
        }
        //水平样式设置结束
        GUILayout.EndVertical();

        //颜色控制
        EditorGUI.BeginChangeCheck();

        bShowColor = EditorGUILayout.Foldout(bShowColor, "颜色控制");
        EditorGUI.BeginChangeCheck();
        bColorNum = EditorGUILayout.IntField("颜色个数:", bColorNum);
        if (EditorGUI.EndChangeCheck())
        {
            ribbon.colorKey = new ColorKey[bColorNum];
        }

        ribbon.mColorPeriodTime = EditorGUILayout.FloatField("颜色周期", ribbon.mColorPeriodTime);
        if (ribbon.mColorPeriodTime != 0)
            bShowColor = true;
        else
            bShowColor = false;

        //颜色控制
        if (bShowColor)
        {

            for (int i = 0; i < bColorNum; i++)
            {
                GUILayout.BeginHorizontal();
                if (ribbon.colorKey[i] != null)
                {
                    ribbon.colorKey[i].time = EditorGUILayout.FloatField("time:", ribbon.colorKey[i].time);
                    ribbon.colorKey[i].color = EditorGUILayout.ColorField(ribbon.colorKey[i].color);
                }
                GUILayout.EndHorizontal();
            }

        }

        if (EditorGUI.EndChangeCheck())
        {
        }

        EditorGUI.BeginChangeCheck();
        ribbon.mTexture = (Texture2D)EditorGUILayout.ObjectField("Image", ribbon.mTexture, typeof(Texture2D), false);
        if (EditorGUI.EndChangeCheck())
        {
            ribbon.ChangeTexture(ribbon.mTexture);
        }
    }


    public override void OnInspectorGUI()
    {      
    }
}
