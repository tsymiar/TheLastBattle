using UnityEngine;
using System.Collections;

public class CompositeXffect : MonoBehaviour {

    ArrayList XffectList = new ArrayList();
	void Awake()
    {
        Initialize();
	}

    public void Initialize()
    {
        //already initialized
        if (XffectList.Count > 0)
            return;
        foreach (Transform child in transform)
        {
            XffectComponent xft = child.GetComponent<XffectComponent>();
            if (xft == null)
                continue;
            xft.Initialize();
            XffectList.Add(xft);
        }
    }

    public void Active()
    {
        if (!XffectComponent.IsActive(gameObject))
            XffectComponent.SetActive(gameObject,true);
        foreach (XffectComponent xft in XffectList)
        {
            xft.Active();
        }
    }

    public void DeActive()
    {
        foreach (XffectComponent xft in XffectList)
        {
            xft.DeActive();
        }
    }

    void LateUpdate()
    {
        for (int i = 0; i < XffectList.Count; i++)
        {
            XffectComponent xft = (XffectComponent)XffectList[i];
            if (XffectComponent.IsActive(xft.gameObject))
                return;
        }
        //if all children is done, then set to non-active.
        XffectComponent.SetActive(gameObject,false);
    }
}
