using UnityEngine;
using System.Collections;

public class UIFunc_Strech : MonoBehaviour
{
	public GameObject m_pcStrechBaseObj;
	public float m_fStrechRate = 1.0f;
	// Use this for initialization
	void Start () {
		if (null == m_pcStrechBaseObj)
		{
			return;
		}
		Vector3 sTempVec3 = Vector3.zero;
		sTempVec3.x = Screen.width;;
		sTempVec3.y = Screen.height;;
		m_pcStrechBaseObj.transform.localScale = sTempVec3;
		
		
		BoxCollider pcBoxCollider = m_pcStrechBaseObj.GetComponent<BoxCollider>();
		if (null != pcBoxCollider)
		{
			sTempVec3.x = Screen.width;
			sTempVec3.y = Screen.height;
			sTempVec3.z = 1;
			pcBoxCollider.size = sTempVec3;
		}
//		Vector3 cBaseObjScale = m_pcStrechBaseObj.transform.localScale;
//		float fWidth = Screen.width;
//		float fHight = Screen.height;
//		float fXScale = fWidth / cBaseObjScale.x;
//		float fYScale = fHight / cBaseObjScale.y;
//		fXScale = fXScale * m_fStrechRate;
//		fYScale = fYScale * m_fStrechRate;
//		
//		Vector3 sTempVec3 = Vector3.zero;
////		sTempVec3.x = fXScale;
////		sTempVec3.y = fYScale;
//		sTempVec3.x = Screen.width;
//		sTempVec3.y = Screen.height;
//		transform.localScale = sTempVec3;
//		
//		BoxCollider pcBoxCollider = transform.GetComponent<BoxCollider>();
//		if (null != pcBoxCollider)
//		{
//			sTempVec3.x = fXScale * cBaseObjScale.x;
//			sTempVec3.y = fYScale * cBaseObjScale.y;
//			sTempVec3.z = 1;
//			pcBoxCollider.size = sTempVec3;
//		}
	}
	
	// Update is called once per frame
	void Update () {
	
	}
	
	void OnEnable()
	{
		if (null == m_pcStrechBaseObj)
		{
			return;
		}
		
		Vector3 sTempVec3 = Vector3.zero;
		sTempVec3.x = Screen.width;;
		sTempVec3.y = Screen.height;;
		m_pcStrechBaseObj.transform.localScale = sTempVec3;
		BoxCollider pcBoxCollider = m_pcStrechBaseObj.GetComponent<BoxCollider>();
		if (null != pcBoxCollider)
		{
			sTempVec3.x = Screen.width;
			sTempVec3.y = Screen.height;
			sTempVec3.z = 1;
			pcBoxCollider.size = sTempVec3;
		}
		
//		Vector3 cBaseObjScale = m_pcStrechBaseObj.transform.localScale;
//		float fWidth = Screen.width;
//		float fHight = Screen.height;
//		float fXScale = fWidth / cBaseObjScale.x;
//		float fYScale = fHight / cBaseObjScale.y;
//		fXScale = fXScale * m_fStrechRate;
//		fYScale = fYScale * m_fStrechRate;
//		
//		Vector3 sTempVec3 = Vector3.zero;
//		sTempVec3.x = fXScale;
//		sTempVec3.y = fYScale;
//		transform.localScale = sTempVec3;
//		
//		BoxCollider pcBoxCollider = transform.GetComponent<BoxCollider>();
//		if (null != pcBoxCollider)
//		{
//			sTempVec3.x = fXScale * cBaseObjScale.x;
//			sTempVec3.y = fYScale * cBaseObjScale.y;
//			sTempVec3.z = 1;
//			pcBoxCollider.size = sTempVec3;
//		}
	}
	
	void OnDisable()
	{
		
	}
	
}
