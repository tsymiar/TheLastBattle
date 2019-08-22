using UnityEngine;

/// <summary>
/// Attach to a game object to make its rotation always lag behind its parent as the parent rotates.
/// </summary>

[AddComponentMenu("NGUI/Examples/Lag Rotation")]
public class LagRotation : MonoBehaviour
{
	public float speed = 10f;
	public bool ignoreTimeScale = false;

	Transform mTrans;
	Quaternion mRelative;
	Quaternion mAbsolute;

	public void OnRepositionEnd ()
	{
		Interpolate(1000f);
	}

	void Interpolate (float delta)
	{
		Transform parent = mTrans.parent;

		if (parent != null)
		{
			mAbsolute = Quaternion.Slerp(mAbsolute, parent.rotation * mRelative, delta * speed);
			mTrans.rotation = mAbsolute;
		}
	}

	void OnEnable ()
	{
		mTrans = transform;
		mRelative = mTrans.localRotation;
		mAbsolute = mTrans.rotation;
	}

	void Update ()
	{
		Interpolate(ignoreTimeScale ? RealTime.deltaTime : Time.deltaTime);
	}
}
