using UnityEngine;
using System.Collections;

public class DbClickBotton : GuiBase {

    private float ClickTime
    {
        set;
        get;
    }

    void OnClick()
    {
        if (Handler != null)
        {
            Handler(PrIe);
        }
        if (ClickTime != 0 && Time.time - ClickTime < 0.3f)
        {
            if (DbHandler != null)
            {
                DbHandler(PrIe);
                ClickTime = 0;
                return;
            }
        }
        ClickTime = Time.time;

    } 

    public delegate void HandleOnClick(int ie);
    public HandleOnClick Handler;

    public delegate void HandleOnDbClick(int ie);
    public HandleOnDbClick DbHandler;

    /// <summary>
    /// 添加按钮双击事件
    /// </summary>
    /// <param name="ie"></param>
    /// <param name="handler"></param>
    public void AddListenerDb(int ie, HandleOnDbClick handler)
    {
        PrIe = ie;
        DbHandler += handler;
    }

    /// <summary>
    /// 添加按钮双击事件
    /// </summary>
    /// <param name="handler"></param>
    public void AddListenerDb(HandleOnDbClick handler)
    {
        DbHandler += handler;
    }

    public void RemoveListenerDb(HandleOnDbClick handler) {
        DbHandler -= handler;
    }

    /// <summary>
    /// 添加按钮监听
    /// </summary>
    /// <param name="ie"></param>
    /// <param name="handler"></param>
    public void AddListener(int ie, HandleOnClick handler)
    {
        PrIe = ie;
        Handler += handler;
    }

    /// <summary>
    /// 添加按钮监听
    /// </summary>
    /// <param name="handler"></param>
    public void AddListener(HandleOnClick handler)
    {
        Handler += handler;
    }

    public void RemoveListener(HandleOnClick handler)
    {
        Handler -= handler;
    }
}
