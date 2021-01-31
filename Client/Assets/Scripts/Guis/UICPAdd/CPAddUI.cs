using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using BlGame.GameEntity;

public class CPAddUI : MonoBehaviour
{
    List<UISprite> m_listsprite = new List<UISprite>();
    float m_addy = 50f;
    TweenAlpha itwee;
    

    void Awake()
    {
        m_listsprite.Add(transform.FindChild("cpAdd_1").GetComponent<UISprite>());
        m_listsprite.Add(transform.FindChild("cpAdd_2").GetComponent<UISprite>());
        m_listsprite.Add(transform.FindChild("cpAdd_3").GetComponent<UISprite>());
    }

    public void Init()
    {
        transform.gameObject.SetActive(true);
        foreach (UISprite sprite in m_listsprite)
        {
            sprite.gameObject.SetActive(false);
        }
    }
    GameObject tempCp;

    public void SetCPAdd_Count(string count,GameObject temp)
    {
        tempCp = temp;
        for (int i = 0; i < count.Length && i < m_listsprite.Count; i++)
        {
            string str = count.Substring(i, 1);
            m_listsprite[i].spriteName = str;
            m_listsprite[i].gameObject.SetActive(true);
        }
        temp.gameObject.SetActive(true);
        BlGame.Effect.EffectManager.Instance.CreateNormalEffect("effect/other/soul_death", PlayerManager.Instance.LocalPlayer.RealEntity.objAttackPoint.gameObject);
        TweenPosition.Begin(temp.gameObject, 0.8f, new Vector3(temp.transform.localPosition.x, temp.transform.localPosition.y + m_addy, temp.transform.localPosition.z));
        itwee = TweenAlpha.Begin(temp.gameObject, 0.8f, 0f);
        itwee.from = 1.0f;
        itwee.to = 0;
        itwee.style = TweenAlpha.Style.Once;
        EventDelegate.Add(itwee.onFinished, FinishMove, true);
    }

    void FinishMove()
    {
        EventDelegate.Remove(itwee.onFinished, FinishMove); 
        itwee.duration = 1.0f;
        itwee.value = 1.0f;

        Reset();
    }

    void Reset()
    {
        tempCp.SetActive(false);
        tempCp.transform.localScale = Vector3.one;
        tempCp.transform.localPosition = Vector3.zero;
    }

}
