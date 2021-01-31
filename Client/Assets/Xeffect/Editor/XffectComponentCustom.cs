using UnityEngine;
using UnityEditor;
using Xft;
 
[CustomEditor(typeof(XffectComponent))]
public class XffectComponentCustom : Editor
{
	string LayerName = "EffectLayer";

	void OnDisable ()
	{
		if (Application.isEditor && !EditorApplication.isPlaying) {
			EditorApplication.update = null;
		}
	}

	public override void OnInspectorGUI ()
	{
		XffectComponent ctarget = (XffectComponent)target;

		if (Application.isEditor && !EditorApplication.isPlaying) {
			EditorApplication.update = ctarget.Update;
		}    

		EditorGUILayout.BeginVertical ();
		EditorGUILayout.Space ();
		EditorGUILayout.Separator ();
		ctarget.LifeTime = EditorGUILayout.FloatField ("life:", ctarget.LifeTime);
		EditorGUILayout.LabelField ("[life -1 means infinite.]");
  
        ctarget.IgnoreTimeScale = EditorGUILayout.Toggle("ignore time scale?",ctarget.IgnoreTimeScale);
        
		EditorGUILayout.Separator ();

		ctarget.Scale = EditorGUILayout.FloatField ("scale:", ctarget.Scale);
		EditorGUILayout.Separator ();

		if (GUILayout.Button ("Add Layer")) {
			GameObject layer = new GameObject (LayerName);
			EffectLayer efl = (EffectLayer)layer.AddComponent ("EffectLayer");
			layer.transform.parent = Selection.activeTransform;

			efl.transform.localPosition = Vector3.zero;
			//fixed 2012.6.25. default to effect layer object.
			efl.ClientTransform = efl.transform;
			efl.GravityObject = efl.transform;
			efl.BombObject = efl.transform;
			efl.TurbulenceObject = efl.transform;
			efl.AirObject = efl.transform;
			efl.VortexObj = efl.transform;
			efl.DirCenter = efl.transform;
		}
		
		
		if (GUILayout.Button ("Add Event")) {
			GameObject layer = new GameObject ("_Event");
			XftEventComponent xevent = (XftEventComponent)layer.AddComponent ("XftEventComponent");
			xevent.transform.parent = Selection.activeTransform;
			xevent.transform.localPosition = Vector3.zero;
			xevent.RadialBlurShader = Shader.Find ("Xffect/PP/radial_blur_new");
			xevent.GlowCompositeShader = Shader.Find ("Xffect/PP/glow_compose");
			xevent.GlowDownSampleShader = Shader.Find ("Xffect/PP/glow_downsample");
			xevent.GlowBlurShader = Shader.Find ("Xffect/PP/glow_conetap");
			xevent.RadialBlurObj = xevent.transform;
            xevent.ColorInverseShader = Shader.Find ("Xffect/PP/color_inverse");
		}
		
		EditorGUILayout.Separator ();
		ctarget.EditView = EditorGUILayout.Toggle ("update in editor?", ctarget.EditView);

		//prevent asset inspector to update.
		if (ctarget.EditView == true) {
			if (!XffectComponent.IsActive(ctarget.gameObject)) {
				ctarget.EditView = false;
				Debug.Log ("you should activate the xffect object: " + ctarget.gameObject.name + " before updating it in editor.");
			}
		}

		if (ctarget.EditView) {
			ctarget.EnableEditView ();
		} else {
			ctarget.DisableEditView ();
		}

		if (ctarget.EditView) {
			if (GUILayout.Button ("Reset Editor Xffect")) {
				ctarget.ResetEditScene ();
			}
		}
		EditorGUILayout.EndVertical ();

		if (GUI.changed) {
			EditorUtility.SetDirty (target);
		}
	}

	[MenuItem("GameObject/Create Other/Xffet Object")]
	[MenuItem("Xffect/Create Xffet Object")]
	static void DoCreateXffectObject ()
	{
		GameObject go = new GameObject ("XffectObj");
		go.transform.localScale = Vector3.one;
		go.transform.rotation = Quaternion.identity;
		go.AddComponent<XffectComponent> ();

		Selection.activeGameObject = go;

		GameObject layer = new GameObject ("EffectLayer");
		EffectLayer efl = (EffectLayer)layer.AddComponent ("EffectLayer");
		layer.transform.parent = go.transform;

		efl.transform.localPosition = Vector3.zero;
		//fixed 2012.6.25. default to effect layer object.
		efl.ClientTransform = efl.transform;
		efl.GravityObject = efl.transform;
		efl.BombObject = efl.transform;
		efl.TurbulenceObject = efl.transform;
		efl.AirObject = efl.transform;
		efl.VortexObj = efl.transform;
		efl.DirCenter = efl.transform;
        efl.DragObj = efl.transform;
	}
}
