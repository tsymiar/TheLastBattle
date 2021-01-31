using UnityEngine; 
using System;
using System.Collections;
using System.Collections.Generic; 
using Holoville.HOTween;
using BlGame.GuideDate;
public class CameraMoveManager :MonoBehaviour
{

    public static CameraMoveManager Instance = null;

    void Awake() {
        Instance = this;
    }

    public void SetPos(Vector3 pos) {
        transform.position = pos;
    }

    public void SetRotation(Quaternion qua) {
        transform.rotation = qua;
    }

    public Tweener SetMove(Vector3 targetPos, float during, EaseType type, Holoville.HOTween.Core.TweenDelegate.TweenCallback call)
    {
        TweenParms parms = new TweenParms();
        parms.Prop("position", targetPos);
        parms.OnComplete(call);
        Tweener tweener = HOTween.To(transform, during, parms);
        tweener.easeType = type;
        return tweener;
    }

    public void StopMove(Tweener tweener) {
        if (tweener == null)
            return;
        tweener.Kill();
        tweener = null;
    }

    
	 
}
