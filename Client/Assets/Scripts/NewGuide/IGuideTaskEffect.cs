using System;
using System.Collections.Generic;
using UnityEngine;
using GameDefine;
using BlGame.GameEntity;
using JT.FWW.Tools;
using System.Linq;
namespace BlGame.GuideDate
{
    public class IGuideTaskEffect
    {

        protected  int taskId = -1;

        protected IGuideData taskData = null;

        private GameObject objLabel = null;

        private GameObject objShake = null;

        private GameObject objFlash = null;

        private ButtonOnPress btnNext = null;

        private UILabel labelTip = null;

        private UILabel labelContent = null;

        private int countText = 0;

        protected IGuideManagerData pData = null;

        private GameObject objP = null;
        private GameObject objParent
        {
            get
            {
                if (objP == null)
                {
                    objP = LoadUiResource.LoadRes(GameMethod.GetUiCamera.transform, GameConstDefine.UIGuideRestPath);
                }
                return objP;
            }
        }

      

        public virtual void OnEnter()
        {
             if (!string.IsNullOrEmpty(taskData.TextPath) && taskData.TextContent != null
                && taskData.TextContent.Count != 0 && !string.IsNullOrEmpty(taskData.TextContent.ElementAt(0)))
            {
                 ShowTextTip();
            }

            if (!string.IsNullOrEmpty(taskData.ShakePath))
            {
                ShowShakeTip();
            }

            if (!string.IsNullOrEmpty(taskData.FlashPath))
            {
                ShowFlashTip();
            }
        }

        void ShowFlashTip() {
            objFlash = objParent.transform.FindChild(taskData.FlashPath).gameObject;
            TweenPosition.Begin(objFlash, 0f, taskData.FlashPos);
            objFlash.transform.localScale = Vector3.one;
            objFlash.gameObject.SetActive(true);
            UITweener tween = TweenPosition.Begin(objFlash, 0.5f, taskData.FlashDstPos);
            tween.method = UITweener.Method.EaseOut;
            tween.style = UITweener.Style.PingPong;
        }

        private const float shakeDuring = 0.5f;
        private Vector3 shakeScale = new Vector3(1.1f, 1.1f, 1.1f);
        void ShowShakeTip() {
            objShake = objParent.transform.FindChild(taskData.ShakePath).gameObject;
            objShake.transform.localPosition = taskData.ShakePos;
            objShake.transform.localScale = Vector3.one;
            objShake.gameObject.SetActive(true);
            //TweenScale.Begin(objShake, 0f, shakeScale);
           // TweenAlpha.Begin(objShake, 0f, 0f);

            UITweener tween = TweenAlpha.Begin(objShake, shakeDuring, 0f);
            tween.method = UITweener.Method.EaseOut;
            tween.style = UITweener.Style.PingPong;

      /*     tween = TweenScale.Begin(objShake, shakeDuring, shakeScale);
            tween.method = UITweener.Method.EaseOut;
            tween.style = UITweener.Style.PingPong;*/
        }

        protected virtual void ShowTextTip() {
            countText = 0;
            objLabel = objParent.transform.FindChild(taskData.TextPath).gameObject;
            objLabel.transform.localPosition = taskData.TextPos;
            objLabel.SetActive(true);
            labelContent = objLabel.transform.FindChild("Content").GetComponent<UILabel>();
            labelTip = objLabel.transform.FindChild("Tip").GetComponent<UILabel>();
            labelContent.text = taskData.TextContent.ElementAt(countText);
            btnNext = objLabel.GetComponent<ButtonOnPress>();
            btnNext.AddListener(TurnTextPage);
            if (taskData.TextContent.Count > 1)
            {
                btnNext.GetComponent<BoxCollider>().enabled = true;
                labelTip.gameObject.SetActive(true);
            }
            else
            {
                labelTip.gameObject.SetActive(false);
                btnNext.GetComponent<BoxCollider>().enabled = false;
            }
           
        }


        public virtual void OnExecute()
        {
           
        }

        public void OnEnd()
        {
            if (objLabel != null)
            {
                objLabel.SetActive(false);
            }
            if (objShake != null)
            {
                objShake.SetActive(false);
            }
            if (objFlash != null)
            {
                objFlash.SetActive(false);
            }
        }

        public IGuideTaskEffect(IGuideManagerData parentData, int id)
        {
            pData = parentData;
            taskId = id;
            taskData = ConfigReader.GetIGuideInfo(taskId);
        }

        public int GetTaskId()
        {
            return taskId;
        }


        private Dictionary<GameObject, int> objmDepth = new Dictionary<GameObject, int>(); 
        private Dictionary<GameObject, bool> objWidget = new Dictionary<GameObject, bool>();
        private const int markDepthAdd = 1000;
        public void ShowMark(bool show, List<GameObject> objList) {
            if (show)
            { 
                GameObject objMark = objParent.transform.FindChild("Center/Mask").gameObject;
                UIPanel panelMark = objMark.GetComponentInParent<UIPanel>();               
                UIWidget markWid = objMark.GetComponent<UIWidget>();
                objMark.gameObject.SetActive(true);
                GameObject tipObj = objParent.transform.FindChild(taskData.TextPath).gameObject;
                if (objList == null) {
                    objList = new List<GameObject>();
                }
                
                objList.Add(tipObj);        
                if (objList != null && objList.Count != 0) {
                    for (int i = 0; i < objList.Count; i++)
                    {
                        GameObject obj = objList.ElementAt(i);
                       
                        UIWidget wid = obj.GetComponent<UIWidget>();
                        bool hasWidget = (wid != null) ? true : false;
                        objWidget.Add(obj, hasWidget);
                        int mDepth = (wid != null) ? wid.depth : 0;
                        objmDepth.Add(obj, mDepth);
                        if (wid == null)
                        {                            
                            obj.AddComponent<UIWidget>(); 
                        }
                        wid = obj.GetComponent<UIWidget>();
                        int panelDepthAdd = (panelMark != null) ? (panelMark.depth * 1000) : 0;
                        wid.depth = markWid.raycastDepth + markDepthAdd + panelDepthAdd;
                    }
                }

            }
            else if (!show && objWidget.Count != 0 && objmDepth.Count != 0)
            {
                if (objP != null) {
                    GameObject objMark = objParent.transform.FindChild("Center/Mask").gameObject;
                    objMark.SetActive(false);
                }
                for (int i = objmDepth.Count - 1; i >= 0; i--) {
                    GameObject obj = objmDepth.ElementAt(i).Key;
                    if (obj == null)
                        continue;
                    UIWidget objWid = obj.GetComponent<UIWidget>();
                    if (objWid == null)
                        continue;
                    bool has = objWidget.ElementAt(i).Value;
                    if (has)
                    {
                        objWid.depth = objmDepth.ElementAt(i).Value;
                    }
                    else { 
                        GameObject.DestroyImmediate(objWid);
                    }                    
                }
                objmDepth.Clear();
                objWidget.Clear();
            } 
        }

        void TurnTextPage(int index, bool isPress)
        {
            if (isPress)
                return;
            countText += 1;
            if (countText >= taskData.TextContent.Count)
            {
                return;
            }
            
            labelContent.text = taskData.TextContent.ElementAt(countText);
            if (countText == taskData.TextContent.Count - 1)
            {
                labelTip.gameObject.SetActive(false);
            }
        }
    }
}
