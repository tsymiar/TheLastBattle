using UnityEngine;
using System.Collections;

public class UIMessageTip : MonoBehaviour {
	
	UILabel DescriptionLabel;
	// Use this for initialization
	float restTime = 0f;
	void Start () {
	}
	
	// Update is called once per frame
	void Update () {
		restTime -= Time.deltaTime;
		if(restTime < 0){
			this.transform.parent = null;
			NGUITools.Destroy(this.gameObject);
		}
	}
	void  init()
	{
		DescriptionLabel = this.transform.Find ("Label").GetComponent<UILabel>();
		
	}
	void OnEnable()
	{
	}
	public void SetCapionInfo(string showInfo, float time)
	{
		restTime = time;
		init ();
		DescriptionLabel.text = showInfo;
	}
}
