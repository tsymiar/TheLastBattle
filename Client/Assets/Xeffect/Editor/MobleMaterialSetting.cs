using UnityEngine;
using System.Collections;
using UnityEditor;

//Author: kuku小夭. Many Thanks:)

public class MobleMaterialSetting : EditorWindow
{
    Shader shader_mo_1 = Shader.Find("Mobile/Diffuse");
    Shader shader_mo_2 = Shader.Find("Mobile/Particles/Additive");
    Shader shader_mo_3 = Shader.Find("Mobile/Particles/Alpha Blended");

    Shader shader_pc_1 = Shader.Find("Diffuse");
    Shader shader_pc_2 = Shader.Find("Particles/Additive");
    Shader shader_pc_3 = Shader.Find("Particles/Alpha Blended");

    [@MenuItem("Xffect/Mobile Material Setting")]
    private static void Init()
    {
        MobleMaterialSetting window = (MobleMaterialSetting)GetWindow(typeof

        (MobleMaterialSetting), true, "MobileMaterialSetting");
        window.Show();
    }

    // 显示窗体里面的内容
    private void OnGUI()
    {
        GUILayout.BeginHorizontal();
        GUILayout.Label("Mobile Material Setting");
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal();
        GUILayout.Label("these functions will change all the materials in a selected folder.");
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal();
        GUILayout.Label("please make a backup before using it.");
        GUILayout.EndHorizontal();

        GUILayout.Label(" ");
        if (GUILayout.Button("Diffuse -> Mobile/Diffuse"))
            LoopSetMaterials(shader_pc_1, shader_mo_1);

        GUILayout.Label(" ");
        if (GUILayout.Button("Particles/Additive -> Mobile/Particles/Additive"))
            LoopSetMaterials(shader_pc_2, shader_mo_2);

        GUILayout.Label(" ");
        if (GUILayout.Button("Particles/Alpha Blended -> Mobile/Particles/Alpha Blended"))
            LoopSetMaterials(shader_pc_3, shader_mo_3);
    }

    private void LoopSetMaterials(Shader old_shader, Shader new_shader)
    {
        Object[] materials = GetSelectedMaterials();

        if (materials.Length == 0)
            Debug.LogWarning("can't find any correspond materials in selected folder.");

        Selection.objects = new Object[0];

        foreach (Material m in materials)
        {
            if (m.shader == old_shader)
            {
                m.shader = new_shader;
            }
        }
    }

    private Object[] GetSelectedMaterials()
    {
        return Selection.GetFiltered(typeof(Material), SelectionMode.DeepAssets);
    }
}