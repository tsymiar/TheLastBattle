using UnityEngine;

/// <summary>
/// Attach to a game object to make its position always lag behind its parent as the parent moves.
/// </summary>

[AddComponentMenu("NGUI/Examples/Lag Position")]
public class LagPosition : MonoBehaviour
{
	public Vector3 speed = new Vector3(10f, 10f, 10f);
	public bool ignoreTimeScale = false;

	Transform mTrans;
	Vector3 mRelative;
	Vector3 mAbsolute;

	public void OnRepositionEnd ()
	{
		Interpolate(1000f);
	}

	void Interpolate (float delta)
	{
		Transform parent = mTrans.parent;

		if (parent != null)
		{
			Vector3 target = parent.position + parent.rotation * mRelative;
			mAbsolute.x = Mathf.Lerp(mAbsolute.x, target.x, Mathf.Clamp01(delta * speed.x));
			mAbsolute.y = Mathf.Lerp(mAbsolute.y, target.y, Mathf.Clamp01(delta * speed.y));
			mAbsolute.z = Mathf.Lerp(mAbsolute.z, target.z, Mathf.Clamp01(delta * speed.z));
			mTrans.position = mAbsolute;
		}
	}

	void OnEnable ()
	{
		mTrans = transform;
		mAbsolute = mTrans.position;
		mRelative = mTrans.localPosition;
	}

	void Update ()
	{
		Interpolate(ignoreTimeScale ? RealTime.deltaTime : Time.deltaTime);
	}
}
