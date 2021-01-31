using System;
using System.Collections.Generic;
using UnityEngine;
using GameDefine;
using BlGame.GameEntity;
using JT.FWW.Tools;
using System.Linq;
using BlGame.GameData;
namespace BlGame.GuideDate
{
    public class SecondaryTaskEffect
    {
        private SecondaryTaskInfo owner;
        public static string parentPath = "Guide/SecondaryBoot";
        private static GameObject objParent;
        private GuideHelpData helpData;
        private bool isCheck = false;
        private List<GameObject> objList = new List<GameObject>();
        DateTime timeStart;

        public SecondaryTaskEffect(SecondaryTaskInfo taskInfo)
        {
            owner = taskInfo;
            helpData = ConfigReader.GetGuideHelpInfo(owner.GetTaskId());
        }

        public void OnEnter()
        {
            objParent = SecondaryGuideManager.Instance.CreateParentPrefab();

            if (objParent == null)
            {
                GameMethod.DebugError("secondary effect ui parent is null");
                return;
            }
            for (int i = 0; i < helpData.helpObjPathList.Count; i++)
            {
                string path = helpData.helpObjPathList.ElementAt(i);
                GameObject obj = objParent.transform.Find(path).gameObject;
                obj.SetActive(true);
                objList.Add(obj);
            }
            timeStart = DateTime.Now;
            isCheck = true;
        }

        public void OnUpdate()
        {
            if (!isCheck)
                return;            
            TimeSpan span = DateTime.Now - timeStart;
            if (span.TotalSeconds >= helpData.helpShowTime)
            {
                SecondaryGuideManager.Instance.SendTaskEndTag(helpData);
            }
        }

        public void OnEnd()
        {
            for (int i = objList.Count - 1; i >= 0; i--)
            {
                if (objList.ElementAt(i) != null && objList.ElementAt(i).gameObject != null) {
                    objList.ElementAt(i).gameObject.SetActive(false);
                }                
            }
            isCheck = false;
        }
    }
}
